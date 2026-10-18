# Phase4 PlacementB candidate aggregate — SURFACE-AXIS REVIEW

**Review object:** Phase4 PlacementB candidate aggregate at the exact tuple below; candidate documentation status; 2026-09-22  
**Baseline:** `. ba32b3af11c08f4c6b0a6897b158d5a41ed2465b`; `gwz-core 6b9be8a2634700a3f47483beb99a582e7acd20b3`; `gwz-transport 03d3011b3ae9b8205bcf07f7f7862194af114856`; `taut bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli 7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs ce78628308e11b4e8901d5061602619109bce21a`; `libgit2 b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence 9980a5d2199df09b055e677e07123f209900ea8c`. Read checked-out user-facing docs at those verified revisions.  
**Date:** 2026-09-22  
**Axis:** Surface interface, lifecycle discoverability, naming, defaults, ownership and first-use walkthrough. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0/P1/P2/P3 findings.

---

## 0. Evidence base

Read only:

- `gwz-core/docs/TransportPlacement.md:1-324`
- `gwz-core/docs/Embedding.md:1-108`
- `gwz-transport/README.md:344-470`
- `dev-docs/GwzRemoteTransportPlacementB-PromptSurface.md`

The opening placement guide clearly labels the API as isolated-harness-only, preserves local placement as the default, and states that no CLI command is added. The embedding walkthrough covers runtime construction, capability checks, Bind/Bound, request execution, cancellation, cleanup, removal and return to local placement.

The candidate signatures document symmetric lifecycle pairs: `new`/`shutdown`, `install_cli`/`remove_cli`, `register_request`/`finish`, and `request`/`cancel`/`finish`. Defaults are stated for placement, endpoint paths, identity selection, receive window, request and stream limits, cleanup deadlines, identity checks and network startup.

The transport README documents both forwarding directions, request ID and envelope ownership, port closure, cancellation, rejection, terminal handoff, cleanup, and the lower-level construct/bind/use/disconnect example. `Embedding.md` links to the candidate placement guide and correctly keeps it separate from the released embedding surface.

The exact repository tuple was verified before inspection and re-verified unchanged afterward. No tests or builds were run; the guide’s candidate-harness compilation statements were treated as documented evidence rather than independently reproduced.

## 1. Findings

No open findings.

## 2. Invariant analysis

The first-day surface walkthrough succeeds from the documentation:

1. The user learns that placement is an embedding lifecycle and that local placement remains the default.
2. The client endpoint and runtime owners are created.
3. Capabilities are queried and checked before explicit CLI placement.
4. Bind/Bound and request registration are performed before work.
5. Bidirectional forwarding is shown with explicit source and destination ports.
6. The ordinary Git handler runs once while transport messages progress independently.
7. Cancellation, request finish, endpoint shutdown, runtime removal and runtime shutdown are all named and ordered.
8. Subsequent local work is described by omitting placement.

The docs state every material option’s default or failure behavior, including relative identity paths, endpoint path bases, ambient identity selection, empty remote overrides, request ID exhaustion, cleanup deadlines and bounded concurrency.

Ownership and lifetime are explicit: returned owners remain alive for reuse, clones share ownership, ports do not create carriers or threads, last-owner drop initiates cleanup, and cleanup reports do not imply Git success or peer cleanup. Request IDs are bounded and non-recycled. Both forwarding directions and their ordering requirements are documented.

The candidate facade is clearly distinguished from the lower-level transport mux and from released production behavior. Deferred carrier, platform, HTTPS, qualification and production activation work is not presented as available functionality.

## 3. Risks and next action

Residual risk is limited to the documented candidate status and deferred production qualification. No Surface correction is required. The next action is the existing release-gate qualification once the deferred carrier and activation work is complete.
