# Python transport session v3 foundation — admission provenance and settlement design

Date: 2026-09-24. Status: **DRAFT v3 foundation design (a transition design under AgentProcessRules §7.1); dual Consistency/Safety design review is required before implementation restarts; this document carries no implementation, build, or activation authority.** It describes the candidate at root `c6da910ae46b32e6673f75faf84cd294908ff0c4` (the [third verdict](GwzPyTransportSessionV2Foundation-Verdict-3.md) commit), gwz-core `351a5c565783c53a4e9ae702fe99a699c55299fd`, and gwz-py `20b86187c766079a3a85d67a3d4d58d7edb73d41`. The dirty root checkpoint file and the untracked SSH N2b prompt and route-mapping drafts are unrelated and out of scope.

## 1. Decision, boundary and precedence

The operator directed a new foundation proposal after the third verdict stopped the v2 foundation lane at its third architectural root cause. This document is that object. Accepting the known defects is not chosen: it would waive the same-ID retry guarantee after a capacity timeout and leave result waiters able to hang or read an error that disagrees with the returned one.

The accepted [v2 contract](GwzPyTransportSessionV2Design.md) remains the caller-facing contract for identity, admission meaning, capacity, outcome ledger, close and typed terminals. V3 replaces the **foundation architecture** underneath it: the native session's record, admission and settlement structure in `gwz-py/native/src/transport_session.rs`, `operations.rs` and `dispatch/mod.rs`, and the core admission API in `gwz-core/src/transport_host/{mod,session,request}.rs` that the session depends on. The [v2 implementation checkpoint](GwzPyTransportSessionV2ImplementationCheckpoint.md) describes the rejected candidate structure; V3 supersedes that structure, not the contract. V3 amends the v2 contract in exactly the three places listed in §10. The Taut request/response method set, the `gwz-transport` envelope and the appended `cancelled=73` and `transport_record_limit=74` codes are unchanged. The deferred stages (full byte ledger, timed expiry, public `start_*` handles and `OperationStream`, generation rollover, SSH/HTTPS and cross-loop stress, platform gates) stay deferred; V3 fixes the record and admission shape they must build on so they do not restructure it again.

Core requirements and design must be amended before core behavior changes, as `gwz-core/AGENTS.md` requires. The v2 paragraphs at the top of `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` are followed in this tuple by draft v3 foundation paragraphs stating the §6 core API; they are marked pending this review and are not authority until GO.

## 2. Evaluation of the third-verdict proposal

The verdict's bounded proposal is correct in direction and insufficient as stated.

What it gets right. Both reviewers converged blind on the same boundary: an issued ID must have exactly one terminal disposition, the returned admission error must agree with the retained record, and Python must not infer core registration from `ModelError.code` or display text. The proposal keeps the Taut method set and the transport envelope unchanged, and it names the two closure tests that matter: pre- versus post-registration `IoError` distinguished without parsing, and both `call()` and `submit()` terminating the issued record on every pre-worker exit.

Why it would likely fail a fourth round as written:

1. **A provenance flag on an error keeps Python as the interpreter of core state.** "Registered" is itself ambiguous in the current core. `TransportRuntime::request` registers **twice**: `ClientRequest::new` inserts the caller ID into the local endpoint session's `used` set, then `RequestContext::new` inserts it into the driver session's `used` set. Each set has its own 256 lifetime limit and mirrors a mux tombstone set (`gwz-transport/src/mux/mod.rs`, `max_requests` default 256). A failure between the two inserts is "registered" on one session and not the other. A boolean cannot say which, and the same-ID retry rule depends on it.
2. **"Centralize settlement across every exit" by enumeration is the audit that already failed three times.** Rounds one to three each closed an enumerated exit and each found another: eight-slot refusal (Code P2-2), spawn failure (State P2-3), deferred `submit()` failure and pre-spawn message-name failure (Code P2-8), current-directory and decode failure (State P2-9). The exits multiply because `submit()` performs admission **on the spawned worker thread** and reports failure back through a channel with `defer_failure`, while `spawn_call`'s outer wrapper records a second, generic failure. Enumerating exits leaves that structure in place.
3. **It does not remove the two-step terminal.** `defer_terminal`, `pending_terminal` and `publish_terminal` let a handler's staged success sit in the record until a later publisher promotes it. State P2-2 and P2-5 were both instances of the wrong party publishing. The proposal's "retain a typed refusal before waking any waiter" is a symptom rule for the same cause.
4. **It does not collapse the five state holders.** `admitting`, `active`, `last_cancel`, `request_operations` and the `OperationStore` record each hold part of one operation's state, with hand-maintained agreement between them. "Exactly one terminal" cannot be proved over five maps written from seven sites.
5. **It does not bind the ID before the earliest exits.** The `call` and `submit` `#[pymethods]` capture `std::env::current_dir()` before any settlement object exists, and `network_meta` decoding runs before the ID is bound. No settlement rule inside the session can reach those exits unless the ID is claimed first.

V3 therefore replaces structure, not rules: one record with a closed phase machine, a claim guard that makes terminality a property of the type system, a core admission result whose variants **are** the provenance, admission on the caller thread with a worker that only ever runs accepted work, and a single terminal writer.

## 3. Structural causes in the rejected candidate

- `submit()` (`transport_session.rs`) inserts an `Admitting` entry, calls `dispatch::submit`, which calls legacy `submit_accepted` and `spawn_call`; the spawned thread calls `spawned_call` → `call_inner(defer_failure = true)`, which performs endpoint construction, placement checks, core admission and registration, then signals the submitter through an `mpsc` channel. Failure before `Accepted` is stored in `Admitting.failure`, later surfaced by `complete_failed_admission`, while the thread's outer `catch_unwind` wrapper independently calls `recorder.finish_error`. Two writers, one record.
- `call_inner` has eleven distinct returns between `operation_for_request` and the `active` insert, each with its own combination of `release_request_mapping`, `end_admission`, `refuse` and `last_cancel` updates.
- `call_inner` removes `request_operations[request_id]` only for `TransportCapacityConflict`, `InvalidRequest` and `UnsupportedOperation` errors. That allowlist is the State P2-8 root cause; `IoError` is returned both by the pre-registration capacity gate and by post-registration `begin()`/`ready()` failures.
- `OperationRecord` accepts a handler result into `pending_terminal` during dispatch and publishes it later; `worker_panicked` and `finish_after_dispatch` had to be taught not to promote it.
- `network_meta` failure calls `abandon_unstarted(None)`, which cannot identify the reserved record, and the pymethods' `current_dir()` failure returns before any session code runs.

## 4. Invariants

- **I1 One record.** Every issued operation ID maps to exactly one session-owned `OperationRecord`, which holds all of that operation's state: phase, terminal, cleanup, cancel request, cancellation handle, generation, worker slot, staged handler output and events. No other map holds per-operation state.
- **I2 One terminal, one writer.** A record's terminal is written at most once, only by `OperationRecord::settle`. Later settle attempts are ignored and, in test builds, counted as a violation.
- **I3 Every claim settles.** A `Claim` guard is the only way to move a record out of `Issued`, and it cannot be dropped without settling the record. Terminality is therefore a property of the guard type, not of an audit of exit paths.
- **I4 Provenance is a variant.** Whether core registered the caller request ID is carried by which `Admitted` variant core returned and by which proof token native code holds. No code path inspects `ModelError.code` or message text to decide it.
- **I5 Python holds no belief about consumption.** The session records the provenance core reported for each terminal; it keeps no allowlist and no consumed-ID set. Duplicate detection within a generation is core's `used` set at registration time.
- **I6 No Git before Accepted.** `Accepted` is written only after core returned `Ready`, the worker slot is held, and (for `submit`) the worker thread exists and is parked at its gate. The gate carries the `TransportRequest`; a worker that never receives it never runs Git.
- **I7 Success only after finish.** Handler output is staged privately; `settle(Completed)` is called only after `TransportRequest::finish()` returned. An unwind at any point settles `Failed` with unconfirmed cleanup and discards staged output.
- **I8 Only close writes Closing and Closed.** Faults set `faulted`, which refuses new claims and construction; they never bypass close's settlement of remaining records.
- **I9 Cancellation is by issued ID in every phase**, and `cancel`, `close` and `release` observe a record only after its terminal exists.
- **I10 Path isolation.** The legacy module-level path (`dispatch::submit`, `submit_accepted`, `spawn_call`, process-global `STORE`, `op_<request_id>`) and the session path share no recorder, store, worker wrapper or thread-local session hook.

## 5. Closed state machine

### 5.1 Phases and terminal kinds

| Phase | Meaning | Owner while in phase |
| --- | --- | --- |
| `Issued` | ID minted synchronously; 4 KiB ledger reservation (deferred ledger) and a live request-ID claim; no slot, endpoint, or core work. | bridge (issuance, cancel, release) |
| `Admitting` | A `Claim` guard holds the record; one of the eight top-level slots is held; decoding, placement, construction, capacity and registration run on the claiming thread. | claiming thread |
| `Accepted` | Registered, opened, generation recorded, cancellation handle stored; worker owns the `TransportRequest`; Git work may run. | worker thread (or the caller thread for `call`) |
| `Finishing` | Handler returned or unwound; `TransportRequest::finish()` in progress. | worker thread |
| `Terminal(kind)` | Immutable outcome plus `CleanupReport`; readable until release or expiry; slot and claim released. | ledger |

| Terminal kind | Provenance | Effect | `request_id_consumed` | Retained payload |
| --- | --- | --- | --- | --- |
| `Refused(error)` | `NotRegistered` | none | false | typed `ModelError` |
| `Refused(error)` | `Registered` | none | true | typed `ModelError` and finish `CleanupReport` |
| `Cancelled` before acceptance | `NotRegistered` or `Registered` | none | per provenance | typed `Cancelled` refusal; no `OperationResult` |
| `Cancelled` after acceptance | `Registered` | possible | true | failed `OperationResult` with `GwzErrorCode.cancelled=73` |
| `Failed` | `Registered` | possible | true | failed `OperationResult` (handler error, or `InternalError` on unwind) |
| `Completed` | `Registered` | n/a | true | successful `OperationResult`, optional retained `MergeResponse` |

`Refused` and pre-acceptance `Cancelled` project to Python as the v2 typed `GwzBridgeError` with `effect="none"`. `Failed` and post-acceptance `Cancelled` project as `GwzOperationError` with `effect="possible"`. `Completed` returns the result. These are the v2 §7 projections; V3 adds only the `request_id_consumed` fact (§10).

### 5.2 Legal transitions

| From | Event | Guard | To | Actions |
| --- | --- | --- | --- | --- |
| — | `reserve(request_id)` | grammar valid; session `Open` and not `faulted`; no live claim for `request_id`; fewer than 64 issued-unreleased records | `Issued` | mint serial; insert record; insert claim |
| `Issued` | `claim(id)` by `call`/`submit` | record `Issued`; session `Open` and not `faulted`; a top-level slot is free | `Admitting` | take slot; construct `Claim` |
| `Issued` | `claim(id)` | slot full | `Terminal(Refused, NotRegistered, TransportSessionFull)` | settle; return typed error |
| `Issued` | `claim(id)` | session `Closing`/`Closed`/`faulted` | `Terminal(Refused, NotRegistered, InvalidRequest closed)` | settle |
| `Issued` | `cancel(id)` | — | `Terminal(Cancelled, NotRegistered)` | settle with default cleanup |
| `Issued` | `release(id)` | — | record discarded | remove claim; high-water mark unchanged |
| `Issued` | `close()` | — | `Terminal(Cancelled, NotRegistered)` | settle |
| `Admitting` | `Claim::refuse(error)` | before `Admitted::Ready` | `Terminal(Refused, NotRegistered)` | settle; release slot and claim |
| `Admitting` | core returns `Admitted::Consumed(error, report)` | — | `Terminal(Refused, Registered)` | settle with report |
| `Admitting` | `Ready(request)` then spawn failure | `submit` only | `Terminal(Refused, Registered, IoError)` | `request.cancel()`; await `finish()`; settle with its report |
| `Admitting` | `cancel_requested` observed at checkpoint 1 | before phase 1 `admit` | `Terminal(Cancelled, NotRegistered)` | settle |
| `Admitting` | cancel signal while phase 1 `admit` is pending | the future is dropped; nothing registered | `Terminal(Cancelled, NotRegistered)` | settle; after pool mutation the endpoint generation closes (core guard) |
| `Admitting` | cancel signal while phase 2 `register_and_open` runs | not cancellable | unchanged until phase 2 returns | then the checkpoint-2 row applies |
| `Admitting` | `cancel_requested` observed at checkpoint 2 | after `Ready`, before `accept` | `Terminal(Cancelled, Registered)` | `request.cancel()`; await `finish()`; settle |
| `Admitting` | `Claim` dropped without `accept` or `refuse` (unwind) | — | `Terminal(Refused, provenance from the attached `RegistrationWitness`, else `NotRegistered`)` | if a `TransportRequest` was held: `cancel()` and drop it (sealed), cleanup unconfirmed; set `faulted`; settle |
| `Admitting` | `Claim::accept(request)` | — | `Accepted` | store generation and cancellation handle; note whether `cancel_requested` was already set and call the handle after releasing the lock; send the `AcceptedOperation` through the gate |
| `Accepted` | gate `send` returns the `AcceptedOperation` (worker gone before `recv`) | caller thread owns the value | `Terminal(Failed, possible)` | `abandon()`: `request.cancel()`; await `finish()`; settle; `submit` returns the typed error instead of `Accepted` |
| `Accepted` | handler returns or unwinds | — | `Finishing` | stage output locally; call `finish()` |
| `Finishing` | `finish()` returned | handler returned `Ok` | `Terminal(Completed)` | promote staged output; settle with report |
| `Finishing` | `finish()` returned | handler returned `Err` or unwound | `Terminal(Failed, possible)` | settle with report |
| `Finishing` | `finish()` returned | cancellation had been signalled and no `Completed` output exists | `Terminal(Cancelled, possible)` | settle with report |
| `Accepted`/`Finishing` | worker unwinds anywhere after `recv`, including inside `finish()` | — | `Terminal(Failed, possible, cleanup unconfirmed)` | the `AcceptedOperation` drop guard settles; set `faulted`; drop request (sealed) |
| `Accepted`/`Finishing` | `cancel(id)` | — | unchanged | call stored cancellation handle; wait for terminal |
| `Terminal` | `cancel(id)` | — | unchanged | return retained cleanup |
| `Terminal` | `release(id)` | — | record discarded | remove; a repeat reports `OperationExpired` |
| `Accepted`/`Finishing` | `release(id)` | — | unchanged | refuse `OpenOperation` |

Illegal observations (a record in `Admitting` with no live `Claim`, a `Terminal` write on a settled record, a `TransportRequest` outside a `Claim` or an `AcceptedOperation`) are programming errors; test builds assert them, release builds fail closed by settling `Failed` and setting `faulted`.

### 5.3 Proof tokens

- `Claim` — a `Drop`-guarded, non-`Clone` value bound to one record. It exposes `refuse(ModelError)`, `refuse_registered(ModelError, CleanupReport)`, `attach_witness(RegistrationWitness)`, `accept(TransportRequest) -> AcceptedOperation`, and `cancel_requested()`. Dropping it settles the record (§5.2), reading the attached witness for provenance on that path. It is the only owner of the top-level slot before acceptance.
- `Admission` (core, §6) — the phase-1 token: leadership held, capacity installed, nothing registered; dropping it consumes nothing. Only it can start phase 2.
- `RegistrationWitness` (core, §6) — written by core at the first request-ID insertion; read by the `Claim` only on its unwind path.
- `Admitted` (core, §6) — the phase-2 result; the variant is the registration provenance.
- `AcceptedOperation` — owns the `TransportRequest`, the slot, the record handle and the staging recorder. Its `run(dispatch)` method executes the handler, `finish()` and `settle`; its `Drop` settles `Failed(possible, cleanup unconfirmed)` if `run` did not settle; `abandon()` settles after cancel and finish when the caller thread keeps ownership (§7.3). It has no other public operations.
- `Terminal` — immutable once written; the sole source for `result`, `try_result`, event completion, cancel reports, `recent_operations()` descriptors and close summaries.

## 6. Core admission API

Add to `TransportRuntime` a two-phase candidate-visible admission API whose phase boundary **is** the registration boundary, plus one result type; keep the existing `request()` as a thin wrapper so the CLI and `with_local_transport` paths are behavior-preserving.

```rust
/// Phase-1 result. Holds admission leadership and the installed capacity
/// epoch; nothing is registered. Dropping it releases leadership and consumes
/// nothing. It is the only value that can start phase 2.
pub struct Admission { /* leader guard, resolved capacity, witness */ }
/// Written by core at the first request-ID insertion. The holder of the
/// matching `Admission` reads it only on an unwind path.
pub struct RegistrationWitness(Arc<AtomicU8>); // NotRegistered | Registered
pub enum Admitted {
    /// No request ID was registered on any session. The caller request ID may be
    /// retried in this generation.
    Refused(ModelError),
    /// At least one session registered the request ID and a later step failed.
    /// The registered scope has been cancelled, sealed and finished; no Git work
    /// ran. The request ID is consumed for this generation.
    Consumed(ModelError, CleanupReport),
    /// Registered on the endpoint and driver sessions, mux begin/ready complete,
    /// backend attached. Dropping it cancels and seals the request.
    Ready(TransportRequest),
}
impl TransportRuntime {
    /// Phase 1: validation, placement, capacity. Cancellable by dropping the future.
    pub async fn admit(&self, meta: RequestMeta, operation_id: String) -> Result<Admission, ModelError>;
}
impl Admission {
    pub fn witness(&self) -> RegistrationWitness;
    /// Phase 2: registration on both sessions, begin, ready. Not cancellable:
    /// the caller drives it to completion, bounded by the mux bootstrap deadline.
    pub async fn register_and_open(self) -> Admitted;
}
```

**Phase 1, `admit`**, serialized by the existing admission leader with the existing single arrival deadline, performs: (1) `validate_meta`, placement resolution and the runtime-closed check; (2) endpoint admission exactly as today's `admit_client_request` up to and including `install_capacity` with its `CapacityMutation` guard: closed check, duplicate/exhausted **pre-check** of the endpoint `used` set, capacity-conflict check, capacity install; (3) the driver-session **pre-check** under the driver lock: not closed, identifier valid, `used` does not contain the ID, `used.len() < 256`, driver mux phase live and not rejecting, mux tombstone count below its limit. Any failure or the deadline returns `Err`, and nothing is registered. The returned `Admission` keeps admission leadership, so no other registration can interleave between these pre-checks and phase 2; this preserves the round-three closures of State P2-6 and P2-7. Cancellation is honored in this phase by dropping the future: before pool mutation it consumes nothing; after pool mutation the existing `CapacityMutation` drop guard closes the endpoint generation, as reviewed.

**Phase 2, `register_and_open`**, is entered only from a live `Admission` and is **not cancellable**: the native session does not select it against any signal and drives it to completion, which the mux bootstrap deadline bounds. It performs: (4) endpoint registration (`ClientRequest::new`), then driver registration (`RequestContext::new`); core sets the `RegistrationWitness` at the first insertion. A driver-side failure after the endpoint insert is the only path to `Consumed`: cancel and seal the endpoint registration, await its finish, return `Consumed(error, report)`. It arises only from a mux-internal failure the pre-checks cannot see; an ordinary refusal by the endpoint mux before any insert returns `Refused`. (5) `begin()` and `ready().await`; failure cancels, seals, awaits `finish()` and returns `Consumed(error, report)`. (6) Attach the backend and return `Ready`.

Provenance therefore survives every boundary by construction: a future can be dropped only while nothing is registered; once phase 2 starts it returns a variant that names the provenance; and if core itself unwinds inside phase 2 (a hidden panic path in the sense of L2-15), the `Claim` reads the witness on its unwind path instead of assuming `NotRegistered`. Nothing inspects `ModelError.code` to decide.

`request()` becomes `let admission = self.admit(meta, operation_id).await?; match admission.register_and_open().await { Ready(r) => Ok(r), Refused(e) | Consumed(e, _) => Err(e) }`, so existing callers and tests are unchanged. `TransportRequest` gains `generation(&self) -> u64` (the driver session's generation counter, currently always the first generation) so the native record can pin cancellation authority now and rollover need not restructure records later. Capacity semantics (equal join without reinstall, differing capacity refused while any operation, non-idle lease or cleanup remains, one leader, five-second arrival deadline) are unchanged from v2 §4.

## 7. Native session structure

### 7.1 Record and indexes

```rust
struct OperationRecord {
    operation_id: String, request_id: String,
    phase: Phase,                       // Issued | Admitting | Accepted | Finishing | Terminal
    terminal: Option<Terminal>,         // kind, provenance, effect, error/result, cleanup
    cancel_requested: bool,
    cancellation: Option<TransportCancellation>, generation: Option<u64>,
    staged: Option<(OperationResult, Option<MergeResponse>)>, // handler output, never readable
    events: Vec<OperationEvent>,        // bounded by the deferred ledger stage
    changed: Condvar,                   // result, event, cancel and close waiters
}
struct Session {
    records: HashMap<String, Arc<OperationRecord>>,   // issued-unreleased, at most 64
    claims: HashMap<String, String>,                   // live request_id -> operation_id, non-terminal only
    slots: u8,                                          // Admitting + Accepted + Finishing, at most 8
    status: Open | Closing | Closed, faulted: bool, constructing: bool,
    runtime: Option<Arc<TransportRuntime>>, close_report: Option<CloseReport>,
    next_serial: u64, nonce: [u8; 16],
}
```

The `admitting`, `active`, `last_cancel` and `request_operations` maps, `Admitting.signal`/`failure`/`failure_report`, `AdmissionFailure`, `end_admission`, `complete_failed_admission`, `abandon_unstarted`, `worker_panicked`, `spawned_call`, `CURRENT_SESSION`/`current_session()`, `defer_terminal`, `publish_terminal` and `pending_terminal` are deleted. The `OperationStore` keeps `issue`, `discard`, `contains`, `events`, `wait_events`, `result`, `try_result` and `merge_response`; its record gains `settle` and the private `staged` slot. `OperationRecorder::finish`, `finish_merge` and `finish_model_error` write to `staged`; `finish_error` and `finish_panic_error` are removed from the session path (the legacy module store keeps its own equivalents).

The deferred ledger stage attaches to exactly three points: `reserve` (the 4 KiB unstarted reservation), `Claim::accept` (the atomic upgrade to the 8 MiB allowance and the primary event-reader cursor, claimed before the worker gate opens) and `settle` (exchange of the reservation for charged bytes). The deferred handle stage calls the same `claim`/`accept` path from `accepted()`; no stage adds a second admission route.

`claims` holds one entry per non-terminal record. `reserve` refuses `InvalidRequest` when a live claim exists for the request ID, refuses `TransportSessionFull` at 64 records, and otherwise mints the next serial. `settle` removes the claim regardless of provenance; whether the ID may be registered again is decided by core at the next `admit_request` (`Refused(InvalidRequest)` if consumed). The session never decides consumption.

### 7.2 Entry points

Both `#[pymethods]` `call(.., operation_id)` and `submit(.., operation_id)` do, as the **first statement inside `py.detach`**: `let claim = self.claim(operation_id)?;`. `claim` validates nonce and serial, returns the retained typed terminal if the record is already terminal (this is how a cancel-before-entry on a blocked executor is honored without minting), refuses `InvalidRequest` ("operation already started") if the record is `Admitting`, `Accepted` or `Finishing`, takes a slot or settles `TransportSessionFull`, and moves the record to `Admitting`. Only then does the entry point capture `current_dir()` and decode the request. Every failure from that point to `Accepted` goes through `claim.refuse(..)` or `claim.refuse_registered(..)`; a panic drops the guard, which settles.

Shared admission: `fn admit(&self, claim: Claim, method, message names, bytes, cwd) -> Result<AcceptedOperation, PyErr>` performs, in order: decode `network_meta` (a non-network method with an issued ID refuses `InvalidRequest`); `validate_request_context`; explicit CLI placement refusal (`UnsupportedOperation`, before construction); cancel checkpoint 1; runtime construction on the claiming thread (`runtime_with` keeps its `catch_unwind`; a panic settles this claim `Refused(NotRegistered, InternalError)` and sets `faulted`; other admitting claims waiting on `constructing` refuse through their own guards; `Issued` records are untouched); core phase 1 `admit` polled against the record's cancel signal (a cancel drops the future; nothing is registered): on `Err` → `claim.refuse`; on `Ok(admission)` → `claim.attach_witness(admission.witness())`, then phase 2 `register_and_open` driven to completion without any select: on `Refused` → `claim.refuse`; on `Consumed` → `claim.refuse_registered`; on `Ready(request)` → cancel checkpoint 2 (cancel, finish, settle `Cancelled(Registered)`), then for `submit` spawn the parked worker thread (spawn failure → `request.cancel()`, `finish()`, `claim.refuse_registered(IoError, report)`), then `claim.accept(request)`.

`call` runs `accepted.run(dispatch)` inline on the claiming thread and returns the response bytes. `submit` hands `accepted` to the worker through the gate and returns the `Accepted` envelope, which the session builds itself with the issued operation ID; the legacy `submit_accepted` is not on this path. Admission latency is unchanged for callers: today's `submit` already blocks on its channel until registration.

`AcceptedOperation::run`: under an unwind guard, execute `dispatch::call` inside `operations::with_store` (session store), `shims::with_operation_id` and `shims::with_scoped_backend(request.backend())`; keep the dispatch result local; move to `Finishing`; `finish()`; then `settle(Completed | Failed | Cancelled)` with the finish report; release the slot. If the unwind guard fires, it settles `Failed(possible, pending_local_work ≥ 1, peer_cleanup_confirmed = false)`, drops the request (its `Drop` seals it), and sets `faulted`.

### 7.3 Worker gate and ownership

The gate is a one-shot channel whose message **is** the `AcceptedOperation`. The thread is spawned before `accept`, and its body is `recv` followed by `run`, with no statement between them. `accept` writes `Accepted` under the session mutex (§7.4), then sends. Ownership of an accepted record is ownership of the `AcceptedOperation` value, which exactly one party holds at any time, and that value settles on `Drop`: `run` settles normally after `finish()`, and any other drop (a panic anywhere in the worker after `recv`) settles `Failed(possible, cleanup unconfirmed)` and sets `faulted`. The handoff failures are therefore defined by ownership rather than by flags:

- **Worker gone before `recv`** (its thread exited before receiving): `send` returns the `AcceptedOperation` to the caller thread, which now owns it and calls `abandon()`: `request.cancel()`, await `finish()`, settle `Failed(InternalError "worker unavailable", cleanup from finish)`. `submit` returns that typed error instead of `Accepted`. No Git ran; the effect is projected conservatively as `possible` per v2 §6, and the record is discoverable by ID.
- **Worker gone after `recv`:** it held the value, so its drop settled the record.
- **Sender dropped without sending** (a panic between spawn and `send`, which drops the `Claim`): the thread's `recv` returns an error and it exits having run nothing; the `Claim` drop path settles and finishes the request.

A worker can never observe a record that is not `Accepted`, no code path needs a `started` flag, and an `Accepted` record has exactly one owner until it is terminal.

### 7.4 Locking model

Two mutex kinds, one order. The **session mutex** guards `status`, `faulted`, `constructing`, the `records` map, `claims`, `slots`, `next_serial`, and every record's phase-bearing fields: `phase`, `cancel_requested`, `cancellation` and `generation`. Each **record mutex** guards only that record's payload: `terminal`, `staged` and `events`; the record condvar (result, event and cancel waiters) is bound to it. The order is session, then record; no path takes a record mutex and then the session mutex. Readers (`result`, `try_result`, `wait_events`, `merge_response`) and handler event appends take only the record mutex, so they never contend with admission or close and cannot invert the order. The session condvar (close and construction waiters) is bound to the session mutex.

Every phase transition (`reserve`, `claim`, `accept`, `settle`, and the cancel and close intents) is one function entered with no lock held. It takes the session mutex, validates the current phase against the intent, updates phase, slots and claims, then takes the record mutex to write the terminal, releases the record mutex, notifies both condvars, and releases the session mutex. An intent that no longer matches the phase (a cancel arriving after the record settled) reads the retained terminal instead of failing. No core, Git or Python call runs under either mutex: `accept` notes under the session mutex whether a cancel was already requested and calls the `TransportCancellation` after releasing it, while a concurrent `cancel` that observes `Accepted` calls the same idempotent handle itself, so no cancellation is lost in either interleaving. `Claim::drop` and `AcceptedOperation::drop` run only in frames that hold neither mutex, because both values live in the entry-point or worker frame outside every lock scope.

## 8. Cancellation, close and faults

`cancel(id)` validates the ID and acts by phase under the record lock: `Issued` → settle `Cancelled(NotRegistered)`; `Admitting` → set `cancel_requested`, notify the cancel signal, then wait for the terminal; `Accepted`/`Finishing` → call the stored `TransportCancellation` (idempotent) and wait; `Terminal` → return the retained cleanup. `accept` stores the cancellation handle under the same lock and calls it immediately if `cancel_requested` is already set, so a cancel racing acceptance cannot be lost. Repeated cancellation returns the same cleanup, as v2 §6 requires.

`close()` moves `Open → Closing` (only close writes this), then settles or signals every record by phase: `Issued` → `Cancelled(NotRegistered)`; `Admitting` → `cancel_requested`; `Accepted`/`Finishing` → cancellation handle. It waits on the session condvar until `slots == 0` and `constructing == false`; I3 and I7 guarantee that wait ends because every claim and every worker settles. It then shuts the runtime down once (or reads a losing constructor's cleanup), composes the close report from physical facts plus at most eight operation summaries taken from terminals, and writes `Closed`. A repeat or concurrent close joins and returns the same report. A `faulted` session reports `peer_cleanup_confirmed = false` and `pending_local_work ≥ 1`. A `Closing` session refuses `reserve` and `claim` with the closed refusal; `cancel`, `release`, result and event reads keep working on the ledger after close, as v2 §6 requires.

Faults never write `Closing` or `Closed`. Construction panic, worker unwind and abandoned-claim unwind set `faulted`, which refuses new `reserve`, `claim` and construction; close then runs unchanged. This removes the round-two P2-4 class (a fault path that bypassed close's settlement of unstarted records) by construction.

## 9. Terminal publication and readers

`settle` is the §7.4 transition that moves a record to `Terminal`: it releases the slot and the claim and updates the phase under the session mutex, writes `terminal` under the record mutex, then notifies both condvars; the first writer wins. `result()` waits for `terminal`, returns `Completed`/`Failed`/post-acceptance `Cancelled` results, and raises the typed refusal for `Refused` and pre-acceptance `Cancelled`. `try_result` returns `None` until a terminal exists, then the same projection. `wait_events` reports completion exactly when `terminal` exists; a terminal event precedes iterator end for accepted operations. `merge_response` is served from the promoted staged payload of a `Completed` terminal only. No reader can observe staged output, so a handler's `recorder.finish(..)` is invisible until `settle(Completed)` promotes it after `finish()`; an unwind before that point leaves it unpromoted and discards it. This differs from the rejected `defer_terminal` mechanism in one decisive way: there is no promotion call. Promotion is a step inside `settle(Completed)` and nothing else can perform it. A session-owned `OperationStore` stages every handler completion; the isolated legacy module store keeps publishing directly, so the shared handler code and `OperationRecorder` type are unchanged and the mode is a property of the store, not a per-call flag.

## 10. Python bridge and bounded contract amendments

The bridge keeps issuing IDs synchronously and passing the issued ID into native `call`/`submit`; `_run_native`'s pre-entry cancellation path is unchanged and now relies on `claim` observing the terminal (§7.2). The bridge's `_issued_requests` cache remains a convenience for the implicit unary/stream forms and must not be consulted for any admission decision.

V3 amends the v2 contract in three places, each additive:

1. **Registration is defined.** v2 §2 "A successful registration consumes it even if worker launch then fails" is restated as: the caller request ID is consumed for the current generation when core reports `Consumed` or `Ready`; it is not consumed when core reports `Refused`. Both endpoint and driver registrations are part of one core admission step; a partial registration counts as consumed.
2. **The retry fact is exposed, not inferred.** Every pre-effect refusal raised to Python (`GwzBridgeError` with `effect="none"`, including `GwzOperationCancelled` before acceptance) carries `request_id_consumed: bool`, derived from the retained provenance. Without it, the caller would have to infer consumption from the error code, which is the State P2-8 defect moved one layer up. The `recent_operations()` descriptor (deferred handle stage) carries the same field. This is an additive attribute. The [v2 caller guide](../gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md) carries a draft note in this tuple that replaces its "A failed worker launch after successful core registration does consume it" sentence with the `request_id_consumed` rule, for a docs-only Surface check.
3. **`Accepted` for `submit()` is produced by the session.** v2 §5's meaning of `Accepted` is unchanged; the envelope is built by the native session after `accept`, not by the legacy `submit_accepted`.

Nothing else in v2 §§2–8 changes. Custom bridges, `TransportCleanup | None`, `close_report`, the two Taut codes and the handle/stream surfaces are as accepted.

## 11. Fault matrix

Each row is a required closure case for **both** `call` and `submit` unless marked. Columns: terminal kind / provenance, effect, request ID retryable in this generation, core registration, Git effect, waiters woken, slot and claim released.

| Fault point | Terminal | Effect | Retry same ID | Core reg. | Git | Waiters | Slot/claim |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `current_dir()` fails after claim | Refused / NotRegistered, `IoError` | none | yes | no | no | yes | released |
| Malformed request CBOR | Refused / NotRegistered, `InvalidRequest` | none | yes | no | no | yes | released |
| Wrong request/response message name (`submit`) | Refused / NotRegistered, `InvalidRequest` | none | yes | no | no | yes | released |
| Issued ID passed for a non-network method | Refused / NotRegistered, `InvalidRequest` | none | yes | no | no | yes | released |
| Explicit CLI placement, HOME present or absent | Refused / NotRegistered, `UnsupportedOperation` | none | yes | no | no | yes | released; no endpoint construction |
| Session `Closing`/`Closed`/`faulted` at claim | Refused / NotRegistered, `InvalidRequest` | none | n/a | no | no | yes | never taken |
| Ninth top-level operation | Refused / NotRegistered, `TransportSessionFull` | none | yes | no | no | yes | slot never taken; claim released |
| 65th issued record | synchronous `TransportSessionFull`, no record | none | yes | no | no | n/a | n/a |
| Endpoint construction error (HOME unset) | Refused / NotRegistered, `InvalidRequest` | none | yes | no | no | yes | released |
| Endpoint construction panic | Refused / NotRegistered, `InternalError`; session `faulted`; sibling `Issued` untouched | none | closed session | no | no | yes | released |
| Capacity conflict | Refused / NotRegistered, `TransportCapacityConflict` | none | yes, after conflict clears | no | no | yes | released |
| Capacity leadership or install timeout | Refused / NotRegistered, `IoError` | none | **yes** | no | no | yes | released |
| Duplicate request ID in generation | Refused / NotRegistered, `InvalidRequest` | none | no (already consumed by the earlier registration) | no | no | yes | released |
| Driver registration failure after endpoint insert | Refused / Registered, typed core error | none | **no** | consumed | no | yes | released after finish |
| `begin()`/`ready()` failure after registration | Refused / Registered, typed core error (`IoError`, `UnsupportedOperation` or `InvalidRequest` from the mux mapping) | none | **no** | consumed | no | yes | released after finish |
| Worker thread spawn failure (`submit`) | Refused / Registered, `IoError` | none | no | consumed | no | yes | released after finish |
| Cancel before native entry (blocked executor) | Cancelled / NotRegistered | none | yes | no | no | yes | never taken; no remint |
| Cancel at checkpoint 1 | Cancelled / NotRegistered | none | yes | no | no | yes | released |
| Cancel while phase 1 `admit` is pending | Cancelled / NotRegistered; the future is dropped; after pool mutation the existing `CapacityMutation` guard closes the endpoint generation and later admissions refuse closed | none | yes, while the generation is open | no | no | yes | released |
| Cancel while phase 2 `register_and_open` runs | not observed until phase 2 returns (not cancellable); then the checkpoint-2 row applies | — | — | — | — | — | — |
| Cancel at checkpoint 2 | Cancelled / Registered | none | no | consumed | no | yes | released after finish |
| Cancel after acceptance, completion loses | Cancelled / Registered, code 73 | possible | no | consumed | possible | yes | released after finish |
| Cancel after acceptance, completion wins | Completed | n/a | no | consumed | yes | yes | released |
| Handler error | Failed / Registered | possible | no | consumed | possible | yes | released after finish |
| Handler panic | Failed / Registered, `InternalError`; `faulted` | possible | no | consumed | possible | yes | released; cleanup unconfirmed |
| Staged success then `finish()` panic | Failed / Registered; staged output discarded | possible | no | consumed | possible | yes | released; cleanup unconfirmed |
| Panic between spawn and `accept` | Refused / Registered via `Claim` drop; worker exits idle | none | no | consumed | no | yes | released; cleanup unconfirmed |
| Gate handoff fails (worker thread gone before `recv`) | Failed / Registered, `InternalError`, via caller-thread `abandon()` | possible (conservative; no Git ran) | no | consumed | no | yes | released after finish |
| Core unwinds inside phase 2 after the first insert | Refused / Registered via `Claim` drop reading the witness; `faulted` | none | no | consumed | no | yes | released; cleanup unconfirmed |
| Close while `Issued` | Cancelled / NotRegistered | none | closed | no | no | yes | n/a |
| Close while `Admitting` | one of the cancel rows above | per row | closed | per row | no | yes | released |
| Close while `Accepted` | Cancelled or Completed, as the race decides | per kind | closed | consumed | possible | yes | released; summary in close report |
| Release while `Accepted`/`Finishing` | unchanged; `OpenOperation` | — | — | — | — | — | held |

## 12. Required proof and gates

Design review first: dual peer-blind Consistency and Safety review of this document together with the draft v3 foundation paragraphs it adds to `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` (§6) and the note it adds to the v2 caller guide (§10), with a docs-only Surface check of that note; all three are in the same committed tuple. Any P0–P2 keeps implementation stopped. Because §6 changes a core API and §7 changes the reviewed native architecture, implementation review uses **fresh** Code and State reviewers on a new exact tuple, per the review-loop rule for a changed architecture boundary.

Focused closure tests, all typed-field assertions with no message parsing:

1. **Provenance without text.** Hold the endpoint capacity gate until the arrival deadline: assert `Admitted::Refused` in core, `request_id_consumed == false` in Python, and that a fresh record with the same request ID is admitted after the gate releases. Separately inject a driver-side failure after endpoint registration (test hook: mark the driver mux rejecting after the pre-check) and a `ready()` failure: assert `Admitted::Consumed`, `request_id_consumed == true`, a finished cleanup report, and that a fresh record with the same ID is `Refused(InvalidRequest)` by core. Both cases surface `IoError` and are distinguished only by variant. Then cancel once while phase 1 is pending (the future drops) and once while phase 2 runs (observed only at checkpoint 2): assert `NotRegistered`/retryable for the first and `Registered`/consumed with a finished cleanup for the second.
2. **Every pre-worker exit settles, both forms.** Parametrize the §11 rows above the acceptance line over `call` and `submit`. For each: exactly one terminal; the returned error and `operation_result(id)` carry the same typed code; `wait_events` reports complete; `try_operation_result` agrees; the slot count returns to its prior value; the claim is released; no core registration except where the row says consumed; no Git effect; release succeeds; repeat past 64 records and admit afterwards.
3. **Terminal uniqueness under faults.** Stage a success and panic in `finish()`; panic in the handler; drop the `Claim` between spawn and accept; assert one `Failed`/`Refused` terminal, no success visible at any time, cancel/close/release ordering per §5.2, and `faulted` refusing later claims while close still terminates and reports. Then the three handoff faults of §7.3 and §6 through test hooks: a worker thread that exits before `recv` (assert the caller-thread `abandon()` path: one `Failed` terminal, `submit` returns the typed error, finish completed, no Git effect); a panic in the worker after `recv` (assert the `AcceptedOperation` drop settlement); a panic inside core phase 2 after the first insert (assert the `Claim` settles `Refused/Registered` from the witness and the same request ID is then refused as a duplicate).
4. **Cancellation at every checkpoint**, including the existing blocked-executor test for both forms, cancel during the capacity wait before and after pool mutation (the latter closes the generation), and cancel racing `accept` (handle stored and called under one lock; assert the transport request observed cancellation).
5. **Close in every phase**, construction panic with an `Issued` sibling, close idempotence, and close-report summaries for live operations, with post-close ledger reads and refused new work.
6. **Path isolation.** The legacy module-level `submit` still records into the process store; a session `submit` never touches it; a session ID never resolves in the module store and a module ID never resolves in a session.
7. **Behavior preservation.** Existing core `transport_host` tests pass unchanged through the `request()` wrapper; the CLI single-operation path is unaffected.
8. **Lock order.** A debug-build lock-order assertion (session before record, no re-entry, no core or Python call under either) is enabled in the focused native tests, and the Safety review traces every §5.2 transition against §7.4.

Then run the settled Code/State review on the exact tuple. The deferred stages and their gates (ledger bytes and timer, public handles, rollover, transport stress, platform, wheel/source pins) remain separate and are not waived by a foundation GO.

## 13. Out of scope, unknowns and risks

- `TransportGenerationBusy` and `TransportRecordLimit` model codes do not exist yet; they belong to the rollover and ledger stages. The record's `generation` field and `staged` slot are laid down now so those stages add behavior without changing the state machine.
- `Consumed` from a driver-side registration failure is expected to be unreachable in practice once the pre-checks exist; it is kept so no caller ever infers. If the Safety review finds a reachable partial-registration path the pre-checks miss, the correction is another pre-check, not a code allowlist.
- Blocking the claiming thread on `admit_request` for up to the five-second deadline is unchanged in effect from the current `submit` (which blocks on its channel until registration) and from `call`; Python already runs both under `asyncio.to_thread`.
- The `Claim` drop path after registration cannot await `finish()` during unwinding; it seals the request and reports unconfirmed cleanup, which close later drains. This is the same conservative fact the current unwind path reports, now attached to the right record.
- Risk: the single-record design concentrates locking on one session mutex plus per-record payload mutexes. §7.4 fixes the order and forbids external calls under either; the Safety review should attack every transition against it.
- Phase 2 of core admission is not cancellable, so a cancel requested during it waits for `begin`/`ready` to complete or hit the mux bootstrap deadline before checkpoint 2 honors it. That bound is the existing mux configuration, not a new clock.
