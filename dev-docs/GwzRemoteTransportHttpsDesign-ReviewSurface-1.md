# HTTPS design correction 1 — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md` plus correction-1 controlling amendments and user-facing documentation at the exact tuple below; documentation-only design re-admission; 2026-09-22  
**Baseline:** `. b905821b7b3fdf1b01607242dc9bdfd837b1a696`; `gwz-core 2954e0b839ec4e6e78d30efc114de472d546ded6`; `gwz-transport 03d3011b3ae9b8205bcf07f7f7862194af114856`; `taut bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli 7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs ce78628308e11b4e8901d5061602619109bce21a`; `libgit2 b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Read only the permitted user-facing documentation.  
**Date:** 2026-09-22  
**Axis:** Surface meaning of `reused`, `credential_offered` and `authenticated` for SSH and planned HTTPS. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0/P1/P2/P3 findings.

---

## 0. Evidence base

Read:

- `gwz-core/docs/TransportPlacement.md:78-117`
- `gwz-core/docs/Embedding.md:104-108`
- `dev-docs/GwzRemoteTransportHttpsDesign-PromptSurface-1.md`

The documentation states that current client placement supports SSH and that HTTPS is planned rather than currently advertised. The observation section explicitly distinguishes:

- SSH reuse: `credential_offered` is false on reuse, while `authenticated` may reflect previously proven connection authentication.
- HTTPS reuse: `reused` describes the TLS connection, while GitHub credentials may be offered again, so `reused` and `credential_offered` may both be true.
- HTTPS authentication: a successful public HTTP response does not prove the accepted account; `authenticated` remains unknown without independent proof.

The exact tuple was verified before and after review. No builds, probes or implementation inspection were performed.

## 1. Findings

No open findings.

The prior ambiguity attack failed: the documentation does not equate TLS connection reuse with credential reuse, does not claim public HTTP success proves account authentication, and preserves SSH’s stronger authenticated-connection semantics. It also clearly labels HTTPS behavior as a future candidate and makes no new constructor, flag or activation claim.

## 2. Invariant analysis

The exposed observation meanings are internally consistent:

- `reused` identifies connection reuse, not credential reuse.
- `credential_offered` describes whether credentials may have been presented for the current request.
- `authenticated` is only asserted where the transport has independent proof; HTTPS public responses leave it unknown.
- SSH and future HTTPS are described separately, preventing SSH’s connection-authentication guarantee from being incorrectly generalized to HTTP.

The surrounding placement documentation continues to present HTTPS as unavailable in current builds, so the amended semantics do not create a user-facing route or lifecycle that does not exist.

## 3. Risks and next action

Residual risk is implementation conformance when HTTPS is eventually activated, especially preserving unknown authentication state for public responses. No Surface correction is required. The next action is to retain these observation definitions as the contract for the later implementation and activation review.
