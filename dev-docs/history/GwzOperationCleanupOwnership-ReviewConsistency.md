# Operation cleanup ownership — Consistency review

**Object:** `dev-docs/GwzOperationCleanupOwnershipDesign.md` and `dev-docs/GwzOperationCleanupCallerGuideDraft.md`  
**Tuple:** gwz-dev `b78f8c2b9db4929a86d03c8642c568ae8864baef`; gwz-core `d3951dcfc04d2f09c9c7a025dec41ee3163c4058`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict: NO-GO** — one P2 contract-boundary finding. The exact tuple matched at the start and end of this read-only review.

## 0. Evidence base

I read the two committed review objects, the prior operation-session NO-GO verdict, the rejected operation-session design and caller guide, the accepted Python transport design and concurrency NO-GO, the draft core capacity amendment, and the committed transport-host request, session, and cleanup paths. The current host can return a cleanup report with physical work pending: `Session::cleanup()` ends at its deadline, and `TransportRequest::finish()` consumes its request and aggregates cleanup snapshots. The new design correctly treats those snapshots as insufficient proof of finality and requires a retained cleanup ticket before activation. No implementation or platform claim is inferred from this design review.

## 1. Findings

### P2-1 — The replacement has no complete normative version-1 base

**Location:** `dev-docs/GwzOperationCleanupOwnershipDesign.md:5`, `:40`, `:50`, `:61`.

The redesign says its lifecycle rules replace selected claims in the **rejected** operation-session draft. It then depends on that draft for the “existing action terminal/result schema,” version-1 methods, owner binding, negotiated fields, response encoding, result budgets, event retention, placement behavior, and capacity supersession. The paired core capacity amendment is itself still marked draft and says accepted plans remain authoritative until it receives GO. The new caller guide explicitly replaces only the cancellation and close portion of the rejected guide.

**Counterexample:** An implementer takes a GO on this new object as authority to implement version 1. They must choose whether to implement the rejected draft’s `OperationTerminalV1` fields, version-0 owner hardening, and 23-field limits unchanged, or treat them as unapproved. Either choice lacks an explicit reviewed contract. The cleanup redesign cannot stand alone as the version-1 implementation baseline.

**Impact:** A GO would certify cleanup ownership while leaving the protocol and public API it attaches to without an accepted normative definition. Reviewers also cannot determine the exact supersession set or whether the capacity amendment becomes authoritative.

**Correction:** Publish one consolidated version-1 specification, or explicitly incorporate the unaffected sections of the rejected draft and the paired core amendment into this review object by exact section and precedence. Mark which old clauses are discarded, which are adopted, and which remain draft. Review that resulting tuple as a whole.

**Closure test:** Starting solely from the accepted documents, independently enumerate every version-1 method, message field, owner check, result shape, resource limit, capacity rule, and lifecycle transition without relying on an unaccepted document to decide behavior.

### P3-1 — Newly enforced endpoint-owner capacity is absent from the advertised limits

**Location:** `dev-docs/GwzOperationCleanupOwnershipDesign.md:40`, `:50–52`; rejected `GwzOperationSessionProtocolDesign.md:175–185`.

The redesign adds a receiver ceiling of 32 active-plus-orphan endpoint owners. Its proposed `operation_session.open` still returns the earlier `OperationSessionLimitsV1` shape, whose 23 fields include session and operation limits but no endpoint-owner limit. Orphans can occupy all endpoint-owner slots while a local-only session opens successfully.

**Impact:** The receiver can refuse that session’s first network operation with `CapacityBusy` for a limit absent from its published limits. The refusal is truthful, so this does not block the ownership model, but the advertised budget is incomplete.

**Correction and closure test:** Add the endpoint-owner ceiling to the negotiated limits or state expressly that it is a receiver-internal admission ceiling reported only through `CapacityBusy`. Test a successful local-only open with all 32 endpoint-owner slots held by orphans, followed by a typed refusal before endpoint construction.

## 2. Invariant analysis

The principal prior failure is addressed at the design level: a five-second return no longer implies physical drain, a sealed-and-drained transition is required, and route loss transfers ownership with existing charges rather than freeing them. The design also allows a network handler to remain charged indefinitely instead of claiming an unsupported 35-second bound. Those are coherent choices.

I found no arithmetic contradiction in the stated 32 endpoint-owner, 128 operation-scope, 64 operation-worker, and 256 member-worker ceilings: an orphan retains its existing reservation, and transfer need not acquire another slot. The design distinguishes logical session retirement from physical epoch release. It explicitly gates request-local cleanup isolation before overlapping admission, so I do not count current `TransportRequest.finish()` behavior as an unacknowledged implementation proof.

## 3. Risks and next action

Resolve P2-1 before treating this as an accepted version-1 design. The next review should examine a complete normative protocol tuple, including the capacity amendment, while keeping implementation and release gates separate. P3-1 can be corrected in that consolidation.
