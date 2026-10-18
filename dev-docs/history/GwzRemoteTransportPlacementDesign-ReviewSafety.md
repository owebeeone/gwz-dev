# GWZ endpoint-placement integration design — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` and `gwz-core/docs/TransportPlacement.md` at core `5be22a1931160c2d124e0705cd516a4b88fdb098`; draft design freeze dated 2026-09-22.  
**Baseline:** root `cf34abc7b739c82af9f466665e0e514e166f5e35`; core `5be22a1931160c2d124e0705cd516a4b88fdb098`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; all tuple HEADs matched at both boundaries.  
**Date:** 2026-09-22  
**Axis:** Safety—mixed-version degradation, authority, irreversible effects, correlation, cancellation, cleanup, and terminal-state ordering. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P1 and one P2 finding block the design freeze. I pre-commit to GO on a revision that resolves P1-1 and P2-1 as specified.

---

## 0. Evidence base

I read the complete generated Safety prompt; the full placement design and embedding guide; relevant authority in `GwzRemoteTransportRequirements.md` G2–G7, `GwzRemoteTransportDesign.md` §§3–3.2, and `GwzRemoteTransportPlan.md` Phase 4; and the accepted N3 boundary referenced by the package.

I inspected the pinned existing GWZ schema and decoder. `src/protocol/generated.rs:2645-2665` shows that the old `TransportOptions` decoder reads tags 1–3 and does not reject an unknown tag 4. `src/operation/push_event.rs:192-229` copies ordinary-request `schema_version` without validating a new protocol profile; only the separate capabilities request validates its own schema. This makes the mixed-version attack below concrete.

No builds or tests were run, as required for this design-only gate. The owner reports that 32 local Markdown links and the whitespace check pass. The workspace documentation checker still reports an unchanged, unrelated `gwz-cli/docs/Releases.md` defect; this review does not claim that gate green.

The final tuple was unchanged. Root status contained generated review artifacts outside the committed object; the current peer report was not opened. Member source trees were clean.

## 1. Findings

### [P1-1] Capability admission is not bound to the core instance that receives the operation

**Location:** Placement design §§2–3, especially lines 52–61 and 93–118; embedding guide lines 26–36 and 78–82; requirements G6; Plan Phase 4 lines 286–303.

**Violated invariant:** An explicit `cli` request must fail before network, credential, helper, or mutation effects when the receiving core cannot enforce that placement. It must never become local through omission or old-decoder behavior.

**Concrete sequence:**

1. A new driver queries capabilities from core instance C1 and observes placement `cli` plus message version 2.
2. Before the ordinary operation is dispatched, the embedding channel reconnects, fails over, or independently routes the request to old core C0. The design does not require the capability result and operation to have same-instance/channel affinity or define invalidation on this handoff.
3. The request carries additive `TransportOptions.placement` tag 4 while retaining the existing GWZ schema/service version.
4. C0’s pinned decoder reads tags 1–3 and ignores tag 4. Its ordinary operation path does not reject the request schema as a new profile.
5. Placement therefore has the old omission meaning: local. C0 may inspect local credentials, invoke local helpers, open a Git-host connection, mutate the workspace, or publish a push under the wrong authority.

Driver-side capability probing is insufficient unless its result is scoped to the exact receiver of the subsequent operation. This can cause irreversible remote publication with credentials the caller explicitly rejected, making it a P1 release blocker.

**Required correction:** Define a capability-admission generation bound to the exact live core instance/channel/backend-family runtime that will dispatch the operation. Closure, reconnection, failover, or receiver replacement must invalidate it before another explicit-CLI request. A host that cannot guarantee affinity must report CLI placement unavailable. The text must not rely on old cores rejecting tag 4. If server-side enforcement is chosen, it must use a discriminator that the retained old receiver demonstrably rejects.

**Closure test:** Query a supporting new core, replace or reroute the operation receiver to an old core, then request explicit CLI placement. Assert that the operation is not sent and that no local filesystem, credential, helper, socket, workspace, or remote effect occurs. Also prove that channel replacement invalidates admission, an unaffined host refuses CLI placement, and old-driver/new-core ordinary local requests remain valid.

### [P2-1] Bootstrap-owner cancellation permits endpoint-ready/mux-unbound split state

**Location:** Placement design lines 135–145; controlling Design §3.0 lines 107–124.

**Violated invariant:** Bind/Bound must have one recoverable terminal outcome, wake every waiter, and never leave the two peers disagreeing about whether a session is bound.

**Concrete interleaving:**

1. Request A owns the single Bind/Bound bootstrap; request B waits.
2. The endpoint validates Bind, marks the session ready as required, and sends Bound.
3. A is canceled or lost before the mux receives and installs Bound.
4. The amendment says A’s cancellation abandons bootstrap and late Bound cannot install it.
5. No clause retires the endpoint’s already-ready binding, closes that carrier generation, wakes B with a terminal result, or permits a conflict-free replacement owner. The endpoint can remain ready while the mux is unbound; B can remain stuck, and a repeated Bind on the same session can conflict with endpoint state.

Disconnect cleanup does not close this gap because owner cancellation is not defined to disconnect or retire the bootstrap session.

**Required correction:** Specify a linearization rule. If validated Bound installation wins first, the session binding survives A’s later cancellation and only A’s operation is canceled. If cancellation wins, both sides must retire/close that bootstrap session generation, fail and wake all waiters, reject late Bound, and retry only on a fresh session identifier. Any alternative must provide the same single-terminal and bounded-recovery guarantees.

**Closure test:** Deterministically exercise cancellation immediately before and after endpoint readiness and mux installation, with multiple waiters, delayed/lost/duplicate Bound, and shutdown racing each state. Assert one terminal outcome, bounded waiter release, no stale installation, no duplicate binding, and successful bootstrap only on the permitted established or fresh session.

## 2. Invariant analysis

The following attacks did not produce additional findings:

- Placement is operation-wide; arbitrary endpoint addresses and cross-client binding selection are excluded.
- Capability advertisement alone does not authorize an endpoint route; within one established generation, Bind/Bound negotiates version, scheme, policy, and hard bounds before Open.
- v2-only identity checks cannot downgrade into v1, while bootstrap remains v1 and retained local v1 meaning is preserved.
- Positive stream identifiers bind the full request, operation, session, and endpoint tuple; stale or unknown messages cannot create work or release another lease.
- CLI identity paths remain opaque to core. The endpoint performs native parsing, whole-plan preflight precedes mutation/network effects, and Open repeats authoritative snapshot admission.
- Host and transport queues, retained identifiers, decode memory, checks, deadlines, and control progress all receive explicit bounds.
- Repository refusal requires complete typed terminal evidence before EOF; late authentication facts cannot rewrite the first terminal cause.
- Carrier loss after possible publication remains uncertain and is not replayed.
- The package accurately defers the physical carrier, platform/source qualification, HTTPS, and activation, and makes A/B evidence independently satisfiable.

## 3. Risks and next action

Actual host delivery, separate-process behavior, platform qualification, and production activation remain later gates and are not certified here. The unrelated existing release-documentation checker failure also remains outside this object.

Revise the design and guide to bind core capability admission to the operation receiver and to close the bootstrap cancellation state machine, then run a focused Safety re-verdict on those clauses and their required regression evidence.
