# GWZ operation cleanup ownership — Safety review

**Object:** [GwzOperationCleanupOwnershipDesign.md](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupOwnershipDesign.md) and [GwzOperationCleanupCallerGuideDraft.md](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupCallerGuideDraft.md)  
**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **NO-GO — one P2 finding**

| Repository | Reviewed commit |
| --- | --- |
| gwz-dev | `b78f8c2b9db4929a86d03c8642c568ae8864baef` |
| gwz-core | `d3951dcfc04d2f09c9c7a025dec41ee3163c4058` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

All four commits matched at the start and end of this read-only review.

## 0. Evidence base

I reviewed the committed redesign and caller guide, the stopped operation-session verdict, the accepted Python transport design, the draft core capacity amendment, and relevant committed core and Python cleanup code. I made no edits and ran no builds or tests. The finding concerns the proposed contract; it does not claim the unpublished version-1 implementation exists.

## 1. Finding

### [P2-1] Endpoint construction is not explicitly owned across close or route loss

**Location and root cause:** The redesign reserves an endpoint-owner slot before endpoint construction or environment/helper access ([design, line 50](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupOwnershipDesign.md:50)). Its atomic orphan transfer names operation workers, request cleanup tickets and the endpoint owner ([line 32](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupOwnershipDesign.md:32)); its final-close predicate names handlers, operation cleanup and endpoint cleanup ([line 44](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupOwnershipDesign.md:44)). Neither rule explicitly owns and joins an **in-flight constructor before an endpoint exists**. This matters because the accepted [Python transport design](/Users/owebeeone/limbo/gwz-dev/gwz-py/dev-docs/GwzPyTransportDesign.md:66) expressly requires close to join construction and shut down a losing constructor result. The current candidate has a separate `constructing` state and waits for it in [transport_session.rs](/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/transport_session.rs:446); the replacement contract must preserve that protection.

**Counterexample:** Reserve the endpoint slot and start constructing the first network endpoint. Before construction publishes an endpoint or accepts an operation, lose the route or call `close()`. If transfer includes only published endpoint and operation owners, close can find neither, return a final report and release the reservation. The constructor may then finish, access endpoint configuration, or publish a host after that purported final close. Retaining the reservation without retaining a constructor owner instead strands capacity with no task responsible for its retirement.

**Impact:** Final close could falsely assert local drain, or an endpoint could escape ownership after route loss. Both violate the redesign’s central rule that every physical resource remains owned and charged until it actually ends.

**Required correction:** Define endpoint construction as a charged, transferable session task from the instant its reservation is taken. Route loss and last-owner drop must transfer its join handle to the supervisor. Explicit close must wait for construction to finish or return `ClosePending`; a constructor that loses the close race must shut down its result before final close. Construction failure must release its reservation exactly once. State whether admission may occur only after publication and rechecking the closing state.

**Closure test:** Block construction after reservation but before publication. Race explicit close and route loss against it. Verify no final close, capacity release, helper access after final close, or late endpoint publication; then unblock construction and verify one losing-result shutdown and one charge release. Repeat with construction failure.

## 2. Invariant analysis

The redesign **does close the previous principal safety gaps at the contract level**: a five-second report is not physical completion; a stuck native handler remains owned and charged without a false 35-second promise; a zero snapshot before producer sealing is insufficient; and cancellation of one operation does not require closing healthy peers. The bounded orphan and endpoint-owner ceilings make permanent saturation an explicit, typed availability outcome rather than an accounting escape. Local drain is correctly separated from uncertain remote effects.

I did not find a second blocking counterexample in terminal publication, operation-local cancellation, request cleanup transfer, or route-bound lookup. The [caller guide](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationCleanupCallerGuideDraft.md:29) allows waiting for cleanup after terminal release; implementation should retain the fixed-size final cleanup report long enough for that promised observation, but the design’s bounded audit-retention rule permits this and does not itself force an unsafe outcome.

## 3. Risks and next action

Close the pre-admission construction ownership gap in the design and test it alongside blocked handlers and delayed disposal. The old operation-session draft remains rejected; this review does not authorize schema changes, implementation or release.
