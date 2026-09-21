# GWZ Remote Transport Endpoint-Placement Design — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core` `5be22a1931160c2d124e0705cd516a4b88fdb098`, `dev-docs/GwzRemoteTransportPlacementDesign.md` and `docs/TransportPlacement.md`; draft design-only interface gate, 2026-09-22  
**Baseline:** root `cf34abc7b739c82af9f466665e0e514e166f5e35`; core `5be22a1931160c2d124e0705cd516a4b88fdb098`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; all commit and tree identities matched the prompt at the start and end of review.  
**Date:** 2026-09-22  
**Axis:** Consistency against the controlling requirements, design, plan, accepted N3 checkpoint, current schemas, and stated deferrals. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block the design freeze. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

I read the complete review prompt and both reviewed documents; `GwzRemoteTransportRequirements.md` placement, lifecycle, framing, and compatibility clauses; `GwzRemoteTransportDesign.md` §§3, 3.0, 3.1, 4.1, 4.1.1, 4.2, 10, and its acceptance matrix; `GwzRemoteTransportPlan.md` Phase 4; `GwzRemoteTransportSshN3.md`; and the root `CurrentProgramCheckpoint.md`.

I checked the pinned GWZ service schema around `GwzCore`, `TransportOptions`, `RequestMeta`, `ResponseMeta`, capabilities, and observations. I checked the pinned transport schema and generated/runtime definitions for `Failure`, `Closed`, `Envelope`, binding acceptance/verification, codec validation, and terminal stream handling. This confirmed that the proposed tag numbers are free, current `Closed` already owns required `Facts`, and current binding acceptance creates endpoint-side binding state before mux verification.

No build or test was run, as required for this design-only review. Owner-provided checks report 32 local Markdown links passing and clean diff whitespace. The workspace documentation checker has one unchanged, unrelated `gwz-cli/docs/Releases.md` compatibility-statement failure; this review does not claim that gate green.

## 1. Findings

### [P2-1] Failed `Closed` has two unspecified authorities for authentication facts

**Location:** `GwzRemoteTransportPlacementDesign.md` §6, lines 217 and 231–235; §6 lines 247–254; §7 lines 264–268. The pinned transport schema already defines `Closed.facts` at tag 3 and optional `Closed.failure` at tag 4 (`protocol/transport.taut.py`, lines 31 and 53–54; generated `src/protocol.rs`, lines 517–532 and 896–925). The amendment adds optional `Failure.facts` at tag 3 and expressly requires it on “failures inside Closed.”

**Violated invariant:** Each terminal stream outcome must have one deterministic, verified source for the current attempt’s facts, from which observations and typed failure projection are constructed.

**Counterexample:** A v2 endpoint sends a failed `Closed` whose existing `Closed.facts` says `authenticated=true`, while `Closed.failure.facts` says `authenticated=false` or reports a different method/offer state. Both values fit the proposed wire shape. The document neither requires equality nor identifies which source is authoritative. “Conflicting terminal outcomes” at lines 253–254 does not define a facts comparison or precedence rule. One layer can project observations from `Closed.facts` while the typed-failure adapter retains `Failure.facts`, yielding contradictory authentication evidence for the same stream.

**Impact:** Wire-valid peers can disagree about observed authentication state, reuse evidence, and failure context. Codec conformance and observation tests cannot have a single expected result.

**Required correction:** Define exactly one facts authority for failed `Closed`. Either prohibit/omit `Failure.facts` there and use `Closed.facts`, or declare `Failure.facts` authoritative and constrain the legacy `Closed.facts` value. If both remain populated, require exact semantic equality and reject disagreement before dispatch. State the rule in the codec/context validation and observation sections.

**Closure test:** Decode a failed `Closed` with equal, absent, and deliberately conflicting facts. Prove the admitted forms yield one facts value and the conflicting form fails the stream/session as the selected rule requires.

### [P2-2] Cancelling the bootstrap owner can strand endpoint and mux in different binding states

**Location:** `GwzRemoteTransportPlacementDesign.md` §4, lines 135–140. The controlling design §3.0, lines 107–123, requires the endpoint to mark a successful binding ready before sending `Bound`, while the mux installs it only after validating `Bound`; disconnect invalidates both sides.

**Violated invariant:** A carrier session must either have the same established binding at both endpoints or be retired; later work must not inherit half-established session authority.

**Counterexample:** Request R owns the first Bind. The endpoint accepts it and marks the session ready, then emits `Bound`. R is cancelled or its attachment delivery is lost before the mux validates that response. The amendment says bootstrap is abandoned and a late `Bound` cannot install it, but says neither that the carrier/session is closed nor how endpoint-side ready state is rolled back. Request S then arrives. It cannot “reuse the established binding” because the mux has none. Retrying Bind on the same session is also undefined, and a delayed response from R is indistinguishable at the transport bootstrap layer because Bind/Bound use stream 0 and contain no bootstrap generation.

**Impact:** A permitted cancellation race can deadlock later requests, cause inconsistent duplicate-Bind handling, or allow a stale acknowledgement to satisfy a later bootstrap implementation. The required cancellation/cleanup evidence is not satisfiable from the frozen state machine.

**Required correction:** Freeze one recovery rule. The bounded option is to retire/close the carrier session and endpoint binding whenever the owning bootstrap is abandoned, wake all waiters, and require a fresh session identifier before later explicit-CLI requests. Alternatively, specify a generation-bound idempotent retry/ownership-transfer protocol on both sides that cannot accept R’s acknowledgement for S.

**Closure test:** Pause after endpoint readiness but before mux receipt, cancel R, deliver its late `Bound`, and start S. Assert R’s acknowledgement cannot install, all R waiters terminate, endpoint and mux retain no usable old binding, and S succeeds only through the specified fresh-session or generation-safe path.

## 2. Invariant analysis

The remaining attacks held.

The v1/v2 split is internally coherent: bootstrap envelopes remain v1, `Bound.version` selects the highest admitted common profile, subsequent messages use that profile, v2 placement cannot downgrade, and v1 peers receive no v2-only kinds or fields. Proposed transport kinds, body tags, error code, GWZ field tags, and observation tags do not collide with the pinned schemas. Missing-field decoding is correctly treated as an implementation prerequisite rather than assumed from nullability.

Placement and authority remain consistent with the controlling graph. Omission selects local; explicit CLI placement requires both core capability and verified endpoint binding; unsupported or mixed routes fail whole-operation preflight; endpoint-local path interpretation does not expose key bytes to core; and Open repeats selected-key admission before pool lookup. The complete selected set is checked before mutation/network effects.

The current `GwzCore` service is unary in the client-to-core direction and its logs are one-way, so it is not itself the required duplex carrier. The amendment acknowledges that fact, defines host callbacks as the semantic A/B seam, forbids treating final responses or event logs as sufficient, and leaves real supplied-connection proof to batch C before activation. Therefore the absence of a present carrier is not a contradiction or missing implementation claim. The optional metadata fields alone must not be cited later as carrier evidence.

Repository-refusal classification is bounded to complete canonical evidence, ordered before clean EOF, and never inferred after loss. Public error projection, no replay, possible-effect handling, observation isolation, and retained local/default behavior agree with the controlling requirements and accepted N3 boundary.

## 3. Risks and next action

The real supplied carrier, platform/selected-source qualification, production activation, and HTTPS remain expressly deferred. No implementation or test acceptance follows from this review. The unrelated documentation-check debt remains outside this object.

Revise the design to establish one failed-`Closed` facts authority and a complete bootstrap-abandonment recovery transition, then run a focused same-reviewer re-verdict on those changes and their direct interactions.
