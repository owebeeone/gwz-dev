# Windows HTTPS integration design — Safety-AXIS REVIEW

**Review object:** `dev-docs/GwzWindowsHttpsIntegrationDesign-DRAFT.md` at root `1ddbfca026347c37deb135934d3b1af610aee781`, with `GwzWindowsHttpsIntegration-RemPlan-1.md`. Draft-stage, design-only remediation dated 2026-10-04.

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

The original reviewed root was `85efff17673d919f12d93846447c6092efc11879`. All seven member revisions remain unchanged. Documents and decisive sources were read with `git show <exact-SHA>:<path>`, supplemented by targeted `rg` inspection.

**Date:** 2026-10-04

**Axis:** Safety — focused review of the qualification-only policy construction, caller reachability, context ownership, refusal boundaries and preservation of other routes. Independent, adversarial, read-only. The prior-round Consistency report is an authorized input; no current-round parallel report was read. Filed verbatim by the lane owner.

**Verdict: GO** — the original Safety GO is confirmed. Zero P0/P1/P2 findings; one nonblocking P3 naming defect. This accepts the narrow design disposition, not implementation, runtime qualification, activation or release.

---

## 0. Evidence base

Read the complete remediation plan and original Consistency report. Inspected the corrected draft diff from the original reviewed root, covering §3, the CLI/Python matrix rows in §6, and new §9.

Retraced the unchanged source graph:

- Core `src/transport_host/mod.rs:270–359`: `request` and `request_with_token` converge on `open_request`; the host-bound backend assignment is at line 359.
- Core `src/git/gitbackend/backend.rs:1–95`: `new()` selects AllowConfigured; `without_credential_helpers()` selects Disabled; `with_host_context` retains that backend policy while attaching the transport context.
- Core `src/git/gitbackend/transport_binding.rs:130–167`: Windows Disabled maps to WindowsDefault; AllowConfigured maps to WindowsConfigured.
- Core `src/transport_host/local_command.rs:1–85`: normal CLI transport execution constructs a runtime request and passes its backend to the action.
- Core `src/transport_host/cancellable.rs:40–151`: Python’s cancellable execution obtains the backend through `request_with_token` and retains request/runtime cleanup.
- Python `src/gwz/client.py:373–420,927–958`: `fetch()` reaches bridge call; `fetch_stream()` reaches bridge submit when available, followed by event and result consumption.
- Previously inspected original-entry CLI/Python capture paths remain unchanged.

Beginning and ending `git rev-parse HEAD` checks matched every revision above.

No files were written. No tests, builds, remote operations or Git mutations ran. New uncommitted private fixture preparation/results were excluded.

### Prior-finding disposition

| Prior record | Focused assessment |
|---|---|
| Safety initial review: GO, zero findings | Retained. The correction does not weaken the previously reviewed safety boundaries. |
| Consistency P2-1: nonexistent CLI/Python helper-disable selector | The substantive design remedy is satisfied: fixed Disabled construction at the existing shared host-bound assignment reaches both callers without a new selector. Formal closure belongs to the original Consistency reviewer. The method-name defect below remains nonblocking because the file and assignment anchor identify the intended location. |

### Changed-range analysis

| Changed range | Safety result |
|---|---|
| §3: fixed qualification backend construction | Restricts the change to the exact Windows qualification predicate and retains the same RequestContext. No late source downgrade or helper fallback is introduced. |
| §6: CLI/Python construction assertions | Requires observation of actual backend policy and context through normal call/submit paths. Detached fixtures cannot satisfy those rows. |
| §9: installed caller recipe and regressions | Uses existing transport selection and fetch surfaces. Separates public construction checks from later real TLS/SSPI/Git qualification and requires preservation of other builds and explicit native selection. |
| RemPlan-1 | Records a bounded design correction, unchanged members and no implementation acceptance. |

## 1. Findings

### [P3-1] The corrected construction point names a nonexistent method

**Location:** Corrected draft §3 names `TransportRuntime::request_kind`; the remediation plan’s disposition repeats that symbol.

**Violated invariant:** A remediation identifying an actual construction point must name the existing method accurately.

**Reproduction:** At the unchanged core revision, inspect `src/transport_host/mod.rs:270–359`. Both `request` and `request_with_token` call `open_request`. The cited `Git2Backend::new().with_host_context(...)` assignment is inside `open_request`. There is no `request_kind` method.

**Impact:** The named symbol misdirects source navigation and later audit references. The accompanying file and assignment anchor identify the intended shared location, so this does not make the policy disposition unreachable or block the design.

**Required correction:** Replace `TransportRuntime::request_kind` with `TransportRuntime::open_request` in the draft and remediation plan. State that both ordinary request construction and cancellable request construction converge there.

**Closure/regression test:** Read the corrected text against the pinned source and verify `request` → `open_request` and `request_with_token` → `open_request`. The already required WH1 backend-policy/context tests remain the implementation regression obligation; this documentation correction needs no new runtime test.

## 2. Invariant analysis

**Actual caller reachability holds under the corrected proposal.** The shared backend assignment can use the existing Disabled constructor while preserving its RequestContext. Normal CLI transport dispatch and Python call/submit obtain their backend through that shared point. No application field or unavailable helper-disable selector is required.

**The change does not reinterpret a refused request.** Disabled is selected when constructing the qualification artifact’s backend, before mechanism selection. Explicit or forged WindowsConfigured/Gh Opens still refuse before effects. A configured request is not converted into current-logon authentication after refusal.

**Context and cleanup ownership remain attached.** `with_host_context` preserves the selected backend’s policy and installs the same context. The correction does not bypass registration, caller capture, cancellation, native worker provenance, fixed D, route retirement or retained cleanup charges.

**Other routes remain explicitly preserved.** The construction change is confined to `all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`. Constructor defaults, Unix, ordinary Windows, candidate-only Windows and explicitly selected native backends retain their current policy. Required guard regressions cover those exclusions.

**The installed recipe uses existing surfaces.** CLI fetch, Python fetch and fetch_stream/bridge submit are real paths. `GWZ_TRANSPORT=gwz` selects the transport without pretending to select authentication. Normal event/result consumption and independent refs/content verification remain necessary; construction tests cannot substitute for installed TLS/SSPI/Git execution.

No new architectural root cause or blocking safety defect was established in the changed ranges.

## 3. Risks and next action

The proposal remains unimplemented. Actual guard isolation, caller policy/context observations, installed worker provenance, TLS/EPA, Git traffic and cleanup races require the existing WH1/WH3 proof rows. No new private fixture result was used to close them.

The next action is to correct the method name while obtaining original Consistency closure, then proceed through the bounded WH1 implementation and its required checks. Full Windows activation and release remain NO-GO.