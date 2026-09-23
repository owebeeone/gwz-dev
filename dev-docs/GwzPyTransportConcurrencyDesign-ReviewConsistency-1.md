# Python concurrent transport session — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT `dev-docs/GwzPyTransportConcurrencyDesign-1.md` and its candidate core-plan and Python caller amendments, dated 2026-09-24.  
**Baseline:** root `c4883f4331b983aa2920808e3798f7c94e2295db`; gwz-core `fcbb45f7fa1a797b1386bf0187ff6a60952bd193`; gwz-py `124e50030afb6f7c0e8edd90c37838b0136981c2`. All cited repository text was read with `git show HEAD:<path>`. `git rev-parse HEAD` matched this tuple at the start and end.  
**Date:** 2026-09-24.  
**Axis:** Consistency—internal agreement, controlling text, exact supersessions and satisfiability of the stated contract. Independent, adversarial and read-only; nothing here relies on the parallel reviewer.

**Verdict: NO-GO** — four P2 findings block. I pre-commit to GO on a revision that resolves P2-1 through P2-4 as specified below, provided it introduces no new blocking defect.

---

## 0. Evidence base

I inspected the root design at lines 7–61; the merged round-1 remediation plan and prior Consistency, Safety and Surface reports; the candidate amendments in `gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md` lines 465–469, `GwzRemoteTransportPlan.md` lines 444–452 and `GwzV110Plan.md`; and the Python design and caller guide in `gwz-py/dev-docs/`. I checked the accepted sequenced-stream design and requirements for the deferred profile-3 boundary. Process authority was `dev-docs/AgentProcessRules.md` as amended by `GwzProcessOptimization.md`, plus the review-loop skill and its report template.

Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, `git show --format= --unified=5 HEAD -- <path>`, `rg`, `sed` and `nl`. I ran no tests, builds, writes or Git mutations. Working-tree material, implementation acceptance, profile-3 host proof and publication evidence were outside this review.

## 1. Findings

### [P2-1] The capacity amendment leaves a contrary no-lease installation rule in force

**Location:** Root design lines 9, 25 and 29; retry plan §6 lines 325–339, S1.4 lines 414–416, and candidate amendment lines 465–469.

**Violated invariant and counterexample:** A later request requiring different physical capacity must be refused while another operation owns the endpoint, even if that operation temporarily holds no non-idle lease. The root design says so. The retry plan still directs installation of an operation’s resolved limits whenever the pool has **no non-idle lease**. Its candidate amendment identifies only the blanket *non-idle* refusal as the narrow replacement. Start A, leave it live between leases, then start B with a different capacity. The retained §6/S1.4 condition directs B to install; the root design directs B to receive `TransportCapacityConflict`.

**Impact:** A conforming implementation cannot satisfy both admission rules. B could change shared capacity while A remains live, or be refused by code that violates the retained retry-plan step.

**Remedy:** Explicitly replace §6 and S1.4’s no-non-idle-lease installation condition for a shared Python endpoint. State the complete condition: a different-capacity transition requires no live operation, no non-idle lease and completed physical cleanup.

**Closure test:** Cross-check the amended sentences together; hold A live with zero leases and assert that different-capacity B is refused before installation or effects, then succeeds after A and cleanup retire. Retain the held-lease equal-capacity case.

### [P2-2] The promised retention period conflicts with the retained post-close refusal

**Location:** Root design lines 9, 45, 47 and 53; Python caller guide lines 31 and 35–37; accepted Python design lines 66–83 and its candidate annotation lines 274–282.

**Violated invariant and counterexample:** The new design promises that a terminal result remains inspectable for 15 minutes unless released, and that a returned handle remains usable through that retention period. Its supersession list does not replace the accepted Python design’s rule that **new calls through the bridge after Closing begins** return `InvalidRequest`, including calls that are neither construction nor admission. Complete A, retain its handle, call `Client.close()` one minute later, then call `A.result()` or inspect its events. The new retention promise requires access; the retained lifecycle rule refuses the call.

**Impact:** The guide advertises recovery of a completed result that the controlling lifecycle can make inaccessible well before expiry. This matters most when close races result inspection.

**Remedy:** Decide whether session-owned result, event and cancellation lookups remain legal after Closing and Closed. Amend the exact older post-close clause and specify which methods survive close, or narrow the retention and handle promises in both the design and guide.

**Closure test:** Complete A, close its Client before the 15-minute deadline, and verify every promised retained lookup and its expiry behavior. Separately verify that new network and local work remains refused.

### [P2-3] `release()` is promised as idempotent but has the expired-ID outcome

**Location:** Root design lines 17, 45, 47 and 53; Python caller guide line 31.

**Violated invariant and counterexample:** The caller guide says `OperationHandle.release()` is idempotent. The root design includes release among calls that validate and look up the session record, says explicit release expires that completed record, and assigns an expired same-session serial `OperationExpired`. Call `await handle.release()` twice. After the first call, the record has expired; the second call has no specified successful repeat path and follows the expired-ID rule.

**Impact:** A caller retrying release after an uncertain response observes an error despite the published idempotency promise.

**Remedy:** Specify a bounded release tombstone or another repeat-release rule that returns success for an already released issued ID, while retaining `InvalidRequest` for foreign or never-issued IDs; alternatively remove the idempotency promise and state the second-call result consistently.

**Closure test:** Release A twice, release A again after unrelated B completes, and compare those outcomes with an expired-but-unreleased A and a foreign ID.

### [P2-4] Exact issued-ID classification lacks a bounded lifetime representation

**Location:** Root design lines 15–17, 33–35, 41 and 45–47.

**Violated invariant and counterexample:** The design assigns a serial to **every submitted operation**, yet says pre-acceptance refusal publishes no accepted operation, completed records expire, and a lookup must distinguish an issued-but-expired serial from a never-issued serial. An accepted A can expose the session nonce and serial 1. Repeated refused submissions consume serials under the stated assignment rule; a later accepted B exposes a higher serial. A caller can construct an intervening same-nonce ID. Without a retained issued-serial record, the session cannot distinguish that gap from an expired accepted operation. Retaining each gap or issued serial indefinitely conflicts with the finite session-record model for a long-lived Client. Rollover does not solve it because public serials never reset.

**Impact:** The advertised `InvalidRequest` versus `OperationExpired` distinction can become wrong, or lifetime identity metadata can grow without the stated bound.

**Remedy:** Freeze the issuance point and representation. For example, allocate contiguous public serials only when acceptance becomes irrevocable, with a separate internal admission token before then; account for registration and worker-start failures without creating public serial gaps. Any alternative must state its finite lifetime charge and how it proves issuance after record expiry.

**Closure test:** Alternate accepted operations with placement, capacity, full-session, registration and worker-start refusals across more than 256 operations and a runtime rollover. After expiry, query accepted IDs and every intervening never-issued serial; assert the distinct typed outcomes and a fixed identity-memory bound.

## 2. Invariant analysis

The correction does resolve several prior textual defects: it separates the public operation ID from caller `request_id`, puts results under the native session, distinguishes pool checkout capacity from top-level worker limits, gives capacity transitions an explicit leader, defines a quiescent rollover, and refuses Python CLI placement before endpoint work. The core Phase 2 amendment explicitly withdraws its old different-physical-limit overlap row. Those statements agree with the corresponding revised design clauses.

The remaining conflicts sit at boundaries between clauses. Capacity transition legality is stricter than the old retry plan’s no-lease condition; result retention extends beyond the old bridge lifecycle; and expiry semantics interact with the new release and identity promises. The accepted profile-3 design’s ticket and generation obligations remain a separate activation gate, as the root design states. I draw no implementation or host-proof conclusion from this document review.

## 3. Risks and next action

Keep the design gate **NO-GO**. Make one bounded document correction that resolves the four findings across the root design, exact retry-plan clauses and Python caller guide, then re-review the corrected committed tuple. The tuple remained unchanged through the final `git rev-parse HEAD` check.
