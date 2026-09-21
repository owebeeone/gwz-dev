# Phase4 PlacementB candidate aggregate — SURFACE-AXIS REVIEW

**Review object:** Phase4 PlacementB candidate aggregate at the exact tuple below; candidate documentation status; 2026-09-22  
**Baseline:** `. 93334058352828b1069b198d795c5860a395dc81`; `gwz-core 4f06384397a67d3dcae4856a93fd032499fda5dc`; `gwz-transport 03d3011b3ae9b8205bcf07f7f7862194af114856`; `taut bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli 7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs ce78628308e11b4e8901d5061602619109bce21a`; `libgit2 b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence a2180f71f9f4f16ecc639eecd25125b980ea54f3`. User-facing documentation bytes were compared between the prior reviewed core revision and this revision.  
**Date:** 2026-09-22  
**Axis:** Surface interface, lifecycle discoverability, naming, defaults, ownership and first-use walkthrough. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0/P1/P2/P3 findings; prior Surface verdict remains closed.

---

## 0. Evidence base

Read:

- `gwz-core/docs/TransportPlacement.md:1-324`
- `gwz-core/docs/Embedding.md:1-108`
- `gwz-transport/README.md:344-470`
- `dev-docs/GwzRemoteTransportPlacementB-RemPlan.md`
- Prior Surface report

The correction plan records no public API or schema change and no Surface finding. SHA-256 byte comparisons confirmed that `TransportPlacement.md` and `Embedding.md` are identical to the prior reviewed core revision. `gwz-transport` remains at the same pinned revision, so its README surface is unchanged.

The exact nine-repository tuple was verified before and after review. No tests or builds were run; this was the requested documentation-only confirmation.

## 1. Findings

No new findings.

| Prior Surface finding | Disposition | Closure |
|---|---|---|
| None | Initial Surface review was GO | Same documentation bytes and same user-facing lifecycle remain present at the correction-1 tuple. |

## 2. Invariant analysis

The previously tested surface invariants still hold:

- Placement is clearly an embedding lifecycle, with local placement as the default and no implied CLI command.
- `new`/`shutdown`, `install_cli`/`remove_cli`, `register_request`/`finish`, and `request`/`cancel`/`finish` remain discoverable lifecycle pairs.
- Defaults and failure behavior remain documented for placement, paths, identity selection, limits, deadlines and cleanup.
- Both forwarding directions, request ID ownership, port closure and ordering requirements remain explicit.
- The walkthrough covers capability checks, Bind/Bound, registration, use, cancellation, cleanup, removal and return to local operation.
- Candidate, lower-level mux and unreleased production status remain clearly separated.

The correction plan’s Code and State findings concern implementation behavior outside the Surface axis; they introduce no documentation regression or new architectural surface defect.

## 3. Risks and next action

Residual risk remains limited to the documented candidate status and deferred production qualification. No Surface correction is required. The next action is the existing release-gate qualification when deferred carrier and production activation work is complete.
