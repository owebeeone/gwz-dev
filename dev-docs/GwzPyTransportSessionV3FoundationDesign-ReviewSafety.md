# Python transport session v3 foundation — SAFETY-AXIS REVIEW

Review object: committed DRAFT `dev-docs/GwzPyTransportSessionV3FoundationDesign.md`, including the draft v3 paragraphs in core requirements and design and the Python caller-guide note.  
Baseline: root `b35ea74bf7e73c15777a3e0fb18587d05faffb17`; gwz-core `58e25012449ee8f4609daba4939e7157e99ea488`; gwz-py `d29d450508138bda9251e797afdb21d71d20d8dc`. All three HEADs matched at the start and end of this review.  
Date: 2026-09-24. Axis: Safety. Independent, read-only review; no files, Git state, tests or builds were changed.

**Verdict: NO-GO — 1 P1, 4 P2 open.** The P1 and P2-1 through P2-3 findings identify architectural root causes. P2-4 is a bounded locking-contract correction.

## 0. Evidence base

I reviewed the committed v3 foundation design §§4–12, its paired draft core paragraphs and Python caller-guide note, the accepted v2 design, the third v2 foundation verdict, `gwz-py/dev-docs/GwzPyTransportDesign.md`, and the relevant committed core request/session source. The process basis was `AGENTS_GWZ.md`, `EVIDENCE.md`, `dev-docs/AgentProcessRules.md` as amended by `dev-docs/GwzProcessOptimization.md`, and the review-loop skill. Current source was used to test the proposed transition, not treated as an implementation of v3.

## 1. Findings

### P1-1 — Phase 2 has no independent completion bound when its mux driver stops

**Location:** V3 §6, especially the “bounded by the mux bootstrap deadline” claim, and §§8, 11 and 13 for cancel and close waits. In current core, `Session::drive()` advances the mux clock (`src/transport_host/session/driver.rs:242–249`), the placement supervisor calls `drive()` from its thread (`src/transport_host/session.rs:314–333`), and `Session::ready()` awaits the mux phase (`src/transport_host/session.rs:715–724`). The mux `ready()` waiter has no wall-clock deadline of its own.

**Violated invariant and sequence:** The draft promises that noncancellable phase 2 eventually returns, allowing a waiting cancellation or close to settle its `Admitting` record. Start phase 2, register the ID, call `begin()`, then have the placement supervisor unwind or exit while the mux remains `Binding`. Without `drive()`, the logical bootstrap clock does not advance and `ready()` remains pending. The `Claim`, slot and admission leadership remain held. `cancel(id)` waits for its terminal; `close()` waits for `slots == 0`; neither has a path to finish.

**Impact:** A degraded core worker can permanently block cancellation, close and subsequent admissions. The asserted mux deadline is not a bound in this sequence.

**Required correction and closure test:** Give phase 2 an independent wall-clock termination path, and make supervisor exit close the affected session and wake `ready()`/finish waiters. The termination path must preserve registered provenance and conservative cleanup. Inject supervisor exit after `begin()` but before `ready()`, then prove phase 2, cancellation and close all terminate within a stated bound and the ID remains classified as consumed.

**Architectural root cause:** The design treats a deadline advanced by the worker being monitored as an independent guarantee of that worker’s progress.

### P2-1 — Ordinary handler panic publishes a terminal before transport finish

**Location:** V3 §§5.2, 7.2–7.3 and §11’s “Handler panic” row. The accepted v2 §5 requires a worker panic after `Accepted` to publish its attributed terminal **after** `TransportRequest::finish()`.

**Violated invariant and sequence:** Accept a push, begin transport work, then panic in the handler. V3 sends the unwind to `AcceptedOperation::Drop`, which seals and drops the request, immediately settles `Failed` with unconfirmed cleanup, and releases the slot. A concurrent `cancel(id)` can therefore return from the terminal before `finish()` has joined the request’s local work. Close can likewise pass its slot wait before that finish path completes.

**Impact:** Cancellation no longer means joined transport cleanup on a routine handler panic, although v3 says it amends only three other v2 clauses. The result correctly says the Git effect is possible, but its terminal and cancellation timing lose the accepted cleanup guarantee.

**Required correction and closure test:** Catch handler unwind inside `run`, retain ownership of the request, await `finish()`, and then settle `Failed` with its report. Reserve the drop fallback with unconfirmed cleanup for an unwind in `finish()` itself or a genuinely unfinishable owner failure. Inject a handler panic with pending transport work; assert that result and cancel waiters remain pending until finish reports, then observe one failed terminal. Separately inject a finish panic to exercise the conservative fallback.

**Architectural root cause:** One drop guard is being used both for exceptional ownership loss and for a catchable handler failure, although `Drop` cannot await the required finish.

### P2-2 — A concurrent release can erase a close report’s required operation summary

**Location:** V3 §8 says close waits for slots to reach zero and *then* composes summaries from terminals, while release and ledger reads remain usable during `Closing`. Accepted v2 §6 requires a summary for every operation live when close began.

**Violated invariant and sequence:** An operation is `Accepted` when close enters `Closing`, so its ID belongs in the close report. The worker settles and releases its slot. Before close scans terminal records, another thread calls `release(id)`, which is legal for that terminal and removes it. Close then sees no record to summarize and returns a report missing an operation that was live at close entry.

**Impact:** The only durable close attribution for an early-abandoned, possibly effective operation can disappear. This is a concrete recovery and diagnosability failure even though the operation’s own release was legal.

**Required correction and closure test:** At the `Open → Closing` transition, capture or pin the exact live operation set and retain each compact summary until the close report is committed. Permit release to discard the individual result without deleting an owed close summary. Race terminal release against close’s slot-zero wakeup and assert the report still contains the operation with its final code and effect.

**Architectural root cause:** Close-report evidence is assembled from a mutable ledger after other threads may legally remove its source records.

### P2-3 — The registration witness has no specified atomic relationship to the first irreversible insertion

**Location:** V3 §§5.3 and 6 say core writes `RegistrationWitness` at the first request-ID insertion and `Claim::Drop` reads it on hidden unwind. Current `Session::register()` calls mux `owner.register()` before inserting into the session `used` and `registrations` sets (`src/transport_host/session.rs:680–704`); mux registration itself inserts a lifetime tombstone.

**Violated invariant and sequence:** The first irreversible registration is inside `owner.register()`. If that call inserts the mux tombstone and unwinds before core updates a separate witness, `Claim::Drop` reads `NotRegistered` and retains a retryable refusal for an ID already consumed by the mux. Updating the witness before calling `owner.register()` creates the opposite risk when registration refuses without insertion. The draft specifies neither a transactional primitive nor an indeterminate fail-closed state for this interval.

**Impact:** The exact hidden-unwind case for which the witness was introduced can publish false retry provenance. The proposed test hook “after the first insert” only proves the intended result if it is placed after the witness write; it does not close the gap inside registration.

**Required correction and closure test:** Make provenance publication part of the registration mutation boundary, or use a scoped registration token that reports a conservative indeterminate/consumed outcome on unwind until insertion is known to have failed. Specify how this is achieved across the mux API; if that requires changing `gwz-transport`, include that boundary in the reviewed scope. Inject unwind immediately after the mux tombstone insertion and before session bookkeeping, then assert the terminal never reports `request_id_consumed == false`.

**Architectural root cause:** A separate atomic witness is presented as proof of a mutation performed by another owner without an atomic handoff between them.

### P2-4 — Cancellation’s lock instruction permits a record-to-session deadlock

**Location:** V3 §7.4 requires session mutex before record mutex for every transition, but §8 says `cancel(id)` “acts by phase under the record lock.” Phase and `cancel_requested` are expressly session-mutex fields in §7.4, and settling an `Issued` cancellation needs the session mutex.

**Violated invariant and sequence:** If cancellation follows §8 literally, thread A holds the record mutex while acquiring the session mutex to settle an `Issued` record. At the same time, thread B has acquired the session mutex for another settlement or close intent and is waiting for that record mutex. Both block. The same wording also permits reading phase without its stated protecting lock.

**Impact:** Cancellation and close can deadlock on an ordinary concurrent terminal transition. The explicit order in §7.4 is sound, but §8 gives an incompatible implementation instruction at the point developers will implement cancellation.

**Required correction and closure test:** State in §8 that cancellation acquires the session mutex first, inspects and updates phase there, then acquires the record mutex only for terminal payload; call the core cancellation handle after both locks are released. Add a focused competing cancel/settle/close lock-order test or debug assertion that rejects record-to-session acquisition.

## 2. Invariant analysis

The claim-first entry sequence covers the earlier `current_dir()`, decode, message-name and placement exits: once `claim()` succeeds, those ordinary failures can settle the issued record through the guard. The phase-1 boundary also keeps registration out of capacity refusal and cancellation before phase 2. An armed `CapacityMutation` guard closes an endpoint when a pool installation is interrupted after mutation, which is conservative for that fault.

The one-shot worker gate gives the accepted value one owner: a failed send returns it to the sender; a receiver that owns and drops it can settle a failure. The single `settle` writer and private staging slot prevent a staged handler success from becoming visible before finish returns. Terminal publication under session-then-record locking can wake record and close waiters consistently if all transitions obey that order. Storing the cancellation handle during `accept` under the session lock, then invoking it outside the lock, covers cancellation on either side of acceptance.

Normal partial-registration and `begin()`/`ready()` error paths have an appropriate `Consumed` outcome *if* core finishes their registered scopes as specified. The witness atomicity finding concerns hidden unwind at the first insertion, and the phase-2 bound finding concerns failure of the worker that advances the mux deadline. The §11 rows for phase-2 cancellation, close while admitting, handler panic, hidden core unwind and close while accepted therefore lack the promised terminal or recovery guarantee in the sequences above.

## 3. Risks and next action

Keep the foundation at NO-GO. Revise the phase-2 watchdog and supervisor-exit behavior, separate catchable handler panic from unfinishable owner loss, preserve close summaries across release, define an atomic registration-provenance boundary, and make cancellation’s lock order unambiguous. Re-review those corrections on a new exact tuple with the corresponding fault sequences. This review neither accepts implementation nor changes the deferred ledger, handle, rollover or platform gates.
