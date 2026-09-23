# GWZ start-ticket design — Consistency review

**Object:** `dev-docs/GwzOperationStartTicketDesign.md`, `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`, `gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md`, and `gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md`  
**Baseline:** root `2ac74b10668e25e63eec83e258e17aeb91cdf91d`; gwz-core `590eefe26a0be6b59fda1f190a94d927b88938bb`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs matched at review start and end.  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict: NO-GO** — three P2 findings; no P0 or P1 findings.

The evidence base was the four review-object documents, the accepted placement design and embedding guide, the accepted retry plan, the current Taut schema and consumer wrapper, the Python concurrency release-gate record, and `AgentProcessRules.md`/`GwzProcessOptimization.md`. The rejected operation-session v1 documents were treated as historical evidence, not authority. This is a design-contract review, not an implementation or release verdict.

## Findings

### P2-1 — The transport request ID crosses an undefined registration boundary

**Location:** Start-ticket design ``2–3, especially [lines 59 and 63](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:59); delivery amendment [``1 and 3](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:7); accepted placement design [`4](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:188).

**Invariant:** A delivery frame’s `request_id` must identify the request already registered under the correct operation and binding before Bind, Open or checks; a frame cannot establish that registration.

**Credible sequence:** The accepted placement contract has the client host register the command’s existing `request_id` before dispatch. The new ticket contract instead assigns each accepted operation a distinct internal transport request ID, separate from `RequestMeta.request_id`, while registration occurs during pre-accept admission. `OperationPrepareV1` and `OperationCommitV1` carry no transport ID or mapping. The delivery amendment still calls its field the “existing request_id” without replacing the accepted host-registration rule. A Python host following the accepted rule registers the caller ID, then receives a delivery frame with the distinct internal ID. It must reject the frame, or abandon the new identity rule.

**Impact:** The frozen documents do not define one interoperable registration and correlation path for a Python-supplied endpoint. Implementations can disagree before the first transport frame, making nonlocal operations fail or weakening stale-frame isolation.

**Correction:** Specify who allocates the internal transport ID, how the bound host learns and registers it before any frame, and its relationship to `RequestMeta.request_id` and the provisional pre-accept operation ID. Explicitly replace the affected placement-design registration sentences.

**Regression test:** Start two operations with distinct caller and transport IDs, including an acceptance race; assert that both directions deliver only to the registered internal ID, and that a stale frame or caller-ID substitution cannot attach to either request.

### P2-2 — The delivery amendment leaves conflicting accepted embedding gates in force

**Location:** Delivery amendment [`4](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:31); accepted placement design [operator clarification](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:14) and [batch C](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:386); embedding guide [“Attachments and results”](/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/TransportPlacement.md:91).

**Invariant:** An accepted amendment must identify which earlier gate it replaces, so implementation and qualification have one required delivery schedule.

**Credible sequence:** The accepted operator clarification and batch C require envelopes to travel inside existing Taut request/response messages. The new amendment requires a delivery-only event when no application message is sent and says a piggyback-only host cannot advertise nonlocal placement. Its authority section explicitly replaces part of placement `4, but merely says to “refine” the operator clarification and `8C; it leaves their contrary test wording and the embedding guide’s attachment description intact.

**Impact:** One implementation can pass the written accepted batch-C attachment test while failing the new idle-delivery requirement; another can implement the new event while failing the unchanged accepted wording. The proposed freeze has two incompatible qualification oracles.

**Correction:** Quote and replace the operator-clarification and batch-C sentences requiring delivery *inside existing request/response messages*. State that tags 10 and 9 remain compatible optional attachments, while `GwzTransportDeliveryV1` on the supplied in-process channel is the required idle and blocked-handler path. Update the embedding guide as part of the same contract tuple.

**Regression test:** With application dispatch blocked and then idle, send frames in both directions using only the delivery-only message; verify ordinary v0 request/response decoding and optional attachment tags remain unchanged.

### P2-3 — A full record budget has two incompatible capacity diagnoses

**Location:** Start-ticket design [capacity context and limits](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:45) and [prepare admission](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:55); caller guide [capacity remedies](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketCallerGuideDraft.md:85).

**Invariant:** A typed capacity refusal must identify a remedy consistent with the resource actually retaining the slot.

**Credible sequence:** Complete operations, release their terminals before cleanup completes, then let cleanup finish. Their cleanup-only markers occupy the retained-record slots until the published marker TTL. If all slots are held this way, `3 says a no-slot prepare returns `RetainedCapacityFull(resource=terminal-record)`. Elsewhere the design and guide assign `resource=cleanup-marker, retry_condition=marker_expiry` to precisely this post-release case. The caller has no terminal left to release, yet receives the terminal-record diagnosis and its release-or-expiry guidance.

**Impact:** The protocol remains safe from duplicate Git work, but its typed recovery advice is false under a reachable saturation state. Clients can retry or attempt cached releases without a route to capacity recovery.

**Correction:** Define the exact refusal resource and retry condition for marker-only and mixed terminal/marker saturation. Make `3, the error-shape rule and the guide use the same decision rule.

**Regression test:** Fill the session and receiver record budgets first with cleanup-only markers, then with mixed retained terminals and markers. Assert that every stateless prepare refusal carries a context whose documented action can free a slot or whose expiry condition is accurate.

## Invariant assessment and next action

The reserved status slot, monotonic `start_seq`, permit expiry and no-slot retry rule form a coherent duplicate-commit boundary as written. The capacity amendment also supplies an explicit charged pre-accept owner and quiescent epoch transition. Terminal publication, physical drain and peer cleanup are kept distinct. Those strengths do not resolve the three contract conflicts above.

Correct the ID registration boundary, replace the precise accepted delivery-gate text, and reconcile the capacity-resource diagnosis. Then review the revised four-document tuple before treating it as an interface freeze. Physical wire, separate-process and release proof remain deferred as stated; the findings concern the required in-process contract.

