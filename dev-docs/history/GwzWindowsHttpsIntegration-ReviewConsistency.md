# Windows HTTPS integration design — Consistency-AXIS REVIEW

**Review object:** `dev-docs/GwzWindowsHttpsIntegrationDesign-DRAFT.md` at root `85efff17673d919f12d93846447c6092efc11879`. Draft-stage WH1 qualification boundary proposal, dated 2026-10-04; no production implementation or activation acceptance.

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

Controlling documents, decisive source passages and archived receipts were read with `git show <exact-SHA>:<path>`. Targeted `rg` searches supplemented those reads.

**Date:** 2026-10-04

**Axis:** Consistency against the controlling graph, internal promises, actual caller/source closure and evidence attribution. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. No P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves P2-1 as specified, without introducing an unreviewed public policy surface.

---

## 0. Evidence base

The complete generated Consistency prompt and complete integration draft were read. `AGENTS_GWZ.md` was read before inspection. Root and all seven named member HEADs matched the exact tuple at both the start and end of review.

Controlling documents inspected:

- Root `dev-docs/CurrentProgramCheckpoint.md`, particularly lines 3–82: current integration prerequisite status, provider preparation and composition acceptance.
- Root `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`: clock/source-selection sections; original caller contract, particularly lines 279–382; policy/facts contract, lines 390–493; lease/cleanup/effects, lines 495–535; complete §9, lines 537–594.
- Root `dev-docs/GwzSspiHttpsCompositionImplementationAcceptance.md`, lines 1–89.
- Root `dev-docs/GwzWindowsHttpsQualificationCheckpoint.md`, lines 1–128.
- Root `EVIDENCE.md`, lines 1–32.
- Root `dev-docs/AgentProcessRules.md`, particularly amendment, evidence, settlement, independent-review and finding rules at lines 255–288 and 357–449.
- Root `dev-docs/GwzProcessOptimization.md`, including physical spikes, budget amendments, review tiers and the 2026-09-28 ruling.
- Core `dev-docs/GWZDesign.md` and `GWZRequirements.md`: accepted composition clauses at lines 3–29, plus targeted transport, environment, off-switch and credential requirements.
- SSPI `docs/Architecture.md`, lines 1–105, and `docs/Testing.md`, lines 1–210.

Decisive production source inspected at the member revisions above:

- Core `src/lib.rs`, lines 1–128.
- Core `src/transport_host/mod.rs`, particularly lines 75–214 and 286–360; `session.rs`, particularly lines 116–140 and 345–429; `endpoint_environment.rs`, lines 1–192.
- Core `src/transport_host/local_command.rs`, lines 1–89; `cancellable.rs`, lines 1–155; `request.rs`, particularly lines 56–100 and 261–382.
- Core `src/git/gitbackend/backend.rs`, lines 1–150; `transport_binding.rs`, particularly lines 8–168; `transport_support.rs`, targeted credential-policy passages.
- Core `src/transport_host/request/https_failure.rs`, lines 115–153.
- Core `protocol/gwz.taut.py`, lines 1104–1130; `docs/GitBackend.md`, lines 23–44.
- CLI `src/globalargs/dispatch.rs`, particularly lines 11–36.
- Python `native/src/client_host.rs`, lines 159–262; `native/src/route/transport.rs`, lines 1–210; `native/src/shims.rs`, lines 1–61.
- Core endpoint `mod.rs`, `https_pool.rs`, `shared_reservation.rs`, `https_connection.rs`, `https_worker/prepare.rs`, relevant native bridge passages, and helper `runner.rs`, `file_worker.rs`, `owner.rs`.
- Core `src/session_host/environment.rs`, including its Windows ordinal case comparison and lossless conversion boundary.

Targeted searches across CLI source/docs, Python native source/docs and core transport-host source found no CLI/Python helper-disable selector. This absence is supported by the positive construction path and explicit source diagnostic described in P2-1, rather than resting on search absence alone.

Private campaign inspection used the exact evidence revision and run:

`campaigns/https-integration/runs/2026-10-04-windows-https-portability/`

Read material included:

- `README.md`, `baseline.json`, targeted input-manifest entries and `prototype-v3-hashes.json`.
- Versioned prototype generator/source passages, including v3 absent-engine construction, helper-refusal facade and WinHTTP read.
- `prepare_spike.py`, `remote_spike.py`, `verify_inputs.py` and unexecuted `native_verifier.py`.
- Provisioning, symlink-copy, preparation, forced-visibility, prototype-v1/v2/v3 and input-readback receipts.
- Raw compile diagnostics and native readback output.

Recorded outcomes agree with the draft: initial provisioning exit 2; forced-visibility compile exit 101 with 23 errors; prototype v1/v2 exits 101; prototype v3 exit 0 with 139 warnings; native readback of 14,435 source files and 18 patched files with no mismatches; Windows 11 build 26200, ReFS and WinHTTP DIRECT.

No files were written. No tests, builds, remote calls or Git mutations were performed. Neither the other current-round prompt nor report was read.

## 1. Findings

### [P2-1] WH1’s installed caller path depends on a helper-disable option that does not exist

**Location:** Integration draft §3, lines 101–105, and dependent installed-caller obligations at lines 130–135 and §6 lines 234–235.

**Violated invariant:** Required qualification cases must be reachable through the actual accepted CLI/Python request surfaces. A design may not substitute a Rust-only backend constructor for a purported existing caller option, or silently reinterpret a configured policy that the same design requires to refuse.

The draft says the actual CLI/Python helper-disabled request policy already maps to WindowsDefault and instructs WH1 scenarios to use its documented option. Only the backend policy mapping exists.

The exact source graph is:

1. `Git2Backend::new()` sets `CredentialHelperPolicy::AllowConfigured` at core `src/git/gitbackend/backend.rs:41–49`.
2. `Git2Backend::without_credential_helpers()` supplies Disabled at lines 52–60.
3. The shared actual transport request constructor unconditionally uses `Git2Backend::new().with_host_context(...)` at core `src/transport_host/mod.rs:359`.
4. CLI dispatch runs its action with that request backend through `with_local_transport_native`; Python call/submit runs with the same request backend through `with_cancellable_local_transport_native`.
5. Windows policy mapping at core `src/git/gitbackend/transport_binding.rs:144–149` maps AllowConfigured to WindowsConfigured and Disabled to WindowsDefault.
6. WH1 admits WindowsDefault and explicitly refuses WindowsConfigured, including Negotiate-only cases.
7. The production diagnostic at core `src/transport_host/request/https_failure.rs:140` expressly states that the CLI and gwz-py never construct the backend without credential helpers. The public Rust guide documents the constructor at `docs/GitBackend.md:25–27`; it is not a CLI/Python option.

**Reproduction/state sequence:** Implement the draft’s qualification cfg, absent SSH/helper boundary and original-entry caller capture, preserving its stated existing request surfaces. Run an installed qualification CLI fetch or Python call/submit against a local server requiring NTLM or Negotiate. The actual request backend remains AllowConfigured, so it selects WindowsConfigured. The required WH1 admission rejects that policy. No documented CLI/Python helper-disable option can select WindowsDefault. Removing configured Git helpers does not change the backend enum, and Negotiate-only does not rescue the case because the draft explicitly requires its configured-policy refusal.

**Impact:** The advertised WH1 default-logon path is unreachable through both required installed caller routes. A detached Rust fixture could succeed while the CLI/Python qualification matrix remains unsatisfiable. Fixing this during implementation would require an unrecorded policy disposition or new caller surface.

**Required correction:** Replace the false existing-option claim with an explicit, reachable qualification-only construction rule. One bounded solution is to specify that the qualification runtime’s actual request backend is constructed using the existing `without_credential_helpers()` constructor and receives the same host context, while ordinary Windows and Unix retain their existing construction. Record this as the limited qualification artifact’s policy disposition; preserve refusal of explicit/forged WindowsConfigured Opens.

If a selectable CLI/Python policy is desired instead, define its actual surface, defaults, transport and schema implications, amend the scope accordingly and obtain the required Surface review. Do not claim such a selector already exists.

**Closure/regression test:** The revised design must identify the exact construction/selection point and an executable caller recipe. Subsequent WH1 tests must exercise normal CLI dispatch and Python call/submit, assert that their actual host-bound backends select Disabled → WindowsDefault, and prove that configured/Gh Opens still refuse before effects. Preserve ordinary Windows, candidate-only Windows, Unix and explicit native-route behavior. WH3 must then execute the installed default-logon cases through those same routes.

## 2. Invariant analysis

The other attacks did not establish independent defects.

**Exact supersession and isolation.** The draft’s start/end quotations identify precisely the first paragraph of accepted composition §9, lines 539–546. Its qualification predicate requires Windows, candidate and qualification cfg together; illegal qualification configurations are explicitly compile-time refusals. Ordinary Windows, candidate-only Windows and existing Unix candidate selection have distinct required preservation rows. The amendment does not claim ordinary Windows activation.

**Actual dependency closure and accounting.** Source inspection confirms the stated Unix/helper seams, unconditional SSH construction and legacy SSH budget coupling. HTTPS uses the existing generic pool and shared reservation owner under SSH names. The proposed extraction preserves that ledger and makes physical SSH absent. It does not authorize a successful placeholder resource or Windows selection of the non-Unix no-op process-group killer.

**Capabilities and refusal.** HTTPS-only Anonymous/WindowsDefault offers, Bound intersection, public capability projection and Open validation are required to agree. SSH/configured/Gh refusals are pre-effect, and native transport fallback is prohibited after qualification-route selection. P2-1 concerns reaching the admitted policy from real callers, rather than a missing refusal rule.

**Caller, deadline and CBT contracts.** The original-entry capture locations agree with accepted composition and source: CLI dispatch before runtime/fanout; Python native entry before detach, admission waiting or submit handoff. Immutable D, native zero refusal, separate allocation/helper/active-I/O clocks, final-origin leaf CBT and generation binding preserve the accepted contract. Synthetic provider bindings are explicitly distinguished from actual Schannel/EPA proof.

**Completion and cleanup.** The draft keeps authoritative mechanism selection, native Complete and accepted HTTP response independent. It requires physical lease discard and route retirement on terminal control, retained real cleanup charges and rejection of late results. Receive-pack effect-before-POST and post-byte terminal behavior remain intact. Real Git refs/content verification prevents static HTTP success from being counted as Git success.

**WH2/WH3 sequencing.** Configured helpers, contained helper descendants and path/config transfer have a separate WH2 contract and physical spike gate. Default-logon WH3 may follow WH1; configured rows wait for WH2. Real TLS/auth/Git and installed-host observations remain qualification obligations. Neither provider preparation nor v3 compilation is presented as completing them.

**Evidence and scope.** Archived receipts support the stated compilation/readback observations and retained failures. V3’s empty unused SSH home, refusal-only helper facade, warnings and missing runtime/build rows are disclosed as characterization limitations. The inbound verifier remains explicitly unexecuted. Budgets, stop triggers, private evidence placement, external outputs, unchanged trust/account/proxy policy and public-CI independence are stated consistently.

## 3. Risks and next action

Remaining physical obligations are substantial: final neutral-budget/absent-helper construction, guard negatives, originating-entry DIRECT capture, native environment cases, strict targeted checks, actual TLS/EPA, Git traffic and installed caller provenance. The current receipts do not close those obligations. The draft labels them accordingly, so their present absence is not an additional finding against this proposal.

The single next action is a bounded design correction resolving P2-1 by specifying how actual qualification CLI/Python requests acquire the admitted WindowsDefault policy. Return that revision to the original reviewer for closure before treating this draft as the implementation contract. Full Windows activation and release remain NO-GO.