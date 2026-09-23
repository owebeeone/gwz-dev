# Draft Python guide: concurrent operations on one GWZ client

This describes a **proposed**, unavailable API for review. It is not part of
the published `gwz` Python package. Existing `Client.fetch()` and
`Client.fetch_stream()` remain supported.

An operation handle lets you start work, identify it before the first progress
event, then observe or cancel that one operation:

```python
from pathlib import Path
from gwz import Client

async with Client(root=Path(".")) as client:
    first = await client.start_fetch()
    second = await client.start_fetch()

    print(first.operation_id, second.operation_id)
    await first.cancel()
    result = await second.result()
```

`start_fetch()` returns only after the client accepts the operation, not after
Git work finishes. Each handle belongs to the client that created it. You may
iterate `first.events()` and await `first.result()` in separate tasks. Cancelling
one handle leaves other operations running. `Client.cancel_operation(id)` is a
convenience for a handle owned by that same client. Another client cannot read
or cancel it, even if both callers supplied the same `request_id`.

The client has finite concurrent-work and retained-result budgets. If a new
operation cannot be admitted, `await client.start_fetch()` raises a typed
admission error before an operation ID is issued. `CapacityBusy` means the
client's live-work budget is full; retry when work retires. `CapacityConflict`
means a requested physical connection ceiling cannot be installed while the
current connections are in use; retry after their operations and cleanup end.
An unsupported explicit endpoint placement raises `PlacementUnavailable` and
will not succeed unchanged. Accepted work reports a later Git or transport
failure through `handle.result()`, not as an admission error.

The same admission errors arise when a unary convenience call begins. A stream
helper performs admission on its first iteration, so that is where it can
raise. Use the handle form when another task needs an operation ID before the
first event. A caller-supplied `request_id` is for correlation and is not a
cancellation handle.

`await handle.cancel()` joins that operation's cleanup and returns its report.
A repeated cancellation returns the same report while the completed record is
retained. `handle.release()` frees the retained result and event log when you
have finished reading them. Once a record expires or is released, reads and
repeat cancellation report `OperationExpired`; a handle from a different
client is refused. Event readers that fall behind the bounded log receive a
typed cursor-gap error; the final result remains available until its own
release or expiry.

Leaving `async with Client(...)` cancels and joins all outstanding operations
and closes the shared endpoint. If an asyncio task waiting for close is
cancelled, cleanup continues. A later `await client.close()` joins the same
cleanup and returns its final report.
