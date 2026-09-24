# Gwz operation cleanup ownership — Surface review

**Object:** Proposed, unreleased Python caller interface in `dev-docs/GwzOperationCleanupCallerGuideDraft.md`  
**Tuple:** root `b78f8c2b9db4929a86d03c8642c568ae8864baef`; core `d3951dcfc04d2f09c9c7a025dec41ee3163c4058`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23  
**Axis:** Surface — Python caller lifecycle  
**Verdict:** **NO-GO** — four P2 findings

## §0 Evidence base

I read only the committed cleanup caller guide, the rejected earlier caller guide for background, and the committed `gwz-py/README.md`. The exact tuple matched at the start and end of review. I did not inspect code, internal design, plans, or other reviews. This verdict concerns the proposed caller contract, not implementation or release readiness.

## §1 Findings

### S1 — P2: The example blocks the peer behind potentially endless cleanup

**Location:** Cleanup caller guide, lines 11–20 and 31.

**Sequence and consequence:** Start `first` and `second`; cancel `first`; its native handler never stops. The example repeatedly awaits `first.wait_cleanup()` before it reaches `second.result()`. The guide explicitly promises no maximum time to local completion. Although `second` can continue executing, the demonstrated caller cannot observe its result. This undercuts the stated purpose of keeping peers independent.

**Correction:** Show the peer’s result being observed independently of the cleanup wait, then offer a separate, optional cleanup wait or polling task. Do not make completion of the cancelled operation a prerequisite for reading its peer.

**Closure test:** In the documented sequence, hold `first` cleanup pending indefinitely and complete `second`; the caller can obtain `second.result()` without closing the client or awaiting `first` cleanup.

### S2 — P2: Outcomes after client close are undefined

**Location:** Cleanup caller guide, lines 29–31.

**Sequence and consequence:** Start two effectful operations, then call `client.close()` or leave its context before reading their terminals. Close cancels both and eventually reports aggregate local and peer-cleanup facts. The guide does not say whether either handle’s `result()`, `terminal()`, or cleanup progress remains readable after a pending or final close, nor whether the close report identifies each operation’s outcome and effect. A caller therefore cannot know how to inspect each operation before deciding whether to retry. This is especially significant when context exit raises `ClosePending`.

**Correction:** Define handle validity and per-operation result retention across pending and final close, including context exit and release. Alternatively, make close return a per-operation outcome report sufficient for recovery. State how a caller holding `ClosePending` retrieves those outcomes.

**Closure test:** Close a client with two outstanding operations and different effects; after close begins and after it finishes, the documented API provides each operation’s distinct terminal outcome and retry-relevant effect until a defined release or expiry point.

### S3 — P2: Capacity refusals lack an actionable recovery contract

**Location:** Cleanup caller guide, line 33.

**Sequence and consequence:** Pending cleanup can hold a finite receiver resource indefinitely. A subsequent start receives `CapacityBusy` or `CapacityConflict`. The guide tells the caller to wait for “relevant cleanup” or release records “as indicated by the error,” but defines no error fields that identify the constrained resource, its scope, an owned operation that can be awaited, or the condition that makes retry useful. It also mixes release of completed result records with `CapacityBusy`, although the described blocker may be physical work that release cannot free. A caller can only guess or poll.

**Correction:** Define typed refusal details and retry conditions for pending-work, retained-result, and capacity-epoch conflicts. If the blocker belongs to another client and cannot be identified, say so and provide a safe retry signal or backoff contract.

**Closure test:** For each refusal caused by pending cleanup, retained records, and an old capacity epoch, the documented error tells the caller which available action can change admission and which actions cannot.

### S4 — P2: `effect` does not yet support the promised retry decision

**Location:** Cleanup caller guide, lines 16–18 and 25–29.

**Sequence and consequence:** Cancellation returns progress with `effect` values `none`, `may_have_applied`, or `applied`; a final `peer_cleanup_confirmed=False` means remote uncertainty. The guide does not define whether `effect` describes a local workspace mutation, a remote Git action, or both, or what evidence permits `none` and `applied`. Its example warns against assuming whether the remote applied the action even after an `effect` value is available. A caller cannot safely interpret `none` as permission to retry or `applied` as proof that retry is unnecessary.

**Correction:** Define the scope and evidentiary meaning of each value, including its relationship to peer-cleanup confirmation and the cancelled operation’s terminal. State which combinations require remote inspection before retry.

**Closure test:** Document outcomes for cancellation before sending an effectful request, after sending but before acknowledgement, and after confirmed remote application. A caller can decide from each documented combination whether retry is safe, unsafe, or requires inspection.

## §2 Walkthrough and invariant analysis

The guide clearly says cancellation of `first` does not cancel `second`, that a terminal may precede final physical cleanup, and that releasing a terminal does not abandon cleanup ownership. Thus, after `first` has a terminal, the proposed handle can release it and still call `wait_cleanup()` while `second` runs.

The walkthrough ceases to be determinate at two points. If `first` never reaches local completion, the example never observes `second`, despite that independence. If the caller closes the client while either operation is outstanding, the guide does not specify how to obtain each terminal or effect afterward. The five-second returns from `cancel()`, `wait_cleanup()`, and `close()` are described as pending reports rather than completion guarantees; that distinction is sound.

## §3 Risks and next action

The central surface risk is **recoverability**: callers need to observe peers during another operation’s stalled cleanup and retrieve individual outcomes after shared close. Resolve the four contracts above in the guide, then re-walk overlapping cancellation, terminal, release, capacity refusal, and context exit using only the public API description.
