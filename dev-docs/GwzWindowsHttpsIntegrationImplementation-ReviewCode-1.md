# Windows HTTPS WH1 remediation round 1 — CODE-AXIS REVIEW

**Review object:** Core remediation `398158b3272e6f3a69132f8375190945dd93192a..261eaca55dca4067548027e8976ff0249a34d2f3`, plus `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-1.md` and `GwzWindowsHttpsIntegrationImplementationCheckpoint-1.md` at root `0d2db2c23afd83d496ca9eb55d8264bf8366314e`. Committed correction awaiting original-reviewer closure, 2026-10-04. Scope is limited WH1 implementation closure.

**Baseline:**

| Repository | Exact reviewed HEAD |
|---|---|
| root | `0d2db2c23afd83d496ca9eb55d8264bf8366314e` |
| gwz-core | `261eaca55dca4067548027e8976ff0249a34d2f3` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `053121cc97664e46539c07d77cdad4effb481955` |

Sources were read through the pinned remediation diff, `git show HEAD:`, and bounded tracked-file reads. The complete tuple matched at review start and end. Core retained only the excluded untracked bug report; CLI and Python were clean.

**Date:** 2026-10-04

**Axis:** Code — original counterexample closure, interfaces, call graphs, ownership, compatibility, and corrective-range regressions. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — Code P2-1 and P2-2 are independently closed. Zero open Code findings; no new architectural root cause identified. This verdict accepts limited WH1 only, not ordinary Windows activation or full Windows release.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1: late native Open publication | Retain fixed D through collection and pending handoff; reject expiry, preserve facts, revoke route, retain disposal charges | Re-traced both original sequences through completed-task collection and repeated backpressured handoff. Inspected executable original REDs and corrected endpoint/mux GREENs, equality and pre-D controls, single-terminal and real physical-charge assertions | **CLOSED** |
| Code P2-2: successful engine-free constructor | Typed qualification refusal before owners; shared boundaries require HTTPS; Unix preserved | Re-traced public `new`, shared runtime construction, and shared endpoint construction. Native RED demonstrates original success; corrected native test checks all three UnsupportedOperation refusals. Unix constructor and real HTTPS offer controls pass | **CLOSED** |

## Changed-range analysis

The core correction changes ten paths: nine source/test files and one mechanical inventory, with 611 insertions and 56 deletions relative to the reviewed implementation.

Production changes are confined to:

- Retaining the existing logical deadline and authenticated-route reference in the endpoint’s opening state.
- Checking expiry when collecting successful preparation and before each pending `Opened` handoff.
- Revoking failed publication and delaying native advertisement serving until successful publication.
- Refusing qualification SSH-only/no-HTTPS construction before session ownership.
- Installing capacity on whichever physical pools actually exist, retaining the paired Unix transaction and existing shared authority.

The remaining changes provide regression fixtures, test observations, and switch-inventory entries. `Authenticated::revoke` becomes crate-visible for the existing host publication boundary; it is not a public caller API.

CLI, Python, SSPI, transport, Git dependency, application schema, generated payloads, and selection predicates are unchanged. Actual CLI/wheel producers were rerun against corrected core. Cumulative scope is recorded as 52 source/test/build files plus three inventories and 1,350 gross added lines, within the prior owner disposition of 55 files and 2,600 lines.

No change falls outside the merged dispositions. **NEW ARCHITECTURAL ROOT CAUSES: none.** These corrections complete existing publication, construction, and capacity obligations without adding an owner, ledger, mechanism, or interface.

## 0. Evidence base

Inspection only. No builds, tests, remote calls, writes, or Git mutations were performed during this review. Recorded executions below were inspected, not independently rerun.

Authority includes the original Code report, merged remediation plan, round-1 checkpoint, current checkpoint entry, accepted integration design/acceptance/budget disposition, core design/requirements, and unchanged SSPI composition contract. Root/member instructions and process authority remain those loaded in the original review.

Corrected implementation inspected:

- Complete core remediation diff and inventory.
- `src/transport_host/mod.rs`, public constructor at line 186, shared construction at 216–228, and defensive capability projection.
- `src/transport_host/session.rs`, endpoint refusal before state/ID/authority construction at 365–375.
- `src/transport_host/https_endpoint.rs`, retained fields, `before_handoff` at 303–332, `handed_off` at 334–349, and equality-expiry helper at 438–440.
- `src/transport_host/https_endpoint/poll.rs:1–220`, including completed-result arbitration, serving transition, and retirement.
- `src/git/endpoint/https_worker.rs`, publication deadline/route accessors; native revocation change and test seams.
- `src/transport_host/session/capacity.rs:1–290`.
- The actual Session pump handoff sequence and transport Owner/mux ownership.
- `src/transport_host/https_cancel_mux_tests.rs:234–539` and qualification-test additions at 153–286.

Private evidence inspected, access required:

`gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-https-portability/`

- README remediation section, `portable-rem1/commands.json`, source manifest/status, and named refresh receipts.
- `publication-red-v3.log`: preparation succeeds before D; pre-D control passes; delayed collection and backpressured handoff both fail because the original code publishes `Opened`.
- `publication-green-v1.log`: all three cases pass.
- Final `endpoint-mux-green-v2.log`: six tests pass, including equality, cancellation control, physical retention, and capacity replacement.
- Portable qualification four-pass, paired constructor/capacity sixteen-pass, and cleanup two-pass logs.
- Native `wh1-rem1-qualification-red-v1`: normal exit 101 after compilation; original two controls pass, constructor and capacity regressions fail.
- Native corrected qualification: five pass, normal exit 0, no timeout, child reaped.
- Corrected refresh reports forty core inputs read back with zero mismatches.
- Rebuilt CLI and installed Python capacity RED/GREEN outputs, installed artifact hashes, and corrected CBT/TLS negative outputs.

The installed-artifact receipt records corrected CLI hash `880f1acf04d9c83d58f09c955556e53e6e112f61533793019b9712114eeaf423`, wheel hash `3b8e08420331709f7ad618d5cda1b251e73294f768524c7089cf44e05ddbad33`, and installed PYD/worker hashes. The client GREEN receipt identifies that same CLI.

## 2. Invariant analysis

**P2-1 original delayed collection:** The completed task still returns its original `Retry` budget. Collection now copies that budget’s fixed deadline into entry state and checks fresh time before treating success as publishable. An expired successful result has its native route revoked and becomes Timeout with its existing facts. No deadline is reconstructed or extended.

**P2-1 original backpressured publication:** The entry retains D while `Opened` remains pending. The unchanged pump invokes `before_handoff` on every send attempt. After expiry, the receipt becomes `OpenFailed/Timeout`; cancellation, route revocation, prepared-resource drop, and peer disconnection occur. Successful handoff clears publication state. Native advertisement serving begins only after that handoff; nonnative advertisement timing follows the existing branch.

The production regression uses an actual HTTP preparation and mux, with a deterministic native-provider fixture. It verifies authentication completed before D, then delays collection or fills the mux queue. Corrected assertions cover Timeout, authenticated facts retained, route revoked, one terminal, and an independently held real Connection keeping the authority charge occupied until disposal. The original RED is a publication failure, distinct from retained earlier harness failures. Equality uses `now >= D`; the pre-D success and cancellation controls remain green.

**P2-2 original direct constructor:** Qualification `TransportRuntime::new` now returns UnsupportedOperation immediately. Direct shared runtime and Session construction also refuse absent HTTPS before IDs, authority, endpoints, drivers, or links are created. Consequently the former successful runtime with neither engine cannot reach Bound or capability publication. Unix continues through the original constructor, with its engine and default capacity verified. The native HTTPS capability/forged-Open controls remain green.

**Merged capacity correction:** Physical pools are selected by presence. Two pools retain `install_capacity_pair`; one pool uses its existing installation transaction. The shared authority is obtained before mutation, updated after retirement, and remains the same owner. The armed mutation guard still closes a partially changed generation on error or dropped retirement. No dummy SSH pool or replacement ledger appears. Tests cover initial/later limits, incompatible overlap, authority limits, cancellation, and retained physical disposal.

**Consumer and disclosure controls:** Corrected CLI and installed Python perform clone/fetch/push with existing one-connection limits. CLI independently verifies checkout and remote refs. Python additionally preserves ordered streaming, completes a call with unconsumed events, and reports local cleanup zero with peer confirmation false. Corrected wrong-CBT refusal and untrusted-chain/hostname refusal retain server disposal/reaping evidence; TLS negatives record no native authentication rounds. These remain bounded normal-path proofs.

## 3. Risks and next action

The evidence does not complete broader native scheduling/cancellation/identity transitions, installed-path/provenance breadth, WH2 helper portability, provider parity, or aggregate release qualification. Strict Clippy45 and the owner-IR mismatch remain disclosed REDs without waivers. The deadline regression’s provider fixture is deterministic; it is not relabelled as a stalled real-provider qualification.

The next action is to record this original Code closure on the exact tuple and complete the independent State closure before filing limited WH1 acceptance. Ordinary Windows activation and full release remain separate NO-GO gates.