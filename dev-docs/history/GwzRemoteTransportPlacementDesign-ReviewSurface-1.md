# GwzRemoteTransportPlacementDesign — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` and revised proposed user documentation at `gwz-core/docs/TransportPlacement.md`, `gwz-core/docs/Embedding.md`; design-only initial freeze.

**Baseline:** `.` `a4c5b22299be9128fef4fd212b374c669353be14`; `gwz-core` `6c9abaef8ef2257371637a3f02d0771cd84bab34`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-transport` `a6562e654b52705b72ef1f793ae2045c320cee47`; `taut` `733e8a78897a90f017f4726e4331aed95e8cb977`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from the working trees at those commits.

**Date:** 2026-09-22

**Axis:** Surface usability: names, defaults, discoverability, and complete embedding lifecycle. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it.

**Verdict: GO** — the prior P2 and P3 findings are closed; one new P3 documentation ambiguity remains and does not block the design-only gate.

## Closure table and changed-range analysis

| Prior finding | Closure evidence | Status |
|---|---|---|
| P2-1: no callable configure/use/undo surface | `TransportPlacement.md:109–230` now declares concrete types, methods, ownership, cancellation, cleanup, capability checks, and affinity rules. The configure/fetch/remove example at `:232–297` traces those signatures. | Closed |
| P3-1: incorrect `remote_identity` command name | `TransportPlacement.md:66–71` now names `gwz auth identity` and `--remote-identity`, matching the existing CLI surface. | Closed |

The revised range adds the proposed `gwz_core::transport_host` signatures, explicit cleanup and ownership rules, capability-cache affinity requirements, and a complete lifecycle example. The example’s calls match the declared signatures and ownership flow by inspection: runtime and client endpoint construction, port installation, capability validation, request registration, request creation, ordinary handler invocation, operation/client cleanup, binding removal, endpoint shutdown, and runtime shutdown. No implementation or compilation claim is made, as required.

## 0. Evidence base

Read only:

- `dev-docs/GwzRemoteTransportPlacementDesign-PromptSurface-1.md`.
- `gwz-core/docs/TransportPlacement.md`, including:
  - defaults and lifecycle: lines 1–107;
  - proposed signatures: lines 109–230;
  - configure/use/remove example: lines 232–297.
- `gwz-core/docs/Embedding.md`, lines 1–108.
- Required tuple verification at the start and end of review.

No source, design, plan, build, test, or mutation was used.

## 1. Findings

### [P3-1] Port direction is not stated explicitly

`TransportPlacement.md:152–159`, `:208–219`, and the example’s `connect` callback at `:234–257` define two ports and two symmetric methods, but never state which forwarding loop calls which method.

A first-time embedding receives `core_port` from `TransportRuntime::install_cli()` and `client_port` from `CliEndpoint::new()`. The supplied `connect` implementation must infer that core-to-client traffic should be read with `core_port.next_message()` and admitted with `client_port.deliver(...)`, while client-to-core traffic should use the reverse mapping. The names make this guess reasonable, but the direction is a required transport invariant and is not declared.

Impact is bounded to integration failures or a silent non-communicating adapter during first implementation.

Correction: add a two-line direction table or pseudocode to the port section and label the `Attachment` tuple’s `String` component. Closure test: the documentation fixture’s `connect` example forwards one synthetic attachment in each direction and identifies the corresponding `next_message`/`deliver` calls.

## 2. Invariant analysis

The revised surface now satisfies the primary lifecycle invariants:

- Runtime creation and shutdown are paired.
- CLI endpoint creation and shutdown are paired.
- CLI installation and removal are paired.
- Request registration and `ClientRequest::finish` are paired.
- Transport request cancellation and `finish` are both documented.
- Port closure has an explicit `disconnect`.
- Every option in the defaults table has a stated default.
- Local placement remains the documented default, with no new CLI command.
- Capability caching is scoped to a live runtime and receiver generation, and reconnect/failover invalidation is explicit.
- The proposed example follows the declared ownership and cleanup sequence without inventing a carrier API.
- Existing identity command names now match the CLI help surface.

The remaining port-direction gap is a bounded documentation issue rather than a lifecycle or compatibility blocker.

## 3. Risks and next action

Residual risks below the blocking threshold:

- The distinction between capability message version `2` and request schema version `gwz.protocol/v0` should be made explicit in the eventual compile-check fixture.
- The proposed signatures and example still require the promised later implementation compile gate.

Next action: add explicit core-port/client-port forwarding direction and attachment tuple labeling before publishing the embedding API documentation.
