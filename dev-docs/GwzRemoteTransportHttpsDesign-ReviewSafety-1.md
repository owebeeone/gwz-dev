# GWZ Remote Transport HTTPS design correction 1 — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md` and controlling correction-1 amendments at core `2954e0b839ec4e6e78d30efc114de472d546ded6`, plus root checkpoint and `GwzRemoteTransportHttpsDesign-RemPlan.md` at root `b905821b7b3fdf1b01607242dc9bdfd837b1a696`; DRAFT documentation re-admission  
**Baseline:** transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Sources were read from immutable Git objects.  
**Date:** 2026-09-22  
**Axis:** Safety: degraded paths, replay and publication effects, credential disclosure, ownership, cancellation, bounded cleanup, and never-worse interleavings. Independent, adversarial, read-only; no current-round peer report was consulted.

**Verdict: GO** — all three original Safety findings are closed and no new Safety finding was established in the changed range.

---

## 0. Evidence base

Read the generated correction prompt, merged remediation plan, full corrected HTTPS design, and changed ranges from the original root/core pins. Checked the amended remote-transport requirements, transport design, placement design, plan, baseline GWZ summaries, public placement guide, and root checkpoint. Rechecked the existing `Open`, `Opened`, and `Facts` shapes and the controlling observation and pooling requirements.

The exact nine-repository tuple was verified at both review boundaries. Member trees were clean. Root contained only excluded current generated prompts/report and four old N2b prompts. No builds, probes, implementation tests, or file writes were performed. The core documentation correction passes whitespace-diff inspection; no implementation result is inferred.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
| --- | --- | --- |
| Safety P2-1 — redirect continuity lacked per-remote correlation | **Closed** | HTTPS §5 now defines a bounded, write-once route per operation/original destination/service family. Admission is capped at 64 before helper/network effects; compare/install precedes `Opened` and body exposure; conflicting bases fail as Protocol before advertisement delivery or POST; stream retirement cannot erase or overwrite the route; only whole-operation retirement clears it after dependents retire. The required barrier test reproduces the original two-remote interleaving and checks conflict refusal plus cancellation isolation. |
| Safety P2-2 — authentication retry contradicted C3 | **Closed** | Requirements C3, Placement §6, HTTPS §§4/6/10, and the plan now admit exactly one anonymous discovery 401/404-to-`Gh` transition. The first terminal receipt is retained privately, cleanup completes first, budgets remain cumulative, and only the final attempt is publicly projected. POST, network-failure, status-class, and generic GET replay remain forbidden. Required tests count requests/helper calls for the original challenge and prohibited-retry cases. |
| Safety P3-1 — HTTP terminal-status behavior was open | **Closed** | HTTPS §7 supplies a closed status policy: bounded informational responses; 200-only success; explicit redirect, authentication, repository-refusal, proxy, remaining 4xx/5xx, malformed, and out-of-range handling; no implicit retries; every terminal non-success connection is discarded. The H1 matrix covers representative classes, effect, disposal, request count, and redaction. |

## 2. Changed-range analysis

The final-connection observation amendment holds under redirect and failure attacks. Discovery GET needs no caller body, so delaying `Opened` until final validated headers and route admission introduces no POST-style cycle. Redirected connections are discarded before the next acquisition, response buffering remains bounded, and `Opened.connection_id` identifies the connection that actually serves the advertisement. Pre-`Opened` failure uses typed `Failed` facts without inventing a connection observation.

The HTTPS reuse amendment remains narrowly bound. `credential_offered` changes only when Authorization enters the HTTP send path; `authenticated` remains nullable on ordinary HTTP success. The validator must derive permission from the already validated `Open` being HTTPS/`Gh`, so claimed facts cannot widen policy. SSH retains its existing `reused && credential_offered` refusal. Unchanged peers remain SSH-only until capability advertisement.

The shared-capacity amendment prevents scheme multiplication of endpoint limits. HTTPS and SSH must use one pool driver or a shared reservation authority with identical aggregate host/total accounting; separate physical owners cannot each receive full ceilings. Mixed-scheme saturation is an explicit H1 acceptance obligation.

The added final HTTPS `RepositoryRefused` predicate is fail-closed: only final discovery 403/404 over verified TLS, valid headers, no delivered Git bytes, matching status, and `Effect::None` qualify. Intermediate anonymous 404, POST responses, trust/helper/loss/timeout/cancellation, malformed responses, and uncertain publication cannot enter suppression. Exact generated-service redirect queries likewise remain action-matched and exclude arbitrary query, fragment, encoding, or caller-supplied destination data.

## 3. Invariant analysis

The original wrong-route POST is replaced by bounded conflict refusal; cancellation cannot transfer or erase authority. Authentication transition cannot replenish budgets or hide a POST/network failure. Credentials remain endpoint-local and origin-scoped, and redirect traversal never copies Authorization. Receive-pack becomes `Effect::Possible` before its first request byte and is never replayed. Terminal errors dispose of connections; retained helper/network cleanup stays accounted. The aggregate pool prevents SSH plus HTTPS from multiplying endpoint resource ceilings.

## 4. Risks and next action

Behavior remains unimplemented and untested at this documentation gate. Platform and selected-source qualification, public constructors and activation, physical wire/iroh, and release remain deferred. Proceed to H1 only after the parallel documentation gates finish, retaining every causal and changed-range test named by this correction.
