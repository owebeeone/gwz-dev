# GwzPyTransportSessionV4FoundationDesign — SURFACE-AXIS RE-REVIEW

**Review object:** `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md` at gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`. It is a DRAFT caller guide dated 2026-09-24, for an API that is not yet active. This is remediation round 1. It re-checks round-1 findings P3-1 to P3-6 and attacks the text changed since gwz-py `0ca424f` (lines 3, 38, 53–88, 100 and 104).

**Baseline:**
- Workspace root: `11358352a237a673ecf0fd82347b45ca4aca8950`
- gwz-core: `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`
- gwz-py: `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`

All three SHAs matched at the start and end of the review. `git -C gwz-py status --short` on the guide was empty both times. The guide and READMEs were read with `git -C gwz-py show 0ffc4cb:<path>`. The filed round-1 report was read with `git show 1135835:<path>`, headings only, to align finding IDs. Nothing was read from gwz-core.

**Date:** 2026-09-24

**Axis:** Surface: the Python API as its caller guide presents it. The review covers names read cold, lifecycle pairs, stated defaults, and a first-day walkthrough using only the guide. It was independent, adversarial and read-only. The other axes run in parallel, and nothing here relies on them. The lane owner files this report verbatim.

**Verdict: NO-GO.** 4 new findings: 0 P0, 0 P1, 1 P2, 3 P3. All six round-1 findings (P3-1 to P3-6) are closed on the corrected tree. The one blocker is new text. The `TransportGenerationBusy` refusal has a name that reads as a temporary condition, but it means the Client's transport has permanently failed (P2-1). To avoid clashing with round-1 IDs, new P3 findings continue the numbering at P3-7. **I pre-commit to GO on a revision that resolves P2-1 as specified. P3-7 to P3-9 do not block.**

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P3-1 | The example is rewritten so each handle has one scope covering `accepted()` and `result()`. Each handle is released exactly once. A still-running operation is cancelled and joined before release. Cancellation is re-raised and never retried. The completeness sentence lists only covered paths. The guide states that `GwzOperationCancelled` is not a `GwzBridgeError`. | I re-ran the round-1 counterexample: task cancellation at the first `accepted()` and at the retried `accepted()`, both before and after admission. In each case `GwzOperationCancelled` is not caught by `except GwzBridgeError` (L74) and is caught by `except BaseException` (L80). Then `cancel()` runs, the exception is re-raised, and `finally` calls `release()` (L84). No retry happens. The class hierarchy is stated at L55 and L104. L88 lists only covered paths. The cleanup calls themselves still have two contract gaps, filed separately as P3-7 and P3-8. | Closed |
| P3-2 | The field means "this request ID is registered in this generation, or that cannot be ruled out". A refusal because the ID is already in use reports `True`. | I re-ran the case of two tasks using the same ID. B's refusal reports `True` (L55 sentence, L63 row), and the table says "Never in this generation: choose a new request ID". The field's name and definition now agree. | Closed |
| P3-3 | `.request_id_consumed` is listed on `GwzOperationCancelled` and is present on every `GwzBridgeError`, `GwzOperationError` and `GwzOperationCancelled`. Cancellation during admission, after registration, is defined. | L104 lists the attribute. L55 says it is present on all three classes and on descriptors. Before registration it is `False` ("True once core registered the ID"). During admission after registration it is `True`, with `.response` `None` and `.effect` `"none"`. After acceptance it is `True`. Unary and stream-helper tasks are covered by L55. | Closed |
| P3-4 | Cancelling a task that awaits `result()` or iterates `events()` stops only that wait. The operation keeps running and remains cancellable. `cancel()` finishes its join even if the task is cancelled again. | The round-1 case of a timeout around a push now has a documented answer: the push keeps running (L38). The handler calls `cancel()` before `release()`, and L88 cites both rules. One clause of my closure test is still unmet: the guide does not say which exception class a cancelled wait raises. This is below the bar. The example catches `BaseException`, and no outcome is carried by that exception. | Closed (residual in §3) |
| P3-5 | A table states, for each refusal, when the same request ID can be retried. The example takes a caller-supplied wait before its single retry. | Table L57–65 gives a retry condition for each refusal and names the `False` refusals that never clear (L62, L65). The example hands the wait to `wait_for_cause_to_clear` before retrying (L85). The round-1 immediate-retry counterexample no longer applies. A new problem in one row's wording is a separate root cause (P3-9). | Closed |
| P3-6 | A refused unary call or stream helper releases its internal record itself. The exception carries the refusal, `operation_id` and `request_id_consumed`. | L100 says nothing needs releasing before a retry. L55 extends the `False` rule to unary and stream calls. The round-1 stream retry loop no longer leaves records behind. | Closed |

## Changed-range analysis

`git -C gwz-py diff 0ca424f..0ffc4cb -- dev-docs/GwzPyConcurrentOperationsV2.md` has four hunks. The guide grew from 101 to 110 lines. No other lines changed. `README.md` and `src/README.md` have empty diffs. `--stat` shows the same commit also changes `dev-docs/GwzPyTransportDesign.md`; I did not read it.

Changes that follow a round-1 disposition:

| Location | Change | Disposition |
| --- | --- | --- |
| L38 | Two sentences added: a cancelled `result()`/`events()` wait stops only that wait; `cancel()` finishes its join under repeated cancellation, like `close()` | P3-4 |
| L53 | The reuse sentence now points to the table and drops "provided the Client and generation remain open". The field definition moves to L55. | P3-5 |
| L55 (new paragraph) | Field present on all three exception classes and on descriptors | P3-3 |
| L55 | Definition based on the ID's state; duplicate ID reports `True` | P3-2 |
| L55 | `False` also covers unary and stream calls | P3-6 |
| L55 | Class hierarchy stated | P3-1 |
| L57–65 (new table) | Seven refusal rows | P3-5 |
| L67–86 | Example rewritten as `fetch_with_one_retry(client, request_id, wait_for_cause_to_clear)` | P3-1, P3-5 |
| L88 | Completeness paragraph rewritten | P3-1, P3-4 |
| L100 | Sentence added on refused unary/stream records | P3-6 |
| L104 | Class hierarchy, `.request_id_consumed` attribute, "before admission" changed to "before acceptance", and the during-admission case | P3-1, P3-3 |

Changes outside the dispositions:
- **L3:** the status line now also calls the refusal table and the example draft additions. This is sound.
- **L53:** a sentence defining a Client's core generation, which answers a round-1 walkthrough guess. This is sound.
- **L62 and L100 (`TransportGenerationBusy`):** a new refusal, plus "Local commands keep working until you close it". This produces P2-1.
- **L61 (`IoError` capacity-wait timeout):** a new admission behavior that is described nowhere else in the guide: admission can wait for capacity and then time out. This is recorded as a residual.

## 0. Evidence base

**Tuple checks**
- Commands run at start and at end: `git rev-parse HEAD`, `git -C gwz-core rev-parse HEAD`, `git -C gwz-py rev-parse HEAD`, and `git -C gwz-py status --short -- dev-docs/GwzPyConcurrentOperationsV2.md`.
- The results are as recorded in the Baseline.

**Documents read**
- **Object:** all 110 lines at 0ffc4cb, numbered with `nl -ba`. Line map:
  - 1–36: status and first example
  - 38: handle contract
  - 40–49: factory table
  - 51–55: common cases, request IDs, the field
  - 57–65: refusal table
  - 67–86: retry example
  - 88: completeness paragraph
  - 90: generated ID
  - 92–98: settings table
  - 100: limits and refusals
  - 102: cancellation race
  - 104: `GwzOperationCancelled`
  - 106: streams
  - 108: close
  - 110: threads and loops
- **Diff:** the one specified above, plus `git -C gwz-py log --oneline 0ca424f..0ffc4cb`, which shows one commit, 0ffc4cb.
- **READMEs:** diffs are empty; files are 149 and 4 lines.
- **Round-1 report at root 1135835:** headings only.

**Example structure (L68–85)**
- An `awk` indentation map of L68–85 confirms the layout: an inner `try` / `except GwzBridgeError` / `else` sits inside an outer `try` / `except BaseException` / `finally`, all inside `for attempt in range(2)`.
- The retry wait (L85) sits after the outer `try` statement.

**Rendering checks**
- Code fences are at 9/36 and 67/86.
- Only the fence lines have odd backtick counts.
- Tables are consistent: 40–49 and 57–65 have 4 pipes per row; 92–98 have 5.
- Blocks are separated by blank lines.
- There is a single H1.

**Not read or run**
- Any code.
- Any design, including `GwzPyTransportDesign.md`; the v4 design link was not followed.
- Any plan, remediation plan, verdict or other-axis report.
- No builds, tests or Python execution.

**Process rules:** AgentProcessRules L1-18 to L1-22 and GwzProcessOptimization, as read in round 1.

## 1. Findings

### [P2-1] `TransportGenerationBusy` reads as a temporary condition, but it means the Client's transport has permanently failed

**Location:**
- L62 (table row): "`TransportGenerationBusy` | `False` | Never on this Client: its transport has failed. Close it and create a new Client."
- L100: "If the Client's transport has failed, every later network operation is refused with `TransportGenerationBusy`: close the Client and create a new one."

**Violated invariant (Surface):** a name that callers see must, read cold, describe the condition and must not suggest the opposite action.
- "Busy" commonly means temporary contention that should be retried, like the guide's own temporary codes `TransportSessionFull` and `TransportCapacityConflict`.
- The documented action here is "never retry on this Client".
- L53 now defines "a Client has one core generation for its whole lifetime". So "GenerationBusy" reads as "this Client's only generation is busy for now".

**Reproduction:**
1. The Client's transport fails.
2. Every fetch is refused with `TransportGenerationBusy` and `request_id_consumed=False`.
3. L55 says `False` means the ID "may reuse it once the refusal's cause clears".
4. The name and the field value both say "wait, then retry". Only the third column of L62 says the cause never clears on this Client.
5. A caller whose policy treats `*Busy` as temporary keeps retrying against a dead Client instead of recreating it. The same applies to a `wait_for_cause_to_clear` written from the code name.
6. The example limits the damage to one wasted wait and one retry. A caller's own unbounded loop does not.

**Impact:**
- Sync and push jobs get stuck on a failed Client.
- Logs and telemetry classify a permanent failure as contention.
- Callers match the code string (`exc.code == "..."`), so renaming it after release is a compatibility break. Under the Surface rule, that makes this P2.

**Required correction:**
- Before release, give this refusal a name that states the permanent condition, for example `TransportGenerationFailed`, and update L62 and L100.
- If the core code has to keep its name, expose a Python-level code, or a documented attribute, that marks the refusal as permanent for the Client, and show it in the table.

**Closure test:** a caller reading only the code name and `request_id_consumed` would not retry on the same Client, and L62 and L100 use the new name consistently.

### [P3-7] The example calls `cancel()` on refused handles, but the guide defines only `release()` for that state

**Location:**
- L80–82: `except BaseException: await handle.cancel()  # joins a still-running operation; returns at once if terminal`. L76 reaches this line on a refusal that consumed the ID and on a second refusal.
- Compare L38: "`release()` removes a terminal, refused or unstarted record … An unstarted handle can be cancelled or released without endpoint effects".
- Compare L100: a refused handle "can be released but cannot be admitted again".

**Violated invariant:** every call the lifecycle template makes must be defined by the reference text for the state the handle is in when the call is made.

**Reproduction:**
1. Attempt 0's `accepted()` is refused with `request_id_consumed=True`, or attempt 1 is refused again.
2. L76 raises, and L81 runs `await handle.cancel()` on a refused handle.
3. The guide never defines `cancel()` for a refused handle:
   - L38 treats refused records as separate from terminal and unstarted ones.
   - `cancel()` is defined only for running operations (join), for repeat calls, and for unstarted handles.
   - The comment's "returns at once if terminal" does not cover refused handles.
   - L100 offers only release for a refused handle.
4. If `cancel()` on a refused handle raises anything other than the same refusal, that exception replaces the refusal on both paths. The caller of `fetch_with_one_retry` then does not receive the refusal whose `request_id_consumed` tells it to choose a new ID; the refusal survives only as `__context__`.
5. L88's "and the exception propagates" is then false.

**Impact:** on two of the paths L88 lists, the exception the caller receives depends on an undefined call. The `finally` still releases the handle, so no record leaks.

**Required correction:** do one of the following.
- State that `cancel()` on an unstarted, refused or terminal handle returns at once without raising, and say what it returns.
- Or restructure the example so `cancel()` runs only after `accepted()` has succeeded.

**Closure test:**
- For each handle state (unstarted, refused, admitted and running, terminal), the reference text says what `cancel()` does.
- On the consumed-ID and second-refusal paths, the refusal itself propagates.

### [P3-8] The "released exactly once" claim relies on `release()` surviving task cancellation, which the guide does not state

**Location:**
- L84: `finally: await handle.release()  # every handle is released exactly once`
- L88: "Every handle the example creates is released exactly once … task cancellation at either `accepted()` or `result()` …"
- Compare L38: "Like `close()`, `cancel()` finishes its join even if the awaiting task is cancelled again, then lets the cancellation propagate".
- Compare L108: "Repeatedly cancelling a Python close waiter does not interrupt native shutdown".

**Violated invariant:** every guarantee the guide makes about its template must rest on a stated property of each call it depends on.

**Reproduction (two scenarios):**
1. **A single cancellation.** The fetch completes, so `return await handle.result()` has its value. A timeout or `TaskGroup` cancellation then fires while the task is suspended in the `finally`'s `await handle.release()`. `CancelledError` is thrown into `release()`, and the guide does not say whether the removal completed.
2. **Level-triggered cancellation.** Some cancellation mechanisms cancel again at every await inside a cancelled scope, for example anyio cancel scopes on asyncio. Cancellation hits `accepted()`. The handler's `cancel()` is cancelled again, finishes its join as L38 promises, and raises. The `finally`'s `release()` is then cancelled at once.

In both scenarios, if `release()` can be interrupted before it removes the record, the record stays until its 15-minute deadline. That contradicts L84 and L88, and repeated occurrences count toward the 64-record limit.

**Why this is now a finding:** round 1 recorded this only as a residual. The revision now claims exactly-once release on cancellation paths, and it specifies cancellation robustness for `cancel()` and `close()` but not for `release()`.

**Impact:** records leak for a bounded time under ordinary cancellation timing. The cleanup contract is asymmetric at exactly the call the template depends on.

**Required correction:** do one of the following.
- State that `release()` completes its removal even if the awaiting task is cancelled, as `cancel()` and `close()` do, and then lets the cancellation propagate.
- Or show the example protecting the release, for example `await asyncio.shield(handle.release())`, and qualify L84 and L88.

**Closure test:**
- L38 states how `release()` behaves under task cancellation.
- Walking both reproductions leaves no retained record.

### [P3-9] The `TransportSessionFull` retry condition tells callers to release records listed by `recent_operations()` without limiting that to their own finished records

**Location:**
- L60: "After you release records listed by `client.recent_operations()`, or after they expire". This repeats L100's "call `client.recent_operations()` and release completed records".
- The example hands this step to `wait_for_cause_to_clear` (L85).

**Violated invariant:** a retry procedure must not tell a caller to destroy state that other holders still need. The guide's premise is one Client shared by independent operations (L5), including across threads and event loops (L110).

**Reproduction:**
1. Component A's push completes. Its record is retained so A can later call `await handle.result()` and check `.effect`, as L102 asks for reconciling possible-effect pushes.
2. Component B's fetch is refused with `TransportSessionFull`.
3. B's `wait_for_cause_to_clear`, written from L60, releases every terminal record that `recent_operations()` lists, including A's.
4. A's record is gone: L38 says release removes a record, and a second release reports `OperationExpired`.
5. A can no longer read the outcome it needs to decide whether its push took effect.

The same wording existed at L100 in round 1. It is raised now because the revision makes it the retry procedure for this refusal.

**Impact:** another operation loses its retained outcome, including facts about a push that may have taken effect, which the guide tells callers to reconcile.

**Required correction:**
- Limit L60 and L100 to "records you own and no longer need".
- State that releasing a record removes it for every holder of its handle or ID.

**Closure test:** the table row and L100 restrict release to the caller's own finished records and warn that a release removes the record for other holders.

## 2. Invariant analysis

**Example lifecycle.** I traced every path the coordinator named. Attacks that failed:

Structure:
- `refusal = exc` survives Python's implicit deletion of `exc`.
- `refusal` is always bound when L85 runs. L85 is reachable only after an attempt-0 refusal whose ID is still reusable.
- The loop cannot fall through: attempt 1 always either returns or raises.
- An exception raised by `result()` in the `else` clause is not caught by L74.
- If `start_fetch` raises synchronously (malformed ID, 65th handle), it does so before the `try`. On attempt 1 this leaves the already-released attempt-0 handle untouched, so there is no double release.

Paths:
- **Admitted and completed:** the result is returned, then released. `cancel()` is not called.
- **Admitted and failed:** `GwzOperationError` is raised. `cancel()` on the terminal record returns at once; L38's repeat semantics and L102's "if success won" race support this. The error is re-raised and the handle released.
- **Refused once, ID not consumed:** the handle is released in `finally` before the wait. Attempt 1 then runs.
- **Refused twice, or refused with the ID consumed:** the refusal is raised, then `cancel()` (see P3-7), then release.
- **Task cancelled at `accepted()`** (either attempt, before or after admission): `cancel()` returns because the operation is already joined (L88, L104). The cancellation is re-raised, the handle released, and there is no retry.
- **Task cancelled at `result()`:** the wait stops and the operation keeps running (L38). `cancel()` joins it, surviving a repeated cancellation (L38), and the record is then released.
- **Task cancelled at the handler's `cancel()`:** this is documented (L38).
- **Task cancelled in the wait, or the wait raises:** no handle is held.

The one failing path is cancellation during `release()` (P3-8).

**Agreement with the rest of the guide:**
- The two rules L88 relies on match L38 and L104.
- L79's comment matches L38 and L102.
- L81 holds for terminal records; refused records are P3-7.
- The table's field values agree with L53 and L55: `False` before registration, `True` after registration or when the ID is in use.
- `IoError` appears with both field values, which is consistent with "Do not work this out from the error code".

**Names:**
- `request_id_consumed` now describes the ID's state and matches its name.
- The `GwzOperationCancelled` hierarchy is explicit.
- `fetch_with_one_retry` and `wait_for_cause_to_clear` read correctly cold.
- `TransportGenerationBusy` does not (P2-1).

**Defaults:** no new options. The capacity-wait timeout has no stated value (residual).

**Rendering:** holds.

**First-day walkthrough (guide only):**
1. **Imports:** `GwzBridgeError` and `GwzOperationCancelled` are used, but their import location is still unstated (unchanged since round 1).
2. **Creating the Client and starting a fetch with an explicit ID:** works.
3. **Meeting a refusal:**
   - The table names every refusal and its field value.
   - To trigger `TransportCapacityConflict` deliberately, you need the per-operation connection override. Its keyword is still unnamed (L95, L100) and has to be guessed.
4. **Deciding what to do:**
   - "Generation" is now defined.
   - I had to guess what capacity an `IoError` capacity-wait is waiting for, and how long `accepted()` may block.
   - I had to guess how to identify "the conflicting operation", because the refusal is not said to name it. The backoff fallback works.
   - `TransportGenerationBusy` misled me into planning a retry (P2-1).
5. **Retrying:** the contract of `wait_for_cause_to_clear` is only implied (return to retry, raise for "Never" rows). I had to guess it.
6. **Observing the result:** works.
7. **Releasing:** every path releases, with these guesses:
   - how `cancel()` behaves on a refused handle (P3-7);
   - how `release()` behaves under cancellation (P3-8);
   - `release()`'s return value, and whether its reports are raised or returned (unchanged since round 1).
8. **Closing:** works. I had to guess the refusal a closed Client gives, which has no table row.

## 3. Risks and next action

**Residual risks below the finding bar:**
- The exception class raised by a cancelled `result()`/`events()` wait is unstated (the unmet P3-4 clause).
- The `TransportSessionFull` row covers only releasable records. When all eight operation slots are live, nothing can be released, and the row should also say "or after a live operation finishes".
- The capacity-wait mechanism and its duration are unstated. `IoError` covers both a temporary cause (`False`) and a cause that consumes the ID (`True`), and only the field tells them apart.
- A `TransportCapacityConflict` refusal is not said to identify the conflicting operation.
- The table has no row for a refusal caused by a closed Client.
- The table header does not scope it to admission refusals. `TransportSessionFull` from an optional event reader (L38) arrives with the ID already registered, so the table's `False` value does not apply there.
- If `wait_for_cause_to_clear` raises, the refusal is not chained unless the wait chains it.
- L88 wording: "On success … the exception propagates"; and it says "two rules above", but the `accepted()` rule is below it, at L104.
- Carried over unchanged from round 1:
  - import location of the exception classes;
  - the unnamed per-operation override keyword;
  - `release()`'s return contract;
  - the status-line link, which leaves the gwz-py repository;
  - the README does not link to this guide;
  - the first example releases only on success.

**Round count:** this is remediation round 1 for V4. P2-1 is a new, non-architectural root cause (a naming problem), so another round is allowed under the two-round cap.

**Next action:** rename `TransportGenerationBusy` (or add a documented permanent-failure signal), updating L62 and L100 as specified. Optionally fold the P3-7 to P3-9 clarifications into the same edit. Then run a focused Surface check of L38, L55–65, L80–88 and L100 only.
