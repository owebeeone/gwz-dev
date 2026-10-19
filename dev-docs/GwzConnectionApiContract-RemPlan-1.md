# GWZ Connection API contract — remediation round 1

Date: 2026-10-19. Object: revision 0 at root
`d0848ed0fc0fc090cd7dde1a112e085b7e58505f`.

## Scope, authority and finding accounting

The operator authorized creating/reviewing an API contract addressing the observed
churn. This is one merged documentation patch; no code, dependency, member or
native qualification changes. Original reports are filed verbatim in
[Consistency](GwzConnectionApiContract-ReviewConsistency.md) and
[Safety](GwzConnectionApiContract-ReviewSafety.md).

The two axes each used P2-1 locally; qualified IDs below keep their roots distinct.
Consistency classified its P2-1 as one new architectural root. Safety classified
its missing transition as non-architectural. Initial root count: one. This is the
first remediation round of at most two; reviewers classify any new root.

| Finding | Disposition in revision 1 | Original counterexample / closure specification |
| --- | --- | --- |
| Safety P2-1 | Accept. Define Reserved claim revocation under the registration authority lock; register retains its infallible signature but a revoked permit yields a terminal NeverRegistered operation. No active publication; exact-once accounting. Add domain/host closure and failure cases. | Acquire P, create unpolled close/drop owner/fail driver, consume P then open: terminal reason, Effect::None, zero adapter effects. Race both orders: Registered and included in cancellation, or revoked/NeverRegistered. Repeat domain close/drop. |
| Consistency P2-1 | Accept. Select a neutral StreamOrder start/ticket and AppliedEvent seam; factor the existing ordering algorithm, keep the Git taut profile as wrapper; add version-free OrderedStreamMachine consuming only admitted events. Name exact sequencing clauses amended. | Walk arbitrary non-Git SshExec and HTTP through RequestStart/ReplyStart, first body, reorder/close and settlement; no manufactured GitService, no v3-to-v2 envelope cast. Stalled other lane cannot block it. Future fake tests use actual selected pure-kernel seams and retained profile-3 vectors. |
| Consistency P3-1 | Accept in the same patch. Add CleanupDomainOwner/handle construction and close/drop recipe over mandatory retained supervisor port, with final pending status and two-host walkthrough. | A pending, B closed, domain closes/drops, all tickets/handles drop: retainer still owns the generation; late disposal releases only once. |
| Consistency P3-2 | Accept in the same patch. InputBudget is explicit AdmissionError; BuildError::InvalidConfig(InputBudget) handles invalid limit configuration. | Over-budget AuthContext fails admit before registration, endpoint construction or physical effect. |

No finding is self-closed by this table. The originating reviewers must verify
the corrected design traces and return closure tables on the revised committed
object. No test execution is claimed: these are design-level closures; execution
is required in the later explicitly authorized portable proof.

## Reviewer continuity and changed surface

The new neutral sequencing seam is a material interface clarification. The skill
normally replaces reviewers at that point. The operator's earlier explicit
instruction in this conversation was **“use the old reviewers”**; continue the
same two independent axis reviewers with their context, in accordance with that
instruction rather than adding/replacing reviewers. Both receive this merged
plan and the whole changed range; current-round peer reports remain forbidden.
No third reviewer or implementation agent is dispatched.

API Surface and authority/necessity checks remain in Consistency. Safety checks
the neutral seam's authority, terminal behavior and retention changes as well as
its original revocation race. Each reviewer must label any new architectural
root; do not pre-classify a changed-range issue as a harmless correction.
