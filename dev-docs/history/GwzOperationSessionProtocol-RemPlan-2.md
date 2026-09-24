# Operation-session protocol — round-2 verdict merge and final bounded correction

Date: 2026-09-23. Status: **NO-GO** on root
`ee11f44efa5a0796a71d874ffaa4bc570a611a4e`, core
`8756fd6b32443b0ac63287ee5b4a3335e8cf0894`, Python
`45bcd7b3ea102ca927935cee1b41b43934d68140`, and transport
`46e65a9a888fbd4a5bbeace946996581dcf23333`.

[Consistency](GwzOperationSessionProtocol-ReviewConsistency-1.md) reports
three P2, [Safety](GwzOperationSessionProtocol-ReviewSafety-1.md) one P0 and
three P2, and the strictly docs-only
[Surface](GwzOperationSessionProtocol-ReviewSurface-1.md) three P2 and two
P3. Consistency P2-2 and Safety P2-2 **independently converge** on the
unbounded caller ID defeating the 4 KiB terminal fallback. Safety P0-1 is
the earlier global-store root still live through the proposed legacy path;
the review does not claim a released package has this defect. Reviewers
classified no new architectural root cause. This is the second and final
remediation allowed for this object; an architectural root found in the
next re-verdict stops the lane for redesign-or-accept.

| Finding | One correction | Original counterexample to re-check |
| --- | --- | --- |
| Safety P0-1 | Bind **legacy as well as v1** operation records and every event/result/merge read to the originating Client/route. Remove or fail closed unowned module-level lookups; do not preserve a public global-ID read. | Two v0 clients choose the same caller ID, complete in opposite orders, then attempt every foreign lookup. |
| Safety P2-1 | Make bounded cancellation/safe termination of any admitted local handler an activation precondition. An unqualified local handler refuses v1 submit before `Accepted`; no orphan may pin a session forever. | Block local mutation, lose owner/route, fill 32 slots, and prove qualified work retires by its bound, with truthful close and no post-success mutation. |
| Safety P2-2 + Consistency P2-2 | Bound caller ID to 256 UTF-8 bytes at admission and prove all mandatory fallback fields, including encoding overhead, fit the 4 KiB charge. | 256-byte multibyte ID succeeds; 257 bytes refuses pre-`Accepted`; both force oversized response and yield one bounded terminal. |
| Safety P2-3 | Use a cheap `operation_session.open` version/placement negotiation that never constructs an endpoint or reads credential/proxy environment. Reject unsupported placement before any existing capability call. | Unbound CLI with poisoned TLS/proxy returns `PlacementUnavailable`, with zero endpoint/environment/helper access. |
| Consistency P2-1 | Extend the core capacity amendment to replace retry §3(7)'s no-lease installation trigger, §6's first installation bullet and S1.4's references; quiescence counts admitted scopes, not only leases. | Hold A before first Open, admit lower B without resize, refuse higher B, then resize after A and cleanup retire. |
| Consistency P2-3 | Put the decoded action-specific response in `GwzOperationError.response` for all failed handle/unary/stream paths; `ResultLimitExceeded` has `response=None` and an explicit effect-bearing fallback. | Failed fetch and conflicted merge preserve their generated responses and recovery fields in `exc.response`. |
| Surface P2-1 | Bind event iterator's default cursor to the acceptance position; first read reports a typed gap if records expired, with oldest retained cursor. | Delay first iteration past event eviction; no silent suffix. |
| Surface P2-2 | Publish the full `start_fetch` metadata signature and defaults, or explicitly narrow parity. Prefer the existing fetch metadata set. | Translate every existing supported fetch metadata argument into a documented start call. |
| Surface P2-3 | Name `OperationCancelled` error code and terminal kind/status variants; show a branch over cancellation, success and failure. | Cancel A and classify its terminal without source inspection. |
| Surface P3-1 | State CLI placement is internal-only in v1 Python until a binding API is separately designed; omit misleading public placement argument. | Guide contains no unreachable advertised Python placement path. |
| Surface P3-2 | Define cleanup field types and what the caller does with incomplete facts. | Guide handles nonzero pending or unconfirmed peer cleanup without claiming success. |

Make one corrected document patch to the root protocol design and caller guide
and the paired core capacity amendment. No implementation or release work is
authorized by a design GO. Use the same round-2 reviewers for focused
re-verdicts if the changes remain bounded to these dispositions; their
prior counterexamples are the cheapest reliable closure. Keep Surface
docs-only. If any reviewer identifies a third new architectural root cause,
stop rather than drafting another correction.
