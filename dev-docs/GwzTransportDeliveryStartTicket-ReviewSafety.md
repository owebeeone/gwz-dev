# GWZ start-ticket and transport-delivery design — Safety review

**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict: NO-GO**

**Review object:** `dev-docs/GwzOperationStartTicketDesign.md`, `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`, `gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md`, and `gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md`. This is a review of the proposed interface freeze, not an implementation or release verdict.

**Baseline:** root `2ac74b10668e25e63eec83e258e17aeb91cdf91d`; `gwz-core` `590eefe26a0be6b59fda1f190a94d927b88938bb`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs matched at the start and end of this read-only review. The reviewed files had no working-tree diff.

**Evidence base:** The four review documents; accepted `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` `4 and `gwz-core/docs/TransportPlacement.md` “Port forwarding”; current `gwz-transport/src/mux/mod.rs`, `src/mux/asynchronous.rs`, and `src/mux/routing.rs`; `gwz-py/dev-docs/GwzPyTransportDesign.md`; and the applicable review rules in `dev-docs/AgentProcessRules.md`. The rejected operation-session v1 documents were treated as historical evidence only. No tests were run because this is a design review.

## Findings

### S-1 — P1: A blocked data delivery can prevent control delivery

**Root cause and location:** The delivery amendment requires one serial `next_message()` → `deliver()` pump per direction and forbids reading the next item until the previous delivery completes (`gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:19`). It also requires control progress under data saturation (`:19`, `:29`) and global per-direction order (`:19`), but gives no mechanism to reconcile those requirements. The accepted port contract says `deliver()` waits for admission (`gwz-core/docs/TransportPlacement.md:244-250`). The current mux returns `WouldBlock` when its destination queue is full, and the async port waits in that case (`gwz-transport/src/mux/mod.rs:623-647`; `src/mux/asynchronous.rs:128-135`).

**Violated invariant:** Window, Cancel, Failed, Close, and shutdown must make progress even when data is backpressured.

**Credible sequence:** One pump dequeues a Data frame. Its peer’s data queue is full, so `deliver()` waits. A Cancel or Window frame becomes available behind that item. The pump cannot call `next_message()` again, regardless of the reserved control queue space. If the peer needs that control frame to end or drain the stalled work, both sides remain charged and the five-second cleanup call only reports pending. The other directional pump does not unblock this same-direction head-of-line wait.

**Impact:** Cancellation and cleanup can stall indefinitely under an ordinary saturation condition, potentially exhausting operation and endpoint capacity. This defeats a stated condition for admitting concurrent operations and for advertising the placement.

**Required correction:** Specify and implement a delivery schedule that can advance control around blocked bulk data while preserving necessary per-stream causal order, or define a bounded fail-closed transition that wakes all affected owners before a control frame is trapped. The contract must state which ordering guarantee applies; global per-direction order and unconditional control progress cannot both rely on the stated serial blocking pump.

**Regression test:** Fill the destination data queue, suspend its bulk consumer, then issue Window, Cancel, Close, and shutdown on that binding while another operation is active. Prove bounded control progress or explicit binding failure, exact ownership transfer, and no false local-completion report. Repeat with the pump paused after dequeue and before `deliver()`.

### S-2 — P2: Refused local start tickets have no defined way to free their 64-slot registry

**Root cause and location:** `Client.start_fetch()` reserves one of 64 local ticket entries synchronously; final release or TTL is said to remove it (`dev-docs/GwzOperationStartTicketDesign.md:61`). The guide directs callers to release or await expiry when those slots fill (`dev-docs/GwzOperationStartTicketCallerGuideDraft.md:88`). Yet the specified release API belongs only to an accepted `OperationHandle` (`:65`, `:79`). A ticket refused at prepare, including the stateless no-slot refusal, never gets such a handle. Neither document defines a local ticket release method or an expiry duration and trigger for these pre-accept outcomes.

**Violated invariant:** A bounded Client registry must have a defined recovery path after tickets settle without acceptance, without discarding a live or effect-uncertain start.

**Credible sequence:** Receiver retained-record capacity is full. A Client creates 64 tickets; each receives stateless `RetainedCapacityFull` before any commit. All 64 local entries are now settled but retained. The caller cannot obtain an `OperationHandle` to release, and the contract does not say when those local entries expire. Once receiver capacity frees, `start_fetch()` still refuses synchronously on the Client’s own ticket cap.

**Impact:** A transient receiver-side capacity condition can strand an otherwise healthy Client behind its independent local cap. Implementations could choose incompatible eviction behavior for refused, cancelled, and unknown-after-commit tickets.

**Required correction:** Define the local ticket lifecycle explicitly: a release operation or automatic retirement rule for settled pre-accept refusals, plus a finite retention rule for unobserved outcomes. Keep committed or effect-uncertain tickets inspectable until their stated recovery boundary, and specify how their registry charge ends.

**Regression test:** Fill all 64 local entries with stateless prepare refusals, restore receiver capacity, retire those settled tickets through the documented API or expiry, and verify a new start succeeds. Separately verify that a committed ticket with a lost reply is not retired as a proved no-effect refusal.

## Invariant assessment and next action

The ticket protocol establishes a useful no-effect boundary before permit receipt, a monotonic sequence watermark against expired-permit replay, owner-checked status recovery after lost commit replies, and charged pre-accept ownership through constructor and registration races. The terminal and cleanup contracts also distinguish handler completion, local drain, peer confirmation, and Git effects; they do not claim rollback from cancellation or transport closure. I found no separate blocking defect in those stated invariants.

Resolve S-1 and S-2 in the paired documents, add the specified regression cases to the activation gates, and request independent re-review of the corrected settled tuple. The deferred physical wire, separate-process, and iroh work does not remove either in-process obligation.

