# HTTPS H2 correction 1 — CODE-AXIS REVIEW

**Review object:** H2 correction-1 diff `gwz-core` `1d3a759acdcc0c977e24199e92f98cf8ca59a129..7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc`, including `dev-docs/GwzRemoteTransportHttpsH2.md`; corrected implementation candidate, not activated  
**Baseline:** root `e97f79e9667c9d31871d3d9210651909eb6a4255`; `gwz-core` `7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc`; `gwz-transport` `aa40936d0805e8cb60f8027615abe20d4f2045e4`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `acd7e4f15f97be9ac4b6414a6e1099c932548df9`. Sources were read from committed objects with `git show`. All nine HEADs matched at both review boundaries.  
**Date:** 2026-09-22  
**Axis:** Code — architecture, interfaces, call graphs, ownership, compatibility, and failure reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — both original P2 findings close, but one new P2 deadline-accounting root blocks. I pre-commit to GO on a revision that resolves P2-3 as specified with a focused regression.

---

## 0. Evidence base

I read the correction prompt, merged remediation plan, original Code report, corrected H2 checkpoint, and focused diff. I traced:

- `transport_host/{request,session,session/driver,https_endpoint}.rs`
- `git/endpoint/{https_worker,https_remote}.rs`
- `git/gitbackend/transport_binding.rs`
- the corrected policy, cancellation, real-mux, worker, and remote tests
- HTTPS Design §§4, 6–8 and its cumulative-budget clauses.

I inspected the correction evidence README, logs, and `final-sources.json`; all 23 recorded source fingerprints match the corrected commit. The archive records host 50, endpoint 69, observations 3, binding 2, default check, formatting, and conditional-boundary checks passing, while candidly retaining intermediate failed attempts. I ran no build or test.

## 1. Findings

### Prior-finding closure

| Prior finding | Status | Closure evidence |
|---|---|---|
| Code P2-1 — retry budgets lacked attempt correlation | **CLOSED** | `request.rs:115-233` canonicalizes the route and holds one route gate across cache lookup and the complete Anonymous→Gh transition. Mode is fixed per request/route, preventing an explicit or cached Gh Open from consuming orphaned state. `https_endpoint.rs:204-213,311-329` carries the retained budget only to the next same-key Gh; `https_worker.rs:339-347,549-560` rejects exhausted domains before work and deducts first-attempt cleanup. The complete anonymous `Failure`, stream ID, and policy remain on the per-remote adapter. Regressions exercise overlapping order `Anonymous,Gh,Anonymous,Gh`, explicit-policy isolation, retained receipts, and exhausted domains. |
| Code P2-2 — pending HTTPS cancellation reported `Effect::Possible` | **CLOSED** | `session/driver.rs:109-168` assigns HTTPS opening cancellation `Effect::None`; `session.rs:365-403` uses it for the bounded local wake. `https_endpoint.rs:122-150,267-357,488-523` keeps POST prepared but unstarted until the first `Data`/`EndWrite`, and pre-publication cancellation emits `OpenFailed/Cancelled/None`. `Wait::complete_with` and the endpoint handoff hooks suppress a late success observation when cancellation wins. Focused tests cover pending discovery, pre-send receive-pack with zero POST requests, queued Opened replacement, sibling survival, and eventual cleanup. |

### [P2-3] NEW ARCHITECTURAL ROOT — the serialization wait replenishes the allocation deadline

`src/transport_host/request.rs:125-141` gives the new route gate its own fixed 30-second wait. After acquiring it, `session/driver.rs:117-123` creates an `Open` with a fresh 30-second allocation allowance; only then does `https_worker.rs:339-413` begin deducting allocation time.

Concrete sequence: caller A holds the same canonical-route gate during a slow opening transition. Caller B waits 29 seconds, acquires the gate, and then receives a fresh 30-second endpoint allocation allowance. B may therefore spend nearly 59 seconds in admission/allocation despite the existing 30-second domain. If A holds the gate slightly longer, B instead gets a locally manufactured timeout without an endpoint Open. The new backpressure stage is neither charged to an existing deadline nor defined as a separate configured domain.

This violates HTTPS Design §6’s requirement to apply the existing allocation deadlines and never replenish them under backpressure. It also makes timeout behavior depend on an undocumented private constant rather than the request’s carried deadline.

Start the allocation deadline before route-gate contention. Bound the gate by that deadline and pass only the remaining allocation allowance into the first Open; the endpoint can continue carrying that remainder across the authenticated continuation. No public schema change is needed.

Closure requires a deterministic short-budget regression: hold the route gate for a known portion of the allowance, release it, then force endpoint allocation to wait. The second caller must time out within the original total allowance, issue no Open once the allowance is exhausted, and never receive a fresh full allocation interval.

## 2. Invariant analysis

The original retry interleaving no longer reproduces: canonical-equivalent callers cannot interleave their two-Open transitions, explicit policy switching fails before effects, and cached Gh cannot consume a retained anonymous budget from another mode. Cleanup and zero-valued domains remain causal, and typed first-attempt context survives final failure mapping.

Cancellation now has one coherent linearization boundary. Cancellation before mux publication replaces Opened; cancellation after publication reaches the stream; a losing success completion cannot emit an observation. POST preparation alone performs no HTTP request, so pre-send receive-pack cancellation remains `Effect::None`. SSH retains its conservative pre-open classification.

The correction adds no public constructor, protocol field, carrier, or fallback. Negotiated limits, mux ownership, per-remote callback isolation, and deferred activation/platform qualification remain intact. The only failed invariant is accounting for time spent at the newly introduced host-side route gate.

## 3. Risks and next action

Platform and selected-source qualification, public activation, physical carrier work, external accounts, system TLS/proxy parity, and release remain valid deferrals.

Charge route-gate contention to the existing allocation deadline, add the focused timing/call-count regression, refresh the exact source/evidence tuple, and request a narrow Code closure review of P2-3.
