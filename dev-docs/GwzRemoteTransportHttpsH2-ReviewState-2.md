# HTTPS H2 CORRECTION 2 — STATE-AXIS REVIEW

**Review object:** H2 correction 2, `gwz-core 7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc..c92abc4110fc7c1ef89600118284724c942f8985`, including `gwz-core/dev-docs/GwzRemoteTransportHttpsH2.md`; correction candidate, not accepted or activated.  
**Baseline:** root `2380a234bf620bacf73a1924f4ac23000385f758`; core `c92abc4110fc7c1ef89600118284724c942f8985`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `3302b5d03f56590a6d521b1b52db302861775e15`. Sources were read from immutable commits with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—deadline state, races, cancellation, fail-closed admission and retained ownership. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — Code P2-3 is closed, the prior State GO remains valid, and no new P0–P3 finding or architectural root was found.

---

## 0. Evidence base

I read the correction-2 prompt and remediation plan, prior correction reports, the four-file core delta, revised H2 checkpoint, new budget-gate tests, and committed correction-2 evidence.

Focused inspection covered `transport_host/request.rs:91-268,408-442`, `session/driver.rs:40-190`, and `https_budget_gate_tests.rs:1-251`. I traced the unchanged endpoint retry-budget consumption and cancellation paths needed to check interactions.

All 24 archived final source fingerprints match the reviewed core commit. `git diff --check` passed. I ran no build or test. Committed evidence records the runtime red at 1.624 seconds against a one-second budget and the corrected host 52, endpoint 69, observations 3, binding 2, default check, formatting and conditional-boundary gates passing.

All nine repository heads matched the exact tuple at both review boundaries. Member trees were clean; root contained only the current generated prompts and expressly excluded old N2b prompts.

## 1. Prior-finding closure and changed-range analysis

| Finding | Status | Evidence |
|---|---|---|
| State P2-1 — cancellation crossed queued `Opened` | **Remains closed** | Correction 2 does not modify endpoint output, pending-handoff, observation or cleanup ownership. |
| State P2-2 — retry lacked causal identity and budget ownership | **Remains closed** | Canonical-route serialization, fixed route mode, retained first receipt and endpoint-owned continuation budget are unchanged. |
| Code P2-3 — route-gate waiting replenished allocation | **Closed** | `open_https_recording_with_allocation` starts one absolute allocation deadline before validation, canonicalization and route contention. The gate repeatedly checks cancellation and expiry. After acquiring the session mutex, `open_stream` computes the positive millisecond remainder; zero or sub-millisecond remainder returns typed Timeout before `owner.open`. Only that remainder enters the first Open. The Gh continuation passes no fresh host deadline because the endpoint removes and shortens its retained first-attempt budget. Helper, connect, network and cleanup domains remain independent. |

The short-budget regression holds one canonical-route transition for 600 ms, then forces the second caller to wait on a real one-slot physical pool. It observes a carried Open allowance below 500 ms and requires typed Timeout within the original one-second allowance plus scheduling margin. The exhausted-gate case permits only the first Open and verifies the second fails locally. The archived pre-fix run exceeds 1.6 seconds; the final run passes both cases.

## 2. Invariant analysis

Allocation now begins before the correction-1 serialization stage and cannot be reset by route or session-lock contention. Flooring the transmitted remainder is conservative: a sub-millisecond allowance fails closed instead of becoming protocol zero and recovering endpoint defaults.

Cancellation while waiting still wins through the route-loop liveness check. Cancellation between the anonymous failure and Gh continuation still prevents the second Open. If the continuation proceeds, the endpoint’s retained budget remains authoritative; the new default used by the host call cannot enlarge it because endpoint shortening preserves the smaller remainder.

The delta adds no protocol field, public API, carrier, authentication path or resource owner. It leaves the established Opened/cancellation linearization, POST effect boundary, first-receipt attribution, shared reservation accounting and cleanup reporting unchanged.

## 3. Risks and next action

Physical carrier timing, platform and selected-source qualification, public activation, real-account testing, proxy parity and release remain deferred and are not certified here.

Accept correction 2 on the State axis and complete the H2 acceptance record after the independent current-round verdict.
