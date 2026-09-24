# Python transport session v4 foundation — operation ownership protocol

Date: 2026-09-24. Status: **DRAFT v4 foundation design (a transition design under AgentProcessRules §7.1), first remediation; focused re-review by the same Consistency, Safety and Surface reviewers is required before implementation restarts; this document carries no implementation, build, or activation authority.** The [first design verdict](GwzPyTransportSessionV4FoundationDesign-Verdict.md) returned NO-GO on root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`, gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`, gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`, with no new architectural root cause. This revision applies its [remediation plan](GwzPyTransportSessionV4FoundationDesign-RemPlan.md) as one patch. V4 replaces the stopped [v3 foundation draft](GwzPyTransportSessionV3FoundationDesign.md), whose [first remediation verdict](GwzPyTransportSessionV3FoundationDesign-Verdict-1.md) reached the program's third-new-root-cause stop. The dirty root checkpoint file and the untracked SSH N2b prompt and route-mapping drafts are unrelated and out of scope.

## 1. Decision, boundary and precedence

The operator's direction after the v3 stop is that the repeated failures are a lifecycle **ownership** problem, not a transport-protocol problem. Python issues an operation ID and schedules native work; the native session admits; core registers transport requests; a worker runs Git; cancellation, close, timeouts and faults intervene throughout. No single contract said who owned an operation in every interval, so each review found one more interval with no owner or with two deciders. Correcting intervals one at a time added a guard, a witness and a watchdog whose interactions produced the next interval. V4 therefore states one ownership protocol first and derives every mechanism from three rules:

- **R1 Immediate ownership.** Every issued operation has exactly one owner at every instant, including before any executor or thread has accepted work for it.
- **R2 Atomic transfer.** Every change of owner is one atomic decision. Either the receiver owns the operation from that decision on, or the sender still owns it and remains responsible for recording its failure.
- **R3 One arbiter.** Timeout, cancellation, completion and fault compete only through the operation's own state machine, decided by its current owner. A timeout that loses to a completed transfer has no authority over the result.

Under R2, a receiver that refuses a transfer records that refusal on the sender's behalf in the same atomic step. The sender's responsibility is discharged exactly once, and the sender only reads the retained disposition (§3.3).

The accepted [v2 contract](GwzPyTransportSessionV2Design.md) remains the caller-facing contract for identity, admission meaning, capacity, outcome ledger, close and typed terminals; §11 lists every place V4 amends or clarifies it. V4 replaces the foundation architecture beneath it: the native session in `gwz-py/native/src/transport_session.rs`, `operations.rs` and `dispatch/mod.rs`, the bridge admission path in `gwz-py/src/gwz/bridge.py`, and a candidate-only local admission API in `gwz-core/src/transport_host/{mod,session,request}.rs`. These are internal Python/native and core host API changes. The Taut request/response method set, the `gwz-transport` envelope, the virtual-stream delivery protocol and the appended `cancelled=73` and `transport_record_limit=74` codes are unchanged: application operations have their own lifecycle here, while transport messages continue to flow independently and Python forwards them regardless of application activity. Existing core `TransportRuntime::request()` keeps its implementation and timing for CLI and local commands.

For the native Python session, V4 also supersedes frozen core-API text in the accepted [Python transport design](../gwz-py/dev-docs/GwzPyTransportDesign.md) §§2–3: the per-call `runtime.request(meta, operation_id)` flow, and the statement that no extra registration is needed. The session uses `bootstrap_ready`, `admit_local` and `register_and_open` instead, and each generation carries one reserved internal registration (§6). That document carries a matching pointer. Its §4 rule that the capability preflight and dispatch use the same runtime generation is kept (§7.6).

V4 withdraws two v3 mechanisms: the core-side wall-clock watchdog with its closing callback, and the asynchronous phase-2 wait with its "not cancellable" clause. The v3 registration witness survives only as a progress handle that the admitting owner's own thread writes and reads (§6.3); nothing reads it across threads. V4 retains from v3 the single record, the `Claim` and `AcceptedOperation` ownership values, session-then-record locking, private staged output with a single terminal writer, close-owed summaries, legacy-path isolation and the local-only two-phase core admission. The deferred stages (byte ledger, timed expiry, public `start_*` handles and `OperationStream`, generation rollover, transport stress, platform gates) stay deferred and attach at the points named in §7.1.

Core requirements and design must be amended before core behavior changes (`gwz-core/AGENTS.md`). This tuple replaces the withdrawn v3 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` with v4 paragraphs marked pending this review.

## 2. Why v3 stopped, in ownership terms

**Before native entry.** Python issued an ID, then scheduled the native call with `asyncio.to_thread`. If the executor was shut down or request encoding failed, the caller received an error while the record stayed `Issued`. The only settlement owner v3 defined, the native `Claim`, is created at native entry, so it never existed. In R1 terms the operation had no owner between Python's decision to admit and native entry.

**At admission completion.** v3 bounded the phase-2 wait with a core watchdog whose callback closed the generation on expiry. That callback was a second decider: it could fire after phase 2 returned `Ready` and the worker had started. In R3 terms two parties had authority over one outcome.

**The same pattern earlier.** The v2 foundation's rounds each closed one interval and found another (eight-slot refusal, spawn failure, deferred `submit` failure, current-directory failure, registration inferred from error codes). v3's first round found phase 2 droppable by a wrapper, a witness not atomic with the mux tombstone, a handler panic settling before finish, and release erasing a close summary. Every one is an interval with no owner or a second decider.

**What V4 does instead.** It defines an owner for every interval (§3.2), makes each transfer one atomic decision (§3.3), routes every intervener through the owner's state machine (§3.4), and removes timers as actors by giving each owner its own clock (§3.5). Two consequences make the machinery smaller rather than larger. First, mux bootstrap moves from the first operation's admission into generation construction, so per-operation admission has no asynchronous step after capacity; phase 2 becomes synchronous and needs no watchdog. Second, the Python side becomes an explicit owner with an atomic handoff to native, so the pre-entry gap is covered by the same state machine as everything else.

Disposition of the [v3 stop verdict](GwzPyTransportSessionV3FoundationDesign-Verdict-1.md) findings: Safety P2-1 (unowned pre-entry interval) is §3.2, §5 and §8; Safety P2-2 (watchdog versus `Ready`) is §3.5 and §6, which delete the watchdog; Consistency R2-C-P2-1 (finish-unwind exception missing from the supersession list) is §11 item 5; Consistency R2-C-P2-2 (retry promised after a generation-closing timeout) is the split rows in §12; Surface P3-2 (retry example lifecycle) is the rewritten example in the caller guide, which now also covers task cancellation. The first V4 review's findings are mapped in its remediation plan and applied here.

## 3. The ownership protocol

### 3.1 Parties and resources

Owners are the parties that can decide an operation's next transition: the **ledger** (the native session's record store, passive), a **Python attempt** (the coroutine executing one admission attempt), a **native admitting thread** (holding a `Claim`), and an **accepted owner** (whoever holds the `AcceptedOperation` value: the worker thread for `submit`, the calling thread for `call`). At generation level, the **constructing owner** holds the `Construction` value (§3.6). Core is not an owner. Core resources (`Admission`, `BootstrapLease`, `TransportRequest`, `RegisteredCleanup`, capacity leadership) are Rust values held by the current native owner, and core's placement supervisor is a service whose failures reach owners as errors or as expired owner clocks. Interveners (cancel, close, release, timeout, fault) are not owners either; they are intents (§3.4) or signals (§3.5).

### 3.2 The owner ladder

| Phase | Owner | Proof of ownership | How ownership leaves |
| --- | --- | --- | --- |
| `Issued` | the ledger (passive) | record phase | T1 `begin_attempt` to a Python attempt, or T2 `claim` directly for a direct native caller (a refused T1 or T2 settles it on the caller's behalf); or an intent settles it (cancel, close, release, expiry) |
| `Attempting` | the Python attempt | the `Attempt` scope in the bridge (`try`/`except`) | T2 `claim` to native (a refused `claim` settles it on the attempt's behalf); or `abandon_attempt`, cancel or close settles it |
| `Admitting` | the native admitting thread | the `Claim` value | `accept` converts the `Claim` into an `AcceptedOperation` on the same thread; `refuse` or the guard's drop settles it |
| `Accepted`, `Finishing` | the holder of the `AcceptedOperation` value | that value (exactly one holder by Rust ownership) | T3 gate send moves the value to the worker thread (`submit` only); T4 `settle` returns ownership to the ledger; the value's drop settles it |
| `Terminal` | the ledger | the immutable terminal | release or expiry discards the record |

There is no phase without a row, and no row with two owners. Local submitted operations (§7.6) enter at `Admitting`: the native submit thread mints them and holds their `Claim`, so they have no `Issued` or `Attempting` interval.

### 3.3 The transfers

- **T1 `begin_attempt(id)`**: one transition `Issued → Attempting` under the session mutex. It fails with the retained terminal if the record is already terminal, and with `InvalidRequest` if an attempt or admission is already in progress. If the session is `faulted`, it is a refused transfer (below). From success on, the Python attempt owns the record and is responsible for reaching T2 or recording its failure.
- **T2 `claim(id)`**: one transition `Attempting → Admitting` (or `Issued → Admitting` for a direct native caller, which has no pre-entry interval) under the session mutex, performed as the first statement of native `call`/`submit`. It takes the top-level slot and the ledger upgrade together (§7.1). If the record is no longer `Attempting` (the attempt abandoned it, or cancel or close settled it), `claim` returns the retained terminal and the native call owns nothing. Correspondingly, the attempt's `abandon_attempt` settles only a record that is still `Attempting` and is a no-op otherwise, returning what the ledger holds. Whichever side transitions first wins; the other reads.
- **T3 gate send** (`submit` only): the `AcceptedOperation` value is sent through a one-shot channel to a worker thread that was spawned before acceptance and does nothing but receive and run. A failed send returns the value to the sender, which therefore still owns it and settles it (`abandon()`); a successful send makes the receiver the owner. Rust ownership makes this atomic without a flag.
- **T4 `settle`**: one transition to `Terminal` under the session mutex by the current owner, writing the terminal once.
- **Refused transfers.** When the receiving side refuses T1 or T2 (the session is `faulted`, no top-level slot is free, or the ledger upgrade does not fit), the refusing transition settles the record on the sender's behalf with the typed refusal, in the same atomic step under the session mutex. The sender owns nothing further. Its `abandon_attempt`, if it runs, is a no-op that returns this disposition, so the refusal keeps its typed code.

Every other change of state is either an owner's own step (checkpoint, construction, registration, Git) or an intent.

### 3.4 Intents

An intent is a request from a non-owner that the state machine applies atomically under the session mutex according to the current phase. The intent either settles a record whose owner is passive, or is recorded for the active owner to decide.

| Intent | `Issued` / `Attempting` | `Admitting` | `Accepted` / `Finishing` | `Terminal` |
| --- | --- | --- | --- | --- |
| cancel | settle `Cancelled(NotRegistered)`; the passive owner learns at its next transition (a late `claim` reads the terminal, a late `abandon_attempt` is a no-op) | set `cancel_requested`, wake the owner's phase-1 select and any construction wait; the owner decides at its checkpoints | signal the stored `TransportCancellation` after locks release; the owner decides at finish | read the retained cleanup |
| close | as cancel, plus refuse new `reserve`, `begin_attempt` and `claim` | as cancel; mark close-owed | as cancel; mark close-owed | read |
| release | `Issued`: discard; `Attempting`: `OpenOperation` | `OpenOperation` | `OpenOperation` | discard; a repeat reports `OperationExpired` |
| expiry (deferred timer stage) | `Issued`: discard after 15 minutes; `Attempting`: not applicable (owner is active) | not applicable (owner is active) | not applicable | discard after 15 minutes |

Timeouts are absent from the table because they are not intents (§3.5). Faults are handled by the owner that suffers them: a Python exception by the attempt's `except` clauses, a native unwind by the `Construction`, `Claim` or `AcceptedOperation` drop guard, a core error by the owner that received it. A local submitted operation cannot be cancelled: cancel returns `UnsupportedOperation` without effect, and close neither cancels nor marks it (§7.6).

### 3.5 Timers are signals, not actors

**An owner never waits on core without its own clock, and core gains no new actor.** Every core future awaited by a native owner (bootstrap readiness and its lease's finish, capacity admission, registered cleanup, request finish, runtime shutdown) is awaited under a timeout on the native session's own executor, whose timer is driven by that executor's worker thread and is independent of core's placement supervisor. When the core future completes first, the timer future is dropped and can never act. When the clock wins, the owner drops the core future, whose Drop guards are fail-closed (release leadership, seal registrations, close a partially mutated generation), and the owner settles conservatively with unconfirmed cleanup.

Each owner clock is the sum of the core deadlines that run in sequence inside the awaited future, plus a one-second margin (§7.5). A live supervisor's driven deadlines therefore always fire first, and an owner clock wins only when the supervisor is dead. A supervisor that hangs inside `drive()` while holding core's session mutex would also block core's synchronous calls, so no clock can bound it; that case is outside the stated failure model (§14).

Core keeps its existing supervisor-driven deadlines: the mux bootstrap and route deadlines, and the 5-second cleanup-expiry close of a sealed registration that has not retired. They fire before any owner clock, may close a generation, and reach owners as errors; they never write a terminal or decide an operation's outcome (§3.6). Core gains no new timer, callback or thread with authority over native records. This is the whole replacement for v3's watchdog, and R3 holds by construction: the only place a timeout and a completion meet is a `select` on the owner's own thread.

### 3.6 Generation ownership

Generation-level decisions follow the same rules. Constructing a generation is an owned step held by a `Construction` value. The first party that needs a runtime and finds none — an admitting claim, or the capability preflight (§7.6) — creates that value when it sets `constructing`, builds the runtime and drives its bootstrap under its clocks; other parties wait on the session condvar. The value is held across `from_environment_for_session()`, `bootstrap_ready()`, the bootstrap lease's `finish()` and, on failure, `shutdown()`. Success installs the runtime as a session field. Failure or a clock win shuts the runtime down under its clock, records the cleanup facts for the close report, discards the runtime, refuses the constructing party and every waiting claim with a `NotRegistered` refusal, and leaves the session `Open` so a later attempt may construct again. If the shutdown's own clock also wins, the session is marked `faulted`. If the constructing owner unwinds, the `Construction` value's `Drop` clears `constructing`, sets `faulted`, closes the partial runtime's sessions synchronously and notifies the session condvar, so waiting claims refuse and close proceeds.

An installed generation can close in four ways: `close()`; an owner's fail-closed transition (a phase-1 future dropped after pool mutation runs the existing `CapacityMutation` guard, and an unwind inside phase 2, or in an admitting `Claim` whose progress handle is not `NotEntered`, closes it deliberately); core's internal closes (a retirement or authority failure inside capacity installation, a protocol error, a mux bootstrap or route deadline, the cleanup-expiry close of any registration, and the supervisor-exit hardening in §6.3); and nothing else. Every close other than `close()` is a fault.

**Generation fault rule.** After any core call returns an error, and after any clock win, the owner reads `TransportRuntime::generation_open()`. If the installed generation has closed, the owner sets `faulted` under the session mutex before settling its own record. A phase-1 refusal typed `GenerationClosed` (§6.2) is the same observation. From then on, network `reserve`, `begin_attempt`, `claim` and the capability preflight refuse with `TransportGenerationBusy` and `request_id_consumed=false`, and the caller's action is to close and recreate the Client; the deferred rollover stage will later make this transient. Local unary commands and local submitted operations are unaffected (§7.6). No core close writes a terminal or decides an outcome.

## 4. Invariants

- **I1 One owner.** Every record has exactly one owner at every instant per §3.2; every change of owner is one of T1–T4, and a refused T1 or T2 settles on the sender's behalf in the same atomic step.
- **I2 One record.** Every issued operation ID maps to exactly one session-owned `OperationRecord` holding all of that operation's state. No other map holds per-operation state.
- **I3 One terminal, one writer.** A record's terminal is written at most once, by `settle`, by the record's owner at that moment or by a refusing transfer on the sender's behalf. Later attempts are ignored and, in test builds, counted as violations.
- **I4 Every owner settles.** A `Construction`, a `Claim` and an `AcceptedOperation` cannot be dropped without settling what they own; a Python attempt cannot exit without either transferring at T2 or calling `abandon_attempt`.
- **I5 Provenance is typed.** Whether core registered the caller request ID is carried by which value core returned — a phase-1 refusal (`Refused`, `AlreadyRegistered` or `GenerationClosed`) or a phase-2 result (`Refused`, `Consumed`, `Ready`) — and, on unwind inside the synchronous registration step, by the owner's progress handle. No code path inspects `ModelError.code` or text to decide it, and Python keeps no consumed-ID allowlist.
- **I6 No Git before Accepted.** `Accepted` is written only after core returned `Ready` on an already bootstrapped generation and the worker slot is held; for `submit`, the worker exists and is parked at its gate, and the gate message is the only way it obtains the `TransportRequest`.
- **I7 Success only after finish.** Handler output is staged privately; `settle(Completed)` follows a returned `finish()`. A caught handler failure also awaits `finish()` before its terminal. Only an unwind inside `finish()`, a lost accepted owner or a clock win during finish uses the conservative fallback with unconfirmed cleanup.
- **I8 Only close writes Closing and Closed.** Faults set `faulted`, which refuses network issuance, attempts, claims and capability preflights with `TransportGenerationBusy`; local unary commands and local submitted operations continue until Closing. Close still settles every remaining record.
- **I9 Intents never act outside the state machine.** Cancel, close and release are applied under the session mutex by phase; timeouts are consumed only by the waiting owner's select.
- **I10 Path isolation.** The legacy module-level path (`dispatch::submit`, `submit_accepted`, `spawn_call`, process-global `STORE`, `op_<request_id>`) and the session path share no recorder, store, worker wrapper or thread-local session hook. Every operation submitted through a native-session bridge, network or local, is a session record, and no lookup through that bridge reaches the process-global store.

## 5. Operation state machine

### 5.1 Phases and terminal kinds

| Phase | Meaning |
| --- | --- |
| `Issued` | ID minted synchronously; live request-ID claim; 4 KiB reservation (deferred ledger); no slot, endpoint or core work. |
| `Attempting` | A Python attempt owns the record; encoding and scheduling of the native call are in progress; no slot, endpoint or core work. |
| `Admitting` | A `Claim` owns the record. A network claim also holds one of the eight top-level slots and the upgraded ledger allowance; decoding, placement, construction, capacity and registration run on the claiming thread. A local claim holds neither slot nor registration. |
| `Accepted` | A network operation is registered and opened on a ready generation, with its generation and cancellation handle recorded, and its accepted owner holds the `TransportRequest`. A local operation holds no transport resource. Git may run. |
| `Finishing` | A network operation's handler returned or was caught; `finish()` is in progress under the owner's clock. |
| `Terminal(kind)` | Immutable outcome plus `CleanupReport`; readable until release or expiry; slot and claim released. |

| Terminal kind | Provenance | Effect | `request_id_consumed` | Retained payload |
| --- | --- | --- | --- | --- |
| `Refused(error)` | `NotRegistered` | none | false | typed `ModelError` |
| `Refused(InvalidRequest)` for a duplicate | `AlreadyRegistered` | none | true | typed `ModelError` |
| `Refused(error)` | `Registered` | none | true | typed `ModelError` and cleanup report |
| `Refused(InternalError)` after an unwind inside registration or in an admitting `Claim` | from the progress handle: `MayHaveRegistered`, `Registered` or `NotRegistered` | none | true unless the handle still reads `NotEntered` | typed `InternalError`, unconfirmed cleanup; the generation is closed after any unwind inside phase 2 (A11) and after a `Claim` unwind whose handle is not `NotEntered` (A14); the session is `faulted` in every case |
| `Cancelled` before acceptance | `NotRegistered` or `Registered` | none | per provenance | typed `Cancelled` refusal; no `OperationResult` |
| `Cancelled` after acceptance | `Registered` | possible | true | failed `OperationResult` with `GwzErrorCode.cancelled=73` |
| `Failed` | `Registered` (or local) | possible | true (false for local) | failed `OperationResult` |
| `Completed` | `Registered` (or local) | n/a | true (false for local) | successful `OperationResult`, optional `MergeResponse` |

`request_id_consumed` states whether this request ID is registered in the current generation, or that cannot be ruled out. It is `true` for `Registered`, `MayHaveRegistered` and `AlreadyRegistered` provenance and `false` only for `NotRegistered`. Local operations never register with core, so theirs is `false`.

`Refused` and pre-acceptance `Cancelled` project to Python as the v2 typed `GwzBridgeError` with `effect="none"`; `Failed` and post-acceptance `Cancelled` project as `GwzOperationError` with `effect="possible"`; `Completed` returns the result. When the outcome reaches a caller whose awaiting task was cancelled, it is raised as `GwzOperationCancelled` instead, with the same `effect`. Every projection carries `request_id_consumed`. `GwzOperationCancelled` subclasses `asyncio.CancelledError` and is not a `GwzBridgeError`.

### 5.2 Legal transitions

Rows are identified for citation by §12. "Generation check" in an owner action is the §3.6 fault rule: the owner reads `generation_open()` and sets `faulted` before settling if the installed generation has closed.

| Row | From | Event | Condition | To | Owner action |
| --- | --- | --- | --- | --- | --- |
| S1 | — | `reserve(request_id)` | grammar valid; session `Open`, not `faulted`; no live claim for `request_id`; under 64 issued-unreleased records | `Issued` | mint serial; insert record and claim; 4 KiB reservation |
| S2 | — | `reserve(request_id)` | invalid grammar; a live claim; 64 records; `Closing`/`Closed`; or `faulted` | no record | raise synchronously `InvalidRequest`, `InvalidRequest`, `TransportSessionFull`, `InvalidRequest` (closed) or `TransportGenerationBusy` respectively |
| S3 | `Issued` | T1 `begin_attempt(id)` | session `Open`, not `faulted` | `Attempting` | none (the attempt now owns) |
| S4 | `Issued` | `begin_attempt(id)` | session `faulted` | `Terminal(Refused, NotRegistered, TransportGenerationBusy)` | refused transfer settles on the caller's behalf |
| S5 | `Issued`, `Attempting` | T2 `claim(id)` | a top-level slot is free and the ledger upgrade fits; session `Open`, not `faulted` | `Admitting` | take slot and ledger upgrade atomically; construct `Claim` |
| S6 | `Issued`, `Attempting` | `claim(id)` | no slot is free, or the ledger upgrade does not fit | `Terminal(Refused, NotRegistered, TransportSessionFull)` | refused transfer settles on the sender's behalf; return the typed error |
| S7 | `Issued`, `Attempting` | `claim(id)` | session `faulted` | `Terminal(Refused, NotRegistered, TransportGenerationBusy)` | refused transfer settles on the sender's behalf |
| S8 | `Attempting` | `abandon_attempt(id, error)` | — | `Terminal(Refused, NotRegistered, error)` | settle; return the retained disposition |
| S9 | any other phase | `abandon_attempt(id, _)` | — | unchanged | no-op; return the retained disposition or phase |
| S10 | `Issued`, `Attempting` | cancel or close intent | — | `Terminal(Cancelled, NotRegistered)` | settle with default cleanup |
| S11 | `Issued` | release or expiry | — | discarded | remove claim; high-water mark unchanged |
| A1 | `Admitting` | `Claim::refuse(error)` before core admission: `current_dir()`, decode, message names, a non-network method with an issued ID, `validate_request_context`, or explicit CLI placement | — | `Terminal(Refused, NotRegistered, typed error)` | settle; release slot, ledger upgrade and claim |
| A2 | `Admitting` | checkpoint 1: `cancel_requested`, read as a level after any construction wait (a waiting claim is woken by its cancel) | — | `Terminal(Cancelled, NotRegistered)` | settle |
| A3 | `Admitting` | construction fails, a construction clock wins, or the constructing owner unwinds (this claim constructing or waiting) | — | `Terminal(Refused, NotRegistered, IoError or InternalError)` | settle; the `Construction` value handles the runtime (§3.6) |
| A4 | `Admitting` | phase 1 returns `Refused(error)` | — | `Terminal(Refused, NotRegistered, typed error)` | settle |
| A5 | `Admitting` | phase 1 returns `AlreadyRegistered` | — | `Terminal(Refused, AlreadyRegistered, InvalidRequest)` | settle |
| A6 | `Admitting` | phase 1 returns `GenerationClosed` | — | `Terminal(Refused, NotRegistered, TransportGenerationBusy)` | set `faulted`; settle |
| A7 | `Admitting` | cancel signal while phase 1 is pending | the owner's select drops the future; nothing registered | `Terminal(Cancelled, NotRegistered)` | generation check; settle |
| A8 | `Admitting` | owner clock wins while phase 1 is pending | the future is dropped; nothing registered | `Terminal(Refused, NotRegistered, IoError timeout)`, or `TransportGenerationBusy` if the drop closed the generation | generation check; settle |
| A9 | `Admitting` | phase 2 returns `Refused(error)` | proved no insertion | `Terminal(Refused, NotRegistered, typed error)` | generation check; settle |
| A10 | `Admitting` | phase 2 returns `Consumed(error, cleanup)` | — | `Terminal(Refused, Registered, typed error)` | await `cleanup.finish()` under its clock; generation check; settle with its report (unconfirmed if the clock wins) |
| A11 | `Admitting` | `register_and_open` unwinds, caught on the owner's thread | — | `Terminal(Refused, provenance from the progress handle, InternalError)` | close the generation; set `faulted`; settle with unconfirmed cleanup |
| A12 | `Admitting` | checkpoint 2 after `Ready`: `cancel_requested` is set, including a cancel requested during phase 2 | — | `Terminal(Cancelled, Registered)` | `request.cancel()`; await `finish()` under its clock; generation check; settle |
| A13 | `Admitting` | `Ready(request)`, then worker spawn fails | `submit` only | `Terminal(Refused, Registered, IoError)` | `request.cancel()`; await `finish()` under its clock; generation check; settle |
| A14 | `Admitting` | `Claim` dropped by an unwind outside phase 2 | — | `Terminal(Refused, provenance from the progress handle, InternalError)` | seal any held request; close the generation unless the handle reads `NotEntered`; set `faulted`; settle with unconfirmed cleanup |
| A15 | `Admitting` | `Claim::accept(request)` | — | `Accepted` | bind the reserved reader cursor; store generation and handle; signal a pending cancel after releasing locks; for `submit`, T3 send |
| C1 | `Accepted` | T3 send fails (worker gone before `recv`) | the sender still owns the value | `Terminal(Failed, Registered, InternalError)`, effect possible | `abandon()`: cancel, await `finish()` under its clock, generation check, settle; `submit` returns the typed error |
| C2 | `Accepted` | handler returns, or its unwind is caught inside `run` | — | `Finishing` | keep the request; stage only an `Ok` output; call `finish()` under its clock |
| C3 | `Finishing` | `finish()` returned | handler `Ok` | `Terminal(Completed)` | promote staged output; generation check; settle with report |
| C4 | `Finishing` | `finish()` returned | handler not `Ok`; cancellation was signalled | `Terminal(Cancelled, possible)`, code 73 | generation check; settle with report |
| C5 | `Finishing` | `finish()` returned | handler not `Ok`; no cancellation signalled | `Terminal(Failed, possible)` | a caught handler panic also sets `faulted`; generation check; settle with report |
| C6 | `Finishing` | owner clock wins during `finish()` | finish future dropped; staged output discarded | `Terminal(Cancelled if cancellation was signalled, else Failed; possible; cleanup unconfirmed)` | set `faulted`; settle |
| C7 | `Accepted`, `Finishing` | `AcceptedOperation` dropped without settlement (unwind in `finish()`, lost owner) | — | same kind rule as C6 | drop guard settles; seals the request; sets `faulted` |
| C8 | `Accepted`, `Finishing` | cancel or close intent | — | unchanged | signal the handle after locks release; the owner decides at finish |
| T1 | `Terminal` | cancel | — | unchanged | return retained cleanup |
| T2 | `Terminal` | release or expiry | — | discarded | a repeat release reports `OperationExpired` |
| T3 | `Attempting`, `Admitting`, `Accepted`, `Finishing` | release | — | unchanged | refuse `OpenOperation` |
| L1 | — | native `submit` of a local method with no issued ID | session `Open` (`faulted` included); under 64 records; the local ledger allowance fits | `Admitting` (local) | mint serial and record with its allowance; hold a local `Claim` (no slot, no registration) |
| L2 | — | same | 64 records or allowance does not fit; or `Closing`/`Closed` | no record | raise synchronously `TransportSessionFull` or `InvalidRequest` (closed) |
| L3 | `Admitting` (local) | decode or validation fails, or worker spawn fails | — | `Terminal(Refused, NotRegistered, typed error)` | settle |
| L4 | `Admitting` (local) | `accept` | — | `Accepted` (local) | T3 send to the parked local worker |
| L5 | `Accepted` (local) | handler returns or its unwind is caught; or T3 send fails | — | `Terminal(Completed)` or `Terminal(Failed, possible)` | settle; no finish step |
| L6 | `Accepted` (local) | cancel intent | — | unchanged | return `UnsupportedOperation` without effect |

Closing and Closed need no T1 or T2 row: entering Closing settles every `Issued` and `Attempting` record (S10) and refuses `reserve` (S2), so a later T1 or T2 reads that terminal. The C-row kind rule is a single precedence: a handler `Ok` output that survived `finish()` wins; otherwise a signalled cancellation settles `Cancelled`; otherwise `Failed`.

Illegal observations (an `Admitting` record with no live `Claim`, an `Accepted` record with no `AcceptedOperation` holder, a second terminal write, a `TransportRequest` outside a `Claim` or `AcceptedOperation`) are programming errors: test builds assert; release builds fail closed by settling `Failed` and setting `faulted`.

### 5.3 Ownership values

- `Attempt` (Python) — a scope in `_admit` (§8) that begins at T1 and ends at T2 or at `abandon_attempt`. It is not a native object; its proof is the phase `Attempting` plus the bridge's `except` clauses.
- `Construction` (native) — held by the constructing owner from setting `constructing` until the runtime is installed or discarded. Its `Drop` on unwind clears `constructing`, sets `faulted`, closes the partial runtime's sessions synchronously and notifies the session condvar.
- `Claim` (native) — `Drop`-guarded, non-`Clone`; exposes `refuse`, `refuse_registered`, `accept(TransportRequest) -> AcceptedOperation`, `cancel_requested()`, and holds the progress handle for its unwind path. A network claim is the sole holder of the top-level slot and the upgraded ledger allowance before acceptance; a local claim holds neither slot nor registration.
- `AcceptedOperation` (native) — owns the `TransportRequest` (network only), the slot, the record handle and the staging recorder. `run(dispatch)` catches handler unwind, awaits `finish()` under its clock and settles; `abandon()` settles after cancel and finish when a T3 send fails; `Drop` settles by the C6 kind rule with unconfirmed cleanup only if neither ran to settlement.
- `Admission`, `ProgressHandle`, `BootstrapLease`, `RegisteredCleanup`, `TransportRequest` (core) — resources held by the native owner; each has a fail-closed `Drop` (release leadership; nothing; seal the reserved registration; seal registrations; cancel and seal the request).
- `Terminal` — immutable once written; the sole source for `result`, `try_result`, event completion, cancel reports, descriptors and close summaries.

## 6. Generation lifecycle and core admission API

### 6.1 Bootstrap at construction

Today the first caller request bootstraps the mux: its `begin()` sends the Bind offer and its `ready().await` waits for the endpoint's Bound reply, so the first operation's admission contains an asynchronous, generation-level wait that a per-operation owner cannot bound without a second decider. V4 moves bootstrap into generation construction, owned by the `Construction` value (§3.6):

```rust
impl TransportRuntime {
    /// Builds a runtime for the native Python session. Its endpoint and driver
    /// sessions allow one registration beyond the 256 caller registrations,
    /// reserved for bootstrap. Legacy constructors keep exactly 256 and refuse
    /// bootstrap_ready() with UnsupportedOperation.
    pub fn from_environment_for_session() -> ModelResult<Self>;
    /// Registers the reserved bootstrap request on the local endpoint and
    /// driver sessions, sends the Bind offer and awaits Ready. Dropping the
    /// future before Ready disconnects the pending bootstrap (existing mux
    /// behavior) and leaves the runtime unusable.
    pub async fn bootstrap_ready(&self) -> ModelResult<BootstrapLease>;
    /// Synchronous: true while the endpoint and driver sessions are open and
    /// the driver mux is Ready.
    pub fn generation_open(&self) -> bool;
}
impl BootstrapLease {
    /// Cancels, seals and finishes the reserved registration after Ready.
    pub async fn finish(self) -> CleanupReport;
}
```

The reserved registration's ID is `bootstrap-` followed by 32 hexadecimal digits of a 128-bit random token generated at construction. It lies inside the identifier grammar but is exposed nowhere: not in results, events, descriptors, errors or close reports. It is not the session nonce, which appears in operation IDs. A caller can collide with it only by guessing the token, so no caller-visible reservation is added to v2. Its internal operation name is `bootstrap`. Because the mux disconnects when its Binding offer's request is cancelled, the reserved registration is cancelled and sealed only after `Ready`; retiring it then merely tombstones it. The constructing owner awaits `bootstrap_ready()` under a 6-second clock and the lease's `finish()` under an 11-second clock (§7.5). On failure or a clock win the owner drops the future, awaits `shutdown()` under its clock, and discards the runtime (§3.6). Legacy `request()` callers on legacy runtimes keep bootstrapping through their first request exactly as today.

### 6.2 Phase 1: capacity, cancellable, registers nothing

```rust
/// Holds local admission leadership and the installed capacity epoch; nothing
/// is registered. Dropping it releases leadership and consumes nothing.
pub struct Admission { /* leader guard, resolved capacity, progress handle */ }
pub enum AdmitRefusal {
    /// Nothing registered; the ID may be retried in this generation once the cause clears.
    Refused(ModelError),
    /// The caller request ID is already registered in this generation.
    AlreadyRegistered(ModelError),
    /// The installed generation is closed: endpoint session closed, driver mux
    /// not Ready, or a capacity installation that closed it.
    GenerationClosed(ModelError),
}
impl TransportRuntime {
    /// Validation, local placement, capacity install and the read-only
    /// registration pre-checks. Cancellable by dropping the future.
    pub async fn admit_local(&self, meta: RequestMeta, operation_id: String) -> Result<Admission, AdmitRefusal>;
}
```

`admit_local` performs, serialized by the existing local admission leader with the existing single arrival deadline: `validate_meta` and the runtime-closed check; refusal of explicit CLI placement with `UnsupportedOperation` before any endpoint construction; endpoint admission exactly as today's `admit_client_request` up to and including `install_capacity` with its `CapacityMutation` guard; the read-only duplicate and exhaustion pre-checks of both the endpoint and driver `used` sets and mux tombstone counts; and a check that the driver mux is `Ready`. A duplicate returns `AlreadyRegistered`. A closed endpoint session, a driver mux that is not `Ready`, and a capacity-installation failure after pool mutation (a retirement timeout, an `install_capacity_pair` failure or an authority failure, each of which closes the generation today) return `GenerationClosed`. Every other failure returns `Refused`. Nothing is registered on any refusal. The returned `Admission` keeps leadership so no local registration can interleave before phase 2. The native owner awaits it under a `select` against the record's cancel signal and its 6-second clock (§3.5). Dropping it before pool mutation consumes nothing; dropping it after pool mutation runs the existing guard that closes the endpoint generation, which the owner's generation check then records as `faulted`. Capacity semantics are unchanged from v2 §4.

### 6.3 Phase 2: registration, synchronous

```rust
pub enum Admitted {
    /// Nothing was registered on any session; the ID may be retried in this generation.
    Refused(ModelError),
    /// At least one session registered the ID and a later step failed. The
    /// registered scope is cancelled and sealed; the caller finishes it.
    Consumed(ModelError, RegisteredCleanup),
    /// Registered on both sessions, begun on the ready mux, backend attached.
    Ready(TransportRequest),
}
pub struct RegisteredCleanup { /* sealed registrations */ }
impl RegisteredCleanup { pub async fn finish(self) -> CleanupReport; }
/// Shared with the owner's Claim; written and read only on the owner's thread.
pub struct ProgressHandle(Arc<AtomicU8>);     // NotEntered | MayHaveRegistered | Registered
impl Admission {
    pub fn progress_handle(&self) -> ProgressHandle;
    /// Synchronous: no await, no timer, nothing to cancel.
    pub fn register_and_open(self) -> Admitted;
}
```

With the generation already bootstrapped, every step of phase 2 is synchronous: endpoint registration (`ClientRequest::new`), driver registration (`RequestContext::new`), `begin()` on a `Ready` mux, and backend attachment. A refusal proved to have inserted nothing (the endpoint mux refusing before its insert) returns `Refused`. A failure after either insert (driver-side refusal, mux closed between the pre-check and `begin`) cancels and seals what was registered and returns `Consumed` with a `RegisteredCleanup`; the owner awaits its `finish()` under an 11-second clock. Success returns `Ready`.

The owner obtains the progress handle before calling `register_and_open` and attaches it to the `Claim`. Inside phase 2, on the owner's thread and in program order, core writes `MayHaveRegistered` immediately before entering any mux `register` that can insert a tombstone, restores `NotEntered` only on a proved no-insert refusal, and writes `Registered` after a proved insertion; a later registration never resets a prior `Registered`. The owner calls `register_and_open` inside `catch_unwind`. If it unwinds, the `Claim` reads the handle after the unwind is caught, on the same thread: `MayHaveRegistered` or `Registered` settles with `request_id_consumed=true`, `NotEntered` with `false`, and in every case the owner closes the generation and sets `faulted` (a hidden panic path in the sense of L2-15). The handle is therefore the v3 witness narrowed to one thread; nothing reads it concurrently. Because phase 2 has no await, there is nothing for a cancel or a timer to interrupt; a cancel requested during phase 2 is applied at checkpoint 2.

`TransportRequest` gains `generation(&self) -> u64` so the native record can pin cancellation authority now. **Supervisor hardening:** the placement supervisor thread wraps `drive()` in `catch_unwind`. On an unwind, or on loop exit while its session is open, it closes the session, runs one final retirement pass that completes the result of every sealed registration with a conservative report, and signals its waiters. `ready()`, `finish()` and capacity waiters therefore wake promptly with `Closed`; a registration sealed after that close completes at its seal, as today. Existing `request()` keeps its implementation, CLI branch, droppable future and error and cleanup timing. The only change legacy callers can observe is that their waiters now wake after a supervisor exit instead of hanging.

## 7. Native session structure

### 7.1 Record and session

```rust
struct OperationRecord {
    operation_id: String, request_id: String,
    kind: Network | Local,
    phase: Phase,                      // Issued | Attempting | Admitting | Accepted | Finishing | Terminal
    close_owed: bool,
    terminal: Option<Terminal>,        // kind, provenance, effect, error/result, cleanup
    cancel_requested: bool,
    cancellation: Option<TransportCancellation>, generation: Option<u64>,
    staged: Option<(OperationResult, Option<MergeResponse>)>, // handler output, never readable
    events: Vec<OperationEvent>,
    changed: Condvar,                  // result, event and cancel waiters
}
struct Session {
    records: HashMap<String, Arc<OperationRecord>>,  // issued-unreleased, at most 64
    claims: HashMap<String, String>,                  // live request_id -> operation_id, non-terminal network records only
    slots: u8,                                         // network Admitting + Accepted + Finishing, at most 8
    status: Open | Closing | Closed, faulted: bool, constructing: bool,
    runtime: Option<Arc<TransportRuntime>>,           // installed only after bootstrap_ready() and the lease's finish()
    close_report: Option<CloseReport>, close_summaries: Vec<CompactSummary>,
    next_serial: u64, nonce: [u8; 16], executor: tokio::runtime::Runtime,
}
```

The v2-candidate `admitting`, `active`, `last_cancel` and `request_operations` maps, `AdmissionFailure`, `end_admission`, `complete_failed_admission`, `abandon_unstarted`, `worker_panicked`, `spawned_call`, `CURRENT_SESSION`, `defer_terminal`, `publish_terminal` and `pending_terminal` are deleted. The `OperationStore` keeps `issue`, `discard`, `contains`, `events`, `wait_events`, `result`, `try_result` and `merge_response`; its record gains `settle` and the private `staged` slot. `OperationRecorder::finish`, `finish_merge` and `finish_model_error` write to `staged` in a session-owned store and publish directly in the legacy module store, so shared handler code is unchanged.

`claims` holds one entry per non-terminal network record; `settle` removes it regardless of provenance, and core decides at the next `admit_local` whether the ID may register again. The deferred ledger stage attaches at these points:

- `reserve`: the 4 KiB unstarted reservation.
- T2 `claim`: the atomic upgrade to the 8 MiB allowance, including the primary reader cursor and the 4 KiB recovery-metadata reserve, taken together with the top-level slot before any core work. A refusal there settles `TransportSessionFull` with `NotRegistered` provenance (S6).
- `accept`: binds the already-reserved primary cursor and cannot fail.
- `settle`: exchanges the reservation for charged bytes.
- Local records take their full allowance at mint (L1); a mint that does not fit raises `TransportSessionFull` synchronously without a record (L2).

The deferred handle stage calls `begin_attempt`/`claim` through the same `_admit` route as every other form.

### 7.2 Entry points and the admission sequence

`begin_attempt(id)` and `abandon_attempt(id, code, message) -> Disposition` are new native methods called by the bridge with the GIL released; both are single transitions (§5.2). `Disposition` is the retained terminal's `(code, request_id_consumed, effect)` or the current phase, so the bridge raises exactly what the ledger holds.

Both `call(.., operation_id)` and `submit(.., operation_id)` with an issued ID do, as the **first statement inside `py.detach`**, `let claim = self.claim(operation_id)?;`. That call validates nonce and serial, returns the retained terminal if the record is terminal, refuses `InvalidRequest` if an admission is already running, and otherwise performs T2 or a refused transfer (S5–S7). Only then is `current_dir()` captured and the request decoded. From this point every exit passes through `claim.refuse`, `claim.refuse_registered` or the guard's drop. Calls without an issued ID follow §7.6.

Shared admission, in order:

1. Decode `network_meta` (a non-network method with an issued ID refuses `InvalidRequest`), `validate_request_context`, and refuse explicit CLI placement before construction (A1).
2. **Construction**, if no runtime is installed (§3.6): this claim becomes the constructing owner or waits on the condvar (A3).
3. **Checkpoint 1**: `cancel_requested` read as a level after any construction wait (A2).
4. **Phase 1**: `admit_local` under `select` against the record's cancel signal and its clock. A drop registers nothing, and the owner then runs the generation check (A4–A8).
5. **Phase 2**: obtain `admission.progress_handle()`, attach it to the `Claim`, then call `register_and_open` synchronously inside `catch_unwind` (A9–A11). `Refused` → generation check, `claim.refuse`. `Consumed` → await `cleanup.finish()` under its clock, generation check, `claim.refuse_registered`.
6. On `Ready(request)`: **checkpoint 2** (A12); for `submit`, spawn the parked worker (spawn failure is A13); then `claim.accept(request)` (A15).

`call` runs `accepted.run(dispatch)` inline on the claiming thread and returns the response bytes. `submit` sends `accepted` through the gate (T3) and returns the `Accepted` envelope built by the session; the legacy `submit_accepted` is not on this path. `run` catches unwind around the handler only, keeps a successful output local, retains the `TransportRequest` in all cases, moves to `Finishing`, awaits `finish()` under its clock, then settles by the C-row precedence and releases the slot. A caught handler panic yields `Failed(InternalError, possible)` after finish and sets `faulted`; a clock win during finish settles by the C6 kind rule and sets `faulted`.

### 7.3 Worker gate

The gate is a one-shot channel whose message is the `AcceptedOperation`. The worker thread is spawned before `accept`; its body is `recv` followed by `run`, with nothing between. `accept` writes `Accepted` under the session mutex, then sends. A failed send returns the value to the sender, which calls `abandon()` (C1). A receiver dropped after `recv` held the value, so its drop settled. A sender that unwinds before `send` still holds either the `Claim` (before `accept`) or the `AcceptedOperation` (after), whose drop settles; the parked worker sees channel closure and exits without Git work. Local submitted operations use the same gate with an `AcceptedOperation` that holds no `TransportRequest`.

### 7.4 Locking model

Two mutex kinds, one order. The **session mutex** guards `status`, `faulted`, `constructing`, `records`, `claims`, `slots`, `next_serial`, `close_summaries` and every record's phase-bearing fields (`phase`, `close_owed`, `cancel_requested`, `cancellation`, `generation`). Each **record mutex** guards only that record's payload (`terminal`, `staged`, `events`) and its condvar. Session before record, never the reverse. Readers and handler event appends take only the record mutex. Every transition and intent (`reserve`, `begin_attempt`, `abandon_attempt`, `claim`, `accept`, `settle`, cancel, close, release, and a refused transfer) is one function entered with no lock held that takes the session mutex, validates phase against intent, updates phase, slots, claims and owed summaries, then takes the record mutex to write the terminal, releases it, notifies both condvars and releases the session mutex. No core, Git or Python call runs under either mutex; `accept` notes a pending cancel under the session mutex and signals the handle after release, and a concurrent cancel that observes `Accepted` signals the same idempotent handle itself. The drop guards (`Construction`, `Claim`, `AcceptedOperation`) run in frames that hold neither mutex.

### 7.5 The owner's clock in native code

Every await of a core future by an owner is `tokio::time::timeout(bound, future)` on the session's executor, combined with the record's cancel signal in a `select` for phase 1. The executor is the existing multi-thread tokio runtime with one worker thread and `enable_all()`; its timer is driven by that worker thread, never by core's supervisor. Each bound is the sum of the core deadlines that run in sequence inside the awaited future, plus one second:

| Awaited core future | Core deadlines that run in sequence | Owner clock |
| --- | --- | --- |
| `admit_local` (phase 1) | one 5 s arrival deadline covering leadership, capacity gate and retirement | 6 s |
| `bootstrap_ready()` | the 5 s mux bootstrap deadline from Bind | 6 s |
| `BootstrapLease::finish()` | driver then endpoint registration cleanup, 5 s each from its own seal | 11 s |
| `TransportRequest::finish()` | driver then endpoint registration cleanup, 5 s each from its own seal (`request.rs:328-339`) | 11 s |
| `RegisteredCleanup::finish()` | at most the same two registrations in turn | 11 s |
| `shutdown()` of a session runtime (no CLI endpoint) | endpoint then driver `cleanup()`, 5 s each | 11 s |

A clock win drops the core future and settles conservatively. Legacy `TransportRequest::finish()` timing is unchanged. No other timer exists in the session or in core.

### 7.6 Entries without an issued operation ID

Three kinds of native call carry no issued operation ID.

1. **Capability preflight.** `transport_capabilities` answers only from an installed runtime, which has always bootstrapped. If none is installed, the caller becomes the constructing owner under §3.6 while holding no record, or waits for the current constructor. It then answers from the installed generation, which later dispatch uses (GwzPyTransportDesign §4's same-generation rule). A construction failure returns its typed error to the preflight caller and leaves the session `Open`. A `faulted` session refuses the preflight with `TransportGenerationBusy`; a `Closing` session refuses it as closed.
2. **Local unary commands** (`status`, `commit` and the other methods that need no transport) bypass the ledger and the transport runtime. `faulted` does not refuse them; they refuse once Closing starts (v2 §6). A unary handler that retains a record, such as unary `merge`, runs with a session-scoped store that is discarded when the call returns, never the process-global store.
3. **Local submitted operations** (`merge`, `clone_local_workspace`) are minted natively at submit entry under the session nonce (L1), so they have no pre-entry interval. The native submit thread holds their local `Claim` through decoding and validation, spawns a parked worker, and transfers the `AcceptedOperation` through the §7.3 gate (L3–L5). Their handlers record into the minted session record, and the session builds the `Accepted` envelope with the minted ID. They count toward the 64-record limit and take no top-level slot, capacity or registration. Cancel returns `UnsupportedOperation` (L6). Close neither cancels nor joins them and lists no summary for them; each settles into the retained ledger when its handler returns, including after close. The high-level merge helpers release the record after reading its final response, as the implicit helpers do (§8). No lookup through a native-session bridge reaches the process-global store (I10).

## 8. The Python owner

The bridge is the owner of every admission attempt from T1 until T2. All three forms (explicit `accepted()`, implicit unary `call`, and a stream helper's first iteration) use one method:

```python
async def _admit(self, native_entry, method, names, request, operation_id):
    self._session.begin_attempt(operation_id)              # T1; raises the retained or refused disposition
    try:
        request_bytes = encode_message(...)                # may raise before native entry
        worker = asyncio.create_task(asyncio.to_thread(native_entry, method, *names, request_bytes, operation_id))
        return await asyncio.shield(worker)                 # native claim (T2) happens inside the worker
    except asyncio.CancelledError:
        ... existing path: cancel_operation(operation_id), await worker completion, re-raise ...
    except Exception as exc:
        typed = self._typed_pre_entry_failure(exc)          # IoError (executor/scheduling), InvalidRequest (encoding), else InternalError
        disposition = self._session.abandon_attempt(operation_id, typed.code, typed.message)
        raise self._bridge_error_from(disposition, operation_id) from exc
    except BaseException:
        self._session.abandon_attempt(operation_id, "InternalError", "admission interrupted")
        raise                                               # GeneratorExit, KeyboardInterrupt and SystemExit propagate unchanged
```

`abandon_attempt` settles the record only if it is still `Attempting` (native never claimed) and otherwise returns what the ledger already holds, so the raised error always matches the retained terminal and no second terminal can arise from a racing native worker. A refused native `claim` has already settled the record on the attempt's behalf (§3.3); the typed error the worker raised and the disposition `abandon_attempt` returns therefore agree. Executor shutdown surfaces as an exception from the awaited task and takes the `Exception` path. A task cancelled while the native call is still queued takes the cancel path, whose `cancel_operation` settles `Attempting → Cancelled`, and whose late native `claim` then reads that terminal without minting. A Python failure after native has completed (for example, decoding the response) finds a `Completed` or `Failed` terminal; the bridge raises its own error carrying `operation_id` so the caller can still read the result.

Implicit unary and stream forms expose the issued `operation_id` on the raised error. After a pre-effect refusal they release their internal record once the typed refusal has been detached onto the raised exception, extending v2 §2's detached-record rule for cancelled helpers; an explicit handle keeps its record until `release()`. Every projection carries `request_id_consumed` (§11 item 2).

Concurrent `accepted()` calls on one handle are serialized by a loop-agnostic in-flight future: a `threading.Lock` guards a per-handle `concurrent.futures.Future` that the first caller completes and every other caller awaits through `asyncio.wrap_future`, so callers on different event loops observe the same outcome (v2 §2). Cancelling a task that awaits `result()` or iterates `events()` stops only that wait: the operation's owner is native and a Python waiter only observes it, so the operation continues and remains cancellable through `cancel()`. `cancel()` and `cancel_operation()` shield their native join from a repeated task cancellation, as `close()` already does (v2 §6): the join completes, then the cancellation propagates. The bridge uses this admission path only when the native session exposes `begin_attempt`; otherwise handle factories refuse `UnsupportedOperation` before issuing, and implicit forms use the pre-V4 call path. The bridge's `_issued_requests` cache remains a convenience and is never consulted for an admission decision.

## 9. Intents in detail

`cancel(id)` takes the session mutex first and acts by phase per §3.4: `Issued`/`Attempting` settle `Cancelled(NotRegistered)`; `Admitting` sets `cancel_requested`, wakes the owner's phase-1 select and any construction wait, then waits on the record condvar for the terminal; `Accepted`/`Finishing` copies the stored handle, releases both locks, signals it idempotently and waits; `Terminal` reads the retained cleanup; a local operation returns `UnsupportedOperation`. Repeated cancellation returns the same cleanup (v2 §6).

`close()` moves `Open → Closing` under the session mutex, marks network records then `Admitting`, `Accepted` or `Finishing` as close-owed (at most eight), applies the cancel intent to every live network record by phase, and waits on the session condvar until `slots == 0` and `constructing == false`. I4, the `Construction` guard and the owner's clocks make that wait finite. Local submitted operations are neither cancelled nor joined. Close then shuts the runtime down once under its clock (or reads a losing constructor's cleanup), combines physical facts with the owed summaries, and writes `Closed`. Concurrent and repeated closes join and return the same report. A `faulted` session reports `peer_cleanup_confirmed = false` and `pending_local_work ≥ 1`. A `Closing` session refuses `reserve`, `begin_attempt`, `claim`, local mints, local unary commands and the capability preflight; cancel, release, result and event reads keep working on the ledger after close (v2 §6).

Timeouts never appear here: they are consumed by owners (§3.5). Faults are settled by the owner that suffers them (§3.4).

## 10. Terminal publication, readers and close summaries

`settle` (§7.4) writes the terminal once; readers wait on the record condvar. `result()` returns `Completed`, `Failed` and post-acceptance `Cancelled` results and raises the typed refusal for `Refused` and pre-acceptance `Cancelled`; `try_result` returns `None` until a terminal exists; `wait_events` completes exactly when a terminal exists, and a terminal event precedes iterator end for accepted operations; `merge_response` is served only from a `Completed` terminal's promoted payload. No reader can observe staged output; promotion is a step inside `settle(Completed)` and nothing else performs it. If `close_owed` is set, `settle` copies the compact status, code and effect into `close_summaries` before releasing the session mutex, so a later `release` can discard the full result but never an owed summary. Local records settle the same way.

## 11. Amendments and clarifications to the v2 contract

V4 amends the v2 contract in items 1–8 and states behavior that v2 leaves unstated in items 9–11. Nothing else in v2 §§2–8 changes. In particular, taking the ledger upgrade at T2 `claim` (§7.1) is v2 §6's reservation "before `Accepted`" and v2 §5's pre-acceptance `TransportSessionFull`, not a change.

1. **Registration is defined.** The caller request ID is consumed for the current generation when core reports `AlreadyRegistered`, `Consumed` or `Ready`, or when an unwind inside the synchronous registration step leaves insertion indeterminate and closes the generation. It is not consumed when core reports a `Refused` that proves no insertion. Both endpoint and driver registrations are one core step; a partial registration counts as consumed.
2. **The retry disposition is exposed, not inferred.** Every refusal and terminal projected to Python carries `request_id_consumed: bool`: `GwzBridgeError`, `GwzOperationError` and `GwzOperationCancelled` alike. It states whether this request ID is registered in the current generation, or that cannot be ruled out. `false` permits reuse on a fresh handle or call only while the Client is not `faulted` and the refusal's cause has cleared; `true` means the ID must not be reused in that generation. A duplicate refusal carries `true`. `GwzOperationCancelled` subclasses `asyncio.CancelledError` and is not a `GwzBridgeError`. `recent_operations()` descriptors (deferred) carry the same field.
3. **`Accepted` for `submit()` is produced by the session**, not by the legacy `submit_accepted`.
4. **Inline `call()` is an accepted worker form.** The bounded claiming thread owns the accepted scope for `call()` after the same prerequisites; `submit()` and stream helpers keep the spawned parked worker and gate. This supersedes only v2 §5's spawn-and-gate wording for `call()`.
5. **Finish-unwind, lost-owner and clock-win terminals.** v2 §5's rule that a worker panic publishes its terminal after `TransportRequest.finish()` holds for every caught handler failure. If `finish()` itself unwinds, the accepted owner is lost outside the handler catch, or the owner's clock wins during finish, the terminal is published without a returned finish report, as `Failed`, or `Cancelled` when cancellation had been signalled, with `effect="possible"`, `pending_local_work ≥ 1` and `peer_cleanup_confirmed = false`, and the session is `faulted` so close drains it.
6. **Generation bootstrap.** A session runtime is bootstrapped at construction through one reserved internal registration whose ID carries a random token, is never reported, and does not count against v2 §5's 256 caller registrations. Construction includes that in-process bootstrap; it still performs no Git-host connection and no credential access.
7. **A faulted Client refuses network work.** When the installed generation closes by any path other than `close()`, the Client becomes `faulted`: network issuance, admission and the capability preflight refuse with `TransportGenerationBusy` and `request_id_consumed=false` until the Client is closed and recreated. v2 §5 and §7 already name this code for a generation that cannot admit; V4 introduces it before rollover exists.
8. **Implicit forms release refused records.** Implicit unary and stream forms release their internal record once a pre-effect refusal has been detached onto the raised exception, extending v2 §2's detached-record rule for cancelled helpers.
9. **Clarification: waits are observers.** Cancelling a task that awaits `result()` or iterates `events()` stops only that wait; the operation continues and stays cancellable through `cancel()`. `cancel()` finishes its join even if its awaiting task is cancelled again, as `close()` does.
10. **Clarification: local submitted operations.** Local submitted operations through a native Client are session records (v2 §6 already assigns merge responses to the session). They count toward the 64-record limit, are not cancellable, and are neither cancelled nor joined by close.
11. **Clarification: expiry.** The 15-minute expiry applies to `Issued` and terminal records, not to an `Attempting` record, which has an active owner.

## 12. Fault matrix

Each row is a required closure case for both `call` and `submit` unless marked, and cites the §5.2 row it instantiates. Columns: fault point, §5.2 row, terminal kind / provenance, effect, whether and when the same request ID can be retried, core registration, Git effect, waiters woken, slot and claim.

| Fault point | §5.2 | Terminal | Effect | Retry same ID | Core reg. | Git | Waiters | Slot/claim |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Executor shut down before native entry (bridge) | S8 | Refused / NotRegistered, `IoError` via `abandon_attempt` | none | yes | no | no | yes | never taken; claim released |
| Request encoding fails after issuance (bridge) | S8 | Refused / NotRegistered, `InvalidRequest` | none | yes, with a corrected request | no | no | yes | never taken; claim released |
| Another `BaseException` during the attempt (bridge) | S8 | Refused / NotRegistered, `InternalError`; the exception propagates unchanged | none | yes | no | no | yes | never taken; claim released |
| Bridge exception after native claimed | S9 | unchanged; `abandon_attempt` returns the native disposition | per native terminal | per native terminal | per native terminal | per native terminal | yes | per native terminal |
| Cancel while `Attempting` (blocked executor) | S10 | Cancelled / NotRegistered; a late `claim` reads it without minting | none | yes | no | no | yes | never taken |
| Ninth top-level operation, or the ledger upgrade does not fit | S6 | Refused / NotRegistered, `TransportSessionFull`, recorded by the refused claim | none | yes, after a slot or ledger space frees | no | no | yes | slot never taken; claim released |
| 65th issued record | S2 | synchronous `TransportSessionFull`, no record | none | yes, after a release or expiry | no | no | n/a | n/a |
| Client `faulted` at `reserve`, `begin_attempt` or `claim` | S2, S4, S7 | `TransportGenerationBusy`; synchronous at `reserve`, otherwise Refused / NotRegistered | none | no; close and recreate the Client | no | no | yes | never taken |
| `current_dir()` fails after claim | A1 | Refused / NotRegistered, `IoError` | none | yes | no | no | yes | released |
| Malformed CBOR, wrong message name, or an issued ID on a non-network method | A1 | Refused / NotRegistered, `InvalidRequest` | none | yes, with a corrected request | no | no | yes | released |
| Explicit CLI placement, `HOME` present or absent | A1 | Refused / NotRegistered, `UnsupportedOperation`, before any endpoint construction | none | no; the placement is unsupported | no | no | yes | released |
| Cancel at checkpoint 1, including while waiting for construction | A2 | Cancelled / NotRegistered | none | yes | no | no | yes | released |
| Endpoint construction error (`HOME` unset) | A3 | Refused / NotRegistered, `InvalidRequest`; session stays `Open` | none | yes, after the environment is fixed | no | no | yes | released |
| Bootstrap fails, or a construction clock wins | A3 | Refused / NotRegistered, `IoError`; runtime shut down under its clock and discarded; waiting claims refuse likewise; session stays `Open` unless the shutdown clock also wins | none | yes, on a later construction | no (reserved ID only) | no | yes | released |
| Constructing owner unwinds (in construction, bootstrap readiness, the lease's finish or the failure-path shutdown) | A3 | Refused / NotRegistered, `InternalError` for this and every waiting claim; the `Construction` drop sets `faulted`; close proceeds | none | no; close and recreate the Client | no | no | yes | released |
| Capacity conflict | A4 | Refused / NotRegistered, `TransportCapacityConflict` | none | yes, after the conflicting operation is terminal | no | no | yes | released |
| Leadership or capacity-wait timeout before pool mutation | A4 | Refused / NotRegistered, `IoError` | none | **yes** | no | no | yes | released |
| Retirement timeout, `install_capacity_pair` failure or authority failure after pool mutation | A6 | Refused / NotRegistered, `TransportGenerationBusy`; core closed the generation; `faulted` | none | **no**; close and recreate the Client | no | no | yes | released |
| Endpoint session closed or driver mux not `Ready` at the phase-1 check | A6 | Refused / NotRegistered, `TransportGenerationBusy`; `faulted` | none | no; close and recreate the Client | no | no | yes | released |
| Duplicate request ID in generation | A5 | Refused / AlreadyRegistered, `InvalidRequest`; `request_id_consumed=true` | none | no; choose a new ID | no (registered earlier) | no | yes | released |
| Cancel while phase 1 is pending | A7 | Cancelled / NotRegistered; a drop after pool mutation closes the generation and the check sets `faulted` | none | yes if the generation stayed open; otherwise close and recreate the Client | no | no | yes | released |
| Owner clock wins while phase 1 is pending (stalled supervisor) | A8 | Refused / NotRegistered, `IoError`, or `TransportGenerationBusy` with `faulted` if the drop closed the generation | none | as the previous row | no | no | yes | released |
| Endpoint mux refuses before insert | A9 | Refused / NotRegistered, typed core error | none | yes | no | no | yes | released |
| Driver registration or `begin()` fails after the endpoint insert | A10 | Refused / Registered, typed core error; cleanup finished under its 11 s clock; `faulted` if the generation closed | none | no; choose a new ID | consumed | no | yes | released |
| Unwind inside `register_and_open` | A11 | Refused, provenance from the progress handle, `InternalError`; generation closed; `faulted` | none | no; close and recreate the Client | per handle | no | yes | released; cleanup unconfirmed |
| Cancel requested during phase 2 or before checkpoint 2 | A12 | Cancelled / Registered | none | no; choose a new ID | consumed | no | yes | released after finish |
| Worker thread spawn failure (`submit`) | A13 | Refused / Registered, `IoError` | none | no; choose a new ID | consumed | no | yes | released after finish |
| Unwind between spawn and `accept` | A14 | Refused / Registered via `Claim` drop, `InternalError`; generation closed; `faulted` | none | no; close and recreate the Client | consumed | no | yes | released; cleanup unconfirmed |
| T3 send fails (worker gone before `recv`) | C1 | Failed / Registered, `InternalError`, via `abandon()` | possible (conservative; no Git ran) | no; choose a new ID | consumed | no | yes | released after finish |
| Cancel after acceptance, completion loses | C4 | Cancelled / Registered, code 73 | possible | no | consumed | possible | yes | released after finish |
| Cancel after acceptance, completion wins | C3 | Completed | n/a | no | consumed | yes | yes | released after finish |
| Handler error | C5 | Failed / Registered | possible | no | consumed | possible | yes | released after finish |
| Handler panic caught inside `run` | C5 | Failed / Registered, `InternalError`; `faulted` | possible | no | consumed | possible | yes, after `finish()` | released after finish |
| Slow but healthy cleanup (each registration near its 5 s window) | C3, C4 or C5 | per handler outcome, with core's report; not `faulted` | per kind | no | consumed | per kind | yes | released after finish |
| Owner clock wins during `finish()` (dead supervisor) | C6 | Cancelled if signalled, else Failed / Registered; cleanup unconfirmed; `faulted` | possible | no | consumed | possible | yes | released |
| `finish()` unwinds or the accepted owner is lost | C7 | as the previous row, via the `AcceptedOperation` drop | possible | no | consumed | possible | yes | released |
| A sibling's cleanup expiry or a mux deadline closes the generation under an accepted operation | C4 or C5, generation check | Cancelled or Failed / Registered, settled once by its owner; `faulted` | possible | no | consumed | possible | yes | released after finish |
| Placement supervisor exits while an operation runs | C4, C5 or C6 | the hardening closes the session and completes sealed registrations; the owner settles once; `faulted` | possible | no | consumed | possible | yes | released |
| Close while `Issued` or `Attempting` | S10 | Cancelled / NotRegistered | none | closed | no | no | yes | n/a |
| Close while `Admitting` | A2, A7 or A12 | per row | none | closed | per row | no | yes | released |
| Close while `Accepted` or `Finishing` | C3 or C4 | Completed or Cancelled as the race decides; summary in the close report | per kind | closed | consumed | possible | yes | released |
| Release after an owed terminal but before the close report commits | T2 | full result discarded; owed summary retained | per terminal | closed | consumed | possible | yes | released |
| Release while `Attempting`, `Admitting`, `Accepted` or `Finishing` | T3 | unchanged; `OpenOperation` | — | — | — | — | — | held |
| Local submitted operation with 64 records, or during Closing | L2 | synchronous `TransportSessionFull` or `InvalidRequest`, no record | none | n/a | no | no | n/a | n/a |
| Local submitted operation: decode, validation or spawn failure | L3 | Refused / NotRegistered, typed error | none | n/a | no | no | yes | no slot |
| Local submitted operation completes, fails, or its T3 send fails | L5 | Completed, or Failed with possible effect | per kind | n/a | no | per kind | yes | no slot |
| Cancel of a local submitted operation | L6 | unchanged; `UnsupportedOperation` | — | — | — | — | — | — |
| Close while a local submitted operation runs | L5 | neither cancelled nor joined; settles into the retained ledger after close; no summary | per kind | n/a | no | per kind | yes | no slot |
| Pre-effect refusal of an implicit unary or stream call | S6–S8, A1–A14 | per row; the refusal is detached onto the raised exception and the internal record released | none | per `request_id_consumed` and row | per row | no | yes | released |

## 13. Required proof and gates

Design review first: focused re-verdicts by the same Consistency, Safety and Surface reviewers on this committed tuple, covering this document, the v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md`, the caller-guide note and the pointer in `gwz-py/dev-docs/GwzPyTransportDesign.md`. Any P0–P2 keeps implementation stopped. Implementation review then uses fresh Code and State reviewers on a new exact tuple.

Focused closure tests, typed-field assertions only:

1. **Ownership before native entry.** Shut down the loop's default executor, then attempt explicit handle admission and implicit `call`/`submit` on issued IDs: one typed retained terminal per ID matching the raised error, `request_id_consumed == false`, zero slots and claims, and no record-limit growth after repeated implicit attempts. Inject an encoding failure for an issued handle: same assertions with `InvalidRequest`. Raise `KeyboardInterrupt` inside the attempt: the record settles and the exception propagates unchanged. Race `abandon_attempt` against a native `claim` that already ran: exactly one terminal, and the bridge raises the native disposition.
2. **Atomic handoffs.** Block the default executor, issue, schedule, cancel, release: one serial, no remint, `Cancelled/NotRegistered`, late native entry reads the terminal. Fill the eight slots: the ninth claim settles `TransportSessionFull` on the attempt's behalf and `abandon_attempt` returns that code. Inject a worker thread that exits before `recv`: `abandon()` path, one `Failed` terminal, `submit` returns the typed error, finish completed, no Git. Panic after `recv`: drop settlement. Panic between spawn and `accept`: `Claim` drop settlement. Call `accepted()` concurrently on one handle from two threads with two event loops while admission is held: both return the same outcome.
3. **Provenance without text.** Hold the capacity gate past the deadline before pool mutation: `Refused`, `request_id_consumed == false`, and the same ID is admitted after release. Admit a duplicate: `AlreadyRegistered`, `true`. Hold retirement past the deadline after mutation: `GenerationClosed`, `faulted`, and the next attempt refuses `TransportGenerationBusy`. Inject a driver-side refusal after the endpoint insert and a `begin()` failure on a closed mux: `Consumed`, `true`, cleanup finished, the same ID refused afterwards. Inject an unwind after the tombstone insertion, driven through the `Claim` path: the progress handle reads `MayHaveRegistered`, the generation closes, and the terminal reports `true`. Every case is distinguished by variant alone.
4. **Owner clocks and core deadlines.** With a live supervisor, hold each of the two sequential registration cleanups for about 4 s: finish returns before its clock with core's report, `faulted` stays clear, a successful handler publishes `Completed`, and the next admission succeeds. Stall the supervisor-driven mux clock (a hang, not an exit) and withhold Bound: the bootstrap-readiness clock wins at 6 s, the runtime is discarded, waiting claims refuse, the session stays `Open`, and a later construction succeeds. With the clock still stalled, the phase-1 clock wins at 6 s and the finish clock at 11 s, the latter with one conservative terminal and `faulted`. Let Bound arrive just before the readiness clock: the timer is dropped and no close follows within twice the bound. Separately, make the supervisor exit while a finish is pending: the hardening completes sealed registrations and wakes waiters before any owner clock.
5. **Every pre-worker exit settles, both forms.** Parametrize the §12 rows before acceptance over `call` and `submit`: one terminal; the returned error and `operation_result(id)` agree; `wait_events` completes; slot and claim released; no registration except where the row says consumed; no Git; release succeeds; repeat past 64 records and admit afterwards.
6. **Terminal uniqueness under faults.** Stage a success and panic in `finish()`; panic in the handler while finish is held (waiters pend until finish returns); drop the `Claim` before accept and an `AcceptedOperation` before send; let a handler return `Err` after a signalled cancellation. One terminal each, following the C-row precedence; no success ever visible.
7. **Intents in every phase.** Cancel and close at each phase including `Attempting`; release refusals; close idempotence; owed summaries surviving release; post-close ledger reads; refused new work. Close a Client holding 64 issued handles and 8 live operations: exactly eight summaries. In the timer stage, an `Attempting` record older than 15 minutes survives and its late claim settles normally.
8. **Legacy preservation and isolation.** Existing `request()` and `transport_host` tests unchanged; a legacy runtime keeps 256 caller registrations and refuses `bootstrap_ready()` with `UnsupportedOperation`; a legacy first request still bootstraps; a supervisor exit wakes legacy waiters; CLI registration, duplicate refusal and capacity via `request()`; the legacy module store and the session store never resolve each other's IDs.
9. **Lock order.** A debug-build assertion that the session mutex is never acquired while a record mutex is held and that no core or Python call runs under either, enabled in the focused native tests.
10. **Construction ownership.** Inject a panic inside `bootstrap_ready()` after Bind, with a second claim waiting and a concurrent `close()`: the waiter refuses `NotRegistered`, `faulted` is set, and close returns a conservative report within its bound. Repeat for a panic in the lease's `finish()` and in the failure-path `shutdown()`.
11. **Generation faults.** A retirement timeout after mutation, a post-mutation cancel, a driver-mux close, and a sibling's cleanup-expiry close under an accepted operation B: B settles exactly once with no second terminal; each case sets `faulted`, and the next network attempt refuses `TransportGenerationBusy`. Local unary commands and local submitted operations keep working.
12. **Entries without an issued ID.** On a fresh Client, an identity-option fetch runs the capability preflight, which constructs and bootstraps; the fetch is admitted on the same generation, with the driver mux `Ready` before the first admission. A preflight construction failure leaves the session `Open`, and a later claim constructs. A preflight racing a claim's construction, and a close during preflight construction, both terminate. `status` and `commit` work before and after network work and while `faulted`. A submitted `merge` result is read by ID through the native bridge and never touches the process-global store. Cancel of a local operation returns `UnsupportedOperation`.
13. **Ledger attach points.** Specified now and run in the ledger stage: with the ledger within 8 MiB of full, admission refuses `TransportSessionFull` at `claim` with `request_id_consumed=false` and no core registration; after records are released, the same ID is admitted.
14. **Reserved bootstrap ID.** `bootstrap-1` and other deterministic forms are admitted as caller request IDs in the first generation. The reserved ID never appears in results, events, descriptors, errors or close reports.

The deferred stages and their gates (ledger bytes and timer, public handles, rollover, transport stress, platform, wheel and source pins) remain separate and are not waived by a foundation GO.

## 14. Out of scope, unknowns and risks

- The `TransportRecordLimit` model code belongs to the ledger stage. `TransportGenerationBusy` is introduced now (§11 item 7). The record's `generation` field and `staged` slot are laid down now so later stages add behavior without changing the state machine.
- A `faulted` Client cannot admit network work until it is closed and recreated, or until the rollover stage exists. A failed or timed-out construction does not fault the session unless its shutdown also times out.
- The reserved bootstrap ID is protected by a 128-bit random token rather than a separate namespace; a caller can collide with it only by guessing the token.
- `Consumed` from a driver-side failure after the endpoint insert is expected to be rare once the pre-checks and the `Ready` check exist; it is kept so no caller ever infers.
- Owner clocks win only when the supervisor is dead. A supervisor that hangs inside `drive()` while holding core's session mutex also blocks core's synchronous calls and is outside the failure model. `from_environment_for_session()` remains unclocked, as `from_environment()` is today.
- The claiming thread can block through construction (up to 6 s plus 11 s), phase 1 (6 s) and a consumed cleanup (11 s). This has the same shape as the current `submit`, which already blocks until registration; Python runs both forms under `asyncio.to_thread`.
- Local submitted operations are not joined by close, so a local handler may still be writing the workspace after `close()` returns, as today's local operations can. Their results remain readable by ID afterwards.
