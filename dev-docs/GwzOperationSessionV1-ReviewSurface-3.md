# Python operation-session v1 caller API: Surface review, round 3

**Review object:** Committed `dev-docs/GwzOperationSessionV1CallerGuideDraft.md`, read as a proposed Python caller contract.  
**Baseline:** root `280cec26df5233993c570ed6452a8cd95ed6730d`; gwz-core `10dd625a46163234d55fdb957f445514fb9f3c43`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs matched at the start and end.  
**Date:** 2026-09-23, Australia/Sydney. **Axis:** adversarial, read-only review of the Python caller surface.

**Verdict: NO-GO.** The four original counterexamples are resolved for their stated cases, but the new start-recovery surface has two concrete P2 gaps. A cancelled caller cannot reliably identify its start when other starts share its correlation ID or use generated IDs. A start still pending admission cannot be cancelled individually.

## Prior-finding closure

| Original finding | Disposition | Verified original counterexample | Status |
| --- | --- | --- | --- |
| Acceptance outlives a cancelled `start_fetch()` await without a recoverable handle | Lines 27–45 add Client-owned registration, discovery, and `operation(start_token)`. | With the original single known request ID, cancellation around acceptance leaves a discoverable entry and a path to its handle without closing the peer. | **Closed for that case.** New identity and pending-start gaps are below. |
| A completed peer remains retained behind an unbounded first terminal | Lines 54–65 release the second handle before checking the first’s cleanup. | If the first handler never stops, the example has already released the completed peer. | **Closed.** |
| Early `release()` makes the later cleanup-marker remedy ineffective | Lines 80 and 89 make expiry, rather than a repeated cached release, the remedy for an early-released marker. | After early release and final cleanup, the marker expires 60 seconds after local completion; a second `release()` is expressly not prescribed. | **Closed.** |
| Fetch has no stable local/remote effect meaning | Lines 70 and 78 define remote as remote Git mutation and say fetch keeps `effects.remote=none`. | Before send, after a remote response, and during local object/ref update, the caller is directed to inspect local objects and tracking refs when local effect is uncertain or applied. | **Closed.** |

## Changed-range analysis

The new range at lines 27–45 supplies recovery for a cancelled start await. Lines 54–65 fix peer release order; lines 68–70 clarify cancellation errors and effect domains; lines 78–80 and 89 define fetch effects and cleanup-marker expiry. The latter changes resolve the earlier counterexamples.

**NEW ARCHITECTURAL ROOT A:** a start’s stable token is created inside `start_fetch()` but is exposed to a cancelled caller only through a snapshot whose correlation fields need not identify that caller’s start.

**NEW ARCHITECTURAL ROOT B:** admission remains Client-owned after the Python wait is cancelled, but the public API has no individual cancellation transition for a start that has not yet been accepted.

## Evidence base

I read the committed guide at the pinned root SHA, its diff from the prior reviewed root SHA, my permitted prior surface report, and the required `AGENTS_GWZ.md`. I did not inspect implementation, design documents, remediation plans, or other reviewers’ reports. Line references below refer only to the committed guide. This is a review of a draft API, not a claim about released behavior.

## Findings

### P2-1 — A cancelled start cannot always be matched to its snapshot entry

**Location:** lines 25, 27, 29–45. **Root cause:** `start_token` is not delivered before the cancellable await; recovery instead matches snapshot entries by `request_id`, which the guide calls correlation only and does not require to be unique. With the default `request_id=None`, generated IDs are visible in the snapshot but were never given to the caller.

**Violated invariant:** an accepted start whose Python await was cancelled must remain individually identifiable and recoverable on the same Client without disturbing a peer.

**Credible sequence:** two concurrent `start_fetch()` tasks use the documented defaults, or the same caller request ID. One is cancelled as admission completes while the other remains pending. `client.operations()` shows two entries, but neither task received a start token and the snapshot contains no task-to-start association. The sample’s `matches[0]` can select the peer when IDs are reused; with generated IDs, the caller cannot even form that match. Waiting for the peer to settle may be unbounded.

**Impact:** the caller may cancel the wrong fetch, leave the intended accepted fetch running, or be unable to retrieve its individual outcome. **Correction:** make a unique start identity available synchronously before the first await, or enforce and document unique in-flight request IDs with an atomic mapping from the cancelled task to its entry. **Regression test:** start two otherwise identical fetches with omitted IDs and with a repeated explicit ID; cancel one await on both sides of acceptance and verify that the caller recovers and controls exactly that start while the peer remains usable.

### P2-2 — A pending pre-accept start has no individual cancellation path

**Location:** lines 27, 29, 41–45, and 82. **Root cause:** cancellation of the awaiting Python task deliberately leaves admission Client-owned, while `operation(start_token)` only waits for settlement and individual handle cancellation becomes available after acceptance. The only documented way to cancel pre-accept starts is `client.close()`, which also cancels peers.

**Violated invariant:** cancellation recovery should let a caller stop the affected start without closing healthy operations on the shared Client.

**Credible sequence:** start A is registered and remains pending admission, for example while endpoint construction or a capacity wait does not settle. The caller cancels A’s awaiting task, finds its `start_token`, and calls `client.operation(start_token)`; that call waits for the same pending admission. Start B is healthy on the Client. Closing the Client stops B as well, and the guide gives no individual action for A before acceptance.

**Impact:** an unwanted pending start may keep consuming admission resources or become accepted later, while the caller has no bounded, peer-preserving way to stop it. **Correction:** specify an individual `cancel_start(start_token)` operation with an atomic acceptance-race result: it either stops the pending start or returns the accepted handle and cancels that operation. **Regression test:** hold one start pending before acceptance, cancel its Python await, then cancel that start through its token; verify that a second operation can complete and the Client remains open. Repeat at the acceptance boundary.

## Invariant analysis

The guide now gives a coherent first-day path for two accepted handles: `cancel()` and `wait_cleanup()` are bounded, the second result is independent of first cleanup, and the example releases the second terminal promptly. It distinguishes terminal outcome from physical cleanup and keeps retained reads available on the same Client through pending and final close. Capacity refusals have typed context and specific recovery conditions. Fetch’s remote effect stays `none`; uncertainty after object or ref writes belongs to the local effect.

The unresolved boundary is earlier than handle creation. Registration before the first await prevents a cancelled wait from silently dropping admission, but recovery requires both a reliable identity and a way to stop admission while it is still pending. The current guide guarantees neither in the sequences above.

## Risks and next action

Resolve the two start-lifecycle gaps, then rerun the caller walkthrough with simultaneous default-ID starts, duplicate explicit IDs, cancellation immediately before and after acceptance, and admission that remains pending while a peer completes. The revised contract should demonstrate exact start identification and individual cancellation in each case.

