# Step 2.2b flush delivery race: State review

Fix for the Windows CI failure after the 2.2b port onto main (gwz-core
`e5b8407b`): `a_record_is_evicted_only_once_every_declared_view_is_delivered`
got `SessionFull` instead of `accepted op-3`. A single State re-verdict
(review-loop, one reviewer); the report below is filed unedited.

---

**Axis State review: step 2.2b operation service, `Inner::flush` early-mark fix**

**Tuple**
- Object: gwz-core at HEAD `e5b8407b5b2652a72cbe5b150bdd0fe2e8f798fc`, plus the uncommitted `git diff -- crates/session-host` (`service.rs`, `service/tests.rs`).
- Diff hash, `shasum` of `git diff -- crates/session-host`, at start and at end: `d27d6897c30975df90448cfcf3a3fa2e9a746ea4`. Both values are the same. HEAD is unchanged.
- Controlling documents:
  - `GwzCoreSessionDesign.md` §3, §5.4, §8.
  - `GwzSplitCommandChannel-Step2.2b-ReviewState.md` (P2-3, whose correction reads "set the flag when the reply is handed over, not when it is built").
- Method: I read the code, ran the full `gwz-session-host --lib` suite (56 passed), and ran the two eviction tests 150 times (0 failures). In a scratch copy outside the repo (the new test applied, `flush` left old) the new test fails. I edited nothing in the repo.

**1. Root cause and regression test: verified**
- Race sequence:
  1. A client calls `operation.result(op-1)` while op-1 runs, so a waiter is registered.
  2. The worker thread runs `settle`, builds the result reply with `delivers = (op-1, Result)`, and calls `flush`.
  3. The old `flush` called `send_reply` and only then took the state lock for `mark_delivered`.
  4. Between the send and the mark, the client can read the reply and submit a new operation.
  5. The read thread and admission thread admit it, `table_entries >= operation_table` triggers `evict()`, `delivered()` is still false, and the call is refused with `SessionFull`.
- The window only exists for replies sent from worker threads.
  - Replies built on the read thread or the admission thread are flushed before the next call from that client is read or admitted.
  - So the race applies to the Windows test, where `operation.result` is issued while the op is still running.
- The new test is deterministic and passes for the right reason.
  - `host.call("operation.result")` returns only after `view()` has registered the waiter, so the worker thread will answer it.
  - `schedule` runs in the same locked section as `settle`, so op-2 has left the queue before the worker flushes. The `queued_operations: 1` check therefore cannot trip.
  - The sink pushes the reply and notifies before it parks, so `wait(result)` returns while the worker is still inside `send_reply`.
  - On the old `flush` it fails with exactly `SessionFull: the session's operation table holds no record it can evict`.
  - It does not pass vacuously: acceptance of op-3 requires evicting op-1, the only delivered record.
  - It has no ordering sleeps. The only timeout is the 20 s park bound.
  - `resume()` is called before the assertion, so a failure cannot leave the worker parked.
  - The park condition's lock temporary is dropped before the `if` body, and `wait_timeout_while` releases `resume` while waiting, so there is no lost wakeup and no deadlock.
  - It runs 56 tests in about 7 s and is stable over 150 runs.

**2. Safety of marking before sending**
- **`SendError::Full` (in-process):** the host calls `close_channel`, which ends the channel at both ends. An early mark is never observable, because no further reply can be delivered.
- **Partial batch:** after the first `Full`, later sends return `Closed` and are counted as dropped. The session is already ending, so mark granularity within a batch does not matter.
- **`SendError::Closed` from a local close, peer close, or protocol end:** the carrier ends the session through `channel_closed`. This is not changed.
- **Eviction versus later calls on the same operation:** the early mark lets a new submit evict op-1 while op-1's result reply is in flight. This matters only for a client that sends a second, redundant call on op-1 before reading the reply to its first one. Examples are a second `operation.result` or `operation.cancel`. That call gets `OperationExpired` (the §5.4 "later calls on an evicted record" outcome), where the old code would have answered it from the retained record. A client that waits for the reply before reusing the operation sees no difference. See F-1.
- **Reordering between flushes:** unchanged. Send order inside a batch is the same, and there was never an ordering guarantee between batches from different threads. The mark never gates sends.
- **Close ordering:** `unflushed` is counted under the table lock before the batch is built and decremented after the sends. `notify_all` and steps 8 and 9 are untouched. Marking cannot let the close report overtake a reply.

**3. Lock discipline: clean**
- `table::mark_delivered(&mut lock(&self.state), ..)` holds its guard only until the end of that statement. It is released before any `send_reply`.
- Every `flush` caller drops the state lock first:
  - `Service::receive`
  - the worker `run`
  - `admission_loop`
  - `read_loop`
  - `run_close` at each step
- The test sink takes `slow`, `closed`, `order`, `replies`, `parked`, and `resume` one at a time. No lock is held across another acquisition.
- `Sink::wait` holds `replies` only inside its condvar wait, which releases it.
- There is no new lock-order edge.

**4. Other callers and consumers**
- `delivers` is set in exactly one place, `view_reply` in `table.rs`. `mark_delivered` has one caller, `flush`. `Record::delivered()` is read only by `evict()`.
- Marking an already-discarded call id is a no-op.
- No other assumption changed.

**Findings**

*F-1 (P3): the `flush` doc comment overstates, and the §5.4 "handed over" rule is relaxed to "about to be handed over"*
- Location: the `Inner::flush` doc comment, `service.rs` L766–770 in the working tree.
- Claim in the comment: "A reply the channel then refuses ends the session either way, so an early mark is never observed."
- Counterexample: `ByteStreamEnd::send` (`session-channel/src/byte_stream.rs`) turns a peer-side write failure (`peer_side_failure`, for example EPIPE) into a sticky `write_failed`. It returns `Err(Closed::Stream)`, and its own comment says "the session goes on reading until the stream ends." The host then only increments `dropped`, and nothing ends the session.
- Behaviour before and after the diff:
  - Old: a refused reply was never marked, so a client that can no longer receive eventually filled the table and got `SessionFull`.
  - New: the record is marked and evicted, so the client keeps getting admission.
  - Nothing in the dead direction can be observed, because every later send fails sticky. The effect is bounded.
- Related relaxation: the early mark also breaks the review's P2-3 wording, "set the flag when the reply is handed over." This is the redundant-call case from section 2.
- Required correction: do one of the following.
  - (a) Soften the comment to say that the early mark is unobservable on every path where the session ends. Name the byte-stream half-close case and state that the dropped-reply counter is the diagnostic.
  - (b) Record in §5.4 or the 2.2b notes that "delivered" means "committed to the sink by the sending thread."
- Regression test: optional. If wanted, a sink that returns `Closed` without ending the session. After the first refused reply, a submit that needs eviction should either be admitted (documented behaviour) or refused (fail-closed). Pin whichever is chosen.

*F-2 (P3, efficiency; not required): the extra lock acquisition on every flush*
- `flush` now takes the state lock once to mark and once more to decrement `unflushed`, even when `delivered` is empty. That is the case for most flushes, including every receive and every empty read-loop pass.
- A guard such as `if !delivered.is_empty()` removes it at no semantic cost.
- No correctness impact.

**P0/P1/P2 findings:** none. The diff fixes a real P2-class defect of mine to confirm: a spurious `SessionFull` for a client within limits. The root-cause analysis in the doc comment is correct.

Verdict: GO
