# Python transport session v4 foundation — operation ownership protocol

Date: 2026-09-24. Status: **DRAFT v4 foundation design (a transition design under AgentProcessRules §7.1); dual peer-blind Consistency/Safety design review and a docs-only Surface check are required before implementation restarts; this document carries no implementation, build, or activation authority.** It replaces the stopped [v3 foundation draft](GwzPyTransportSessionV3FoundationDesign.md), whose [first remediation verdict](GwzPyTransportSessionV3FoundationDesign-Verdict-1.md) reached the program's third-new-root-cause stop at root `21fac9f4236cd12df7719a92f039ae5cd787f067`, gwz-core `e7b4c499a2e5e2bfe8be0db2fbe067d6599a506f`, gwz-py `f6ae40afaa67ddaa561002dcee54d3c522739dd1`. The baseline this draft describes is root `106c08f` (the verdict commit) with those member commits. The dirty root checkpoint file and the untracked SSH N2b prompt and route-mapping drafts are unrelated and out of scope.

## 1. Decision, boundary and precedence

The operator's direction after the v3 stop is that the repeated failures are a lifecycle **ownership** problem, not a transport-protocol problem. Python issues an operation ID and schedules native work; the native session admits; core registers transport requests; a worker runs Git; cancellation, close, timeouts and faults intervene throughout. No single contract said who owned an operation in every interval, so each review found one more interval with no owner or with two deciders. Correcting intervals one at a time added a guard, a witness and a watchdog whose interactions produced the next interval. V4 therefore states one ownership protocol first and derives every mechanism from three rules:

- **R1 Immediate ownership.** Every issued operation has exactly one owner at every instant, including before any executor or thread has accepted work for it.
- **R2 Atomic transfer.** Every change of owner is one atomic decision. Either the receiver owns the operation from that decision on, or the sender still owns it and remains responsible for recording its failure.
- **R3 One arbiter.** Timeout, cancellation, completion and fault compete only through the operation's own state machine, decided by its current owner. A timeout that loses to a completed transfer has no authority over the result.

The accepted [v2 contract](GwzPyTransportSessionV2Design.md) remains the caller-facing contract for identity, admission meaning, capacity, outcome ledger, close and typed terminals; §11 lists the six exact places V4 amends it. V4 replaces the foundation architecture beneath it: the native session in `gwz-py/native/src/transport_session.rs`, `operations.rs` and `dispatch/mod.rs`, the bridge admission path in `gwz-py/src/gwz/bridge.py`, and a candidate-only local admission API in `gwz-core/src/transport_host/{mod,session,request}.rs`. These are internal Python/native and core host API changes. The Taut request/response method set, the `gwz-transport` envelope, the virtual-stream delivery protocol and the appended `cancelled=73` and `transport_record_limit=74` codes are unchanged: application operations have their own lifecycle here, while transport messages continue to flow independently and Python forwards them regardless of application activity. Existing core `TransportRuntime::request()` keeps its implementation and timing for CLI and local commands.

V4 withdraws three v3 mechanisms: the core-side wall-clock watchdog and its closing callback; the asynchronous phase-2 wait and its "not cancellable" clause; and the cross-thread `RegistrationWitness`. It retains from v3 the single record, the `Claim` and `AcceptedOperation` ownership values, session-then-record locking, private staged output with a single terminal writer, close-owed summaries, legacy-path isolation and the local-only two-phase core admission. The deferred stages (byte ledger, timed expiry, public `start_*` handles and `OperationStream`, generation rollover, transport stress, platform gates) stay deferred and attach at the three points named in §7.1.

Core requirements and design must be amended before core behavior changes (`gwz-core/AGENTS.md`). This tuple replaces the withdrawn v3 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` with v4 paragraphs marked pending this review.

## 2. Why v3 stopped, in ownership terms

**Before native entry.** Python issued an ID, then scheduled the native call with `asyncio.to_thread`. If the executor was shut down or request encoding failed, the caller received an error while the record stayed `Issued`. The only settlement owner v3 defined, the native `Claim`, is created at native entry, so it never existed. In R1 terms the operation had no owner between Python's decision to admit and native entry.

**At admission completion.** v3 bounded the phase-2 wait with a core watchdog whose callback closed the generation on expiry. That callback was a second decider: it could fire after phase 2 returned `Ready` and the worker had started. In R3 terms two parties had authority over one outcome.

**The same pattern earlier.** The v2 foundation's rounds each closed one interval and found another (eight-slot refusal, spawn failure, deferred `submit` failure, current-directory failure, registration inferred from error codes). v3's first round found phase 2 droppable by a wrapper, a witness not atomic with the mux tombstone, a handler panic settling before finish, and release erasing a close summary. Every one is an interval with no owner or a second decider.

**What V4 does instead.** It defines an owner for every interval (§3.2), makes each transfer one atomic decision (§3.3), routes every intervener through the owner's state machine (§3.4), and removes timers as actors by giving each owner its own clock (§3.5). Two consequences make the machinery smaller rather than larger. First, mux bootstrap moves from the first operation's admission into generation construction, so per-operation admission has no asynchronous step after capacity; phase 2 becomes synchronous and needs no watchdog. Second, the Python side becomes an explicit owner with an atomic handoff to native, so the pre-entry gap is covered by the same state machine as everything else.

Disposition of the [v3 stop verdict](GwzPyTransportSessionV3FoundationDesign-Verdict-1.md) findings: Safety P2-1 (unowned pre-entry interval) is §3.2, §5 and §8; Safety P2-2 (watchdog versus `Ready`) is §3.5 and §6, which delete the watchdog; Consistency R2-C-P2-1 (finish-unwind exception missing from the supersession list) is §11 item 5; Consistency R2-C-P2-2 (retry promised after a generation-closing timeout) is the split rows in §12; Surface P3-2 (retry example lifecycle) is the completed example in the caller guide.

## 3. The ownership protocol

### 3.1 Parties and resources

Owners are the parties that can decide an operation's next transition: the **ledger** (the native session's record store, passive), a **Python attempt** (the coroutine executing one admission attempt), a **native admitting thread** (holding a `Claim`), and an **accepted owner** (whoever holds the `AcceptedOperation` value: the worker thread for `submit`, the calling thread for `call`). Core is not an owner. Core resources (`Admission`, `TransportRequest`, `RegisteredCleanup`, capacity leadership) are Rust values held by the current native owner, and core's placement supervisor is a service whose failures reach owners as errors or as expired owner clocks. Interveners (cancel, close, release, timeout, fault) are not owners either; they are intents (§3.4) or signals (§3.5).

### 3.2 The owner ladder

| Phase | Owner | Proof of ownership | How ownership leaves |
| --- | --- | --- | --- |
| `Issued` | the ledger (passive) | record phase | T1 `begin_attempt` to a Python attempt; or an intent settles it (cancel, close, release, expiry) |
| `Attempting` | the Python attempt | the `Attempt` scope in the bridge (`try`/`finally`) | T2 `claim` to native; or `abandon_attempt`, cancel or close settles it |
| `Admitting` | the native admitting thread | the `Claim` value | `accept` converts the `Claim` into an `AcceptedOperation` on the same thread; `refuse` or the guard's drop settles it |
| `Accepted`, `Finishing` | the holder of the `AcceptedOperation` value | that value (exactly one holder by Rust ownership) | T3 gate send moves the value to the worker thread (`submit` only); T4 `settle` returns ownership to the ledger; the value's drop settles it |
| `Terminal` | the ledger | the immutable terminal | release or expiry discards the record |

There is no phase without a row, and no row with two owners.

### 3.3 The transfers

- **T1 `begin_attempt(id)`**: one transition `Issued → Attempting` under the session mutex. It fails with the retained terminal if the record is already terminal, and with `InvalidRequest` if an attempt or admission is already in progress. From success on, the Python attempt owns the record and is responsible for reaching T2 or recording its failure.
- **T2 `claim(id)`**: one transition `Attempting → Admitting` (or `Issued → Admitting` for a direct native caller, which has no pre-entry interval) under the session mutex, performed as the first statement of native `call`/`submit`. If the record is no longer `Attempting` (the attempt abandoned it, or cancel or close settled it), `claim` returns the retained terminal and the native call owns nothing. Correspondingly, the attempt's `abandon_attempt` settles only a record that is still `Attempting` and is a no-op otherwise, returning what the ledger holds. Whichever side transitions first wins; the other reads.
- **T3 gate send** (`submit` only): the `AcceptedOperation` value is sent through a one-shot channel to a worker thread that was spawned before acceptance and does nothing but receive and run. A failed send returns the value to the sender, which therefore still owns it and settles it (`abandon()`); a successful send makes the receiver the owner. Rust ownership makes this atomic without a flag.
- **T4 `settle`**: one transition to `Terminal` under the session mutex by the current owner, writing the terminal once.

Every other change of state is either an owner's own step (checkpoint, construction, registration, Git) or an intent.

### 3.4 Intents

An intent is a request from a non-owner that the state machine applies atomically under the session mutex according to the current phase. The intent either settles a record whose owner is passive, or is recorded for the active owner to decide.

| Intent | `Issued` / `Attempting` | `Admitting` | `Accepted` / `Finishing` | `Terminal` |
| --- | --- | --- | --- | --- |
| cancel | settle `Cancelled(NotRegistered)`; the passive owner learns at its next transition (a late `claim` reads the terminal, a late `abandon_attempt` is a no-op) | set `cancel_requested`, wake the owner's phase-1 select; the owner decides at its checkpoints | signal the stored `TransportCancellation` after locks release; the owner decides at finish | read the retained cleanup |
| close | as cancel, plus refuse new `reserve`, `begin_attempt` and `claim`; mark close-owed | as cancel | as cancel | read |
| release | `Issued`: discard; `Attempting`: `OpenOperation` | `OpenOperation` | `OpenOperation` | discard; a repeat reports `OperationExpired` |
| expiry (deferred timer stage) | discard after 15 minutes | not applicable (owner is active) | not applicable | discard after 15 minutes |

Timeouts are absent from the table because they are not intents (§3.5). Faults are handled by the owner that suffers them: a Python exception by the attempt's `finally`, a native unwind by the `Claim` or `AcceptedOperation` drop guard, a core error by the owner that received it.

### 3.5 Timers are signals, not actors

**An owner never waits on core without its own clock, and nothing but an owner acts on an operation or a generation.** Every core future awaited by a native owner (generation bootstrap, capacity admission, registered cleanup, request finish, runtime shutdown) is awaited under a timeout on the native session's own executor, whose timer is driven by that executor's worker thread and is independent of core's placement supervisor. When the core future completes first, the timer future is dropped and can never act. When the clock wins, the owner drops the core future, whose Drop guards are fail-closed (release leadership, seal registrations, close a partially mutated generation), and the owner settles conservatively with unconfirmed cleanup. The clock for each wait is core's own deadline for that step plus a one-second margin, so a healthy supervisor's driven deadline fires first and the owner's clock is only a backstop for a dead or hung supervisor. Core gains no timer, callback, or thread with authority over records or generations. This is the whole replacement for v3's watchdog, and R3 holds by construction: the only place a timeout and a completion meet is a `select` on the owner's own thread.

### 3.6 Generation ownership

Generation-level decisions follow the same rules. Constructing a generation is an owned step: the first admitting claim that finds no runtime becomes the constructing owner, builds the runtime and drives its bootstrap under its clock; other claims wait on the session condvar. Success installs the runtime as a session field; failure or timeout shuts the runtime down under the clock, discards it, refuses the constructing claim and every waiting claim with a `NotRegistered` refusal, and leaves the session `Open` so a later attempt may construct again; a panic sets `faulted`. An installed generation is closed only by `close()`, or fail-closed by an owner during its own transition (a phase-1 future dropped after pool mutation runs the existing `CapacityMutation` guard; an owner whose clock wins during finish or shutdown marks the session `faulted`). Core's internal closes on protocol errors and the supervisor-exit hardening in §6 are faults that reach owners as errors; they never write a terminal or decide an outcome.

## 4. Invariants

- **I1 One owner.** Every record has exactly one owner at every instant per §3.2; every change of owner is one of T1–T4.
- **I2 One record.** Every issued operation ID maps to exactly one session-owned `OperationRecord` holding all of that operation's state. No other map holds per-operation state.
- **I3 One terminal, one writer.** A record's terminal is written at most once, by `settle`, by the record's owner at that moment. Later attempts are ignored and, in test builds, counted as violations.
- **I4 Every owner settles.** A `Claim` and an `AcceptedOperation` cannot be dropped without settling; a Python attempt cannot exit without either transferring at T2 or calling `abandon_attempt`.
- **I5 Provenance is a variant.** Whether core registered the caller request ID is carried by which value core returned (`Refused`, `Consumed`, `Ready`) and, on unwind inside the synchronous registration step, by the owner's own progress record. No code path inspects `ModelError.code` or text to decide it, and Python keeps no consumed-ID allowlist.
- **I6 No Git before Accepted.** `Accepted` is written only after core returned `Ready` on an already bootstrapped generation and the worker slot is held; for `submit`, the worker exists and is parked at its gate, and the gate message is the only way it obtains the `TransportRequest`.
- **I7 Success only after finish.** Handler output is staged privately; `settle(Completed)` follows a returned `finish()`. A caught handler failure also awaits `finish()` before `Failed`. Only an unwind inside `finish()` or a lost accepted owner uses the drop fallback with unconfirmed cleanup.
- **I8 Only close writes Closing and Closed.** Faults set `faulted`, which refuses new issuance, attempts and claims; close still settles every remaining record.
- **I9 Intents never act outside the state machine.** Cancel, close and release are applied under the session mutex by phase; timeouts are consumed only by the waiting owner's select.
- **I10 Path isolation.** The legacy module-level path (`dispatch::submit`, `submit_accepted`, `spawn_call`, process-global `STORE`, `op_<request_id>`) and the session path share no recorder, store, worker wrapper or thread-local session hook.

## 5. Operation state machine

### 5.1 Phases and terminal kinds

| Phase | Meaning |
| --- | --- |
| `Issued` | ID minted synchronously; live request-ID claim; 4 KiB reservation (deferred ledger); no slot, endpoint or core work. |
| `Attempting` | A Python attempt owns the record; encoding and scheduling of the native call are in progress; no slot, endpoint or core work. |
| `Admitting` | A `Claim` owns the record and one of the eight top-level slots; decoding, placement, construction, capacity and registration run on the claiming thread. |
| `Accepted` | Registered and opened on a ready generation; generation and cancellation handle recorded; the accepted owner holds the `TransportRequest`; Git may run. |
| `Finishing` | Handler returned or was caught; `finish()` in progress under the owner's clock. |
| `Terminal(kind)` | Immutable outcome plus `CleanupReport`; readable until release or expiry; slot and claim released. |

| Terminal kind | Provenance | Effect | `request_id_consumed` | Retained payload |
| --- | --- | --- | --- | --- |
| `Refused(error)` | `NotRegistered` | none | false | typed `ModelError` |
| `Refused(error)` | `Registered` | none | true | typed `ModelError` and cleanup report |
| `Refused(error)` after an unwind inside registration | `MayHaveRegistered` | none | true (conservative; the generation is closed) | typed `InternalError`, unconfirmed cleanup |
| `Cancelled` before acceptance | `NotRegistered` or `Registered` | none | per provenance | typed `Cancelled` refusal; no `OperationResult` |
| `Cancelled` after acceptance | `Registered` | possible | true | failed `OperationResult` with `GwzErrorCode.cancelled=73` |
| `Failed` | `Registered` | possible | true | failed `OperationResult` |
| `Completed` | `Registered` | n/a | true | successful `OperationResult`, optional `MergeResponse` |

`Refused` and pre-acceptance `Cancelled` project to Python as the v2 typed `GwzBridgeError` with `effect="none"` and the `request_id_consumed` fact; `Failed` and post-acceptance `Cancelled` project as `GwzOperationError` with `effect="possible"`; `Completed` returns the result.

### 5.2 Legal transitions

| From | Event | Condition | To | Owner action |
| --- | --- | --- | --- | --- |
| — | `reserve(request_id)` | grammar valid; session `Open`, not `faulted`; no live claim for `request_id`; under 64 issued-unreleased records | `Issued` | mint serial; insert record and claim |
| `Issued` | T1 `begin_attempt(id)` | session `Open`, not `faulted` | `Attempting` | none (the attempt now owns) |
| `Issued` | `begin_attempt(id)` | session `Closing`/`Closed`/`faulted` | `Terminal(Refused, NotRegistered, InvalidRequest closed)` | settle |
| `Issued`, `Attempting` | T2 `claim(id)` | slot free; session `Open`, not `faulted` | `Admitting` | take slot; construct `Claim` |
| `Issued`, `Attempting` | `claim(id)` | slot full | `Terminal(Refused, NotRegistered, TransportSessionFull)` | settle; return typed error |
| `Issued`, `Attempting` | `claim(id)` | session `Closing`/`Closed`/`faulted` | `Terminal(Refused, NotRegistered, InvalidRequest closed)` | settle |
| `Attempting` | `abandon_attempt(id, error)` | — | `Terminal(Refused, NotRegistered, error)` | settle; return the retained disposition |
| any non-`Attempting` | `abandon_attempt(id, _)` | — | unchanged | no-op; return the retained disposition or phase |
| `Issued`, `Attempting` | cancel or close intent | — | `Terminal(Cancelled, NotRegistered)` | settle with default cleanup |
| `Issued` | release | — | discarded | remove claim; high-water mark unchanged |
| `Admitting` | `Claim::refuse(error)` | before `Ready` | `Terminal(Refused, NotRegistered)` | settle; release slot and claim |
| `Admitting` | core returns `Consumed(error, cleanup)` | — | `Terminal(Refused, Registered)` | await `cleanup.finish()` under the owner's clock; settle with its report (unconfirmed if the clock wins) |
| `Admitting` | `Ready(request)` then worker spawn failure | `submit` only | `Terminal(Refused, Registered, IoError)` | `request.cancel()`; await `finish()` under the clock; settle |
| `Admitting` | `cancel_requested` at checkpoint 1 | before phase 1 | `Terminal(Cancelled, NotRegistered)` | settle |
| `Admitting` | cancel signal during phase 1 | the owner's select drops the future; nothing registered | `Terminal(Cancelled, NotRegistered)` | settle; after pool mutation the core guard closed the generation |
| `Admitting` | owner clock wins during phase 1 | the future is dropped; nothing registered | `Terminal(Refused, NotRegistered, IoError timeout)` | settle; after pool mutation the core guard closed the generation |
| `Admitting` | `cancel_requested` at checkpoint 2 | after `Ready`, before `accept` | `Terminal(Cancelled, Registered)` | `request.cancel()`; await `finish()` under the clock; settle |
| `Admitting` | `Claim` dropped without `accept` or `refuse` (unwind) | — | `Terminal(Refused, provenance from the owner's progress record)` | seal any held request; `MayHaveRegistered` or `Registered` closes the generation; set `faulted`; settle with unconfirmed cleanup |
| `Admitting` | `Claim::accept(request)` | — | `Accepted` | store generation and handle; note a pending cancel and signal it after releasing locks; for `submit`, T3 send |
| `Accepted` | T3 send fails (worker gone before `recv`) | sender still owns | `Terminal(Failed, possible)` | `abandon()`: cancel, await `finish()` under the clock, settle; `submit` returns the typed error |
| `Accepted` | handler returns or is caught | — | `Finishing` | keep the request; stage only returned output; call `finish()` under the clock |
| `Finishing` | `finish()` returned | handler `Ok` | `Terminal(Completed)` | promote staged output; settle with report |
| `Finishing` | `finish()` returned | handler `Err` or caught unwind | `Terminal(Failed, possible)` | settle with report |
| `Finishing` | `finish()` returned | cancellation signalled and no `Completed` output | `Terminal(Cancelled, possible)` | settle with report |
| `Finishing` | owner clock wins during `finish()` | finish future dropped | `Terminal(Failed or Cancelled, possible, cleanup unconfirmed)` | settle; set `faulted` |
| `Accepted`, `Finishing` | `AcceptedOperation` dropped without settlement (unwind in `finish()`, lost owner) | — | `Terminal(Failed, possible, cleanup unconfirmed)` | drop guard settles; seal request; set `faulted` |
| `Accepted`, `Finishing` | cancel or close intent | — | unchanged | signal handle after locks release; owner decides at finish |
| `Terminal` | cancel | — | unchanged | return retained cleanup |
| `Terminal` | release | — | discarded | a repeat reports `OperationExpired` |
| `Attempting`, `Admitting`, `Accepted`, `Finishing` | release | — | unchanged | refuse `OpenOperation` |

Illegal observations (an `Admitting` record with no live `Claim`, an `Accepted` record with no `AcceptedOperation` holder, a second terminal write, a `TransportRequest` outside a `Claim` or `AcceptedOperation`) are programming errors: test builds assert; release builds fail closed by settling `Failed` and setting `faulted`.

### 5.3 Ownership values

- `Attempt` (Python) — a scope in `_admit` (§8) that begins at T1 and ends at T2 or at `abandon_attempt`. It is not a native object; its proof is the phase `Attempting` plus the bridge's `finally`.
- `Claim` (native) — `Drop`-guarded, non-`Clone`; exposes `refuse`, `refuse_registered`, `accept(TransportRequest) -> AcceptedOperation`, `cancel_requested()`, and holds the `Progress` record of the registration step for its unwind path. It is the sole holder of the top-level slot before acceptance.
- `AcceptedOperation` (native) — owns the `TransportRequest`, the slot, the record handle and the staging recorder. `run(dispatch)` catches handler unwind, awaits `finish()` under the clock and settles; `abandon()` settles after cancel and finish when a T3 send fails; `Drop` settles `Failed(possible, unconfirmed)` only if neither ran to settlement.
- `Admission`, `RegisteredCleanup`, `TransportRequest` (core) — resources held by the native owner; each has a fail-closed `Drop` (release leadership; seal registrations; cancel and seal the request).
- `Terminal` — immutable once written; the sole source for `result`, `try_result`, event completion, cancel reports, descriptors and close summaries.

## 6. Generation lifecycle and core admission API

### 6.1 Bootstrap at construction

Today the first caller request bootstraps the mux: its `begin()` sends the Bind offer and its `ready().await` waits for the endpoint's Bound reply, so the first operation's admission contains an asynchronous, generation-level wait that a per-operation owner cannot bound without a second decider. V4 moves bootstrap into generation construction, owned by the constructing claim (§3.6):

```rust
impl TransportRuntime {
    /// Idempotent. Registers the generation's reserved bootstrap request on the
    /// local endpoint and driver sessions, sends the Bind offer, awaits Ready,
    /// then cancels, seals and finishes that registration. Dropping the future
    /// before Ready disconnects the pending bootstrap (existing mux behavior)
    /// and leaves the runtime unusable, which the owner then shuts down.
    pub async fn bootstrap(&self) -> ModelResult<()>;
}
```

The bootstrap registration uses a reserved internal ID (`bootstrap-<generation-serial>`) with the internal operation name `bootstrap`; it is never a caller request ID, never appears in results, and is reserved beyond the 256 caller registrations (the core and mux per-generation limits are raised by exactly one). Because the mux disconnects when its Binding offer's request is cancelled, the bootstrap registration is cancelled and sealed only after `Ready`; retiring it then merely tombstones it. The constructing owner awaits `bootstrap()` in two clocked steps (Ready, then the registration's finish), each bounded by the mux's own 5-second deadline plus the one-second margin. On failure or an expired clock the owner drops the future, awaits `shutdown()` under the clock, and discards the runtime. Legacy `request()` callers that never call `bootstrap()` keep bootstrapping through their first request exactly as today; after `bootstrap()`, their `begin()` and `ready()` return immediately.

### 6.2 Phase 1: capacity, cancellable, registers nothing

```rust
/// Holds local admission leadership and the installed capacity epoch; nothing
/// is registered. Dropping it releases leadership and consumes nothing.
pub struct Admission { /* leader guard, resolved capacity, progress */ }
impl TransportRuntime {
    /// Validation, local placement, capacity install and the read-only
    /// registration pre-checks. Cancellable by dropping the future.
    pub async fn admit_local(&self, meta: RequestMeta, operation_id: String) -> Result<Admission, ModelError>;
}
```

`admit_local` performs, serialized by the existing local admission leader with the existing single arrival deadline: `validate_meta` and the runtime-closed check; refusal of explicit CLI placement with `UnsupportedOperation` before any endpoint construction; endpoint admission exactly as today's `admit_client_request` up to and including `install_capacity` with its `CapacityMutation` guard; the read-only duplicate and exhaustion pre-checks of both the endpoint and driver `used` sets and mux tombstone counts; and a check that the driver mux is `Ready` (bootstrapped and not closed). Any failure returns `Err` with nothing registered. The returned `Admission` keeps leadership so no local registration can interleave before phase 2. The native owner awaits it under a `select` against the record's cancel signal and its clock (§3.5); dropping it before pool mutation consumes nothing, and dropping it after pool mutation runs the existing guard that closes the endpoint generation. Capacity semantics are unchanged from v2 §4.

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
impl Admission {
    pub fn progress(&self) -> Progress;            // NotEntered | MayHaveRegistered | Registered
    /// Synchronous: no await, no timer, nothing to cancel.
    pub fn register_and_open(self) -> Admitted;
}
```

With the generation already bootstrapped, every step of phase 2 is synchronous: endpoint registration (`ClientRequest::new`), driver registration (`RequestContext::new`), `begin()` on a `Ready` mux, and backend attachment. A refusal proved to have inserted nothing (the endpoint mux refusing before its insert) returns `Refused`. A failure after either insert (driver-side refusal, mux closed between the pre-check and `begin`) cancels and seals what was registered and returns `Consumed` with a `RegisteredCleanup`; the owner awaits its `finish()` under the owner's clock. Success returns `Ready`. The `Progress` record is the owner's own bookkeeping, written on the owner's thread in program order: `MayHaveRegistered` immediately before entering any mux `register` that can insert a tombstone, `NotEntered` restored only on a proved no-insert refusal, `Registered` after a proved insertion. The `Claim` reads it only if `register_and_open` unwinds (a hidden panic path in the sense of L2-15): `MayHaveRegistered` or `Registered` closes the generation and settles `request_id_consumed=true`; `NotEntered` settles `NotRegistered`. Because phase 2 has no await, there is nothing for a cancel or a timer to interrupt, no watchdog, and no "not cancellable" clause; a cancel requested during phase 2 is applied at checkpoint 2.

`TransportRequest` gains `generation(&self) -> u64` so the native record can pin cancellation authority now. Supervisor hardening: the placement supervisor thread wraps `drive()` in `catch_unwind` and, on unwind or loop exit while the session is open, closes the session and signals its waiters so `ready()`, `finish()` and capacity waiters wake with `Closed` promptly instead of at their owner's clock. Existing `request()` is unchanged, including its CLI branch, droppable future and error and cleanup timing.

## 7. Native session structure

### 7.1 Record and session

```rust
struct OperationRecord {
    operation_id: String, request_id: String,
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
    claims: HashMap<String, String>,                  // live request_id -> operation_id, non-terminal only
    slots: u8,                                         // Admitting + Accepted + Finishing, at most 8
    status: Open | Closing | Closed, faulted: bool, constructing: bool,
    runtime: Option<Arc<TransportRuntime>>,           // installed only after bootstrap()
    close_report: Option<CloseReport>, close_summaries: Vec<CompactSummary>,
    next_serial: u64, nonce: [u8; 16], executor: tokio::runtime::Runtime,
}
```

The v2-candidate `admitting`, `active`, `last_cancel` and `request_operations` maps, `AdmissionFailure`, `end_admission`, `complete_failed_admission`, `abandon_unstarted`, `worker_panicked`, `spawned_call`, `CURRENT_SESSION`, `defer_terminal`, `publish_terminal` and `pending_terminal` are deleted. The `OperationStore` keeps `issue`, `discard`, `contains`, `events`, `wait_events`, `result`, `try_result` and `merge_response`; its record gains `settle` and the private `staged` slot. `OperationRecorder::finish`, `finish_merge` and `finish_model_error` write to `staged` in a session-owned store and publish directly in the legacy module store, so shared handler code is unchanged.

`claims` holds one entry per non-terminal record; `settle` removes it regardless of provenance, and core decides at the next `admit_local` whether the ID may register again. The deferred ledger stage attaches at `reserve` (4 KiB), `Claim::accept` (upgrade to 8 MiB and the primary reader cursor, before the gate opens) and `settle` (exchange for charged bytes); the deferred handle stage calls `begin_attempt`/`claim` through the same `_admit` route as every other form.

### 7.2 Entry points and the admission sequence

`begin_attempt(id)` and `abandon_attempt(id, code, message) -> Disposition` are new native methods called by the bridge under the GIL released; both are single transitions (§5.2). `Disposition` is the retained terminal's `(code, request_id_consumed, effect)` or the current phase, so the bridge raises exactly what the ledger holds.

Both `call(.., operation_id)` and `submit(.., operation_id)` do, as the **first statement inside `py.detach`**, `let claim = self.claim(operation_id)?;`, which validates nonce and serial, returns the retained terminal if the record is terminal, refuses `InvalidRequest` if an admission is already running, takes a slot or settles `TransportSessionFull`, and moves the record to `Admitting`. Only then is `current_dir()` captured and the request decoded. From this point every exit passes through `claim.refuse`, `claim.refuse_registered` or the guard's drop.

Shared admission, in order: decode `network_meta` (a non-network method with an issued ID refuses `InvalidRequest`); `validate_request_context`; explicit CLI placement refusal before construction; **checkpoint 1** (`cancel_requested` → `Cancelled(NotRegistered)`); **construction** if no runtime is installed (`from_environment()` inside `catch_unwind`, then `bootstrap()` under the owner's clock; failure or timeout discards the runtime and refuses `NotRegistered`; a panic also sets `faulted`; other claims waiting on `constructing` refuse through their own guards; `Issued` and `Attempting` records are untouched); **phase 1** `admit_local` under `select` against the record's cancel signal and the owner's clock (a drop registers nothing); **phase 2** `register_and_open` called synchronously inside `catch_unwind`, with `admission.progress()` attached to the `Claim` first; `Refused` → `claim.refuse`; `Consumed` → await `cleanup.finish()` under the clock, then `claim.refuse_registered`; `Ready(request)` → **checkpoint 2** (`cancel_requested` → cancel, finish under the clock, `Cancelled(Registered)`); for `submit`, spawn the parked worker (spawn failure → cancel, finish, `refuse_registered(IoError)`); then `claim.accept(request)`.

`call` runs `accepted.run(dispatch)` inline on the claiming thread and returns the response bytes. `submit` sends `accepted` through the gate (T3) and returns the `Accepted` envelope built by the session; the legacy `submit_accepted` is not on this path. `run` catches unwind around the handler only, keeps a successful output local, retains the `TransportRequest` in all cases, moves to `Finishing`, awaits `finish()` under the clock, then settles and releases the slot; a caught handler panic yields `Failed(InternalError, possible)` after finish and sets `faulted`; a clock win during finish settles conservatively and sets `faulted`.

### 7.3 Worker gate

The gate is a one-shot channel whose message is the `AcceptedOperation`. The worker thread is spawned before `accept`; its body is `recv` followed by `run`, with nothing between. `accept` writes `Accepted` under the session mutex, then sends. A failed send returns the value to the sender, which calls `abandon()` (§5.2). A receiver dropped after `recv` held the value, so its drop settled. A sender that unwinds before `send` still holds either the `Claim` (before `accept`) or the `AcceptedOperation` (after), whose drop settles; the parked worker sees channel closure and exits without Git work.

### 7.4 Locking model

Two mutex kinds, one order. The **session mutex** guards `status`, `faulted`, `constructing`, `records`, `claims`, `slots`, `next_serial`, `close_summaries` and every record's phase-bearing fields (`phase`, `close_owed`, `cancel_requested`, `cancellation`, `generation`). Each **record mutex** guards only that record's payload (`terminal`, `staged`, `events`) and its condvar. Session before record, never the reverse. Readers and handler event appends take only the record mutex. Every transition and intent (`reserve`, `begin_attempt`, `abandon_attempt`, `claim`, `accept`, `settle`, cancel, close, release) is one function entered with no lock held that takes the session mutex, validates phase against intent, updates phase, slots, claims and owed summaries, then takes the record mutex to write the terminal, releases it, notifies both condvars and releases the session mutex. No core, Git or Python call runs under either mutex; `accept` notes a pending cancel under the session mutex and signals the handle after release, and a concurrent cancel that observes `Accepted` signals the same idempotent handle itself. Drop guards run in frames that hold neither mutex.

### 7.5 The owner's clock in native code

Every await of a core future by an owner is `tokio::time::timeout(bound, future)` on the session's executor, optionally combined with the record's cancel signal in a `select` for phase 1. Bounds: bootstrap Ready 6 s and bootstrap finish 6 s; capacity 6 s; registered cleanup and request finish 6 s; runtime shutdown 6 s. The executor is the existing multi-thread tokio runtime with one worker thread and `enable_all()`; its timer is driven by that worker thread, never by core's supervisor. A clock win drops the core future and settles conservatively. No other timer exists in the session or in core.

## 8. The Python owner

The bridge is the owner of every admission attempt from T1 until T2. All three forms (explicit `accepted()`, implicit unary `call`, and a stream helper's first iteration) use one method:

```python
async def _admit(self, native_entry, method, names, request, operation_id):
    self._session.begin_attempt(operation_id)              # T1: raises the retained refusal if terminal
    try:
        request_bytes = encode_message(...)                # may raise before native entry
        worker = asyncio.create_task(asyncio.to_thread(native_entry, method, *names, request_bytes, operation_id))
        return await asyncio.shield(worker)                 # native claim (T2) happens inside the worker
    except asyncio.CancelledError:
        ... existing path: cancel_operation(operation_id), await worker completion, re-raise ...
    except BaseException as exc:
        typed = self._typed_pre_entry_failure(exc)          # IoError (executor/scheduling), InvalidRequest (encoding), else InternalError
        disposition = self._session.abandon_attempt(operation_id, typed.code, typed.message)
        raise self._bridge_error_from(disposition, operation_id) from exc
```

`abandon_attempt` settles the record only if it is still `Attempting` (native never claimed) and otherwise returns what the ledger already holds, so the raised error always matches the retained terminal and no second terminal can arise from a racing native worker. Executor shutdown surfaces as an exception from the awaited task and takes this path; a task cancelled while the native call is still queued takes the cancel path, whose `cancel_operation` settles `Attempting → Cancelled` and whose late native `claim` then reads that terminal without minting. A Python failure after native has completed (for example, decoding the response) finds a `Completed` or `Failed` terminal; the bridge raises its own error carrying `operation_id` so the caller can still read the result. Implicit forms expose the issued `operation_id` on the raised error as today; the retained refusal follows v2 §6 retention rules.

Concurrent `accepted()` calls on one handle are serialized by a per-handle `asyncio.Lock` so the second observes the first's outcome (v2 §2). The bridge's `_issued_requests` cache remains a convenience and is never consulted for an admission decision. Every pre-effect refusal raised to Python carries `request_id_consumed` (§11 item 2).

## 9. Intents in detail

`cancel(id)` takes the session mutex first and acts by phase per §3.4: `Issued`/`Attempting` settle `Cancelled(NotRegistered)`; `Admitting` sets `cancel_requested` and wakes the owner's phase-1 select, then waits on the record condvar for the terminal; `Accepted`/`Finishing` copies the stored handle, releases both locks, signals it idempotently and waits; `Terminal` reads the retained cleanup. Repeated cancellation returns the same cleanup (v2 §6).

`close()` moves `Open → Closing` under the session mutex, marks records then `Admitting`, `Accepted` or `Finishing` as close-owed (at most eight), applies the cancel intent to every live record by phase, and waits on the session condvar until `slots == 0` and `constructing == false`; I4 and the owner's clock make that wait finite. It then shuts the runtime down once under the closing owner's clock (or reads a losing constructor's cleanup), combines physical facts with the owed summaries, and writes `Closed`. Concurrent and repeated closes join and return the same report. A `faulted` session reports `peer_cleanup_confirmed = false` and `pending_local_work ≥ 1`. A `Closing` session refuses `reserve`, `begin_attempt` and `claim`; cancel, release, result and event reads keep working on the ledger after close (v2 §6).

Timeouts never appear here: they are consumed by owners (§3.5). Faults are settled by the owner that suffers them (§3.4).

## 10. Terminal publication, readers and close summaries

`settle` (§7.4) writes the terminal once; readers wait on the record condvar. `result()` returns `Completed`, `Failed` and post-acceptance `Cancelled` results and raises the typed refusal for `Refused` and pre-acceptance `Cancelled`; `try_result` returns `None` until a terminal exists; `wait_events` completes exactly when a terminal exists, and a terminal event precedes iterator end for accepted operations; `merge_response` is served only from a `Completed` terminal's promoted payload. No reader can observe staged output; promotion is a step inside `settle(Completed)` and nothing else performs it. If `close_owed` is set, `settle` copies the compact status, code and effect into `close_summaries` before releasing the session mutex, so a later `release` can discard the full result but never an owed summary.

## 11. Bounded amendments to the v2 contract

1. **Registration is defined.** The caller request ID is consumed for the current generation when core reports `Consumed` or `Ready`, or when an unwind inside the synchronous registration step leaves insertion indeterminate and closes the generation; it is not consumed when core reports `Refused`. Both endpoint and driver registrations are one core step; a partial registration counts as consumed.
2. **The retry disposition is exposed, not inferred.** Every pre-effect refusal raised to Python (`GwzBridgeError` with `effect="none"`, including `GwzOperationCancelled` before acceptance) carries `request_id_consumed: bool`. `false` proves no registration and permits reuse only while the generation remains open and the refusal's cause has cleared; `true` means registration occurred or could not be ruled out, and the ID must not be reused in that generation. `recent_operations()` descriptors (deferred) carry the same field.
3. **`Accepted` for `submit()` is produced by the session**, not by the legacy `submit_accepted`.
4. **Inline `call()` is an accepted worker form.** The bounded claiming thread owns the accepted scope for `call()` after the same prerequisites; `submit()` and stream helpers keep the spawned parked worker and gate. This supersedes only v2 §5's spawn-and-gate wording for `call()`.
5. **Finish-unwind and lost-owner terminals.** v2 §5's rule that a worker panic publishes its terminal after `TransportRequest.finish()` holds for every caught handler failure. If `finish()` itself unwinds, the accepted owner is lost outside the handler catch, or the owner's clock wins during finish, the terminal is published without a returned finish report, as `Failed` (or `Cancelled` if cancellation had been signalled) with `effect="possible"`, `pending_local_work ≥ 1` and `peer_cleanup_confirmed = false`, and the session is `faulted` so close drains it.
6. **Generation bootstrap.** A generation is bootstrapped at construction through one reserved internal registration that is not a caller request ID, is never reported, and does not count against the 256 caller registrations of v2 §5. Construction includes that in-process bootstrap; it still performs no Git-host connection and no credential access.

Nothing else in v2 §§2–8 changes.

## 12. Fault matrix

Each row is a required closure case for both `call` and `submit` unless marked. Columns: terminal kind / provenance, effect, request ID retryable in this generation, core registration, Git effect, waiters woken, slot and claim released.

| Fault point | Terminal | Effect | Retry same ID | Core reg. | Git | Waiters | Slot/claim |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Executor shut down before native entry (bridge) | Refused / NotRegistered, `IoError` via `abandon_attempt` | none | yes | no | no | yes | never taken; claim released |
| Request encoding fails after issuance (bridge) | Refused / NotRegistered, `InvalidRequest` via `abandon_attempt` | none | yes | no | no | yes | never taken; claim released |
| Bridge exception after native claimed | unchanged; `abandon_attempt` is a no-op and returns the native disposition | per native terminal | per native terminal | per native terminal | per native terminal | yes | per native terminal |
| Cancel while `Attempting` (blocked executor) | Cancelled / NotRegistered; late `claim` reads it, no remint | none | yes | no | no | yes | never taken |
| `current_dir()` fails after claim | Refused / NotRegistered, `IoError` | none | yes | no | no | yes | released |
| Malformed request CBOR; wrong message name (`submit`); issued ID on a non-network method | Refused / NotRegistered, `InvalidRequest` | none | yes | no | no | yes | released |
| Explicit CLI placement, HOME present or absent | Refused / NotRegistered, `UnsupportedOperation` | none | yes | no | no | yes | released; no endpoint construction |
| Session `Closing`/`Closed`/`faulted` at `begin_attempt` or `claim` | Refused / NotRegistered, `InvalidRequest` | none | n/a | no | no | yes | never taken |
| Ninth top-level operation | Refused / NotRegistered, `TransportSessionFull` | none | yes | no | no | yes | slot never taken; claim released |
| 65th issued record | synchronous `TransportSessionFull`, no record | none | yes | no | no | n/a | n/a |
| Endpoint construction error (HOME unset) | Refused / NotRegistered, `InvalidRequest`; session stays `Open` | none | yes | no | no | yes | released |
| Bootstrap fails or the constructing owner's clock wins | Refused / NotRegistered, `IoError`; runtime shut down under the clock and discarded; waiting claims refuse likewise; session stays `Open` | none | yes, on a later construction | no (bootstrap ID only) | no | yes | released |
| Construction panic | Refused / NotRegistered, `InternalError`; `faulted`; sibling `Issued`/`Attempting` untouched | none | closed session | no | no | yes | released |
| Capacity conflict | Refused / NotRegistered, `TransportCapacityConflict` | none | yes, after the conflict clears | no | no | yes | released |
| Leadership or capacity-wait timeout before pool mutation | Refused / NotRegistered, `IoError` | none | **yes** | no | no | yes | released |
| Retirement timeout after pool mutation | Refused / NotRegistered, `IoError`; the core guard closes the generation | none | **no**, only in a later generation | no | no | yes | released |
| Owner clock wins during phase 1 | as the two rows above by mutation state | none | as above | no | no | yes | released |
| Cancel during phase 1 | Cancelled / NotRegistered; after pool mutation the generation closes | none | yes while the generation is open | no | no | yes | released |
| Driver mux not `Ready` at the phase-1 pre-check | Refused / NotRegistered, `IoError` | none | yes on a later generation | no | no | yes | released |
| Duplicate request ID in generation | Refused / NotRegistered, `InvalidRequest` | none | no (consumed earlier) | no | no | yes | released |
| Endpoint mux refuses before insert | Refused / NotRegistered, typed core error | none | yes | no | no | yes | released |
| Driver registration or `begin()` fails after the endpoint insert | Refused / Registered, typed core error; cleanup finished under the clock | none | **no** | consumed | no | yes | released |
| Unwind inside `register_and_open` after `MayHaveRegistered` | Refused / MayHaveRegistered, `InternalError`; generation closed; `faulted` | none | no | treated consumed | no | yes | released; cleanup unconfirmed |
| Cancel at checkpoint 1 | Cancelled / NotRegistered | none | yes | no | no | yes | released |
| Cancel requested during phase 2 | applied at checkpoint 2 (phase 2 is synchronous) | none | no | consumed | no | yes | released after finish |
| Cancel at checkpoint 2 | Cancelled / Registered | none | no | consumed | no | yes | released after finish |
| Worker thread spawn failure (`submit`) | Refused / Registered, `IoError` | none | no | consumed | no | yes | released after finish |
| T3 send fails (worker gone before `recv`) | Failed / Registered, `InternalError`, via `abandon()` | possible (conservative; no Git ran) | no | consumed | no | yes | released after finish |
| Panic between spawn and `accept` | Refused / Registered via `Claim` drop; worker exits idle | none | no | consumed | no | yes | released; cleanup unconfirmed |
| Cancel after acceptance, completion loses | Cancelled / Registered, code 73 | possible | no | consumed | possible | yes | released after finish |
| Cancel after acceptance, completion wins | Completed | n/a | no | consumed | yes | yes | released |
| Handler error | Failed / Registered | possible | no | consumed | possible | yes | released after finish |
| Handler panic caught inside `run` | Failed / Registered, `InternalError`; `faulted` | possible | no | consumed | possible | yes, after `finish()` | released after finish |
| Owner clock wins during `finish()`; supervisor dead | Failed or Cancelled / Registered; cleanup unconfirmed; `faulted` | possible | no | consumed | possible | yes | released |
| `finish()` unwinds or the accepted owner is lost | Failed / Registered via `AcceptedOperation` drop; cleanup unconfirmed; `faulted` | possible | no | consumed | possible | yes | released |
| Placement supervisor exits while an operation runs | the owner observes `Closed` errors or its clock; one of the rows above | per row | no | consumed | possible | yes | released |
| Close while `Issued`/`Attempting` | Cancelled / NotRegistered | none | closed | no | no | yes | n/a |
| Close while `Admitting` | one of the cancel rows above | per row | closed | per row | no | yes | released |
| Close while `Accepted` | Cancelled or Completed as the race decides; summary in the close report | per kind | closed | consumed | possible | yes | released |
| Release after an owed terminal but before the close report commits | full result discarded; owed summary retained | per terminal | closed | consumed | possible | yes | released |
| Release while `Attempting`/`Admitting`/`Accepted`/`Finishing` | unchanged; `OpenOperation` | — | — | — | — | — | held |

## 13. Required proof and gates

Design review first: dual peer-blind Consistency and Safety review of this document with the v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` and the caller-guide note, plus a docs-only Surface check of that note and its completed example, all in one committed tuple. Any P0–P2 keeps implementation stopped. Implementation review then uses fresh Code and State reviewers on a new exact tuple.

Focused closure tests, typed-field assertions only:

1. **Ownership before native entry.** Shut down the loop's default executor, then attempt explicit handle admission and implicit `call`/`submit` on issued IDs: one typed retained terminal per ID matching the raised error, `request_id_consumed == false`, zero slots and claims, no record-limit growth after repeated attempts. Inject an encoding failure for an issued handle: same assertions with `InvalidRequest`. Race `abandon_attempt` against a native `claim` that already ran: exactly one terminal, and the bridge raises the native disposition.
2. **Atomic handoffs.** Block the default executor, issue, schedule, cancel, release: one serial, no remint, `Cancelled/NotRegistered`, late native entry reads the terminal (existing test extended to `begin_attempt`). Inject a worker thread that exits before `recv`: `abandon()` path, one `Failed` terminal, `submit` returns the typed error, finish completed, no Git. Panic after `recv`: drop settlement. Panic between spawn and `accept`: `Claim` drop settlement.
3. **Provenance without text.** Hold the capacity gate past the deadline: phase-1 `Err`, `request_id_consumed == false`, same ID admitted after release. Inject a driver-side refusal after the endpoint insert and a `begin()` failure on a closed mux: `Consumed`, `request_id_consumed == true`, cleanup finished under the clock, same ID refused afterwards. Inject an unwind inside `register_and_open` after `MayHaveRegistered`: generation closed, terminal never reports `false`. Both `IoError` cases are distinguished only by variant.
4. **One arbiter.** Hold the endpoint's Bound reply so the constructing owner's clock wins: runtime discarded, all waiting claims refused `NotRegistered`, session still `Open`, a later attempt constructs successfully. Let Bound arrive just before the clock: the timer future is dropped and no later close occurs (assert no close within twice the bound). Stop the placement supervisor after registration while an operation runs: the owner's finish clock wins, one conservative terminal, cancel and close terminate within the bound, `faulted` refuses later attempts.
5. **Every pre-worker exit settles, both forms.** Parametrize the §12 rows above acceptance over `call` and `submit`: one terminal; returned error and `operation_result(id)` agree; `wait_events` completes; slot and claim released; no registration except where the row says consumed; no Git; release succeeds; repeat past 64 records and admit afterwards.
6. **Terminal uniqueness under faults.** Stage a success and panic in `finish()`; panic in the handler while finish is held (waiters pend until finish returns); drop the `Claim` before accept and an `AcceptedOperation` before send. One terminal each; no success ever visible.
7. **Intents in every phase.** Cancel and close at each phase including `Attempting`; release refusals; close idempotence; owed summaries surviving release; post-close ledger reads; refused new work.
8. **Legacy preservation and isolation.** Existing `request()` and `transport_host` tests unchanged; a legacy first request still bootstraps; after `bootstrap()` a legacy request's `begin`/`ready` return immediately; CLI registration, duplicate refusal and capacity via `request()`; the legacy module store and the session store never resolve each other's IDs.
9. **Lock order.** A debug-build assertion that the session mutex is never acquired while a record mutex is held and that no core or Python call runs under either, enabled in the focused native tests.

The deferred stages and their gates (ledger bytes and timer, public handles, rollover, transport stress, platform, wheel and source pins) remain separate and are not waived by a foundation GO.

## 14. Out of scope, unknowns and risks

- `TransportGenerationBusy` and `TransportRecordLimit` model codes belong to the rollover and ledger stages; the record's `generation` field and `staged` slot are laid down now.
- After a generation is closed by a fail-closed decision, this Client cannot admit network work until the deferred rollover stage exists; `faulted` makes that explicit and close drains it. A failed or timed-out bootstrap does not close the session, so a later attempt may construct again.
- The reserved bootstrap registration raises the core and mux per-generation limits by one; if a reviewer finds a mux path where a tombstoned bootstrap request could be confused with a caller request, the correction is a distinct internal namespace, not a caller-visible change.
- `Consumed` from a driver-side failure after the endpoint insert is expected to be rare once the pre-checks and the `Ready` check exist; it is kept so no caller ever infers.
- The owner's clocks add at most one second beyond core's own deadlines and only matter when the supervisor is dead; a healthy supervisor's driven deadlines fire first.
- Blocking the claiming thread through construction, phase 1 and phase 2 is unchanged in shape from the current `submit`, which already blocks until registration; Python runs both forms under `asyncio.to_thread`.
