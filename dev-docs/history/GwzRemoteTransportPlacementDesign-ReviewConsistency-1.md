# GWZ Remote Transport Endpoint-Placement Design — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core` `6c9abaef8ef2257371637a3f02d0771cd84bab34`, `dev-docs/GwzRemoteTransportPlacementDesign.md` and `docs/TransportPlacement.md`; remediation-round-1 draft design gate  
**Baseline:** root `a4c5b22299be9128fef4fd212b374c669353be14`; core `6c9abaef8ef2257371637a3f02d0771cd84bab34`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`/`git diff`. Every commit and tree identity matched the prompt at the start and end of review.  
**Date:** 2026-09-22  
**Axis:** Focused Consistency re-verdict on the original P2 counterexamples and changed-range interactions. Independent, adversarial, read-only. The current peer review was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — P2-1 and P2-2 are closed. No new P0–P3 finding or architectural root cause was found.

---

## 0. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-1 — failed `Closed` had two facts authorities | **Closed** | Design §6, lines 269–278, now makes `Closed.facts` the sole authority for every `Closed`. `Closed.failure.facts` must be absent/null and is rejected even when equal. The terminal adapter combines `Closed.failure` code/effect with `Closed.facts` into one receipt used by both failure and observation projection. Lines 364–365 require absent/null admission and rejection of equal and conflicting nested facts. |
| P2-2 — bootstrap-owner cancellation could split endpoint and mux binding state | **Closed** | Design §4, lines 156–178, serializes verified `Bound` installation against abandonment. Installation wins and preserves the session binding; abandonment wins and atomically retires the session/port, wakes every waiter, propagates endpoint closure, and requires a fresh port, session identifier, and endpoint. Bind is never resent on an abandoned session. Lines 357–361 require deterministic before/after-readiness and installation races, delayed/lost/duplicate `Bound`, multiple waiters, shutdown, bounded release, and fresh-session recovery. |

## 1. Changed-range analysis

The P2-1 correction removes the original wire ambiguity rather than adding precedence after projection. A failed `Closed` has one facts source, while `OpenFailed` and `Failed` continue to use `Failure.facts`. Bind and identity-check failures still forbid facts. This remains compatible with v1, where `Failure.facts` is absent, and gives the v2 codec a deterministic contextual rejection rule.

The P2-2 correction covers both sides of the readiness race. A cancelled request cannot discard a successfully installed session binding, while cancellation that wins before installation retires all local authority immediately and requires host-propagated closure for endpoint cleanup. A new request cannot reuse the old acknowledgement or resend Bind on that session. Fresh session and endpoint identity prevent delayed old traffic from authorizing later work even when peer cleanup confirmation is unavailable.

The proposed runtime/port API is consistent with that state machine. `request` owns bootstrap installation; `ClientRequest` establishes request admission before command dispatch; the first valid Open or CheckIdentity fixes `operation_id`; registration persists through terminal cleanup; and port loss closes the binding and wakes waiters. Transferred messages must be delivered or followed by disconnect, so cancelling a forwarding loop cannot silently lose a bootstrap or control message while leaving the session live. `remove_cli`, endpoint shutdown, and runtime shutdown have distinct ownership scopes, are idempotent, and report only local cleanup facts.

The new capability-generation affinity rule closes a direct interaction with placement admission: a successful capability probe is pinned to the exact core/runtime/channel generation through dispatch, replacement invalidates it, and hosts unable to guarantee affinity must report CLI placement unavailable. Bind/Bound is expressly insufficient to prove core receiver affinity. No hidden reroute or replay is admitted.

No changed clause weakens the no-carrier, no-new-service, preflight, credential ownership, versioning, or activation boundaries. Candidate regeneration remains isolated until dependency activation, and a non-null `endpoint_path_base` is now unambiguously invalid for local placement.

## 2. Evidence base

I read the complete remediation prompt, the committed merged remediation plan, the filed original Consistency report, and the exact core diff from `5be22a1931160c2d124e0705cd516a4b88fdb098` to `6c9abaef8ef2257371637a3f02d0771cd84bab34`.

Focused inspection covered revised design §§2–9, especially lines 103–178, 237–303, and 326–387; the complete revised embedding guide, including the proposed Rust declarations, registration rules, port cancellation semantics, cleanup reporting, and lifecycle example; the root checkpoint; and the unchanged controlling Bind/Bound and transport-schema facts structures needed to replay the original counterexamples. `git diff --check` on the two core documents was clean.

No build or executable test was run or claimed. The API example is explicitly proposed and requires a compile fixture during implementation. The broader documentation checker’s unchanged CLI release-note failure remains unrelated and is not represented as green.

## 3. Findings

None.

## 4. Invariant analysis

Both original adversarial sequences now terminate deterministically: conflicting terminal facts are rejected before dispatch, and bootstrap cancellation leaves either a shared installed binding or a retired session requiring fresh identity.

The corrections also preserve the previously held invariants: v1 bootstrap and reader compatibility; v2-only new messages and facts; additive unoccupied tags; local placement by omission; whole-operation route and identity preflight; no fallback; endpoint-owned credentials and network effects; bounded message admission and control progress; canonical repository-refusal ordering; and no operation replay after carrier or receiver replacement.

The current unary GWZ service and one-way logs still do not constitute a carrier. The documents continue to acknowledge that limitation and require real supplied-connection evidence before activation, so the proposed in-memory application port does not silently become a new physical service.

## 5. Risks and next action

Actual carrier qualification, API compilation, schema/code implementation, platform and selected-source qualification, production activation, and HTTPS remain later gates. This GO admits only the documented A/B implementation work and does not certify those future results.

Proceed with the bounded schema/mux lifecycle implementation using the corrected facts and bootstrap state machines as acceptance criteria.
