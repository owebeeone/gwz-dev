# GWZ sequenced virtual-stream design — SAFETY-AXIS RE-REVIEW 1

Date: 2026-09-24  
Review object: `gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md` and `GwzTransportSequencedStreamDesign.md`, revised from core `bf60d472f41404316d35927624b82600d0713c63`.  
Axis: Safety; peer-blind, read-only.  
Settled tuple, verified at start and end: root `5774bd5293f68f1685df5352251c90a2318ca374`; core `b00a59229a3bc48fdb102cad61b9c9889a2ab1d6`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.

**Verdict: GO — design contract only. P0: 0; P1: 0; P2: 0; P3: 0.** No new architectural root cause was found in the changed range. This verdict does not qualify implementation or activation.

## Prior-finding closure table

| Prior finding | Re-review result |
| --- | --- |
| Consistency P2-1: `Close` incorrectly finalized the initiator sequence | **Closed.** Q4 and the design now make `Close` an ordered cleanup request, permit later `Window` and owed `Flushed`, and reserve the final sequence watermark for an actual sender-final message ([requirements line 13](gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md); [design lines 11–13, 27](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). The directed proof case now requires a reverse response exceeding initial credit, permuted post-`Close` controls, exact bytes, finite cleanup and one lease disposition ([design line 43](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). |
| Consistency P2-2: remote design §4.1 retained an ordered-carrier obligation | **Closed.** Both drafts expressly supersede that obligation and the placement §4 ordering sentence for profile 3, retain the other §4.1 protections, and preserve the ordered-carrier contract for profiles 1/2 ([requirements line 25](gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md); [design line 5](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). |

## Changed-range analysis

The revision changes terminal classification, the post-`Close` control grammar, the authority statement and their proof cases. A `Close` with sequence N must still wait for its own predecessors, but N no longer cuts off initiator controls. A later `Window` can replenish credit during bounded reverse drain, and an owed `Flushed` can acknowledge an earlier reverse barrier. An abortive `Cancel` remains available during that period. `Closed` still requires the real `Close`, required `EndWrite`, complete reverse drain and cleanup fence; its own direction cannot skip missing predecessors ([design lines 11–13, 27–29](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). This removes the prior normal-response deadlock without creating an apparent path to early graceful success.

## 0. Evidence base

I inspected the committed draft pair and its change from the prior core tuple, the two historical reviews and remediation plan, the process rules, the sequencing direction, the controlling remote requirements and design, the placement design, the authoritative GWZ documents, the Taut envelope and current stream/mux shape. The controlling lifecycle requires ordered `Data`/`EndWrite`/`Close`, bounded reverse drain, truthful `Closed`, cancellation cleanup and one lease owner ([remote requirements §5.4](gwz-core/dev-docs/GwzRemoteTransportRequirements.md); [remote design §6](gwz-core/dev-docs/GwzRemoteTransportDesign.md)). The current stream shape can emit `Flushed` and `Window` ahead of pending `Closed` and after `Close` ([outgoing.rs lines 18–55, 90–113](gwz-transport/src/stream/outgoing.rs)); it is feasibility context, not a profile-3 implementation. Unrelated working-tree changes were excluded. No files were changed, and no builds or tests were run.

## 2. Invariant analysis

- **Reverse drain and graceful completion.** With a response larger than initial credit, post-`Close` `Window` can advance the endpoint’s reverse output. Per-direction sequencing makes permuted `Window`/`Flushed` arrivals wait for their own predecessors, while reverse `Data` and `EndWrite` must precede a successful `Closed` in the endpoint direction. Byte offsets provide an independent continuity check ([design lines 13, 27, 43](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
- **Abortive cutoff and late controls.** A validated `Cancel` or failure at sequence N may supersede missing earlier frames in its sending direction; later covered arrivals receive `Superseded` without changing failure, facts or lease outcome. Opposite-direction in-flight work remains charged until delivered or atomically aborted. A successful `Closed` has no skip authority ([design lines 29–31](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
- **Pre-Open and generation authority.** A higher-sequence pre-Open frame consumes bounded provisional capacity but cannot create a route or endpoint effect. A correlated `Cancel` may seal the ID; otherwise the fixed gap deadline fails the binding. Host items pin both Port generations, so a stale or wrong-session receipt cannot acknowledge the intended peer’s application ([design lines 15, 19, 23](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
- **Backpressure and ownership.** A missing-predecessor reserve, fair dispatch and a non-resetting 30-second gap deadline provide a finite failure path under reorder saturation. `Buffered` ownership is distinct from `Applied`; cancelled application waiters do not discard tickets, and carrier loss resolves them. Request and lease ownership persist through cleanup rather than ending at user-visible completion ([design lines 23, 31, 35–37](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).

## 3. Risks and next action

The aggregate memory and ticket representation proof remains an explicit implementation gate; the candidate 4096-ID cap may be reduced before profile freeze ([design line 37](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). Implementation proof should exercise the required reverse-drain permutation and a `Cancel` racing with `Closed`, checking ticket resolution, terminal facts and exactly one lease disposition. Retained-reader compatibility, deterministic interleavings and seed-replayable tests remain required before activation ([design lines 41–45](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
