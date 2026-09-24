# Python transport session v4 foundation — first remediation verdict

Date: 2026-09-24. Status: **NO-GO; bounded remediation round 2 of at most 2 under the review loop, not a lane stop.** The reviewed committed tuple is root `11358352a237a673ecf0fd82347b45ca4aca8950`, gwz-core `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`, and gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`. It applied the [first remediation plan](GwzPyTransportSessionV4FoundationDesign-RemPlan.md) to the tuple the [first design verdict](GwzPyTransportSessionV4FoundationDesign-Verdict.md) rejected. No implementation, build, activation, push or tag is authorized by this verdict.

The same three reviewers re-verdicted their own findings with their context intact, as focused re-reviews. The [Consistency](GwzPyTransportSessionV4FoundationDesign-ReviewConsistency-1.md) re-review returned **NO-GO**, the [Safety](GwzPyTransportSessionV4FoundationDesign-ReviewSafety-1.md) re-review returned **GO**, and the docs-only [Surface](GwzPyTransportSessionV4FoundationDesign-ReviewSurface-1.md) re-review returned **NO-GO**. The reports are filed verbatim. All three reviewers verified the exact tuple at the start and end. They ran on Opus 5.5, a different model from the one that drafted the object.

| Axis | Verdict | Round-1 findings | New blocking | New nonblocking |
| --- | --- | --- | --- | --- |
| Consistency | NO-GO | all 16 closed (P2-1 to P2-5, P3-1 to P3-11) | R2-C-P2-1 | R2-C-P3-1 to R2-C-P3-4 |
| Safety | GO | all 9 closed (P2-1 to P2-4, P3-1 to P3-5) | none | P3-6 to P3-9 |
| Surface (docs-only) | NO-GO | all 6 closed (P3-1 to P3-6) | P2-1 | P3-7 to P3-9 |

**Classification.** All 31 round-1 findings are closed; each reviewer re-ran its original counterexamples on the corrected text. Every reviewer classified every new finding as a bounded contract or text correction; none is a new architectural root cause. Safety records that the ownership ladder, transfers, intents, owner clocks, generation fault rule and construction ownership held under a second attack. Consistency and Surface each pre-committed to GO on a revision that resolves its one P2 as specified. Safety's GO honours its round-1 pre-commitment.

## Merged blocking root causes

| ID | Root cause | Findings | Convergence |
| --- | --- | --- | --- |
| B8 | The first remediation filed local submitted operations (`merge`, `clone_local_workspace`) as natively minted records outside v2's slot bound, cancellation and close join, and called that a clarification. The caller guide still promises v2's behavior: close can return while a merge is writing the workspace. The same native mint gives a submission cancelled before native entry no ID for its caller, lets it start later, and leaves its record in the ledger. | Consistency R2-C-P2-1; Safety P3-7 and §3 residuals (local work after close, local unwinds faulting network work, unspecified local ledger allowance) | Blind, P2, P3 and residuals on one mechanism |
| B9 | The faulted refusal is named `TransportGenerationBusy`, which reads as temporary contention, but in V4 it means the Client's transport has failed for good. v2 already uses that name for a temporary rollover condition. | Surface P2-1 | Single axis |

B8's two axes attack the same mechanism, §7.6 rule 3, from opposite sides: Consistency from the v2 contract it narrows, Safety from the pre-entry interval it leaves without an owner in the caller's sense. B9 is one of three independent findings about the faulted signal. The other two are nonblocking: the capability preflight re-types the refusal (Consistency R2-C-P3-1), and a stalled supervisor never faults the Client (Safety P3-9). Together they show that the faulted state is not yet reliably named, reached or delivered.

Nonblocking convergences:

- The guide's `TransportSessionFull` row omits the slot-full case (all three axes: Consistency R2-C-P3-2(a); Surface P3-9 and its residual; Safety residual).
- An owner clock is said to win only against a "dead" supervisor, while it wins only against a stalled one (Safety P3-9, Consistency R2-C-P3-3).
- `request_id_consumed=false` is not established for refusals made before core's duplicate check or for the synchronous live-claim refusal (Safety P3-8, Consistency R2-C-P3-2(b)).
- The shared `accepted()` future lets one caller's cancellation or its first caller's outcome reach other callers (Safety P3-6, Consistency residual).
- The deliberate generation close in A11 and A14 has no declared synchronous core API (Safety residual, Consistency residual).

## Stop-rule accounting and next action

This was V4's first remediation round. Neither V4 round produced a new architectural root cause, so the [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop is not triggered. The next round is remediation round 2 of at most 2. A later round would be permitted only for non-architectural corrections, and any architectural root cause found in it would stop the lane.

The next action is the single consolidated patch in the [second remediation plan](GwzPyTransportSessionV4FoundationDesign-RemPlan-1.md), covering the V4 design, the paired core paragraphs and the caller guide. Commit it as a new exact tuple, then obtain focused re-verdicts from the same three reviewers. The V4 design remains a draft, not an accepted foundation.
