# GWZ operation-session protocol — Consistency review

**Review object:** Committed `dev-docs/GwzOperationSessionProtocolDesign.md` (controlling DRAFT) and `dev-docs/GwzOperationSessionCallerGuideDraft.md`.  
**Tuple:** root `852fd94a082f82b8d4e9d777edf7d20acdf12576`; gwz-core `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four SHAs matched at review start and end.  
**Date / axis / verdict:** 2026-09-23 · Consistency · **NO-GO** — 0 P0, 0 P1, 4 P2, 1 P3.

## Evidence base

I inspected the committed review object; the accepted core remote transport, retry and v1.1.0 plans; the Taut schema; the accepted Python transport and API designs; and the prior Python concurrency findings and remediation. Process authority was `AgentProcessRules.md`, its `GwzProcessOptimization.md` amendment, and the review-loop skill. This was read-only inspection. I ran no builds or tests and did not inspect another current-round review.

## Findings

### P2-1 — The terminal record cannot preserve the existing typed response

**Location:** `GwzOperationSessionProtocolDesign.md:61,92–95,101–104,110–114`; `gwz-core/protocol/gwz.taut.py:1724–1737,2196–2225`; `gwz-py/dev-docs/GwzPyDesign.md:351–356`.

**Invariant and sequence:** The draft promises that accepted work has one immutable terminal result and that handle, unary and stream forms expose the same typed outcome. The existing `OperationResult` contains status, members and errors, but lacks action-specific response fields. For example, `FetchResponse.repos` contains repository summaries; `MergeResponse` contains merge state, record and recovery fields. Submit a fetch through `start_fetch()`, then call `handle.result()`: if `result_v1` returns the existing `OperationResult`, the `repos` data returned by `Client.fetch()` is unavailable. A separate retained response store would contradict the draft’s one-record retention and release contract unless explicitly owned and bounded with it.

**Impact:** The proposed handle and stream paths can lose public response data or develop another unaccounted result store. **Required correction:** Define the version-1 terminal shape or typed response retrieval that preserves each action’s full response, its error policy, ownership and retention. **Closure test:** Compare handle, unary and stream results for fetch and a response-rich action such as merge; assert identical typed fields and expiry together after release.

### P2-2 — The worker budget excludes aggregate member workers

**Location:** `GwzOperationSessionProtocolDesign.md:131–137,173–182`; `gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md:295–303,315–323`; prior `GwzPyTransportConcurrency-RemPlan-1.md` worker-budget closure row.

**Invariant and sequence:** Accepted concurrent work must have a finite aggregate worker bound independent of caller-controlled `jobs`. The draft distinguishes Python worker slots and live-operation slots from pool checkout capacity, but gives each admitted operation its own `jobs` limit without an aggregate limit on the core member workers those operations spawn. Sixteen admitted operations at the default 100 jobs can create up to 1,600 member workers while staying within sixteen Python worker slots and the physical connection cap. Larger explicit `jobs` multiplies this further.

**Impact:** The prior aggregate-worker finding remains open; admission can succeed before the process runs out of threads or memory. **Required correction:** Assign aggregate member-worker permits or an equivalent bounded scheduler to the session/host, with a defined admission or queue outcome. **Closure test:** Overlap many multi-member operations, including large explicit `jobs`, and assert a fixed aggregate worker/queue ceiling through cancellation and close.

### P2-3 — The claimed capacity supersession is absent from the reviewed authority tuple

**Location:** `GwzOperationSessionProtocolDesign.md:139–153,188–193`; `gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md:127–133,336–339,415`; `gwz-core/dev-docs/GwzRemoteTransportPlan.md:212–214`.

**Invariant and sequence:** A reviewed capacity rule must identify precisely which accepted clauses it replaces. The draft admits a lower-limit operation while an installed capacity accommodates it, even with live leases. Retry §6 still says, “If any lease is non-idle, the new operation is refused with a typed error”; S1.4 says, “Refuse the operation when a non-idle lease exists.” Phase 2 still requires overlapping operations with different per-host limits. The draft calls its change an explicit amendment but defers the exact retry and Phase 2 amendments to a separate future review; those amendments and replacement text are absent from this tuple. An implementer following §6/S1.4 would refuse the draft’s required overlap, while one following this draft would violate the still-accepted text.

**Impact:** The proposed design cannot yet be used as an unambiguous implementation contract or satisfy its own authority cross-check. **Required correction:** Put the exact superseded §6, S1.4 and Phase 2 sentences and their replacement rules into the reviewed amendment tuple, including the lower-limit held-lease case. **Closure test:** Check the three documents together, then admit a lower-limit operation under a held lease, refuse a required raise before Open, and verify the revised Phase 2 different-limit case.

### P2-4 — The review prerequisite for numeric resource defaults is unmet

**Location:** `GwzOperationSessionProtocolDesign.md:57,173–182,188–194`.

**Invariant and sequence:** The draft says the session publishes finite limits and that the implementation design “must choose and publish numeric defaults before review.” Its ordered closure list places numeric defaults in step 1 and this design review in step 2, yet the reviewed object supplies none. A session with one live slot and one with a million slots both satisfy “finite,” while having materially different overlap and memory behavior. A reviewer cannot validate the promised resource bound or the `CapacityBusy` threshold from this tuple.

**Impact:** The resource and admission contract remains implementation-selected, and its stated design gate is not satisfiable as written. **Required correction:** Specify reviewable default values and which limits are configurable, including their allowed bounds, before re-review. **Closure test:** Assert admission at each published boundary and refusal at the next unit; verify result and event retention stay within their published defaults.

### P3-1 — The caller guide uses a release method absent from the handle contract

**Location:** `GwzOperationSessionCallerGuideDraft.md:46–50`; `GwzOperationSessionProtocolDesign.md:62–64,101–108`.

**Invariant and sequence:** The caller guide instructs users to call `handle.release()` to free the terminal record. The draft defines `operation.release_v1` but lists the Python `OperationHandle` methods as `events()`, `result()` and `cancel()`, without specifying a release method or whether it must be awaited. A caller following the guide has no settled Python signature or completion point for releasing its retained-result slot.

**Impact:** A required half of the retention lifecycle is ambiguous at the public interface. **Required correction:** Add the release signature and completion semantics to the handle contract and make the guide match. **Closure test:** Run the guide’s handle lifecycle through result, release, repeat release and expired reads using only its documented API.

## Invariant assessment and next action

The draft addresses several earlier counterexamples on paper: session-owned version-1 lookup, early operation handles, close completion surviving waiter cancellation, CLI placement refusal, retained results and a generation concept for the 256-ID rollover. Those are design claims, not implementation evidence. The blocking findings above prevent a Consistency GO on this tuple.

**I pre-commit to GO on a revision that resolves P2-1 through P2-4 as specified**, provided the corrected text introduces no new contradiction. P3-1 should be corrected with the same caller-guide revision. The implementation and release NO-GO remain separate gates.
