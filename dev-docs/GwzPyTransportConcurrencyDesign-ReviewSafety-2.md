# Python concurrent transport session — Safety-axis review, round 2

**Review object:** `dev-docs/GwzPyTransportConcurrencyDesign-1.md`, committed DRAFT at root `e4b035c4d273eed5e76a6540a6841a54863269f1`, with candidate core and Python document amendments.  
**Baseline:** root `e4b035c4d273eed5e76a6540a6841a54863269f1`; gwz-core `1bd5e5c5ce2ac647ea1a144b82badad4769efe0c`; gwz-py `5541b851266da7267f49531b98c061af1fd670e1`. Prior reviewed tuple: root `c4883f4331b983aa2920808e3798f7c94e2295db`; core `fcbb45f7fa1a797b1386bf0187ff6a60952bd193`; gwz-py `124e50030afb6f7c0e8edd90c37838b0136981c2`. Committed sources were checked with `git show HEAD:` and the prior-to-current diffs.  
**Date:** 2026-09-24.  
**Axis:** Safety — degraded paths, irreversible effects, disclosure, stuck states, interleavings and blast radius. Independent, adversarial and read-only; nothing here relies on another current-round report.

**Verdict: NO-GO** — two new P2 findings block. The prior findings close as specified, but the corrected lifecycle can still hide the outcome of an admitted push and retain records whose IDs callers never received.

---

## Prior-finding closure table

The statuses below re-trace the original counterexamples against this tuple. They verify the revised *document contract*, not an implementation.

| Original ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| Consistency P2-1 | Replace the retry plan’s no-non-idle-lease installation condition for shared Python endpoints. | With A live between leases, different-capacity B is refused by design §3 and retry-plan candidate lines 465–467; equal-capacity B can join without installation. | Closed |
| Consistency P2-2 | Limit record lookups to the open Client lifetime and retain repeated close reporting. | After A completes, close makes A’s lookups invalid; design lines 9, 45 and 47, Python design lines 283–286, and guide lines 49–51 now agree. This contractual closure exposes new P2-1 below. | Closed |
| Consistency P2-3 | Remove idempotent release. | A second release of an issued A yields `OperationExpired`; foreign and never-issued IDs yield `InvalidRequest` in design lines 41 and 47 and guide line 35. | Closed |
| Consistency P2-4 | Issue contiguous public serials only after successful admission. | Failed placement, registration and spawn use no public serial; the high-water mark distinguishes expired issued IDs from never-issued IDs across rollover (design lines 15, 35 and 41). | Closed |
| Safety P2-1 | Reject caller request-ID reuse in the same core generation. | Sequential reuse after A completes receives typed pre-`Accepted` `InvalidRequest`; later-generation reuse is separated by a fresh Port, as design lines 15 and 41 and guide line 35 specify. | Closed |
| Safety P2-2 | Reserve the full lossless record allowance before effects. | Near-full aggregate retention refuses a new push with `TransportSessionFull` before acceptance; only that operation’s own 2/8 MiB overflow can produce `TransportRecordLimit(effect="possible")` (design lines 35 and 45). | Closed |
| Surface S1 | Define cancellation completion and possible effect. | `cancel()` joins terminal publication; `result()` returns a completion that won the race or a retained `Cancelled` result with `effect="possible"` (design line 47; guide lines 21–25 and 35). | Closed |
| Surface S2 | Give unary record-limit failures attributable evidence and reconciliation. | The exception has `response`, IDs, available facts and `effect="possible"`; guide line 49 directs remote-ref inspection before replay. | Closed |
| Surface S3 | Specify cross-loop use. | Guide line 53 states direct method use across loops and gives a two-thread start/cancel/result/close sequence. | Closed |
| Surface S4 | State capacity and retry settings at the call site. | Guide lines 37–45 identify argument locations, defaults, derived pool fields and retry meaning. | Closed |
| Surface S5 | Specify event replay and result independence. | Guide line 35 makes iterators replayable and independent of `result()` and requires a terminal event before iterator end. | Closed |

## Changed-range analysis

The root diff changes the supersession boundary, core request-ID reuse rule, capacity transition condition, admission and serial issuance, high-water classification, full record reservation, cancellation outcome, close lookup lifetime, and cross-loop handle contract (`dev-docs/GwzPyTransportConcurrencyDesign-1.md` lines 9–53). The core retry-plan diff replaces both the no-lease installation condition and the blanket held-lease refusal for shared Python endpoints (`gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md` lines 465–467). The Python diffs align the historical design’s Closing rule and revise the caller guide’s examples, limits, errors and thread guidance.

**New architectural root cause P2-1:** The revised post-Closing rule removes access to per-operation terminal evidence precisely when close itself cancels live operations. This is a lifecycle and outcome-ownership defect, not a restatement of the prior retention-wording conflict.

**New architectural root cause P2-2:** Admission now reserves bounded retained records, but cancellation during the accepted-start handoff can complete an operation without delivering its public ID. The retention ledger has no caller-reachable release path for such records.

## 0. Evidence base

I read the root design through line 61; the three filed round-1 reports and `dev-docs/GwzPyTransportConcurrencyDesign-RemPlan-2.md`; the changed core retry plan, Python transport design and concurrent-operations guide; and relevant core transport, v1.1 and accepted sequenced-stream clauses. I checked the current core request registration, lifetime-used set, request finish and drop behavior in `gwz-core/src/transport_host/{mod,request,session}.rs` to test the request-ID and cleanup assumptions. Process authority was `dev-docs/AgentProcessRules.md` as amended by `dev-docs/GwzProcessOptimization.md`, with the review-loop report template.

Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, prior-to-current `git diff`, `rg`, `sed` and `nl`. I ran no builds or tests and made no writes or Git mutations. The exact three-repository tuple matched at both the start and final recheck. Implementation acceptance, profile-3 host proof, platforms and publication were outside scope.

## 1. Findings

### [P2-1] Close hides the terminal outcome of operations it cancels

**Location and invariant:** Root design lines 45–47 and 53; Python guide lines 35, 49 and 51. An admitted mutating operation with a possible remote effect must leave an attributable outcome or conservative effect warning accessible to its caller. The design promises a `Cancelled` `OperationResult` with `effect="possible"`, but Closing refuses every result, event and cancel lookup while close itself cancels live operations. The retained close report describes physical cleanup, not each operation’s result or effect.

**Counterexample:** Start a push and receive its handle. The remote accepts the update while the local operation is still finishing. An exception exits `async with Client(...)`; `__aexit__` enters Closing and cancels the push. Its terminal record is published after cleanup, but `handle.result()` and `events()` now refuse `InvalidRequest`. A repeated close returns only `TransportCleanup`. The caller cannot obtain the promised per-operation `Cancelled` result, request-attributed facts or `effect="possible"` warning, and may replay a push whose effect already occurred. “Inspect before exit” cannot cover an operation still live when exception-driven exit starts.

**Impact:** Automatic shutdown can permanently remove the only designed attribution channel for a possible remote mutation. Concurrent close widens that loss to every active operation on the Client.

**Required correction:** Preserve a bounded, caller-accessible per-operation terminal outcome through close, or include equivalent per-operation identity, result and effect facts in a retained close outcome. Keep new admission refused and retain the existing finite cleanup deadline.

**Closure test:** Start a push, arrange remote acceptance before local completion, then make `__aexit__` or `close()` race its finish. After close, obtain its identity, terminal outcome and conservative effect classification without reopening the old host; verify the same for two overlapping operations and repeated close.

### [P2-2] Cancellation before handle delivery strands retained records

**Location and invariant:** Root design lines 35, 45 and 53; Python guide lines 35, 47 and 49. Every retained record that consumes the completed-record budget needs a reachable release path or an explicit shorter ownership rule when no handle was delivered.

**Counterexample:** Repeatedly cancel `start_push` tasks in the interval after `Accepted` and public serial issuance but before their handles return. The prescribed path cancels and joins each operation, then propagates `CancelledError`; it delivers no operation ID. Each completed record remains retained until its 15-minute expiry, and `Client.release_operation` requires the ID. After 64 such small terminal records, the completed-record limit makes otherwise valid new operations fail `TransportSessionFull` for up to 15 minutes. The session nonce prevents a caller with no other handle from reconstructing the missing IDs. The same ownership gap can affect a cancelled unary call.

**Impact:** A normal cancellation race can make a still-open Client unavailable with no targeted release action. For a mutating operation, its retained terminal evidence also cannot be inspected by the caller.

**Required correction:** Define the accepted-but-undelivered ownership transfer. Either return attributable identity and terminal evidence with cancellation through a supported channel, or release an unobservable completed record under an explicit rule that still conveys possible remote effect. Ensure repeated cancellations cannot fill retention with unreachable records.

**Closure test:** Force cancellation at the admission gate after public serial allocation and before handle delivery, for a push with a possible effect. Verify the caller can obtain its identity/effect evidence or that the record is safely retired; repeat beyond 64 cancellations and show a subsequent valid operation admits without waiting for expiry.

## 2. Invariant analysis

The corrected request-ID rule now matches core’s generation-lifetime `used` set; a fresh public operation ID does not pretend to make a reused core request ID legal. The contiguous serial and high-water scheme avoids an unbounded issued-ID tombstone set. A held lease and a live operation between leases both prevent different-capacity installation, while exact equal capacity does not resize a shared pool. Full per-operation reservation prevents unrelated retained records from converting a within-limit successful push into a post-effect record-limit failure. Explicit Python CLI placement is refused before credential or mutation work.

Those attacks did not produce another finding. Endpoint-owned secrets remain excluded from Python events and errors in the stated contract. The accepted sequenced-stream design’s delivery tickets, generation pinning and separate host proof gate remain required; this review makes no implementation or activation claim.

## 3. Risks and next action

Keep the design gate **NO-GO**. Correct the close-outcome and accepted-but-undelivered ownership paths in the root design and caller contract, then review the new pinned tuple against both counterexamples. The final tuple recheck remained root `e4b035c4d273eed5e76a6540a6841a54863269f1`, gwz-core `1bd5e5c5ce2ac647ea1a144b82badad4769efe0c`, gwz-py `5541b851266da7267f49531b98c061af1fd670e1`.
