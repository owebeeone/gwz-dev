# Draft Python guide: concurrent operations on one GWZ client

This describes a **proposed**, unavailable API for review. It is not part of
the published `gwz` Python package. Existing `Client.fetch()` and
`Client.fetch_stream()` remain supported.

`Client.start_fetch(*, request_id=None, concurrency=None,
max_connections_per_host=None, placement="local")` is an awaited method
returning `OperationHandle[FetchResponse]`. `request_id` is an optional caller
correlation string. `concurrency` is the operation's maximum member workers
(default 100); `max_connections_per_host` is its maximum member work against
one host (default 32). They are ceilings, not promised thread or connection
counts. `placement` is `"local"` by default; `"cli"` is usable only when that
endpoint was bound in the same process. An unbound or unsupported `"cli"`
request raises `PlacementUnavailable` before Git work or credentials are read.
Other existing fetch metadata arguments keep their current meanings.

The handle identifies the operation as soon as it is accepted, before Git
work finishes or the first event arrives:

```python
from pathlib import Path
from gwz import Client, GwzOperationError

async with Client(root=Path(".")) as client:
    first = await client.start_fetch(request_id="fetch-a")
    second = await client.start_fetch(request_id="fetch-b")
    print(first.operation_id, second.operation_id)

    first_cleanup = await first.cancel()
    print(first_cleanup.pending_local_work,
          first_cleanup.peer_cleanup_confirmed)

    try:
        async for event in second.events():
            print(event.sequence, event.kind, event.member_id)
    except GwzOperationError as exc:
        if exc.code != "EventCursorGap":
            raise
        # Earlier events expired. Read the final result, or reopen events
        # from the oldest retained cursor exposed by the gap error.

    try:
        response = await second.result()  # generated FetchResponse
        print(response.repos)
    except GwzOperationError as exc:
        print(exc.code, exc.operation_result)

    await first.release()
    await second.release()
```

`handle.events()` is an async iterator over generated `OperationEvent` values.
It starts at the oldest event still retained when iteration begins, yields in
increasing per-operation `sequence` order and ends when the operation and
event stream finish. A new iterator can replay only events still retained.
A slow reader receives `EventCursorGap` with the oldest retained cursor;
it may restart there or read `handle.terminal()` for the final outcome.
Losing event history never erases the terminal result.

`await handle.result()` returns the same generated action response type as
the existing unary method on success. It raises `GwzOperationError` for a
Git failure, cancellation or output-limit failure; the exception carries a
stable `code`, `operation_id`, caller `request_id` and terminal
`operation_result` when available. `await handle.terminal()` returns the
immutable terminal record, including status, optional typed response,
cleanup facts and any typed error. `await handle.cancel()` targets only that
handle, joins its worker and transport cleanup, and returns
`TransportCleanup(pending_local_work, peer_cleanup_confirmed)`. A cancelled
operation's `result()` raises an error with cancellation code; it does not
return a successful fetch response. A result too large for the published
retention budget raises `ResultLimitExceeded` and reports whether Git work
may have applied; it never implies rollback. Cancellation of one handle
leaves its peers running.

`await handle.release()` frees the completed record and event log. It is
idempotent for that owned handle. Read results/events and any repeat
cancellation report before release; later reads or cancellation report
`OperationExpired`. A foreign client's handle is refused even if its caller
used the same `request_id`. Completed records otherwise expire 10 minutes
after completion or last owned read, with a hard 30-minute limit. Closing
the Client releases its records after joining its operations. Release of a
live reader does not cancel the operation; call `cancel()` or close first.

Version 1 initially fixes, rather than exposes knobs for, these resource
limits: 8 executing and 16 queued accepted operations per Client; 128 core
member workers per Client and 256 across the receiver; 64 retained terminal
records per Client; 16 MiB per full terminal record, 64 MiB total terminal
bytes per Client; 1 MiB events per operation and 8 MiB per Client. The
receiver allows 32 sessions and 4 per route. `await start_fetch()` raises
`CapacityBusy` when accepted-work/worker slots are full (retry after work
retires), `RetainedCapacityFull` when retained records or bytes are full
(release/expiry), and `SessionCapacityFull` when opening a Client's session
would exceed receiver or route capacity (close another session). A request
that needs to raise installed physical connection caps while another
operation is live raises `CapacityConflict` (retry after all such work and
cleanup retire). A lower-limit operation can overlap under higher installed
caps while observing its own lower work limit. The receiver publishes its
exact current budgets when opening a session.

The same admission errors arise when a unary convenience call begins. An
existing stream helper performs admission on its first iteration, so that
is where it can raise. Use the handle form when another task needs an
operation ID before the first event. `Client.cancel_operation(id)` remains
a convenience for an ID owned by that same Client; a caller-supplied
`request_id` is not a cancellation handle. Once `start_fetch()` accepts,
later Git/transport failures appear through `result()` or the stream's
terminal outcome, not as an admission error.

Leaving `async with Client(...)` cancels and joins outstanding work and
closes the shared endpoint. If an asyncio task waiting for close is cancelled,
cleanup continues; another `await client.close()` joins it. A five-second
wait can raise `ClosePending` while the same close task continues. A later
`close()` obtains the final `TransportCleanup`; a successful close means no
local handler continues to mutate the workspace. Repeat close returns that
report for 60 seconds, after which the closed session expires.
