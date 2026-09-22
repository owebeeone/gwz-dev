# GWZ Remote Transport HTTPS H1 — State Re-verdict 2

**Date:** 2026-09-22  
**Verdict:** **GO**  
**Open findings:** None. No P0/P1/P2/P3 findings.

## Review object

The exact tuple matched at both review boundaries:

- root: `63ef26308979b6ce2e2925d71a96f42afcde2645`
- core: `e29e799ee65fb9794ac2fad7972d94707262b4cb`
- transport: `aa40936d0805e8cb60f8027615abe20d4f2045e4`
- taut: `bcf98b64d465fc54841121b6d1a2d46940f81a3c`
- CLI: `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`
- Python: `d07d55dacb1725d9306be9c04d157ac29a78e000`
- git2-rs: `ce78628308e11b4e8901d5061602619109bce21a`
- libgit2: `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`
- evidence: `fc1caa478c1fcd9539b2c061c51be17b64924d7c`

All member trees were clean. Root contained only the permitted generated current-round prompts and four old N2b prompts. I did not inspect the current Code-axis prompt or report.

This focused review covered core range `afd3b8f58ca4e5bea788f1a4d40317d4e2a74a6e..e29e799ee65fb9794ac2fad7972d94707262b4cb`, limited to `https_auth.rs`, `https_worker.rs`, the H1 checkpoint update, RemPlan-2, and correction-two evidence.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Endpoint shutdown owns active preparations and retained helper children, rejects later work, and reports pending cleanup. | Re-traced the original started-helper, post-shutdown admission, future-abort, and two-endpoint isolation paths. Correction two retains the endpoint owner, cancellation, child guard, and semaphore ownership established in correction one. Shutdown now reads active preparations before its final retained-child snapshot, preventing an active-to-retained transfer from disappearing between observations. | **Closed** |
| State P2-2 | Operation dependencies preserve the write-once route until whole-operation retirement. | Operation, route, and local-RPC sources are byte-identical to the correction-one GO tuple; the 66-test final gate includes the A/B/C lifecycle regression. | **Closed** |
| State P2-3 | Actual worker opening failures traverse the mux as `OpenFailed`. | Opening/local composition is byte-identical to the correction-one GO tuple; the final gate retains status, malformed response, loss, trust, deadline, and automatic-auth opening tests. | **Closed** |
| State P2-4 | Credential-offer facts remain cumulative across redirects. | Redirect/fact code is byte-identical to the correction-one GO tuple; the final gate retains the redirected helper-failure regression. | **Closed** |
| Owner P2-1 | A reap future must retain child and permit ownership if aborted, report children arriving during its wait, and preserve the active-to-retained shutdown count. | Re-traced both original red sequences through the corrected guarded batches. Aborting the future drops `ReapBatch`, which returns every unreaped child and permit to the same owner. In-flight children contribute through `reaping`; concurrent arrivals remain in `pending`; the final count observes both. The orphan fallback applies the same guarded ownership and in-flight accounting. Endpoint shutdown’s active-then-retained snapshot cannot return zero across the transfer. | **Closed** |

## Changed-range analysis

`AuthOwner` now maintains an atomic in-flight reap count in addition to its pending queue. `reap_pending` moves queued children into a `ReapBatch` while holding the queue lock and increments the count before releasing that lock. Each successfully joined child decrements the in-flight count. Dropping or aborting the batch reacquires the queue lock, returns every unreaped child with its `OwnedSemaphorePermit`, and only then decrements the count.

This ordering is cancellation-safe:

1. A child cannot exist outside both the queue and the in-flight count at an observable suspension point.
2. A count reader holding the queue lock may transiently overcount a transfer, but cannot undercount it.
3. A child added while another batch waits remains queued and is included in the returned snapshot.
4. The batch retains an `AuthOwner` clone, so the owner cannot fall into orphan cleanup while its reap is in flight.
5. The orphan batch uses the same retain-on-drop ordering and global in-flight count.

`Endpoint::shutdown` now completes its bounded reap and physical shutdown, reads active preparation slots, then reads retained/in-flight helper work. A helper becomes retained before its enclosing preparation releases its slot. The ordering can conservatively double-count a racing completion, as documented, but cannot falsely report zero.

I found no changed-range state defect. **Owner P2-1 is a bounded continuation of the original helper ownership/accounting invariant, not a new architectural root cause.** The endpoint-scoped owner and cleanup grammar remain unchanged; correction two closes two missed cancellation/snapshot interleavings inside that implementation.

## Evidence

I verified both corrected source hashes and all six archived artifact hashes with zero mismatches. The retained red run executes nine helper tests and records both causal failures on correction one:

- aborted reap reports `0`, expected `1`;
- concurrent arrival reports `0`, expected `1`.

The final archived gate reports 66 endpoint tests passing, including both new regressions and all correction-one lifecycle tests. Formatting and token-aware conditional-boundary checks also pass. The archive correctly excludes the earlier zero-test filter attempt from causal evidence.

Per the prompt, I did not rebuild or rerun the archived gates.

## Verdict and scope

**GO** for HTTPS H1 correction two on the State axis. All original State findings and Owner P2-1 are closed, with no new architectural root or other finding.

This verdict remains limited to the private H1 candidate. It does not accept H2 integration, higher-host shared authority injection, Placement C cleanup-accounting closure, platform or selected-source qualification, real GitHub parity, production activation, physical wire transport, release readiness, or performance.
