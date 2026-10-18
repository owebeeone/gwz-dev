# WH1 Windows HTTPS remediation round 1 — STATE-AXIS REVIEW

**Review object:** Core remediation `398158b3272e6f3a69132f8375190945dd93192a..261eaca55dca4067548027e8976ff0249a34d2f3`, with `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-1.md` and `GwzWindowsHttpsIntegrationImplementationCheckpoint-1.md` at root `0d2db2c23afd83d496ca9eb55d8264bf8366314e`. Committed correction, original-reviewer closure pending, 2026-10-04. Limited WH1 implementation only.

**Baseline:** Committed sources and evidence were read using `git show HEAD:PATH` and the prescribed core diff. The full tuple matched at review start and end:

| Repository | SHA |
|---|---|
| root | `0d2db2c23afd83d496ca9eb55d8264bf8366314e` |
| gwz-core | `261eaca55dca4067548027e8976ff0249a34d2f3` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| gwz-core-evidence | `053121cc97664e46539c07d77cdad4effb481955` |

**Date:** 2026-10-04

**Axis:** State transitions, admission, publication arbitration, lock scope, cancellation and retained cleanup ownership. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — original State P2-1 and P2-2 are closed; one P2 residual defect in the merged publication correction blocks acceptance. No P0 or P1 found. I pre-commit to GO on a bounded revision resolving P2-3 as specified, with no additional defects or scope expansion.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1: HTTPS-only capacity installation requires SSH | Install capacity on actual present pools while retaining authority and retirement guards | Original first-request nondefault-capacity path now selects the HTTPS pool. Later changes, incompatible overlap, retained physical disposal and dropped retirement are covered by inspected production-path tests and recorded GREEN runs. Actual original CLI/wheel one-connection REDs become corrected clone/fetch/push GREENs. | **Closed** |
| State P2-2: empty constructor advertises HTTPS | Refuse SSH-only/no-HTTPS Windows construction before owners; require actual HTTPS for offers | Direct `TransportRuntime::new`, shared runtime construction and shared endpoint construction now refuse before owner creation. The native regression incorrectly succeeds on the original tree and passes on the corrected tree. Existing actual Bound/capability tests remain GREEN; Unix public constructor control passes. | **Closed** |

## Changed-range analysis

The core diff contains ten files, 611 added and 56 deleted lines, including one mechanical inventory. Production corrections are confined to present-pool capacity installation, qualification constructor/capability guards, and native Open publication state. Remaining additions are test seams and regressions.

Capacity installation preserves the existing paired transaction when both pools exist and uses the existing single-pool transaction for HTTPS-only or SSH-only ownership. No replacement ledger or dummy engine appears. Qualification construction now rejects missing HTTPS before creating runtime or endpoint owners. Unix constructor behavior is retained.

The deadline correction carries D and the authenticated route through preparation collection and pending Opened handoffs, checks equality as expired, preserves facts, and delays native advertisement serving until successful mux publication. This is within the merged disposition, but its final check remains outside the mux synchronization boundary. P2-3 identifies that residual gap.

**Architectural classification:** P2-3 is **not a NEW ARCHITECTURAL root cause**. It is an incomplete correction of the already identified native-publication arbitration root represented by merged Code P2-1: publication can still use time sampled before acquiring its publication lock. It does not constitute a third independent architectural discovery for the two-round cap. The exact synchronization remedy must nevertheless respect the existing scope and interface stop rules.

No other change outside the merged dispositions was identified.

## 0. Evidence base

This reviewer performed inspection only. No files were modified; no builds, tests, remote calls or Git mutations were executed. Recorded executions below are committed evidence, not reviewer reruns. No current peer closure report was accessed.

Authority and status inspected:

- Canonical State-1 prompt, the original State report context, committed merged RemPlan-1 and Checkpoint-1.
- Current checkpoint and remediation budget disposition.
- Accepted integration design §4 and the carried composition publication obligations, particularly composition §4’s requirement for fresh arbitration time rather than stale pre-lock time.
- Previously read root/member AGENTS, EVIDENCE and process rules remain applicable.

Corrected production and regression paths inspected:

- `gwz-core/src/transport_host/mod.rs`, constructor/shared-construction/capability guards and original request-capacity derivation.
- `session.rs`, early no-HTTPS refusal and unchanged actual HTTPS construction.
- `session/capacity.rs:105–264`, including present-pool selection at 208–228, retirement at 230–248, authority installation at 255–263 and the retained mutation guard.
- `qualification_tests.rs:153–286`, first/later/overlap, dropped-retirement, direct/shared constructor and Unix controls.
- `https_endpoint.rs:301–349,351–459`, publication guard, successful handoff, cancellation and failure construction.
- `https_endpoint/poll.rs:21–186`, preparation collection, route retention, native serving deferral and retirement.
- `https_worker.rs:139–175,225–238`, retained route and immutable-deadline accessors; scoped native changes.
- `https_cancel_mux_tests.rs:229–538`, late collection/backpressure, pre-D success, equality and actual physical-disposal regressions.
- Unchanged `session/driver/pump.rs:9–14,173–195`, actual handoff call order.
- `gwz-transport/src/mux/asynchronous.rs:11–36,74–84`, mutex acquisition and send wrapper.
- `gwz-transport/src/mux/mod.rs:334–395,530–599` and `routing.rs:270–319`, enqueue, transition and separately driven expiry.

Private committed evidence inspected under:

`campaigns/https-integration/runs/2026-10-04-windows-https-portability/`

- README remediation entries, `portable-rem1/commands.json`, implementation status and bounded RED/GREEN logs.
- Portable capacity RED: `SSH endpoint unavailable`; qualification GREEN: four pass including Unix constructor and dropped retirement.
- Paired construction/capacity controls: sixteen pass.
- Endpoint/mux GREEN: six pass, including real physical-disposal retention.
- Publication RED-v3: pre-D control passes; delayed collection and backpressure wrongly produce Opened. Earlier harness failures remain separately labeled.
- Native `wh1-rem1-qualification-red-v1`: compiler completes; original two controls pass, constructor and both capacity regressions fail.
- Native `wh1-rem1-qualification-green-v1`: all five pass, ordinary runner exit zero.
- Corrected source refresh receipt/readback: forty core inputs, zero mismatches.
- Actual CLI capacity RED/GREEN receipts: original clone returns IoError; corrected `--max-per-host 1` clone/fetch/push succeeds with independent content/ref checks.
- Installed Python capacity RED/GREEN receipts: original `Client(max_connections_per_host=1)` fails; corrected clone/fetch/push, stream and second-call consumer-independence checks pass; close reports pending zero and peer confirmation false.
- Corrected CLI/wheel producer receipts and installed wheel/PYD/worker/CLI hashes.
- Corrected CBT, untrusted-chain and hostname negatives: refusal; TLS negatives record no native rounds.

The new mux tests exercise direct `Mux::send` after `before_handoff`; they do not exercise contention between that check and asynchronous `Owner::send` acquiring its mutex.

## 1. Findings

### [P2-3] Final native publication check precedes the mux publication lock

**Location:** `gwz-core/src/transport_host/https_endpoint.rs:303–315`, called by `session/driver/pump.rs:183–186`; downstream `gwz-transport/src/mux/asynchronous.rs:22–25,74–75` and `mux/mod.rs:377–395`.

**Violated invariant:** Native D remains authoritative through publication. Composition §4 explicitly prohibits publication using stale pre-lock time. A result prepared before D but not eligible for publication when D expires must not publish.

**Counterexample/interleaving:**

1. Native preparation completes before D and produces a pending authenticated Opened receipt.
2. The Session pump calls `before_handoff` at `D−ε`. Its fresh clock check passes and leaves Opened intact.
3. Before `Owner::send` acquires `Shared.inner`, the publishing thread is suspended or waits behind another mux user. Resume it after D.
4. `Owner::send` acquires the mutex and invokes `Mux::send` without another native-D check.
5. `Mux::send` validates the receipt, enqueues Opened and transitions the route to Stream.
6. `handed_off` treats publication as successful, clears the retained deadline/route guard and permits native advertisement serving.

The Session’s `owner.advance(now)` occurs earlier in the pass and uses that pass’s earlier clock. It does not refresh time after the intervening delay. `Mux::send` itself performs no expiry advancement or native-D arbitration. Consequently, the receipt can become publishable only after D yet still enter the successful stream state.

This is a source-derived counterexample; it was not newly executed during this read-only review.

**Impact:** The merged correction closes delayed collection and repeated backpressure attempts but leaves late successful native publication possible across lock acquisition. The expiry path that revokes the route and retains physical cleanup is bypassed in this interleaving.

**Required correction:** Make the final fresh-time/cancellation decision part of the same synchronized admission that determines whether the receipt can be queued. A pre-lock check alone cannot establish publication eligibility. If expiry wins, publish the single Timeout outcome, preserve observed facts, revoke the authenticated route and retain physical/native cleanup charges. Preserve nonnative behavior and the existing application/wire surface.

**Closure/regression:** Exercise the actual asynchronous Owner/Session publication boundary. Complete preparation before D, pause or contend publication after the outer guard but before mux-lock acquisition, release after D, and assert no Opened, one Timeout, preserved facts, route revocation and retained charge until real disposal. Include a pre-D success control and equality-expired control. Retain existing collection/backpressure and nonnative tests.

## 2. Invariant analysis

The original capacity counterexample no longer reaches a mandatory SSH lookup. HTTPS-only installation changes the actual pool, waits for closing resources, then updates the existing shared authority. Failed or dropped installation retains the fail-closed mutation guard. The inspected physical-disposal regression holds a real Connection reference, demonstrates retained reservation and replacement refusal, then succeeds after disposal. Live incompatible overlap still refuses without consuming a reusable request ID.

The original empty-constructor counterexample is stopped before runtime/session owner creation at both public and shared construction boundaries. The successful qualification path still requires the actual HTTPS engine; capability agreement and forged-policy refusal controls remain intact. Unix public construction and paired capacity controls are preserved.

Collection-time and repeated-handoff deadline checks use fresh monotonic time and treat equality as expired. Failure preserves authenticated facts, revokes the route and discards prepared ownership. Native advertisement serving is deferred until handoff succeeds. These changes close the tested schedules, but P2-3 prevents concluding that final publication arbitration is complete.

No durable format, recovery vocabulary, caller API, wire payload, dependency or process owner was added. No separate crash-between-writes defect was established. Unsupported-route isolation, helper exclusion, origin capture, CBT source and effects/retry rules were not broadened by this correction.

Evidence attribution is appropriately limited. Native constructor/capacity REDs are executable failures, distinct from harness errors. Corrected artifacts are rebuilt against corrected core. One-connection success is observed through actual CLI and installed Python routes. Portable native-publication tests are not relabeled as full Windows scheduler qualification.

## 3. Risks and next action

WH2 helpers, ordinary activation, broader WH3 native adversity/identity and installed-path proof, provider parity and release/platform/source/performance/package qualification remain deferred. Strict Clippy45 and owner-IR pin mismatch remain disclosed REDs without waivers.

The next action is a bounded correction of P2-3 at the synchronized publication boundary, followed by focused original-reviewer closure on a newly settled tuple. State P2-1 and P2-2 remain closed; their correction does not need to be redesigned.