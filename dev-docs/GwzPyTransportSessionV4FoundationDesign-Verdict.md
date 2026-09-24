# Python transport session v4 foundation — first design verdict

Date: 2026-09-24. Status: **NO-GO; bounded remediation under the review loop, not a lane stop.** The reviewed committed tuple is root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`, gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`, and gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`. No implementation, build, activation, push or tag is authorized by this verdict.

The peer-blind [Consistency](GwzPyTransportSessionV4FoundationDesign-ReviewConsistency.md) and [Safety](GwzPyTransportSessionV4FoundationDesign-ReviewSafety.md) reviews both returned **NO-GO**. The docs-only [Surface](GwzPyTransportSessionV4FoundationDesign-ReviewSurface.md) review returned **GO** under the P0–P2 gate, with six P3 findings. The reports are filed verbatim. All three reviewers verified the exact tuple at the start and end. They ran on Opus 5.5, a different model from the one that drafted the object (Fable 5.1), as [GwzProcessOptimization §4.3](GwzProcessOptimization.md) asks.

| Axis | Verdict | Blocking | Nonblocking |
| --- | --- | --- | --- |
| Consistency | NO-GO | P2-1 to P2-5 | P3-1 to P3-11 |
| Safety | NO-GO | P2-1 to P2-4 | P3-1 to P3-5 |
| Surface (docs-only) | GO | none | P3-1 to P3-6 |

**Classification.** Every reviewer classified every finding as a bounded contract or text correction; none is a new architectural root cause. Both blocking reviewers pre-committed to GO on a revision that resolves their P2 findings as specified. The ownership protocol itself held under both attacks. Consistency re-traced the V3 stop findings and recorded Safety P2-1 (unowned pre-entry interval), Safety P2-2 (watchdog versus `Ready`) and both Consistency contract findings as closed by V4. Safety's invariant analysis records the Python handoff, the worker gate, the owner clocks' independence from the supervisor and the synchronous phase 2 as holding.

## Merged blocking root causes

| ID | Root cause | Findings | Convergence |
| --- | --- | --- | --- |
| B1 | The 6-second owner clock on finish is shorter than core's own finish bound, which runs two 5-second cleanup windows in sequence. A healthy but slow cleanup therefore faults the Client. | Consistency P2-1, Safety P2-2 | Blind, both P2 |
| B2 | Entries that carry no issued operation ID are unspecified. The capability preflight installs a runtime that never bootstrapped, outside the owned construction step, so every later admission refuses. Local submitted operations cannot be read by ID through a native-session bridge. | Consistency P2-2, Safety P2-1 | Blind, both P2 |
| B3 | The deferred 8 MiB ledger upgrade is attached at the infallible, post-registration `Claim::accept`, so a v2 pre-registration session-full refusal would consume the request ID. | Consistency P2-3 | Single axis |
| B4 | The paired core requirement forbids the supervisor-driven mux and cleanup expiry closes that core keeps and V4 relies on. | Consistency P2-4, Safety P3-5 | Blind, P2 and P3 |
| B5 | The reserved bootstrap registration ID is a valid caller request ID. | Consistency P2-5, Safety P3-1 | Blind, P2 and P3 |
| B6 | A panic inside `bootstrap()` leaves `constructing` set, so waiting claims and `close()` hang. | Safety P2-3; Consistency §3 residual | Blind, P2 and residual |
| B7 | A closed installed generation leaves the session `Open` and unfaulted, so every later refusal reads as retryable. | Safety P2-4, Consistency P3-9 | Blind, P2 and P3 |

Five of the seven blocking roots were found independently by both axes, including the two with the widest impact (B1 and B2). Nonblocking convergences: the retry example's cancellation leak (all three axes), the progress-record API sketch (Consistency P3-7, Safety P3-2), the cross-loop `asyncio.Lock` (Consistency P3-10, Safety P3-3) and the duplicate-ID value of `request_id_consumed` (Consistency P3-8, Surface P3-2).

The prior-object Surface P3-2 (retry example lifecycle) remains open as the current Surface P3-1. It does not block, but it has now appeared in three consecutive Surface rounds, so the remediation plan makes it a named closure item.

## Stop-rule accounting and next action

V4 is a new object after the V3 stop. This was its first review round. No reviewer classified any finding as a new architectural root cause, so the [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop is not triggered. This is remediation round 1 of at most 2.

The next action is the single consolidated patch in the [remediation plan](GwzPyTransportSessionV4FoundationDesign-RemPlan.md). It covers the V4 design, the paired core paragraphs, the caller guide, and a precedence pointer in `gwz-py/dev-docs/GwzPyTransportDesign.md`. Commit it as a new exact tuple, then obtain focused re-verdicts from the same three reviewers. The V4 design remains a draft, not an accepted foundation.
