# GwzRemoteTransportPlacementA — SURFACE-AXIS REVIEW

**Review object:** PlacementA aggregate at `.` `e4bf1328de6ac5a56645a13827a0b6d739a70462`, `gwz-core` `018176c6783d900570ba8f918aa744bd642fc3b6`, and `gwz-transport` `d7051eede48f6cd8b07815edc9ced1916f82585c`; Batch B facade remains explicitly deferred.

**Baseline:** `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence` `a3dc6989aa769dec65e41f39d7e53a8febb4e916`. All tuple entries were verified unchanged at start and end. Only user-facing documentation was read.

**Date:** 2026-09-22

**Axis:** Surface usability, lifecycle symmetry, defaults, discoverability, and separation of implemented PlacementA from proposed Batch B. Independent, adversarial, read-only.

**Verdict: GO** — the prior P3 direction/tuple-labeling finding is closed; no P0–P2 findings remain.

## 0. Evidence base

Read:

- `gwz-transport/README.md:344–470`, including mux lifecycle, defaults, rejection/terminal behavior, and the concrete async construction example.
- `gwz-core/docs/TransportPlacement.md:109–312`, including the proposed Batch B facade, explicit forwarding directions, tuple labeling, and lifecycle example.
- `gwz-core/docs/Embedding.md:104–108`.
- Required tuple hashes at start and end.

No source, design, plan, or remediation document was read. No files were modified.

## 1. Findings

No open findings.

## 2. Invariant analysis

The prior P3-1 finding is closed:

- `Attachment` explicitly identifies its `String` as the existing `request_id`.
- Both forwarding directions are stated in `gwz-transport/README.md:356–357` and `TransportPlacement.md:211–218`.
- The executable lower-level example now constructs both sides with `Mux::initiator` and `Mux::endpoint`, registers requests, performs bootstrap, forwards Bind/Bound, waits for readiness, exercises identity-check rejection, drains terminal messages, finishes, and disconnects (`README.md:411–470`).
- The rejection lifecycle is explicit: `BindRejected`, rejecting phase, retained failure, closure, and `Owner::ready()` returning `Error::Rejected` (`README.md:403–409`).
- Cancellation, finish retry behavior, terminal handoffs, clock servicing, disconnect, owner drops, and cleanup deadlines are documented (`README.md:372–395`).
- Defaults and limits for roles, requests, streams, cleanup, identity checks, queues, and pending async calls are stated (`README.md:378–388`).
- The lower-level implemented fixture is clearly separated from the proposed Batch B facade (`README.md:390–395`, `TransportPlacement.md:219–221`).
- The proposed core facade remains explicitly unavailable and carries no implementation or compilation claim (`TransportPlacement.md:109–114`).

The first-day lower-level walkthrough is now actionable without consulting source or tests.

## 3. Risks and next action

No surface blocker remains. The next gate is the separately deferred Batch B implementation and documentation-fixture compilation; it is outside this PlacementA surface verdict.
