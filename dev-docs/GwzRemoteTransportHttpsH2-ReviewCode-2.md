# HTTPS H2 correction 2 — CODE-AXIS REVIEW

**Review object:** H2 correction-2 diff `gwz-core` `7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc..c92abc4110fc7c1ef89600118284724c942f8985`, including `dev-docs/GwzRemoteTransportHttpsH2.md`; corrected implementation candidate, not activated  
**Baseline:** root `2380a234bf620bacf73a1924f4ac23000385f758`; `gwz-core` `c92abc4110fc7c1ef89600118284724c942f8985`; `gwz-transport` `aa40936d0805e8cb60f8027615abe20d4f2045e4`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `3302b5d03f56590a6d521b1b52db302861775e15`. Sources were read from committed objects with `git show`. All nine HEADs matched at both review boundaries.  
**Date:** 2026-09-22  
**Axis:** Code — architecture, interfaces, call graphs, ownership, compatibility, and failure reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — P2-3 closes; no P0–P3 finding remains in this focused changed range, and no new architectural root was found.

---

## 0. Evidence base

I read the correction-2 prompt and remediation plan, the prior Code re-verdict, the corrected H2 checkpoint, HTTPS Design §6, and the complete four-file correction:

- `src/transport_host/request.rs`
- `src/transport_host/session/driver.rs`
- `src/transport_host/https_budget_gate_tests.rs`
- `dev-docs/GwzRemoteTransportHttpsH2.md`

I inspected the correction-2 evidence README, command records, runtime red/green logs, and final source manifest. All 24 archived source fingerprints match the committed core object. The archived runtime counterexample took 1.624 seconds against a one-second allowance; the corrected gate records host 52, endpoint 69, observations 3, binding 2, default check, formatting, and conditional-boundary checks passing. Diff whitespace inspection passed. I ran no build or test.

## 1. Findings

### Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Code P2-3 — route serialization replenished allocation time | **CLOSED** | `request.rs:136-167` starts one absolute allocation deadline before canonical-route lookup and contention, rechecks cancellation and expiry while waiting, and retains the deadline through first admission. `session/driver.rs:118-158` rechecks after acquiring the session mutex, floors the remaining duration, rejects zero or sub-millisecond allowance before `owner.open`, and carries only a positive remainder in `Open.deadlines.allocation_ms`. The Gh continuation does not restart host admission accounting; the endpoint continues using its retained cumulative retry budget. The synchronized one-slot-pool regression observes a reduced second Open allowance and bounds route plus pool waiting within the original interval. The exhausted-gate regression observes no second Open. |
| Code P2-1 and P2-2 | **CLOSED, retained** | Correction 2 does not change retry correlation, receipt ownership, cancellation linearization, or POST-start behavior. Their correction-1 closures remain applicable. |

No new findings.

## 2. Invariant analysis

The original P2-3 sequence now fails: a caller waiting behind another same-route transition cannot acquire the gate and then receive a fresh full allocation interval. Time spent at the route gate and session mutex is deducted before the first Open is admitted. Expiry produces typed `Timeout/Effect::None` without allocating a stream or emitting an Open.

Zero cannot regain default meaning because it is rejected before `owner.open`; only positive remaining milliseconds cross the existing protocol field. The endpoint still owns cumulative Anonymous→Gh budget accounting, while helper, connect, network, and cleanup domains remain independent. Production retains the prior 30-second allocation default, and the SSH path remains unchanged.

The observer and adjustable allowance are test-only private seams. No public constructor, protocol field, schema, carrier, fallback, or activation surface changes.

## 3. Risks and next action

The focused tests use scheduling margin, but they also inspect the carried allowance and Open count, so their conclusion does not depend solely on wall-clock timing.

Platform and selected-source qualification, public activation, physical carrier work, external accounts, system TLS/proxy parity, and release remain deferred. Record the retained review acceptance at this exact tuple and proceed to the next separately gated checkpoint.
