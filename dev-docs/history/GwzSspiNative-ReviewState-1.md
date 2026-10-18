# GWZ SSPI native worker remediation 1 — State-AXIS REVIEW

**Review object:** Remediation 1, member `610964282663c3b7844c9620d063e40d9ee76258..425e13dc011c42e94fdea31779a8e5967aedc82b`; root `40fd121fbc2d9e6e727ed1a1b2501275200a1797..bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e`. Controlled by `dev-docs/GwzSspiNativeCheckpoint.md` and `dev-docs/GwzSspiNative-RemPlan.md` at the corrected root HEAD. Status: remediation pending reviewer closure, 2026-10-03.

**Baseline:**

| Repository | Verified HEAD |
|---|---|
| gwz-dev | `bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e` |
| gwz-sspi | `425e13dc011c42e94fdea31779a8e5967aedc82b` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c` |

Controlling documents were read with `git show HEAD:`; corrected source was read with numbered working-tree inspection after confirming the member was clean. Four HEADs and statuses matched at both start and end. Unrelated untracked material remained outside scope.

**Date:** 2026-10-03

**Axis:** Failure ownership, unwind cleanup, held-handle completion and containment-proof discrimination. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — State P2-1 and P3-1 are closed. No new findings or NEW ARCHITECTURAL root causes were established. This accepts the bounded native-worker checkpoint and remediation; the documented Digest, composition and Windows qualification gates remain open.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Immediately guard the spawned helper and retain helper/scratch cleanup across failure and unwind. | `helper_start` transfers the successful spawn directly into `Guard`. The guard attempts termination, observes actual helper exit with a finite held-handle wait, then consumes `Child::wait`; scratch cleanup is separately attempted and reported. Native forced after-spawn, after-PID, failed observer acquisition and after-observer unwind cases passed, requiring independent helper/worker exit observations and removed scratch. | **Closed** |
| State P3-1 | Eliminate EOF as a competing containment explanation using suspended children and a fixture-owned no-kill control. | Neither Job-drop nor parent-loss children are resumed. The Job-drop fixture expects held-process exit with production kill-on-close and `WAIT_TIMEOUT` with the no-kill Job, then explicitly terminates and confirms cleanup of the control. Corrected native receipts passed. Earlier receipts are explicitly limited to exit observations with competing EOF. | **Closed** |

## Changed-range analysis

The member patch changes 13 files. Its changes fit the merged dispositions:

- Test-only cleanup ownership and fixtures are added in `fixture_cleanup.rs`, `fixture_support.rs` and `containment_tests.rs`. The previous unguarded parent-loss implementation is replaced.
- `create_owned` still creates and configures the production kill-on-close Job. The remaining creation code is extracted into private `create_in_job`, allowing the test-only negative control to provide its independently owned no-kill Job. Search found only the configured production caller and the fixture caller. No public containment option or fallback was added.
- Initial Negotiate fixtures exercise the production query path. A per-instance audit records fixed query status, allocation presence and checked release outcome. The non-test audit is empty and does not change native decisions or retain secret data.
- Caller/testing status and the native-fixture recipe are corrected. The recipe saves/restores four process environment values, owns a fresh runtime root and conditions teardown on evidence retention.
- Root changes record prior reports, merged dispositions, corrected evidence and member revisions. The lockfile changes identify the corrected member/evidence commits; the integrity marker changes accordingly.

The patch materially strengthens the **evidence**, by removing EOF ambiguity and testing cleanup failures. It does not change the accepted authority, public API, wire, authentication policy, production supervisor state machine or native ownership architecture. No change outside the merged dispositions requiring a new design review was found. **NEW ARCHITECTURAL causes: none.**

## 0. Evidence base

Read the generated remediation State prompt, committed NativeCheckpoint/RemPlan and changed CurrentProgramCheckpoint. The accepted revision-2 design, section 8, plan step 3 and original State analysis remain the authority and prior context for this focused review.

Inspected corrected sources and diffs:

- `src/supervisor/fixture_cleanup.rs:1–131`;
- `src/supervisor/windows/fixture_support.rs:1–181`;
- `src/supervisor/windows/containment_tests.rs:1–140`;
- `src/supervisor/windows/native_tests.rs` replacement and retained EOF fixture;
- `src/supervisor/windows/launch.rs` creation extraction;
- `src/supervisor/mod.rs` test-only inclusion;
- `src/worker/windows/observation.rs:1–101`;
- changed conversation audit wiring, native-owner tests and public native-worker tests;
- complete corrected `docs/NativeFixtures.md:1–104`;
- CallerValues and Testing changed ranges;
- root/member diff statistics and root checkpoint/lockfile changes.

Ran only the permitted prebuilt Darwin binary:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 supervisor::fixture_cleanup
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 worker::tests
```

Both exited 0; all selected tests passed and no ignored tests ran. These establish pure guard/worker checks, not fresh compilation or Windows native execution.

Inspected private campaign `2026-10-03-sspi-native-remediation-1`:

- README, collector, recipe adaptation and retained PowerShell harness;
- corrected source receipt: committed member archive, fresh candidate path, exit 0 and archive-input hash;
- corrected build receipt: pinned Rust 1.95.0, exit 0;
- native Supervisor receipt/output: exit 0, initial NTLM/pre-Begin Finish and local initial Negotiate passed;
- native owner receipt/output/stderr: exit 0; suspended Job/no-kill, actual parent loss, forced helper failures, EOF and native ownership checks passed;
- forced-failure stderr showed the expected after-spawn, after-PID, failed handle construction and after-observer panics caught by the fixture;
- real Negotiate receipt reported `query_status=0`, `allocation_returned=true`, `release_success=true`, with NTLM selection;
- ordinary Windows all-feature receipt: exit 0;
- recipe file-invocation receipt/output: all four absent/prior-value × success/forced-failure cases restored environment and removed the owned root;
- final inventory receipt/output: exit 0, `ownedRemaining=0`, ReFS, Windows build `26200`.

These native results are archived owner executions, not executions performed by this reviewer. The final census corroborates cleanup but is not substituted for the fixtures’ held-handle proofs. No files were modified, builds performed, native helpers launched or current peer reports/prompts read.

## 2. Invariant analysis

**The original helper-abandonment sequence is blocked.** After successful spawn there is no intervening fallible observation before ownership enters `Guard`. Failure of the first worker-handle acquisition, PID timeout, or later assertion unwinds through that guard. Native tests separately preserve an independent helper handle and, after publication, a worker observer. Their post-cleanup assertions occur outside the deliberate panic catch, so an expected injected panic alone cannot count as success.

The helper is gated before worker creation. Thus the after-spawn case can verify cleanup without racing an unobserved worker into existence. After publication, the helper owns the suspended worker’s Job. Helper termination is followed by actual helper exit observation and successful wait consumption; worker exit is observed through a separate handle.

**Cleanup failure cannot become confirmation.** Kill results alone are ignored as requests. Failed/timed-out held-handle observation yields a false cleanup report. Scratch cleanup is attempted independently, including when helper cleanup fails. Guard Drop contains cleanup panics and retains the result in the receipt. Pure tests exercise failed cleanup without skipping scratch; native forced failures require both report fields to be true and the owned file to be absent.

**The original EOF counterexample is unavailable.** Corrected containment children never execute bootstrap or worker IPC, because their primary threads remain suspended. Closing their pipes therefore cannot run the clean-EOF return path. The no-kill control stays alive during the finite observation interval and is then explicitly terminated through its independent observer. The passing production counterpart consequently discriminates Job termination from ordinary worker exit.

The parent-loss case also holds a still-suspended worker handle before terminating the helper. It confirms actual helper exit before observing worker exit. PID publication is coordination only; neither PID disappearance nor the helper kill request is proof.

**Production containment semantics remain fixed.** The extracted creation function preserves suspended creation, creation-time Job attachment, explicit inherited handles and membership checking. Production continues to supply only the configured kill-on-close Job. The negative control is confined to test code.

**Query auditing preserves ownership ordering.** Both native query-status branches release the owned allocation, record the fixed result and propagate release failure before returning an observation. Recording pointer presence after release does not dereference freed storage. Native selection decisions remain unchanged. The real initial Negotiate execution adds evidence for the query/free path without claiming completed remote authentication.

**Earlier worker-state conclusions remain applicable.** The remediation does not change serial phase admission, token caps, round-8 refusal, Complete handling, output wiping/free-before-publication, Finish cleanup or parent terminal arbitration. The permitted pure worker suite passed again. Existing clean-EOF coverage remains separate from forced containment coverage.

## 3. Risks and next action

Finite fixture waits and reported cleanup failure do not establish a universal OS cancellation bound. Suspended-child containment does not qualify blocked real providers or descendants. Initial Negotiate/NTLM tokens do not prove completed remote authentication, TLS/EPA fidelity or HTTP/Git success. Digest remains refused pending the H(Entity) amendment; installed fingerprint production and host composition remain step 4.

The next action is to record this State GO in the bounded acceptance checkpoint, subject to the other required independent re-verdicts, and proceed under the existing step-4 and qualification gates.
