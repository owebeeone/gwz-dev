# SSPI design — merged remediation 1

2026-10-03. Revision 1 remains NO-GO until reviewer closure. No implementation.
Reviewed tuple: root `377c5e29882e353e42dbe75031b175374b213f7f`, core
`d78a664e3c5a325c6f12be409eb7645c1c1b51d0`, evidence
`1beb1d204c824701ddbd033c7f89df9a3561f5e5`.

One document patch, all axes' dispositions:

| Finding | Disposition | Closure walkthrough / future regression |
|---|---|---|
| Consistency P2-1 / Safety P2-3 | Accept. Begin owns Digest initial_challenge, method and URI; fields required only for Digest, explicit identity required; enforce token/frame limits and zeroization. | start→step(None) delivers exact C at first Digest Initialize; missing/oversized/wrong-package inputs refuse before native call; cancel before Begin wipes retained input. |
| Surface P2-1 | Clarify retained scope and accept missing output description. Negotiate permits Windows Kerberos/NTLM; no Kerberos-only policy. Expose native mechanism observation, final identity on Complete or fail, unresolved intermediate only under caller acceptance of either. Reviewer's clarification agrees this avoids a new policy knob. | Trace Kerberos, NTLM and unresolved Continue; Kerberos-only caller must reject this unsupported request before start and transmit no token. No independent parsing. |
| Surface P3-1 | Accept. Deadline is immutable after start; choose shorter before start, use Cancellation for earlier parent expiry during work. | D2 earlier triggers supplied cancellation; D3 cannot extend original deadline. |
| Consistency P3-1 | Accept. Finish legal after Hello, including before Begin and after Complete. | start→finish initializes no credential/context handles, returns only confirmed cleanup. |
| Consistency P3-2 | Accept. Explicit Code/State stops after codec/API and native credential handling; retain cohesive chunks rather than per-function gates. | Cold plan walkthrough locates each mandatory secret-boundary stop. |
| Safety P3-1 | Accept. Remove cooperative worker-budget promise; parent alone enforces immutable deadline. | Delayed Begin cannot gain/reset deadline; cancellation at earlier D2 still revokes publication. |
| InitialScope Safety P2-1/P2-2 | Superseded/withdrawn by the same reviewer's full Safety report. No new policy knob/queue redesign is inferred from the limited-scope pass. | Final full Safety disposition is authority; implementation must verify bounded waiter bookkeeping and caller-future drop ownership. |

Blind convergence: Consistency and full Safety independently identified the
Digest bootstrap omission. Initial limited Safety report is retained verbatim
but not the full-axis gate. Surface's requested Kerberos-only counterexample
was outside the inherited policy; clarification preserves that policy and makes
its limitation observable, without adding selection settings.

This is remediation round 1. Reviewers classify architecture: Consistency calls
the Digest omission an interface-contract root cause; Safety calls it a bounded
schema correction, not a containment-architecture defect. Surface identifies
missing mechanism identity in the interface. No third architectural discovery
round has occurred. Continue the original reviewers per the operator's existing
instruction to use the old reviewers; each must inspect the complete changed
range, not assume prior proofs cover the amended message/result contract. Full
Windows remains NO-GO independently of this mechanism review.
