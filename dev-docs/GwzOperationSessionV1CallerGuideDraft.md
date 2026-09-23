# Draft Python guide: concurrent operations on one GWZ Client

This is a **proposed, unavailable** Python API. It replaces the earlier rejected operation-session caller draft in full. Existing `Client.fetch()` and `Client.fetch_stream()` remain available under their current semantics until a separately reviewed release. The protocol's internal `cli` placement has no public Python binding in version 1; the ordinary Client uses local endpoint placement.

`await Client.start_fetch(...)` returns an `OperationHandle[FetchResponse]` as soon as the request is accepted, before Git work or its first event. Its candidate arguments are the existing `Client.fetch` metadata plus the planned retry option:

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

With no selection arguments, fetch targets the workspace root and all members, as `Client.fetch()` does today; members declared local-only report no upstream rather than being contacted. Empty selector collections add no explicit selection. `root=None` uses the Client's root (or its normal current-directory resolution); `workspace_id=None` makes no additional identity assertion. `dry_run=None` runs normally (`False`); `all_members=None` uses the default root-plus-members selection; `partial=None`, `destructive=None`, `sync=None` and `unsupported_member=None` add no policy override. Fetch does not integrate fetched commits, so a missing sync override does not request merge/rebase/reset. `remote=None` uses each selected repository's configured/default remote; there is no one invented common remote name. `concurrency=None` resolves to a maximum of 100 member workers, `max_connections_per_host=None` to 32, `progress_min_interval_ms=None` to emit every event, and the **new proposed** `max_retries=None` to three extra setup attempts. `None` for attribution/transport uses the current Client/endpoint context; an explicit value overrides it. The actual worker and connection counts can be lower than the ceilings. A caller `request_id` is correlation only and must be ≤256 UTF-8 bytes; a generated ID is used when omitted.

Each handle has `operation_id`, `events(after_sequence=None)`, `result()`, `terminal()`, `cancel()`, `wait_cleanup()`, and `release()`. `events()` is an async iterator of generated `OperationEvent` in increasing sequence order, ending when that operation's stream ends. By default it starts at the acceptance cursor (sequence zero). If events expired before the first or a later read, it raises `EventCursorGap` with `oldest_retained_cursor`; pass `after_sequence=oldest_retained_cursor - 1` to resume at the oldest available event, or read `terminal()` for the final outcome. Missing events never erase the terminal. A caller-supplied request ID is **not** a cancellation handle; `Client.cancel_operation(id)` may be a convenience but resolves only IDs owned by that Client.

Cancellation and peer observation are independent:

```python
first = await client.start_fetch(request_id="fetch-a")
second = await client.start_fetch(request_id="fetch-b")

progress = await first.cancel()  # signals only first; waits at most 5 seconds
second_response = await second.result()  # does not wait for first's cleanup
print(second_response.repos)

# Optional: check first's cleanup later, without closing the shared Client.
progress = await first.wait_cleanup()  # at most 5 seconds per call
if not progress.local_completion_confirmed:
    print("first still owns local resources; check again later")
else:
    print("first local work ended", progress.peer_cleanup_confirmed)

first_outcome = await first.terminal()  # may still wait for first's handler
await first.release()                   # does not abandon pending cleanup
await second.release()
```

`cancel()` is idempotent, signals only its handle and returns `OperationCleanupProgress` when local cleanup finishes or the five-second wait expires. `wait_cleanup()` does not signal cancellation and has the same bounded wait. The return can be pending; it is never a falsely final cleanup report. The caller may stop waiting and use other handles. If a handler never stops, its terminal and local completion may remain pending and its capacity charge remains. A handler that succeeds while cancellation races it retains its success result; the request to cancel does not rewrite history. `result()` returns the generated action response on success, or raises `GwzOperationError` with stable code, operation/request IDs, `operation_result`, and its **original generated response in `exc.response`** if one exists. A cancelled handler's eventual terminal has kind `cancelled` and `result()` raises `OperationCancelled`. `ResultLimitExceeded` uses a bounded terminal with `exc.response=None`; do not assume the Git effect was rolled back. `terminal()` returns the immutable action outcome after the handler and every mutation-capable child join. It can appear before physical cleanup finishes.

`OperationCleanupProgress` contains `phase` (`running`, `cancelling`, `draining`, `local_complete`), `handler_running`, `pending_local_work`, `local_completion_confirmed`, `peer_cleanup_confirmed`, and `effects.local`/`effects.remote`. Only `local_completion_confirmed=True` proves the handler and local physical tasks have stopped. A zero pending count in an earlier snapshot is insufficient. `peer_cleanup_confirmed=False` may remain in a **final local** report; it records uncertainty about remote teardown, not a reason to close healthy peers. For each effect domain, `none` means that domain is **proved untouched by this operation**, `may_have_applied` means the final state is uncertain, and `applied` means that domain's effect is confirmed. Local and remote effects are separate; peer-cleanup confirmation does not change either. `terminal()` freezes conservative knowledge at handler join. Later cleanup progress may add specific proof of `applied` where the terminal said `may_have_applied`; use the latest progress for a retry decision. A terminal's `none` is never later changed to a possible effect.

| When cancellation occurs | Local effect | Remote effect | Retry decision |
| --- | --- | --- | --- |
| Before any local mutation or effectful request is sent, with both facts proved | `none` | `none` | A retry is safe **on effect grounds**, after normal preconditions are checked. |
| After an effectful remote request is sent but before its outcome is confirmed | `none` if local work is proved untouched; otherwise `may_have_applied` | `may_have_applied` | Inspect the local/remote state before retrying. Peer-cleanup confirmation does not prove the Git request failed. |
| After the remote Git effect is confirmed | `none`, `may_have_applied` or `applied` from local evidence | `applied` | Inspect whether the desired state is already present before starting another action. A final peer-cleanup flag can still be false. |

`release()` frees a completed terminal and event log, not a pending cleanup owner or its capacity charge. A live operation refuses release. Calling `release()` again on the same Python handle succeeds from its local cache; a different/raw terminal lookup after release gets `OperationExpired`. A reachable handle can still `wait_cleanup()` after terminal release. Once cleanup is final, its small report remains readable for 60 seconds after local completion, or while its terminal remains retained; after release and that period it expires. A completed terminal otherwise has a 10-minute sliding, 30-minute hard retention limit. Foreign Clients cannot read or control it even if they use the same request ID.

`await client.close()` stops admission and cancels **all** outstanding handles on that Client. It does not collapse their distinct results into one aggregate outcome. Each call waits at most five seconds; `ClosePending` carries current charged progress and means a later `close()` can join the same task. A final close means all of that Client's handlers and local physical tasks ended; `peer_cleanup_confirmed` may still be false. After close starts and after final close, that **same Client** remains usable for `result()`, `terminal()`, `wait_cleanup()`, `events()` over retained events, and `release()` until each record's expiry; it cannot submit new operations. A retained closed-session read-only binding is charged with the records, not a new live endpoint. This also holds when `async with Client(...)` exits. Its exit propagates `ClosePending` if cleanup remains pending and preserves a body exception as context; a still-reachable Client can be queried and closed again. Dropping the Client transfers unfinished work to the receiver's cleanup owner but gives the caller no later reattachment path. Keep a Client reference if you need to inspect outcomes after close. A replacement route/Client cannot retrieve them.

Admission errors are typed and occur before `Accepted`; later Git/transport failures appear in `result()`/`terminal()`. A capacity error includes `resource`, `scope`, `limit`, `in_use`, `retry_condition` and minimum `retry_after_ms=1000`, but never another Client's operation IDs or credentials. Caller-owned blocker IDs may be present. `retry_after_ms` is a minimum backoff, not a promise that capacity will recover; for a blocker owned elsewhere, use exponential backoff with jitter capped at 30 seconds or stop retrying.

| Code | What can change it | What will not change it |
| --- | --- | --- |
| `CapacityBusy` | Completion of charged operation, worker, endpoint-constructor or physical cleanup; await a listed caller-owned handle when available, otherwise back off. | Releasing a terminal alone while physical work is still pending. |
| `RetainedCapacityFull` | Release completed owned terminals/cleanup markers or wait for their expiry. | Repeatedly cancelling already drained work. |
| `SessionCapacityFull` | Close/expiry of a live logical session within the relevant route/receiver limit. | Changing `jobs`. |
| `CapacityConflict` | All admitted/orphaned scopes and physical cleanup in the installed capacity epoch actually become quiescent; then a later request may install higher caps. | Releasing a result, a five-second timeout, or merely losing a route. |
| `PlacementUnavailable` | Bind a supported placement through a separately reviewed API, or choose local placement. | Retrying the same unbound `cli` request. |

Version 1 initially fixes 32 live logical sessions per receiver (4 per route), 32 active-plus-orphan endpoint owners, 128 active-plus-orphan operation/cleanup scopes, 8 executing and 16 queued operations per Client, 64 operation workers and 256 member workers per receiver, 64 retained terminal records per Client, and finite terminal/event byte budgets (16 MiB per full terminal, 64 MiB terminal and 8 MiB events per Client). These limits are published at session open; a local-only Client can open while all endpoint slots are held, but its first network start then refuses `CapacityBusy` before endpoint construction. The proposed API does not promise a deadline for a stuck native handler or physical cleanup. It preserves ownership and reports bounded saturation rather than silently releasing a live resource.
