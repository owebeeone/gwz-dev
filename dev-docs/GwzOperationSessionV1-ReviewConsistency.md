# GWZ operation-session v1 — Consistency review

**Object:** `dev-docs/GwzOperationSessionV1Design.md`, `dev-docs/GwzOperationSessionV1CallerGuideDraft.md`, and `gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md`  
**Tuple:** gwz-dev `f2f10aeeafa0e0443a88f50983435422980de9f3`; gwz-core `28eb3d59d62282eba8cb2f376d51ea0521c50190`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict: NO-GO.** Two P2 contract findings remain. The exact tuple matched at the start and end of this read-only review.

## Prior-finding closure

| Prior finding | Disposition |
| --- | --- |
| Consistency P2-1 — no complete normative v1 base | **Partly addressed, not closed.** The replacement consolidates methods, lifecycle, results, limits and capacity precedence without incorporating the rejected draft. Its schema nevertheless conflicts with an accepted transport allocation (P2-1 below), and its promised structured refusal/progress outcomes lack a defined message path (P2-2). An implementer still cannot generate the stated contract solely from this tuple and the accepted contracts it preserves. |
| Consistency P3-1 — endpoint-owner limit absent from negotiation | **Closed at design level.** `OperationSessionLimitsV1` field 24 publishes the receiver’s 32 active-plus-orphan endpoint owners. The design and guide place the first-network-use check before construction and specify `CapacityBusy` when local-only open succeeded under full endpoint capacity. Enforcement remains an activation gate. |

## Changed-range analysis

Root `f2f10ae` adds the consolidated specification and full caller guide. Core `28eb3d59` changes the paired amendment so an endpoint constructor reserves and pins the physical capacity epoch **before** endpoint setup, and keeps that reservation until publication or losing-result drain. The amendment quotes and replaces the retry plan’s §3(7), §6 and S1.4 capacity clauses and the remote transport plan’s Phase 2 exit sentence. No reviewed change implements these proposals. The accepted predecessors retain authority until this tuple receives design GO.

## §0 Evidence

I read the three committed review objects, the prior Consistency report and disposition plan, the operation-session lane verdict, the Python concurrency NO-GO and accepted Python transport design, the accepted retry and remote transport plans, the accepted transport placement design, and the current Taut schema, generated candidate and transport registration source. The placement design is explicitly accepted and reserves `RequestMeta.transport_message` at tag 10 (`gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:3,105–116`); the candidate generator encodes and decodes that tag as an `Envelope` (`gwz-core/tests/transport_consumer/candidate/candidate_generated.rs:2767–2805`). Current `GwzError` has only fields 1–7, none for capacity or close-progress context (`gwz-core/protocol/gwz.taut.py:1146–1157`). The current transport session also demonstrates the 256 lifetime request-ID ceiling (`gwz-core/src/transport_host/session.rs:496–510`). These observations establish feasibility questions, not implementation acceptance.

## §1 Findings

### P2-1 — Session ID reuses the accepted transport-envelope tag

**Location:** `dev-docs/GwzOperationSessionV1Design.md:5,11,17,30`; accepted `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md:105–116,173–185`.

The replacement assigns `RequestMeta.session_id?` to tag 10 while saying other accepted transport contracts remain in force. The accepted placement design assigns **that same tag** to `RequestMeta.transport_message`, a shared transport `Envelope`; its candidate generated decoder already treats tag 10 as that type. The replacement also adds a second placement field at tag 11 without defining how it agrees with accepted `TransportOptions.placement`.

A v1 request carrying a session ID at tag 10 is decoded as an envelope by the accepted attachment path, or an attached transport message is decoded as a session ID by a generator following this draft. The in-process message embedding that the draft preserves cannot satisfy both interpretations. Independent placement fields can also select different routes unless their relationship is defined.

**Consequence:** Taut codegen and the accepted transport embedding cannot both pass; owner binding and placement admission lack one unambiguous request shape.

**Correction:** Allocate unused `RequestMeta` tags after the accepted envelope tag, retain its type and omission meaning, and specify the agreement and precedence rule between the new placement field and `TransportOptions.placement`.

**Closure test:** Generate one schema containing both contracts; round-trip a v1 request with session identity and an embedded envelope, reject conflicting placement values before endpoint or helper access, and verify existing attachment decoding remains unchanged.

### P2-2 — Structured refusals and pending-close progress have no defined Taut carrier

**Location:** `dev-docs/GwzOperationSessionV1Design.md:21–30,36,58`; caller guide `:65–72`; current `gwz-core/protocol/gwz.taut.py:1146–1157`.

The specification requires a typed capacity refusal containing `resource`, `scope`, `limit`, `in_use`, `retry_condition` and `retry_after_ms`, and requires `ClosePending` to carry current `SessionCloseProgressV1`. It allocates new error **codes**, but no fields or message binding for either structured payload. Existing `GwzError` carries a code, prose and unrelated member/record context. The method descriptions do not define an alternative result union or error envelope with these fields.

For example, when all 32 endpoint-owner slots are held by orphans, `submit_v1` must refuse before construction and tell the caller the exact resource and retry condition. A generator using the specified method outputs and existing `GwzError` can transmit the code, but must invent an encoding for those required values. Likewise, a five-second close timeout cannot carry the promised charged progress without an invented response/error shape.

**Consequence:** Two implementations can expose incompatible error APIs while each claims to follow the draft; the guide’s recovery instructions are not supported by a complete generated contract.

**Correction:** Define named Taut context messages and field allocations, or explicit method result variants, for capacity refusal and pending close. Specify their binding to each method and Python exception, including omission rules.

**Closure test:** Generate the schema, round-trip each capacity code with all required fields and a `ClosePending` with progress, and demonstrate that a caller can choose the documented retry action without parsing prose.

## §2 Invariant analysis

The principal cleanup attacks failed at the design level. A constructor is charged from reservation, pins the capacity epoch before environment access, and cannot publish after Closing; a losing endpoint remains owned until shutdown. A timed `CleanupReport` or an unsealed zero-pending snapshot does not release physical charges. A stuck handler retains its worker and scope indefinitely and keeps its terminal pending. Route loss transfers existing reservations without acquiring a new supervisor slot. Terminal release leaves active cleanup charged and observable. These rules address the earlier false-finality and orphan-ownership counterexamples without asserting an unsupported termination deadline.

The capacity amendment’s high-capacity A/lower B/higher C sequence is coherent: B can overlap under A’s installed epoch while enforcing its own work limit; C must refuse until scopes, constructors and physical cleanup quiesce. The specified 32 endpoint owners, 128 operation/cleanup scopes, 64 operation workers, 256 member workers and retained byte/record caps show no arithmetic contradiction in the reviewed cases. Legacy reads are explicitly required to be owner-bound and isolated before v1 activation. None of those design claims closes the schema conflicts above.

## §3 Risks and next action

Generation rollover retains an implementation-chosen finite number of draining mux generations and may refuse admission when that bound fills (`GwzOperationSessionV1Design.md:46`). Before activation, choose a bound that cannot become an additional unadvertised capacity ceiling under the published scope limits, or publish its limit and refusal resource. This is an implementation-interface check, not an additional finding here.

Resolve P2-1 and P2-2 in one revised exact tuple, then repeat independent design review. Taut generation, core cleanup tickets, Python concurrency, platform evidence and release remain separate gates; this NO-GO does not change the accepted v0 behavior or authorize activation.
