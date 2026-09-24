# GWZ operation-session protocol — Consistency re-review, round 2

**Date:** 2026-09-23  
**Verdict:** **NO-GO** — 0 P0, 0 P1, 3 P2, 0 P3.

## Reviewed object and evidence

I reviewed the committed protocol design, caller guide and paired **draft** core capacity amendment at this exact tuple:

| Repository | Commit |
| --- | --- |
| gwz-dev | `ee11f44efa5a0796a71d874ffaa4bc570a611a4e` |
| gwz-core | `8756fd6b32443b0ac63287ee5b4a3335e8cf0894` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

All four commits matched at the start and end. I inspected committed documents and targeted schema and Python source, including both prior-round reports and their remediation plan. I made no changes, ran no builds or tests, and did not inspect a current-round peer prompt or report. Wire-carrier implementation, platform qualification and release remain outside this verdict.

## Prior-finding closure

These are design-text assessments, not implementation acceptance.

| Round-1 finding | Re-review disposition |
| --- | --- |
| Consistency P2-1, full typed terminal response | The new terminal record includes the action-specific encoded response. Storage-shape concern addressed; failed-response delivery still conflicts with the accepted Python API contract (new P2-3). |
| Consistency P2-2, aggregate member workers | Addressed by session and receiver permits, separate operation-worker and queue limits, and numeric ceilings. |
| Consistency P2-3, exact capacity supersession | **Open.** The amendment quotes the named refusal and Phase 2 sentences, but leaves earlier capacity-installation clauses in force (P2-1). |
| Consistency P2-4, numeric defaults | Addressed by the 22 allocated limit fields and numeric session and receiver budgets. The fixed fallback reservation has an unbounded-input gap (P2-2). |
| Safety P2-1, local-operation completion | Addressed on paper by an execution scope for every accepted operation and by withholding successful close until local handlers join. |
| Safety P2-2, caller-ID reuse | Addressed on paper by a distinct internal transport request ID within each mux generation. |
| Safety P2-3, terminal-result bytes | Per-record and aggregate limits are specified, but the guaranteed small terminal is not bounded for an accepted caller ID (P2-2). |
| Safety P2-4, abandoned sessions | Addressed on paper by receiver and route session limits, owner-loss close, idle expiry and finite close-report retention. |
| Consistency P3-1, handle release | Addressed by `async release()` and matching caller-guide behavior. |

The changed ranges since the round-1 tuple are the root design’s execution scope, terminal/schema allocation, admission, capacity and numeric-budget sections; the caller guide’s signatures and complete handle lifecycle; and the new 88-line core capacity amendment. Python and transport commits did not change. I checked the full corrected contract as well as those ranges.

## Findings

### P2-1 — The capacity amendment leaves a conflicting installation trigger

**Location:** `gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md:10–18,20–44,61–72`; accepted `gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md:127–133,325–339,415`; `dev-docs/GwzOperationSessionProtocolDesign.md:246–256`.

**Invariant and reproduction:** The corrected rule must forbid changing installed physical capacity while **any admitted operation scope** is live, including one that has not opened a lease. The amendment establishes that rule at lines 61–72. Its supersession of retry §3 item 7 addresses only the lower-limit implication, however, leaving “At operation start, when no lease is non-idle, the operation installs its resolved …” intact. It replaces §6’s non-idle refusal bullet but leaves §6’s preceding “If the pool has no non-idle lease” installation bullet intact. S1.4 still directs installation “as §6 and §3.7–§3.8 describe.”

Admit A and pause it before its first Open, so A holds no non-idle lease. Submit B requesting a higher cap. The remaining accepted text instructs B to install its cap; the amendment’s epoch rule forbids that change because A is admitted. This is the exact pre-lease race that the new definition of quiescence intends to close.

**Impact:** The purported exact supersession leaves two valid-looking implementation and test oracles for the same state. A pool may resize beneath an accepted operation or reject a request that another implementer admits.

**Required correction:** Explicitly replace §3 item 7’s no-non-idle installation trigger, §6’s first installation bullet, and S1.4’s reference to them with the admitted-scope/cleanup quiescence rule. Preserve the quoted lower-limit overlap and Phase 2 replacement.

**Closure test:** Hold accepted A before its first lease; verify that equal or lower B reserves the installed epoch without resizing and that higher B refuses before Open. After A and cleanup retire, verify the higher cap installs. Cross-check every surviving §3, §6 and S1.4 sentence against that sequence.

### P2-2 — A fixed 4 KiB fallback cannot retain every admitted caller ID

**Location:** `dev-docs/GwzOperationSessionProtocolDesign.md:82–105,172–185,289–311`; `gwz-core/protocol/gwz.taut.py:1108–1121`.

**Invariant and reproduction:** Every accepted operation must have one readable, attributed terminal outcome within its reserved byte charge. `RequestMeta.request_id` is an unrestricted caller-provided `STR` in the reviewed schema, and the new admission checks impose no length limit. The required `ResultLimitExceeded` fallback itself contains that caller request ID, plus the operation ID, action, effect and error. The design reserves only 4 KiB for it.

Submit an otherwise valid operation with an 8 KiB request ID, receive `Accepted`, then cause its full response to exceed the terminal budget. The required fallback cannot fit the charge reserved for it even before encoding overhead. Either it exceeds the published budget, loses attribution, or fails to produce the promised terminal.

**Impact:** The byte-bound correction to Safety P2-3 can still lose an accepted result or violate its aggregate memory limit.

**Required correction:** Bound caller ID bytes at admission so the entire worst-case encoded fallback fits 4 KiB, or reserve a measured fallback charge that includes the accepted ID and all mandatory fields. State the refusal code and perform the check before `Accepted`.

**Closure test:** Exercise the maximum allowed multibyte ID and the next byte over it, then force an oversized fetch or merge response. Assert one readable attributed fallback within per-record, session and receiver byte charges.

### P2-3 — The failed-result API does not preserve the original typed response

**Location:** `dev-docs/GwzOperationSessionProtocolDesign.md:79–95,196–210`; `dev-docs/GwzOperationSessionCallerGuideDraft.md:61–73`; accepted `gwz-py/dev-docs/GwzPyDesign.md:339–356`; committed `gwz-py/src/gwz/errors.py:46–55` and `src/gwz/client_helpers.py:62–80`.

**Invariant and reproduction:** The accepted Python contract requires `GwzOperationError` to preserve the **original protocol response** on rejected, failed, partial, dirty and conflicted outcomes. The current exception has a `response` field and the current helper fills it. The correction retains a full typed response in the terminal record, but specifies that a failed or cancelled `handle.result()` raises an exception with only the terminal `OperationResult` and code attached. The caller guide lists code, IDs and `operation_result`, without specifying the original generated response.

Produce a conflicted merge whose `MergeResponse` contains state, record and recovery fields. A caller using the existing unary `Client.merge()` contract catches `GwzOperationError` and reads `exc.response`; an implementation following the revised error description can raise with only the summary `OperationResult`. That summary lacks the merge-specific fields. The unary caller has no handle through which to retrieve the separately retained terminal.

**Impact:** The proposed wrapper can regress existing recovery and diagnostic access despite retaining the response internally. Handle, unary and stream failures no longer have the promised typed-response parity.

**Required correction:** Require every failure with an available action response to attach the decoded generated response to `GwzOperationError.response`, alongside the `OperationResult` and code, across handle, unary and stream paths. Specify the distinct exception shape for `ResultLimitExceeded`, where no full response exists, and align the guide.

**Closure test:** For failed fetch and conflicted merge, compare each path’s exception `response` type and action-specific fields with the retained terminal. Verify the oversized-result exception has the explicitly documented fallback shape.

## Verdict and two-round cap

**NO-GO** while P2-1 through P2-3 remain open. Each has a bounded text or schema-contract correction. I identified **no new architectural root cause** that triggers the two-round redesign cap.

I pre-commit to **GO** on a revision that resolves P2-1 through P2-3 as specified, provided it introduces no new blocking contradiction. Implementation, generated-schema, Python Phase 6/7 and release gates remain separate.
