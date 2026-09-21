# PlacementA correction 1 — CODE-AXIS REVIEW

**Review object:** Phase 4 PlacementA correction 1 at root `e4bf1328de6ac5a56645a13827a0b6d739a70462`, controlling DRAFT `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md` at `018176c6783d900570ba8f918aa744bd642fc3b6`, 2026-09-22.  
**Baseline:** `gwz-core` `018176c6783d900570ba8f918aa744bd642fc3b6`; `gwz-transport` `d7051eede48f6cd8b07815edc9ced1916f82585c`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `a3dc6989aa769dec65e41f39d7e53a8febb4e916`. Committed sources were read with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Code — focused original-counterexample closure and changed-range interface/error-path review. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — both original P2 findings are closed, but one new P2 error-domain defect blocks. I pre-commit to GO on a revision that resolves P2-3 as specified.

---

## 0. Evidence base

I read the prior Code report, merged RemPlan, transport range `f6e9ee2..d7051ee`, core range `c0649c3..018176c`, and exact committed versions of:

- `src/mux/{mod.rs,routing.rs,asynchronous.rs}`, `src/binding.rs`, and `src/codec/validate.rs`;
- `tests/mux.rs`, including all new terminal and rejection regressions;
- the corrected README, PlacementA checkpoint, PlacementDesign bootstrap/lifecycle requirements, and embedding guide.

The correction changes six transport files and the core checkpoint; schema and Taut are unchanged.

The permitted external-target suite passed 44 tests: `mux` 23, `mux_async` 3, `placement_v2` 10, and `async_stream` 8.

All nine pinned HEADs and trees matched at start and end. Member trees remained clean. Current-round untracked prompts/report output were excluded and unread.

## 1. Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| P2-1: `finish` could delete an admitted terminal before port transfer | **Closed** | `Queue::discard` now retains every operation terminal; `finish` checks pending outbound and local-action terminals; the first seal fixes a cleanup deadline; stalled handoff closes the generation. Direct and async tests cover all five terminal families, exact facts, timeout terminals, local action transfer, saturation, repeated finish, and dropped ports. The original `send terminal → finish → missing next_message` sequence now returns `WouldBlock`, preserves the terminal, and permits finish only after transfer. |
| P2-2: typed `BindRejected` collapsed into generic closure | **Closed** | Endpoint negotiation now enters `Rejecting`, queues `BindRejected`, admits no new work, and retires after transfer or deadline. Initiators retain the failure across closure; `ready` returns `Rejected` and `bootstrap_failure` exposes it. The original SSH/default-limits counterexample with `metadata_bytes=128` now produces exact `UnsupportedOperation`, with no binding, route, or endpoint work. V1-only negotiation produces `UnsupportedVersion`; malformed Bind remains Protocol with no invented rejection. |

## 2. Finding

### [P2-3] `BindRejected` accepts operation-time failure codes outside its frozen semantic domain

`src/codec/validate.rs:91-95` rejects only facts and `RepositoryRefused` for `BindRejected`. `src/mux/routing.rs:160-167` additionally checks only `Effect::None` and absent facts before retaining the failure as the authoritative bootstrap outcome. Consequently, a validly encoded bootstrap reply carrying `Failure { code: Authentication, effect: None, facts: None }` passes admission, closes the session as `Rejected`, and is returned verbatim by `bootstrap_failure`.

That state sequence requires no credential lookup or network effect, yet reports an authentication failure. The same path accepts `Io`, `Trust`, `CarrierLost`, `Timeout`, and other operation-time codes. This violates `GwzRemoteTransportDesign.md:96-109`, which restricts `BindRejected` to unsupported version/capability/limit outcomes, and makes the newly exposed typed result capable of driving a false public error classification.

This is a new changed-range root: the old implementation erased every rejection, while correction 1 makes the exact peer-supplied code observable and authoritative without constraining its message-specific domain.

Centralize `BindRejected` validation so only the frozen bootstrap dispositions are admitted—currently `UnsupportedVersion` and `UnsupportedOperation`, with `Effect::None` and no facts. Apply the same predicate to locally generated rejection and received envelopes. Invalid local endpoint configuration must fail locally rather than emit an out-of-domain peer rejection.

Closure tests should inject `Authentication` and `Io` `BindRejected` replies and require Protocol closure with no retained `bootstrap_failure`, while confirming both permitted codes survive Rejecting, port transfer, closure, and async readiness.

## 3. Changed-range invariant analysis

Terminal ownership now survives sealing on both outbound port and inbound action paths, remains queue-bounded, and has bounded failure on stalled handoff. Rejecting cannot install authority or accept new requests; transfer and deadline retirement preserve the exact valid failure. Malformed Bind remains distinct. Queue reservation, request/session correlation, v1/v2 compatibility, and the unchanged schema/reader boundary were not regressed.

## 4. Risks and next action

No Batch B facade, physical carrier, platform qualification, activation, or HTTPS behavior was inferred. Apply the bounded `BindRejected` code-domain validation and focused negative tests, then perform a final changed-range Code re-verdict.
