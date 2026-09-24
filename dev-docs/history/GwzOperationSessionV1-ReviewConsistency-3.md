# GWZ operation-session v1 — Consistency review, round 3

**Review object:** committed `dev-docs/GwzOperationSessionV1Design.md`, `dev-docs/GwzOperationSessionV1CallerGuideDraft.md`, and `gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md`  
**Baseline:** root `f2f10aeeafa0e0443a88f50983435422980de9f3`; core `28eb3d59d62282eba8cb2f376d51ea0521c50190`  
**Reviewed tuple:** root `280cec26df5233993c570ed6452a8cd95ed6730d`; core `10dd625a46163234d55fdb957f445514fb9f3c43`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict: NO-GO.** One P2 finding remains. All four HEADs matched the reviewed tuple at the start and end of the read-only review.

## Prior-finding closure

| Original finding or risk | Disposition and verified counterexample | Status |
| --- | --- | --- |
| Earlier Consistency P2-1: no complete normative v1 base | The consolidated design and guide now declare a single proposed application contract without incorporating the rejected drafts. The remaining status-retention defect below prevents an unqualified recovery contract. | Partly closed |
| Earlier Consistency P3-1: endpoint-owner limit absent from negotiation | Limits field 23 publishes 32 active-plus-orphan endpoint owners; first network use checks that budget before construction, including after a local-only open. | Closed at design level |
| Round-2 Consistency P2-1: session ID collides with the accepted transport attachment | `RequestMeta.transport_message` remains an `Envelope` at tag 10; `session_id` uses tag 11. The design uses accepted `TransportOptions.placement` as the sole per-request placement and refuses an omitted/local placement in a `cli` session before setup. | Closed at design level |
| Round-2 Consistency P2-2: no carrier for capacity and pending-close context | Additive `GwzError` fields 8 and 9 carry `CapacityRefusalV1` and `SessionCloseProgressV1`. Codes 73–76 require the former; `close_pending=80` requires the latter. Missing or conflicting contexts are protocol errors, and the Python mapping is specified. | Closed at design level |
| Round-2 generation-limit risk | Limits field 26 publishes four live/draining generations per session; a fifth receives `CapacityBusy(resource=draining-generation, scope=session)`. | Closed at design level |

## Changed-range analysis and controlling graph

The root changes resolve the prior tag and error-context defects, define the start registry and recovery token, charge admission through request registration, publish the generation limit, and correct the guide’s release and fetch-effect examples. The core amendment extends its pre-accept epoch reservation through request registration and the atomic `Accepted` handoff.

The amendment identifies exact accepted text for replacement. Its quoted retry-plan `3 item 7 three-sentence installation rule, both `6 capacity bullets, S1.4’s first three sentences, and the remote-transport plan’s Phase 2 overlapping-policy sentence match the current accepted plans. Its replacement preserves their remaining scope and the accepted placement design’s tags, enum and attachment meaning. Until this draft tuple receives GO, those accepted predecessors control.

**NEW ARCHITECTURAL ROOT — bounded refusal recovery lacks an admission boundary.** The new start-status guarantee creates retained server state even for rejected submissions, but the design does not reserve or bound that state before a token can be submitted. This is a new root cause in the final remediation round, so the adopted process’s two-round cap calls for redesign and re-freeze rather than another incremental guard.

## Evidence base

I read the three committed review objects, the earlier Consistency finding and merged round-2 dispositions, the accepted transport placement design, the accepted retry and remote-transport plans, the current GWZ Taut schema, and the controlling process documents. The current schema has `RequestMeta` fields 1–9 and `GwzError` fields 1–7; the accepted placement design reserves the transport `Envelope` at request tag 10 and response tag 9. This is a design review; no generated schema, implementation, wire carrier or release evidence is claimed.

## Finding

### P2-1 — Rejected-start status cannot be both universally recoverable and bounded

**Root cause and location:** The design promises that `operation.start_status_v1` returns the original typed refusal or setup error, and that a refused token retains that error for 60 seconds (`GwzOperationSessionV1Design.md:23,49`). It also says settled refused starts occupy a retained slot (`:51`), while defining finite retained-record capacity and saying its 256 receiver slots cover terminal/cleanup records reserved at acceptance (`:41`). No capacity or pre-submit reservation rule is defined for refused-start statuses.

**Violated invariant:** Every submitted token must have one recoverable outcome after a lost reply, while every retained record must be charged to a finite published budget.

**Credible sequence:** Fill the receiver’s retained slots with completed terminals, then submit another token. It must refuse with `RetainedCapacityFull` and retain that exact refusal for status lookup. There is no specified slot in which to do so. A separate finite refusal store only moves the boundary: repeated unsupported or capacity-refused starts fill it within its 60-second TTL. At saturation, storing another refusal breaks the bound, evicting one breaks its promised recovery, and dropping the new refusal makes a lost reply unrecoverable. The Client’s local registry does not recover a receiver refusal whose reply was lost.

**Impact:** Two conforming implementations can make different choices at this boundary, and the documented lost-reply recovery fails precisely under capacity pressure. This is a concrete recovery and compatibility defect.

**Correction:** Define a finite, published start-status budget and reserve a status slot before a token is eligible for server admission. Specify a distinct, provably unaccepted path when that reservation cannot be made, including what `start_status_v1` and a repeated submit return; qualify the guide’s recovery promise accordingly. No handler, constructor or request registration may begin on a token lacking its required recovery reservation.

**Regression test:** Saturate terminal records and then refused-status reservations; lose each refusal reply, query status and repeat the token. Assert bounded memory, one unambiguous accepted-or-unaccepted outcome per eligible token, no duplicate Git work, and the documented behavior at status-budget exhaustion.

## Invariant analysis

The original tag-10 collision and duplicate placement authority are gone. The typed capacity and `ClosePending` payloads have allocated error fields and code-to-context rules. A pre-accept `AdmissionScope` now owns constructor and registration work through acceptance or losing cleanup, and the paired amendment pins the same physical epoch until that ownership drains. The four-generation ceiling is visible and has a typed refusal. The guide releases a completed peer before waiting on a stalled one, distinguishes terminal release from marker expiry, and treats fetch reads as no remote Git mutation.

Those corrections do not solve the status-retention bootstrap: a refusal can itself need retained state when retained capacity is exhausted.

## Risk and next action

Stop this design lane under the adopted two-round remediation cap. Rework the bounded start-status admission and lost-reply contract, then re-freeze a committed tuple for independent review. Design acceptance would still be separate from Taut generation, Python concurrency implementation, transport qualification and release.

