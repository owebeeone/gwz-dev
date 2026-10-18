# Windows HTTPS integration design — Safety-AXIS REVIEW

**Review object:** `dev-docs/GwzWindowsHttpsIntegrationDesign-DRAFT.md` at root `85efff17673d919f12d93846447c6092efc11879`, dated 2026-10-04. Draft-stage WH1 qualification boundary proposal; no production implementation or activation accepted.

**Baseline:**

| Repository | Revision |
|---|---|
| root | `85efff17673d919f12d93846447c6092efc11879` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `1e2798ad9e017947a521dbacf3680944693d5e86` |

Committed document, source and receipt extracts were read using `git show <exact-SHA>:<path>`. `rg` supplied source navigation and targeted inspection. All eight HEADs matched this tuple at both the beginning and end.

**Date:** 2026-10-04

**Axis:** Safety — attack degraded configurations, effects, disclosure, ownership, cleanup, qualification isolation and scope expansion. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings established against this draft-stage boundary. This verdict does not accept implementation, Windows runtime qualification, activation or release.

---

## 0. Evidence base

Read the complete controlling draft, §§1–8, including the exact §9 supersession, three work packages, retained invariants, physical-spike requirements, qualification matrix, budgets and characterization limits.

Inspected the relevant authority:

- Workspace `AGENTS_GWZ.md` and member instructions for core, CLI, SSPI and evidence.
- `AgentProcessRules.md`, particularly its authority, baseline, characterization, amendment, evidence and freeze rules; `GwzProcessOptimization.md`, including physical characterization before freeze and review granularity.
- `dev-docs/CurrentProgramCheckpoint.md`, current Windows integration and native-provider preparation entries. The canonical prompt’s root-level checkpoint spelling resolves to this tracked path.
- `GwzSspiHttpsCompositionDesign-DRAFT.md`, §§1–9, especially original-entry capture, immutable D, source selection, final-origin CBT, success predicates, cleanup and the existing Unix activation boundary.
- `GwzSspiHttpsCompositionImplementationAcceptance.md` and `GwzWindowsHttpsQualificationCheckpoint.md`.
- Core `GWZDesign.md` and `GWZRequirements.md`, relevant native composition, transport selection and no-fallback requirements.
- SSPI `docs/Architecture.md` and `docs/Testing.md`; these are the tracked locations of the prompt’s Architecture/Testing references.
- Root `EVIDENCE.md`.

Source inspection covered:

- Core `src/lib.rs`, `src/git/mod.rs` and `gitbackend/transport_binding.rs`: current enclosing Unix candidate guards and host-context/native-route distinction.
- CLI `globalargs/dispatch.rs:1–140`: capture before transport runtime construction and backend dispatch.
- Python `client_host.rs:150–260` and route capture references: capture before route handoff, registration and operation admission.
- Core `transport_host/session.rs:290–445`, `transport_host/mod.rs:55–245` and `endpoint_environment.rs:1–245`: mandatory SSH construction, shared budgets, advertised capabilities, environment capture and the unsupported platform arm.
- `shared_reservation.rs:1–155`: physical admission and reservation release only after successful disposal observation.
- `https_auth/owner.rs:235–285`: the non-Unix process-group cleanup no-op.
- `https_connection.rs:405–477`: final-origin TLS followed by binding capture before HTTP type erasure.
- `https_worker/prepare.rs:1–115` and native bridge extracts at `305–465`, `780–907`, `930–1007`, `1000–1113`: caller ownership, fixed deadline handoff, binding selection, native/HTTP completion, Finish and route publication.

At the pinned private evidence revision, inspected the portability run’s README, baseline, input-manifest excerpt, versioned prototype generators and selected prototype sources, provisioning/readback scripts, remote compiler runner, and unexecuted native verifier source. Inspected named raw receipts and relevant diagnostics:

- Initial provisioning exit 2 and separately recorded identical-content symlink adaptation.
- Forced visibility exit 101, with 23 compile errors.
- Prototype v1 exit 101, extra closing delimiter.
- Prototype v2 exit 101, remaining SSH setup dependency.
- Prototype v3 exit 0, candidate plus qualification cfg, with 139 warnings.
- Input readback: 14,435 source files and 18 prototype files, no reported content mismatches; Windows 11 build 26200, ReFS, successful WinHTTP DIRECT read and returned-storage disposal.

These are archived observations, not tests rerun by this reviewer. Hash manifests were inspected; this review did not independently recompute every archived hash.

Only read-only inspection commands ran. No files, Git state, builds, tests, remote systems or OS settings were modified. The other current-round review was neither read nor requested.

## 2. Invariant analysis

**Qualification isolation and amendment authority held.** The proposed amendment identifies exactly one paragraph of composition §9 and confines its replacement to `all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`. It preserves ordinary Windows, candidate-only Windows and Unix selection, requires invalid combinations to fail compilation, and keeps release artifacts outside this gate. The prototype’s compilation is not treated as proof of guard negatives or caller selection.

**Unsupported mechanisms cannot acquire success-shaped ownership under the final contract.** Current source demonstrates why widening the guard alone is insufficient: endpoint construction creates SSH, budgets depend on its configuration, and helper containment has a non-Unix no-op. The draft explicitly requires absent SSH settings/engine, separate private budgets, excluded helper platform code and pre-effect refusals. It also explicitly disallows carrying the prototype’s empty SSH home or helper facade into final ownership semantics.

**Capabilities and refusals remain aligned.** WH1 admits HTTPS with Anonymous and WindowsDefault only. Bound, public projection and Open validation must agree. Forged SSH/configured/Gh requests must fail before credential consumption, worker launch, filesystem, agent, DNS or network effects. Configured policy remains unsupported even for a challenge that could otherwise avoid a helper; no reinterpretation silently widens authentication authority.

**Degraded installed-host paths remain fail-closed.** The retained contracts distinguish unavailable metadata, worker provenance mismatch and caller capture failure. Selecting native authentication consumes the retained refusal rather than recapturing an executor thread or falling back to PATH, in-process SSPI or native Git. Anonymous availability does not require native worker availability.

**Caller identity and deadline provenance survive handoff.** Existing CLI/Python sources place capture at the original entry. The draft requires enabling those actual routes, including Python before detach or submit. It preserves origin liveness, impersonation refusal and launch-time recheck. D remains immutable across pool reuse, discovery, 401 and native rounds; zero refuses native publication without inventing a fallback allowance. Allocation and active-I/O clocks remain distinct.

**Authentication and Git success remain separate.** Final-origin TLS supplies CBT after trust/hostname validation; neither proxy TLS, a root certificate nor synthetic fixture bytes may substitute. Native Complete, authoritative mechanism and accepted HTTP response are independent prerequisites. The Git matrix additionally requires real advertisement/pack traffic and independent remote ref/content verification. Static HTTP 200 and server-side Git plumbing cannot become client success or fallback evidence.

**Cancellation cannot manufacture disposal or replacement capacity.** Publication revocation and route retirement precede cleanup completion. Native Start/session/Finish, process/Job/I/O and physical pool charges remain retained until actual completion. Late tokens or HTTP results cannot revive terminal work. Unknown cleanup remains visible and charged; the draft does not promise that containment cancels LSASS work or proves erasure.

**Effect boundaries prevent replay.** Receive-pack retains Effect::Possible before POST, with no replay after bytes/effects. Fetch body failures remain terminal. Authentication failures do not acquire fresh-connect retry eligibility merely because they occur during the broader setup deadline.

**Evidence and sequencing claims are bounded.** Archived failures remain visible. V3 proves a library compilation question with disclosed warnings and temporary structures. It does not prove public tests, installed callers, runtime refusals, TLS/EPA or Git. WH2 containment/path work is separately frozen and reviewed; its configured rows cannot be admitted by WH1. WH3 obligations remain qualification exits.

**Disclosure and environmental limits remain explicit.** Secret owners are bounded and wiping; dependency/OS copies and forced-exit limits are disclosed. Evidence excludes tokens, credentials and private keys. DIRECT capture is read-only, non-direct configurations refuse, fixture roots cannot count as installed environment qualification, and system trust/account/service/proxy policy changes are outside authorization.

## 3. Risks and next action

The largest remaining risks are implementation and qualification obligations already named in the draft: complete transitive closure, invalid-cfg and ordinary-route preservation, native environment case handling, WinHTTP failure/storage paths, real Schannel/EPA, installed provenance, and cleanup races through actual callers. A successful library check closes none of those rows.

Unconfirmed cleanup may retain capacity indefinitely. That is an explicit conservative outcome, not evidence of disposal. Truly stalled provider work, Kerberos, Digest, proxy authentication, SSH/Pageant and full strict-Clippy debt remain separate gates.

The next action is owner disposition and implementation of the bounded WH1 qualification boundary, with the stated physical and deterministic checks completed before WH1 implementation acceptance. Full Windows activation and release remain NO-GO.