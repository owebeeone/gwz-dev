# GWZ sequenced virtual-stream design — remediation plan 1

Date: 2026-09-24. Object: the draft requirements and design at gwz-core `bf60d472f41404316d35927624b82600d0713c63`, workspace `38d5235926c61c66f3fda4cc37f4da01b3c140d0`.

The peer-blind [Consistency review](GwzTransportSequencedStream-ReviewConsistency.md) returned NO-GO with two P2 findings; the [Safety review](GwzTransportSequencedStream-ReviewSafety.md) returned GO on that exact tuple. This plan makes one design-contract correction. It does not authorize implementation or activation.

| Finding | Disposition | Correction | Closure evidence |
| --- | --- | --- | --- |
| Consistency P2-1: `Close` incorrectly finalizes the initiator's message sequence | Accept | Define `Close` as an ordered cleanup request, not the sender-final watermark; keep later `Window` and owed `Flushed` legal during bounded reverse drain. Reserve final watermark for actual sender-final messages. | Reviewer retraces a reverse response larger than initial credit, with initiator `Window` after `Close`, and the specified permuted-delivery test. |
| Consistency P2-2: remote design §4.1 remains an unqualified ordered-carrier authority | Accept | Precisely supersede §2 and §4.1 of the remote design and §4 “Message handoff and correlation” of the placement design for profile 3 only; preserve reliability, validation and limits. | Reviewer checks a profile 1/2/3 authority matrix and an out-of-order higher-sequence in-process case. |

The single patch will change both draft documents and their proof cases. As this corrects the protocol's terminal and precedence contract, dispatch a fresh numbered peer-blind dual review on the revised settled tuple under the review-loop rule for material interface changes. File their reports verbatim and merge a new verdict before declaring the design accepted.
