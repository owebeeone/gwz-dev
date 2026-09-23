# Draft Python guide: independent transport delivery and start tickets

This is a **proposed, unavailable** Python API. It is the complete caller guide for the new [start-ticket design](GwzOperationStartTicketDesign.md); the rejected v1 guide is historical only. Existing `Client.fetch()` and `Client.fetch_stream()` retain their current semantics until a separately reviewed release. The ordinary Python Client uses local endpoint placement; the internal `cli` placement has no public Python binding in this version. Transport delivery has its own loop-bound host when it crosses the Python/core boundary and is independent of action awaits and results.

`Client.start_fetch(...)` is **synchronous** and immediately returns a `StartTicket[FetchResponse]` with a unique `start_seq`, before any await or network work. If its 64 Client-owned ticket slots are full, it raises typed `CapacityBusy(resource=start-ticket)` synchronously and sends nothing. `await ticket.accepted()` later returns an `OperationHandle[FetchResponse]` after admission; `await ticket.result()` waits for that handle and its result. The method requires the Client's running event loop. Its candidate arguments are the existing `Client.fetch` metadata plus the planned retry option:

```python
def start_fetch(
    *, request_id: str | None = None, root: str | Path | None = None,
    workspace_id: str | None = None, all_members: bool | None = None,
    member_ids: Iterable[str] = (), paths: Iterable[str] = (),
    targets: Iterable[str] = (), exclude_targets: Iterable[str] = (),
    dry_run: bool | None = None, partial: bool | None = None,
    destructive: bool | None = None, sync: SyncBehavior | str | None = None,
    unsupported_member: UnsupportedMemberBehavior | str | None = None,
    remote: str | None = None, concurrency: int | None = None,
    progress_min_interval_ms: int | None = None,
    max_connections_per_host: int | None = None,
    attribution: OperationAttribution | None = None,
    transport: TransportOptions | None = None,
    max_retries: int | None = None,
) -> StartTicket[FetchResponse]: ...
```

With no selection arguments, fetch targets the workspace root and all members, as `Client.fetch()` does today; members declared local-only report no upstream rather than being contacted. Empty selector collections add no explicit selection. `root=None` uses the Client's root (or its normal current-directory resolution); `workspace_id=None` makes no additional identity assertion. `dry_run=None` runs normally (`False`); `all_members=None` uses the default root-plus-members selection; `partial=None`, `destructive=None`, `sync=None` and `unsupported_member=None` add no policy override. Fetch does not integrate fetched commits, so a missing sync override does not request merge/rebase/reset. `remote=None` uses each selected repository's configured/default remote; there is no one invented common remote name. `concurrency=None` resolves to a maximum of 100 member workers, `max_connections_per_host=None` to 32, `progress_min_interval_ms=None` to emit every event, and the **new proposed** `max_retries=None` to three extra setup attempts. `None` for attribution/transport uses the current Client/endpoint context; an explicit value overrides it. The actual worker and connection counts can be lower than the ceilings. A caller `request_id` is correlation only and must be ≤256 UTF-8 bytes; a generated ID is used when omitted.

A ticket is the **start identity**. Keep it even if a task awaiting it is cancelled. `StartTicket.start_seq` is unique and monotonic within its Client session; `StartTicket.request_id` is correlation only. An internal, never-reused transport request ID is generated separately and is not a public cancellation key. `Client.tickets()` returns a bounded snapshot of this Client's live/retained tickets, and `Client.ticket(start_seq)` retrieves the exact ticket until its local release or expiry. A caller ID may be generated or shared with another Client without making recovery ambiguous. A duplicate live caller ID on this same Client is refused. No other Client or replacement route may inspect or cancel the ticket.

`ticket.state` reports `preparing`, `prepared`, `committing`, `accepted`, `refused`, `cancelled`, or `unknown_or_expired`. `accepted()` is repeatable: once acceptance is known, every call returns the same operation handle/ID, including after a waiter is cancelled. Before acceptance it waits for admission, then raises the original typed refusal/cancellation if one occurred. `result()` has the same admission behavior, then awaits the accepted operation's result. Neither method consumes the ticket. `ticket.wait_cleanup()` is a bounded, repeatable read for a cancelled/refused start even if no operation handle exists; an accepted ticket delegates to its handle. It returns local cleanup progress, not a claim about remote rollback. A stateless no-slot refusal is already locally complete because no permit or work existed.

`await ticket.cancel()` works before and after acceptance and waits at most five seconds. It always returns `StartCancelProgress` with `phase`, `operation_id: str | None`, and `cleanup: OperationCleanupProgress`. Before a server permit is received, it prevents a commit from being sent; a late prepare reply cannot start work, `operation_id` is `None`, and local cleanup is complete. During server admission, cancellation seals only that start's owner and may return pending local cleanup. After acceptance it cancels exactly that operation and returns its ID in `operation_id`; the `cleanup` field can still be pending. If acceptance won the race, the original handle and terminal stay readable. Cancelled Python **waiters** do not cancel tickets; callers invoke `ticket.cancel()` when they want to stop work. A second healthy ticket is unaffected:

```python
first = client.start_fetch(request_id="fetch-a")
second = client.start_fetch(request_id="fetch-b")

first_handle = await first.accepted()
second_handle = await second.accepted()
progress = await first.cancel()       # only first; bounded wait
print(progress.operation_id, progress.cleanup.local_completion_confirmed)
try:
    second_response = await second.result()
    print(second_response.repos)
except GwzOperationError as exc:
    print(exc.code)                    # still retire this settled peer
finally:
    await second_handle.terminal()
    await second.release()

cleanup = progress.cleanup
while not cleanup.local_completion_confirmed:
    cleanup = await first.wait_cleanup()  # each observation waits at most 5 s
await first_handle.terminal()
await first.release()
```

Cancelling a wait cannot lose the start identity, including when request IDs are omitted or equal across Clients:

```python
a = client.start_fetch()
b = client.start_fetch()
waiter = asyncio.create_task(a.accepted())
waiter.cancel()
try:
    await waiter
except asyncio.CancelledError:
    pass
progress = await a.cancel()  # never selects b by request_id
recovered = client.ticket(a.start_seq)  # retain identity before its TTL
b_handle = await b.accepted()
try:
    b_response = await b.result()
except GwzOperationError as exc:
    print(exc.code)
finally:
    await b_handle.terminal()
    await b.release()

# The cancelled waiter did not consume a's identity or acceptance.
try:
    a_handle = await recovered.accepted()  # same handle if acceptance won
except GwzOperationError as exc:
    if exc.code != "OperationCancelled":
        raise  # an unknown/expired start needs inspection, not this path
    cleanup = await recovered.wait_cleanup()
    while not cleanup.local_completion_confirmed:
        cleanup = await recovered.wait_cleanup()
    await recovered.release()
else:
    cleanup = await a_handle.wait_cleanup()
    while not cleanup.local_completion_confirmed:
        cleanup = await a_handle.wait_cleanup()
    await a_handle.terminal()
    await recovered.release()
```

These loops may remain pending if native work never stops. You may stop polling, but keep the Client and ticket reachable; their charged owner remains until real cleanup. A five-second observation alone is never a reason to release a live handler. In either example, once a handle's terminal is available, release in a `finally` path even if its `result()` raised.

The Client sends a no-effect prepare first and sends commit **only after** receiving a server permit. Prepare reserves one bounded result/status slot for all later outcomes. If no slot exists, it returns a stateless `RetainedCapacityFull`: even if that reply is lost, this Client has not received a permit and cannot have sent commit, so retrying prepare is safe. If a prepare reply is lost, the Client retries the same sequence without sending commit. An old prepared permit can expire before that retry; the ticket then reports expiry, but the Client knows it sent no commit and may offer a **new** ticket after an explicit caller retry. If a commit reply is lost, the Client uses its known permit and owner-checked server status. It never starts Git work twice. A prepared but never committed permit expires after 60 seconds. Once commit has been sent, an unknown/expired server status is **not** proof of no effects and the Client never automatically resubmits that action. `accepted()` and `result()` raise `GwzStartUnknownError` (code `StartUnknownOrExpired`, numeric 83) with `start_seq`, `commit_sent`, `effects.local`, `effects.remote`, optional known `accepted_operation_id`, and optional `local_completion_confirmed`. If no permit was received and no commit sent, both effects are `none`; if commit was sent and no terminal proof survives, each domain the action can mutate is conservatively `may_have_applied`. For fetch, `remote` remains `none` because fetch cannot mutate remote Git state, while `local` may have applied. The optional operation ID never authorizes a replacement Client to inspect it. `wait_cleanup()` raises the same error when no retained cleanup record remains. This state requires inspection before any new action; it never triggers an automatic retry. A server result/refusal after commit remains retained under the reserved slot for its published retention period.

| Ticket state | `cancel()` | `wait_cleanup()` | `release()` |
| --- | --- | --- | --- |
| `preparing` or `prepared` | Seals the start; if a permit exists, sends owner-bound cancellation. Returns `StartCancelProgress(operation_id=None, cleanup=local_complete)`. | Reports current bounded local progress. | Refuses until the start settles. |
| `committing` | Signals that admission owner; returns the same `StartCancelProgress` shape, with an ID only if acceptance won. | Bounded read of the pre-accept owner. | Refuses until admission settles. |
| `accepted` | Cancels that operation; returns its ID and bounded cleanup progress. | Delegates to its handle. | Delegates to handle release; refuses before terminal. |
| `refused` or `cancelled` before acceptance | Idempotent, with no operation ID and truthful local progress. | Reads its retained cleanup owner, even if no handle exists. | Retires local ticket/status; active physical cleanup remains charged. |
| `unknown_or_expired`, **no commit sent** | Seals local admission, best-effort cancels a still-reachable permit, returns no ID and proved local completion. | Reports proved local completion; no effectful admission ran. | Retires the local ticket; a remote prepared permit, if any, expires without Git work. |
| `unknown_or_expired`, **commit sent** | Raises `GwzStartUnknownError` if owner status cannot be recovered; it cannot claim cancellation. | Raises the same error without a retained cleanup record. | Retires **only** the local ticket and its 64-slot charge; server work/cleanup ownership is untouched. |

Both unknown states count as locally settled for `ticket.release()`, even though the committed case has uncertain Git effects. Save the exception details before releasing: `Client.ticket(start_seq)` then fails, and a replacement Client cannot reattach. For example:

```python
ticket = client.start_fetch()
try:
    handle = await ticket.accepted()
except GwzStartUnknownError as exc:
    if exc.commit_sent:
        print(exc.effects.local, exc.effects.remote)  # inspect affected Git state
        # No automatic retry; cancel/wait cannot promise to reach a lost owner.
    else:
        progress = await ticket.cancel()
        assert progress.operation_id is None
    await ticket.release()  # local slot only in the commit-sent case
else:
    try:
        await handle.result()
    finally:
        await handle.terminal()
        await ticket.release()
```

After `ticket.accepted()`, each operation handle has `operation_id`, `events(after_sequence=None)`, `result()`, `terminal()`, `cancel()`, `wait_cleanup()` and `release()`. `events()` is an async iterator of generated `OperationEvent` in increasing sequence order, ending when that operation's stream ends. By default it starts at acceptance cursor zero. If events expired before a first or later read, it raises `EventCursorGap` with `oldest_retained_cursor`; pass `after_sequence=oldest_retained_cursor - 1` to resume at the oldest retained event, or read `terminal()` for the final outcome. Missing events never erase the terminal. A request ID is **not** a cancellation handle.

`cancel()` is idempotent, signals only its handle and returns `OperationCleanupProgress` when local cleanup finishes or the five-second wait expires. `wait_cleanup()` does not signal cancellation and has the same bounded wait. The return can be pending; it is never a falsely final cleanup report. The caller may stop waiting and use other handles. If a handler never stops, its terminal and local completion may remain pending and its capacity charge remains. A handler that succeeds while cancellation races it retains its success result; the request to cancel does not rewrite history. `result()` returns the generated action response on success, or raises `GwzOperationError` with stable code, operation/request IDs, `operation_result`, and its **original generated response in `exc.response`** if one exists. A cancelled handler's eventual terminal has kind `cancelled` and `result()` raises `GwzOperationError(code="OperationCancelled")`, so the same exception branch catches it. `ResultLimitExceeded` uses a bounded terminal with `exc.response=None`; do not assume the Git effect was rolled back. `terminal()` returns the immutable action outcome after the handler and every mutation-capable child join. It can appear before physical cleanup finishes.

`OperationCleanupProgress` contains `phase` (`running`, `cancelling`, `draining`, `local_complete`), `handler_running`, `pending_local_work`, `local_completion_confirmed`, `peer_cleanup_confirmed`, and `effects.local`/`effects.remote`. Only `local_completion_confirmed=True` proves the handler and local physical tasks have stopped. A zero pending count in an earlier snapshot is insufficient. `peer_cleanup_confirmed=False` may remain in a **final local** report; it records uncertainty about remote teardown, not a reason to close healthy peers. For each effect domain, `none` means that domain is **proved untouched by this operation**, `may_have_applied` means the final state is uncertain, and `applied` means that domain's effect is confirmed. `local` means local Git objects/refs or workspace changes; `remote` means mutation of remote Git state, not a network read or peer cleanup. Local and remote effects are separate; peer-cleanup confirmation does not change either. `terminal()` freezes conservative knowledge at handler join. Later cleanup progress may add specific proof of `applied` where the terminal said `may_have_applied`; use the latest progress for a retry decision. A terminal's `none` is never later changed to a possible effect.

| When cancellation occurs | Local effect | Remote effect | Retry decision |
| --- | --- | --- | --- |
| Before any local mutation or effectful request is sent, with both facts proved | `none` | `none` | A retry is safe **on effect grounds**, after normal preconditions are checked. |
| After an effectful remote request is sent but before its outcome is confirmed | `none` if local work is proved untouched; otherwise `may_have_applied` | `may_have_applied` | Inspect the local/remote state before retrying. Peer-cleanup confirmation does not prove the Git request failed. |
| After the remote Git effect is confirmed | `none`, `may_have_applied` or `applied` from local evidence | `applied` | Inspect whether the desired state is already present before starting another action. A final peer-cleanup flag can still be false. |

For **fetch**, remote reads and object transfer do not mutate remote Git state, so `effects.remote` is `none` throughout. Cancellation before a local object/ref update, including after a remote response, can report `effects.local=none` only when no local mutation is proved. During an uncertain local object/ref update it reports `may_have_applied`; after a confirmed update it reports `applied`. Inspect the fetched objects and local tracking refs before retrying when local effect is uncertain or applied. The table's remote-effect rows instead apply to operations that can write remote Git state, such as push.

`handle.release()` frees a completed terminal and event log, not a pending cleanup owner or its capacity charge. A live operation refuses release. Calling it again on the same Python handle succeeds from its local cache; a different/raw terminal lookup after release gets `OperationExpired`. A reachable handle can still `wait_cleanup()` after terminal release. Once cleanup is final, its small report remains readable for 60 seconds after local completion, or while its terminal remains retained; after release and that period it expires. A completed terminal otherwise has a 10-minute sliding, 30-minute hard retention limit. Foreign Clients cannot read or control it even if they use the same request ID.

`ticket.release()` retires its local ticket slot after admission has settled, including a locally settled `unknown_or_expired` state. For an accepted ticket it delegates to `handle.release()`; for a pre-accept refusal/cancellation it releases the retained status while preserving any still-active cleanup owner; for an unknown committed ticket it releases **only** the local slot and leaves any server owner untouched. A pending prepare/commit refuses release. Repeating release on the same ticket succeeds from its local cache. A released ticket no longer appears in `Client.tickets()` or `Client.ticket(start_seq)`, although that same ticket object may still read a retained cleanup marker until its 60-second expiry. Stateless no-slot refusals can be released immediately. Unreleased settled refusals expire 60 seconds **after local cleanup completes**; accepted ticket/terminal retention slides for 10 minutes with a 30-minute hard limit. An effect-uncertain committed ticket stays charged until explicit release or the hard limit; expiry never discards active physical work. Keep the object until you have observed the outcome you need, then release it to restore the 64-slot local budget.

`await client.close()` stops admission and cancels **all** outstanding handles and pre-accept starts on that Client. It does not collapse their distinct results into one aggregate outcome. Each call waits at most five seconds; `GwzOperationError(code="ClosePending")` carries current charged progress in `exc.progress` and means a later `close()` can join the same task. A final close means all of that Client's handlers, pre-accept work and local physical tasks ended; `peer_cleanup_confirmed` may still be false. After close starts and after final close, that **same Client** remains usable for `tickets()`, `ticket(start_seq)`, repeated ticket `accepted()`/`result()`, `terminal()`, `wait_cleanup()`, `events()` over retained events, and `release()` until each record's expiry; it cannot submit new operations. `accepted()` returns the same handle for a retained accepted ticket, raises the original typed refusal/cancellation for a settled pre-accept ticket, and raises `GwzStartUnknownError` for an unknown ticket. A retained closed-session read-only binding is charged with the records, not a new live endpoint. This also holds when `async with Client(...)` exits. Its exit propagates `ClosePending` if cleanup remains pending and preserves a body exception as context; a still-reachable Client can be queried and closed again. Dropping the Client transfers unfinished work to the receiver's cleanup owner but gives the caller no later reattachment path. Keep a Client reference if you need to inspect outcomes after close. A replacement route/Client cannot retrieve them.

For example, if an acceptance waiter was cancelled just before close, keep the original Client and sequence. A pending `close()` does not consume the ticket:

```python
ticket = client.start_fetch()
seq = ticket.start_seq
waiter = asyncio.create_task(ticket.accepted())
waiter.cancel()                 # cancels this Python wait only
try:
    await client.close()
except GwzOperationError as exc:
    if exc.code != "ClosePending":
        raise
recovered = client.ticket(seq)
try:
    handle = await recovered.accepted()  # same ID if acceptance won
except GwzStartUnknownError as exc:
    print(exc.effects.local, exc.effects.remote)  # inspect before any retry
    await recovered.release()
except GwzOperationError:
    await recovered.wait_cleanup()       # pre-accept close/cancel outcome
    await recovered.release()
else:
    await handle.terminal()              # may wait while handler is still pending
    await recovered.release()
```

Admission errors are typed and occur before `Accepted`; later Git/transport failures appear in `result()`/`terminal()`. A capacity refusal is `GwzOperationError` with `exc.capacity` containing `resource`, `scope`, `limit`, `in_use`, `retry_condition` and minimum `retry_after_ms=1000`; `exc.progress` is absent. `ClosePending` has `exc.progress` and no capacity context. These are typed Taut error fields, never parsed from error-message text. Neither context exposes another Client's operation IDs or credentials. Caller-owned blocker IDs may be present. `retry_after_ms` is a minimum backoff, not a promise that capacity will recover; for a blocker owned elsewhere, use exponential backoff with jitter capped at 30 seconds or stop retrying.

| Code | What can change it | What will not change it |
| --- | --- | --- |
| `CapacityBusy` | Completion of charged operation, worker, endpoint-constructor or physical cleanup; await a listed caller-owned handle when available, otherwise back off. | Releasing a terminal alone while physical work is still pending. |
| `CapacityBusy(resource=start-ticket)` | Release or let a completed ticket expire, then start a new ticket. This refusal occurs synchronously and sends nothing. | Repeating the same `start_fetch()` call while all 64 ticket slots remain held. |
| `RetainedCapacityFull` | This prepare refusal has no permit or Git work. The typed context selects a real remedy from occupied slots: `terminal-record/record_release_or_expiry` for a prepared or settled record whose retention expiry frees a slot; `cleanup-marker/marker_expiry` only for a locally final cleanup-only marker with its 60-second TTL running; `cleanup-record` or `operation-scope` with `owned_cleanup`/`external_cleanup` for active work. Release the refused local ticket too. | An active cleanup owner has no 60-second expiry; releasing a terminal can leave its marker charged. Repeating a cached `release()` does not free that marker. |
| `SessionCapacityFull` | Close/expiry of a live logical session within the relevant route/receiver limit. | Changing `jobs`. |
| `CapacityConflict` | All admitted/orphaned scopes and physical cleanup in the installed capacity epoch actually become quiescent; then a later request may install higher caps. | Releasing a result, a five-second timeout, or merely losing a route. |
| `PlacementUnavailable` | Bind a supported placement through a separately reviewed API, or choose local placement. | Retrying the same unbound `cli` request. |

Version 1 initially fixes 32 live logical sessions per receiver (4 per route), 32 active-plus-orphan endpoint owners, 128 active-plus-orphan operation/cleanup scopes, 8 executing and 16 queued operations per Client, 64 operation workers and 256 member workers per receiver, 64 retained prepared/terminal/cleanup records and 64 live-or-retained Python tickets per Client, four live/draining transport generations per Client, and finite terminal/event byte budgets (16 MiB per full terminal, 64 MiB terminal and 8 MiB events per Client). A final cleanup-only marker and a refused-start status each expire after 60 seconds. These limits are published at session open; a local-only Client can open while all endpoint slots are held, but its first network start then refuses `CapacityBusy` before endpoint construction. A Python-supplied endpoint binds its delivery pumps to the Client's running event loop for their whole transport lifetime, independent of these ticket/result awaits. If that loop closes while transport work remains, the binding fails outstanding streams and retains local cleanup ownership; no success or rollback is inferred. A core-local Rust endpoint runs its pumps in Rust and does not route its internal frames through Python. The proposed API does not promise a deadline for a stuck native handler or physical cleanup. It preserves ownership and reports bounded saturation rather than silently releasing a live resource.
