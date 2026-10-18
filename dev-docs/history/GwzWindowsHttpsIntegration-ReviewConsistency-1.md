# Windows HTTPS integration design — Consistency-AXIS REVIEW

**Review object:** `dev-docs/GwzWindowsHttpsIntegrationDesign-DRAFT.md` at root `1ddbfca026347c37deb135934d3b1af610aee781`. Focused design re-verdict of remediation 1 against previously reviewed root `85efff17673d919f12d93846447c6092efc11879`. Draft-stage boundary; no implementation or production activation acceptance.

**Baseline:**

| Repository | Revision |
|---|---|
| root | `1ddbfca026347c37deb135934d3b1af610aee781` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `1e2798ad9e017947a521dbacf3680944693d5e86` |

Documents and decisive source passages were read using `git show <exact-SHA>:<path>`. The complete corrected-document diff was inspected between the two root revisions.

**Date:** 2026-10-04

**Axis:** Consistency closure of the original caller-reachability counterexample and examination of the cohesive corrected ranges. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its current-round result. Filed verbatim by the lane owner.

**Verdict: GO** — original P2-1 is closed for the design. Zero open P0/P1/P2 findings; one new, nonblocking P3 documentation finding. No new architectural root cause.

---

## 0. Evidence base

Root and all seven named member HEADs matched the revised tuple at both the start and end of this review. Product member and evidence revisions remain unchanged from the original review.

Read:

- Root `dev-docs/GwzWindowsHttpsIntegration-RemPlan-1.md`, complete.
- Complete integration-design diff from `85efff17673d919f12d93846447c6092efc11879` to `1ddbfca026347c37deb135934d3b1af610aee781`.
- Corrected draft §3, particularly lines 96–114; amended CLI/Python matrix rows; complete new §9, lines 341–366.
- Core `src/transport_host/mod.rs`, lines 267–360: both request entries converge on `open_request`; the current host-bound backend assignment is at line 359.
- Core `src/git/gitbackend/backend.rs`, lines 40–69: existing constructors and context attachment.
- Core `src/git/gitbackend/transport_binding.rs`, lines 139–154: Disabled → WindowsDefault and AllowConfigured → WindowsConfigured.
- CLI `src/globalargs/dispatch.rs`, lines 11–36; source references identifying existing fetch dispatch and transport selection.
- Python `src/gwz/client.py`, lines 395–419 and 931–945: `fetch` uses bridge call; `fetch_stream` uses `_stream_call`, which uses bridge submit when available.
- Python `native/src/route/transport.rs`, lines 173–203: transport execution and the separate explicitly selected native route.
- Targeted source search for `request_kind`, confirming the symbol does not exist in the named transport-host source.

The original report’s controlling-document and private-receipt analysis remains applicable to unchanged ranges and member revisions. Private uncommitted fixture preparation/results were not inspected or used.

No files were written. No tests, builds, remote calls or Git mutations were performed. No current-round report from the other reviewer was read.

## 1. Findings

### Prior-finding closure table

| Prior finding | Original severity | Disposition | Closure evidence | Status |
|---|---|---|---|---|
| P2-1: WH1’s installed caller path depends on a helper-disable option that does not exist | P2 | Accepted; replace the false selector claim with fixed qualification-only backend construction | Corrected §3 explicitly constructs the existing Disabled backend with the same host context; §6 and §9 require actual caller-route and construction regressions. Existing source confirms the constructor and mapping. | **Closed for design** |

The corrected contract removes the original impossible dependency. It requires the shared host-bound request construction to use `Git2Backend::without_credential_helpers()` under exactly `all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`, followed by the existing `with_host_context`. Ordinary builds and explicitly selected native routes retain their existing construction.

The original counterexample therefore no longer holds under the revised design: installed CLI/Python qualification requests will acquire Disabled at their shared construction point and select WindowsDefault through the existing mapping. Explicit/forged configured Opens remain refusals rather than being reinterpreted.

This is design closure, not evidence that the construction has been implemented or executed.

### [P3-1] The correction misnames the existing shared request constructor

**Location:** Integration draft §3, line 103; remediation-plan disposition for P2-1.

**Violated invariant:** An exact-source implementation location must identify the real owning function, or explicitly identify a proposed new function.

The correction names `TransportRuntime::request_kind`. At the pinned core revision, `request` and `request_with_token` both call `TransportRuntime::open_request`, declared at `src/transport_host/mod.rs:286`. The cited backend assignment is inside that function at line 359. No `request_kind` exists in the named source.

**Reproduction:** Resolve the specified symbol against the pinned source. It cannot be found; resolving the accompanying file/assignment anchor instead reaches `open_request`.

**Impact:** The design and remediation record give an inaccurate symbol-level implementation/audit anchor. The accompanying file and assignment description make the intended location recoverable, so this does not reopen P2-1 or block the design.

**Required correction:** Replace `TransportRuntime::request_kind` with `TransportRuntime::open_request` in both records. If a rename is intended, state it explicitly; none is needed for this remedy.

**Closure check:** Read the corrected references against the pinned source and confirm that both request entries converge on the named function and its host-bound backend assignment.

**Classification:** Bounded documentation correction; no new architectural root cause.

## 2. Invariant analysis

### Changed-range analysis

The complete diff contains one cohesive correction:

- §3 removes the nonexistent helper-disable selector and defines a fixed qualification-only construction rule.
- §6 aligns the CLI/Python matrix with that rule.
- New §9 supplies existing installed caller entry points and distinguishes construction regressions from later live qualification.

No supersession paragraph, deadline, CBT, secret owner, cleanup owner, capability subset, WH2 boundary or budget was changed.

The corrected selection point is shared by the actual CLI and Python transport routes. `without_credential_helpers()` changes the backend’s helper policy while preserving its existing service composition; `with_host_context` then attaches the same request context. The design expressly preserves constructor defaults and nonqualification/native routes.

The prohibition on forged WindowsConfigured/Gh Opens remains independent of this fixed construction. The correction does not treat an explicit configured request as default-logon authentication, create a public policy knob or introduce an ambient reread.

The recipes use actual existing surfaces. CLI fetch is an existing dispatch. Python `Client.fetch()` invokes bridge call; `Client.fetch_stream()` invokes the existing streaming path and bridge submit. The revised recipe no longer requires a nonexistent `Client.submit` method.

§9 requires tests to observe the actual backend policy and attached context, exercise normal call/submit paths, preserve other-build/native behavior and refuse forged policies before effects. It also requires qualifying the existing diagnostic that says CLI/Python always enable helpers. These obligations make the correction reviewable without claiming present runtime success.

The original exact §9 supersession, absent SSH/helper construction, fixed D/zero policy, leaf CBT, retained cleanup and HTTP-versus-Git success analysis remains unchanged. No new contradiction was established in those boundaries.

## 3. Risks and next action

The corrected construction and caller recipes remain implementation and qualification obligations. No product source changed, and no new live TLS/auth/Git or installed-host evidence is accepted by this re-verdict.

The next action is to correct the two `request_kind` references to `open_request`, then record design acceptance and proceed with the bounded WH1 implementation under its required tests and settled-tree review. P3-1 is nonblocking. Full Windows activation and release remain NO-GO.