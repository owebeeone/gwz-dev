# PlacementA correction 2 — CODE-AXIS REVIEW

**Review object:** Phase 4 PlacementA correction 2 at root `d20e168bb93cbe6bf5238b0b0295f0cd51f5cd20`, controlling DRAFT `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md` at `28f667c0462c74798761ec9710de793c697c7fb8`, 2026-09-22.  
**Baseline:** `gwz-core` `28f667c0462c74798761ec9710de793c697c7fb8`; `gwz-transport` `03d3011b3ae9b8205bcf07f7f7862194af114856`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `acdd2f98f395e51c60014c8faa82e315a48c0ada`. Committed sources were read with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Code — focused P2-3 closure and changed-range interface/admission review. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — P2-3 is closed; prior P2-1 and P2-2 remain closed. No P0–P3 findings and no new architectural root.

---

## 0. Evidence base

I read the round-one Code report, `GwzRemoteTransportPlacementA-RemPlan-2.md`, transport diff `d7051ee..03d3011`, core diff `018176c..28f667c`, and the exact committed versions of:

- `src/codec.rs` and `src/codec/validate.rs`;
- `src/mux/mod.rs` and `src/mux/routing.rs`;
- all changed rejection and endpoint-configuration tests in `tests/mux.rs`;
- the correction-2 checkpoint annotation and controlling bootstrap semantics in `GwzRemoteTransportDesign.md` §3.0.

The transport change is limited to validation, mux construction/admission, and focused tests. Schema, Taut, public API, terminal lifecycle, README surface, and physical-transport boundaries are unchanged.

The permitted external-target suite passed 47 tests: `mux` 26, `mux_async` 3, `placement_v2` 10, and `async_stream` 8.

All nine pinned HEADs and commit trees matched at start and end. Member trees remained clean; allowed untracked generated prompts were excluded and current peer output was not read.

## 1. Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| P2-1: `finish` could discard an admitted terminal | **Remains closed** | Correction 2 does not alter queue retention, terminal classification, finish blocking, cleanup deadlines, or handoff paths. All terminal lifecycle tests remain green. |
| P2-2: typed bootstrap rejection collapsed into generic closure | **Remains closed** | `Rejecting`, `Rejected`, exact retained failure, bounded handoff, and malformed-Bind separation are unchanged. Both valid rejection outcomes still survive async transfer and closure. |
| P2-3: operation-time codes admitted as authoritative `BindRejected` | **Closed** | `validate::bind_rejection` now centrally requires exactly `UnsupportedVersion` or `UnsupportedOperation`, `Effect::None`, and absent facts. Generic envelope admission, incoming mux routing, and locally constructed rejection all use that predicate. `Authentication`, `Io`, every other enum code, possible effects, and facts fail typed construction and encoded decoding. Mux reception closes as Protocol without retaining `bootstrap_failure`. Invalid endpoint identity, capability, and hard-limit configuration now fails locally at `Mux::endpoint`. |

The original P2-3 sequence—validly encoded `BindRejected(Authentication, None, no facts)` delivered during Binding—now fails codec admission, closes the mux as Protocol, installs no binding or work, and leaves `bootstrap_failure()` empty. The corresponding `Io` sequence behaves identically. Valid `UnsupportedVersion` and `UnsupportedOperation` retain their exact reasons.

## 2. Invariant analysis

The centralized predicate prevents divergence between encoded-wire admission, mux receipt, and endpoint-generated rejection. Its domain matches the frozen bootstrap contract: binding performs no credential or network operation, so it cannot authoritatively report authentication, trust, carrier, repository, or exchange failures.

The new constructor checks reject malformed local endpoint identity/capability/hard-limit state before bootstrap. Negotiable but usable-under-hard-caps incompatibility remains a typed `UnsupportedOperation`, preserving the accepted P2-2 behavior. Neither valid rejection path installs a binding, route, or endpoint work.

The changed range introduces no schema/tag change, allocation path, new authority, carrier claim, or altered v1/v2 reader behavior.

## 3. Risks and next action

This GO accepts the PlacementA Code boundary only. Batch B facade/backend integration, a real supplied carrier, platform and selected-source qualification, activation, HTTPS, publication, and remote CI remain deferred as documented. The next action is to record aggregate review acceptance and proceed to the separately gated Batch B work.
