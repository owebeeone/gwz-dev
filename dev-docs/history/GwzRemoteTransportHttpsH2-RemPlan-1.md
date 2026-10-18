# HTTPS H2 — consolidated correction 1

Status: in progress, no finding self-closed. Original reviewed tuple is recorded
in the verbatim Code and State reports. Both axes reported NO-GO.

| Finding | Disposition | Closure evidence |
|---|---|---|
| Code P2-1; State P2-2 | Correct the shared causal retry root. Serialize each canonical automatic two-Open transition; prevent explicit/cached Gh from consuming orphaned state; retain the complete first failure, stream identity and policy alongside the final result. No timeout allowance may refill or be borrowed from another attempt. | Overlapping same-request, equivalent-route callers with asymmetric first-attempt work; explicit/cached isolation; cancellation retires state; typed first-receipt assertions. |
| Code P2-2 | Retain HTTPS pre-open effect classification and establish a trustworthy POST-start boundary: endpoint POST serving waits for initiator Data/EndWrite after Opened. Discovery/upload-pack remains non-publishing; pre-send receive-pack cancellation is None. | Pending discovery and pre-send receive-pack cancellation assert Cancelled/None and zero POST effects; existing possible-after-send endpoint tests stay passing. |
| State P2-1 | Linearize cancellation against Opened publication in both endpoint output and driver pending queues. Suppress late success observations; retain preparation/serving/resource disposal ownership. | Deterministic queued-Opened and blocked-mux handoff barriers; one opening failure, no Opened callback, sibling survives, eventual cleanup zero. |

Blind convergence: Code P2-1 and State P2-2 independently identified the same
uncorrelated retry-budget root. Three distinct blocking roots across four findings.

One consolidated patch and a new exact tuple will be presented to the retained
reviewers. Each reviewer must verify its original counterexamples before closure.
No physical wire, public constructor, platform or selected-source work is added.
The correction remains subject to the existing two-round cap.

## Retry correlation without schema changes

The backend helper policy is immutable within a request. The private host now
guards that invariant per canonical URL and serializes opening (not the ensuing
stream exchange) across its complete automatic Anonymous→Gh pair. All callers
check/cache policy inside the same bounded gate. Explicit policy switching on
the same registered request/URL fails before effects; an independent policy
requires a fresh registration. This is a private candidate restriction, not a
new user-facing setting. A fixed disabled-helper backend never issues Gh.

At the endpoint, a qualifying Anonymous receipt retains its remaining budget
until the next same-key Gh consumes it or the request retires. There is no TTL
eviction/refill: zero remaining domains stay zero and fail before helper work.
Under the frozen Open shape, that next Gh necessarily denotes the continuation;
it cannot also denote an unrelated explicit attempt. No identity/deadline/facts
field is overloaded with hidden correlation. Supporting arbitrary mixed direct
policies in one registration would require a separate protocol design change.

Full first Failure, known stream ID and attempt policy live on the per-remote
RPC adapter and accompany final failure context; final public observation rows
remain final-attempt-only.

Owner audit also ensured preparation handles remain joined/accounted during
cancellation, every first POST data message is delivered losslessly, and SSH
pre-opening cancellation retains its original conservative classification.
These draft corrections precede correction-1 acceptance. The initial queued
opening and concurrent auth regressions were observed failing before fixes.
Later barrier/receipt/expired-domain tests extend those counterexamples; universal
per-test red-first conformance is not claimed.
