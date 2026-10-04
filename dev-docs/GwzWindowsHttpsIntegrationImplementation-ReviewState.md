# WH1 Windows HTTPS integration implementation — STATE-AXIS REVIEW

**Review object:** WH1 implementation diffs: core `c011aaee864fbe56c12a30b17664c099b8e67512..398158b3272e6f3a69132f8375190945dd93192a`; CLI `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311..6ab16d461acb6daf9fca8281eca6384971cc0c44`; Python `e0c5af10b33289a455f662680af8ac12fd24f9d3..5df15766298fbbd97da1d6ecec74c6cc9dd69fda`; root `dev-docs/GwzWindowsHttpsIntegrationImplementationCheckpoint.md`. Committed implementation, acceptance pending, 2026-10-04. Limited WH1 acceptance only.

**Baseline:** Sources were inspected through committed `git show HEAD:PATH` and the prescribed diffs. Repository HEADs were verified at both start and end and remained:

| Repository | SHA |
|---|---|
| root | `48a71cf9516ae2887ed3735b27ed5b0416eaa6eb` |
| gwz-core | `398158b3272e6f3a69132f8375190945dd93192a` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `f607e4fec7f38ab09407a457a47a99149076988d` |

**Date:** 2026-10-04

**Axis:** State machines, admission, refusal-before-effects, cleanup ownership, recovery legality and evidence attribution. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block limited WH1 acceptance. No P0 or P1 found. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified, with the stated regressions and no additional scope changes.

---

## 0. Evidence base

Inspection only: no builds, tests, remote calls, file writes or Git mutations were performed. The counterexamples below are source-derived deterministic paths, not newly executed runtime results. No current peer report was accessed.

Authority inspected:

- Root `AGENTS_GWZ.md`, `EVIDENCE.md`, and applicable core, CLI and evidence-member instructions.
- `AgentProcessRules.md`, especially L1-13 through L1-17, L1-19 and L1-20; `GwzProcessOptimization.md`, including settlement, review and remediation provisions.
- `CurrentProgramCheckpoint.md`, current WH1 entry.
- `GwzWindowsHttpsIntegrationDesign-DRAFT.md`, especially §§1, 3–7 and 9; its Acceptance and BudgetDisposition documents.
- `GwzSspiHttpsCompositionDesign-DRAFT.md`, deadline, caller, lease, binding and cleanup obligations; its Acceptance document.
- Core `GWZDesign.md` and `GWZRequirements.md`, accepted Windows HTTPS boundary and composition clauses.
- The complete WH1 implementation checkpoint.

Production and regression sources inspected include the prescribed core/CLI/Python diffs and these dependency paths:

- Core `transport_host/mod.rs:153–419`, `session.rs:345–475`, `session/capacity.rs:1–270`, `session/driver/opening.rs`, `session/driver/pump.rs:1–145` and request-retirement portion, `session/close.rs:1–72`, `session/local_link.rs:1–109`.
- Core `endpoint_environment.rs:1–119` and Windows WinHTTP platform section; `cancellable.rs`, `local_command.rs`, `qualification_tests.rs:1–152`, and portable cleanup-test changes.
- Core `gitbackend/transport_binding.rs:1–275`, identity guards, and changed helper/SSH boundaries.
- Core `https_worker/prepare.rs:1–180`, `budget.rs:1–74`, `native.rs:309–340,665–746,825–1018`, `https_pool.rs:1–190`, and final-origin TLS binding construction in `https_connection.rs`.
- Core `session_host/environment.rs:1–190`, including lossless snapshot ownership and native name lookup.
- Transport `pool/mod.rs:20–87`, establishing default capacity `(32,32,256,1024)`.
- CLI original-entry dispatch and transport selection; Python `client_host` capture and `route/transport.rs:1–205`.
- Public `test_windows_qualification_boundary.py`, including predicate truth-table, entry-order and shared-constructor assertions.

Private evidence inspected at the pinned evidence SHA:

`campaigns/https-integration/runs/2026-10-04-windows-https-portability/`

- Complete campaign README.
- `raw/wh1-refresh-v2.json`, identifying the eight refreshed source hashes.
- Qualification-test v2 receipt and stdout: two tests passed, 2,277 filtered.
- Matched CLI v2 stdout: independently checked clone/fetch/push results, four accepted NTLM contexts and four verifier disposals.
- Installed Python v4 receipt/stdout and relevant probe source: successful clone/fetch/push, ordered stream events, another call with unconsumed application events, `pending_local_work=0`, `peer_cleanup_confirmed=false`, six verifier disposals and reaped server.
- Mismatched-CBT v1, untrusted-chain v1 and hostname v1 outputs: refusal; TLS negatives recorded no native authentication rounds.
- Python package v1/v2 outputs: original exit 1 retained, final exit 0 retained.
- Illegal qualification v2 output: compiler exit 101 and recorded expected guard refusal.
- Relevant CLI/Python fixture commands: positive probes do not specify nondefault per-host capacity.

`git rev-parse HEAD` and each prescribed `git -C MEMBER rev-parse HEAD` matched the complete tuple at review start and end.

## 1. Findings

### [P2-1] HTTPS-only admission cannot install a different physical capacity

**Location:** `gwz-core/src/transport_host/session/capacity.rs:208–233`, reached from `transport_host/mod.rs:383–397`. The new absence of the SSH engine is established by `session.rs:371–396`.

**Violated invariant:** Private scheme-neutral budgets must work through the actual pool/Session path with SSH absent. An admitted existing concurrency policy must not require an unavailable SSH owner. Legal fresh or quiescent capacity transitions must remain possible without manufacturing that owner.

**Counterexample:**

1. Start the Windows qualification route with normal endpoint settings. The HTTPS pool exists; `state.engine` is `None`; installed capacity is the default `(32,32,256,1024)`.
2. Invoke an existing HTTPS operation with `RequestPolicy.max_connections_per_host=Some(1)`, such as the qualification CLI with `--max-per-host 1`.
3. `open_request` derives requested capacity `(1,1,256,1024)` and calls `admit_client_request`.
4. The runtime has no live work, so the conflict checks permit the transition. The installed-capacity equality shortcut does not apply.
5. `install_capacity` reaches the unconditional SSH-engine lookup at lines 208–211 and returns `Unavailable("SSH endpoint unavailable")`.
6. Request creation fails before its backend or HTTPS operation is established.

The same failure occurs when a later quiescent request changes capacity. Large valid jobs values that change `total` or `max_requests` also reach this branch.

**Impact:** Existing valid request policies fail on the newly selected qualification path. The default-limit positive receipts bypass this branch, so their success does not establish capacity handling. Refusal is fail-closed, but the HTTPS-only state lacks a legal transition supported by the existing interface.

**Required correction:** Make capacity installation operate on the actual present pools. Support an HTTPS-only owner using the existing HTTPS pool and shared authority, retaining retirement, pending-cleanup and cancellation guarantees. Preserve the existing paired SSH/HTTPS transition on Unix. Do not add a dummy SSH endpoint or replace the ledger.

**Closure/regression:** Exercise the real HTTPS-only Session with nondefault per-host capacity on its first request and with a changed capacity after a completed request. Verify the HTTPS pool and shared authority receive the limits; overlapping incompatible requests still refuse; pending disposal retains charges and blocks replacement; cancellation during retirement cannot leave a partially usable policy. Execute a native qualification CLI or installed Python case with a nondefault existing capacity option.

### [P2-2] The SSH-only public constructor creates an empty Windows endpoint that advertises HTTPS

**Location:** `gwz-core/src/transport_host/mod.rs:186–188,212–244,287–306`; `session.rs:371–429`; consequence at `session/driver/pump.rs:61–83`.

**Violated invariant:** Bound, capability projection and usable engines must agree. An unsupported construction must refuse explicitly; absence must not become a successful endpoint that advertises an engine it does not own.

**Counterexample:**

1. In a Windows qualification build, supply a valid `SshEndpointConfig` directly to the exposed `TransportRuntime::new`. This does not call the correctly refusing `SshEndpointConfig::from_environment`.
2. `new` calls `build(local, None)`.
3. The Unix SSH construction is excluded, and the `None` HTTPS argument constructs no HTTPS engine. Both `state.engine` and `state.https` are absent.
4. Nevertheless, endpoint construction succeeds and its qualification branch advertises HTTPS plus Anonymous/WindowsDefault. Public `capabilities` makes the same unconditional offer, despite `RuntimeState.https=false`.
5. With default capacity, request admission can pass the equality shortcut and bind that advertised endpoint.
6. An HTTPS Open is then dispatched to the missing HTTPS engine, returning `InvalidRequest` and closing the Session.

**Impact:** A newly reachable constructor accepts an unusable state and publishes false capabilities. A caller can bind successfully and discover the missing engine only through carrier failure. This is independent of P2-1: correcting capacity installation would still leave this empty endpoint and misleading offer.

**Required correction:** Refuse the SSH-only constructor under the exact Windows qualification predicate before starting endpoint/session owners, while retaining its Unix behavior. Also ensure endpoint and capability offers cannot be emitted for an absent HTTPS engine. No new public selector or configuration shape is needed.

**Closure/regression:** In the Windows qualification build, construct `TransportRuntime::new` from an explicitly supplied valid `SshEndpointConfig` and assert typed unsupported refusal. Verify the actual HTTPS constructor still creates the engine and emits matching Bound/capabilities. Retain a Unix regression proving the public SSH constructor and defaults remain functional.

## 2. Invariant analysis

The following attacks did not produce additional findings:

- **Selection isolation:** Changed core, CLI and Python guards use the explicit Unix-candidate/Windows-qualification union. Illegal qualification predicates contain explicit compiler refusal. Ordinary and candidate-only Windows are not opened by this diff.
- **Unsupported policy/scheme effects:** Session and worker preparation reject configured/Gh/SSH policies before allocation or credential work. Host-bound non-HTTPS validation refuses, and the installed smart-transport refusal prevents silent native fallback.
- **Backend policy/context:** Both request paths converge at `open_request`. The exact qualification predicate selects the existing Disabled constructor and attaches the unchanged context. Its Windows mapping is WindowsDefault; ordinary constructor defaults remain AllowConfigured.
- **Caller/environment/proxy:** CLI and Python capture NativeCaller before handoff; Python captures before its route’s detach. WinHTTP capture initializes output, requires successful no-proxy output with neither returned pointer populated, and frees partial returned allocations. Snapshot lookup retains native case rules and owned paths.
- **Cleanup with absent SSH:** Changed Session dispatch recognizes endpoint ownership independently of SSH presence. Existing shutdown/report paths count the HTTPS owner independently. Portable blocked-disposal coverage remains available.
- **Native publication/deadline:** The retained adapter anchors the logical deadline before checkout/adoption, refuses missing finite native deadlines, rechecks cancellation/expiry around native and HTTP completion, retains pending native cleanup ownership, and publishes only after native completion plus accepted response and finish.
- **Binding:** The existing verified final-origin TLS stream supplies the prefixed endpoint binding; the native adapter does not substitute proxy/root/test binding bytes.
- **Helper exclusion:** Unix helper owning modules are enclosed. Windows qualification does not instantiate their process owner or acquire a successful helper launcher; unavailable lookup paths return refusal.
- **Durable-state scope:** WH1 changes no journal, recovery vocabulary or durable write protocol. No new crash-between-writes defect was established. The concrete defects are in newly admitted runtime states and transitions.
- **Evidence honesty:** The checkpoint distinguishes source/native compilation, integrated default-path traffic, installed wheel evidence and deferred adversity. Original failures remain failures; false peer-cleanup confirmation is not asserted. The limited receipts do not prove nondefault capacities, empty-constructor handling or the deferred WH3 matrix.

## 3. Risks and next action

Configured helpers, ordinary activation, broader native adversity and identity transitions, provider parity, full release/platform/performance/source qualification remain explicitly deferred. They are not findings in this review. Disclosed strict Clippy45 and generator pin mismatch remain release debt, without a waiver.

The single next action is bounded remediation of P2-1 and P2-2, with the specified regressions, followed by an exact-tuple State closure review. The existing default-path native receipts remain useful evidence but cannot close either finding.