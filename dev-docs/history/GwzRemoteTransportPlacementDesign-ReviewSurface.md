# GwzRemoteTransportPlacementDesign — SURFACE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` and proposed user documentation at `gwz-core/docs/TransportPlacement.md`, `gwz-core/docs/Embedding.md`; design-only initial freeze.

**Baseline:** `.` `cf34abc7b739c82af9f466665e0e514e166f5e35`; `gwz-core` `5be22a1931160c2d124e0705cd516a4b88fdb098`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-transport` `a6562e654b52705b72ef1f793ae2045c320cee47`; `taut` `733e8a78897a90f017f4726e4331aed95e8cb977`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from the working trees at those unchanged commits.

**Date:** 2026-09-22

**Axis:** Surface usability: names, defaults, discoverability, and complete embedding lifecycle. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it.

---

## Verdict: NO-GO — one P2 finding blocks; one P3 finding remains.

The P2 finding has a bounded documentation/API-surface remedy.

## 0. Evidence base

Read only:

- `gwz-core/docs/TransportPlacement.md`, lines 1–101.
- `gwz-core/docs/Embedding.md`, lines 1–108.
- `gwz --help`.
- `gwz auth --help`.
- `gwz auth identity --help`.
- `gwz repo --help`.
- `gwz help push`.

The placement guide explicitly says this is an embedding lifecycle, not a CLI walkthrough, and that exact Rust constructor/method spellings are deferred until implementation (`TransportPlacement.md:21–24`, `48–49`). The existing CLI help exposes no placement command and places identity configuration under `gwz auth identity`, with `--remote-identity` as a global option.

The required repository tuple was verified unchanged at both the start and end of review.

## 1. Findings

### [P2-1] The configure/use/undo lifecycle has no callable surface

`TransportPlacement.md:26–46` describes constructing a “backend-family runtime,” installing endpoints, supplying delivery hooks, completing `Bind/Bound`, querying `transport_capabilities`, shutting down a “runtime binding,” and retaining a returned owner. It gives no public constructor, runtime type, endpoint installation method, hook signature, capability request/response shape, bind acknowledgement type, cancellation method, or shutdown completion method. `Embedding.md:104–108` only links to the same semantic description.

Reproduction:

1. An embedder can find `Git2Backend::new()` in `Embedding.md`.
2. The next step says to construct a backend-family runtime and install local/client endpoints, but provides no callable name or example.
3. The next step names `transport_capabilities`, but provides no operation invocation or response fields beyond prose.
4. The bind step names `Bind/Bound`, but provides no message construction or acknowledgement API.
5. The undo step requires finishing/cancelling operations and shutting down the runtime binding, but provides no discoverable lifecycle pair or way to await bounded cleanup.

The user must invent an adapter contract or wait for implementation-specific names. Independent embedders can therefore make incompatible choices before the promised implementation API is published, and correcting those choices after release becomes a compatibility problem.

Required correction: publish the concrete runtime/endpoint constructor, delivery-hook contract, capability query, bind acknowledgement, operation attachment, cancellation, and shutdown/owner-lifetime APIs together with a minimal configure/use/undo example. Closure requires a documented compile-level example that performs the six steps without guessing a method, type, or message field.

### [P3-1] The identity-management command name in the placement guide does not match the discoverable CLI surface

`TransportPlacement.md:61–65` says “Existing `remote_identity` commands still manage repository configuration locally.” The existing help surface has `gwz auth identity` and the global `--remote-identity` option; there is no `remote_identity` command family.

A user following the placement guide and searching `gwz remote_identity --help` cannot find the referenced command. The mismatch is bounded, but it creates avoidable confusion around whether repository-local identity configuration is a command, a request field, or the global option.

Required correction: replace the backticked term with the exact existing command and option names, such as `gwz auth identity` and `--remote-identity`. Closure requires a documentation/help consistency check that every referenced identity command resolves to an existing help path.

## 2. Invariant analysis

The following surface invariants held:

- Local placement is clearly the default: omission and explicit `local` are both documented as core-local (`TransportPlacement.md:12–17`).
- The options table states defaults for `placement`, `endpoint_path_base`, `default_identity`, `remote_identities`, and `url_scheme` (`TransportPlacement.md:51–59`).
- The guide clearly rejects silent fallback from explicit client placement to local credentials and documents channel-loss behavior (`TransportPlacement.md:78–82`).
- The supported SSH route families and unsupported route classes are enumerated (`TransportPlacement.md:71–76`).
- The conceptual lifecycle has an install/use/return-to-local shape, including owner retention and cleanup on final drop (`TransportPlacement.md:26–46`).
- Existing CLI help confirms there is no placement command, consistent with the guide’s explicit “no new CLI command” statement (`TransportPlacement.md:7–10`).

The lifecycle could not be completed from the documented surface without inventing API names or message shapes, which is the blocking defect.

## 3. Risks and next action

Residual risk is limited to documentation/API discoverability and the one identity-command naming mismatch. No source, design, plan, or repository state was modified.

Next action: publish the concrete embedding lifecycle API and correct the identity command reference before accepting the placement surface.
