# Python concurrent transport session — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT `dev-docs/GwzPyTransportConcurrencyDesign-1.md` at root `e4b035c4d273eed5e76a6540a6841a54863269f1`, with candidate core and Python document amendments at the tuple below.  
**Baseline:** root `e4b035c4d273eed5e76a6540a6841a54863269f1`; gwz-core `1bd5e5c5ce2ac647ea1a144b82badad4769efe0c`; gwz-py `5541b851266da7267f49531b98c061af1fd670e1`. Prior reviewed tuple: root `c4883f4331b983aa2920808e3798f7c94e2295db`, core `fcbb45f7fa1a797b1386bf0187ff6a60952bd193`, Python `124e50030afb6f7c0e8edd90c37838b0136981c2`. Committed sources were inspected with `git show HEAD:` and numbered read-only inspection. All three heads matched the stated tuple at the start and end.  
**Date:** 2026-09-24.  
**Axis:** Consistency — internal agreement, controlling contracts, exact supersession, and satisfiable evidence. Independent, adversarial, read-only. Nothing here relies on another current-round report.

**Verdict: NO-GO** — two P2 findings block design acceptance.

---

## Prior-finding closure table

| Original ID | Disposition claimed | Original counterexample re-traced on corrected tuple | Status |
| --- | --- | --- | --- |
| Consistency P2-1 | Replace both the retry plan’s non-idle refusal and its no-non-idle-lease installation condition for the shared Python endpoint. | A live between leases now blocks B’s different capacity; equal capacity joins without installation. The retry-plan candidate amendment says both explicitly. | Closed in the design text. |
| Consistency P2-2 | End result and handle access at Closing; retain repeat close access. | Complete A, close within 15 minutes, then inspect A: the revised design and guide now both require refusal. Repeat close returns its retained report. | Closed for new lookups; the separate active-iterator contradiction is P2-2 below. |
| Consistency P2-3 | Second release of an issued ID returns `OperationExpired`. | Release A twice, after B and after expiry: the high-water rule classifies A as issued and expired; a foreign ID remains `InvalidRequest`. The guide agrees. | Closed. |
| Consistency P2-4 | Issue contiguous public serials only at irrevocable acceptance. | Failed placement, registration, or gated worker spawn consumes no serial. Accepted A and B remain consecutive across such failures and generation rollover; one high-water integer distinguishes their later expiry from never-issued values. | Closed in the design text. |
| Safety P2-1 | Reject reuse of a caller request ID within its current core generation. | A second sequential `request_id="batch-a"` is refused before `Accepted`; successful core registration consumes that ID even if worker launch later fails. A new generation may reuse it under a distinct Port. | Closed in the design text. |
| Safety P2-2 | Reserve the full 8 MiB allowance before acceptance. | Near-full aggregate retention refuses the push as `TransportSessionFull` before effects; after release it can admit. Only that operation’s own overflow becomes `TransportRecordLimit`. | Closed in the resource accounting text; terminal encoding remains blocked by P2-1 below. |
| Surface S1 | Define cancellation completion, result, effect, and terminal event. | The guide now makes `cancel()` join publication and gives a possible-effect failed result. Its example inspects that result before release. | Caller sequence specified; typed result encoding remains blocked by P2-1 below. |
| Surface S2 | Give unary record overflow an attributed result and reconciliation path. | The guide gives `GwzOperationError.response`, operation/request identity, `effect="possible"`, and a remote-ref inspection step. | Caller sequence specified; typed result encoding remains blocked by P2-1 below. |
| Surface S3 | State cross-loop ownership. | The guide permits direct awaits on separate loops and gives a two-thread start, cancel, result, close path. | Closed. |
| Surface S4 | State capacity and retry defaults at the call site. | The guide locates the worker and per-host arguments, derives both pool limits, and explains three extra setup retries. | Closed for the original capacity walkthrough. |
| Surface S5 | State replay and result independence. | The guide says each event iterator replays from the start and that result access does not require draining it. | Closed for ordinary open-Client access; close racing an active iterator is P2-2 below. |

## Changed-range analysis

The root diff changes the supersession and close boundary (§1), generation-scoped caller ID and public serial issuance (§2, §5), different-capacity installation condition (§3), gated worker acceptance (§4), full record reservation and possible-effect terminal (§6), and cross-loop handle use (§7). The core retry-plan diff explicitly replaces its old installation condition. The Python design diff narrows post-Closing access; its caller-guide diff adds the cancellation, overflow, defaults, replay, and cross-loop instructions. The core transport-plan and v1.1 candidate annotations did not change in this round; their earlier amendments remain consistent with the revised overlap and placement statements.

**NEW ARCHITECTURAL root cause:** The now-explicit failed `OperationResult` contract has no representation for its promised `Cancelled` and `TransportRecordLimit` codes in the unchanged outer Taut error enum. This is P2-1. The revised close rule also fails to say how an already active event reader receives its promised terminal event after lookups become invalid. This is P2-2. Neither is a restatement of an original finding.

## 0. Evidence base

I read the root design at lines 7–61; all three filed prior reports and `GwzPyTransportConcurrencyDesign-RemPlan-2.md`; the changed ranges against the prior tuple; core retry-plan §3 item 7, §6, S1.4 and its candidate amendment; the remote transport plan’s Phase 2 row and candidate correction; v1.1 S6.3 and its candidate clarification; the accepted sequenced-stream design’s registration, ticket, and generation rules; and the Python transport design and concurrent-operations guide. I checked the committed Taut `GwzErrorCode`, `GwzError`, and `OperationResult` definitions and the existing Python event-iterator call path. Process authority was `AgentProcessRules.md` as amended by `GwzProcessOptimization.md`, with the review-loop report contract.

Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, `git diff <prior>..HEAD -- <documents>`, `rg`, `sed`, and `nl`. No files were written; no build, test, or Git mutation ran. Implementation acceptance, profile-3 host proof, platform evidence, and publication were outside this review.

## 1. Findings

### [P2-1] Promised terminal error types cannot be encoded in the unchanged `OperationResult`

**Location and invariant:** Root design lines 9, 45, and 47 promise unchanged request/response Taut schema, an attributed `TransportRecordLimit` terminal in an existing `OperationResult`, and a failed `OperationResult` with a `Cancelled` error. The caller guide lines 35 and 49 requires callers to inspect those results. In core `protocol/gwz.taut.py:669–794`, `GwzErrorCode` has neither `cancelled` nor `transport_record_limit`; `GwzError.code` is that enum at line 1148, and `OperationResult.errors` is a list of `GwzError` at line 1735. The generated core enum confirms the same absence.

**Counterexample and impact:** Cancel an admitted push, or let its own result exceed 8 MiB, then encode the required retained `OperationResult` for `handle.result()` or a unary exception’s `response`. There is no enum value for the promised error. Using a display string or an unrelated generic code would lose the stable classification that the guide asks the caller to use for recovery; adding the enum values would change the schema that §1 says remains unchanged. The required cancellation and overflow closure checks are therefore not satisfiable as written.

**Required correction:** Specify one coherent representation for both terminal classes. Either amend and review the outer Taut error enum and its compatibility implications, or define an explicit Python/native classification alongside an `OperationResult` whose existing error code remains truthful. State the exact value exposed by `GwzOperationError`, including unary calls, and remove any claim that the Taut result itself contains an unrepresentable code.

**Closure test:** Encode and decode cancelled and self-overflow terminal results through the actual core/Python projection. For handle, stream, and unary paths, assert the stable failure classification, both IDs, available transport facts, and `effect="possible"` without parsing message text.

### [P2-2] Closing invalidates event reads before an active iterator can receive its terminal event

**Location and invariant:** Root design line 47 says the terminal event is observable before `events()` ends, then says Closing refuses result/event lookups. Line 17 lists `wait_events` among session calls that look up the record. The caller guide line 35 promises a terminal event before the end of **every** event iterator, while line 51 refuses operation-record methods once Closing starts. The existing Python iterator calls native `wait_events` on each pass (`gwz-py/src/gwz/bridge.py:357–375`).

**Counterexample and impact:** Start A, consume its first event, and leave its iterator waiting for another. Concurrently call `Client.close()`. Closing begins, cancels A, and later publishes its terminal event, but the iterator’s next `wait_events` is an event lookup after Closing and must be refused. It cannot satisfy the unqualified promise to deliver the terminal event before ending. A caller using the documented event path loses the stated cancellation outcome or receives an undocumented close error.

**Required correction:** Define the lifecycle of an event subscription created before Closing. Permit that existing reader to drain through its terminal event while refusing new subscriptions, with its charged snapshot and waiter wakeup bounded; or explicitly narrow the terminal-event promise and specify the typed close outcome for an active iterator. Make the design and guide agree.

**Closure test:** Hold an iterator after a nonterminal event, race close against its next read, and verify the chosen documented outcome, finite wakeup, and reader-charge release. Check both an already pending native `wait_events` and a subsequent iterator read after Closing.

## 2. Invariant analysis

The original capacity counterexample now has a consistent answer across the root design and retry-plan candidate amendment: equal installed capacity does not resize a pool under a lease, and different capacity waits for zero live operations and finished cleanup. The Phase 2 exit-row correction distinguishes physical capacity from operation fan-out. The caller ID rule now matches core’s lifetime-used set, and the contiguous acceptance serial supports a fixed-size high-water expiry classifier across generation rollover. Full 8 MiB admission reservation prevents unrelated retained records from changing a within-limit success into record overflow.

The corrected post-close lookup rule is internally consistent for a *new* result, cancel, release, or event lookup, and repeat close has its own retained report. The active event-reader case is distinct because it started before Closing and needs another read to observe the terminal. The accepted sequenced-stream design’s delivery tickets and generation pinning remain a separate activation gate; I found no contradiction in the revised text’s treatment of that gate.

## 3. Risks and next action

The Python guide’s retry table describes the *current* API while the accepted Python transport design still calls for a later `Client.meta(max_retries=...)` addition. Align that wording when the draft becomes an active package guide; it does not change this verdict.

Keep the design **NO-GO**. Resolve P2-1’s terminal representation and P2-2’s active-reader close rule in one pinned correction, then re-run those two sequences and the original closure checks against the new tuple.
