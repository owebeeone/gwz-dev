# GwzSspiSupervisor — State-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; gwz-sspi `44879481fbd54dab84b99fecdadc89a34a84dcbd..fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`. Controlling document: `dev-docs/GwzSspiSupervisorCheckpoint.md`, **DRAFT implementation checkpoint**, dated 2026-10-03.
**Baseline:** Root `d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; gwz-sspi `fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`; reference-only gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Committed sources were read with `git show HEAD:` and bounded commit diffs. All three HEADs matched at both start and end.
**Date:** 2026-10-03
**Axis:** State machines, interruption, races, retained ownership, fail-closed cleanup and evidence coverage. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. One additional P3 finding concerns required orchestration coverage. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P3-1 as specified, without changing the accepted contract.

---

## 0. Evidence base

Instructions and authority inspected:

- Workspace `AGENTS_GWZ.md` and member `AGENTS.md`.
- `dev-docs/AgentProcessRules.md`, particularly exact-tuple review, independence, severity, actionable findings and closure rules.
- `dev-docs/GwzProcessOptimization.md`, including review depth, remediation cap and the later review-granularity ruling.
- `dev-docs/GwzSspiDesign.md` revision 2, especially §§4–8.
- `dev-docs/GwzSspiPlan.md`, particularly step 2.
- `dev-docs/GwzSspiSupervisorCheckpoint.md`, complete.
- Member `docs/Architecture.md`, `docs/Testing.md`, `docs/Supervision.md` and `docs/WireProtocol.md`, complete. The prompt’s `protocol/WireProtocol.md` path does not exist; `docs/WireProtocol.md` is the controlling member document, as the lane owner subsequently confirmed.

Implementation inspected:

- Entire `src/supervisor/kernel.rs:1–255`, `context.rs:1–353`, `api.rs:1–154`, `futures.rs:1–329`, `control.rs:1–98`, `driver.rs:1–61`, `dispatch.rs:1–68`, `monitor.rs:1–189`, `io.rs:1–165`, `ports.rs:1–42`, `values.rs:1–147`, and clock/platform selection.
- Entire Windows adapter: `windows/attributes.rs:1–78`, `handles.rs:1–128`, `identity.rs:1–172`, `launch.rs:1–211`, and `mod.rs:1–33`.
- `src/protocol/supervision.rs:1–154`, its tests, and the bounded adapter/module changes.
- Entire supervisor test support, lifecycle, kernel, control and I/O tests.
- `src/secret.rs:13–64` and its live-before-deallocation audit mechanism.
- Root checkpoint/caller-guide/drafter-report changes and member manifest, CI and public export changes.

Executed only the permitted existing pure unit binary:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 supervisor --nocapture
```

Result: **19 passed, 0 failed**, exit 0. The synthetic waker panic was caught and its test passed.

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 protocol::supervision_tests
```

Result: **2 passed, 0 failed**, exit 0.

The new counterexamples below are source-derived interleavings and retained-object sequences. No new test was authored or compiled under this read-only mandate. Owner build/lint/archive results remain owner evidence; they were not independently rerun.

Member status was clean. Reference core had an unrelated untracked document. Root had the declared out-of-scope prompts/documents and, at the final check, an untracked Surface report whose contents were not read. No writes, builds, compiler probes, native execution or Git mutations were performed.

## 1. Findings

### [P2-1] Reaping between token readiness and publication replaces the winning terminal error

**Location:** `gwz-sspi/src/supervisor/futures.rs:214–243`, particularly the fallback at lines 238–242; `context.rs:334–350`.

**Violated invariant:** First recorded terminal error wins. `docs/Supervision.md:44–47` states this explicitly, and accepted design §5 requires first-terminal arbitration under the state lock.

**Credible interleaving:**

1. A started `Step` has a valid token in `record.payload.token`; its kernel is `TokenReady { write_done: true, … }`.
2. `Step::poll` finds no terminal error and its readiness check at lines 214–216 returns true.
3. Before the subsequent publication update, cancellation wins. Cleanup finishes, payload retirement occurs, the dispatcher establishes actual launch completion, and the control driver reaps the record. `record.completion` now preserves `Cancelled`.
4. The step takes the token, or finds it already discarded, then calls `context.update` at lines 224–225. The record has been removed, so this returns `None`.
5. The catch-all match arm constructs `Protocol` and directly returns `context.failure(Some(record), fault)`. Unlike the neighboring error arm, it never calls `terminal` to recover the saved winning fault.

The same sequence can replace a winning `Timeout`. All cleanup proofs can be legitimate; the defect is in the public failure projection after record removal.

**Impact:** The caller receives `Protocol` despite a recorded cancellation or deadline expiry. This violates the promised first-terminal result and can misclassify an ordinary operation cancellation/timeout as worker protocol failure. Token publication remains revoked; this finding does not claim a late token escapes.

**Required correction:** When publication loses the live record, recover the saved terminal fault through the same authoritative terminal/completion path used elsewhere. Synthesize `Protocol` only if no recorded terminal cause exists. Keep the payload disposed.

**Closure test:** Deterministically pause a started step after its readiness check; then cancel or expire the record, retire its payload, establish cleanup proofs and reap it before resuming publication. Assert `Cancelled` or `Timeout`, respectively, with Confirmed cleanup and no returned token. Include both token-taken-before-reap and token-discarded-before-take orders.

### [P2-2] Completed failed steps retain their owned challenge indefinitely

**Location:** `gwz-sspi/src/supervisor/futures.rs:127–131`, terminal refusal at lines 166–168, preparation/send refusal at lines 197–202, and the success-only challenge release at line 211.

**Violated invariant:** Secret ownership cleanup must cover refused semantic/state checks and terminal conversation failures. `docs/WireProtocol.md:80–84,151–163` applies these obligations to subsequent caller challenges. The checkpoint specifically includes retained-future cleanup in this boundary.

**Reproduction sequence:**

1. Obtain a conversation between rounds with a token cap of 64 bytes.
2. Supply an owned 65-byte `SecretBytes` challenge to `step`.
3. Poll the step. Challenge admission rejects it as `InvalidRequest`; the conversation becomes terminal and the future returns `Ready(Err(...))`.
4. Keep that completed future alive.

The failed future still contains `challenge: Some(SecretBytes)`. The refusal path sets `done` but never takes the challenge. `Storage::drop` is the mechanism that wipes it, so the challenge remains live until the completed future itself is dropped.

A valid challenge also remains retained when cancellation or expiry is detected before the step starts: lines 166–168 return without clearing it. Supervisor cleanup cannot retire this storage because it belongs exclusively to the step future. It can therefore remain live after the record has been reaped.

**Impact:** Library-owned sensitive input survives terminal refusal and completed supervision solely because a caller retains an already-completed future. This is avoidable secret-lifetime extension, not a physical-erasure claim. The existing failed-start audit handles the analogous retained-future case, but steps do not.

**Required correction:** Dispose of the owned challenge on every terminal `Ready(Err(...))` path, including refusal before command preparation and preparation/send failure. A common completion helper is acceptable if it preserves fault arbitration and avoids destruction under the state lock.

**Closure test:** Attach the existing wipe probe to a challenge and retain the completed step future. Cover oversized challenge, wrong-phase challenge, cancellation before first poll and expiry before first poll. Assert the live-before-deallocation zeroization event occurs before `Ready(Err(...))` returns, without dropping the future and without writing a frame.

### [P3-1] Schedule tests bypass the production ownership orchestration

**Location:** `gwz-sspi/src/supervisor/test_support.rs:21–47,95–106`; `kernel_tests.rs:133–219`; `lifecycle_tests.rs`; `io_tests.rs:45–159`.

**Violated requirement:** Plan step 2 and checkpoint lines 89–96 require deterministic/seeded coverage around state/effect boundaries, including late launch, failed kill/wait/Job observations, owner failure/panic and release only after actual completion proofs.

**Evidence and consequence:**

- The only fake `Platform::launch` panics to prohibit dispatch.
- There is no fake `Child` implementation.
- Lifecycle tests manually install phases and cleanup proofs.
- `complete` directly sets all five proofs true and invokes `Context::reap`.
- Seeded schedules operate on `Kernel`, rather than registration, payload movement, dispatcher/launch ownership, reader/writer loops and reaping.
- I/O tests execute framing helpers, but not the conversation reader/writer loops.

Consequently, the suite proves that the kernel respects supplied proof bits; it does not exercise how the production owners produce those bits or how futures race with record removal. P2-1 illustrates a seam that these schedules cannot reach. The required failed-observation and owner-failure sequences are likewise absent from executable fake orchestration coverage.

**Required correction:** Add deterministic fake-port coverage of the actual orchestration decisions. It may use bounded stepping/extracted loop decisions to preserve process-free, sleep-free tests. Exercise ownership publication, late launch, failed observations, I/O failures and owner completion, rather than assigning all proofs directly. Update coverage claims to distinguish kernel schedules from orchestration schedules.

**Closure test:** Fake launch/child/read/write outcomes must demonstrate a late successful launch after cancellation without resume/secrets; failed termination and wait/Job observations retaining capacity; unfinished reader/writer ownership preventing Confirmed cleanup; owner failure/panic; and final disposal/join before permit release. Retain reproducible action traces.

## 2. Invariant analysis

The following attacks did not produce additional findings:

- **Admission and IDs:** Registration checks cancellation, deadline, closure, FIFO position and capacity under the state lock. A record exists before dispatch. Checked sequence increment refuses overflow. Foreign IDs and FIFO-evicted tombstones return Unknown.
- **Immutable deadline:** No command installs a fresh deadline. `Context::update` samples the clock under the state guard, preventing stale pre-lock time from bypassing expiry. Existing tests cover admission and publication with stale timestamps.
- **Late result exclusion:** Kernel transitions reject terminal Hello/Token/Finished publication. A token requires both the correct reply and completed write. Round-eight Continue refuses; Complete prohibits another challenge.
- **Cleanup proof direction:** All five proof fields are required. The Windows adapter derives process exit from the held process handle and Job emptiness only from a successful query. Kill requests and advisory I/O cancellation do not establish those proofs.
- **Actual owner disposal:** The monitor keeps `launch_finished` false while reaping, joins finished reader/writer owners, retires payloads and the held child, and returns. The dispatcher sets launch completion after joining that thread. I found no source path that normally releases capacity merely on EOF, Finished or a termination request.
- **Lock ordering:** State transitions are separated from payload/task/launch ownership locks. Native calls, joins, payload disposal and waker invocation occur outside the state lock. Reentrant and panicking-waker tests pass.
- **Drop and shutdown:** Pending starts/steps and armed conversations initiate cancellation. Supervisor shutdown closes admission synchronously and uses its separate deadline. Outstanding records remain context-owned.
- **Windows containment structure:** Creation uses suspended `CreateProcessW` with Job and explicit handle-list attributes, followed by membership verification before resume. Attribute storage and referenced arrays are boxed and survive through attribute-list destruction. Successful creation returns held ownership even when membership verification refuses.
- **Originating-thread provenance:** Start captures an owned original-thread handle synchronously. Charged launch rechecks that handle and primary identity; polling does not substitute executor-thread metadata.

These are source-level and permitted pure-test conclusions. They do not qualify Windows runtime behavior.

## 3. Risks and next action

Native Windows process execution, provider disposal, worker entry and installed-host composition remain explicitly deferred. Cross-compilation and source inspection cannot close those qualification rows. No additional finding is raised merely because those deferred outcomes are unproved.

The next action is bounded remediation of P2-1 and P2-2, with the missing fake orchestration coverage in P3-1, followed by State re-review on a newly settled tuple. Acceptance of this supervision checkpoint should remain pending.
