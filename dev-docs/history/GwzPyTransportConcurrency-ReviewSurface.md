# Python transport concurrency — independent Surface review

**Date:** 2026-09-23  
**Axis:** Python caller surface  
**Review object:** Committed `dev-docs/GwzPyTransportConcurrencyNoGo.md`, controlling DRAFT `dev-docs/GwzPyTransportConcurrencyRemPlan.md`, and the status annotation in `gwz-py/dev-docs/GwzPyTransportDesign.md`.  
**Baseline:** root `c1cb20c98c803500fe7c4ad4d0451e6c5905df28`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-core `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. The tuple matched at review start and end.  
**Verdict: NO-GO** — four P2 findings remain open.

| P0 | P1 | P2 | P3 |
| ---: | ---: | ---: | ---: |
| 0 | 0 | 4 | 1 |

## 0. Evidence base

I compared the committed draft with `gwz-py/dev-docs/GwzPyTransportDesign.md` §§3–4, `gwz-py/dev-docs/GwzPyDesign.md`’s public API and error model, and the committed `gwz-py/README.md`. Process authority was `dev-docs/AgentProcessRules.md`, as amended by `dev-docs/GwzProcessOptimization.md`, and the review-loop skill. This was document inspection only for the findings; no build or test was run.

**Method deviation:** During document discovery I opened committed Python bridge and client source, contrary to this axis’s docs-only instruction. No source observation supports a finding below. This report should not be counted as a fully docs-blind Surface review if that independence is required for acceptance.

## 1. Findings

### [P2-1] A live high-level operation has no documented way to expose its cancellation ID

- **Where:** Remediation plan §3, lines 25–27; accepted Python design §4, lines 181–189; `GwzPyDesign.md` lines 241–246.
- **Violated invariant:** A caller offered `Client.cancel_operation(operation_id)` must be able to obtain that ID while the operation is cancellable.
- **Caller sequence:** A caller starts `fetch_stream()` and its submission is accepted, but no progress event has arrived. A second task needs to cancel it by ID. The documented stream form yields events; it does not return the accepted response or an operation handle. A unary `await client.fetch()` likewise exposes its response only after completion. The bridge maps IDs internally, but the public contract gives this caller no pre-completion ID.
- **Impact:** The new by-ID cancellation API is unreachable for ordinary high-level calls during precisely the interval in which it is useful. Cancelling a Python task is possible when the caller owns that task, but does not provide the promised by-ID control to another component.
- **Remedy:** Freeze a documented way to supply or obtain the stable operation ID before waiting for the first event or final response, while preserving existing call signatures if desired.
- **Closure test:** Start a high-level stream with submission accepted and its first event blocked; obtain its ID through the documented public API, cancel it from another task, and show an overlapping peer continues.

### [P2-2] The public `Capacity` outcome does not distinguish its causes or delivery point

- **Where:** Remediation plan §2, lines 15 and 19, and §3, line 25; `GwzPyDesign.md` error model, lines 339–356.
- **Violated invariant:** A caller must be able to identify a refused operation and the condition required for a safe retry, whether it used `call`, early-returning `submit`, or a stream helper.
- **Caller sequence:** A default-policy fetch holds capacity. One concurrent submission requests a different physical capacity; another reaches `max_requests`. Both are specified as `Capacity`, although the former requires all conflicting work and cleanup to retire while the latter can clear when a live slot frees. The draft also permits `submit` to return `Accepted` before capacity admission, but does not pin whether these refusals surface from `await submit` or from its later operation result, nor require equivalent typed attribution through the stream form.
- **Impact:** A caller cannot reliably choose when to retry or where to catch the refusal. `Accepted` can be mistaken for admission despite the draft’s warning, and a generic `Capacity` gives no stable basis for recovery.
- **Remedy:** Specify the public timing and typed representation of each refusal for all three call forms. If refusal follows `Accepted`, require a retained, request-attributed terminal result and the same typed outcome from the high-level stream. Give live-ID exhaustion and incompatible installed capacity distinguishable stable reasons and retry conditions.
- **Closure test:** Hold a compatible operation open; exercise both capacity refusals through `call`, `submit` plus `operation_result`, and a stream helper. Assert reason, operation attribution, delivery point, and success after each stated retirement condition.

### [P2-3] One global completed-cancellation slot breaks repeat cancellation under overlap

- **Where:** Remediation plan §3, line 27.
- **Violated invariant:** A repeated cancellation of an operation must not change meaning merely because a different operation completes.
- **Caller sequence:** A and B are live. Cancellation of A finishes and records A’s cleanup snapshot. B then finishes cancellation and replaces the “latest completed” slot. A caller repeating `cancel_operation(A)` after an interrupted or uncertain first response receives `InvalidRequest`, indistinguishable from an ID that never belonged to this client.
- **Impact:** The promised idempotent repeat is unreliable under the newly supported concurrency; a caller cannot recover A’s cleanup facts after an unrelated completion race.
- **Remedy:** Define bounded per-ID terminal retention sufficient for concurrent completions, with an explicit expiration rule, or narrow the repeat guarantee and provide another stable way to recover the completed cancellation outcome.
- **Closure test:** Cancel A and B concurrently, arrange B’s completion after A’s, then repeat cancellation of A and verify the contract’s retained outcome. Also verify eventual bounded expiration and foreign-ID refusal.

### [P2-4] Cancellation of `close()` and `async with` exit has no completion contract

- **Where:** Remediation plan §3, line 29; accepted Python design §4, lines 172–178.
- **Violated invariant:** Once close enters `Closing`, caller cancellation must not strand its only inspectable completion path or leave the context manager’s cleanup outcome undefined.
- **Caller sequence:** Two network tasks are active. An `async with Client(...)` block exits during task cancellation, or the caller’s `close()` await is cancelled by a timeout or repeated `Task.cancel()`. The draft says concurrent close calls join and return one report, but says nothing about shielding or retaining the underlying close operation when its Python waiter is cancelled.
- **Impact:** A normal asyncio cancellation can interrupt the awaited cleanup path; the caller cannot tell from the frozen contract whether shutdown continues, whether another `close()` joins it, or when the retained `TransportCleanup` becomes available.
- **Remedy:** Specify one shared close-completion task that survives waiter cancellation, the propagation of `CancelledError` after cleanup, and the behavior of a later `close()` and `__aexit__` in that case.
- **Closure test:** With two active operations, cancel the first close waiter repeatedly and exercise cancellation during `async with` exit. Verify every operation finishes once, shutdown runs once, and a subsequent awaited close returns the same final report.

### [P3-1] The proposed concurrent workflow is absent from the caller guide

- **Where:** Remediation plan §4, lines 33–36; committed `gwz-py/README.md` lines 26–39.
- **Violated invariant:** The published Python guide should let a caller discover the supported overlap and recovery path without reading an internal remediation plan.
- **Caller sequence:** A reader follows the README’s single `async with Client` example, then launches two fetches with different per-host policies. The new compatibility rule refuses one, but the guide explains neither that rule nor how `call`, `submit`/streams, by-ID cancellation, and close differ.
- **Impact:** The feature can appear broken or inconsistent even when implemented as drafted.
- **Remedy:** Add a short documented same-client concurrent example and state capacity compatibility, where terminal results appear, and what context exit does to unfinished work.
- **Closure test:** Review the README alone as a first-day walkthrough: start two default-policy operations, observe both outcomes, cancel one, and close the client.

## 2. Invariant analysis

The unchanged async signatures can support a useful basic workflow: two default-policy `client.fetch()` calls in separate tasks can overlap, and each can retain its own result and cancellation scope. The draft correctly distinguishes per-operation `jobs` and retry settings from shared physical capacity, and correctly refuses a different physical capacity while live leases remain.

The caller contract is incomplete at the boundaries those signatures expose. `call` waits for a terminal answer; `submit` can return `Accepted` before admission; a stream helper hides the accepted response behind an async generator. The draft must state where the same typed capacity failure appears in each form. Its ID cancellation method also needs a public ID source before terminal completion. Finally, concurrent completion makes “latest completed cancellation” an unstable recovery rule, and Python task cancellation needs an explicit close rule.

The prior accepted design’s blanket overlap refusal is superseded by this draft and was not treated as authority against concurrency.

## 3. Risks and next action

Keep the amendment at **NO-GO** until the four P2 caller contracts are frozen and their counterexamples are added to the closure gates. Add the README walkthrough before claiming the new behavior discoverable. Because of the disclosed source-inspection deviation, obtain a fresh docs-only Surface review if strict peer-blind Surface evidence is required. No implementation or release-gate conclusion follows from this document review.
