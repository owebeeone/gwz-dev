# Python transport concurrency amendment — independent Safety review

**Review object:** Committed `dev-docs/GwzPyTransportConcurrencyNoGo.md`, controlling DRAFT `dev-docs/GwzPyTransportConcurrencyRemPlan.md`, and the status annotation in `gwz-py/dev-docs/GwzPyTransportDesign.md`.  
**Baseline:** root `c1cb20c98c803500fe7c4ad4d0451e6c5905df28`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-core `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. Verified at the start and end of review.  
**Date / axis:** 2026-09-23 / Safety.  
**Verdict:** **NO-GO** — 1 P0, 0 P1, 5 P2, 0 P3.

## 0. Evidence base

I read the committed review object and controlling authorities with `git show`, including the accepted Python design, core transport Phase 2 and retry plans, v1.1.0 Phase 6, and the committed core host and Python native/bridge sources as feasibility checks. The process basis was `dev-docs/AgentProcessRules.md`, its `GwzProcessOptimization.md` amendment, and the review-loop skill. This was a read-only design review; no build or test was run. Unrelated working-tree material was excluded.

## 1. Findings

### [P0-1] Process-global results can be attributed to another client

**Location:** Remediation plan §§1 and 3, which retain the existing operation IDs and event/result APIs while promising independent clients and results. In committed source, `Client.meta(request_id=...)` accepts a caller-supplied ID; `native/src/shims.rs::operation_id` maps it to `op_{request_id}`; `native/src/operations.rs::OperationStore` is process-global and `begin` reuses an existing record for that string. `NativeCoreBridge.operation_result` and event reads use the module-global store.

**Invariant and counterexample:** A result and its events must belong to the submitting client’s operation. Two clients can submit with the same explicit `request_id`. Their distinct native sessions admit work against distinct hosts, but both workers attach to one global `OperationRecord`. If A completes first, B can read A’s result; B’s later completion is rejected as a duplicate. A client can also query a known foreign operation ID without a session ownership check.

**Impact:** False composition of results, with possible disclosure of another client’s repository and member details. Session-scoped cancellation alone does not contain the event/result path.

**Remedy and closure test:** Give records an unforgeable session owner, make all reads enforce it, and reject any colliding public ID before publishing `Accepted`; preserve the public operation ID shape if required. Submit the same explicit request ID on two clients, force opposite completion orders, and prove independent results/events and refusal of foreign lookups.

### [P2-1] Capacity transition ownership covers only the initial installation

**Location:** Remediation plan §2, especially its “before the first capacity is installed” leader rule and instruction to change the compatible-capacity guard. In committed `gwz-core/src/transport_host/session.rs::install_capacity`, registration precedes installation; `installed_capacity` holds the old value until a resize begins, then becomes `None` while retirement completes.

**Invariant and counterexample:** Two compatible requests arriving together after an idle capacity change should not both fail because they saw each other’s registration. After prior work at capacity C0 retires, A and B both request C1. The draft specifies a leader only for the *first* installation. With the prescribed guard change alone, each can observe a live peer while `installed_capacity` is C0 or `None` and refuse, although C1 could have been installed once for both. Close or cancellation during that transition also lacks an identified owner responsible for waking the waiter and retiring the reservation.

**Impact:** Timing-dependent `Capacity` refusals and stranded admission state on a long-lived client.

**Remedy and closure test:** Define a per-placement capacity transition with a leader, target value, waiting peers, cancellation and close behavior for **every** installation or change. Atomically publish the installed value before admitting compatible peers. Use barriers around a later C0→C1 transition: two C1 requests must proceed, a C2 request must refuse without mutation, and close must drain every waiter.

### [P2-2] Explicit `cli` placement has no defined pre-Open capacity admission path

**Location:** Remediation plan §2 says different placements use their respective capacity owner and that incompatible capacity refuses before remote Open or helper access. Its §4 fixture makes installed in-process `cli` placement conditional. In committed `gwz-core/src/transport_host/mod.rs::TransportRuntime::request`, the `cli` branch has no `local_endpoint`; `install_capacity` runs only when `local_endpoint` exists. `CliEndpoint::register_request` registers an ID but does not install policy capacity.

**Invariant and counterexample:** A caller’s resolved physical capacity must be checked by the physical endpoint before it handles the operation. With an installed `cli` endpoint at its construction capacity, an explicit `cli` request asking for a different per-host limit skips the only shown capacity installation path and can proceed to Open under stale caps. A second incompatible request likewise has no specified pre-Open refusal point.

**Impact:** Explicit policy can be silently ignored, and a request intended to be refused can reach endpoint credential or helper work.

**Remedy and closure test:** Freeze how the bound `cli` endpoint receives and atomically admits resolved capacity before handler dispatch; if that cannot be guaranteed for a placement, refuse it before Open. Test two compatible `cli` requests with a held lease, then a differing-capacity request, asserting zero Opens, helpers and pool mutations for the refusal.

### [P2-3] The proposed live-ID bound conflicts with the host’s lifetime registration limit

**Location:** Remediation plan §2 makes installed `max_requests` the bound for reserved and admitted **live** IDs and promises a long-lived host. Committed `gwz-core/src/transport_host/session.rs::register` rejects when `used.len() >= 256`; neither `used` nor `registrations` is removed at request finish. The accepted retry policy sets default `max_requests` to 1024.

**Invariant and counterexample:** A completed operation must release admission capacity without allowing stale IDs to attach to later work. A client that performs 256 sequential successful network operations is refused on its 257th, despite zero live work and a default advertised live bound of 1024. The same hidden 256 ceiling prevents the stated default live bound from being reached.

**Impact:** A healthy long-lived client becomes permanently unable to perform network work.

**Remedy and closure test:** Reconcile the host’s actual concurrent cap with `max_requests`, retire completed registrations after physical cleanup, and retain a bounded generation or equivalent stale-ID defense. Prove more than 256 sequential operations on one host, the declared concurrent boundary, and refusal of a stale ID after retirement.

### [P2-4] Completed event and result retention is unbounded

**Location:** Remediation plan §3 limits only the completed-*cancellation* record and explicitly allows a submitted worker to continue after its event consumer stops. Committed `gwz-py/native/src/operations.rs` keeps a process-global `HashMap` of operation records with no eviction; each record retains an event `Vec`, result and optional merge response.

**Invariant and counterexample:** Long-lived clients need bounded retained completion state, including when consumers disappear. Repeated `submit` calls with unique IDs, followed by dropped stream generators or completed reads, leave every record and every event in the global store for the process lifetime. The draft’s live-ID limit does not bound completed records.

**Impact:** Memory grows with operation count and event volume even after all transport work finishes.

**Remedy and closure test:** Define bounded result/event retention and an explicit expired-ID response while preserving active readers and the worker’s ownership. Run many completed submissions with abandoned and consumed event streams; verify a fixed retention bound and correct results for still-live readers.

### [P2-5] Submitted workers lack an independent finite admission and failure path

**Location:** Remediation plan §§2–3 use `max_requests` to bound live IDs and require each submitted worker to retain its scope through finish and result recording. The accepted retry policy derives `max_requests = max(1024, jobs)` and removes its upper validation bound. Committed `gwz-py/native/src/dispatch/mod.rs::spawn_call` creates one OS thread per submit with `thread::spawn`, and `TransportSession::close_inner` waits until each admitting or active entry signals completion.

**Invariant and counterexample:** Accepted work and close must remain finite under resource pressure, including worker-start failure. A caller can request large `jobs`, raising the proposed live-ID bound, then submit many operations. Per-operation `jobs` bounds each operation’s workers but not their aggregate. If OS thread creation fails after an ID and result record are reserved, the current spawn path panics rather than terminalizing that reservation; close can then wait indefinitely for a worker that never started. A worker panic before its completion signal has the same consequence.

**Impact:** Process resource exhaustion and an unrecoverable close wait.

**Remedy and closure test:** Add a host-level finite worker/queue bound independent of caller-controlled `jobs`; reserve it before publishing `Accepted`. Make worker start fallible and guarantee one terminal result, request cleanup and waiter notification on start failure or unwind. With blocked workers and a large requested `jobs`, assert bounded thread count, typed excess-work refusal, injected spawn-failure cleanup, and a completing close.

## 2. Invariant analysis

The draft’s per-request cancellation rule is appropriately narrow, but operation ownership must extend through event and result lookup; the current global recorder breaks that boundary. Capacity has three distinct states—installed, changing and unavailable—and the text assigns a leader only to initial installation. The physical owner for explicit `cli` capacity is also unidentified at the point where refusal must occur. Finally, a bound derived from caller-controlled `jobs` is not a finite host resource budget, and the current lifetime ID and result registries do not retire with physical cleanup.

## 3. Risks and next action

The amendment should remain **NO-GO**. Revise it to freeze session-owned results, all capacity transitions and placement admission, lifetime ID retirement, finite completion retention, and bounded/fallible submitted-worker ownership. Add the closure tests above to the design gate, then obtain a fresh independent verdict on the revised committed tuple. This verdict does not assess implementation acceptance or release readiness.
