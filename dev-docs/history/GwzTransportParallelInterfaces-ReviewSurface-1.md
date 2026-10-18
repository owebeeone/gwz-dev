# GwzTransportParallelInterfaces — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` and `gwz-py/dev-docs/GwzPyTransportDesign.md` at the correction-1 tuple; interface draft, design-only; 2026-09-23  
**Baseline:** `. 00827c75afb93f5855ee77019df7cb459c12754`; `gwz-core 479926c18265276e5a45659c4523a13a71f4a51f`; `gwz-py 259f73cc030c0da0bf29903bab258de0463b7d02`; `gwz-transport aa40936d0805e8cb60f8027615abe20d4f2045e4`. Read only `gwz-py/dev-docs/GwzPyTransportDesign.md:44-228` with `git show`; no implementation or peer reports.  
**Date:** 2026-09-23  
**Axis:** Surface semantics for Python close/cancel lifecycle, capability placement and exposed defaults. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0/P1/P2/P3 findings.

---

## 0. Evidence base

The corrected Python design now specifies:

- Monotonic native lifecycle and close race behavior at `:61-91`.
- Public immutable `TransportCleanup`, async `NativeCoreBridge.close()` and `Client.close()` at `:163-174`.
- Async `cancel_operation(operation_id)` on both bridge and client, request mapping, completion, repeat behavior, invalid-ID errors and queued-waiter semantics at `:176-191`.
- Same-generation capability preflight, local default and explicit `cli` requirements at `:193-204`.
- Accepted exposed retry defaults at `:215-228`.

All four repository heads matched the required correction-1 tuple before and after review. No builds or tests were run.

## 1. Findings

| Prior finding | Closure |
|---|---|
| P2-1: close lifecycle lacked a concrete public contract | **Closed.** Public async bridge/client signatures, cleanup value, idempotent snapshot behavior and `__aexit__` delegation are explicit. |
| P2-2: cancellation lacked target and completion semantics | **Closed.** `operation_id`, typed invalid-ID behavior, request-specific mapping, awaited finish, repeat cancellation and queued-lock behavior are explicit. |

Changed-range analysis found no new Surface defect. No architectural or documentation findings remain.

## 2. Invariant analysis

The revised text provides one discoverable lifecycle owner, preserves cleanup facts, prevents post-close publication, and distinguishes queued Python cancellation from admitted native cancellation. Capability placement defaults to local; explicit `cli` requires the same installed runtime, endpoint and capability intersection. Defaults for `jobs`, `max_per_host` and `max_retries` remain exposed.

## 3. Risks and next action

Surface review is closed. The next action is implementation against these frozen signatures and lifecycle guarantees.
