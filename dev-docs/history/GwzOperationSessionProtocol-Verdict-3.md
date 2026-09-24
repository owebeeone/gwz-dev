# Operation-session protocol — focused re-verdict and lane stop

Date: 2026-09-23. **Verdict: NO-GO for the proposed version-1 operation-session contract.** This is a design verdict; it does not change the implementation or release gates.

The reviewed tuple was gwz-dev `c5b497bc0934b8970d5abba7022b5da020ff7f72`, gwz-core `d3951dcfc04d2f09c9c7a025dec41ee3163c4058`, gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`, and gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All three independent reviewers verified it at the start and end of their read-only reviews.

| Axis | Report | Verdict | Blocking findings |
| --- | --- | --- | --- |
| Consistency | [ReviewConsistency-2](GwzOperationSessionProtocol-ReviewConsistency-2.md) | NO-GO | P2-1: the five-second cleanup report does not prove physical cleanup finished. |
| Safety | [ReviewSafety-2](GwzOperationSessionProtocol-ReviewSafety-2.md) | NO-GO | P2-1: the same physical-cleanup ownership gap; P2-2: the 35-second orphan promise omits a network-handler termination bound. |
| Surface | [ReviewSurface-2](GwzOperationSessionProtocol-ReviewSurface-2.md) | NO-GO | P2-1: the guide's recovery from one operation's incomplete cancellation closes the shared client and cancels peers despite its isolation promise. |

Consistency and Safety independently reproduced the same new architectural root cause. `CleanupReport` can return at five seconds with `pending_local_work > 0`; the proposed 35-second orphan retirement simultaneously requires completed physical cleanup and release of the session charge. Safety additionally found that a network handler has no numeric termination bound matching that deadline. Surface found the related caller-level consequence: operation-local cancellation can require shared-client close, which cancels another live operation. Its P3-1 also asks for effective `None` defaults in the public fetch guide, but is not blocking by itself.

The prior round's blocking counterexamples were closed in the revised design, subject to later implementation proof. These new findings are not the previous corrections left unfinished. They identify an unowned interval between a timed cleanup report and actual physical termination, plus a user-facing isolation promise that cannot be met by the stated recovery action.

The two-round remediation cap in [RemPlan-2](GwzOperationSessionProtocol-RemPlan-2.md) now applies. **Stop this lane; do not draft another bounded correction to this object or start operation-session implementation from it.** The next authorized design object must choose and review the ownership boundary for residual physical work: either enforce a proved end-to-end termination bound for every accepted handler and endpoint, or transfer pending work to a separately bounded and charged cleanup owner with truthful reporting and admission recovery. It must also settle whether `cancel()` offers operation-local cleanup without cancelling peers, and align the caller guide with that choice. That is a redesign and new review gate, not an automatic third remediation round.

The existing transport, retry, and Python release gates remain unchanged by this verdict. Work on those independent lanes may continue under their own accepted contracts.
