# SSH N2b selected admission/pool-worker integration remediation 1 — State-axis review

**Review object:** N2b remediation 1 at `gwz-core` `6616a2cd66d64f04f9eb3c370e8a3a677fba1c36`, transport `16a383e7d1c0e7e3234006688986afc2c6e54ca5`, and evidence `593d2c36780d6278eee21e67dc0cc102673ed887`.  
**Baseline:** root `9459ae4c2f5ad5061a2eaba92785a1f87bece938`; core `6616a2cd66d64f04f9eb3c370e8a3a677fba1c36`; evidence `593d2c36780d6278eee21e67dc0cc102673ed887`; transport `16a383e7d1c0e7e3234006688986afc2c6e54ca5`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show` and `git diff`. Start and end tuple checks matched exactly. Only the four generated N2b review prompts were untracked at root; all reviewed members were clean.  
**Date:** 2026-09-22  
**Axis:** Focused durable-state, deadline, cancellation, and retained-ownership re-verdict. Independent, adversarial, read-only. Nothing here relies on the current peer review.

## Prior-finding closure

| Finding | Status | Closure evidence |
| --- | --- | --- |
| P2-1 — helper interaction replaced the absolute request deadline with a later interaction deadline | **Closed** | `pool/clock.rs:123-155` now caps the active interaction clock at the request’s absolute deadline. `clock.rs:38-41,82-119` also expires and schedules unclaimed ready leases at that deadline. `network_timeouts.rs:69-101` causally covers an interaction left open across the boundary and a ready lease not claimed before it. |

**Verdict: GO** — the prior P2-1 is closed and the focused changed-range review found no new P0, P1, P2, or P3 finding.

---

## 0. Evidence base

I read the committed prior State report, `GwzRemoteTransportSshN2b-RemPlan-1.md`, the revised N2b control document, and the exact diffs from core `0f476cb9` to `6616a2cd` and transport `50bb6b12` to `16a383e7`.

The transport change is confined to `src/pool/clock.rs` and `tests/network_timeouts.rs`. The core change adds deterministic admission lifecycle coverage in `tests/transport_ssh/tests/selected_pool.rs`; production core code is unchanged.

I inspected the committed remediation evidence README, source manifest, and raw logs under `campaigns/ssh-integration/runs/2026-09-22-selected-key-n2b-rem1`. The archive identifies the revised core and transport commits and records:

- Five selected-pool tests passing, including both new stalled-admission tests.
- Eleven transport timeout tests passing, including interaction-boundary and ready-lease regressions.
- The complete isolated SSH and transport suites passing under Rust 1.95 in locked, offline mode.

No builds were rerun during this read-only re-review.

## 1. Findings

None.

## 2. Invariant analysis

The original counterexample no longer reproduces. For `request_until(..., Some(25))`, entering interaction computes `until = min(now + interaction_budget, 25)`. At time 24 the request remains pending and `next_deadline()` reports 25. Advancing to 25 classifies the active interaction as timed out, fails the request, and exposes the normal bounded `CancelConnect` action. The helper interaction therefore cannot extend the absolute request budget.

The added ready-state rule closes the adjacent completion race. If connection setup finishes before 25 but the checkout has not been claimed, advancing to 25 converts `RequestState::Ready` to `AllocationTimeout` and moves the physical connection to closing. `take` cannot return a lease after the absolute deadline. Existing checkout behavior without an absolute deadline remains unchanged.

The deterministic admission tests substantiate the previously inspected ownership path. A blocked reader reserves snapshot capacity before entering its helper. Caller expiry returns `TimedOut` without invoking native setup, while registry usage remains charged. Endpoint shutdown reports one pending admission and keeps `cleanup_complete == false`; only releasing and joining the helper removes the admission, releases its reservation, and permits cleanup completion. The companion test proves an already active stream continues making progress while a later admission is stalled. Shutdown rejects that admission, and no late native setup or stream publication occurs.

These tests exercise the actual injected reader, `Admissions`, endpoint worker, shared shutdown owner, Job supervisor, and registry ledger. Their barriers make the expiry and join ordering causal rather than timing inferred.

## 3. Risks and next action

The accepted deferrals remain unchanged: N3 production routing/backend attachment, HTTPS, platform and selected-source qualification, and production activation are outside this checkpoint.

No State-axis remediation remains for N2b. The revised tuple may proceed through the review gate without reopening accepted N2a behavior or deferred activation work.
