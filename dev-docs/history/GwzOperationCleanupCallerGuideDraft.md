# Draft Python guide: cancelling one operation without closing its peers

This is a **proposed, unavailable** replacement for the cancellation and close portion of the rejected [operation-session caller guide](GwzOperationSessionCallerGuideDraft.md). It is a standalone user-facing review page for this lifecycle; the existing `Client.fetch()` API is unaffected. No public Python version of these methods has been released.

`await client.start_fetch(...)` is proposed to return an `OperationHandle` as soon as the operation is accepted. Use the handle to observe the result, cancel that operation, and wait for its cleanup. Cancelling one handle does not close the `Client` or cancel another handle on it.

```python
first = await client.start_fetch(request_id="fetch-a")
second = await client.start_fetch(request_id="fetch-b")

progress = await first.cancel()  # signals only first; waits up to five seconds
while not progress.local_completion_confirmed:
    # first still owns resources; second continues independently
    progress = await first.wait_cleanup()  # waits up to five seconds per call

if not progress.peer_cleanup_confirmed:
    # Local work ended, but do not assume the remote did or did not apply it.
    inspect_first_before_retrying = True

second_response = await second.result()
await first.release()
await second.release()
```

`cancel()` is idempotent. Each call requests cancellation of that handle and returns `OperationCleanupProgress` after local completion or at most five seconds. `wait_cleanup()` does not send a new cancellation request; each call waits up to five seconds and returns the latest progress. Both methods can return while cleanup is pending. A pending return is **not** a successful cleanup report; repeat `wait_cleanup()` later if local completion matters to the caller. Neither method closes the shared client. A successful handler completion that races a cancellation request stays successful in `result()`; cancellation does not rewrite an already completed Git result. `result()` may remain pending while a native handler is still running, even after `cancel()` returns.

Progress has `phase` (`running`, `cancelling`, `draining`, `local_complete`), `handler_running`, `pending_local_work`, `local_completion_confirmed`, `peer_cleanup_confirmed`, and `effect` (`none`, `may_have_applied`, `applied`). Only `local_completion_confirmed=True` establishes that the handler and all local physical work have stopped; a momentary zero in `pending_local_work` alone does not. `peer_cleanup_confirmed=False` can be a **final** report: it records remote uncertainty, not local work left to join. It never by itself requires `client.close()`. Check the operation's result and remote state before retrying an effectful network action when the effect is uncertain.

`result()` exposes the immutable Git/action outcome once the handler and its mutation-capable children join. It does not wait for unrelated physical cleanup. A cancellation that actually stops the handler yields the proposed `OperationCancelled` outcome; Git failure retains its generated action response in the exception. `terminal()` exposes the same immutable action outcome. Cleanup progress is read separately and may change from pending to final after the terminal appears. `release()` discards a completed terminal and its event log, but it never abandons a pending cleanup owner or its capacity charge. A still-reachable handle can call `wait_cleanup()` after releasing its terminal. If the handle/Client is dropped, the receiver keeps ownership; dropping a Python reference is not proof of cleanup.

`await client.close()` stops new admissions and cancels **all** outstanding operations on that Client, including `second`. It waits up to five seconds per call. `ClosePending` means the shared close task and its resource charges remain active; the same Client can call `close()` again. A final close report means all its handlers joined and local cleanup drained, so no handler from that Client can still mutate the workspace. It may still state `peer_cleanup_confirmed=False`. Leaving `async with Client(...)` starts this same close; if cleanup remains pending, the exit propagates `ClosePending` and the Client object can be used for another `close()` while it remains reachable. If it is dropped instead, the receiver retains the pending cleanup without a caller-visible handle. There is no promised maximum time to local completion when a native handler or physical disposal does not stop.

All pending work and retained results count against finite receiver limits. If another start is refused with `CapacityBusy`, wait for relevant cleanup to finish or release completed result records as indicated by the error; do not retry in a tight loop. A request that needs to raise physical connection limits while pending cleanup holds the old capacity epoch receives `CapacityConflict`. If cleanup never completes, the receiver may continue refusing new work rather than silently freeing its physical charge.
