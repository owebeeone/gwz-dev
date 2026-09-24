# GwzPyTransportSessionV4FoundationDesign — SURFACE-AXIS REVIEW

**Review object:** `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md` at gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`. It is a DRAFT caller guide dated 2026-09-24, and the API it describes is not active yet. The review covers the `request_id_consumed` note (line 53) and the retry example with its completeness sentence (lines 55–79). Both are judged as part of the whole 101-line guide.

**Baseline:**
- Workspace root: `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`
- gwz-core: `a1f2102dd6da1bf96538b4972de7995255a53b48`
- gwz-py: `0ca424f36a2872a0896abaea89641513352fdf47`

These three SHAs were the same at the start and end of the review. `git -C gwz-py status --short` on the guide was empty both times. How each source was read:
- The guide and the READMEs: `git -C gwz-py show 0ca424f:<path>`.
- The prior reports: `git show fbee49c:<path>`.
- The process documents: from the working tree, where `git status` showed them unmodified.

Nothing was read from gwz-core.

**Date:** 2026-09-24

**Axis:** Surface: the Python API as its caller guide presents it. The review attacks names read cold, lifecycle pairs and stated defaults, and runs a first-day walkthrough from the guide alone. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — 6 findings: 0 P0, 0 P1, 0 P2, 6 P3.
- Prior Surface P3-1 (code fence) stays closed.
- Prior Surface P3-2 (retry-example lifecycle) is **not closed**. It carries forward as P3-1 below, which also shows that the new line-79 completeness sentence is false. If this lane's exit criteria require P3-2 to close, P3-1 is the blocking item.
- All six findings are documentation defects that a text edit can fix. None is an architectural root cause.
- P3-1 and P3-4 concern text introduced in this revision.
- P3-2, P3-3, P3-5 and P3-6 concern text that has been present in substance since gwz-py `f6ae40a`. They are new findings, not regressions.

---

## 0. Evidence base

**Tuple checks**
- Commands run at start and at end: `git rev-parse HEAD`, `git -C gwz-core rev-parse HEAD`, `git -C gwz-py rev-parse HEAD`, and `git -C gwz-py status --short -- dev-docs/GwzPyConcurrentOperationsV2.md`.
- Results are as recorded in the Baseline. The status output was empty both times.

**Process authority (used for review procedure only)**
- `dev-docs/GwzProcessOptimization.md`, all 173 lines.
- `dev-docs/AgentProcessRules.md` L1-18 to L1-22: the Surface-axis amendment, the severity contract, the finding format and the closure rule.

**The object**
- Read with `git -C gwz-py show 0ca424f:dev-docs/GwzPyConcurrentOperationsV2.md`, all 101 lines, numbered with `nl -ba`.
- Line map:

| Lines | Content |
| --- | --- |
| 1–7 | Status and introduction |
| 9–36 | First example |
| 38 | Handle contract |
| 40–49 | Factory table |
| 51 | Common cases |
| 53 | Convenience forms, request-ID grammar and reuse, `request_id_consumed` definition |
| 55–77 | Retry example |
| 79 | Completeness sentence |
| 81 | Generated-ID sentence |
| 83–89 | Settings table |
| 91 | Limits and refusals |
| 93 | Cancellation race |
| 95 | `GwzOperationCancelled` |
| 97 | Streams and retention |
| 99 | Close |
| 101 | Threads and loops |

**Changes**
- `git -C gwz-py diff f6ae40a..0ca424f -- <guide>`:
  - The status link moves from v3 to v4.
  - The old fragment is replaced by the `fetch_once_with_retry` function.
  - Line 79 is added.
  - Line 53 and the generated-ID paragraph are unchanged.
- `git -C gwz-py diff d29d450..0ca424f -- <guide>` additionally shows:
  - the line-53 rewrite ("provided the Client and generation remain open", and the new False/True definitions);
  - the fence repair and the separate generated-ID paragraph.

**Package docs at 0ca424f**
- `README.md` (149 lines) does not link to the guide. It never mentions `request_id`, handles or `GwzBridgeError`.
- `src/README.md` (4 lines) is a one-line directory description.
- Neither README says where the exception classes are imported from.

**Prior Surface reports at fbee49c, read in full**
- `...V3FoundationDesign-ReviewSurface.md`: P3-1 (fence) and P3-2 (lifecycle).
- `...-ReviewSurface-1.md`: P3-1 closed, P3-2 partially closed. Its closure check includes the case where "the awaiting task is cancelled".

**Rendering checks** (read-only `grep` and `awk` over `git show` output)
- The code fences are at lines 9/36 and 55/77. Each is on its own line with a blank line around it.
- Every line other than the fences has an even number of backticks.
- The tables at lines 40–49 (4 pipes per row) and 83–89 (5 pipes per row) are consistent and set off by blank lines.
- There is a single H1.

**Not read or run**
- Any code.
- The v4 design; its link was not followed.
- Any plan, remediation plan, verdict or other-axis report.
- No builds or tests were run.

## 1. Findings

### [P3-1] The retry example leaks the handle when its task is cancelled while awaiting `accepted()`, and the line-79 claim is false (carries forward prior Surface P3-2)

**Location:** `GwzPyConcurrentOperationsV2.md` lines 58–70 (both `accepted()` awaits) and line 79.

**Violated invariant:** every handle the example creates must reach its documented terminal cleanup on every path, including task cancellation. The guide documents that cleanup for this exact path:
- Line 95: "Cancelling a task awaiting `handle.accepted()` leaves the previously returned handle available … You may still inspect and release the handle."
- Line 97: records that are not released count against the 64-record limit until their 15-minute deadline.

**Reproduction**
```python
task = asyncio.create_task(fetch_once_with_retry(client, "job-1"))
await asyncio.sleep(0)   # task is now awaiting the first handle.accepted() (line 59)
task.cancel()
```
1. `accepted()` raises `GwzOperationCancelled` after cleanup (line 95).
2. The guide calls that exception "a subclass of `asyncio.CancelledError`" and lists it separately from `GwzBridgeError` (line 53). So `except GwzBridgeError` at line 60 does not catch it.
3. The `try/finally` at lines 71–76 has not been entered yet. Nothing releases the handle, and the record stays in `recent_operations()` until its deadline.
4. The retried handle behaves the same way: a cancellation at line 67 escapes `except GwzBridgeError` at line 68.
5. If `GwzOperationCancelled` were instead meant to also be a `GwzBridgeError`, the result is worse. Line 60 would catch the first-handle cancellation, and with `request_id_consumed=False` the example would start a new fetch inside a task its owner had just cancelled.

Either class hierarchy breaks this path. Line 79 still says "Every handle the example creates reaches release … the retried handle whether it is refused, cancelled, fails or completes." That is true for cancellation during `result()`, but not during `accepted()`.

**Impact**
- A supervisor using `asyncio.timeout()`, `wait_for` or `TaskGroup` cancellation during admission leaks one record per cancellation.
- 64 such leaks within 15 minutes make `start_*` raise `TransportSessionFull` synchronously (line 91). All new network work on the Client is then refused until the records expire.
- The limit may be reached sooner where admission won the race. Line 91 does not say that the 8 MiB admission allowance shrinks when the terminal result is published.
- Line 79 tells callers the template is complete, so they have no reason to add the missing handler.

**Required correction**
- Give each handle a single scope that covers both `accepted()` and `result()` and releases it exactly once. One `try/finally` per attempt works, as does `except GwzOperationCancelled: await handle.release(); raise` around each `accepted()`.
- Never release a handle twice: line 38 says "A second release reports `OperationExpired`".
- State whether `GwzOperationCancelled` is a `GwzBridgeError`.
- Make line 79 list only the paths the code actually covers.

**Closure test:** walk the corrected example through each of these paths:
- the first `accepted()` cancelled before admission, and after admission won;
- the same two cases for the retried handle;
- the existing refused, consumed, second-refusal, completed, failed and `result()`-cancelled paths.

On every path, each handle is released exactly once and the cancellation still propagates.

### [P3-2] `request_id_consumed` has no stated value when a refusal happens because the ID is already registered, and the literal definition gives the wrong action

**Location:** line 53, these two passages:
- "a request ID successfully registered with core cannot be reused, even after completion; `accepted()` refuses it with `InvalidRequest` before effects"
- "`False` proves no registration … `True` means registration occurred or could not be ruled out …"

Also the example's branch at lines 62–65.

**Violated invariant:** for every pre-effect refusal the guide names, each value of the field must map to one correct caller action. The guide says "Do not work this out from the error code", so the field alone has to be enough.

**Reproduction**
1. Two tasks on one Client both call `client.start_fetch(request_id="job-7", targets=["mem_app"])`.
2. Task A is admitted.
3. Task B's `accepted()` is refused with `InvalidRequest`, because "job-7" is already registered.
4. The guide uses "consume" to describe a single attempt: "A pre-registration … refusal does **not** consume it"; "A failure **after** successful core registration … does consume it". On that reading, B's refusal registered nothing, so the literal definition gives `False`.
5. `False` "permits retry on a fresh handle only after the refusal's cause clears and the generation remains open". This cause never clears while the generation is open, because the ID "cannot be reused, even after completion".
6. If the name is read as describing the ID's state instead, the value is `True`. The guide does not say which reading is meant.
7. The example would retry once and raise a second `InvalidRequest`. A caller who implements the documented wait-then-retry, keyed on the field, would wait indefinitely or retry that ID in a loop for the rest of the generation.

**Impact:** this refusal is the one that concerns the request ID itself, and here the field does not answer its own question. Callers have to fall back on the error code, which the guide tells them not to use.

**Required correction**
- State that a refusal because the ID is already registered in the current generation reports `request_id_consumed=True`.
- Reword `True` as "this ID is registered in this generation, or that cannot be ruled out".
- Reword `False` as "this ID is not registered in this generation".

This wording matches the name, so no rename is needed. If the draft actually intends `False` for this case, then the name is misleading and the retry rule must exclude this case explicitly. That must be settled before release, because afterwards it would change an observable value.

**Closure test:** from line 53 alone, a reader can state the field's value for each refusal the guide names:
- duplicate-ID `InvalidRequest`;
- `TransportCapacityConflict`;
- `TransportSessionFull`;
- post-registration launch failure;
- pre-acceptance `GwzOperationCancelled`.

For the duplicate case, the value's action is "do not reuse in this generation".

### [P3-3] Whether `request_id_consumed` exists depends on the exception's state, and the `GwzOperationCancelled` attribute list leaves it out

**Location:**
- Line 53: "Every pre-effect refusal (`GwzBridgeError` with effect `none`, and `GwzOperationCancelled` raised before acceptance) carries `request_id_consumed`".
- Line 95: "`GwzOperationCancelled` … has `.handle`, `.operation_id`, `.request_id`, `.response: OperationResult | None`, and `.effect`". `request_id_consumed` is not in this list.

**Violated invariant:** an exception attribute that callers are told to branch on must be documented on its class, with its presence and value stated for every instance.

**Reproduction**
1. Cancel a task that is awaiting `handle.accepted()` just as admission completes.
2. Line 95 says the resulting `GwzOperationCancelled` has `.response` set to "the detached typed terminal result after acceptance" and `.effect == "possible"`. This exception comes from `accepted()` but arrives after acceptance, and line 53 defines the field only "before acceptance".
3. A handler such as `except GwzOperationCancelled as e: reuse = not e.request_id_consumed` either reads an undefined value or raises `AttributeError`. An `AttributeError` raised inside a `CancelledError` handler replaces the cancellation, which breaks `asyncio.timeout`, `wait_for` and `TaskGroup` handling.
4. Line 95 describes two cases, "after acceptance" and "before admission". It leaves out cancellation during admission after core registration, which is exactly when the field would be `True`.
5. Line 53 implies that some `GwzBridgeError` instances have an effect other than `none`. It never says whether those instances carry the field.

**Impact:** callers cannot write one handler that works for every instance. The corrected P3-1 example needs this attribute on `GwzOperationCancelled`. Readers who use line 95 as the class reference never learn that the attribute exists.

**Required correction**
- Add `.request_id_consumed: bool` to line 95.
- State that the attribute is present on every `GwzOperationCancelled` and every `GwzBridgeError`, and that it is `True` once registration has occurred, including after acceptance. Alternatively, state exactly when it is `None`.
- Define the case of cancellation during admission: its `.response`, `.effect` and field value.

**Closure test:** lines 53 and 95 together give the field's type and value for each case below:
- `GwzOperationCancelled` raised before admission;
- raised during admission after registration;
- raised after acceptance;
- raised from a unary or stream-helper task;
- a `GwzBridgeError` that is not a pre-effect refusal.

### [P3-4] The example's `finally` depends on a cancellation rule for `result()` that is stated only in its own comment

**Location:** the comment at lines 74–75: "Task cancellation joins the operation before GwzOperationCancelled is raised, so the record is terminal here and release succeeds on every exit". Compare:
- Line 38, which says `result()` "returns an `OperationResult` or raises `GwzOperationError`" and that readers are "independent … of `result()`".
- Line 95, which documents task cancellation only for tasks awaiting `accepted()` and for unary or stream-helper tasks.

**Violated invariant:**
- Any claim in an example comment must be backed by the reference text.
- The cancellation behavior of every awaitable handle method must be documented.

**Reproduction**
```python
h = client.start_push(refspec="main:main", targets=["mem_app"], request_id="rel-9")
await h.accepted()
try:
    async with asyncio.timeout(30):
        await h.result()
except TimeoutError:
    ...   # Was the push cancelled (effect "possible"), or is it still running?
```
1. Neither line 38 nor line 95 answers the question in the snippet.
2. The comment's "joins" has two readings: "cancel the operation, then join it" or "wait for it to finish normally".
3. Line 38's "independent … of result()" suggests a third reading: cancelling the waiter only stops the wait and leaves the operation running.
4. Under that third reading, the example's `finally` calls `release()` on an active record, which "reports `OpenOperation`" (line 38). "release succeeds on every exit" is then false, and if that report is raised, it replaces the `CancelledError`.
5. Line 101 endorses awaiting a handle from more than one event loop. Whether a second waiter's cancellation cancels the push then depends on this unstated rule.

**Impact:** callers cannot predict whether a timeout around `result()` cancels a push. The guide's bold warning about pushes whose effect is "possible" (line 93) depends on exactly that. The example's cleanup is correct under only one reading.

**Required correction:** in the handle contract (line 38 or 95), state for `result()` and for `events()`:
- what cancelling the awaiting task does to the operation;
- which exception is raised;
- whether the record is terminal on exit.

Then have the comment point to that rule.

**Closure test:** for each of `accepted()`, `result()` and `events()`, lines 38 and 95 state whether task cancellation cancels the operation, what is raised, and whether a following `release()` succeeds. The comment at lines 74–75 matches that text.

### [P3-5] The retry precondition "after the refusal's cause clears" cannot be observed, and the example retries immediately

**Location:**
- Line 64: "Retry once after the refusal's cause clears, only while this Client remains open." Lines 65–67 retry immediately after it.
- Line 53: "permits retry … only after the refusal's cause clears and the generation remains open".
- Line 91: "retry after the conflicting operation and cleanup retire".

**Violated invariant:**
- A precondition the guide puts on a retry must be observable through the documented API.
- The example must either implement that precondition or show where the caller must.

**Reproduction**
1. A fetch that inherited a 16-connection per-host limit is still running.
2. A second fetch overrides the limit to 32. The guide never names the per-operation keyword for this.
3. The second fetch is refused with `TransportCapacityConflict` and `request_id_consumed=False`.
4. Lines 65–67 retry at once, while the conflicting operation is still running. The retry meets the same refusal, releases the handle and raises.

The guide gives no signal for any part of the precondition:
- **Cleanup retirement.** Publication of the result is not said to follow cleanup. `recent_operations()` status is not said to reflect cleanup.
- **Client or generation still open.** There is no `closed` attribute. `close_report` is documented only after close, and it is `None` on custom bridges.
- **Transient or permanent.** The guide does not say which refusals that report `False` will ever clear. Two that would not:
  - `UnsupportedOperation` from a custom bridge (line 99);
  - a malformed-ID `InvalidRequest`, if it carries the field.

**Impact:** the guide's only retry template performs a retry that the note's own rule does not permit. For the two causes the guide names, that retry almost never succeeds. Callers have to invent their own polling or backoff with no documented signal.

**Required correction:** document when each refusal code clears:
- `TransportSessionFull`: release records found through `recent_operations()`, or wait for them to expire.
- `TransportCapacityConflict`: wait until the conflicting operation reaches a terminal state, then use bounded backoff if cleanup still lags.
- Permanent codes: do not retry.

Show the wait in the example, either as a documented step or as an explicit placeholder the caller fills in.

**Closure test:**
- Between the refusal and the retry, the example performs a wait that the guide defines for each transient code, or explicitly hands that wait to the caller.
- The guide names which refusals that report `False` never clear.

### [P3-6] The note's retry rule covers only handles, and the guide never says whether a refused unary or stream-helper call leaves a record

**Location:**
- Line 53: "Every pre-effect refusal … carries `request_id_consumed`: `False` … permits retry on a fresh handle".
- Line 91: "A stream helper reports admission refusal on its first iteration; a unary call reports it before performing Git work."
- Line 95: records for cancelled unary and stream tasks are released automatically, "so repeated task cancellations cannot exhaust retention".
- Lines 97 and 99: dropped streams and accepted records are kept until released by ID or until they expire.
- `OperationStream` exposes `operation_id`, `request_id`, `result()` and `aclose()`, but no `release()`.

**Violated invariant:** every form whose refusals carry the field must document what the caller has to release, if anything, before retrying with the same ID. The example itself stresses that "a refused handle holds a record until released or expired".

**Reproduction**
```python
while True:
    try:
        async for ev in client.fetch_stream(request_id="nightly", targets=["mem_app"]):
            ...
        break
    except GwzBridgeError as exc:
        if exc.request_id_consumed:
            raise
        await asyncio.sleep(5)
```
1. Each refused stream object is dropped.
2. The guide states retention only for dropped streams and accepted records. It says nothing about a refused unary or stream attempt. It also never says whether the refusal exception carries an `operation_id` that could be passed to `release_operation()`.
3. If refused attempts are kept like dropped streams, sustained contention piles up 64 records within 15 minutes. Every new operation then refuses with `TransportSessionFull`.
4. If refused attempts are released automatically, a caller who calls `release_operation()` just in case gets a report ("reports", line 38) whose delivery is unstated: it may be raised or returned.

**Impact:** callers who use the unary or streaming convenience forms cannot write a same-ID retry from the guide that is known not to leak records.

**Required correction**
- For unary calls and stream helpers, state whether a pre-effect refusal leaves a ledger record.
- If it does, say how to release it. Examples: document that the exception carries `operation_id` and show `client.release_operation(exc.operation_id)`, or release refused records automatically as cancelled ones already are.
- Extend line 53's "on a fresh handle" to say "or a fresh unary or stream call".

**Closure test:** the guide answers, for a refused unary call and for a refused stream helper, whether a record remains and how to release it. A same-ID retry loop written from the guide alone, for each form, leaves no refused records behind.

## 2. Invariant analysis

**Retry-example lifecycle.** Most attacks on the lifecycle failed:
- **Refused first handle.** It is released before any retry (line 61). When `True` is reported, its ID is not reused, because line 63 re-raises.
- **Second refusal.** It is released and re-raised (lines 69–70).
- **Paths that reach the `finally` release (lines 71–76), given the P3-4 premise:**
  - normal completion;
  - `GwzOperationError`;
  - operation cancellation through `cancel()` or `close()`;
  - task cancellation during `result()`.
- **Double release.** No path releases a handle twice, so line 38's second-release `OperationExpired` never occurs.
- **Record limit.** At most one record is held at a time, so the example cannot exhaust the 64-record limit on its own.
- **One admission per handle.** Each attempt uses a fresh handle, which agrees with line 91 ("cannot be admitted again").
- **Synchronous raises from `start_fetch`.** These happen before a handle exists, so nothing leaks. The two cases are a malformed-ID `InvalidRequest` (line 53) and `TransportSessionFull` for a 65th handle (line 91).
- **Closed Client.** It is handled whether `start_fetch` raises or returns a handle that is then refused.
- **Close during `result()`.** A `release()` after it is permitted by line 99.

The only path that fails is task cancellation during either `accepted()` await (P3-1).

**Comment claims checked against the guide**

| Line | Comment claim | Result |
| --- | --- | --- |
| 61 | A refused handle holds a record until released or expired | Agrees with lines 91, 97 and 99 |
| 63 | This generation will not accept the ID again | Agrees with line 53's `True` |
| 64 | Retry after the cause clears, while the Client is open | Not implemented by the code (P3-5) |
| 69 | Release and give up on this ID for now | Consistent |
| 72 | `GwzOperationError` carries `.response` and `.effect` | Agrees with lines 38 and 93 |
| 74–75 | Task cancellation joins the operation | Stated only in this comment (P3-4) |
| 79 | Every handle reaches release | False (P3-1) |

**The name and its values, read cold**
- **Values.** `request_id_consumed` is a boolean, and each value has a stated action.
- **Conservative `True`.** `True` may mean registration is only uncertain, and its action ("do not reuse … in that generation") is still safe in that case.
- **Attacks that succeeded.** The duplicate-ID case (P3-2) and the presence contract (P3-3).
- **Where the field appears:**
  - `GwzBridgeError` with effect `none`;
  - `GwzOperationCancelled` raised before acceptance;
  - `recent_operations()` descriptors;
  - the refused handle, through `result()` re-raising the retained error (lines 38 and 91).
- **Where it does not appear:** the `aclose()` outcome and close summaries. For close summaries this is acceptable, because the generation has already closed.

**Rendering.** The rendering attacks failed:
- Both code fences close on their own lines, so prior P3-1 stays closed.
- Inline code is balanced.
- Both tables are well formed.
- The single heading renders.

**Prior Surface P3-2**
- Now closed: release of the refused handle, release of the retried handle, and observation of the result.
- Still open: the case where the awaiting task is cancelled, which the prior closure check named, for `accepted()` (P3-1).

**First-day walkthrough, from the guide alone.** Points where I had to guess:
1. **Import.** Only `Client` and `GwzOperationError` are shown being imported (line 11). I guessed `from gwz import GwzBridgeError, GwzOperationCancelled`.
2. **Creating the Client and starting the fetch.** No guessing needed for `Client(root=...)`, `start_fetch(request_id=..., targets=[...])`, the ID grammar, `operation_id` or `request_id`.
3. **Provoking a pre-effect refusal.**
   - The keyword for the per-operation connection limit is never named. Line 86 says "optional per-operation override" and line 91 says "the operation keyword"; I guessed it.
   - `.code` and `.effect` on `GwzBridgeError` are inferred from the `GwzBridgeError(code=...)` notation.
4. **Deciding whether to retry.**
   - The value for the duplicate-ID case is a guess (P3-2).
   - When the cause has cleared is a guess (P3-5).
   - "Core generation" is never defined or exposed. I assumed it means the Client's lifetime, as the example's comment implies. Treating it conservatively is safe.
   - When a cancelled `accepted()` is wrapped in `asyncio.timeout` or `wait_for`, the caller sees `TimeoutError`. The field is then reachable only through `__cause__` or `recent_operations()`, and the guide does not say so.
   - Whether releasing a record frees its request ID is not stated. "Even after completion" suggests it does not.
5. **Retrying and observing `result()`.** No guessing needed.
6. **Releasing.**
   - What `release()` returns is not stated.
   - Whether `OpenOperation` and `OperationExpired` are raised or returned ("reports") is not stated.
   - What `handle.result()` raises after a cancelled `accepted()` is not stated.
7. **Closing.**
   - No guessing needed for exiting `async with`, `close_report`, or retention after close.
   - What `start_*` does on a closed Client is not stated (synchronous raise, or a handle that is then refused). The example handles both.

## 3. Risks and next action

**Residual risks below the finding bar**
- **"Core generation"** is caller-facing vocabulary that is never defined. It is harmless, because the `True` handling is conservative and line 53 allows the same ID on a different Client.
- **`release()` and `cancel()`:** what they return is not specified ("reports", "cleanup facts"). This matters for any code that releases in a `finally`.
- **Second cancellation during `release()`:** if a second cancellation arrives while `release()` is being awaited in the example's handlers or `finally`, the outcome is not documented.
- **Status-line link:** `../../dev-docs/GwzPyTransportSessionV4FoundationDesign.md` points outside the gwz-py repository, so it will not resolve in a standalone gwz-py checkout. I did not follow it.
- **README:** `README.md` still does not link to the guide. The prior round treated this as acceptable for a draft.
- **First example:** lines 14–35 release only on the success path. This is outside the object, and the process exits at the end in any case.
- **Round count:** this is the third Surface round in which the example's lifecycle is incomplete. Under the two-round cap, whether another correction round runs is the lane owner's decision. All six findings are text corrections, not architectural changes.

**Next action**
1. Revise the guide so that each attempt's `accepted()` and `result()` share one scope that releases the handle exactly once and handles `GwzOperationCancelled`.
2. Make line 79 list only the paths the code covers. Together with step 1, this closes P3-1 and prior P3-2.
3. Make the P3-2 through P3-6 clarifications in the same edit.
4. Run a single-axis Surface re-review of lines 53–79, 91 and 95 on the new commit.
