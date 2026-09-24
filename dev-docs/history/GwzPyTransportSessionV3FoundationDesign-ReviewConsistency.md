# Python transport session v3 foundation — CONSISTENCY-AXIS REVIEW

**Review object:** Committed DRAFT `dev-docs/GwzPyTransportSessionV3FoundationDesign.md`, with the committed core v3 paragraphs and Python caller-guide note.  
**Baseline:** root `b35ea74bf7e73c15777a3e0fb18587d05faffb17`; gwz-core `58e25012449ee8f4609daba4939e7157e99ea488`; gwz-py `d29d450508138bda9251e797afdb21d71d20d8dc`.  
**Date:** 2026-09-24. **Axis:** Consistency. **Method:** Independent, read-only review of committed files. All three HEADs matched the baseline at the start and end. Unrelated working-tree changes were excluded.

**Verdict: NO-GO — 0 P0, 0 P1, 4 P2, 1 P3.** The findings concern contract exactness and the proposed core API boundary. No implementation or test gate was run.

## 0. Evidence base

I compared the [v3 draft](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md), the accepted [v2 contract](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV2Design.md) and [third verdict](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV2Foundation-Verdict-3.md), the committed v3 additions to core [design](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md) and [requirements](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZRequirements.md), the Python [caller guide](/Users/owebeeone/limbo/gwz-dev/gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md), and committed core [runtime](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/mod.rs:185), [session](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/session.rs:449), and [request](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/request.rs:299) source. I also checked the mux [registration](/Users/owebeeone/limbo/gwz-dev/gwz-transport/src/mux/mod.rs:242) and [async owner](/Users/owebeeone/limbo/gwz-dev/gwz-transport/src/mux/asynchronous.rs:56) paths. The review followed `AgentProcessRules.md`, `GwzProcessOptimization.md`, and the read-only review-loop procedure.

## 1. Findings

### C-P2-1 — `call()` changes the accepted meaning of `Accepted` without an amendment

**Location and invariant.** V3 §7.2 says `call` runs `accepted.run(dispatch)` inline on its claiming thread ([line 196](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:196)); its phase table expressly permits the caller thread to own `Accepted` ([line 58](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:58)). The accepted v2 §5 definition requires a successful fallible native worker spawn and a worker parked at a gate before `Accepted` ([line 37](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV2Design.md:37)). V3 §10 says that meaning is unchanged and lists only three additive amendments ([lines 232–238](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:232)).

**Sequence and impact.** A `call()` can enter `Accepted` and run Git inline without the worker spawn and gate that v2 defines as admission prerequisites. The design cannot simultaneously preserve that definition and implement the stated inline path. Tests using `submit()` alone would not expose the mismatch.

**Correction and closure.** Amend v2 §5 expressly for inline `call()` ownership, including its slot, spawn-failure and gate semantics, or give `call()` the required parked worker. Add a `call()` acceptance test that proves whichever definition is adopted, including failure before any Git work.

### C-P2-2 — The core phase-2 API is called “not cancellable,” but its specified wrapper can drop it

**Location and invariant.** V3 §6 declares `Admission::register_and_open(self)` noncancellable and says it is driven to completion ([lines 145–155](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:145)). The proposed existing-caller wrapper directly awaits it with `admission.register_and_open().await` ([line 157](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:157)). The draft core requirement likewise says phase 2 **MUST NOT** be cancellable. An ordinary caller can drop the `request()` future at that await; the design gives that wrapper no owner that continues phase 2 or returns its `Admitted` variant.

**Sequence and impact.** Cancellation after the first insertion drops the wrapper’s future before it receives `Consumed` or `Ready`. The claimed variant-based provenance and finished-cleanup guarantee then do not apply to that caller. Native session code promising not to select against cancellation does not constrain other users of the public core API.

**Correction and closure.** Specify a core-owned phase-2 completion guard or task whose lifecycle survives caller-future drop, including where its result and cleanup go; alternatively narrow the API and requirement so every permitted caller has a proved non-dropping owner. Test dropping `request()` at each phase-2 suspension point after the first insertion, then verify ID consumption and bounded cleanup.

### C-P2-3 — A behavior-preserving `request()` wrapper is contradicted by its new cleanup timing

**Location and invariant.** V3 §6 calls `request()` a behavior-preserving thin wrapper and maps `Consumed(error, report)` to `Err(error)` ([lines 117 and 153–157](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:153)); both draft core paragraphs repeat that claim. Today, a failure from `begin()` or `ready()` propagates immediately through `?` ([core runtime lines 228–236](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/mod.rs:228)). Dropping the pending `TransportRequest` cancels and seals it, but does not await `finish()` ([core request lines 328–345](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/request.rs:328)). V3 requires phase 2 to await `finish()` before returning `Consumed`.

**Sequence and impact.** On a registered request whose `ready()` fails while cleanup is pending, existing CLI/local callers receive the error after scope drop; the proposed wrapper returns only after cleanup finishes. That changes observable latency and cancellation behavior, potentially by the cleanup bound. “Existing tests pass unchanged” in V3 §12.7 does not establish behavior preservation.

**Correction and closure.** State the intended compatibility boundary precisely. If cleanup-before-error is intended, amend the affected core caller contract and prove a bounded delay; if exact existing timing is required, design a separate candidate path without silently changing `request()`. Test a registered `ready()` failure with cleanup held and compare the wrapper’s return and cleanup timing.

### C-P2-4 — Phase 1 leaves the existing CLI placement branch undefined

**Location and invariant.** V3 §6 and the draft core paragraphs describe phase 1 as endpoint capacity admission plus endpoint and driver pre-checks, with `Admission` holding the existing admission leader ([V3 lines 139–153](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:139)). The current `request()` has a placement branch: CLI uses `state.cli` and **no local endpoint**; only local placement calls `endpoint.admit_client_request` ([core runtime lines 190–227](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/mod.rs:190)). The draft nonetheless says the wrapper preserves CLI behavior.

**Sequence and impact.** A CLI request has no endpoint capacity step or endpoint admission leader to put into the documented `Admission` token. Applying the described phase 1 literally changes CLI admission; skipping it leaves unspecified what serializes the CLI driver pre-check and registration. The contract does not define the token’s shape for a route that the wrapper must continue serving.

**Correction and closure.** Define placement-specific `Admission` ownership: local endpoint and driver checks versus CLI driver-only checks, with an explicit serialization boundary for each. Add a CLI request test alongside local placement that verifies registration, duplicate refusal, and unchanged capacity behavior.

### C-P3-1 — The caller guide turns a consumption fact into an immediate-retry promise

**Location and invariant.** V3 §10 defines `request_id_consumed` solely from registration provenance ([line 235](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:235)). Its fault matrix says a phase-1 cancellation after pool mutation closes the endpoint generation and later admissions refuse closed ([line 264](/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyTransportSessionV3FoundationDesign.md:264)). The committed caller-guide note says `False` means the same request ID “may be retried on a fresh handle now” ([line 53](/Users/owebeeone/limbo/gwz-dev/gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md:53)).

**Sequence and impact.** That cancellation yields `False`, but a fresh handle on the closed generation cannot be admitted now. The field truthfully reports nonconsumption; the guide overstates availability.

**Correction and closure.** Say `False` permits reuse **if the session is open and the admission condition has cleared**. Add a guide example or documentation check for the phase-1 post-mutation cancellation case.

## 2. Invariant analysis

Several attacks held. The committed core v3 paragraphs agree with V3’s intended `Refused`/`Consumed`/`Ready` provenance and prohibit inferring it from error text. The current core source confirms two registration sites: `ClientRequest::new` calls endpoint `Session::register`, then `RequestContext::new` calls driver `Session::register` ([request lines 18–24 and 351–357](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/request.rs:18)); `Session::register` inserts the lifetime ID only after mux registration succeeds ([session lines 680–704](/Users/owebeeone/limbo/gwz-dev/gwz-core/src/transport_host/session.rs:680)). A partial-registration provenance category is therefore warranted. The native `Claim` and `AcceptedOperation` ownership scheme also gives a coherent intended single-terminal path for ordinary `submit()` worker handoff, subject to implementation proof. The v3 caller-guide addition generally matches the draft’s pre-effect versus possible-effect terminal projections.

These observations do not close the four P2s: the accepted `Accepted` definition, a droppable core phase-2 future, existing `request()` timing, and CLI placement each need an explicit design decision.

## 3. Risks and next action

Resolve C-P2-1 through C-P2-4 in one bounded amendment to the v3 design and corresponding core draft paragraphs, and correct C-P3-1 in the caller guide. The closure tests above should be added to V3 §12. A corrected committed tuple needs a fresh consistency verdict because these decisions change the reviewed admission and ownership boundary.
