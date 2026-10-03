# GwzSspiSupervisor — Code-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; member `44879481fbd54dab84b99fecdadc89a34a84dcbd..fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`. Controlling document: `dev-docs/GwzSspiSupervisorCheckpoint.md`, DRAFT implementation checkpoint dated 2026-10-03.

**Baseline:** Root `d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; gwz-sspi `fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`; reference-only gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Reviewed committed sources using `git show` and bounded diffs. All three HEADs matched at review start and end.

**Date:** 2026-10-03  
**Axis:** Code — architecture, interfaces, ownership, actual call graphs and compatibility. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; two P3 findings also require tracking. I pre-commit to GO on a revision that resolves P2-1, P3-1 and P3-2 as specified.

---

## 0. Evidence base

Read the supplied scope rules, `AGENTS_GWZ.md`, member `AGENTS.md`, and the Code prompt. Authority inspected included the load-bearing rules in `AgentProcessRules.md`, `GwzProcessOptimization.md`, accepted `GwzSspiDesign.md` revision 2 and §8, and `GwzSspiPlan.md` step 2.

Read the controlling supervisor checkpoint; member `docs/Architecture.md`, `docs/Testing.md`, `docs/Supervision.md`, and `docs/WireProtocol.md`. The prompt’s original `protocol/WireProtocol.md` path does not exist; the controlling member document is `docs/WireProtocol.md`, consistent with the owner’s path correction.

Implementation inspected:

- `src/supervisor/api.rs:1–154`, `values.rs:1–147`, `context.rs:1–353`, and `kernel.rs:1–255`.
- `futures.rs:1–329`, `io.rs:1–165`, `monitor.rs:1–189`, `dispatch.rs:1–68`, `driver.rs:1–61`, and `control.rs:1–98`.
- Private ports/platform/module boundaries, test support, lifecycle/control/I/O/kernel tests.
- Windows `launch.rs:1–211`, `handles.rs:1–128`, `attributes.rs:1–78`, `identity.rs:1–172`, and module wiring.
- `src/protocol/supervision.rs:1–154`, its phase tests, modified adapters, relevant retained profile/CBOR routines.
- Public exports, manifest/lockfile, CI, all-source formatting script, and changed root/member implementation-status documentation.

Read-only searches confirmed that production launch/reaper and complete reader/writer loops have no fake-child test caller.

Executed only the prompt-authorized prebuilt pure unit binary:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 supervisor:: --nocapture
```

Result: 19 passed, zero failed. The synthetic waker panic was caught as intended.

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 protocol::supervision_tests::
```

Result: two passed, zero failed.

These executions corroborate the supplied binary’s pure tests; they do not independently establish its build provenance or native behavior. No writes, builds, compiler probes, campaigns or native workers were performed. Current State/Surface prompts and reports were not read.

## 1. Findings

### [P2-1] Owned start and shutdown futures implicitly borrow the Supervisor

**Location:** `gwz-sspi/Cargo.toml:4`; `src/supervisor/api.rs:48–75,84–92`. Related owned-future promise: `api.rs:81–83`.

**Violated invariant:** Caller futures own supervision handles and must support independent retained supervision. `shutdown` explicitly describes its result as an owned future.

The crate uses Rust edition 2024. Both methods return opaque `impl Future` without a precise capture list. Rust 2024 therefore captures the anonymous lifetime of `&self` in the opaque return type, even though the hidden `Start` and `Shutdown` structs store owned `Arc<Context>` values.

**Reproduction:** A caller attempting this ordinary executor handoff cannot compile:

```rust
fn accept_owned<F: std::future::Future + Send + 'static>(_: F) {}

fn handoff(
    supervisor: &gwz_sspi::Supervisor,
    request: gwz_sspi::AuthRequest,
    deadline: gwz_sspi::Deadline,
    cancellation: gwz_sspi::Cancellation,
) {
    accept_owned(supervisor.start(request, deadline, cancellation));
    accept_owned(supervisor.shutdown(deadline));
}
```

Likewise, constructing either future, dropping an owned Supervisor, then using the future is rejected because the opaque future captures the Supervisor borrow. This reproduction follows directly from the signatures and edition; it was not executed because compiler probes are forbidden for this review.

**Impact:** The public interface prevents the owned future from escaping the Supervisor borrow into a typical executor requiring `'static`, and prevents observing shutdown through its owned future after dropping the Supervisor. The internal ownership architecture supports these operations, but its public signatures conceal that capability.

**Required correction:** Explicitly exclude the receiver lifetime from these two opaque return types, for example with `+ use<>`, and expose/document the intended owned lifetime. Preserve `step`’s required mutable Conversation borrow.

**Closure test:** Add compiled public API examples asserting that both returned futures satisfy `Future + Send + 'static`. Also compile examples that construct the futures, drop the Supervisor, and subsequently retain or await them. These examples need no native execution.

### [P3-1] Admission refusal closes a Windows handle inside Future::poll

**Location:** `src/supervisor/futures.rs:67–73`, specifically `this.origin.take()` at line 71; `windows/identity.rs:127–130,148–171`; `windows/handles.rs:28–34`. Contradictory promise: `api.rs:43–46`.

**Violated invariant:** The API states that `Future::poll` performs no OS call, while the caller documentation says later native cleanup is offloaded.

**Reproduction sequence:**

1. Invoke `start` on Windows with a valid request. `capture_origin` returns a `Captured` owner containing a real duplicated thread handle.
2. Cancel, expire the deadline, or close admission before the first successful registration.
3. Poll the future.
4. `Context::register` refuses; `this.origin.take()` immediately drops `Captured`.
5. `Handle::drop` synchronously invokes `CloseHandle` on the polling thread.

The existing refusal test uses an invalid request, so it never obtains the Windows origin handle and does not exercise this path.

**Impact:** The advertised polling boundary is false on an ordinary refusal path. Native resource disposal executes on the executor thread, without the offloaded ownership boundary used for other native cleanup. This is bounded: no process is launched and no secret is exposed.

**Required correction:** Retire captured native origin resources through an owned offloaded disposal path when polling refuses admission. Alternatively, if synchronous handle disposal is an intentional permitted exception, explicitly reconcile the checkpoint and API promises rather than claiming polling performs no OS call.

**Closure test:** Use a fake Origin with observable disposal provenance. Exercise cancellation, expiry and closed admission after successful origin capture, and verify the documented disposal boundary. Retain native handle-disposal qualification in the deferred Windows runtime tier.

### [P3-2] Pure tests bypass the production supervision ownership bridge

**Location:** `src/supervisor/test_support.rs:45–47`; `lifecycle_tests.rs:110–148,150–179`; `kernel_tests.rs:18–30`; production `monitor.rs`, `dispatch.rs` and full `io.rs` loops.

**Violated invariant:** Plan step 2 and the checkpoint require fake-port coverage of launch-return/resume, failed cleanup observations, owner failures and actual completion before permit release.

The sole fake Platform’s `launch` method panics because tests must never dispatch it. Lifecycle tests manually remove launch tickets and directly advance kernels. Cleanup helpers directly set every completion proof. I/O tests exercise framing helpers, but do not execute the complete production reader/writer ownership loops. There is no fake Child implementation.

**Reproduction:** Run the authorized supervisor filter: all 19 tests pass without executing a fake child through production launch, resume, termination, observation, I/O-owner retirement and launch completion. Read-only call-site searches confirm the only `Child` implementation is the Windows adapter.

**Impact:** Tests establish the pure proof predicate, but cannot detect a production orchestration error that supplies those proofs incorrectly, loses ownership during an owner failure, or mishandles a late launch. Those are precisely the bridge obligations this checkpoint introduces. Windows runtime deferral does not defer fake-port testing of the parent implementation.

**Required correction:** Add deterministic fake-port coverage of the production ownership bridge. A suitable approach is to extract bounded launch/reaper iterations behind the existing private ports, leaving thread creation and waiting in thin drivers. Avoid sleeps and native processes.

**Closure test:** Exercise late successful launch after cancellation with no resume; failed containment/resume; owner failure or panic; failed termination and process/Job observations; and delayed reader, writer and launch completion. Verify that production orchestration retains the permit and resources until every actual completion obligation is satisfied.

## 2. Invariant analysis

The following attacks did not expose an additional defect:

- **Dependency and package isolation:** Manifest and lockfile add only the approved target-Windows `windows-sys`/`windows-link` dependency chain. Public API types expose no core, transport, Git, Python, runtime or native handles. Publication remains disabled.
- **Wire compatibility:** Schema/generated artifacts are retained. The new bridge uses the generated borrowed projection and existing profile admission. Expected message kind, Error.phase, package, token cap and round are checked before owned token publication.
- **Hello before secrets:** `start` becomes ready only after admitted Hello. Begin encoding/sending is reached through the first `step(None)`, with exact schema/build/primary matching.
- **Terminal arbitration:** State updates sample time under the state guard and observe cancellation before transitions. Token publication performs a second guarded transition after taking owned bytes. First terminal failure remains fixed.
- **Capacity and IDs:** Records are installed before queued launch effects. Quarantine retains records. Checked IDs are scoped to a context; FIFO tombstone eviction and foreign IDs return Unknown.
- **Proof requirements:** The pure kernel requires process exit, empty Job and launch/read/write completion. Neither Finished nor kill/EOF alone satisfies it. The missing production-bridge coverage is reported separately.
- **Native containment structure:** Successful CreateProcess results become owned process/thread/Job values even when membership verification fails. There is no Create→Assign fallback. Resume follows membership admission; late terminal launch returns enter retained cleanup.
- **Unsafe storage ownership:** Attribute allocations are aligned and fixed; boxed referenced arrays survive until attribute-list destruction. Identity scratch is initialized, aligned and zeroizing. Blocking pipe buffers and handles remain owned by their I/O threads.
- **Lock boundaries:** Inspected transitions keep native operations, joins, handle destruction and host waker execution outside the state lock. Payload/task locks are not nested with it.
- **Scope discipline:** New conditional sections use enclosing modules and inspected control-flow bodies have braces. Documentation does not claim native provider or installed-host qualification.

## 3. Risks and next action

Native Windows behavior remains unqualified, as explicitly deferred. Cross-compilation cannot establish actual containment, originating-thread identity behavior, synchronous-I/O cancellation or cleanup. No additional finding is inferred merely from that deferral.

The next action is bounded remediation of the opaque-future lifetime contract, refusal-path disposal promise and production fake-port coverage, followed by Code re-review on a newly settled tuple. Native provider work and activation should remain behind their existing gates.
