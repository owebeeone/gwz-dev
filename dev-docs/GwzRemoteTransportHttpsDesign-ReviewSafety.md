# GWZ Remote Transport HTTPS design — SAFETY-AXIS REVIEW

**Review object:** `GwzRemoteTransportHttpsDesign.md` at core `bcc2ebf6f6652d4f37429c5cbb6e68f987006b94`; documentation-only design admission, 2026-09-22  
**Baseline:** root `c17b93b1e6197b1f40e2bb7f25ade8c478971c64`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Sources read from immutable Git objects.  
**Date:** 2026-09-22  
**Axis:** Safety: degraded paths, replay and publication effects, credential isolation, ownership, cancellation, bounded cleanup, and never-worse interleavings. Independent, adversarial, read-only; no peer report was consulted.

**Verdict: NO-GO** — two P2 findings block design admission; one P3 requires closure. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified and disposes P3-1.

---

## 0. Evidence base

Read the full HTTPS design, its Phase 5 plan pointer and root checkpoint; controlling requirements, transport design, placement design and plan; `gwz-transport` protocol/policy interfaces; core concurrent host fan-out and stream-wrapper paths; and relevant git2 smart-transport interfaces. No builds or probes were permitted or run.

The exact nine-repository tuple was verified at both review boundaries. Member trees were clean. Root contained only the six prompt files explicitly excluded by the review object.

## 1. Findings

### [P2-1] Redirect continuity lacks a per-remote correlation key

**Location:** HTTPS design §§3 and 5, lines 63–68 and 175–180; `gwz-transport/src/protocol.rs` `Open`, lines 689–699.

The design gives each smart RPC a fresh stream and says per-remote state owns its current RPC, but stores the effective redirect route by only registered operation, original destination, and service family. `Open` carries no remote/subtransport instance identifier.

A single operation can concurrently process two remotes with the same HTTPS destination and service family. If their discovery GETs legitimately resolve to different effective bases, the later record overwrites the earlier one. The first remote’s subsequent POST can then use the second remote’s route. For receive-pack this can publish to the wrong redirected target; for reads it breaks the required discovery-to-exchange continuity. Cancellation or completion of either remote can also expire shared route state needed by the other.

Route ownership must be correlated to one smart-subtransport/remote instance across its advertisement and exchange sequence. Add an unambiguous private correlation to each open, or define safe serialization that rejects conflicting redirect outcomes. If this requires a protocol/interface amendment, admit that later gate explicitly; no public freeze is required here.

**Closure test:** barrier two same-operation, same-URL remotes whose discoveries return distinct bases; interleave their POSTs and cancellation. Each POST must use its own base, and closing one sequence must not erase the other’s route.

### [P2-2] The authentication retry contradicts the controlling no-replay requirement

**Location:** HTTPS design §4, lines 106–115, and §6, lines 214–218; requirements C3, lines 210–214.

C3 says that after transmission begins, failure must surface without automatic replay of the Git exchange. The design instead transmits anonymous discovery, suppresses a 401 or 404, and automatically sends the GET again with `Gh`. “No transparent retry is required, even for GET” also leaves other GET retries permitted rather than forbidden. As written, authority and candidate design prescribe different degraded behavior.

This creates incompatible implementation and acceptance oracles and expands origin disclosure/helper invocation after a completed anonymous request. Amend the controlling requirement with an exact bounded exception, or remove the retry and choose authentication before transmission. Any exception must be limited to discovery GET 401/404, once, when helper policy permits, under cumulative budgets, with no intermediate result publication and no retry for transport errors, other statuses, or POST.

**Closure test:** count HTTP requests and helper calls for 401, 404, 5xx, connection loss, and POST challenge; only the expressly authorized cases may issue a second request.

### [P3-1] HTTP terminal-status behavior is not closed

**Location:** HTTPS design lines 82–87 and 258–264.

The design validates status and content type and maps selected failures, but does not define handling for 1xx, 204/other 2xx, non-followed 3xx, 407, 408/409/410/413/429, or 5xx. Combined with the permissive GET-retry wording, implementers can diverge on retry, reuse, error code, and retained status.

Specify accepted success statuses per action, redirect statuses for discovery only, and typed terminal handling for every remaining status class, with no implicit retry and conservative receive-pack effect preservation.

**Closure test:** representative status matrix covering 1xx, 204, 3xx, 407, 429, and 5xx, asserting error class, reuse/disposal, retry count, effect, and redaction.

## 2. Invariant analysis

The remaining attacked paths held: POST redirects and retries are forbidden; receive-pack becomes `Effect::Possible` before its first network byte and retains it through late failure; anonymous requests do not invoke `gh`; credentials are request-scoped, freshly obtained, bounded, redacted, and never copied across origins; proxy and origin authentication remain separate; connection leases are exclusive and reusable only after complete framed EOF and healthy sender state; cancellation and incomplete bodies force disposal; body flow follows transport credit with independent allocation limits; unsupported trust/proxy policy refuses before origin credential access; helper children are bounded, killed, reaped, and accounted.

## 3. Risks and next action

Implementation, public constructors/schema freeze, selected-source/platform qualification, physical carriers, activation, and release remain correctly deferred and are not certified here. Revise the design and controlling retry authority for P2-1/P2-2, close the status grammar in P3-1, then perform a focused Safety re-verdict before H1 implementation.
