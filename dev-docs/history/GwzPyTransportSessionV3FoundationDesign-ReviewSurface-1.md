# Python transport session v3 foundation — SURFACE-AXIS RE-REVIEW

**Review object:** Python caller guide only: `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`, compared with `gwz-py` commit `d29d450508138bda9251e797afdb21d71d20d8dc`.  
**Baseline SHAs:** root `21fac9f4236cd12df7719a92f039ae5cd787f067`; `gwz-core` `e7b4c499a2e5e2bfe8be0db2fbe067d6599a506f`; `gwz-py` `f6ae40afaa67ddaa561002dcee54d3c522739dd1`. Verified at start and end.  
**Date:** 2026-09-24  
**Axis:** SURFACE — Python caller guidance  
**Verdict:** **GO under the P0–P2 gate, with Surface P3-2 still open.** The changed example fixes the original refused handle but does not complete the retry handle’s lifecycle.

## Prior-finding closure table

| Finding | Status | Evidence |
| --- | --- | --- |
| Surface P3-1 — malformed code fence | Closed | The Python fence now ends on its own line; the generated-ID sentence and settings table follow outside it. |
| Surface P3-2 — leaked handle and incomplete retry | Partially closed | The refused handle is released before retry. The example then accepts a new handle and ends without observing its result or releasing it. |

## Changed-range analysis

The revised wording makes `request_id_consumed=False` contingent on the refusal’s cause clearing and the Client and generation remaining open. It treats `True` conservatively when registration cannot be ruled out. This is coherent with the guide’s distinction between Client identity, core generation, and retained records after close. The standalone sentence about generated request IDs is now outside the code fence. The Markdown table has a complete header and separator.

## 0. Evidence base

Read only the caller guide, its specified Git diff, and the three `rev-parse` values. No code, design, review, plan, tests, or builds were examined.

## 1. Findings

**Surface P3-2 — retry example still abandons an accepted operation.** Location: `GwzPyConcurrentOperationsV2.md`, lines 63–65. After a recoverable refusal, the example creates a fresh fetch handle and awaits `accepted()`, then stops. A caller following it has started real work but has neither awaited `result()` nor released the new record. The guide says dropping an accepted handle retains its ledger record until its deadline; repeated use can fill the 64-record limit and cause further pre-effect refusals. A second refusal at the new `accepted()` also leaves that new handle unreleased. Complete the example through result observation and release, including cleanup if the second admission fails or the awaiting task is cancelled. Closure check: every handle created in the example reaches its documented terminal cleanup path.

## 2. Invariant analysis

The text correctly directs callers to use `request_id_consumed`, rather than the error code, to decide ID reuse. It limits reuse to a fresh handle after the cause clears in an open Client and generation. The `True` branch avoids reusing an ID in that generation. The guide separately states that pre-acceptance `GwzOperationCancelled` carries the field and that its handle remains inspectable and releasable; the short `GwzBridgeError` example does not purport to handle task cancellation.

## 3. Risks and next action

Finish the retry example’s accepted, refused, and interrupted paths. This is a bounded documentation defect; no P0–P2 finding arose from the permitted surface.