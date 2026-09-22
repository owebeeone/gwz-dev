# GWZ Remote Transport HTTPS H1 — State Re-verdict 1

**Date:** 2026-09-22  
**Verdict:** **GO**  
**Open findings:** None. No P0/P1/P2/P3 findings.

## Review object

The exact tuple matched at both review boundaries:

- root: `7944b963b96f557768a332e64efaa018346573a1`
- core: `afd3b8f58ca4e5bea788f1a4d40317d4e2a74a6e`
- transport: `aa40936d0805e8cb60f8027615abe20d4f2045e4`
- taut: `bcf98b64d465fc54841121b6d1a2d46940f81a3c`
- CLI: `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`
- Python: `d07d55dacb1725d9306be9c04d157ac29a78e000`
- git2-rs: `ce78628308e11b4e8901d5061602619109bce21a`
- libgit2: `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`
- evidence: `3f5a4cce6bc108960f81a69880c2fda91be3895b`

All member trees were clean. Root contained only the permitted generated current-round prompts and four old N2b prompts. I did not inspect the current Code-axis prompt or report.

The review covered core range `280f970a66047942b006cfecfae51db60fa0550b..afd3b8f58ca4e5bea788f1a4d40317d4e2a74a6e`, the merged remediation plan, my filed initial State report, the corrected H1 checkpoint, and correction evidence. Transport was unchanged.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | Endpoint-scoped admission and cancellation own active preparations and retained helpers; shutdown closes admission before cancellation and reports all owned work. | Re-traced the original blocking-helper sequence through `https_worker.rs:91-118` and `https_auth.rs:49-173,273-492`. Admission closes before owner cancellation; active preparations retain the endpoint request slot; post-shutdown acquisition fails before helper spawn; aborted helper futures transfer the live child and global helper permit to the endpoint owner. `https_lifecycle_tests.rs:157-193` covers started-helper shutdown, post-shutdown rejection, repeated convergence, and two-endpoint isolation. `https_auth.rs:775-833` covers abort after the child-start barrier and proves the permit remains reserved until reap. | **Closed** |
| P2-2 | Whole-operation sealing and dependency accounting preserve a write-once route until every remote and prepared stream retires. | Re-traced A/B/C against `https_operation.rs:10-79`, `https_local.rs:36-67,173-177`, and `https_worker.rs:284-389`. Each `LocalRpc` and each admitted preparation owns a dependency; dropping A only cancels A. Sealing rejects new preparations, while B’s retained dependency prevents route deletion. `https_lifecycle_tests.rs:25-138` proves C cannot replace RA with RB and B’s POST still uses RA. | **Closed** |
| P2-3 | Actual HTTPS preparation failures pass through the existing mux as `OpenFailed`, including the first anonymous failure before a distinct Gh attempt. | `https_opening.rs:65-284` now admits a real `Open`, runs `Client::prepare_budget`, constructs `Opened` or `OpenFailed` from the actual result, and routes the receipt through endpoint and initiator muxes. `https_local.rs:113-158` uses that admitted session and stream identity for the subsequent stream. `https_opening_tests.rs:60-237` covers status, malformed HTTP, transport loss, trust, deadline exhaustion, the anonymous-to-Gh transition, exactly one receipt per attempt, and no active stream after failure. | **Closed** |
| P2-4 | Credential-offer facts accumulate across redirect attempts while status and authentication describe the current/final origin. | Re-traced the A-to-B redirect/helper-failure sequence through `https_worker.rs:316-450`. `credential_offered` is carried monotonically between redirect iterations and attached to helper, checkout, network, and policy failures; status/authentication are reset for the new origin. `https_opening_tests.rs:249-270` proves A receives the sentinel Authorization value, B’s helper fails before a request, and terminal `OpenFailed` reports `credential_offered=true`, absent status/authentication, and no secret or raw URL. | **Closed** |

## Changed-range analysis

The correction adds endpoint-scoped helper ownership, an owned-child guard, explicit operation dependency state, actual worker-to-mux opening composition, cumulative redirect facts, separate timing budgets, protocol-versus-I/O classification, lifecycle/opening/budget regressions, and the required enclosing conditional boundary. The changes remain within the accepted private H1 candidate.

I attacked the new cancellation boundary at three points: cancellation before helper admission, shutdown after the child has started, and task abort while the child is live. Each direction fails closed. A live child keeps its global permit until observed exited; endpoint shutdown either joins it or reports active/retained work. Closing endpoint semaphores prevents a retained client clone from initiating later credential effects.

I also checked the operation-sealing race. Existing `LocalRpc` owners retain dependencies before work begins, and successful preparations retain independent dependencies through stream completion. Calling `finish_operation` while any such owner exists seals the entry and rejects later preparation. The original A/B/C counterexample therefore cannot recur: B’s dependency prevents retirement after A drops. Reuse after all dependencies have intentionally retired represents a later operation lifetime; H2 still owns the higher-level sequencing and authority injection.

The opening path now uses the actual worker failure rather than a fabricated envelope. The permitted anonymous retry creates a distinct Gh `Open` under the same remaining budget, preserving the first typed failure. No failure path examined exposed a stream before `Opened`.

No changed-range defect or new architectural root cause was found.

## Evidence

The correction archive at `campaigns/https-integration/runs/2026-09-22-h1-rem1` is candid about compile-blocked attempts, partial test-first coverage, and the twelve-seed batch belonging to the initial tuple. I independently verified all 21 final-source hashes and all 19 artifact hashes in its manifest with zero mismatches.

Recorded final gates report:

- 64 endpoint tests passed;
- default core library check passed;
- scoped formatting passed;
- token-aware conditional-boundary inspection passed across 21 files;
- the original route and shutdown counterexamples were retained failing before correction.

Per the prompt, I did not rebuild or rerun the archived gates.

## Verdict and scope

**GO** for the corrected HTTPS H1 candidate on the State axis. All four original P2 findings are closed, and the changed range introduces no new finding.

This verdict does not accept H2 host/all-command integration, shared SSH/HTTPS authority injection by the higher host, Placement C cleanup-accounting closure, platform or selected-source qualification, real GitHub/account parity, production construction or activation, physical wire transport, release readiness, or performance.
