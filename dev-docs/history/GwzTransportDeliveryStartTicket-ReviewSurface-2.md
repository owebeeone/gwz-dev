# Independent delivery and start tickets — SURFACE-AXIS REVIEW (round 3)

**Review object:** `dev-docs/GwzOperationStartTicketCallerGuideDraft.md` at root commit `918627cb88ee35228850eb65ee2cc1b54917f9d5`  
**Pinned tuple:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; gwz-core `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23, Australia/Sydney  
**Axis:** Cold caller-facing Python API surface  
**Verdict:** **NO-GO** — three P2 caller-contract defects remain.

## Prior-finding closure table

| Concern | Closure from this guide alone |
| --- | --- |
| Unknown ticket `cancel()`, `wait_cleanup()`, and `release()` | **Closed for state behavior.** Lines 99–110 distinguish no-commit from commit-sent unknowns and specify each operation. The general `cancel()` return promise still conflicts with the committed-unknown exception; see F1. |
| Uniform cancel return | **Open.** Line 31 says `ticket.cancel()` always returns `StartCancelProgress`; line 108 says it can raise `GwzStartUnknownError`. See F1. |
| Error-safe examples and later cleanup | **Open.** The examples describe pending cleanup and eventual release, but several admission errors bypass their cleanup paths. See F2. |
| Accepted handle after close | **Closed.** Lines 150–175 explicitly retain same-Client ticket and handle reads after close starts and after final close, including recovery by `start_seq`. |

## Changed-range analysis

The pinned commit records 93 insertions and 19 deletions in the reviewed guide. This cold review examined only the guide’s pinned contents, not earlier text or peer reports. The relevant current ranges are the cancel contract and state table (lines 29–31, 99–110), caller examples (lines 33–130), and handle, cleanup, release, and close contracts (lines 132–175).

**NEW ARCHITECTURAL root cause:** None established. The findings concern the published caller contract and examples; this review makes no implementation or wire-protocol claim.

## 0. Evidence base

The sole substantive source was `git show HEAD:dev-docs/GwzOperationStartTicketCallerGuideDraft.md`, numbered for location references. The four pinned HEADs matched at both the beginning and end of review. The guide had no working-tree modification. No design, plan, code, prior report, or peer review was read.

## 1. Findings

### F1 — P2: `ticket.cancel()` has contradictory return and exception contracts

**Location:** Lines 31 and 101–110.

**Invariant:** A caller must know whether a cancellation call returns progress or can raise, especially when an uncertain committed start still occupies a ticket slot.

**Caller sequence:** A commit was sent, owner status becomes unknown, and the caller invokes `await ticket.cancel()`. Line 31 promises it “always returns” `StartCancelProgress`; line 108 instead requires `GwzStartUnknownError` when owner status cannot be recovered. Code written to the general promise can skip saving the uncertainty details and skip `ticket.release()` after the exception. Line 148 says that uncertain ticket remains charged until explicit release or the hard limit.

**Impact:** Incorrect exception handling delays recovery of the 64-slot local ticket budget and can lose the information needed before retrying an effect-uncertain action.

**Remedy:** State the precise success and raise contract beside the method’s first description: successful calls return `StartCancelProgress`; a commit-sent unknown with unrecoverable owner status raises `GwzStartUnknownError`. Show the exception and local release path in a cancellation example.

**Closure test:** A guide-only caller can write exhaustive branches for `await ticket.cancel()` in every row of the state table without resolving a contradiction.

### F2 — P2: Examples leave accepted work or tickets unmanaged on admission errors

**Location:** Lines 33–55, 68–95, and 112–130.

**Invariant:** Examples presented as lifecycle patterns must retain and retire every ticket and accepted handle when admission or result raises.

**Caller sequence:** In the two-ticket example, `first.accepted()` succeeds and `second.accepted()` raises a typed refusal. The second await is before the `try/finally`, so neither the accepted first handle nor the refused second ticket reaches the shown release path. The cancelled-waiter example similarly awaits `b.accepted()` before its cleanup block. The unknown-status example handles `GwzStartUnknownError` but provides no release path for an ordinary typed pre-accept refusal.

**Impact:** Following these examples can strand a running operation and retain ticket capacity until expiry. It also leaves the caller without the later cleanup observation the guide says to preserve.

**Remedy:** Put admission and result handling inside lifecycle guards for each created ticket. Cover typed refusal, cancellation, committed unknown, and accepted-handle outcomes; retain pending cleanup owners and release each locally settled ticket.

**Closure test:** For each example, inject a refusal at every `accepted()` await and a result error after acceptance. Every created ticket then reaches a documented release or retained-pending-cleanup path, and every accepted handle remains available for terminal and cleanup inspection.

### F3 — P2: Published return-value shapes are incomplete where callers must branch

**Location:** Lines 27, 31, and 132–136.

**Invariant:** A standalone caller guide for a proposed API must specify the types and fields used to inspect outcomes and make recovery decisions.

**Caller sequence:** A handle’s `result()` raises `GwzOperationError`; the caller awaits `terminal()` to inspect the immutable outcome and its conservative effects before retrying. The guide says a terminal has a `kind` and effect knowledge, but never names its return type or the field path and values for that knowledge. It likewise names `StartCancelProgress.phase` without defining its values, and calls `Client.tickets()` a bounded snapshot without specifying its element type. The caller must guess how to branch on these advertised values.

**Impact:** Consumers cannot implement or type-check the documented outcome and recovery flow from this guide alone. Different plausible field layouts would create incompatible callers.

**Remedy:** Add explicit public signatures and field definitions for the terminal value, `StartCancelProgress.phase`, and the ticket snapshot. Include a short failure example reading the terminal’s effect fields and choosing the documented retry or inspection path.

**Closure test:** Using only the guide, a caller can write concrete field accesses and exhaustive branches for terminal outcome, cancel phase, and ticket enumeration, with no inferred type or field name.

## 2. Invariant analysis

The guide supplies a workable conceptual path for two independent tickets: waiter cancellation preserves ticket identity; cancellation may race acceptance without rewriting a successful result; pending cleanup remains charged; and the same Client can recover an accepted handle after close. It also separates no-commit unknowns from committed unknowns and gives capacity remedies.

The three findings prevent that path from being fully executable as a caller contract. The cancellation exception must be unambiguous, sample error paths must preserve ownership, and the values used to inspect outcomes must have defined public shapes. None requires an architectural change on the evidence available here.

## 3. Risks and next action

Revise the guide’s method contract and examples, define the missing public value shapes, then repeat a cold guide-only caller walk. Keep the verdict **NO-GO** while these P2 defects remain. Implementation, physical wire behavior, process handling, iroh, activation, and release remain outside this review.

**End tuple check:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; gwz-core `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All unchanged.
