# GWZ endpoint-placement integration design remediation 1 — SAFETY-AXIS REVIEW

**Review object:** Revised `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` and `gwz-core/docs/TransportPlacement.md` at core `6c9abaef8ef2257371637a3f02d0771cd84bab34`; draft design freeze, remediation round 1, dated 2026-09-22.  
**Baseline:** root `a4c5b22299be9128fef4fd212b374c669353be14`; core `6c9abaef8ef2257371637a3f02d0771cd84bab34`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Safety—focused closure of mixed-version authority and bootstrap-cancellation findings, plus direct safety interactions of the added runtime/port API. Independent, adversarial, read-only. The current other-axis report was not read. Filed verbatim by the lane owner.

**Verdict: GO** — Safety P1-1 and P2-1 are closed. No new P0–P3 finding was identified in the changed boundary. This accepts the corrected design for its stated A/B implementation scope; it does not certify implementation, a physical carrier, platform qualification, or production activation.

---

## 0. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Safety P1-1: capability admission was not bound to the core instance receiving the operation | **Closed** | Design lines 118–135 bind the capability result to one live host-admission generation and exact core/backend runtime, require receiver pinning through dispatch acceptance, invalidate on reconnection/failover/replacement, prohibit hidden reroute/replay, and make CLI placement unavailable without affinity. Guide lines 29–38 and 177–186 carry the same direct/split-host rule. Evidence lines 361–364 require the exact new-core-probe/old-core-swap attack to produce zero operation sends and zero effects. |
| Safety P2-1: bootstrap-owner cancellation could leave endpoint ready while mux remained unbound | **Closed** | Design lines 156–178 serialize Bound installation against abandonment. Installation winning preserves the binding and cancels only the request; abandonment winning retires the session and port, wakes every waiter, propagates endpoint closure, rejects late Bound, and permits recovery only with a fresh port/session/endpoint. Guide lines 188–219 expose matching ownership and port-disconnect behavior. Evidence lines 357–361 require deterministic barriers around readiness and installation, multiple waiters, delayed/lost/duplicate Bound, shutdown, bounded release, and fresh-session recovery. |

## 1. Changed-range analysis

The P1-1 counterexample no longer reaches the old decoder. A capability response is explicitly non-transferable between receiver generations. A host must hold the same receiver from query through dispatch acceptance; replacement invalidates cached support before another explicit-CLI request, and an operation already sent when replacement occurs fails without replay. The correction also states that endpoint Bind/Bound cannot substitute for core-receiver affinity and does not pretend old cores reject the additive placement tag. This directly closes the original silent-local-degradation path.

The P2-1 race now has one linearized winner and a complete losing transition. If Bound installation wins, later owner cancellation cannot destroy session authority needed by other requests. If abandonment wins, local waiters fail without awaiting peer cleanup, the application port closes, the endpoint retires pending or ready state when closure arrives, and stale acknowledgements cannot authorize work. Reusing the abandoned session is forbidden.

The proposed API preserves these rules:

- `TransportRuntime::request` reserves the request/operation tuple and returns only after verified Bound installation.
- `ClientRequest` registers the request before command dispatch and remains through cleanup; attachments cannot create an unregistered request.
- Dropping a pending request future invokes the same cancellation transition.
- `next_message` and `deliver` define cancellation before transfer/admission; after transfer, the host must deliver or disconnect.
- `remove_cli`, `shutdown`, last-owner drop, and port disconnect invalidate the applicable generation and wake waiters.
- `CleanupReport` describes local retained work and cannot be interpreted as peer cleanup or Git success.
- Passing different metadata or operation identity through the scoped backend is expressly invalid and must be enforced by the later implementation and compile/test gate.

I found no new architectural root cause in these additions.

## 2. Evidence base

I read the complete round-one Safety prompt, the committed merged remediation plan, my filed initial Safety report, and the exact core diff from `5be22a1931160c2d124e0705cd516a4b88fdb098` to `6c9abaef8ef2257371637a3f02d0771cd84bab34`. Inspection covered the revised placement design §§2–4 and §8, the complete proposed Rust interface and lifecycle example in `docs/TransportPlacement.md`, and their interaction with the retained requirements G6 and binding rules.

No build or test was run or claimed. The owner records package link and whitespace checks as passing and the unchanged CLI release-documentation checker debt as still outstanding and unrelated.

All eight tuple HEADs matched at both review boundaries. Member trees were clean. Root status contained only the explicitly excluded generated prompt artifacts; no current-round peer report was read.

## 3. Findings

None.

## 4. Invariant analysis

The corrected design now preserves the attacked invariants:

- Explicit CLI placement cannot cross an unverified receiver handoff or degrade to local through an old decoder.
- Bootstrap cancellation and Bound installation cannot both win, strand waiters, or leave reusable split authority.
- Fresh recovery requires a fresh port, session identifier, and endpoint instance.
- Request registration precedes attachments; request, operation, session, endpoint, and stream correlation remain bounded and non-reusable.
- Port cancellation cannot silently discard an already-transferred frame while leaving the session open.
- Cleanup remains bounded and is kept distinct from Git success and remote-effect certainty.
- Whole-operation identity/route preflight, endpoint-owned credential authority, v1/v2 negotiation, terminal-fact ordering, and no-replay rules remain intact.
- The API remains an application handoff surface and does not create or imply the deferred physical carrier.

## 5. Risks and next action

The receiver-generation guard, scoped-backend tuple enforcement, bootstrap barriers, port-transfer cancellation, and cleanup accounting remain implementation obligations. Separate-process carrier evidence, deferred platform/source qualification, HTTPS, and activation remain later gates.

Proceed to the stated A/B implementation batches and require the exact deterministic regressions in design §8 before any production advertisement.
