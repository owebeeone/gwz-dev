# GwzRemoteTransportPlacementA — SURFACE-AXIS REVIEW

**Review object:** PlacementA aggregate at `.` `9cc9747c9466e877579bf95779dece092864b97e`, `gwz-core` `c0649c3d9a4804cee6eb180911da4863266c3045`, `gwz-transport` `f6e9ee21fe5e39e3fc61d8484a9410062728131c`; design-only Batch B facade remains explicitly deferred.

**Baseline:** Tuple verified unchanged at start and end. User-facing documentation was read from the exact tuple; no source, design, or plan documents were read.

**Date:** 2026-09-22

**Axis:** Surface usability, lifecycle symmetry, defaults, discoverability, and separation of implemented PlacementA from proposed Batch B. Independent, adversarial, read-only.

**Verdict: GO** — no P0, P1, or P2 findings; one bounded P3 documentation finding remains.

## 0. Evidence base

Read:

- `gwz-transport/README.md`, especially the Request mux and application ports section, lines 344–397.
- `gwz-core/docs/TransportPlacement.md`, especially the proposed facade and lifecycle example, lines 109–312.
- `gwz-core/docs/Embedding.md`.
- Required tuple hashes at start and end.
- Allowed focused tests with an external target:
  - `mux`: 13 passed.
  - `mux_async`: 3 passed.
  - `placement_v2`: 10 passed.
  - `async_stream`: 8 passed.

The README now explicitly labels the mux as a candidate lower-level surface and states that the future `transport_host` facade is Batch B and unimplemented.

## 1. Findings

### [P3-1] Lower-level mux setup lacks a concrete first-construction example

`gwz-transport/README.md:344–367` names the lifecycle—construct initiator and endpoint, register requests, call `begin`, forward through both ports, await `ready`, process actions, then cancel and finish—but does not show how to construct `mux::Mux`, `EndpointConfig`, or the registered request objects, nor the concrete registration and bootstrap call sequence.

A new host can determine the required session identity, roles, directions, cancellation, finish, clock, and disconnect behavior, but must still guess the initial constructor/configuration calls before reaching `Owner::new(mux)`. The cited executable fixture is useful evidence, but it is a test path rather than a user-facing setup example.

Impact is limited to onboarding and adapter implementation friction; the lifecycle and wire-direction contracts are otherwise explicit.

Correction: add a short lower-level Rust example or signatures showing `Mux`/`EndpointConfig` construction, initiator/endpoint creation, request registration, and `begin`/`ready` bootstrap. Closure test: a reader can create both sides and reach the first admitted request without consulting source or test code.

## 2. Invariant analysis

The prior port-direction and tuple-labeling issue is closed:

- `TransportPlacement.md:209–218` explicitly maps core-to-client and client-to-core forwarding.
- The `Attachment` string is identified as the existing `request_id`.
- The same direction and ownership rules appear in `gwz-transport/README.md:346–362`.

The implemented lower-level lifecycle is otherwise discoverable:

- Request registration precedes bootstrap.
- Bootstrap ownership and cancellation races are documented.
- `Owner::ready`, `Owner::binding`, `check_identity`, `open`, `next_action`, and `send` are named.
- `cancel`, `finish`, disconnect, final-owner drop, clock advancement, timeout behavior, and cleanup limits are documented.
- Defaults and ceilings for requests, streams, queues, cleanup, identity checks, and pending async calls are stated.
- `TransportPlacement.md` clearly marks the core `transport_host` facade and full example as proposed Batch B rather than claiming implementation.
- Focused mux, async, protocol, and stream tests passed.

## 3. Risks and next action

Residual risk is limited to the lower-level setup example gap identified in P3-1.

Next action: add the minimal mux construction and registration snippet to `gwz-transport/README.md`; no placement verdict blocker remains.
