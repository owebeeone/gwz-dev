# GWZ Remote Transport HTTPS Design Correction 2 — CONSISTENCY-AXIS RE-REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md` at core `2ea02835a15a9f56afdda43ccbcadec66b5b776e`; documentation-only correction 2, 2026-09-22.  
**Baseline:** root `bcdca800ab19fb767f6e7d2ab8107f12dab48810`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Immutable Git objects were used.  
**Date:** 2026-09-22  
**Axis:** Focused Consistency closure of P2-4 and its changed range. Independent, adversarial, read-only; no current peer output was read.

**Verdict: GO** — P2-4 is closed; no new findings.

---

## 0. Evidence base

I read the correction-2 prompt and merged remediation plan; core diff `2954e0b839ec4e6e78d30efc114de472d546ded6..2ea02835a15a9f56afdda43ccbcadec66b5b776e`; the revised HTTPS Design §3 and correction-2 lifecycle clause; the Phase 5 status-only plan delta; root checkpoint delta; and pinned transport mux transition and codec rules.

The complete nine-repository commit/tree tuple was verified unchanged at start and end. Member worktrees were clean; only excluded generated prompts remained at root. No tests, builds, probes, or writes were performed.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-4 — pre-`Opened` discovery used illegal stream-state `Failed` | **Closed** | HTTPS Design §3 now requires `OpenFailed` with v2 `Failure.facts` for every discovery failure before `Opened`. The correction-2 clause covers 401, 403/404, 5xx, malformed headers, trust, loss, and exhausted budgets; forbids `Opened` and body exposure; and preserves `Failed`/`Closed` only after entry to Stream. This exactly matches `mux/routing.rs`: `Kind::Opening` admits `Opened` or `OpenFailed`, `OpenFailed` retires the route, and `Failed` remains stream-only. |

## 2. Invariant analysis

The required H1 oracle is satisfiable: each listed case traverses the real mux, verifies accepted `OpenFailed`, observes Opening retirement without Stream transition, and retains the typed first receipt used by the bounded anonymous-to-Gh transition. `Failure.facts` is already legal on v2 `OpenFailed`; no field, tag, API, or state-machine change is implied.

The two-file core delta does not alter the previously closed retry, repository-refusal, redirect-query, observation, or aggregate-pool clauses.

## 3. Risks and next action

This verdict admits the corrected design only. Implement H1/H2 and their stated real-mux oracle before advertising HTTPS; implementation, platform/source qualification, activation, carrier, and release evidence remain separate gates.
