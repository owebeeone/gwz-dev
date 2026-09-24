# Python concurrent transport design — second re-verdict merge

Date: 2026-09-24. Reviewed tuple: root `e4b035c4d273eed5e76a6540a6841a54863269f1`, gwz-core `1bd5e5c5ce2ac647ea1a144b82badad4769efe0c`, gwz-py `5541b851266da7267f49531b98c061af1fd670e1`. This merges the independent [Consistency](GwzPyTransportConcurrencyDesign-ReviewConsistency-2.md), [Safety](GwzPyTransportConcurrencyDesign-ReviewSafety-2.md), and docs-only [Surface](GwzPyTransportConcurrencyDesign-ReviewSurface-2.md) reports. Earlier findings and their claimed dispositions are in [RemPlan-2](GwzPyTransportConcurrencyDesign-RemPlan-2.md).

**Verdict: NO-GO for this design, implementation, Python Phase 6 completion and Phase 7 activation.** Consistency reports two P2 defects; Safety reports two P2 defects; Surface reports GO with one P3 documentation defect. The reviewers re-traced and closed the earlier counterexamples in the *document contract*, but did not accept runtime code or the new design.

The remaining architectural questions are coupled:

1. Closing a Client can cancel an admitted push after a possible remote update and then hide its operation result. An event iterator already active at Closing cannot receive the terminal event the guide promises. A retained close report containing only physical cleanup does not convey the per-operation effect.
2. Cancellation between acceptance and delivery of a public handle can retain up to 64 inaccessible results, blocking new work until expiry. The same path hides possible remote-effect evidence from its caller.
3. The unchanged Taut `GwzErrorCode` cannot encode the promised `Cancelled` and `TransportRecordLimit` `OperationResult` errors. A Python-only effect attribute does not by itself supply a truthful, stable encoded failure class.

These are **new architectural root causes** identified after the two remediation rounds on this object. The [review-loop rule](/Users/owebeeone/.claude/skills/review-loop/SKILL.md) says: “at most two remediation rounds per object” and “If a reviewer ... identifies a third new architectural root cause on the same object, stop the lane. Do not draft another patch. Report to the operator that the object needs redesign-or-accept — that is an operator decision.” The lane therefore stops at this verdict. No third correction, code implementation, or activation is authorized by these reports.

The decision for the operator is **redesign the operation outcome and close boundary as a new review object**, or explicitly accept the identified risks and change the release gate. Redesign is the recommended path: make per-operation terminal evidence reachable after close and during accepted-start cancellation, define whether active readers drain, and choose a typed terminal representation before implementation. The Surface P3 method-name mapping can be addressed in that new object's caller guide. This document records the gate; it does not choose on the operator's behalf.
