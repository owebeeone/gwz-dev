# GWZ Remote Transport HTTPS Design Correction 1 — CONSISTENCY-AXIS RE-REVIEW

**Review object:** Documentation-only correction at root `b905821b7b3fdf1b01607242dc9bdfd837b1a696`, core `2954e0b839ec4e6e78d30efc114de472d546ded6`, centered on `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md`; DRAFT correction 1, 2026-09-22.  
**Baseline:** root `b905821b7b3fdf1b01607242dc9bdfd837b1a696`; core `2954e0b839ec4e6e78d30efc114de472d546ded6`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Sources were read from immutable Git objects.  
**Date:** 2026-09-22  
**Axis:** Consistency against controlling requirements, designs, plan, checkpoint, and implemented protocol interfaces. Independent, adversarial, read-only. Current peer output was not read.

**Verdict: NO-GO** — all three original P2 findings are closed, but one directly introduced P2 message-lifecycle defect blocks design admission. I pre-commit to GO on a revision that resolves P2-4 as specified.

---

## 0. Evidence base

I read the merged remediation plan; the corrected HTTPS Design §§3–10; amended Requirements G4/C3/C8; Transport Design §§3.3, 6, 7, 9–11; Placement Design §§6–7; Plan Phase 5; baseline GWZ requirements/design summaries; the placement guide; checkpoint delta; and the pinned transport protocol, codec validation, mux routing, and pool API.

The nine commits and trees were verified at review start and end and did not move. Member worktrees were clean. Root contained only excluded generated review artifacts and retained old prompt noise; their current peer contents were not read. No build, test, probe, or write was performed.

## 1. Prior-finding closure

| Finding | Status | Exact closure evidence |
|---|---|---|
| P2-1 — anonymous discovery replay contradicted no-replay authority | **Closed** | Requirements C3 now expressly admits only once-only anonymous discovery 401/404→Gh with retained facts and cumulative budgets (`GwzRemoteTransportRequirements.md:213-222`). Placement Design §6 repeats the bounded exception and retains the POST/network-failure prohibition (`:322-329`). HTTPS §§4, 6, and 10 define cleanup-before-retry, first-receipt retention, final-only projection, exhausted-budget behavior, and no POST replay (`:114-137`, `:249-257`, `:417-420`). Transport Design §9 delegates this exact amendment. |
| P2-2 — HTTPS broadened `RepositoryRefused` beyond its authority | **Closed** | Placement Design §6 now defines a separate HTTPS final-discovery 403/404 predicate while preserving SSH’s canonical-refusal rule (`:314-329`). HTTPS §7 requires verified TLS and final headers, matching status facts, no delivered Git bytes, `Effect::None`, and excludes intermediate anonymous 404, POST, trust, helper, timeout, cancellation, loss, and malformed responses (`:301-331`). |
| P2-3 — redirect query conflicted with categorical query rejection | **Closed** | Requirements G4, Transport Design §3.3 and its acceptance matrix, both baseline summaries, and Plan Phase 5 now admit only the action-matching generated discovery query. HTTPS §§5/10 give one consistent grammar: caller URL and Taut destination remain query-free; exact raw single matching `service=` is allowed; duplicate, encoded, unrelated, empty, and fragment-bearing forms refuse (`:176-204`, `:423-429`). |

## 2. Findings

### [P2-4] Failed GET uses a stream-state message that is illegal before `Opened`

**Location:** HTTPS Design §3, especially `GwzRemoteTransportHttpsDesign.md:82-89`; pinned `gwz-transport/src/mux/routing.rs:231-247`; Placement Design `:301-305`.

The correction deliberately delays discovery GET `Opened` until final headers and route admission, then says: “A failed discovery may emit `Failed` before `Opened` with facts.” The frozen mux admits only `Opened` or `OpenFailed` from an endpoint while a route is `Kind::Opening`. `Failed` becomes legal only after `Opened` moves the route to `Kind::Stream`.

Concrete sequence: an `Open` for anonymous discovery creates an Opening route; the endpoint receives a final 404 and follows the corrected text by sending `Failed` without `Opened`; `validate_transition(Opening, Failed, from_endpoint)` returns `Protocol`. The core therefore cannot receive the retained typed first-attempt receipt required by the corrected authentication transition, nor the final 403/404 receipt required for HTTPS `RepositoryRefused`.

Replace `Failed` with `OpenFailed` for every discovery failure before `Opened`. `OpenFailed` already carries `Failure.facts` in v2, so this needs no field, tag, or state-machine amendment. H1 must cover final 401, 403/404, 5xx, malformed headers, trust/loss, and exhausted-budget discovery failures, asserting an accepted `OpenFailed` with the expected facts, no `Opened`, no stream-state transition, and a usable retained receipt for the permitted authentication transition.

## 3. Invariant analysis

The other changed-range attacks held. Delaying successful GET `Opened` avoids attributing an abandoned redirect connection and does not create the POST body cycle because discovery has no caller body; bounded response holding is explicit. HTTPS request-scoped credential offer on reused TLS is express in Requirements C3, Placement §6, Transport Design §10, HTTPS §10, and the public placement guide. The required validator change is narrowly bound to the validated HTTPS/Gh `Open`, preserves SSH refusal, and changes no schema.

The single aggregate SSH+HTTPS reservation authority in HTTPS §6 agrees with Transport Design §7.2’s per-host ceiling across schemes. The existing generic pool key already distinguishes schemes, while one driver or equivalent shared reservation owner can enforce common host/total counts. H1 must choose and test one authority; independent full-sized pools are expressly forbidden.

## 4. Risks and next action

Implementation, tests, native platform/source qualification, public constructors, production activation, physical carrier, and release remain properly deferred. Correct P2-4 in the design and add its focused H1 oracle before admitting implementation.
