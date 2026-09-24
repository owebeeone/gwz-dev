# Python operation-session v1 caller API: Surface review

**Object:** committed `dev-docs/GwzOperationSessionV1CallerGuideDraft.md`, a proposed and unavailable Python caller contract. This review covers the public surface only; it does not judge implementation or release readiness.

**Exact tuple:** root `f2f10aeeafa0e0443a88f50983435422980de9f3`; core `28eb3d59d62282eba8cb2f376d51ea0521c50190`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.

**Date:** 2026-09-23 (Australia/Sydney). **Axis:** adversarial, peer-blind, read-only review of the caller-visible operation lifecycle. **Verdict: NO-GO** for this proposed contract while the P1/P2 findings below remain. The main improvement is real: a stalled cancellation no longer has to block a peer's `result()`, and retained results remain available after close.

## Prior-finding closure

| Prior concern | Status in this draft | Evidence and remaining boundary |
| --- | --- | --- |
| S1: stalled cleanup blocks peer result | **Closed for result; example still needs correction.** | Lines 35-40 explicitly let `second.result()` finish before waiting for first cleanup. Lines 46-48 then await the possibly unbounded first terminal before releasing the completed peer; P2-1 covers the resulting retained-capacity trap. |
| S2: individual outcomes after pending/final close | **Closed.** | Line 63 preserves each handle's `result()`, `terminal()`, retained `events()`, `wait_cleanup()`, and `release()` through pending and final close on the same reachable Client. It also states that `async with` exit follows this rule and that a replacement Client cannot reattach. |
| S3: capacity refusal recovery | **Substantially closed, with one contradiction.** | Lines 65-75 distinguish charged live work, retained records, session limits, capacity epochs, placement, minimum backoff, and ownership visibility. The `RetainedCapacityFull` remedy conflicts with early-release idempotency; P2-2 covers it. |
| S4: local/remote effect meaning and retry | **Partially closed.** | Lines 53-59 separate local and remote evidence, state that peer cleanup does not prove an effect, and make `none` a proof claim. For the sole documented `start_fetch()` action, the remote Git effect and retry instruction remain ambiguous; P2-3 covers it. |

## Changed-range analysis

The rejected session draft directed a pending cancellation toward shared `close()` and said closing released completed records. The rejected cleanup draft corrected bounded cancellation and independent peer work but still left post-close retrieval and recovery detail thin. The new draft changes the public surface materially: it adds `wait_cleanup()`, splits immutable terminal outcome from later cleanup progress, preserves same-Client reads after close, introduces an early terminal release with a surviving cleanup owner, distinguishes local from remote effects, and publishes a typed capacity table. Those are the ranges tested here. The new early-release and retry language creates the specific inconsistencies below; the older drafts are historical comparison only.

## §0 Evidence

For substantive evidence, I read only the committed proposed caller guide, the two explicitly permitted rejected caller drafts, and the committed `gwz-py/README.md`; I also followed the workspace's required `AGENTS_GWZ.md` instructions. I did not inspect source, internal designs, plans, amendments, or peer reports. Locations below are line numbers in `dev-docs/GwzOperationSessionV1CallerGuideDraft.md` at the root commit in the tuple. The README establishes that current callers use `Client` and streaming operations; the reviewed handle API is expressly unreleased (guide line 3). The tuple was checked at the beginning and end of this review.

## §1 Findings

### P1-1 — Acceptance can outlive a cancelled `start_fetch()` await without a recoverable handle

**Location:** lines 5, 25, 27, 63, and 65. **Root:** the guide promises a handle after acceptance and calls `request_id` correlation only, but does not define the atomic boundary between receiver acceptance and delivery of that handle to an awaiting Python task.

**Sequence:** a task awaits `client.start_fetch(request_id="fetch-a")`; the receiver accepts and charges the operation; the Python task is cancelled before the await returns the handle. The operation may execute or remain queued, but the caller has neither its `operation_id` nor a documented same-Client lookup by request ID. `Client.cancel_operation(id)` is only a possible convenience, not a committed recovery path. The only documented broad action is `client.close()`, which cancels healthy peers too.

**Consequence:** an accepted effectful operation can become caller-invisible while still consuming capacity and possibly changing local state. The caller cannot reliably observe its terminal/effect facts or cancel only that operation. **Correction:** specify and expose a recoverable acceptance handoff: either cancellation before handle delivery guarantees no admission, or an accepted operation can be found on the same Client by a stable identifier returned/recorded through the cancelled await path. State how the terminal and cleanup owner are retrieved. **Closure test:** cancel the awaiting task at every point around acceptance and response delivery; in every accepted case the caller can identify, observe, and individually cancel/release the operation without closing a peer.

### P2-1 — The first-day example can strand a completed peer behind an unbounded first terminal

**Location:** lines 35-48 and 51. **Root:** the sample awaits `first.terminal()` before `second.release()`, although it expressly permits first's handler to run forever.

**Sequence:** first cancellation returns pending after five seconds; second succeeds and `second.result()` returns; first's native handler never stops. The sample reaches line 46 and never reaches line 48. **Consequence:** the completed peer's terminal and event log remain retained until expiry, and repeating this otherwise successful workflow can exhaust retained capacity even though independent peer release is available. **Correction:** release the completed second handle immediately after consuming its result/events, before any await on first's possibly stalled terminal. Show the first terminal as an optional later observation. **Closure test:** run the documented sequence with permanently pending first cleanup and verify the sample still releases second and can continue submitting work subject only to first's real capacity charge.

### P2-2 — Early `release()` leaves no documented way to release a later-completed cleanup marker

**Location:** lines 61 and 70. **Root:** release is locally idempotent on the same Python handle and cannot abandon pending cleanup, while the `RetainedCapacityFull` table tells callers to release completed cleanup markers.

**Sequence:** first reaches a terminal while physical cleanup is pending; the caller calls `first.release()`, which frees terminal/events but retains the cleanup owner. Cleanup finishes later and its small report remains for 60 seconds. A subsequent `first.release()` succeeds from the local cache without a native call, so the caller cannot perform the table's stated marker-release remedy. **Consequence:** a caller refused for retained-marker capacity is directed to an action that cannot free this marker; only expiry can, despite the table promising release as a recovery lever. **Correction:** either make an already released handle able to retire its completed cleanup marker through an explicit operation, or say clearly that early-released markers recover only on expiry and separate that case from releasable retained terminals. **Closure test:** release a terminal during pending cleanup, let cleanup finish, induce marker retention pressure, and verify the documented remedy actually frees capacity or explicitly directs the caller to expiry.

### P2-3 — `effects.remote` has no stable meaning for the only documented action, fetch

**Location:** lines 5, 25, and 53-59. **Root:** the guide defines `remote` as an applied effect and gives a row for a confirmed “remote Git effect,” but `start_fetch()` is the only handle action shown. A fetch obtains data from a remote; the state it normally changes is local. The guide never says whether a completed remote read counts as `applied`, whether remote repository mutation is meant, or whether server-side incidental effects are in scope.

**Sequence:** cancellation occurs after the remote has served a fetch request but before local ref update is confirmed. One reader can treat `effects.remote=applied` as “the request was served”; another can treat it as “remote Git state changed.” The table tells both to inspect remote state before another action, which cannot establish whether the local fetch was applied. **Consequence:** callers cannot use the two effect fields for a consistent retry decision, precisely where the guide asks them to do so. **Correction:** define each effect domain in terms of observable state for `start_fetch()` and give a fetch-specific cancellation/retry example; if remote Git mutation is impossible for fetch, say when `effects.remote` must be `none` and direct the caller to inspect local refs/results when local state is uncertain. Keep any future push-like action in its own table. **Closure test:** for cancellation before request send, after remote response, and during local ref update, two independent readers should derive the same permitted local/remote effect values and the same state to inspect before retrying.

## §2 First-day walkthrough and invariant analysis

A first-day caller can start two fetches with the documented defaults: no selectors means root plus all members; empty selectors add none; `None` root, remote, policy, attribution, and transport use the stated Client or repository resolution; the new retry default is three extra setup attempts. Admission returns a handle before work/events; admission errors precede `Accepted`. The caller can cancel first, receive pending progress within five seconds, and obtain second's `result()` without joining first. If first's handler never stops, first's terminal and local completion remain pending and charged. The guide makes that limit explicit. The sample should release second before waiting indefinitely for first (P2-1).

For observation, events are ordered per operation and a cursor gap is explicit; the terminal survives event loss. The guide preserves an original generated failure response in `GwzOperationError.response` when one exists, and uses `None` for a bounded `ResultLimitExceeded`. A cancelled `result()` is described as raising `OperationCancelled` after the paragraph says failures raise `GwzOperationError`; the intended subclass/code relationship should be made explicit during editing, so callers know whether one `except GwzOperationError` catches cancellation. This wording alone is not a separate gate finding here.

For cleanup, `local_completion_confirmed` is the only stated proof that local work stopped; a zero pending count is insufficient. Terminal may precede physical cleanup, release may precede local completion, and peer cleanup can remain unconfirmed after local completion. Those invariants are coherent. The early-release marker remedy is not (P2-2). A reachable same Client can query individual records after `ClosePending`, final `close()`, and context exit until expiry; dropping it loses caller reattachment. Foreign or replacement Clients cannot retrieve records. This closes the earlier individual-outcome concern while making the acceptance handoff especially important (P1-1).

Capacity refusals give scope, counters, a minimum one-second backoff, and different recovery conditions. The guide correctly says a timeout, result release, or lost route does not free still-running physical work. No finite completion deadline is promised for a stuck native handler. Local and remote effects remain separate from peer cleanup and terminal effects are conservative, but the fetch-specific meaning must be settled before callers can follow the retry table (P2-3).

## §3 Risks and next action

Resolve P1-1 at the API contract boundary first, since it can leave an accepted operation without any caller-visible identity. Then revise the example and cleanup-marker recovery language, and define fetch effect domains with a concrete retry sequence. After those edits, repeat the two-operation walkthrough with cancellation at acceptance, indefinite first cleanup, a peer success, an event gap, early release, `ClosePending`, final close, context exit, record expiry, and capacity refusal. This remains a surface review of a proposed API; it makes no claim about present implementation behavior.
