# GWZ SSPI native worker — State-AXIS REVIEW

**Review object:** Native implementation checkpoint under accepted revision-2 design, controlled by `dev-docs/GwzSspiNativeCheckpoint.md` at root `40fd121fbc2d9e6e727ed1a1b2501275200a1797`. Status: implementation in progress; not accepted. Member range `aec1b9c65b75ad53dd3ae780fc1e04af18de795c..610964282663c3b7844c9620d063e40d9ee76258`; root range `e220886f0641e4d3e5e67443171808666e191ff7..40fd121fbc2d9e6e727ed1a1b2501275200a1797`; bounded native evidence campaign.

**Baseline:**

| Repository | Verified HEAD |
|---|---|
| gwz-dev | `40fd121fbc2d9e6e727ed1a1b2501275200a1797` |
| gwz-sspi | `610964282663c3b7844c9620d063e40d9ee76258` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `57d132b823d405bf91b698c74aaaf75b9bee5980` |

Controlling documents were read with `git show HEAD:`. Source was inspected with numbered working-tree reads after verifying the member was clean. All four HEADs and statuses were checked at the start and end; the tuple remained unchanged. The evidence member is named `gwz-core-evidence`, not `evidence`.

**Date:** 2026-10-03

**Axis:** State machines, interruption, ownership, fail-closed cleanup, terminal publication and native containment evidence. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 fixture-lifecycle defect blocks; one P3 containment-coverage defect is also recorded. No production credential-exposure or terminal-publication defect was established. I pre-commit to GO on a revision that resolves P2-1 as specified, retaining the stated qualification limits and recording P3-1 accurately.

---

## 0. Evidence base

Read:

- `AGENTS_GWZ.md`, the canonical State prompt, `AgentProcessRules.md`—particularly L1-13 through L1-20—and `GwzProcessOptimization.md`.
- Committed `GwzSspiNativeCheckpoint.md`, accepted `GwzSspiDesign.md` revision 2, including §§3–8, and `GwzSspiPlan.md`, particularly step 3.
- Member `docs/Architecture.md`, `docs/Testing.md`, `docs/WireProtocol.md`, `docs/WorkerEntry.md`, `docs/NativeFixtures.md`, and native fixture ownership documentation.
- Worker production sources:
  - `src/worker/session.rs:1–208`;
  - `src/worker/framing.rs:1–54`;
  - `src/worker/storage.rs:1–67`;
  - `src/worker/bootstrap.rs:1–57`;
  - `src/protocol/worker_codec.rs:1–221`;
  - `src/worker/windows/conversation.rs:1–277`;
  - `src/worker/windows/owners.rs:1–152`;
  - `src/worker/windows/observation.rs:1–60`;
  - `src/worker/windows/provider.rs:1–39`;
  - Windows entry and minimal executable.
- Worker bridge, adversarial, support and native-owner tests, including cleanup faults, mechanism admission, round 8, illegal phases, truncation, outbound failure offsets and live wipe probes.
- Parent production sources relevant to actual-worker interaction:
  - `src/supervisor/kernel.rs:1–255`;
  - `src/supervisor/io.rs:1–178`;
  - `src/supervisor/monitor.rs:1–189`;
  - `src/supervisor/owners.rs:1–31`;
  - `src/supervisor/context.rs:220–354`;
  - `src/protocol/supervision.rs:1–186`;
  - `src/supervisor/windows/handles.rs:1–126`;
  - `src/supervisor/windows/launch.rs:1–233`;
  - `src/supervisor/windows/inherited.rs:1–55`.
- Public native tests:
  - `src/supervisor/windows/native_tests.rs:1–194`;
  - `tests/native/windows_worker.rs:1–91`;
  - `src/worker/windows/native_owner_tests.rs:1–54`.
- Member/root diff statistics and the relevant launch, handle, fixture, CI and documentation diffs.

The permitted prebuilt Darwin pure test binary was run once:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 worker::tests
```

It exited 0; all selected tests passed, with no ignored tests executed. This is pure worker-bridge evidence, not Windows native execution or a fresh build.

Inspected private campaign `2026-10-03-sspi-native-bec3ba15` README and these raw receipts/output:

- Native Supervisor command: exit 0; production pre-Begin Finish and initial NTLM fixture passed.
- Native owner/unit command: exit 0; EOF, last-Job-drop, actual-parent-loss and NTLM-owner fixtures passed. The direct helper invocation’s no-op success was not counted as separate evidence.
- Final inventory: Windows build `26200`, ReFS, `ownedRemaining: 0`.

These were existing owner executions, not executions performed by this reviewer. No files were changed, no builds or compiler probes were run, and no native processes/helpers were launched.

The member remained clean. Root untracked review prompts/SSH draft, the core bugreport, and the unrelated evidence alpha campaign remained outside scope. No peer current-round prompt or report was opened.

## 1. Findings

### [P2-1] Parent-loss fixture can abandon its live helper on an intermediate failure

**Location:** `gwz-sspi/src/supervisor/windows/native_tests.rs:158–194`, especially the unguarded `Child` created at lines 158–164 and the `OpenProcess`/handle-construction `unwrap` at lines 181–188.

**Violated invariant:** A native fixture must retain cleanup ownership across its failure paths. Failing to acquire an observation handle must not leave an independently running helper and its owned Job outside fixture cleanup.

**State sequence:**

1. The fixture spawns `parent_loss_child`.
2. The helper launches the production worker, publishes its PID and parks indefinitely while retaining `OwnedLaunch`, including the Job and pipe endpoints.
3. The root fixture reads the PID.
4. Acquiring the worker observation handle fails—for example, the worker exits between PID publication and `OpenProcess`, or the observation call is faulted.
5. `Handle::new(...).unwrap()` panics before `parent.kill()` and `parent.wait()`.

The timeout branch explicitly kills and waits for the helper, but the later failure branches have no equivalent owner. Dropping `std::process::Child` does not terminate or reap the helper. The parked helper therefore survives the failed test; if the worker is still alive, the helper also retains its Job and pipes. The coordination file is likewise removed only on the successful path.

The same ownership gap applies if an assertion or fallible operation panics after spawn and before successful kill/wait completion.

**Impact:** A failing qualification run can leave an indefinite helper process, retained native resources and scratch state. The successful campaign’s empty inventory does not exercise this failure path. This is a fixture-lifecycle correctness defect, not evidence of a production supervisor leak.

**Required correction:** Immediately place the spawned helper in a fixture cleanup owner that terminates and waits on every return/unwind path, with scratch-file cleanup under the same lifecycle. Keep the worker’s independent held-handle observation for the success assertion. Report cleanup failures explicitly rather than treating a kill request as confirmed reaping.

**Closure/regression test:** Force failure after helper spawn and after PID publication, including failure of worker-handle acquisition. Catch the fixture failure externally and verify actual helper exit through a held helper handle, no surviving owned worker/helper, and removal of the coordination file. Exercise successful parent loss as well. Use normal exit status and actual handle observations; no PID-disappearance or test-count gate.

### [P3-1] Job-drop and parent-loss fixtures cannot distinguish Job kill from normal EOF exit

**Location:** `gwz-sspi/src/supervisor/windows/native_tests.rs:101–123` and `125–194`; production clean-EOF path at `src/worker/session.rs:113–116,177–181`; fixture description at `docs/NativeFixtures.md:33–37`.

**Violated invariant:** Native evidence for Job containment must distinguish the containment action from an independently sufficient normal-exit path. A held process handle proves exit, but does not identify why the process exited.

**State sequence:**

- The last-Job-drop test uses `launch_fixture()`, which resumes the worker and consumes Hello. The worker then waits for Begin.
- `drop(owned)` closes both the Job and the parent input writer.
- The worker can receive clean EOF and return normally even if Job kill-on-close is ineffective.
- The test checks only that the held process becomes signalled.

The actual-parent-loss fixture has the same ambiguity: killing the helper closes its Job **and** its pipe writer. A worker waiting for Begin can exit through ordinary EOF. Both tests can therefore pass with Job termination broken while normal pipe cleanup remains functional.

This is established by the source state sequence; no source mutation or native counterfactual was executed during this review.

**Impact:** The tests provide real held-handle exit observations, but cannot serve as independent regression proof of forced Job containment. Their passing receipts should not be promoted to that stronger conclusion. The production launch code does configure creation-time Job membership and `KILL_ON_JOB_CLOSE`; this finding does not demonstrate that production setting is defective.

**Required correction:** Make at least one production Job-drop/parent-loss row unable to exit through EOF—for example, retain an independent input-writer handle in the controller through the exit observation, or observe a still-suspended production child before resume. Preserve finite waits and failure cleanup. Until then, describe these rows as exit observations with EOF as a competing cause.

**Closure/regression test:** The containment row must observe actual worker exit while EOF remains unavailable. It should discriminate an otherwise identical no-kill Job configuration in an explicitly authorized manual fault exercise. No blocked-provider or descendant qualification is required to close this bounded gap.

## 2. Invariant analysis

**Closed worker phases held under the attacks inspected.** Hello precedes native acquisition. Finish before Begin performs no provider work. Begin is accepted once; challenges require the expected round; Complete permits only Finish. Continue at round 8 is refused before token encoding/publication. Round-8 Complete remains legal.

**Admission precedes credential work.** Worker decoding validates the closed profile and phase before package metadata and owned secret copies. Provider maximum narrows the immutable caller cap before acquisition. Challenges are checked against both caps before another Initialize call. Digest refusal occurs after strict admission and before acquisition/context work; the documented missing H(Entity) contract remains a deferral.

**Status and output ordering held.** Only the four supported ISC statuses are accepted. Complete statuses call completion before observation and copy. Unknown status, unacceptable mechanism, oversized output, completion failure and output-free failure terminate before Token publication. Complete Negotiate requires a known authoritative mechanism. Native metadata allocations are owned and released; provider strings do not cross IPC.

**Native storage survives synchronous use.** Explicit Unicode fields occupy stable fixed UTF-16 allocations; the identity record points into those allocations. Target and aligned CBT remain conversation-owned through native processing and handle-disposal attempts. Provider output remains owned across ISC and Complete, with its initialized extent retained for wiping when completion shortens the reported output. Cleanup attempts context deletion before credential release. Failure remains an error and cannot acknowledge Finished. The fake bridge verifies orchestration; it does not reproduce every real FFI handle/pointer mutation.

**EOF and truncation remain distinct.** Clean EOF performs normal cleanup without Finished. Partial header/body, malformed input and I/O failures terminate. The selected pure tests exercise truncation and outbound failure offsets and passed. Parent framing treats EOF without Finished as failure, so normal worker EOF does not itself invent successful parent completion.

**Terminal publication remains revoked.** Parent state updates sample/control the deadline and cancellation under the state lock. Token storage may be installed outside that lock, but the subsequent phase transition and public publication still arbitrate terminal state. Late worker output cannot revive a cancelled conversation. Cleanup keeps process, Job and I/O ownership charged until actual completion proofs; requesting termination, receiving Finished or seeing EOF is insufficient to release capacity.

**Normal Finish requires disposal and reaping.** Worker native cleanup precedes Finished. Parent completion additionally requires held process exit, empty Job and actual launch/read/write completion. Existing native receipts exercise successful initial-NTLM disposal and Supervisor Finish. The two findings concern fixture adversity and interpretation, not an observed failure of this production path.

**Bootstrap remains fail-closed.** Decimal handles are bounded by parsing and rejected when malformed, equal, zero or invalid. Both inherited pipe endpoints validate before adoption; onward inheritance is cleared. Worker Hello uses actual primary identity. Minimal executable metadata has no runtime/default fallback. Missing installed fingerprint production remains step 4.

**No new durable restart grammar was introduced.** Authentication state is process-local and cannot be resumed. Parent loss must contain the child; it does not create a recoverable credential journal or restart permission. Filesystem sync/rename ordering is consequently not a production edge in this object. The fixture coordination file is nonsecret, but its ownership still requires P2-1’s cleanup correction.

## 3. Risks and next action

Real provider error/Complete behavior, completed Negotiate/NTLM/Kerberos, TLS/EPA fidelity, blocked-provider cancellation, descendant qualification and installed host composition remain explicitly unqualified. Fake bridge outcomes and the first-token native receipts do not close those rows. Forced exit provides containment, not physical wiping or external-provider cancellation.

The next action is a bounded fixture remediation for P2-1, preferably fixing P3-1 in the same change, followed by a settled-tuple State re-review and focused native fixture receipts. No wire, authentication-policy or production state-machine amendment is required by these findings.
