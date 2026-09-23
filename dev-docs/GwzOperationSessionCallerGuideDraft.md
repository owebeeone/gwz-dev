# Draft Python guide: concurrent operations on one GWZ client

This describes a **proposed**, unavailable API for review. It is not part of
the published `gwz` Python package. Existing `Client.fetch()` and
`Client.fetch_stream()` remain supported.

`Client.start_fetch(...)` is an awaited method returning
`OperationHandle[FetchResponse]`. It accepts the full current fetch metadata
set with these proposed defaults:

```python
async def start_fetch(
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
) -> OperationHandle[FetchResponse]: ...
```

These arguments select the same workspace, members, policy, remote,
attribution and transport options as `Client.fetch(...)`; omitted `None`
values retain its existing resolution rules. The listed empty collections
select nothing explicitly. `request_id` is a caller correlation string,
limited to 256 UTF-8 bytes; a longer value raises `InvalidRequest` before
acceptance. `concurrency` resolves to the operation's maximum 100 member
workers by default, `max_connections_per_host` to 32, and `max_retries` to
3; they are ceilings, not promised thread/connection counts. `None` for
`progress_min_interval_ms` keeps the existing emit-every-event policy.
The ordinary Python Client uses local endpoint placement. The protocol's
`cli` placement is reserved for an internally bound endpoint and has no
public Python binding API in this version.

The handle identifies the operation as soon as it is accepted, before Git
work finishes or the first event arrives:

```python
from pathlib import Path
from gwz import Client, GwzOperationError, OperationTerminalKindV1

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
        # Earlier events expired. Resume from the earliest retained event.
        async for event in second.events(
            after_sequence=exc.oldest_retained_cursor - 1
        ):
            print(event.sequence, event.kind, event.member_id)

    try:
        response = await second.result()  # generated FetchResponse
        print(response.repos)
    except GwzOperationError as exc:
        print(exc.code, exc.response, exc.operation_result)

    first_terminal = await first.terminal()
    if first_terminal.kind == OperationTerminalKindV1.cancelled:
        print("first fetch was cancelled")

    await first.release()
    await second.release()
```

`handle.events(after_sequence=None)` is an async iterator over generated
`OperationEvent` values. By default it starts at the cursor captured when
the operation was accepted (sequence zero), yields in increasing
per-operation `sequence` order and ends when the operation and event stream
finish. A new iterator can replay only retained events. A reader that starts
late **or** falls behind receives `EventCursorGap` rather than a silent
suffix. That exception carries `oldest_retained_cursor`; pass one less than
it as `after_sequence` to resume, or read `handle.terminal()` for the final
outcome.
Losing event history never erases the terminal result.

`await handle.result()` returns the same generated action response type as
the existing unary method on success. It raises `GwzOperationError` for a
Git failure, cancellation or output-limit failure; the exception carries a
stable `code`, `operation_id`, caller `request_id` and terminal
`operation_result` when available. If the operation produced a generated
action response, `exc.response` is that **original decoded response**, even
on rejected, failed, partial, dirty or conflicted outcomes; this includes
merge recovery fields. For `ResultLimitExceeded`, no full response exists:
`exc.response is None`, `exc.code == "ResultLimitExceeded"` and `exc.effect`
is `"none"`, `"may_have_applied"` or `"applied"`.
`await handle.terminal()` returns the immutable terminal record, including
its `kind` (`succeeded`, `failed`, `cancelled`, `output_limited`), aggregate
status, optional typed response, cleanup facts and typed error. A cancelled
`result()` raises `GwzOperationError(code="OperationCancelled")`; the
`terminal().kind` branch in the example identifies the same outcome.
`await handle.cancel()` targets only that
handle, joins its worker and transport cleanup, and returns
`TransportCleanup(pending_local_work: int, peer_cleanup_confirmed: bool)`.
`pending_local_work > 0` means local cleanup is still outstanding;
`peer_cleanup_confirmed is False` means peer cleanup was not confirmed.
In either case, do not treat the cleanup as complete: await `client.close()`
to join the shared cleanup, and if it raises `ClosePending`, await it again
later. A final report can still say peer cleanup was unconfirmed; preserve
that fact when deciding whether to retry network work. Cancellation of one
handle leaves its peers running.

`await handle.release()` frees the completed record and event log. It is
idempotent on the same Python handle after a successful release; no second
native call is made. A different handle or raw lookup after release reports
`OperationExpired`. Releasing a live operation raises `InvalidRequest` and
does not cancel it. Read results/events and any repeat
cancellation report before release; later reads or cancellation report
`OperationExpired`. A foreign client's handle is refused even if its caller
used the same `request_id`. Completed records otherwise expire 10 minutes
after completion or last owned read, with a hard 30-minute limit. Closing
the Client releases its records after joining its operations. Call
`cancel()` or close before releasing live work.

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
