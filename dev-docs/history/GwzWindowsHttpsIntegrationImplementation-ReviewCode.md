# Windows HTTPS WH1 implementation — CODE-AXIS REVIEW

**Review object:** WH1 implementation diffs in core `c011aaee864fbe56c12a30b17664c099b8e67512..398158b3272e6f3a69132f8375190945dd93192a`, CLI `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311..6ab16d461acb6daf9fca8281eca6384971cc0c44`, Python `e0c5af10b33289a455f662680af8ac12fd24f9d3..5df15766298fbbd97da1d6ecec74c6cc9dd69fda`, and root `dev-docs/GwzWindowsHttpsIntegrationImplementationCheckpoint.md` at `48a71cf9516ae2887ed3735b27ed5b0416eaa6eb`. Committed implementation awaiting acceptance, 2026-10-04. Limited WH1 acceptance only; full Windows release is outside this verdict.

**Baseline:**

| Repository | Exact reviewed HEAD |
|---|---|
| root | `48a71cf9516ae2887ed3735b27ed5b0416eaa6eb` |
| gwz-core | `398158b3272e6f3a69132f8375190945dd93192a` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `f607e4fec7f38ab09407a457a47a99149076988d` |

Sources were read through pinned Git diffs, `git show HEAD:`, and bounded reads of tracked files. Core had only the explicitly excluded untracked bug report; CLI and Python status were clean. The complete tuple was checked at the start and end and did not move.

**Date:** 2026-10-04

**Axis:** Code — architecture, interfaces, call graphs, compatibility, and error/publication contracts. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block limited WH1 acceptance. No P0, P1, or P3 finding is raised. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

Inspection only. No build, test execution, remote call, file write, or Git mutation was performed.

Authority read:

- Root `AGENTS_GWZ.md`, `EVIDENCE.md`, and applicable core, CLI, SSPI, transport, and evidence-member instructions.
- `AgentProcessRules.md` L1-16–L1-20, L1-32, and relevant toolchain/compatibility rules; `GwzProcessOptimization.md` review-tier, budget, and unchanged load-bearing rules.
- `CurrentProgramCheckpoint.md:1–170`, particularly the live WH1 status and explicit release deferrals.
- `GwzWindowsHttpsIntegrationDesign-DRAFT.md` §§1–4 and qualification/budget/installed-caller clauses; integration Acceptance and BudgetDisposition.
- `GwzSspiHttpsCompositionDesign-DRAFT.md` §§1–5, especially §4’s fixed-clock and publication requirements; composition Acceptance.
- Core `GWZDesign.md:1–50` and `GWZRequirements.md:1–50`, carrying the accepted WH1 and composition obligations.
- The complete WH1 implementation checkpoint, including its RED gates and evidence limitations.

Implementation inspected:

- The three complete implementation diff inventories and targeted production diffs across selection guards, backend binding, optional SSH settings, helper exclusion, endpoint environment, caller capture, Session construction, request construction, and pump routing.
- Core `transport_host/mod.rs:145–419`, `session.rs:324–454`, `session/driver/opening.rs:1–343`, `session/driver.rs:1–85`, and `session/driver/pump.rs:1–210`.
- Core `transport_host/https_endpoint.rs`, particularly construction, Open admission, and `before_handoff` at lines 292–306; `https_endpoint/poll.rs:1–213`; `https_endpoint/retry.rs:1–307`.
- Core `git/gitbackend/transport_binding.rs:1–330`, native identity refusal, and actual Disabled-to-WindowsDefault mapping.
- Core `git/endpoint/https_worker/prepare.rs:1–430`, `native.rs:300–540` and `620–1123`, relevant serving/effect handling, route ownership, and final-origin TLS binding capture at `https_connection.rs:436–477`.
- Transport mux Open/deadline handling in `mux/mod.rs:300–380` and `mux/routing.rs:1–105`.
- Python `client_host.rs:1–260` and `route/transport.rs:1–200`; CLI entry/dispatch diffs.
- Public qualification tests, source-predicate tests, fixture-enclosure diffs, and portable physical-cleanup fixture.

Private evidence inspected, access required:

`gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-https-portability/`

- Complete committed README and bounded native-build/client/Python runner inspection.
- Named raw receipts for qualification tests v1/v2, final CLI build, Python package v1/v2 and installation v2, ordinary/candidate-only Windows library checks, illegal-qualification refusal, matched CLI v2, installed Python v4, mismatched CBT, untrusted chain, and hostname refusal.
- Original native fixture compilation errors and Python `WinError 145` packaging cleanup failure remain present.
- Qualification-test output reports 2 passed and 2,277 filtered. Matched CLI output independently reports checkout/fetched/pushed-ref agreement. Installed Python v4 reports ordered streaming, completion with unconsumed events, and `pending_local_work=0`, with `peer_cleanup_confirmed=false`.

These receipts were inspected as recorded evidence, not independently rerun. The source counterexamples below are reproducible state sequences, not claimed executed tests.

## 1. Findings

### [P2-1] The fixed native deadline is lost before actual `Opened` publication

**Location:** Core `src/transport_host/https_endpoint/poll.rs:21–77`; `src/transport_host/https_endpoint.rs:292–306`. Supporting paths: `https_endpoint/retry.rs:245–271`, `git/endpoint/https_worker/prepare.rs:340–376`, and `src/transport_host/session/driver/pump.rs` at the `before_handoff` → `owner.send` sequence.

**Violated invariant:** Accepted composition §4 requires a result obtained before D but not yet eligible for core publication when D expires to be rejected. Taking a receipt is not publication; this endpoint explicitly documents successful mux send as the `Opened` linearization point. WH1 retains this invariant even though broader integrated adversity execution is deferred.

**Counterexample:**

1. A WindowsDefault advertisement owns positive fixed D.
2. TLS, native authentication, accepted HTTP response, and native Finish complete shortly before D. The worker’s final time check passes, and its preparation task returns `Ok(Prepared)` together with the `Retry` containing `budget.logical_deadline`.
3. Delay the endpoint/session pump until after D, or hold its pending outbound slot through mux backpressure until after D. Keep cancellation false and remain below the separate Session opening backstop.
4. `HttpsEndpoint::step` polls the completed task. Its success arbitration checks cancellation, shutdown, and retirement, but never checks `retry.budget.logical_deadline`. It creates `Opened`.
5. `before_handoff` checks cancellation only. The mux accepts the late `Opened`.

There is no equivalent lower guard: mux Open routes are created with `Kind::Opening, None` in `gwz-transport/src/mux/mod.rs:339` and `mux/routing.rs:47`. The Session opening backstop is a much longer retry/cleanup bound, not D.

**Impact:** The actual qualification path can report and consume a successful native Open after its fixed authentication deadline. Delayed scheduling or message backpressure extends the accepted deadline at the publication boundary. Passing provider completion or normal clone/fetch/push receipts does not close this case.

**Required correction:** Retain the native logical deadline with the endpoint’s pending opening state through successful publication. Arbitrate expiry using fresh time when collecting a completed preparation result and on every pending `Opened` handoff, including after backpressure. Equality with D is expired. Expiry must publish Timeout with preserved facts, revoke the authenticated route, cancel/discard retained physical work, and retain cleanup charges until actual disposal. Keep anonymous and existing nonnative timing semantics unchanged.

**Closure/regression:** Exercise the real endpoint/mux path with successful preparation before D, then hold collection or successful mux handoff until D and beyond. Assert no `Opened`, one Timeout terminal, revoked route/generation, and retained physical cleanup capacity until disposal. Include a pre-D success control and a backpressured handoff retry.

### [P2-2] The public SSH constructor succeeds without an engine and advertises usable HTTPS

**Location:** Core `src/transport_host/mod.rs:186–187`, `212–248`, and `287–326`; `src/transport_host/session.rs:368–443`.

**Violated invariant:** WH1’s advertised capabilities, Bound offer, and usable implementation must agree. Unsupported SSH construction must represent absence truthfully; a successful runtime with neither engine cannot advertise HTTPS merely because the binary has the qualification cfg. Preserving Unix public constructors does not require accepting an unsupported Windows construction.

**Counterexample:**

1. In the exact Windows qualification build, construct the existing public `SshEndpointConfig` directly with ordinary pool settings and any `PathBuf` home. The public fields allow this without calling `from_environment`.
2. Call `TransportRuntime::new(config)`.
3. `new` passes `https=None`. Windows excludes the SSH construction block, so both `state.engine` and `state.https` remain absent.
4. Construction still returns `Ok(TransportRuntime)`. The endpoint’s cfg-based Bound offer declares HTTPS plus Anonymous/WindowsDefault, and `capabilities()` reports the same regardless of `state.https=false`.
5. A normal runtime request can bind successfully against that offer. Its HTTPS Open then reaches the pump with no HTTP engine and closes the Session through `EndpointError::InvalidRequest`.

`SshEndpointConfig::from_environment` correctly refuses Windows qualification, but it does not protect this public direct-construction path. The new qualification tests always supply `Some(HttpsEndpointConfig)` and do not cover it.

**Impact:** A callable public constructor produces a success-shaped unusable runtime and false capability negotiation. Callers receive a later Session failure for an operation that the runtime just advertised as supported. This also contradicts the checkpoint’s unqualified claim that the qualification endpoint’s offers describe the actual HTTPS path.

**Required correction:** Refuse the legacy SSH-only runtime construction under the exact Windows qualification predicate with typed UnsupportedOperation, before creating endpoint/session/link ownership. Enforce the same invariant at the shared construction boundary so `https=None` cannot produce an engine-free qualification endpoint. Preserve the existing constructor and behavior on Unix; do not add a caller flag or configure a synthetic SSH engine.

**Closure/regression:** In a native qualification test, directly construct the public `SshEndpointConfig` and assert `TransportRuntime::new` refuses with UnsupportedOperation before runtime ownership starts. Cover the shared `https=None` construction path. Retain a Unix constructor-success regression and the existing qualification HTTPS construction/Bound/capability agreement tests.

## 2. Invariant analysis

The following attacks did not establish additional defects:

- **Selection isolation:** Core, CLI, and Python use the valid Unix-candidate/Windows-qualification union. All three contain explicit illegal-qualification compile guards. Ordinary and candidate-only Windows keep the prior route; inspected native receipts distinguish those builds.
- **Actual backend policy:** Shared `TransportRuntime::open_request` explicitly constructs `without_credential_helpers()` in the exact qualification predicate and attaches the same RequestContext. Existing constructor defaults remain AllowConfigured. The mapping produces WindowsDefault, and ordinary explicit native-route backends retain their behavior.
- **Unsupported effects and fallback:** Qualification policy checks precede worker preparation effects. Session SSH admission refuses before URL/helper/network work. Host-bound backend identity validation rejects non-HTTPS remotes, with a defensive smart-transport refusal rather than native fallback.
- **Neutral ownership:** Private endpoint settings separate pool/I/O budgets from optional SSH settings. No replacement reservation ledger or fake HOME is introduced. Unix helper ownership is enclosed; Windows credential lookup refuses instead of creating a successful no-op owner.
- **Original caller and environment:** CLI captures NativeCaller before local runtime handoff. Python call/submit share entry capture before detach or operation-thread creation. Proxy DIRECT disposition travels with that captured caller. Existing EnvironmentSnapshot uses lossless Windows encoding and ordinal case-insensitive name matching.
- **WinHTTP admission:** Output storage is initialized. Admission requires successful NO_PROXY with neither returned named pointer, and both returned pointers are freed even on failure. No proxy-setting mutation appears in the inspected path.
- **TLS/authentication:** Final-origin TLS provides the prefixed leaf binding. Native authentication requires a finite D, validates native mechanism/completion separately from accepted HTTP response, and retains one exclusive scoped physical generation. The publication gap in P2-1 is the identified exception.
- **Cleanup and effects:** Existing Start/session/Finish retention and conservative cleanup accounting are reused. Receive-pack marks possible effects before POST; inspected retry classification does not promote auth/body/post-byte failures into setup retry. Portable blocked-disposal coverage remains compiled outside the Unix-only fixture enclosure.
- **Compatibility and evidence honesty:** No new caller option, application message shape, or generated protocol change appears in these diffs. The checkpoint distinguishes provisioned CLI, installed wheel, normal successes, TLS/CBT negatives, and unexecuted adversity. It retains original failures, the explicit nonincremental packaging condition, strict-Clippy RED45, and the owner-IR mismatch.

## 3. Risks and next action

Broader integrated cancellation/deadline/identity-transition qualification, WH2 helpers, provider parity, ordinary activation, and full release gates remain explicitly deferred. They are not additional findings here. The inspected positive receipts support the bounded normal-path claims, but do not cover either counterexample.

The next action is bounded remediation of P2-1 and P2-2 with the stated production-path regressions, followed by this reviewer’s closure on a newly pinned tuple. Limited WH1 acceptance remains NO-GO until both close.