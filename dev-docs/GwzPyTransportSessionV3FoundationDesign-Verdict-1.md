# Python transport session v3 foundation — first remediation verdict

Date: 2026-09-24. Status: **NO-GO; V3 foundation lane stopped for redesign and re-freeze.** The reviewed committed tuple is root `21fac9f4236cd12df7719a92f039ae5cd787f067`, gwz-core `e7b4c499a2e5e2bfe8be0db2fbe067d6599a506f`, and gwz-py `f6ae40afaa67ddaa561002dcee54d3c522739dd1`. No implementation, build, activation, push or tag is authorized by this verdict.

The peer-blind [Consistency](GwzPyTransportSessionV3FoundationDesign-ReviewConsistency-1.md) and [Safety](GwzPyTransportSessionV3FoundationDesign-ReviewSafety-1.md) re-reviews both returned **NO-GO**, two P2 findings each. The docs-only [Surface](GwzPyTransportSessionV3FoundationDesign-ReviewSurface-1.md) re-review returned **GO** under the P0–P2 gate; it left one P3 example-lifecycle defect. These reports are filed as returned, without substantive editing. All three reviewers verified the exact tuple at the start and end.

The first-round blockers are closed at the design level, but new gaps remain:

| Finding | Remaining defect | Classification |
| --- | --- | --- |
| Consistency R2-C-P2-1 | V3's exhaustive V2 supersession list omits the finish-unwind/lost-owner exception to V2's finish-before-terminal rule. | Contract correction, not a new architectural root. |
| Consistency R2-C-P2-2 | The fault matrix promises same-generation retry after a post-mutation install timeout even though its guard closes that generation. | Recovery-contract correction, not a new architectural root. |
| Safety P2-1 | Issuance occurs before native entry; a Python executor or encoding failure can return a refusal without creating the `Claim` that must settle the issued record. | **New architectural root cause:** missing issue-to-claim settlement owner. |
| Safety P2-2 | An independent watchdog can fire after phase 2 returns `Ready` and the worker starts, closing its accepted generation. | **New architectural root cause:** no exclusive `Ready`/expiry outcome handoff. |
| Surface P3-2 | The retry example releases the refused handle but ends after admitting the replacement, without showing its result and release. | Nonblocking documentation debt. |

Safety explicitly classified P2-1 and P2-2 as new architectural root causes. Together with the first round's architectural phase-2 and provenance defects, this reaches the program's third-new-root-cause stop rule in [GwzProcessOptimization §4.1](GwzProcessOptimization.md). A second V3 remediation patch would repeat the architecture loop. The next design object must put ownership around the Python issue-to-native-entry gap and give watchdog expiry and successful readiness one atomic winner; it must also reconcile the two contract findings and the caller example before a new exact-tuple review. The present V3 design remains a draft, not an accepted foundation.
