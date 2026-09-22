# HTTPS H2 CORRECTION 1 — STATE-AXIS REVIEW

**Review object:** H2 correction 1, `gwz-core 1d3a759acdcc0c977e24199e92f98cf8ca59a129..7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc`, including `gwz-core/dev-docs/GwzRemoteTransportHttpsH2.md`; status **candidate correction, not accepted or activated**.  
**Baseline:** root `e97f79e9667c9d31871d3d9210651909eb6a4255`; core `7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `acd7e4f15f97be9ac4b6414a6e1099c932548df9`. Sources were read from immutable commits with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—state machines, races, fail-closed behavior, cancellation, cleanup and recovery. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both original State P2 findings are closed. No new P0–P3 finding or new architectural root was found in the changed range.

---

## 0. Evidence base

I read the correction prompt, merged remediation plan, prior State report, corrected H2 document, and the complete core correction range. Focused source inspection covered:

- `transport_host/https_endpoint.rs:113-237,239-525`;
- `transport_host/request.rs:74-240,336-375`;
- `transport_host/session.rs:65-115,365-430`;
- `transport_host/session/driver.rs:76-175,217-493`;
- `git/endpoint/https_worker.rs:330-365,531-562,630-880`;
- `git/gitbackend/transport_binding.rs:108-203`;
- the new cancellation, mux-backpressure, retry-policy, budget and worker tests;
- the unchanged stream flush and lease-drop machinery in `gwz-transport`;
- committed correction evidence, including red attempts, final logs, source fingerprints and claim limits.

All 23 archived source fingerprints match the reviewed core commit. `git diff --check` passed. I did not execute builds or tests. The committed evidence records host 50, endpoint 69, observations 3, binding 2, default library check, formatting and conditional-boundary checks passing.

The exact nine-repository tuple matched at both review boundaries. All member trees were clean. Root contained only the two current generated prompts and four expressly excluded old N2b prompts.

## 1. Prior-finding closure and changed-range analysis

| Prior finding | Status | Closure evidence |
|---|---|---|
| State P2-1: cancellation can cross queued `Opened` | **Closed** | `cancel_entry` replaces an unhanded queued `Opened` with `OpenFailed(Cancelled, None)`. If `Opened` has moved into the driver’s blocked pending slot, `before_handoff` rechecks cancellation on every send attempt and rewrites that exact pending message. `handed_off` marks publication only after successful mux acceptance. A cancellation after publication uses the stream terminal path. `Wait::complete_with` makes the opened observation conditional on winning the blocking waiter, suppressing late success callbacks after local cancellation. Preparation and serving handles remain owned; dropping an unstarted `Prepared` discards its lease through `Lease::drop`, and pool closing remains counted. The direct queued-output and real mux `WouldBlock` regressions require one cancellation terminal, no opened callback, surviving siblings and eventual cleanup zero. |
| State P2-2: two-Open authentication retry lacks causal identity | **Closed** | `RequestContext::open_https_recording` canonicalizes the URL and holds a per-request/per-route mutex across cache selection and the complete Anonymous→Gh pair. Same-route pairs therefore cannot interleave. The route’s helper mode is immutable; explicit policy switching fails before effects. The endpoint’s retained retry budget can only be consumed by the serialized continuation, is removed once, and is retired with the request. Zero allocation, helper, connect, network or cleanup allowances fail before new helper or connection work, and first-attempt cleanup is deducted from the carried budget. `HttpsAttemptReceipt` retains the complete first `Failure`, stream identity and policy beside the final failure and in the per-remote adapter. Tests exercise the former `[Anonymous, Anonymous, Gh, Gh]` race and now require `[Anonymous, Gh, Anonymous, Gh]`, explicit/cached isolation, complete receipts and no refill of exhausted domains. |

The changed POST-start boundary also holds. Exchange preparation retains `Prepared` without starting HTTP. The first `Data` or `EndWrite` is delivered losslessly before the serving task starts. Cancellation before that boundary emits `Cancelled/None`, drops the prepared lease as discarded and performs no POST. A zero-offset `Flush` does not deadlock: the unchanged stream machine acknowledges it locally because the endpoint sink offset is already zero; after data, ordered `Data` starts serving before its following flush can complete. Once receive-pack serving starts, `Effect::Possible` is set before `send_request`.

## 2. Invariant analysis

The original queued-output and blocked-mux interleavings no longer permit a canceled opening to publish success. The endpoint distinguishes receipt ownership from mux publication, retries the cancellation check at the actual handoff boundary, and retains physical cleanup ownership regardless of which side wins.

The retry correction creates one private causal owner without changing the frozen message shape. Equivalent canonical URLs share the gate; unrelated explicit policy cannot borrow retained state; cancellation while waiting for the gate or between attempts revalidates request liveness. Final public observations remain final-attempt-only while the private adapter retains the first receipt.

Attacks on first-body loss, leading flush, request cancellation during route contention, prepared-resource abandonment, sibling survival and zero-budget continuation found no reachable invariant violation. Shared reservation accounting and existing fail-closed lease disposal remain intact.

## 3. Risks and next action

The per-route serialization and fixed opening wait are private candidate restrictions. Physical carrier, platform and selected-source qualification, public activation, real-account tests, proxy parity and release remain deferred and are not certified here.

Accept correction 1 on the State axis and proceed with the remaining planned H2 acceptance gate.
