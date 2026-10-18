# GWZ Transport Parallel Interfaces — SAFETY-AXIS RE-VERDICT

**Review object:** Draft `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` at `479926c18265276e5a45659c4523a13a71f4a51f` and draft `gwz-py/dev-docs/GwzPyTransportDesign.md` at `259f73cc030c0da0bf29903bab258de0463b7d02`, 2026-09-23.

**Baseline:** root `00827c75afb93f5855ee77019df7cb459c12754a`; core `479926c18265276e5a45659c4523a13a71f4a51f`; Python `259f73cc030c0da0bf29903bab258de0463b7d02`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`. Verified unchanged at start and end. All reviewed sources came from committed objects using `git show` and the specified commit-to-commit diffs; dirty scheduler and clock work was excluded.

**Date:** 2026-09-23

**Axis:** Safety—mixed-version interpretation, lifecycle ownership, cancellation, shutdown and credential containment. Independent, adversarial, read-only. The current other-axis report was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — both prior P2 findings are closed; no P0–P3 findings or new architectural root were found.

---

## 0. Evidence base

Read the merged `GwzTransportParallelInterfaces-RemPlan.md`; the complete core diff `f3640463..479926c1`; the complete Python diff `4a884174..259f73cc`; and the revised documents with exact line locations. Consulted committed Python bridge/client signatures only to check the amended public API boundary. No builds, tests, writes, dirty sources or current peer reports were used.

## 1. Prior-finding closure

| Prior finding | Status | Closure evidence |
|---|---|---|
| Safety P2-1 — `SetupFailureCause` lacked frozen wire discriminants | **Closed** | Core amendment lines 21–32 assign immutable values 1–7, reserve zero, reject unassigned values and forbid reassignment. Lines 74–80 preserve old/new behavior; lines 102–108 require golden cases for every value, missing cause, unknown values and old-reader compatibility. The former mixed-version reinterpretation counterexample is no longer permitted. |
| Safety P2-2 — Python close lacked a terminal, race-safe lifecycle | **Closed** | Python design lines 61–78 define one Rust-owned monotonic lifecycle and serialize construction, admission and close. Close joins in-flight construction/admission, prevents late publication or handler start, shuts each constructed runtime down once and makes Closing/Closed terminal. Lines 85–91 preserve cleanup facts; lines 247–254 require deterministic race, post-close and repeated-close tests. The former publish-after-close and reconstruct-after-close interleavings are prohibited. |

## 2. Changed-range and invariant analysis

The new public async surface preserves the same ownership boundary. `NativeCoreBridge.close()` and `Client.close()` return an immutable cleanup value, repeated native close returns the retained snapshot, context exit uses the same path, and a custom bridge without close returns `None` rather than claiming cleanup.

Cancellation remains request-scoped: the bridge maps a public operation ID to the active request, uses a cloneable request-specific native handle, shields finish from further Python cancellation, and resolves only after cleanup. Wrong, foreign and expired identities cannot affect the current request. Retaining at most the latest completed cancellation snapshot prevents an unbounded completed-operation registry. No Python-visible pool, raw transport object, credential or second lifecycle authority was introduced.

The typed-failure amendment remains fail-closed: missing or unknown causes cannot enable retry, old readers may discard only the optional field, and code/effect/facts retain their prior meaning.

**New-root classification:** none. The additions refine public observability and lifecycle ordering within the same two corrected roots.

## 3. Risks and next action

This is document acceptance only. The specified CBOR compatibility, lifecycle-race, cancellation-identity and cleanup tests remain implementation gates and were not executed here. This Safety axis permits the two implementation lanes to proceed against the corrected drafts.
