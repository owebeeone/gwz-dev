# GWZ operation session v1 — Safety review, round 3

**Review object:** Committed [consolidated design](GwzOperationSessionV1Design.md), [caller guide](GwzOperationSessionV1CallerGuideDraft.md), and [core capacity amendment](../gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md).  
**Baseline:** Root `f2f10aeeafa0e0443a88f50983435422980de9f3`; core `28eb3d59d62282eba8cb2f376d51ea0521c50190`.  
**Reviewed tuple:** Root `280cec26df5233993c570ed6452a8cd95ed6730d`; core `10dd625a46163234d55fdb957f445514fb9f3c43`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs matched at the start and end.  
**Date:** 2026-09-23. **Axis:** Safety. **Verdict: NO-GO — one P2 finding.**

## Prior-finding closure

| Original finding | Disposition | Verified original counterexample | Status |
| --- | --- | --- | --- |
| Cleanup-ownership Safety P2-1: construction can outlive close before an endpoint exists. | Constructor is part of a charged `AdmissionScope` from reservation; close joins it and route loss transfers it. | Pause construction before publication, then close or lose the route. A losing endpoint drains before charge release; final close remains pending. [Design lines 47, 57, 65–67](GwzOperationSessionV1Design.md). | Closed at contract level. |
| Stopped protocol Safety P2-1: a five-second cleanup snapshot can be mistaken for physical retirement. | Completion requires producer sealing, handler join and actual local drain; zero pending work in an earlier snapshot is insufficient. | Return a timed report while request cleanup continues. The scope and physical charge remain live, so final close and epoch resize cannot follow that snapshot. [Design lines 59, 63, 69–71](GwzOperationSessionV1Design.md). | Closed at contract level. |
| Stopped protocol Safety P2-2: no termination bound supports a fixed orphan-retirement promise. | The design removes the fixed retirement deadline. A stuck native handler remains charged and yields pending progress and typed capacity refusal. | Keep the native handler blocked past five seconds. Its worker and scope remain owned; close cannot report finality. [Design lines 55, 63–67](GwzOperationSessionV1Design.md). | Closed at contract level. |
| Operation-session v1 Safety P2-1: request registration after endpoint publication and before `Accepted` lacked a close/route-loss owner. | One charged `AdmissionScope` now spans reservation, construction, request registration and atomic acceptance. A cleanup ticket precedes registration side effects; a losing registration seals and drains. The core epoch transfers at acceptance without a gap. | Pause after registration but before readiness/`Accepted`; race close, route loss or failure. The task and partial request remain owned and charged, cannot accept after Closing, and prevent final close or resize until drained. [Design lines 47–49, 57, 65–69](GwzOperationSessionV1Design.md); [core amendment lines 98–115](../gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md). | Closed at contract level; implementation proof remains an activation gate. |

## Changed-range analysis

The root changes add token-based start recovery, typed error contexts, a finite generation ceiling, and the full pre-accept ownership handoff. The paired core changes extend the physical epoch reservation through request registration and acceptance. The caller guide now explains cancelled start-waiter recovery and releases a completed peer before waiting on a stalled one. These changes close the prior registration race.

**NEW ARCHITECTURAL ROOT — bounded refusal history conflicts with unconditional token recovery.** The new start-status contract promises the original typed refusal for every repeated owner/token, while refused starts consume bounded retained-record slots. It does not define a recoverable outcome when those slots are already full. This root was introduced by the round-3 start-recovery interface; it is separate from the prior constructor and registration ownership defects.

## Evidence base

I compared the two committed document ranges and read the prior [Safety report](GwzOperationSessionV1-ReviewSafety.md), [round-2 remediation plan](GwzOperationSessionV1-RemPlan-2.md), relevant prior safety verdicts, [AgentProcessRules](AgentProcessRules.md) and [GwzProcessOptimization](GwzProcessOptimization.md). I checked the committed core request path: [TransportRuntime::request](../gwz-core/src/transport_host/mod.rs) registers a client request, installs capacity, begins mux work and awaits readiness; [TransportRequest and ClientRequest](../gwz-core/src/transport_host/request.rs) distinguish cancellation/sealing on drop from awaited cleanup. This was a read-only design and source review. No version-1 implementation or test was run.

## Finding

### [P2-1] A full retained-record pool cannot preserve the promised original refusal

**Root cause and location:** [Design lines 22–23](GwzOperationSessionV1Design.md) promise that repeating a token or querying `operation.start_status_v1` returns its original refusal. [Line 49](GwzOperationSessionV1Design.md) requires a refused/setup-failed token to retain its typed error for 60 seconds, and [line 51](GwzOperationSessionV1Design.md) puts settled refused entries in a retained slot. Yet [line 41](GwzOperationSessionV1Design.md) caps retained records at 64 per session and 256 per receiver, reserves those slots for accepted terminal/cleanup records, and supplies no separate reserved refusal capacity or rule for failure to reserve it.

**Violated invariant:** A start token must have one recoverable original outcome for its advertised retention period, while admission must never exceed fixed record capacity.

**Credible sequence:** Other Clients fill the receiver’s 256 retained slots with valid terminal or cleanup records. A Client with free local slots submits a new token. The receiver must return `RetainedCapacityFull` before acceptance, but has no retained slot in which to store that refusal. Lose the refusal reply. The Client-owned task cannot recover the promised original typed error through `start_status_v1`. If it repeats the token after a slot frees, the receiver can instead accept Git work unless a separate, unspecified negative-outcome owner exists.

**Impact:** Start recovery can remain pending indefinitely or change a token’s original refusal into a later acceptance. This is a concrete recovery and idempotency defect at ordinary capacity saturation.

**Correction:** Define the bounded start-token admission boundary explicitly. Reserve any required receiver-side status record before work that needs idempotent recovery; specify a stateless, provably no-work outcome when even that reservation is unavailable, including what `start_status_v1` and repeat submit return. Make the Python registry reflect that distinction. The contract must never promise 60-second recovery for an error it had no capacity to record.

**Regression test:** Fill all receiver retained slots from other Clients, submit a new token from a Client with local capacity, lose its refusal reply, and query/repeat the token both before and after a slot frees. Verify a defined, bounded recovery result and no unexpected Git start. Repeat for setup failure after a status reservation and verify exactly one original outcome and one charge release.

## Invariant analysis

The revised `AdmissionScope` covers the original paused-registration attack: it owns the partial request, cleanup ticket and physical epoch until acceptance or sealed local drain. Closing forbids a late acceptance; route loss transfers existing charges without requiring a new supervisor slot. A blocked native handler truthfully keeps its worker and scope charged, while a completed peer can publish and release its own terminal. Final close depends on local drain, not peer confirmation or a timed snapshot. Effect claims remain conservative across cancellation and cleanup; fetch reads do not claim remote Git mutation. Legacy records require owner binding before activation, and process loss makes no success or rollback claim.

The remaining failure is at the new token-recovery boundary under record saturation. It does not reopen the physical ownership findings.

## Risk and next action

Resolve P2-1 as an interface decision and re-review the corrected tuple. Under [GwzProcessOptimization `4.1](GwzProcessOptimization.md), this finding is a **new architectural root in the final remediation round**, so the two-round cap calls for redesign and a new freeze decision rather than another bounded guard. Design approval would still leave Taut generation, implementation, wire carrier, Python concurrency and release gates separate.

