# GWZ Remote Transport HTTPS design correction 2 — SAFETY-AXIS REVIEW

**Review object:** Documentation-only HTTPS design correction 2 at root `bcdca800ab19fb767f6e7d2ab8107f12dab48810`, core `2ea02835a15a9f56afdda43ccbcadec66b5b776e`  
**Baseline:** transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`  
**Date:** 2026-09-22  
**Axis:** Safety lifecycle confirmation, independent, adversarial, read-only, and current-peer-blind.

**Verdict: GO** — correction 2 preserves the prior Safety GO. No new Safety findings.

---

## 0. Evidence base

Read the correction-2 prompt and merged remediation plan; inspected only the delta from correction 1 in the HTTPS design, Phase 5 plan, root checkpoint, and the pinned transport protocol/mux lifecycle. No builds, probes, tests, or writes were performed.

The complete nine-repository tuple matched at both review boundaries. Member trees were clean; root contained only excluded generated prompts and old N2b prompt files.

## 1. Closure

The corrected lifecycle now matches the existing protocol exactly. While a discovery remains `Opening`, every pre-`Opened` outcome uses `OpenFailed` with optional `Failure.facts`. The pinned mux accepts only `Opened` or `OpenFailed` from the endpoint in this state; `OpenFailed` retires the route without entering `Stream`. `Failed` remains reserved for post-`Opened` stream failure.

The design applies this rule to 401, final 403/404, 5xx, malformed headers, trust failure, network loss, and exhausted budget. It forbids `Opened` and body exposure on those paths. This preserves the correction-1 guarantees: no manufactured connection observation, typed first-attempt facts remain available to the sole permitted anonymous-to-`Gh` transition, and terminal failure cannot leave an opening route stuck.

The added H1 real-mux oracle covers the corrected boundary and requires each class to produce accepted `OpenFailed`, retire `Opening`, avoid a `Stream` transition, and preserve the first receipt where authentication retry is permitted.

## 2. Risks and next action

This is documentary lifecycle closure only; no implementation or passing-test claim is made. The original Safety P2-1, P2-2, and P3-1 closures remain unchanged. Proceed under the existing H1/H2 implementation and deferred qualification gates.
