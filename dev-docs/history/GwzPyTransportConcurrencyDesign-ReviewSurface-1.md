# Python concurrent operations caller-surface review

**Review object:** `dev-docs/GwzPyConcurrentOperations.md`  
**Baseline:** `gwz-py` HEAD `124e50030afb6f7c0e8edd90c37838b0136981c2`, verified at start and end  
**Date:** 2026-09-24  
**Axis:** Independent Python caller surface  
**Verdict:** **NO-GO** until the P2 cancellation and record-limit contracts are resolved.

## 0. Evidence base

I read the concurrency caller guide at HEAD and compared it with `README.md` and `src/README.md`. I did not open source code, runtime internals, design or plan documents, or other reviews. I ran no imports or tests. The guide explicitly says the proposed methods are not active in the current candidate, so these findings concern the proposed caller contract.

## 1. Findings

### S1 — P2: Cancellation has no defined result or effect contract

**Location:** `dev-docs/GwzPyConcurrentOperations.md:21-25,31,35`.

**Reproduction:** Start a push, retain its handle, call `await handle.cancel()`, then inspect `await handle.result()` before deciding whether to retry. The guide says `cancel()` returns `TransportCleanup`, `result()` raises on “failure,” and cancellation facts remain available, but it never defines the terminal result of cancellation, whether `cancel()` waits until that result is stable, or how an unknown remote effect is represented. The example releases the cancelled handle without inspecting its result.

**Impact:** A caller cannot determine from the documented contract whether a cancelled push may have taken effect or what evidence to check before retrying.

**Remedy:** Define cancellation’s terminal states, the completion guarantee of `cancel()`, and the exact `result()` and event behavior after cancellation. Represent an uncertain Git effect explicitly and give push reconciliation guidance.

**Closure test:** A docs-only walkthrough cancels an admitted push, inspects its terminal outcome, and determines whether replay is safe without guessing.

### S2 — P2: `TransportRecordLimit` has no actionable recovery path

**Location:** `dev-docs/GwzPyConcurrentOperations.md:33,35`.

**Reproduction:** A unary `await client.push(...)` raises `TransportRecordLimit`. The guide says the Git effect may already have occurred and warns against automatic replay, but it specifies neither when this error can occur nor whether the exception carries an operation ID, result, or other evidence. A unary caller has no handle to query.

**Impact:** The caller has no documented way to resolve an effect that may have occurred. The warning prevents an unsafe automatic retry but leaves the operation’s outcome indeterminate.

**Remedy:** State the failure phase and the error payload, then document a concrete way to recover the result or reconcile the remote state when no record exists.

**Closure test:** A docs-only push walkthrough handles `TransportRecordLimit` through a specified investigation and retry decision.

### S3 — P3: Cross-thread use lacks a usable asyncio boundary

**Location:** `dev-docs/GwzPyConcurrentOperations.md:5,31,35,37`.

**Reproduction:** Create a Client on one event loop, start an operation from a task on another thread’s loop, cancel via the first thread, and close the Client. The guide promises the same concurrency rules across Python threads but does not say whether direct cross-loop calls are supported or whether calls must be scheduled onto an owning loop. It also does not state the rule for handle methods.

**Impact:** A reader cannot choose a supported threading pattern from the guide alone.

**Remedy:** State loop ownership and thread-safety rules for Client and handle methods, with one two-thread example covering start, cancel, result, and close.

**Closure test:** The documented example gives an unambiguous supported call path for every lifecycle step.

### S4 — P3: Capacity and retry defaults are incomplete at the Python call site

**Location:** `dev-docs/GwzPyConcurrentOperations.md:33`.

**Reproduction:** Configure two starts to use equal physical capacities and predict whether they overlap. The guide names defaults for `concurrency`, `max_connections_per_host`, and `max_retries`, but does not say where the latter two are set, what the total pool limit or default is, or whether `max_retries=3` means three attempts or three retries.

**Impact:** A reader cannot configure or predict the documented `TransportCapacityConflict` rule reliably.

**Remedy:** Add a compact option table with Python argument location, scope, default, accepted values, total capacity, and retry meaning.

**Closure test:** A reader can determine from the guide alone whether two example starts are admitted together.

### S5 — P3: Event access and result access are not sequenced

**Location:** `dev-docs/GwzPyConcurrentOperations.md:31,35`.

**Reproduction:** Start an operation, await `result()`, then call `events()` to inspect retained progress; alternatively, read some events and then await `result()`. The guide promises ordered events and 15-minute retention but does not say whether `events()` replays from the beginning, permits multiple iterators, or must be drained for `result()` to complete.

**Impact:** A caller may miss progress or leave an iterator waiting through an undocumented access pattern.

**Remedy:** Define replay, iterator, and result independence rules, then show an example that uses both events and result.

## 2. Invariant analysis

The guide clearly separates operation IDs from request IDs, states that one cancellation does not cancel a peer, places `TransportCapacityConflict` and `TransportSessionFull` before admission and Git effects, and describes shared Client shutdown. The example demonstrates two admitted operations, identification, cancellation of one, retrieval of the other result, release, and context-managed close.

The unresolved invariants are the observable terminal state after cancellation and the evidence available after a possible-effect `TransportRecordLimit`. These determine whether a caller can safely retry a push. Cross-thread ownership, option placement, and event access need bounded documentation before the guide can serve as a first-day reference.

## 3. Risks and next action

Resolve S1 and S2 in the proposed API contract and update the walkthrough to inspect the cancelled operation before release. Address S3–S5 in the caller guide. When the methods become active, link the guide from the Python API section of `README.md`; that section currently mentions streaming forms but gives no route to `start_*`.
