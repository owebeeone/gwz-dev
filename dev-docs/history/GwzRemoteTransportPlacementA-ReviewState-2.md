# GWZ Remote Transport Placement A correction 2 — STATE-AXIS REVIEW

**Review object:** Placement A correction 2: core `018176c6783d900570ba8f918aa744bd642fc3b6..28f667c0462c74798761ec9710de793c697c7fb8` and transport `d7051eede48f6cd8b07815edc9ced1916f82585c..03d3011b3ae9b8205bcf07f7f7862194af114856`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md`, dated 2026-09-22.  
**Baseline:** root `d20e168bb93cbe6bf5238b0b0295f0cd51f5cd20`; core `28f667c0462c74798761ec9710de793c697c7fb8`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `acdd2f98f395e51c60014c8faa82e315a48c0ada`. Sources were read from committed objects with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—changed-range admission, retained outcomes, terminal ownership, and fail-closed lifecycle behavior. Independent, adversarial, read-only. The current other-axis report was not read. Filed verbatim by the lane owner.

**Verdict: GO** — no open State finding and no new architectural root cause. The strengthened `BindRejected` admission preserves the previously accepted lifecycle proof.

---

## 0. Evidence base

I read the complete correction-two prompt, merged `GwzRemoteTransportPlacementA-RemPlan-2.md`, my filed round-one State report, the exact core and transport ranges, and the changed implementation and tests:

- `gwz-transport/src/codec/validate.rs`: centralized rejection-domain admission;
- `src/codec.rs`: crate-local validator exposure;
- `src/mux/mod.rs`: local endpoint configuration admission;
- `src/mux/routing.rs`: incoming and locally generated rejection paths;
- `tests/mux.rs`: complete rejection matrix, forbidden-operation failures, invalid configuration, and both permitted outcomes;
- `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md`: correction-two checkpoint annotation.

The permitted focused suite passed: `async_stream` 8/8, `mux` 26/26, `mux_async` 3/3, and `placement_v2` 10/10—47 executions total.

All nine tuple HEADs matched at both review boundaries. Member trees remained clean. Root noise was limited to the explicitly excluded generated prompts/reports and old N2b prompts.

## 1. Prior-finding closure

| Finding or proof obligation | Status | Closure evidence |
|---|---|---|
| State P2-1: `finish` could discard an admitted terminal and strand the peer | **Remains closed** | Correction two does not modify queue discard, terminal transfer, `finish`, cleanup deadlines, asynchronous ownership, or port-drop behavior. The focused mux suite reran the direct/async handoff, saturation, cancellation, timeout, and stalled-handoff regressions successfully. |
| Correction-two admission must preserve the accepted bootstrap lifecycle | **Closed** | Validation now precedes retained rejection state on both incoming and local construction paths. Forbidden states close as `Protocol` without `bootstrap_failure`, binding, action, or work; both admitted codes still traverse `Rejecting`, async handoff, closure, readiness, and exact retained-cause projection. |

## 2. Findings

None.

## 3. Invariant analysis

The changed range remains a closed state grammar:

- `validate::bind_rejection` admits only `UnsupportedVersion` or `UnsupportedOperation` with `Effect::None` and no facts. Codec admission, encoding, decoding, incoming mux routing, and local rejection construction share that rule.
- Incoming forbidden rejection states fail before `self.rejection` is assigned. The mux closes as a protocol failure and cannot invent an authoritative negotiation outcome, binding, cleanup action, or endpoint work.
- Structurally invalid local endpoint identity, trust ownership, capabilities, or limits fail during construction, before registration or bootstrap state exists.
- Both permitted rejection causes retain their exact code across `Rejecting`, port transfer, peer receipt, closure, and readiness. Late disconnect does not erase the first authoritative outcome.
- Tightening bootstrap admission does not touch the already accepted terminal-ownership machinery: admitted request terminals remain owned until outbound or local-action transfer, and stalled handoff still closes the generation at its nonrenewable deadline.
- Duplicate, cancellation, saturation, waiter, last-owner, and port-drop behavior therefore retains the round-one proof. No physical cleanup or carrier completion is inferred.

No new architectural root was introduced; the correction narrows one existing admission boundary.

## 4. Risks and next action

Batch B host/backend integration, Batch C carrier work, platform and selected-source qualification, production routes and capabilities, HTTPS, publication, and activation remain separate gates.

State-axis acceptance can proceed for Placement A correction 2 on this exact tuple.
