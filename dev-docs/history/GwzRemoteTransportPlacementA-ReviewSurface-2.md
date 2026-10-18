# GwzRemoteTransportPlacementA — SURFACE-AXIS REVIEW

**Review object:** PlacementA aggregate at `.` `d20e168bb93cbe6bf5238b0b0295f0cd51f5cd20`, `gwz-core` `28f667c0462c74798761ec9710de793c697c7fb8`, and `gwz-transport` `03d3011b3ae9b8205bcf07f7f7862194af114856`.

**Baseline:** `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; `gwz-core-evidence` `acdd2f98f395e51c60014c8faa82e315a48c0ada`. All tuple entries matched at start and end.

**Date:** 2026-09-22

**Axis:** Surface usability, lifecycle symmetry, defaults, discoverability, and implemented-versus-proposed API boundaries. Independent, adversarial, read-only.

**Verdict: GO** — prior surface findings remain closed; no new findings.

## Closure table and changed-range analysis

| Prior finding | Verification | Status |
|---|---|---|
| Port direction and tuple labeling | `gwz-transport/README.md` and `TransportPlacement.md` are byte-identical to the prior reviewed revisions. | Closed |
| Lower-level mux construction walkthrough | Previously added README construction/register/bootstrap example remains byte-identical. | Closed |

The exact-document checks returned clean:

- `gwz-core/docs/TransportPlacement.md` and `docs/Embedding.md` unchanged from `gwz-core` `018176c...`.
- `gwz-transport/README.md` unchanged from `gwz-transport` `d7051ee...`.

No new architectural root cause was found.

## 0. Evidence base

Read:

- `dev-docs/GwzRemoteTransportPlacementA-PromptSurface-2.md`.
- Prior reviewed user-facing documentation revisions by exact-byte comparison.
- Required repository tuple at start and end.

No source, design, plan, or peer report was read. No files were modified.

## 1. Findings

No open findings.

## 2. Invariant analysis

The previously accepted surface invariants remain intact:

- The lower-level mux lifecycle has concrete construction, registration, bootstrap, forwarding, readiness, cancellation, finish, rejection, clock, and disconnect guidance.
- Both forwarding directions are explicit.
- `Attachment` identifies its `String` as `request_id`.
- Defaults and bounded limits remain documented.
- Terminal rejection and cleanup behavior remain discoverable.
- The lower-level implemented fixture remains clearly separated from the deferred Batch B `transport_host` facade.
- User-facing placement documentation remains unchanged and continues to distinguish proposed APIs from released functionality.

## 3. Risks and next action

No surface action is required for this correction round. Remaining implementation and facade work is explicitly deferred by the review scope.
