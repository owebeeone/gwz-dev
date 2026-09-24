# Independent delivery and start tickets — CONSISTENCY-AXIS REVIEW (round 2)

**Review object:** Root `dev-docs/GwzOperationStartTicketDesign.md` and `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`; core `dev-docs/GwzIndependentTransportDeliveryAmendment.md`, `dev-docs/GwzRemoteTransportCapacityAmendment.md`, and `docs/TransportPlacement.md`.  
**Exact tuple:** root `a1f42eaabf690cac1f4ca54804550c482a1d417d`; gwz-core `e06c752dbe4048a7f319b28d8351702490e63e7f`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.  
**Date:** 2026-09-23. **Axis:** Consistency.  
**Verdict: NO-GO** — five P2 findings remain or arise in the revised interface. This is a design-contract verdict, not an implementation, physical-wire, platform, or release verdict.

## Prior-finding closure table

| Original finding | Round-2 disposition and counterexample |
| --- | --- |
| P2-1 — request-ID authority and handoff | **Partially closed.** The internal ID now occupies `RequestMeta.request_id` and the delivery event, and the amendment names the accepted registration clauses it replaces. The host must still register before commit, whereas the charged `AdmissionScope` and its cleanup ticket begin at commit. A permit can expire or commit can refuse after host registration, with no defined owner to retire that registration. See P2-1 below. |
| P2-2 — exact replacement of embedding gate | **Partially closed.** The amendment quotes and replaces the accepted operator sentence and two batch-C sentences. The amended embedding guide nevertheless still says its next integration gate proves attachments *inside existing* request/response messages, alongside the new delivery-only gate. See P2-2. |
| P2-3 — marker-only and mixed capacity context | **Partially closed.** The design now distinguishes marker-only from mixed occupancy. Its marker-only remedy assumes expiry after 60 seconds even when a cleanup-only marker is still active; its 60-second TTL begins only after local cleanup completes. See P2-3. |

## Changed-range analysis

The changed root design adds the internal transport ID, caller-ID field 6, ticket lifecycle, and record-occupancy rule. The core amendment changes the host pump from one ordered FIFO to urgent and ordered queues, specifies the host registration handoff, and quotes the accepted placement clauses being replaced. The embedding guide gained a proposed delivery paragraph and queue note. The capacity amendment itself is unchanged in this round.

**NEW ARCHITECTURAL root cause:** P2-4 introduces a second queue without preserving stream-opening precedence across those queues. It can turn a valid cancellation or failure into a protocol disconnect. P2-5 is a new schema/replay-integrity defect caused by placing caller identity outside the stated duplicate-prepare comparison. P2-1 through P2-3 are incomplete closure of the original findings.

## 0. Evidence base

I read the five review-object files, the prior Consistency report and round-1 disposition, core `GwzRemoteTransportPlacementDesign.md`, the relevant retry/transport plan clauses, root `AgentProcessRules.md` and `GwzProcessOptimization.md`, and the current transport mux routing code to check stream transition order. I compared the changed ranges from the prior reviewed root/core revisions. No build, test, or mutation was performed. Unrelated working-tree changes were present; none of the review-object files was dirty. All four HEADs matched the exact tuple at both the start and end of inspection.

## 1. Findings

### P2-1 — Host registration precedes its specified cleanup owner

**Location:** Start-ticket design [§2, request ID and commit](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:37) and [§3, admission scope](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:59); delivery amendment [§3](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:29); capacity amendment [atomic transition](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md:111).

**Invariant:** Every live host request registration has a charged owner and a defined retirement path, including when no commit succeeds.

**Sequence:** A Client receives a permit, and its host registers the internal ID before sending commit as required by the delivery amendment. Commit is then lost, refused for capacity, or arrives after permit expiry. The start-ticket design creates the `AdmissionScope` and installs its request-local cleanup ticket only when a valid commit enters admission; the capacity amendment likewise charges request registration during that admission. The already-created host registration therefore has no specified owner or retirement transition. Repeating this sequence can consume the binding’s lifetime request-ID budget without accepted operations.

**Impact:** Implementations can leak host registrations, disagree about whether a pre-commit ID is live, or retire it at different times. That undermines the promised request isolation and bounded generation rollover.

**Remedy:** Give pre-commit host registration its own charged ticket and explicit abort/expiry path, or move registration inside the charged commit transition while preserving the requirement that it precede every frame. Specify the handoff to the accepted operation and the behavior for every no-commit/refused-commit branch.

**Closure test:** Register after a permit, then lose, reject, expire, cancel, and race commit with close. In each case prove the host entry is retired or transferred exactly once, no frame can use an abandoned ID, and repeated starts do not exhaust lifetime IDs through abandoned registrations.

### P2-2 — The embedding guide retains the superseded attachment gate

**Location:** Delivery amendment [§4](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:35); embedding guide [“Attachments and results”](/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/TransportPlacement.md:91), especially its [next-integration-gate sentence](/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/TransportPlacement.md:123).

**Invariant:** The proposed tuple must present one testable host-delivery gate after an exact authority change.

**Sequence:** The amendment requires `GwzTransportDeliveryV1` to progress while application traffic is idle and replaces the accepted requirement to carry envelopes inside existing request/response messages. The guide acknowledges that proposal at lines 101–110 but still tells an implementer that the “next integration gate proves transport attachments inside existing CLI/core and gwz-py/core messages.” An attachment-only implementation satisfies that guide sentence while failing the amendment’s idle-delivery requirement.

**Impact:** The consumer-facing gate and controlling amendment still admit different implementations. Passing the guide’s stated gate cannot establish the revised host schedule.

**Remedy:** Replace the guide’s remaining gate sentence with the delivery-only event oracle. Keep optional attachment tags as separate compatibility fixtures, with no claim that they prove delivery progress.

**Closure test:** Trace every current guide gate to the amendment, then test bidirectional event delivery with application dispatch blocked and idle while old optional attachment decoding remains compatible.

### P2-3 — Marker-only saturation promises expiry while cleanup may remain live

**Location:** Start-ticket design [capacity diagnosis](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:49) and [record/marker limits](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:51); caller guide [capacity remedies](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketCallerGuideDraft.md:110).

**Invariant:** A typed capacity refusal’s retry condition must describe a reachable way for its occupied slot to become free.

**Sequence:** Complete and release terminals while their physical cleanup remains pending. The retained-record slots now contain only cleanup records or cleanup-only markers; releasing a terminal does not free the active cleanup charge. Once the slots fill, prepare reports `resource=cleanup-marker, retry_condition=marker_expiry`. The design and guide say that only their 60-second expiry frees a slot. Yet the stated marker TTL is 60 seconds **after local completion**, and the design expressly allows physical cleanup to remain pending indefinitely.

**Impact:** A caller receives a false time-based recovery diagnosis for an indefinitely active owner. Backoff or waiting 60 seconds cannot make that slot available.

**Remedy:** Distinguish active cleanup records from final cleanup markers in the occupancy decision and typed retry condition. Use `owned_cleanup` or `external_cleanup` for active records; reserve `marker_expiry` for records whose final-local-completion TTL has begun. State the mixed-occupancy rule for both active and final markers.

**Closure test:** Saturate slots separately with active cleanup-only records, final markers, and mixed terminal/marker records. Assert each refusal’s resource and retry condition matches the transition that can actually free a slot, including a cleanup task that never finishes.

### P2-4 — Urgent delivery may overtake the stream transition that makes it legal

**Location:** Delivery amendment [queue selection](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:19); accepted placement [stream-context rule](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:206); current transport mux [stream creation and transition validation](/Users/owebeeone/limbo/gwz-dev/gwz-transport/src/mux/routing.rs:34).

**Invariant:** Urgent control may bypass blocked Data only after the peer has received the Open/CheckIdentity and, for stream-only kinds, Opened transition that establishes its stream context.

**Sequence:** An ordered pump holds an undelivered Open while an urgent Cancel for that stream becomes available. The amendment forbids overtaking Bind/registration but does not forbid overtaking Open. The peer receives Cancel for an unknown stream and fails the live session. Similarly, an endpoint can queue Opened in the ordered lane and then Failed in the urgent lane; Failed can reach the initiator while its route is still `Opening`, where Failed is an invalid transition. The current mux treats such protocol errors as binding failure, affecting healthy peer operations too.

**Impact:** A valid cancellation or failure can disconnect the shared binding and disrupt unrelated operations. The new queue design does not yet satisfy its own peer-isolation and control-progress claims.

**Remedy:** Define per-stream barriers across the two lanes. An urgent item must wait until the peer has admitted the stream-opening transition it depends on, without waiting behind unrelated Data. State how Cancel/Failed discards same-stream unsent Data while preserving Open/Opened and how failure of a blocked opening fails the binding without sending an invalid orphan control frame.

**Closure test:** Pause delivery immediately before Open, CheckIdentity, and Opened; issue Cancel, Window, and Failed where legal. Verify peer transition order, no unexpected protocol disconnect, and progress despite blocked Data on another stream.

### P2-5 — Duplicate prepare compares request bytes but excludes caller identity

**Location:** Start-ticket design [prepare fields and replay rule](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:37) and [terminal caller ID](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzOperationStartTicketDesign.md:43).

**Invariant:** A live permit’s identity and terminal correlation fields must be immutable across idempotent prepare replay.

**Sequence:** A prepare for one session and `start_seq` carries canonical action request bytes plus caller correlation ID `A` in separate field 6. Its reply is lost. A retry uses the same action bytes and sequence but caller ID `B`. The contract says the same owner/sequence with the same **request bytes** returns the same permit and only “differing bytes” refuse. Both replays therefore qualify for the same permit, although the terminal can contain only one caller ID and duplicate-live-caller-ID validation depends on that field.

**Impact:** Two conforming receivers can choose different terminal correlation IDs or duplicate-ID outcomes for the same accepted operation. This is a diagnosability and replay-consistency defect in the proposed schema.

**Remedy:** Bind the permit to the complete canonical prepare tuple, including field 6, method name, request-message name, and action bytes. A replay with any changed field must return `InvalidRequest`; an identical replay returns the original permit and caller identity.

**Closure test:** Lose the first prepare reply, replay an identical message, then change only caller ID, method, or request-message name. Assert only the identical replay receives the original permit and all later terminal/status correlation remains unchanged.

## 2. Invariant analysis

The revised tuple now has a clear internal transport-ID value and names the accepted placement clauses it intends to supersede. Its sequence watermark and retained permit prevent a known old sequence from starting Git work twice. The capacity amendment’s physical epoch remains internally coherent: a charged pre-accept admission pins the installed capacity through accepted operation or losing cleanup.

The remaining breaks occur at boundaries that those rules do not cover: host registration happens before the charged admission; the guide still carries a contrary integration oracle; active cleanup is described as an expiring marker; urgent transport queues omit stream-opening precedence; and the idempotent prepare key excludes a field used in terminal identity. These are interface-shape defects within the requested in-process design scope.

## 3. Risks and next action

Keep this design tuple at **NO-GO** while the five P2 findings remain. Correct the registration owner, guide gate, active-marker diagnosis, cross-queue stream barriers, and full prepare replay key in one settled revision. The next Consistency review should trace those transitions against the exact amended placement clauses and the stated closure cases. Implementation, physical wire, separate-process, iroh, platform, and version-1 release proof remain deferred.

**End-of-review tuple check:** root `a1f42eaabf690cac1f4ca54804550c482a1d417d`; gwz-core `e06c752dbe4048a7f319b28d8351702490e63e7f`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All match the starting tuple.
