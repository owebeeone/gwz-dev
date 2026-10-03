# GwzSspiSupervisor — Code-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..255d05e09c0433e27dd7afeaa9f9fe04699cad05`; member `44879481fbd54dab84b99fecdadc89a34a84dcbd..a75485cbdd03607909d11637c07f97548dd7902c`. Controlling document: `dev-docs/GwzSspiSupervisorCheckpoint.md`, DRAFT implementation checkpoint with remediation round 1, dated 2026-10-03.

**Baseline:** Root `255d05e09c0433e27dd7afeaa9f9fe04699cad05`; gwz-sspi `a75485cbdd03607909d11637c07f97548dd7902c`; reference-only gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Sources read using committed `git show` and diffs from the original reviewed tuple. All three HEADs matched at review start and end.

**Date:** 2026-10-03  
**Axis:** Code — architecture, interfaces, ownership, actual call graphs and compatibility. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — original Code P2-1, P3-1 and P3-2 are closed. No new findings.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1 | Exclude the Supervisor receiver lifetime from start/shutdown futures using precise capture. | Both signatures use `+ use<>`. Public integration and Rustdoc examples construct both futures, drop Supervisor, then require `Future + Send + 'static`. `step` retains its mutable borrow. | Closed |
| Code P3-1 | Document synchronous captured-origin handle disposal as a narrow pre-registration exception. | API, checkpoint and caller documentation consistently state the exception and absence of a hard OS time bound. The successful-capture regression observes disposal during cancellation, expiry, closed admission and Drop, outside the state lock. | Closed |
| Code P3-2 | Exercise production orchestration through private bounded iterations and owned completion ports. | Fake Platform/Child/ports now execute production launch, read/write, reaper and join decisions. Tests cover late launch, failed containment/resume/launch, failed termination/observations, delayed owners, owner errors/panics, disposal ordering and eventual capacity release. | Closed |

## Changed-range analysis

Reviewed root `d2821a2..255d05e` and member `fb6a5fc..a75485c`.

The member correction comprises precise public lifetime capture; accurate polling/disposal documentation; centralized Step completion wiping; publication fallback preserving the saved terminal cause; private extraction of existing launch/read/write/reap/join decisions; and corresponding tests.

The new private `Owner` port wraps the actual `JoinHandle<()>` in production. Its implementation delegates completion, consuming join and advisory cancellation to the same existing operations. Fake completion owners remain test-only. Production drivers call the extracted iterations while retaining thread creation, blocking I/O, waits and native disposal on their previous owners.

The extraction preserves record installation before launch, process/Job retention, containment admission before resume, fixed framing, first-terminal arbitration and completion before permit release. It adds no public injection, wire change, dependency, replacement disposal mechanism or worker policy.

The additional publication/challenge corrections and trusted-packaging clarification fall within the merged remediation dispositions. Root changes record the remediation and reviewed member revision. No changed production range falls outside those dispositions.

**NEW ARCHITECTURAL root causes: none identified.** Precise capture restores the intended owned-future interface; private iteration/completion seams provide testability without materially changing the ownership architecture.

## 0. Evidence base

The original review’s authority and unchanged-source analysis remain applicable. For this focused re-verdict, read the round-1 Code prompt, committed merged remediation plan and revised checkpoint. No current-round peer prompts or reports were read.

Corrected files inspected:

- `src/supervisor/api.rs`, especially start/shutdown signatures and polling documentation.
- `tests/supervisor_values.rs:31–44` and `docs/Supervision.md:22–59,87–144`.
- `src/supervisor/futures.rs:154–258`; context changes replacing task handles with private owned completion ports.
- `owners.rs:1–31`, `dispatch.rs:1–69`, `monitor.rs:1–189`, and `io.rs:1–178`.
- `orchestration_support.rs:1–286`, `orchestration_tests.rs:1–401`, `orchestration_schedule_tests.rs:1–74`, and `remediation_tests.rs:1–113`.
- Test-module wiring, private protocol fixtures and changed Architecture/Testing documentation.
- Complete production diffs for monitor, I/O and dispatcher extraction.

Executed the prompt-authorized prebuilt pure unit binary:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 supervisor::
```

Result: 29 passed, zero failed.

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 protocol::supervision_tests::
```

Result: two passed, zero failed.

These executions include the new disposal, orchestration, seeded reaper and failure/wiping regressions. The counts describe observed runs, not acceptance thresholds.

Compiled public examples were inspected in source; their successful compilation is recorded in the owner’s checkpoint gates. This reviewer performed no builds or compiler probes. The prebuilt binary executions do not independently establish build provenance or native behavior.

No files were written, repositories mutated, native workers launched or campaigns run.

## 2. Invariant analysis

**Owned async interface:** Empty precise capture prevents Rust 2024 from importing the `&self` lifetime into the start/shutdown opaque future types. Their hidden structs own their context references. The original executor-handoff and drop-Supervisor counterexamples are represented directly by the new public compilation examples.

**Polling boundary:** The previous blanket “no OS call” claim is removed. Refused unregistered starts still synchronously dispose their captured origin handle, now explicitly documented and tested. Metadata/provider queries, worker/thread creation, IPC and process waits/joins remain absent from polling.

**Production ownership bridge:** Tests no longer rely exclusively on manually supplied kernel proofs. Production `reap_once` consumes fake held observations and completed-owner joins, disposes payload/child ownership, and leaves launch completion for the production dispatcher join decision. Tests demonstrate Pending capacity until that final obligation completes.

**Failure paths:** Late successful creation after cancellation produces neither resume nor secret writes. Containment and resume failures retain child ownership until held exit and empty Job observations. Failed termination does not manufacture either observation. Reader/writer errors and caught panics dispose ports/frames; unfinished owners retain capacity.

**Changed publication and secret handling:** Lost-record publication recovers terminal/completion authority before returning its error. Cancel/expiry regressions cover both taken-token and retired-token orders. Every completed Step path goes through challenge disposal before Ready, with live-before-deallocation wipe observations while the completed future remains retained.

**Compatibility and scope:** The wire schema/profile, Windows FFI adapter, dependency set, capacity/deadline policy and public lifecycle verbs are unchanged. New protocol code supplies test fixtures only. Inspected new control-flow bodies are braced and conditional test sections have enclosing module boundaries.

## 3. Risks and next action

Native Windows execution, actual OS cancellation/containment behavior, provider ownership and installed-host composition remain separately unqualified. Fake completion ports establish parent orchestration decisions, not native scheduling or cleanup guarantees.

The next action is to record this Code GO for the revised tuple and complete the remaining independent acceptance gates before proceeding to native SSPI step 3.
