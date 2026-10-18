# HTTPS design correction 2 — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md` with correction-2 amendments; documentation-only re-admission at the exact tuple below; 2026-09-22  
**Baseline:** `. bcdca800ab19fb767f6e7d2ab8107f12dab48810`; `gwz-core 2ea02835a15a9f56afdda43ccbcadec66b5b776e`; `gwz-transport 03d3011b3ae9b8205bcf07f7f7862194af114856`; `taut bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli 7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs ce78628308e11b4e8901d5061602619109bce21a`; `libgit2 b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`.  
**Date:** 2026-09-22  
**Axis:** Surface confirmation of unchanged public HTTPS observation semantics. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0/P1/P2/P3 findings.

---

## 0. Evidence base

Read-only tuple verification succeeded before and after review.

The permitted public documents were compared byte-for-byte with the prior reviewed correction-1 revision:

- `gwz-core/docs/TransportPlacement.md` — identical SHA-256.
- `gwz-core/docs/Embedding.md` — identical SHA-256.
- `gwz-transport` remained at the same pinned revision.

Therefore the documented distinctions remain unchanged: SSH reuse reports no renewed credential offer while preserving previously proven authentication; planned HTTPS may offer credentials on a reused TLS connection, and public HTTP success does not prove account authentication.

No implementation, design document, peer report, build or test was inspected or run.

## 1. Findings

No findings. The correction range introduces no public Surface change.

## 2. Invariant analysis

The prior Surface GO remains valid. HTTPS remains clearly future-only, with no new public constructor, flag, route or activation claim. The exposed meanings of `reused`, `credential_offered` and `authenticated` remain transport-specific and do not conflate TLS connection reuse with account authentication.

## 3. Risks and next action

No Surface action is required. Future HTTPS implementation and activation reviews must preserve these documented observation semantics.
