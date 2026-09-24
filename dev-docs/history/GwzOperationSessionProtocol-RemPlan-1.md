# Operation-session protocol — round-1 verdict merge and remediation

Date: 2026-09-23. Status: **NO-GO** on root
`852fd94a082f82b8d4e9d777edf7d20acdf12576`, core
`26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`, Python
`45bcd7b3ea102ca927935cee1b41b43934d68140`, and transport
`46e65a9a888fbd4a5bbeace946996581dcf23333`.

Independent [Consistency](GwzOperationSessionProtocol-ReviewConsistency.md) and
[Safety](GwzOperationSessionProtocol-ReviewSafety.md) reviews each found four
P2 defects. The [docs-only Surface](GwzOperationSessionProtocol-ReviewSurface.md)
review found four P3 guide defects and reported GO. There was no exact blind
convergence: the axes attacked complementary contract gaps. None of these
reports is implementation acceptance. The Python Phase 6/7 release NO-GO stays
in force.

| Finding | One correction | Closure evidence for the revised design |
| --- | --- | --- |
| Consistency P2-1 | Make one session-owned terminal record contain the existing `OperationResult` **and** the full action-specific typed Taut response or a typed output-limit failure, with one release/expiry boundary. | Fetch and merge handle/unary/stream paths expose the same typed fields; all terminal components expire together. |
| Consistency P2-2 | Give core member workers an aggregate session and receiver permit budget independent of caller `jobs`; bound operation workers and accepted queue separately. | Many operations, large explicit `jobs`, cancellation and close keep worker/queue counts within declared ceilings. |
| Consistency P2-3 | Put exact retry §6/S1.4 and Phase-2 supersessions in a core draft capacity amendment in the reviewed tuple; keep old plans authoritative until amendment GO. | Text cross-check plus held-lease lower-limit admission, higher-limit refusal before Open and differing-limit Phase-2 test. |
| Consistency P2-4 | Publish finite numeric defaults and configurable ranges for session, receiver, workers, events and terminal retention. | Boundary and +1 admission tests; byte-budget and expiry cases. |
| Safety P2-1 | Give **all** accepted operations an execution/completion scope; transport finish is an optional child of it. Successful close joins local work and reports no post-close mutation. | Block local clone after acceptance; race cancel/close; release block; one terminal outcome and no later mutation. |
| Safety P2-2 | Separate reusable caller `request_id` from unique internal transport request ID, generated per operation and generation. | Reuse caller ID below 256 threshold; late A transport message cannot touch B. |
| Safety P2-3 | Bound terminal-result bytes including action-specific responses, and define a small retrievable typed outcome when a response exceeds its budget. | Large workspace and oversize response produce bounded retained bytes and one attributable terminal outcome per accepted ID. |
| Safety P2-4 | Bound sessions per receiver and route; define idle expiry, route-loss close trigger, orphan close behavior and finite close-record retention. | Limit+1 open refuses; abandoned idle/active and route-loss cases retire without cross-route reattachment. |
| Consistency P3-1 | Specify awaited `OperationHandle.release()` and idempotency, then align guide. | Guide-only result/release/repeat-release/expired-read walkthrough. |
| Surface P3-1 | Add relevant signatures, defaults and every admission failure's recovery. | Guide reader can choose default/explicit options and diagnose each full budget. |
| Surface P3-2 | Specify event iteration, replay, sequence, EOF and cursor-gap recovery with example. | Guide-only full event read and lagging-reader recovery. |
| Surface P3-3 | Define returned/raised forms and fields for success, operation failure and cancellation cleanup. | Guide-only classification of all three outcomes. |
| Surface P3-4 | Show both handles released after use, repeated release and close behavior. | Repeated example does not retain completed records indefinitely. |

Make **one** corrected design package: protocol design, caller guide and the
bounded core capacity amendment. Do not patch implementation against the
rejected draft. Because ownership and the public API change, dispatch a new
numbered round with fresh independent Consistency and Safety reviewers on the
new tuple, plus a fresh docs-only Surface reviewer. Keep the two-round
remediation cap and record any new architectural root cause explicitly.
