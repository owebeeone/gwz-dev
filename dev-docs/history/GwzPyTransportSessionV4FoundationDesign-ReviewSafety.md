# GwzPyTransportSessionV4FoundationDesign — SAFETY-AXIS REVIEW

**Review object:** The DRAFT v4 foundation design package committed on 2026-09-24:
- `dev-docs/GwzPyTransportSessionV4FoundationDesign.md` at root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`. Its status is "DRAFT v4 foundation design … no implementation, build, or activation authority".
- The v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` at gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`.
- The `request_id_consumed` note and retry example in `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md` at gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`.
- The WITHDRAWN status line on the v3 draft.

**Baseline:**
- Root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`.
- gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`.
- gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`.
- gwz-transport `36ae2b13d7beaf289c72143e2f451c76489110ed`, which matches its root lock pin. Its only dirty file is `Cargo.toml`, which is out of scope.

All reviewed documents and source were read from committed objects with `git show <sha>:<path>` or `git show HEAD:<path>`. Working-tree content was not used. I verified the three HEADs and a clean `git status --short` on every reviewed file at the start and at the end. Nothing moved.

**Date:** 2026-09-24
**Axis:** Safety — what the text permits to go wrong: degraded and mixed-version paths, irreversible steps, stuck states, the truthfulness of recovery facts, and blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — four P2 and five P3 findings are open. The four P2 findings block. Every P2 has a bounded text correction inside the stated R1–R3 architecture. **No finding is a new architectural root cause.** I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3 and P2-4 as specified. The P3 findings do not block.

---

## 0. Evidence base

**Process and prior-object inputs**
- `AgentProcessRules.md`: L1-19 to L1-21, L2-15 and §7.1.
- `GwzProcessOptimization.md` §4.1 (the two-round cap and the third-new-root-cause stop).
- `GwzPyTransportSessionV2Design.md` (all sections) and `GwzPyTransportSessionV2-Verdict.md`.
- `GwzPyTransportSessionV2Foundation-Verdict-3.md`, `-ReviewCode-3.md` and `-ReviewState-3.md`.
- `GwzPyTransportSessionV3FoundationDesign-Verdict.md`, `-Verdict-1.md` and `-RemPlan.md`.
- Every `GwzPyTransportSessionV3FoundationDesign-Review*.md`: Safety, Safety-1, Consistency, Consistency-1, Surface and Surface-1.
- `gwz-py/dev-docs/GwzPyTransportDesign.md` §§1–3, including line 200–202: the capability preflight and dispatch must use the same live runtime generation.
- `gwz-core/AGENTS.md`.

**The object**
- The full V4 design, lines 1–393, read with `nl -ba`.
- `git show fbee49c`, `git -C gwz-core show a1f2102` and `git -C gwz-py show 0ca424f`.
- The caller guide at `0ca424f`: lines 1–101, with the example at 55–79.

**Current source (the starting point V4 must be implementable against)**
- gwz-core `transport_host/mod.rs`, all lines:
  - `request()` at 185–237;
  - `shutdown()` at 247–269;
  - `unavailable` → `IoError` at 344–346.
- gwz-core `session.rs`, all lines:
  - `CLEANUP` = 5 s at line 30;
  - `CapacityMutation` at 240–250;
  - the supervisor thread at 313–333;
  - `admit_client_request` at 449–512, which refuses a closed session with IoError at 478–480;
  - `install_capacity` at 514–679, which closes internally at 663–672;
  - `register` at 680–705;
  - `seal` and `finish` at 766–808;
  - `cleanup` at 850–863;
  - `inbound_port` at 864–898.
- gwz-core `session/driver.rs`, all lines:
  - `drive()` at 242–524;
  - mux Closed → `close_state` at 250–257;
  - the sealed-registration loop at 470–517, which closes on CLEANUP expiry at 498–501.
- gwz-core `request.rs`, all lines:
  - `TransportRequest::finish`, a driver wait followed by an endpoint wait, at 328–339;
  - `Drop` at 341–346;
  - `ClientRequest::finish` at 359–361;
  - `identifier` at 390–392.
- gwz-core `local_command.rs`, lines 1–74: legacy per-command runtime isolation.
- gwz-core `https_endpoint.rs`, lines 55–115: the endpoint owns its own runtime thread, not the session executor.
- gwz-transport `mux/mod.rs`, all lines:
  - Config default of 256 requests and 5 s deadlines at 44–55;
  - `register` inserts only after all checks, at 242–269;
  - `begin` at 270–293;
  - cancel of the Binding offer disconnects the mux, at 464–469;
  - `finish` at 516–526;
  - `advance` expiry disconnects at 527–541;
  - `identifier` at 650–652.
- gwz-transport `mux/asynchronous.rs` (all lines) and `mux/routing.rs` (all lines): an Open or Bind for a request that is not live is a Protocol error that disconnects the mux, at 10–15, 41 and 110.
- gwz-py `native/src/transport_session.rs`, all 1512 lines:
  - capabilities → `self.runtime()` at 469–483;
  - `runtime_with` installs the runtime without bootstrap and resets `constructing` explicitly, at 381–457;
  - ID-less local calls at 491–500 and 1245–1262;
  - close at 878–958.
- gwz-py `operations.rs` (all lines), `dispatch/mod.rs` (1–200 and 360–548), `dispatch/merge.rs` (1–41), `shims.rs` and `lib.rs`.
- gwz-py `src/gwz/bridge.py`, all lines: `_operation_source` routes every by-ID lookup to the session at 252–259.
- gwz-py `client.py`: the capability preflight precedes network calls at 349–398; `MergeOperationHandle` is at 1322–1346.
- gwz-py `errors.py`, all lines, and `pyproject.toml` (`requires-python >=3.10`).

**Commands run**
- `git rev-parse` and `git status --short`, at the start and at the end.
- `git show`, `git grep` and `rg`, all read-only.
- No builds, tests or file writes.

## 1. Findings

### [P2-1] ID-less calls into native `call`/`submit` are unspecified, and the capability preflight constructs the session runtime outside the owned bootstrap step

**Location**
- V4 §7.2, line 256: `let claim = self.claim(operation_id)?;` is the first statement of every `call`/`submit`.
- V4 §3.6, line 77: construction is owned by "the first admitting claim that finds no runtime".
- V4 §7.1, line 242: `runtime` is "installed only after bootstrap()".
- V4 §6.2, line 194, and the §12 row at line 344: phase 1 refuses when the driver mux is not Ready.
- V4 I10, line 90, and §13 item 8, line 381.
- V4 never mentions `transport_capabilities` or ID-less local calls.
- Current source:
  - `transport_session.rs:469–483`: `transport_capabilities` → `self.runtime()`, which calls `runtime_with`. That installs the runtime right after `from_environment()`, with no bootstrap (lines 381–457).
  - `transport_session.rs:491–500` and `1245–1262`: local commands enter with no ID.
  - `client.py:349–372`: `_require_transport_capability` runs before any network call that carries identity options.
  - Core `GWZDesign.md` lines 194–199 make that preflight mandatory.
  - `GwzPyTransportDesign.md:200–202` requires the preflight and the dispatch to use the same live runtime generation.

**Violated invariant**
- R1/§3.6: every generation-level construction has an owner running under a clock.
- §7.1: no runtime is installed before bootstrap.
- Blast radius: local-command and capability paths keep working.

**Reproduction**
1. On a fresh Client, call `await client.fetch(...)` with `transport.default_identity` set.
2. `client.py` first sends `bridge.call("transport_capabilities", …)` to native `call` with no operation ID. V4 defines no entry for that call.
3. If the existing special case is kept, `runtime()` installs an unbootstrapped runtime.
4. The fetch's claim then finds a runtime installed and skips construction. Phase 1's "driver mux is Ready" pre-check sees `Unbound` and refuses with `Refused/NotRegistered IoError` ("yes on a later generation").
5. No later generation exists because rollover is deferred. Every network operation on that Client refuses until close.
6. If the implementer instead bootstraps inside the capabilities path, construction runs with no claim or record. Nothing then specifies the owner's clock, the refusal of waiting claims on failure, or close's join.

The same entry rule leaves ID-less local unary commands (`status`, `commit`) and local submitted operations undefined. Local submitted operations include `merge`, whose `op_<request_id>` records live in the legacy module store (`dispatch/merge.rs:23–41`). `bridge.py:252–259` routes their by-ID lookups to the session, and I10 and §13 item 8 forbid the session from resolving legacy-store IDs.

**Impact:** One of two outcomes follows, and both break a documented public path:
- every network operation refuses for callers who use explicit SSH identity options, or
- construction happens with no owner, which violates R1/R3 at generation level.

Local commands and merge results through a native-session Client are also left without a contract.

**Required correction**
1. Specify the ID-less entry:
   - Local unary and submitted commands bypass `claim` and are gated only by the session status (state whether `faulted` refuses them).
   - Legacy-store IDs (merge) get a lookup route consistent with I10.
2. The capability preflight either:
   - answers from an installed, bootstrapped runtime, or
   - constructs through the same owned construction step: the `constructing` flag, bootstrap under the constructor's clock, refusal of waiting claims on failure, and close joining it. In that case the capabilities caller is a named constructing owner that holds no record.
3. It never installs an unbootstrapped runtime.

**Closure tests**
- On a fresh Client, an identity-option fetch runs capabilities first and then admits successfully on the same generation.
- Capabilities races a claim's construction.
- A capabilities construction failure leaves the session Open, and a later claim constructs.
- Close runs during capabilities construction.
- `status`, `commit` and a submitted `merge` work before and after network work, and the merge result is read by ID.

**Classification:** bounded contract/text correction. Not a new architectural root cause.

### [P2-2] The 6 s owner clock on `finish()` is shorter than core's own deadline for that step, so a healthy but slow cleanup faults the Client

**Location**
- V4 §7.5, line 272: "registered cleanup and request finish 6 s".
- V4 §3.5, line 73: each clock is "core's own deadline for that step plus a one-second margin, so a healthy supervisor's driven deadline fires first".
- V4 §14, line 392: the clocks "only matter when the supervisor is dead".
- V4 §5.2, line 145, and §11 item 5, line 315: a clock win settles conservatively and sets `faulted`.
- Current source:
  - `request.rs:328–339`: `TransportRequest::finish()` awaits the driver registration's `Session::finish` and only then `local.finish()`.
  - `session.rs:792–808`: each `finish` seals first, so the endpoint registration's CLEANUP window starts only after the driver wait returns.
  - `driver.rs:470–517`: each result is set when the registration is retired and has nothing pending, or when its own 5 s `CLEANUP` elapses (`session.rs:30`).
  - `mux/mod.rs:510–513` and `527–596`: the driver's route retires on the endpoint's terminal or on its own 5 s cleanup deadline.

**Violated invariant:** R3 as §3.5 states it. A clock that loses to core's own bounded completion must not decide the outcome. A healthy supervisor must not trip the backstop.

**State sequence**
1. The placement supervisor runs normally. An accepted operation is cancelled, or its handler fails, mid-stream.
2. The endpoint's SSH worker returns the cancelled stream's terminal after 3 s, so the driver `finish` returns at about 3 s. That is inside its 5 s window.
3. `local.finish()` seals the endpoint registration at about 3 s. The endpoint's physical work for that request stays pending until about 7 s, still inside the endpoint's own window (seal + 5 s = about 8 s).
4. Core alone would return at about 7 s.
5. V4's owner clock fires at 6 s and drops the future. It settles `Failed` or `Cancelled` with `possible` effect and `cleanup unconfirmed`, and sets `faulted`.

A handler that returned `Ok` and left a similar cleanup tail loses its staged `Completed` output and publishes `Failed`.

**Impact**
- A healthy, in-bound cleanup permanently disables all further network admission on the Client, and possibly local commands too (see P2-1).
- A handler success can be reported as failure.
- §3.5 and §14 are false for this step. The design's worst case for core's composite deadline is about 2 × CLEANUP, not one CLEANUP window.

**Required correction:** Derive every owner bound from the sum of core's sequential deadlines inside the awaited future: request finish ≈ 2 × CLEANUP + drive tick + 1 s. State the rule for every composite wait (bootstrap finish, registered cleanup, shutdown). The alternative is a candidate-only finish that seals both registrations at entry; that leaves legacy `TransportRequest::finish` timing unchanged.

**Closure test:** With a live supervisor, use core test hooks to make each of the two sequential registration cleanups take 4 s, inside core's windows.
- Assert the owner's clock does not win.
- Assert `faulted` is not set.
- Assert a successful handler publishes `Completed`.
- Separately, stop the supervisor and assert the backstop fires within the corrected bound.

**Classification:** bounded contract/text correction (calibration). Not a new architectural root cause.

### [P2-3] A panic inside `bootstrap()` leaves `constructing` set forever, so waiting claims and `close()` hang

**Location**
- V4 §7.2, line 258: "`from_environment()` inside `catch_unwind`, then `bootstrap()` under the owner's clock; … a panic also sets `faulted`".
- V4 §3.6, line 77: `constructing` is released only on success, failure or timeout.
- V4 §9, line 301: close waits until `slots == 0 && constructing == false`, "I4 and the owner's clock make that wait finite".
- V4 I4, line 84: only `Attempt`, `Claim` and `AcceptedOperation` guards.
- V4 §5.2, line 138: the Claim-drop row never touches `constructing`.
- V4 §12, line 338: the "Construction panic … released" row.
- Current source contrast: `transport_session.rs:404–443` resets `constructing` explicitly, but only around a `catch_unwind` that covers `from_environment` alone.

**Violated invariant**
- R1 at generation level (§3.6): the constructing owner must leave no unowned state.
- §9: close's wait is finite under "an unwind at any point".
- L2-15: hidden panic paths count.

**Reproduction**
1. Claim A becomes the constructor. Claim B waits on the session condvar and holds a slot.
2. A panic is injected inside `bootstrap()` after the Bind is sent. `bootstrap()` is outside `catch_unwind` per the text.
3. The unwind leaves A's frame. A's `Claim` drop guard settles A and sets `faulted`.
4. Nothing specified clears `constructing` or notifies waiters, so B waits forever.
5. `close()` marks B close-owed and waits for `slots == 0 && constructing == false` forever.

The same applies to an unwind during the failure-path `shutdown()`.

**Impact:** `close()` and every waiting admission hang permanently, and the process must be killed. The §12 "Construction panic → released" row cannot hold for this panic site.

**Required correction:** Make construction one owned value, a construction guard created when `constructing` is set. It covers `from_environment`, `bootstrap` and the failure shutdown. Its Drop on unwind must:
- clear `constructing`;
- set `faulted`;
- close the partial runtime's sessions synchronously;
- notify the session condvar, so waiting claims refuse `NotRegistered` and close proceeds.

**Closure test:** Inject a panic inside `bootstrap()` after Bind, with a second claim waiting and a concurrent `close()`. The waiter refuses `NotRegistered`, `faulted` is set, and close returns a conservative report within the bound.

**Classification:** bounded contract/text correction. Not a new architectural root cause.

### [P2-4] Core and guard closes of a generation leave the session Open and unfaulted, so every later refusal looks retryable

**Location**
- V4 §14, line 389: "`faulted` makes that explicit".
- V4 §3.6, line 77: `faulted` is set only on owner clock wins during finish or shutdown. Core-internal closes "reach owners as errors".
- V4 §5.2, lines 135–136, and §12, lines 340–344: `NotRegistered IoError`, retry "no, only in a later generation" or "yes on a later generation".
- Caller guide, line 53: `False` "permits retry … only after the refusal's cause clears and the generation remains open".
- Current source, all retained by `admit_local`:
  - `session.rs:244–250`: guard close;
  - `session.rs:663–672`: `install_capacity` closes internally on retirement or authority failure;
  - `session.rs:478–480`: a closed session refuses with IoError;
  - `driver.rs:250–257` and `498–501`: core-internal closes.

**Violated invariant**
- Truthful recovery facts: a caller must be able to tell "retry later" from "this Client can no longer admit network work".
- §14's own statement.

**Reproduction**
1. Force a capacity change whose retirement misses the 5 s deadline. Core closes the endpoint session and returns `IoError`. V4 settles `Refused/NotRegistered`, `request_id_consumed=false`, and sets no `faulted`.
2. The caller follows the guide, releases the handle, and admits a new one. `reserve`, `begin_attempt` and `claim` succeed because the session is Open and unfaulted.
3. Phase 1 refuses with the same typed `IoError` and `consumed=false`. This repeats forever.

The same happens after a phase-1 cancel or clock drop after pool mutation (guard close), after a CLEANUP-expiry close, and after a mux protocol disconnect.

**Impact:** The Client is silently dead for network work, and its typed facts are indistinguishable from the retryable pre-mutation timeout. The guide's condition "the generation remains open" cannot be observed, so callers retry forever or never learn to recreate the Client. This is a diagnosability and recovery defect.

**Required correction:** Any owner that causes or observes a closed generation must set `faulted` under the session mutex before settling, or refuse with a typed generation-closed disposition. That covers:
- a phase-1 refusal because the endpoint session is closed or the driver mux is not Ready;
- a guard close;
- an internal retirement or authority close.

Update the §12 retry column and the guide to "no — close and recreate the Client, or await rollover".

**Closure test:** Hold retirement past the deadline. Assert `faulted` is set, and that the next `begin_attempt` or `claim` refuses typed-closed rather than `IoError` with `consumed=false`. Repeat for a post-mutation cancel and for a driver-mux close.

**Classification:** bounded contract/text correction. Not a new architectural root cause.

### [P3-1] The reserved bootstrap ID is a valid caller request ID

**Location:** V4 §6.1, line 179 ("never a caller request ID"); §11 item 6, line 316; §14, line 390 (the promised remedy is "a distinct internal namespace, not a caller-visible change"); the core v4 paragraphs. Identifier grammars: core `request.rs:390–392`, mux `mod.rs:650–652` and v2 §2 line 13 are identical.

**Sequence:** `start_fetch(request_id="bootstrap-1")` → `accepted()` → the phase-1 duplicate pre-check finds the ID in the used sets written at construction → `Refused/NotRegistered InvalidRequest`, `request_id_consumed=false`. The retry repeats forever in that generation.

**Impact:** An undocumented caller-visible reservation, with a retry fact whose cause never clears. §14's remedy cannot be met, because no mux or core registration ID exists outside the caller grammar.

**Correction:** Document and refuse the reserved form synchronously at `reserve` as an explicit v2 amendment, or keep the bootstrap ID out of the caller-visible used sets.

**Test:** `reserve` with the reserved ID fails synchronously with a typed error, or is admitted.

### [P3-2] The `Progress` interface sketch cannot see writes made inside `register_and_open`

**Location:** V4 §6.3, lines 210–214 (`fn progress(&self) -> Progress; // NotEntered | MayHaveRegistered | Registered` and `fn register_and_open(self)`); §7.2, line 258 ("`admission.progress()` attached to the `Claim` first"); the GWZDesign v4 paragraph.

**Sequence:** Implemented as sketched, the Claim stores a `NotEntered` snapshot. `register_and_open(self)` moves the Admission, writes `MayHaveRegistered`, and the endpoint mux inserts the tombstone. On unwind the Admission is dropped, the Claim reads `NotEntered`, and settles `request_id_consumed=false`. This recreates v3 Safety P2-3 and breaks I5.

**Correction:** Return a shared progress handle that `register_and_open` writes (for example an atomic cell), or take `&mut self` so the Admission survives the catch.

**Test:** §13 item 3's post-insert unwind, driven through the Claim path.

### [P3-3] The per-handle `asyncio.Lock` breaks under documented cross-loop handle use

**Location:** V4 §8, line 295; caller guide, line 101 (handle methods may be awaited on different loops and threads).

**Sequence (Python ≥3.10):**
1. Thread A's loop holds the lock across a native admission.
2. Thread B runs `asyncio.run(h.accepted())`. Its contended acquire binds the lock to loop B.
3. A's `release()` calls `set_result` on loop B's future from thread A. That uses non-threadsafe `call_soon`, which does not wake B's selector.
4. B's `accepted()` never returns. If the lock was bound to A first, B instead gets an untyped `RuntimeError`.

The native state machine is unaffected.

**Correction:** Use a loop-agnostic in-flight future (a `threading.Lock` plus a `concurrent.futures.Future`, awaited with `wrap_future`), or rely on `begin_attempt`'s in-progress refusal plus a native wait.

**Test:** Two threads and loops call `accepted()` concurrently on one handle while the admission is held. Both get the same outcome within the bound.

### [P3-4] The committed retry example leaks handles on task cancellation, contrary to its own claim

**Location:** caller guide, lines 55–79; line 95 (`GwzOperationCancelled` subclasses `asyncio.CancelledError`, and the handle "may still … release"); line 38.

**Sequence:** Cancel the task during either `await handle.accepted()` (lines 59 and 67). `GwzOperationCancelled` bypasses `except GwzBridgeError` and exits before the `try/finally` at line 71, so neither handle is released. That contradicts line 79 ("the retried handle whether it is … cancelled"). The `finally` comment at lines 74–75 also relies on an undocumented rule that `result()` joins the operation on cancellation. If it does not, `release()` raises `OpenOperation` (line 38) and replaces the `CancelledError`.

**Impact:** A record leaks per cancelled admission, toward `TransportSessionFull`, and `asyncio.timeout` and TaskGroup cancellation can be masked.

**Correction:** Put each handle's whole lifecycle in `try/finally` (including `BaseException`), and document or perform cancel-and-join before release.

**Test:** Walk through a cancellation at each await; every handle ends released and `CancelledError` propagates.

### [P3-5] A v4 core requirement forbids expiry closes that core keeps and V4 relies on

**Location:** `GWZRequirements.md` line 11: "Core MUST NOT own any timer, callback or thread with authority to … close a generation on expiry; every core wait MUST be bounded by its caller's own clock". Retained behavior it contradicts:
- `driver.rs:498–501`: close on CLEANUP expiry;
- `driver.rs:250–257` with `mux/mod.rs:527–541`: bootstrap and cleanup deadline disconnects;
- V4 §6.1, line 179, relies on "the mux's own 5-second deadline";
- legacy `request()` waits (`mod.rs:185–237`) have no caller clock.

**Impact:** The requirement can only be met by removing fail-closed expiry closes that V4 depends on. Otherwise it is silently false in the baseline.

**Correction:** Scope it to "gains no new timer or callback with authority over V4 records; existing core and mux expiry closes remain faults observed by owners; every core wait awaited by a V4 native owner is owner-clocked".

**Test:** A documentation check against those source sites.

## 2. Invariant analysis

These attacks failed. They are what the next GO will rest on.

**R1 and R2 at the Python handoff (§§3.2–3.3, §8)**
- T1, T2, abandon, cancel and close are single transitions under the session mutex, so exactly one wins and the late party reads the terminal.
- Default-executor shutdown raises from `run_in_executor` inside the worker task. `abandon_attempt` then settles `Attempting` as `IoError/NotRegistered`.
- An encoding failure settles the same way.
- The only await is `shield(worker)`. Cancellation there takes the cancel path, which either settles `Attempting` or leaves the decision to the native owner. `_await_completion` absorbs repeated cancellation.
- Exceptions raised after native completion hit a no-op abandon, and the error matches the ledger.
- On event-loop closure (GeneratorExit), abandon settles a still-`Attempting` record, and a late claim reads it. The only cost is noisy exception reporting.

**T3**
- A failed `send` returns the value, and `abandon()` settles.
- A buffered value dropped with the channel runs the guard's Drop.
- An unwind before `send` settles through the Claim or AcceptedOperation guard, and the parked worker exits idle.
- `accept` records a pending cancel under the mutex and signals after release. The handle is idempotent (`active.swap`).

**R3 and the owner's clock**
- The timer is driven by the session executor's own worker thread, independent of the supervisor thread.
- The HTTPS endpoint runs on its own runtime thread (`https_endpoint.rs:74–99`), so transport tasks never occupy that worker.
- Dropping each awaited core future is fail-closed:
  - `admit_local`: the leader guards release, and `CapacityMutation` closes the generation after mutation;
  - `bootstrap`: the guard cancels the offer's request, which disconnects the mux while Binding;
  - `finish` and cleanup: `TransportRequest` and `ClientRequest` Drop cancel and seal, and core retires in the background;
  - `shutdown`: the sessions were already closed synchronously.
- A timer and a completion meet only in the owner's select. The v3 watchdog-versus-Ready race is gone.

**Phase 2**
- Every step is synchronous in current source: the two `Session::register` calls, `begin` on a Ready mux (which returns without enqueueing), and backend attachment.
- The mux inserts a tombstone only after every refusal check (`mod.rs:242–269`), so `Refused` does prove no insertion.
- `MayHaveRegistered` written before the mux call is conservative on unwind, subject to the handle shape in P3-2.

**Truthfulness of recovery facts**
- `effect="none"` appears only before `Accepted`.
- Git work happens only after the gate opens.
- `Completed` comes only after a returned `finish()`.
- `consumed=false` is reported only for a never-claimed record or a proved no-insert.
- A T3 failure reports `possible` conservatively.

**Bootstrap**
- A Bind requires a live endpoint registration (`routing.rs:110`).
- Cancel-after-Ready ordering avoids the offer disconnect.
- After Ready, legacy `begin` and `ready` return immediately.
- Concurrent claim constructors are serialized. Two exceptions: the ID-less path (P2-1) and unwind (P2-3).
- Retry after a failed construction starts from a fresh runtime.

**Isolation and blast radius**
- Legacy commands build their own runtime (`local_command.rs:12–32`), and `request()` is unchanged.
- V4 adds no new secret-bearing fields.
- Close summaries stay compact.
- Stuck states are bounded under single faults, except for P2-3 and the residual items in §3.

## 3. Risks and next action

These residual risks are below the finding bar.

**Owner-clock limits**
- A supervisor that hangs inside `drive()` while holding core's `Session.state` mutex defeats every owner clock. Phase 2, `TransportCancellation::cancel`, drop-guard seals and every core poll block synchronously on that mutex. §3.5's "dead or hung" wording should narrow to a dead supervisor, which the hardening covers.
- A stalled executor worker combined with a dead supervisor leaves `finish` unbounded.
- `from_environment()` is still unclocked (pre-existing).

**Admission and the Python bridge**
- A cancel that arrives during the construction wait should be checked as a level (a flag read), not an edge (a wake), at phase-1 entry; otherwise it can consume the ID needlessly.
- An `Attempting` record whose loop stopped before the worker started can only be recovered by cancel or close. Release refuses it with `OpenOperation`.
- The `except BaseException` in `_admit` rewrites GeneratorExit, KeyboardInterrupt and SystemExit into a bridge error.
- A V4 bridge paired with an older native module has no feature detection for `begin_attempt`, and `submit`'s `except AttributeError` fallback would re-route the call.
- The bridge's 2-worker control executor can queue close behind blocked cancels (pre-existing).
- Implicit-form refusals are retained until the deferred expiry. That conflicts with §13 item 1's "no record-limit growth".

**Text consistency and legacy scope**
- The §3.4 table marks `Issued`/`Attempting` close-owed, while §9 bounds close-owed records to at most eight.
- The supervisor hardening, and the +1 registration cap if applied globally, change legacy/CLI behavior despite the "`request()` unchanged" wording.

**Next action:** Revise V4 and its paired core and caller-guide text so that P2-1 to P2-4 are resolved as specified, fold in the P3 corrections, commit a new exact tuple, and run a fresh peer-blind Consistency and Safety re-review. No implementation authority follows from this verdict.
