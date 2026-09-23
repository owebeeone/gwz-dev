# GwzTransportParallelInterfaces — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` and `gwz-py/dev-docs/GwzPyTransportDesign.md` at the exact committed tuple; interface draft, design-only; 2026-09-23  
**Baseline:** `. 4ad1aa3d00c7f3ac5b725cc271dc94d28b96a326`; `gwz-core f3640463bd1c69d29322de9d92902eff54af7b8e`; `gwz-py 4a884174a0a1a3a9f8d9aa756017d1b8c6034813`; `gwz-transport aa40936d0805e8cb60f8027615abe20d4f2045e4`. Read only `gwz-py/dev-docs/GwzPyTransportDesign.md` sections 2–4 with `git show`; no implementation or peer reports.  
**Date:** 2026-09-23  
**Axis:** Surface semantics for Python close/cancel lifecycle, capability placement and exposed defaults. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 documentation/interface findings block.

---

## 0. Evidence base

Read `gwz-py/dev-docs/GwzPyTransportDesign.md:44-180`, covering host and operation lifetime, admission/cancellation/thread behavior, and Python API/protocol/security.

The document clearly specifies lazy host construction, local-only isolation, one native lifecycle owner, overlap refusal, request-scoped cancellation, explicit `cli` placement requirements, and retry defaults of `jobs=100`, `max_per_host=32`, and `max_retries=3`.

All four repository heads matched the required tuple before and after review. No builds, tests, source inspection or writes were performed.

## 1. Findings

### [P2-1] Close lifecycle has no concrete public Python contract

`GwzPyTransportDesign.md:66-71, 135-143` names `NativeCoreBridge.close()` and says it awaits request finish and host shutdown, but gives no Python signature or awaitability contract. The same section says the native session remains private to `gwz.bridge`; only `Client.__aexit__` is mentioned as an existing caller.

A user constructing `Client` cannot determine whether to call `await client.close()`, `await bridge.close()`, or rely exclusively on `async with`. This makes the cleanup lifecycle and returned `CleanupReport` undiscoverable and risks abandoning bounded cleanup facts.

Define one public lifecycle path, including exact async signature and ownership: either document `Client.close()` and `async with Client`, or explicitly expose and specify `NativeCoreBridge.close()`. Closure requires a Python fixture that awaits close twice, verifies idempotence and cleanup facts, and verifies the context-manager path uses the same operation.

### [P2-2] Request cancellation lacks target and completion semantics

`GwzPyTransportDesign.md:113-116, 135-143` states that native cancellation is keyed by request ID and that task cancellation awaits bounded finish, but adds `cancel_operation` without specifying its argument, whether it is synchronous or awaitable, its result, or its race with queued Python-lock cancellation.

A caller cannot reliably identify the operation to cancel or know when request cleanup is complete. Define the target identity and return contract, including wrong-ID behavior and whether cancellation resolves only after `TransportRequest.finish()`.

Closure requires tests for active setup cancellation, active stream cancellation, a wrong request identity, queued lock cancellation, and proof that only the targeted request is cancelled while cleanup facts remain observable.

## 2. Invariant analysis

The design preserves one Rust-owned session and correctly separates Python convenience locking from authoritative Rust overlap refusal. Capability checks and explicit `cli` placement are described with local placement as the default, and retry defaults are exposed.

The two lifecycle gaps prevent those otherwise clear invariants from being used reliably through the proposed Python surface.

## 3. Risks and next action

The risks are interface-definition defects, not implementation findings. Amend sections 2–4 with concrete close and cancellation signatures and their async/result semantics, then rerun the Surface review.
