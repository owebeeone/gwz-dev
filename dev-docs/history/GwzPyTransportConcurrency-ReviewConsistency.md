# Python transport concurrency amendment — Consistency review

**Review object:** committed `dev-docs/GwzPyTransportConcurrencyNoGo.md`, `dev-docs/GwzPyTransportConcurrencyRemPlan.md` (controlling DRAFT), and the status annotation in `gwz-py/dev-docs/GwzPyTransportDesign.md`.  
**Baseline:** root `c1cb20c98c803500fe7c4ad4d0451e6c5905df28`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-core `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.  
**Date / axis / verdict:** 2026-09-23 · Consistency · **NO-GO** — five P2 findings and one P3 finding.

## 0. Evidence base

I read the committed review object, the accepted Python design, `GwzRemoteTransportPlan.md` Phase 2, `GwzRemoteTransportRetryPlan.md` §3(7), §6 and S1.4, and `GwzV110Plan.md` Phase 6. I used the committed core session/runtime, Python native session/bridge, pool and mux source only to check feasibility. I followed `AgentProcessRules.md` as amended by `GwzProcessOptimization.md` and the review-loop skill. This was a read-only document review; no tests were run. The exact four-repository tuple matched at the start and end.

## 1. Findings

### P2-1 — The amendment leaves an accepted blanket refusal in force

**Location:** `GwzPyTransportConcurrencyRemPlan.md:11,17,21,39`; `GwzRemoteTransportRetryPlan.md:336–339,415`.

**Invariant and counterexample:** The draft requires request B with identical physical capacity to be admitted while A holds a non-idle lease. The accepted retry plan’s §6 says **any** new operation under a non-idle lease is refused, and S1.4 repeats that implementation requirement. The draft names only retry §3(7) as preserved; it never supersedes §6 or S1.4. A conforming implementer cannot both admit and refuse B.

**Impact:** Reviewers could accept code that satisfies one controlling text while violating the other. **Remedy:** Explicitly amend the exact §6 and S1.4 sentences, limiting refusal to the defined incompatible-capacity case, and obtain the required amendment review. **Closure test:** A contract cross-check names every changed retry clause; a held-lease core test admits identical B and refuses differing C before either opens.

### P2-2 — The retained Phase 2 exit criterion requires the overlap this draft forbids

**Location:** `GwzPyTransportConcurrencyRemPlan.md:11,17–19`; `GwzRemoteTransportPlan.md:212–214`; `GwzRemoteTransportRetryPlan.md:127–133`.

**Invariant and counterexample:** Phase 2 still requires *overlapping operations with different per-host policy limits* and says lowering one operation’s fan-out does not evict the other’s connections. Under the accepted retry mapping, different per-host values produce different physical capacities. The draft therefore refuses the second operation instead of allowing that exit case. Retry §3(7) superseded selected Phase 2 bullets, but did not name this exit criterion; the new draft does not name it either.

**Impact:** The proposed closure cannot pass the retained exit criterion, and evidence for equal-capacity overlap cannot establish the different-limit claim. **Remedy:** Make an explicit decision about that exit criterion and amend its exact text if differing-limit overlap is no longer required. **Closure test:** Review the resulting Phase 2, retry and Python clauses together; run the precise differing-limit sequence required by the final contract.

### P2-3 — The proposed live-ID bound cannot work on the retained long-lived mux

**Location:** `GwzPyTransportConcurrencyRemPlan.md:7,15,21,27`; `gwz-core/src/transport_host/session.rs:38–42,501–517`; `gwz-transport/src/mux/mod.rs:45–53,242–268`.

**Invariant and counterexample:** The draft uses installed pool `max_requests` (normally 1024) as the bound on live top-level IDs while keeping one long-lived runtime. The core session and mux each retain request-ID tombstones and cap registrations at **256 for the lifetime of that session**, irrespective of how many IDs remain live. After 256 successful sequential operations on one client, the 257th fails registration even with no active work and spare pool capacity. Failed admissions can also consume tombstones.

**Impact:** A long-lived client permanently loses network service well before the stated live-ID limit. Merely removing a failed request’s active registration cannot safely erase the mux tombstone, which prevents a delayed message from claiming a reused ID. **Remedy:** Define a bounded session/mux generation or rollover rule, or explicitly amend the mux limit and its tombstone safety model; distinguish lifetime uniqueness from live admission count. **Closure test:** More than 256 sequential requests, failed admissions and delayed stale messages on the same client preserve service and cannot attach to a later request.

### P2-4 — Pool checkout capacity is used as an aggregate operation budget

**Location:** `GwzPyTransportConcurrencyRemPlan.md:15,17`; `GwzRemoteTransportRetryPlan.md:284–290,317–323`; `gwz-transport/src/pool/machine.rs:172–190`; `gwz-core/src/operation/par_map_per_host.rs:108–112,142–162`.

**Invariant and counterexample:** Accepted `max_requests` limits outstanding **pool checkouts**, while `jobs` limits worker threads **within one operation**. The draft assigns that pool value to the number of live top-level operations without defining a session-wide worker budget. For example, 16 admitted default-policy operations can each start 100 member workers—up to 1,600 workers—while their shared physical host remains capped at 32 connections. Increasing the mux’s 256 lifetime limit to honor the proposed 1024 live IDs would widen this further.

**Impact:** Physical connection caps do not bound accumulated workers, queues or memory; ordinary overlapping calls can fail from resource exhaustion despite satisfying the proposed ID rule. **Remedy:** Define a separate aggregate admission/worker budget and its refusal or queue behavior; retain pool `max_requests` for pool checkouts. **Closure test:** Concurrent multi-member operations assert aggregate workers and queued work stay within the declared session bound, including cancellation and close.

### P2-5 — CLI placement has no capacity-admission path matching the draft

**Location:** `GwzPyTransportConcurrencyRemPlan.md:17–19,34–35`; `gwz-core/src/transport_host/mod.rs:180–228,270–285`.

**Invariant and counterexample:** The draft applies compatible-capacity admission to each placement’s endpoint owner and includes installed in-process `cli` placement in its closure gate. Current `TransportRuntime::request` calls `install_capacity` only when it has a local endpoint; the `cli` branch has `local_endpoint=None`. The separate `CliEndpoint::register_request` receives only a request ID. Two explicit-`cli` requests with different policies can therefore pass this path without the draft’s capacity comparison or pre-handler refusal.

**Impact:** The proposed local guard change does not implement the contract for a placement the draft claims to cover. **Remedy:** Name the CLI endpoint’s policy/admission owner and the interface by which it receives the resolved capacity, within the accepted message boundary; include that change in implementation scope. **Closure test:** On an installed CLI endpoint, hold A’s lease, admit equal-capacity B and refuse incompatible C before handler, credential access or capacity mutation.

### P3-1 — The overlap gate misses the capacity-install transition

**Location:** `GwzPyTransportConcurrencyRemPlan.md:15,21,35–36`; `gwz-core/src/transport_host/session.rs:367–493`.

**Invariant and counterexample:** Later admissions must wait for the first policy installation, then compare against its completed capacity. The current core path temporarily sets `installed_capacity=None` between pool installation and authority installation. A test that starts B only after A holds a lease exercises the completed value and can pass while a cold simultaneous A/B start refuses B during that transition.

**Impact:** The stated closure tests would not prove the first-admission rule. **Remedy:** Specify an observable installation-in-progress state or equivalent serialization for that transition. **Closure test:** A deterministic barrier pauses A between pool and authority installation; equal-capacity B waits and is admitted after completion, while close and installation failure wake it to the specified result.

## 2. Invariant analysis

Equal-capacity overlap can avoid resizing once installation is complete: a comparison against the installed capacity can return without resetting either pool or its idle timer. The draft’s claim is therefore plausible for that state. It does not resolve the accepted retry plan’s contrary refusal, the retained different-limit Phase 2 exit row, or admission during an incomplete install.

The proposed request-scoped cancellation and close rules require distinct live IDs, but the existing mux also needs lifetime tombstones. Pool `max_requests`, mux registrations and aggregate operation workers are three different resource domains. Treating one limit as all three leaves both service lifetime and aggregate work unspecified. The current CLI placement follows a fourth admission path that the local capacity guard does not cover.

## 3. Risks and next action

Keep the amendment and Phase 6/7 gate **NO-GO**. Revise the draft and named controlling documents in one bounded correction: settle the contradictory retry and Phase 2 clauses, define ID lifetime and aggregate work limits, and specify CLI capacity ownership. Add the first-install race test to the closure gates. Re-review that exact corrected text before using it as implementation authority.
