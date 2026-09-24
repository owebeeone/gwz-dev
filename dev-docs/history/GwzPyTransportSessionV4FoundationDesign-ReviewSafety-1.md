# GwzPyTransportSessionV4FoundationDesign — SAFETY-AXIS RE-REVIEW

**Review object:** The first remediation of the DRAFT v4 foundation design, committed 2026-09-24. It consists of:
- `dev-docs/GwzPyTransportSessionV4FoundationDesign.md` at root `11358352a237a673ecf0fd82347b45ca4aca8950`. Status: "DRAFT v4 foundation design … first remediation … no implementation, build, or activation authority".
- The v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` at gwz-core `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`.
- The caller guide `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md` and the precedence pointer in `gwz-py/dev-docs/GwzPyTransportDesign.md` at gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`.

**Baseline:**
- Root `11358352a237a673ecf0fd82347b45ca4aca8950`.
- gwz-core `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`.
- gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`.
- gwz-transport `36ae2b13d7beaf289c72143e2f451c76489110ed`, matching its lock pin.

The reviewed revision was root `fbee49c`, gwz-core `a1f2102`, gwz-py `0ca424f`. All documents were read from committed objects with `git show <sha>:<path>`. I verified the three HEADs and a clean `git status --short` on every reviewed file at the start and at the end; nothing moved.

**Date:** 2026-09-24
**Axis:** Safety — what the text permits to go wrong: ownership intervals, transfers, arbiters, stuck states, recovery facts and blast radius. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1 or P2 finding is open.
- All nine round-1 findings (P2-1 to P2-4, P3-1 to P3-5) are closed. Each original counterexample was re-traced on the corrected text.
- The remediation introduces four new P3 findings (P3-6 to P3-9). They do not block.
- No finding in this round is a new architectural root cause.
- My round-1 pre-commitment (GO on a revision resolving P2-1 to P2-4 as specified) is honoured.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P2-1 (entries without an issued ID; capability preflight constructs outside the owned bootstrap) | B2. §7.6 defines three entries without an issued ID. The preflight becomes the constructing owner and answers from an installed, bootstrapped runtime. Local unary commands bypass the ledger. Local submitted operations are session records. | Re-traced the identity-option fetch on a fresh Client. The preflight finds no runtime and creates the `Construction` value (§3.6, line 86). It runs `from_environment_for_session()`, then `bootstrap_ready()` under 6 s, then the lease's `finish()` under 11 s. It installs the runtime only after both (§7.1, line 301) and answers from it (§7.6, line 363). The fetch's claim then finds a bootstrapped runtime, and phase 1's Ready check passes. `status` and `commit` no longer enter `claim` (§7.2 line 323; §7.6 line 364). A submitted `merge` is a session-minted record that the bridge resolves (§7.6 line 365; I10 line 103). No path installs an unbootstrapped runtime. | **Closed** |
| P2-2 (6 s finish clock below core's composite deadline) | B1. Each clock is the sum of the sequential core deadlines plus 1 s. §7.5 tabulates the values. | Re-traced the 3 s + 4 s sequence and the 5 s + 5 s worst case against `request.rs:328–339`, `session.rs:792–808` and `driver.rs:470–517`. Core returns by about 10 s; the 11 s clock (§7.5 line 353) does not win on a live supervisor. C3–C5 settle with core's report, and §12 line 462 says the session is not faulted. Every other bound matches its source:<br>– `admit_local` 6 s: one arrival deadline;<br>– `bootstrap_ready` 6 s: the mux deadline from Bind;<br>– lease finish, `RegisteredCleanup` and `shutdown` 11 s: two sequential windows (`mod.rs:263–267`). | **Closed** |
| P2-3 (panic in bootstrap leaves `constructing` set; close hangs) | B6. A `Construction` value is held across all construction steps, and its `Drop` releases on unwind. | Re-traced a panic in `bootstrap_ready()` after Bind, with claim B waiting and close pending.<br>– The `Drop` clears `constructing`, sets `faulted`, closes the partial runtime's sessions and notifies the condvar (§3.6 line 86; §5.3 line 190).<br>– B refuses `NotRegistered` (A3, line 152), and A settles through its `Claim` guard.<br>– Close's wait on `slots == 0 && constructing == false` (§9 line 399) terminates.<br>The same holds for `from_environment_for_session()`, the lease's finish and the failure-path shutdown (§12 line 443; §13 item 10). | **Closed** |
| P2-4 (core or guard closes leave the Client Open and unfaulted) | B7. A generation fault rule (`generation_open()`), a typed `GenerationClosed`, and `TransportGenerationBusy` afterwards. | Re-traced a retirement timeout after pool mutation. `admit_local` returns `GenerationClosed` (§6.2 lines 237–239, 248), and A6 sets `faulted`. The next `reserve`, `begin_attempt` and `claim` refuse `TransportGenerationBusy` (S2/S4/S7), and the guide says to close and recreate the Client (line 62). A post-mutation cancel (A7), a driver-mux close (A6) and a sibling's cleanup-expiry close under an accepted operation (the C-row generation check; §12 line 465) each set `faulted` through the check (§3.6 line 90). | **Closed** for every generation close. The stalled-supervisor path, which closes nothing, is new P3-9. |
| P3-1 (reserved bootstrap ID collides with caller IDs) | B5. A 128-bit random token; legacy runtimes keep 256. | `bootstrap-1` is now an ordinary caller ID. The reserved ID carries a random token, is not the session nonce and is reported nowhere (§6.1 line 224; §11 item 6). Legacy constructors keep 256 and refuse bootstrap (§6.1 lines 204–208; GWZRequirements line 11). | **Closed** |
| P3-2 (progress sketch cannot see writes inside phase 2) | `ProgressHandle(Arc<AtomicU8>)`, obtained before phase 2. | The handle is taken before `register_and_open(self)` consumes the Admission and is attached to the `Claim` (§6.3 lines 264–267 and 275; §7.2 step 5). Writes inside phase 2 are visible after the caught unwind on the same thread, and `Registered` is never reset. | **Closed** |
| P3-3 (loop-bound `asyncio.Lock` hangs across loops) | A loop-agnostic in-flight future (`threading.Lock` plus `concurrent.futures.Future` awaited via `wrap_future`). | The loop-bound lock and its non-threadsafe wakeup are gone (§8 line 393). `wrap_future` completes waiters through `call_soon_threadsafe`, so the original hang no longer reproduces. The replacement has its own defect (P3-6). | **Closed**; new P3-6 |
| P3-4 (retry example leaks handles on cancellation) | Rewritten example: one scope per handle, cancel-and-join on abnormal exit, one release. | Traced cancellation at both `accepted()` awaits and at `result()`, plus a consumed refusal, a second refusal, success and failure. `except BaseException` cancels and joins, then re-raises. `finally` releases exactly once, and no retry follows a cancellation (guide lines 67–88). The hierarchy is stated: `GwzOperationCancelled` is a `CancelledError`, not a `GwzBridgeError` (guide line 55; V4 §5.1 line 131). The `result()` wait-only rule and `cancel()`'s shielded join are specified (guide line 38; V4 §11 item 9). | **Closed** |
| P3-5 (requirement forbids expiry closes that core keeps) | B4. The requirement is restated and scoped. | GWZRequirements line 11 now forbids only new timers, callbacks or threads with authority over caller records. It keeps the mux bootstrap and route deadlines and the cleanup-expiry close as errors observable through a synchronous query. It scopes the owner-clock MUST to the Python-session path. This matches GWZDesign line 11 and V4 §3.5 line 82. | **Closed** |

## Changed-range analysis

**What changed**
- **V4 design:** 445 changed lines in the root diff `fbee49c..1135835`.
- **Core:** one paragraph each in `GWZDesign.md` and `GWZRequirements.md` (`a1f2102..d43b147`).
- **Caller guide:** the `request_id_consumed` meaning, a per-code retry table, the rewritten example, the `result()`/`cancel()` rules and the `GwzOperationCancelled` attributes (`0ca424f..0ffc4cb`).
- **Transport-design pointer:** a 10-line precedence pointer in `GwzPyTransportDesign.md`.
- **Nothing else:** `git diff --name-only` shows no source file changed in any member. The root also changes `gwz.conf` pins and markers (bookkeeping) and adds the round-1 reports, verdict and remediation plan in `9d846c2`. The round-1 source analysis therefore still describes the implementable starting point.

**Every change maps to a remediation-plan disposition:**
- B1: §3.5 and §7.5.
- B2: §3.6 and §7.6, the L-rows, I8 and I10.
- B3: §7.1 and S5/S6.
- B4: the core paragraphs, §3.5 and §3.6.
- B5: §6.1.
- B6: `Construction`, I4 and A3.
- B7: the fault rule, `AdmitRefusal::GenerationClosed`, S2/S4/S7, §11 item 7 and the guide.
- The nonblocking items and adopted residuals: §3.3's refused transfers, `ProgressHandle`, `BootstrapLease`, the in-flight future, the example, §11 items 8–11, the level check at checkpoint 1, the `except BaseException` split and feature detection.

The new named mechanisms are each the direct implementation of a disposition: `generation_open()`, `AdmitRefusal`, `from_environment_for_session()`, `BootstrapLease`, the supervisor's final retirement pass, non-cancellable local operations (L6) and the shielded `cancel()` join. I found no change outside the dispositions.

**NEW ARCHITECTURAL root causes: none.** The four new findings are Python-bridge or contract-wording defects in mechanisms the remediation introduced (P3-6 to P3-9). Each has a bounded text correction inside the R1–R3 protocol, and none touches an ownership value, a transfer or the arbiter rule.

## 0. Evidence base

**Tuple and diff scope**
- `git rev-parse` and `git status --short` on all reviewed files, at the start and at the end.
- `git log` and `git diff --stat` / `--name-only` for all three repositories over the stated ranges.

**Legitimate prior-round inputs**
- `GwzPyTransportSessionV4FoundationDesign-Verdict.md` and `-RemPlan.md`, read in full at root `1135835`.
- The headings of my own filed round-1 report.
- I did not read the round-1 Consistency or Surface reports.

**The object**
- The revised V4 design, lines 1–510, read with `nl -ba`.
- GWZDesign.md and GWZRequirements.md line 11 at `d43b147`.
- The caller guide at `0ffc4cb`, lines 1–3 and 36–110 (the table at 57–65 and the example at 67–88).
- The `GwzPyTransportDesign.md` pointer diff, plus its heading list to confirm that the capability rule is in §4.

**Source, unchanged since round 1 and re-cited from that read**
- gwz-core `request.rs` 328–361, `session.rs` 449–512, 663–679 and 792–863, `driver.rs` 242–257 and 470–517, `mod.rs` 247–269.
- gwz-transport `mux/mod.rs` 242–293 and 464–541.
- gwz-py `bridge.py` 252–259 and 330–383, `client.py` 349–398 and 998–1039, `dispatch/merge.rs` 1–41.

**CPython stdlib, installed 3.13, read-only**
- `asyncio/futures.py` 342–356 (`_copy_future_state`), 384–396 (`_chain_future._call_check_cancel`) and 406–412 (`wrap_future`).
- `concurrent/futures/_base.py` 364–378 (`Future.cancel`), 497–505 (`set_running_or_notify_cancel`) and 537–544 (`set_result`).
- The project requires Python ≥3.10, where these semantics hold.

**No builds, tests, formatters or writes.**

## 1. Findings

### [P3-6] The in-flight `accepted()` future lets one waiter's cancellation cancel the others, and does not track the native record

**Location**
- V4 §8 line 393: "a `threading.Lock` guards a per-handle `concurrent.futures.Future` that the first caller completes and every other caller awaits through `asyncio.wrap_future`".
- The guide at line 110 allows cross-loop handle use.
- V4 §5.1 line 131 says `GwzOperationCancelled` is raised only to a caller whose own task was cancelled.
- v2 §2: a cancelled `accepted()` waiter cancels and joins the operation.
- Stdlib behaviour:
  - `_call_check_cancel` calls `source.cancel()` when a wrapper is cancelled.
  - A PENDING `concurrent.futures.Future` accepts `cancel()`.
  - `_copy_future_state` then cancels every other wrapper.
  - `set_result` on a cancelled future raises `InvalidStateError`.

**Violated invariant:** Concurrent callers observe the first caller's outcome. No caller receives a cancellation it did not request. A cancelled waiter cancels and joins the operation (v2 §2).

**Sequence**
1. Thread A calls `h.accepted()` first and runs `_admit` while native admission is in progress.
2. Threads B and C, on their own loops, await `wrap_future(fut)`.
3. B's task is cancelled, for example by its `asyncio.timeout`. B's wrapper is cancelled, which cancels the shared future (still PENDING) and therefore C's wrapper.
4. C's task raises `CancelledError` although nobody cancelled it.
5. B raises a plain `CancelledError`. It neither cancels nor joins the operation, and carries no `GwzOperationCancelled` evidence.
6. A's later `fut.set_result(...)` raises `InvalidStateError`, so A's `accepted()` fails after a successful admission. A caller following the guide's pattern then cancels the admitted operation.

Two related sequences:
- If A's loop stops mid-admission, or A's coroutine is closed without completing the future in a `finally`, B and C wait forever, even after the native record settles.
- If A's own task is cancelled and the future carries A's `GwzOperationCancelled`, B and C receive a `CancelledError` subclass, contrary to §5.1 line 131.

**Impact:** Spurious cancellation crosses tasks and loops, so TaskGroups on B and C unwind. A successful admission can be reported as an untyped `InvalidStateError`, and waits can become indefinite. Native state is unaffected.

**Correction:** Either:
- make waiters observe the native record, through a native wait for phase ≥ `Accepted` or `Terminal` run off-loop; or
- keep the shared future but make it safe:
  - call `set_running_or_notify_cancel()` before publishing it;
  - await it through `asyncio.shield(asyncio.wrap_future(fut))`;
  - complete it in a `finally` with the retained disposition, never a caller-specific exception;
  - project per waiter: a cancelled waiter runs cancel-and-join and raises `GwzOperationCancelled`; the others get the handle or the typed refusal.

**Closure test:** Three threads, each with its own event loop, call `accepted()` on one handle while admission is held. Cancel the second waiter: the first and third observe the admitted handle, and the second cancels and joins per v2 §2. Separately, stop the first loop mid-admission: the other waiters complete from the native terminal.

**Classification:** bounded correction. Not architectural.

### [P3-7] A cancelled local submission loses its natively minted ID, still runs, and leaks a ledger record

**Location**
- V4 §7.6 item 3 (line 365); §5.2 L1 and L6 (lines 176 and 181); §3.4 line 74; §11 item 10.
- The ID-less bridge path, which V4 leaves unchanged (`bridge.py:341–383`): with no operation ID, the `CancelledError` branch skips `cancel_operation`, awaits the worker and re-raises, discarding the native result.
- `client.py:998–1039`: `merge_stream` learns the operation ID only from the returned `Accepted` envelope.
- v2 §2: no accepted operation whose ID the caller was never given; a cancellation never strands a charged record.

**Sequence**
1. A caller awaits `client.merge_stream(...)`. The native `submit` is queued, for example because every default-executor thread is busy with inline network `call()` work, or it is already in flight.
2. The caller's task is cancelled.
3. The bridge waits for the worker. The native submit later mints the record (L1), starts the merge, and returns the `Accepted` envelope, which the bridge discards when it re-raises.
4. The merge cannot be cancelled (L6) and close does not join it. It completes and mutates the workspace after the caller saw cancellation, and its terminal record stays in the 64-record ledger.
5. The ID was never delivered. With `recent_operations()` and expiry deferred, only `close()` reclaims the record.

Repeated cancelled local submissions exhaust the ledger, after which every network and local submission refuses `TransportSessionFull`.

Part of this predates V4: the late local mutation already happened on the legacy path. V4's delta is that the record now counts against the session's 64-record limit and its natively minted ID cannot be recovered.

**Correction:** On the ID-less submit path, when the native call returns an `Accepted` envelope after the caller was cancelled, attach the minted ID and effect to the raised exception and release the record, following v2 §2's detached-record rule. State in the guide that a cancelled local submission may still start and complete. The alternative is to mint local IDs before entry so that a queued submission can be settled.

**Closure test:** Saturate the default executor, submit a merge, cancel before native entry, then free the executor. The raised exception carries the operation ID and effect, or the record is released. 65 repetitions never produce `TransportSessionFull`.

**Classification:** bounded correction. Not architectural.

### [P3-8] Under the new generation-wide definition, `request_id_consumed=false` is unproved for refusals made before core's duplicate check

**Location**
- V4 §5.1 line 129: the field states "whether this request ID is registered in the current generation, or that cannot be ruled out … `false` only for `NotRegistered`".
- V4 §11 item 2 (line 412).
- Guide line 55 ("`False` means the ID is not registered") and the table rows at lines 60–61.
- These rows settle `NotRegistered` before any duplicate check: S2 and S6 (lines 140, 144), A1 and A2 (lines 150–151), A7 (line 156), and A4's leadership-wait timeout.
- Core `session.rs:458–480`: the leader wait and the closed-session check return before the duplicate check at lines 481–486.

**Violated invariant:** I5. A reported fact must be established by the path that reports it.

**Sequence**
1. Request ID `r` completes in this generation, so it is registered.
2. A new handle for `r` is refused at `claim` with `TransportSessionFull` because all eight slots are busy (S6), or times out waiting for admission leadership (A4 IoError). Either way it reports `request_id_consumed=false`, which the guide reads as "not registered".
3. The caller follows the table and retries.
4. The retry is refused as `AlreadyRegistered`, now with `true`.

**Impact:** A misleading recovery fact and one wasted retry, which ends in a typed pre-effect refusal. There is no effect and no false success.

**Correction:** Scope `false` to the attempt: "this attempt did not register the ID, and core did not report it as already registered". Adjust the guide sentence and the table accordingly.

**Closure test:** Register `r`, then force S6 and the leadership timeout for new handles on `r`. The documented meaning of `false` matches what the refusal proves, and the retry reports `AlreadyRegistered` with `true`.

**Classification:** bounded text correction.

### [P3-9] A stalled supervisor never faults the Client; it keeps returning retryable `IoError` refusals

**Location**
- V4 §3.5 line 80: "an owner clock wins only when the supervisor is dead".
- V4 §3.6 line 90: the fault rule is keyed only on `generation_open()`, which a stalled supervisor never changes (§6.1 lines 214–216).
- §5.2 A8 (line 157) faults only if the drop closed the generation. C6 (line 170) and a timed-out construction shutdown fault unconditionally.
- §12 lines 449–450 give the retry as "yes if the generation stayed open". §13 item 4 (line 488) and guide line 61 say back off and retry.

**Violated invariant:** The intent of B7 and §11 item 7. A Client whose transport has failed says so, and recovery facts do not invite endless retries.

**Sequence**
1. No network operation is live. The installed generation's placement supervisor stalls (a hang, not an exit — the case test 4 injects).
2. Each new admission's phase 1 is never re-polled, so the 6 s owner clock wins (A8).
3. The generation check reads `generation_open() == true` and settles `Refused/NotRegistered IoError`, `request_id_consumed=false`.
4. The guide says to retry. Every retry blocks for 6 s and fails the same way.
5. The Client becomes `faulted` only if some accepted operation reaches finish (C6).

**Impact:** A dead transport is reported as a retryable timeout indefinitely, contradicting §3.5's own premise. Nothing is registered and there is no effect.

**Correction:** Treat any owner-clock win as a generation fault: A8 sets `faulted`, consistent with C6. Alternatively, have `generation_open()` report a supervisor that has not advanced within its bound. Update the §12 rows and the guide's `IoError` row.

**Closure test:** Stall the supervisor with no live operation. The first phase-1 clock win faults the Client, and the next attempt refuses `TransportGenerationBusy`.

**Classification:** bounded text correction. The underlying A8 behaviour predates this round; the gap arises against the new fault rule and retry table.

## 2. Invariant analysis

These attacks failed.

**Refused transfers (R2, I1, I3)**
- S4, S6 and S7 settle the record atomically under the session mutex, on the sender's behalf.
- A later `abandon_attempt` is a no-op that returns that disposition, so the refusal keeps its typed code through `_bridge_error_from`.
- A racing `abandon_attempt` and refused `claim` still produce one terminal.
- A local mint and its `Claim` are one step; L2 refuses without creating a record.

**Construction ownership (R1 at generation level)**
- The `Construction` value is created with `constructing`, held across all four construction steps, and released on success, failure, clock win or unwind.
- The preflight and claims share it.
- A waiting claim is woken by its own cancel, and checkpoint 1 reads cancellation as a level. This closes my round-1 edge-trigger residual.
- Close joins through `constructing == false`.

**Owner clocks (R3)**
- Every bound was checked against the core deadlines that run in sequence inside it (§7.5).
- A live supervisor's deadlines fire first.
- The hardening's final retirement pass completes sealed registrations, so a supervisor exit needs no clock.
- The hardening writes only core cleanup reports, never native terminals.
- Timer and completion meet only in the owner's select.

**Generation fault rule**
- `generation_open()` uses the session `is_closed()` checks, which include the owner's mux phase, plus the driver mux's Ready phase. A closed generation never reopens, so the observation is monotonic.
- It is read outside the native mutexes, and `faulted` is set before the owner's own settle.
- `faulted` only refuses new transfers, through refused transfers. It never decides an existing record, so there is no second decider.
- Close shuts the runtime down only after `slots == 0`, so a network owner cannot fault the session through close's own shutdown.

**Ledger at `claim` (B3)**
- The upgrade is taken with the slot before any core work.
- S6 settles `NotRegistered`, `accept` stays infallible, and I6 is unchanged.

**Entries without an issued ID**
- Local operations have one native owner from mint through the §7.3 gate to settle.
- Cancel and close never decide a local outcome.
- The unary merge store is discarded when the call returns.
- Bridge lookups never reach the process-global store (I10).

**Python owner (§8)**
- `except Exception` settles an `Attempting` record and raises the typed refusal.
- `except BaseException` settles it and re-raises unchanged. This covers GeneratorExit, KeyboardInterrupt, SystemExit and PyO3's `PanicException`.
- A late native `claim` reads the settled terminal.
- The bridge feature-detects `begin_attempt`, which covers mixed versions.

**Truthfulness**
- `effect="none"` appears only before `Accepted` (L3 for local operations).
- `true` is reported for `AlreadyRegistered`, `Registered` and `MayHaveRegistered`.
- The C-row precedence never publishes success without a returned `finish()`, and C6 discards staged output.

**Retry example:** Every exit path was traced, including repeated cancellation during `cancel()`, whose join is shielded.

## 3. Risks and next action

These residual risks are below the finding bar.

**Unwind and drop paths**
- A14 closes the installed shared generation from a `Drop` guard, but the core API names no synchronous generation close; `shutdown()` is async. `faulted` still forbids reuse.
- The generic guards set `faulted` on unwind:
  - an `AcceptedOperation` drop (C7) for a local worker's lost owner;
  - a `Claim` drop (A14) for a panic while decoding a local submission.
  Either would disable network work because of a local fault.
- An `Attempting` record whose event loop stopped before its worker task ran is now excluded from expiry (§11 item 11). Only cancel or close settles it, and release refuses it with `OpenOperation`.

**Local submitted operations**
- They can still write the workspace after `close()` returns. This is documented and predates V4.
- The size of the local ledger allowance (L1) is unspecified. If it draws on the same 64 MiB, live local submissions reduce the eight network admissions v2 allows. The result is only a pre-effect `TransportSessionFull`; this belongs to the deferred ledger stage.

**Guide accuracy:** The guide's `TransportSessionFull` row omits "after a live operation finishes" for slot exhaustion; V4 §12 line 434 includes it.

**Out of the stated failure model**
- A supervisor hung inside `drive()` while holding core's session mutex (now stated).
- The unclocked `from_environment_for_session()` (stated).
- The pre-existing two-worker control executor, which can queue close behind blocked cancels.

**Next action:** Record Safety GO on this tuple. Fold P3-6 to P3-9 into the design text before the implementation tuple. Implementation starts only after the merged re-verdict records GO on every axis, with fresh Code and State reviewers on the new exact tuple.
