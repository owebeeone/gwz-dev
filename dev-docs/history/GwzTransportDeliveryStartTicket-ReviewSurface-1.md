# Independent delivery and start tickets — SURFACE-AXIS REVIEW (round 2)

**Review object:** Proposed, unavailable Python API in `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`  
**Pinned tuple:** root `a1f42eaabf690cac1f4ca54804550c482a1d417d`; gwz-core `e06c752dbe4048a7f319b28d8351702490e63e7f`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23  
**Axis:** SURFACE — cold Python caller  
**Verdict:** **NO-GO** while the P2 findings below remain open.

## Prior-finding closure table

These four concerns were checked independently against the pinned guide; no prior report was read.

| Concern | Assessment | Evidence |
| --- | --- | --- |
| Release of a start that never becomes a handle | **Partial.** `ticket.release()` explicitly covers pre-accept refusal and cancellation. The disposition of an `unknown_or_expired` committed start remains unclear. | Lines 82, 100; P2-1 |
| Repeat `accepted()` after a cancelled waiter | **Partial.** Repeatability and same-handle recovery are explicit, including an example. The post-`close()` readable-method list omits `accepted()`. | Lines 29, 50–80, 102; P2-4 |
| Structured unknown/expired error | **Closed for the stated concern.** The guide specifies a typed error, code, start sequence, commit flag, effect domains, and optional operation and cleanup facts. | Line 82 |
| Cleanup of the healthy peer in the example | **Partial.** The success paths release the peer handle. An error from `result()` skips release, and pending cleanup has no illustrated later completion path. | Lines 34–48, 53–80, 86; P2-3 |

## Changed-range analysis

Only this pinned guide was reviewed. Without reading an earlier version, I cannot identify textual changes or establish that an architectural root cause is **new**. The relevant contract ranges are ticket identity and recovery (27–31), examples (33–80), unknown admission (82), handle and cleanup behavior (84–100), and closed-client recovery (102). No new transport-architecture defect is established by this surface review. P2-1 is an unresolved API lifecycle boundary.

## 0. Evidence base

I read the guide at the pinned root commit, with line numbers, and verified all four repository SHAs both before and after the review. I did not inspect implementation, design documents, plans, peer reports, builds, or working-tree changes. Findings assess the proposed caller contract, not implementation completeness.

## 1. Findings

### P2-1 — Unknown committed tickets have no unambiguous cancel-and-release disposition

**Location:** Lines 82 and 100.

**Root cause:** The guide defines `unknown_or_expired` after a lost commit outcome, but does not say whether that state counts as “admission has settled” for `ticket.release()`, or what `ticket.cancel()` does in that state. Line 100 says a pending commit refuses release while an effect-uncertain committed ticket remains charged until explicit release or the hard limit.

**Violated invariant:** Every allocated ticket needs a defined same-client disposition, including one that never yields a handle, without treating uncertain Git effects as absent.

**Credible caller sequence:** A commit is sent, its reply is lost, and retained status expires. `accepted()` raises `GwzStartUnknownError(commit_sent=True)`. The caller inspects local Git state and wants to retire its ticket slot. It cannot tell whether `release()` succeeds, refuses because the commit remains pending, or discards the only cleanup observation. It likewise cannot tell whether `cancel()` can signal the original owner.

**Impact:** Recovery code cannot reliably free the 64-slot local ticket budget or preserve the intended cleanup observation.

**Remedy:** Specify `cancel()`, `wait_cleanup()`, and `release()` outcomes for both unknown cases (`commit_sent=False` and `True`), including whether release retires only the local ticket while an active cleanup owner remains charged.

**Closure test:** A caller-facing state table and example must let a reader handle a lost commit reply through unknown status, inspect effects, call or decline cancellation, and release the local ticket with a stated result and retained-cleanup rule.

### P2-2 — `ticket.cancel()` has no usable return type across the acceptance race

**Location:** Lines 31, 39, and 88.

**Root cause:** The guide says post-accept cancellation returns an operation ID “plus cleanup progress,” but does not define the return shape. The enumerated `OperationCleanupProgress` fields contain no operation ID.

**Violated invariant:** One cancellation call must have a predictable Python result that a caller can inspect whether admission loses or wins the race.

**Credible caller sequence:** The caller invokes `await ticket.cancel()` while commit admission is in flight and wants to record the operation ID if acceptance won, then inspect `local_completion_confirmed`. The guide leaves open whether the result is a progress object, tuple, or wrapper.

**Impact:** Correct race-handling code cannot be written from the proposed API; a caller can misread the result or lose the accepted identity.

**Remedy:** Define one concrete return type with a nullable `operation_id` and named cleanup-progress field, or make the return consistently `OperationCleanupProgress` and direct callers to repeat `accepted()` for the ID. Show both race outcomes.

**Closure test:** A typed example accesses the documented fields after cancellation before permit and after acceptance, without changing the result shape.

### P2-3 — First-day examples strand ticket slots on ordinary failures and pending cleanup

**Location:** Lines 34–48 and 53–80.

**Root cause:** Both examples release healthy handles only after `result()` succeeds. The guide explicitly says `result()` can raise. The cancelled-handle branches stop when a five-second cleanup observation is pending, without showing how the caller later releases a settled ticket.

**Violated invariant:** Example usage should restore a ticket slot after an outcome is known, and retain a clear owner for cleanup that is still pending.

**Credible caller sequence:** `second.result()` raises a Git or transport `GwzOperationError`, so its following `release()` is skipped. Repeating the example eventually fills the 64 retained Python ticket slots. Separately, cancellation returns pending, later completes, and the example never revisits or releases the first ticket.

**Impact:** The published walkthrough teaches a capacity leak under normal error and timeout paths, leading to synchronous `CapacityBusy(resource=start-ticket)`.

**Remedy:** Put settled-handle release in an error-safe `finally` path. For pending cleanup, show the retained ticket/Client and a later `wait_cleanup()`/terminal/release path; do not imply the five-second observation completes cleanup.

**Closure test:** Walk the examples with `result()` raising and with cancellation initially pending but later completing. Every settled ticket is released, and a still-running one retains a reachable owner.

### P2-4 — Closed-client recovery omits repeat `accepted()`

**Location:** Lines 29, 50–80, and 102.

**Root cause:** The guide promises every later `accepted()` call returns the same known handle, yet the explicit post-`close()` readable-method list includes `result()` and other methods but omits `accepted()`.

**Violated invariant:** A cancelled waiter must be able to recover a known acceptance through the same retained Client for the lifetime of its record, including after close begins.

**Credible caller sequence:** A waiter awaiting `accepted()` is cancelled; admission then succeeds; `client.close()` starts and returns `ClosePending`. The caller retrieves the ticket by `start_seq` but cannot tell whether repeating `accepted()` remains supported to obtain the handle for events, terminal, and release.

**Impact:** The documented same-client recovery path becomes uncertain during shutdown, when it is most needed.

**Remedy:** Explicitly include retained-ticket `accepted()` in the closed-client read contract and state its result for accepted, refused, cancelled, and unknown tickets.

**Closure test:** A documented sequence cancels an acceptance waiter, starts close, retrieves the ticket, repeats `accepted()`, and releases its completed handle.

## 2. Invariant analysis

The guide establishes useful boundaries: `start_seq` identifies the ticket within one Client; caller `request_id` does not authorize cancellation; cancelled waiters leave the ticket intact; a replacement Client cannot recover it; and unknown committed effects are not automatically retried. The structured unknown error supports effect-aware inspection. The remaining surface gap is the lifecycle at that unknown state and at close, plus examples that do not consistently pair retained identities with release.

## 3. Risks and next action

Resolve the four P2 contract defects in the guide before API approval. In particular, publish a complete ticket-state method table and make the walkthrough executable through failure, pending cleanup, and post-close recovery. The pinned repository tuple was unchanged at the end of inspection.
