# SSH N2b selected admission/pool-worker integration — State-axis review

**Review object:** N2b implementation checkpoint at `gwz-core` `0f476cb9c67bce00e24afbdef6caee57fb6cd3dd`, controlled by `dev-docs/GwzRemoteTransportSshN2b.md`; transport deadline extension at `gwz-transport` `50bb6b1275b26f7c2a38493f06a1638d220a6c7a`; evidence archive `7cea42eb004ab439519dd86293852adf74e050cd`; review pending as of 2026-09-21.  
**Baseline:** root `3d411d8c84a93839cd04ddc4262e37c31c27e53b`; core `0f476cb9c67bce00e24afbdef6caee57fb6cd3dd`; evidence `7cea42eb004ab439519dd86293852adf74e050cd`; transport `50bb6b1275b26f7c2a38493f06a1638d220a6c7a`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show`/`git diff`; start and end tuple checks matched exactly. Only the two permitted generated root prompts were untracked.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics, lifecycle ownership, races, cancellation, deadlines, and fail-closed recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I inspected the N2b control document and the accepted selected-identity ownership contract, then reviewed the complete changed core paths: `ssh_admission.rs`, `ssh_worker.rs`, `ssh_setup.rs`, `ssh_shutdown.rs`, `ssh_key_snapshot.rs`, `ssh_key_auth.rs`, `ssh_pool.rs`, `worker_queue.rs`, and `selected_pool.rs`. I reviewed the transport changes in `pool/{allocation,asynchronous,clock,lifecycle,machine}.rs` and `tests/network_timeouts.rs`, plus the committed evidence README, source manifest, and raw logs.

The core N2b delta contains 351 added production lines and 117 added test lines, within its stated bounds. Evidence source hashes identify the reviewed core and transport revisions.

Both permitted focused commands passed:

- `selected_pool`: 3 passed.
- `network_timeouts`: 9 passed.

The final tuple and cleanliness check remained unchanged.

## 1. Findings

### [P2-1] Helper interaction replaces the absolute request deadline with a later interaction deadline

**Location:** `gwz-transport/src/pool/clock.rs:13-40`, `78-112`, and `116-139`; the incomplete regression is `tests/network_timeouts.rs:48-66`.

**Violated invariant:** N2b requires one unchanged absolute caller deadline through allocation, connection, helper interaction, and handoff. No sub-clock may extend that deadline.

**Counterexample:** Create a pool with a 100 ms connect timeout and the default 120,000 ms interaction allowance. Submit `request_until(request, Some(25))`, consume `Action::Connect`, then call `begin_interaction(connection)` at time zero. The method replaces `Network(Some(25))` with `Interaction { until: 120000, remaining: Some(25) }`. At `advance(25)`, expiry checks only `clock.deadline()`, which is now 120,000. `next_deadline()` likewise reports 120,000, and `take(request)` remains `WouldBlock`. The absolute deadline is restored only if `end_interaction` runs.

The existing test ends interaction at time 5 and therefore proves resume behavior, but never advances across the absolute deadline while interaction remains pending. The endpoint caller’s separate receive timeout mitigates its current use, but it does not repair the changed pool contract or independently bound a host that relies on `checkout_until`.

**Impact:** A helper interaction can retain a request and physical capacity beyond its absolute budget, delaying cancellation and cleanup. This directly contradicts `GwzRemoteTransportSshN2b.md:25-27` and the selected-identity contract’s unchanged-deadline requirement.

**Required correction:** Preserve the absolute deadline as an independent ceiling while `ConnectClock::Interaction` is active. `advance` and `next_deadline` must use the earlier of the interaction deadline and request absolute deadline, and expiry must start the normal bounded cancellation/disposal path with the defined interaction-stage error.

**Closure test:** Start interaction under `request_until(..., Some(25))`, do not call `end_interaction`, assert `next_deadline() == Some(25)`, assert the request remains pending at 24, then advance to 25 and require `InteractionTimeout` plus the expected bounded `CancelConnect`/cleanup sequence. Cover the same sequence when the ordinary network timeout is disabled.

## 2. Invariant analysis

The admission boundary otherwise held. Selected paths are owned, read through supervised Jobs before checkout, and never reopened by the connector. Admission completion rechecks live cancellation/deadline state before interning. Timed-out admission keeps both its request permit and snapshot reservation until the Job is joined and its result disposed.

Entry ownership also held. The admitted request pins its entry through checkout; setup obtains a second pin by opaque identity lookup; successful native setup transfers the pin into `NativeResource`; the resource-level field survives idle, active, reclaim, and disposal. Connection ownership precedes the pin in destruction. Exact-byte concurrent admissions converge on one entry, while changed or invalid bytes cannot reach an old idle lease. The capacity-one selected-pool test causally demonstrates one physical authentication for six concurrent opens and a single credential offer.

Worker and shutdown attacks found no additional blocker. Admission polling and incoming work are bounded per turn; permits release on every queue, admission, checkout, reply, panic, and receiver-drop path examined. Panic unwinds queued/pending/active request owners while `PoolHost` and `Admissions` remain outside the catch boundary. Retained cleanup advances both domains and sets `cleanup_complete` only after physical pool disposal and zero pending admissions.

Old `Pool::checkout` behavior remains routed through `checkout_until(..., None)`. Allocation and ordinary connection deadlines are correctly capped, and completion after an absolute deadline is rejected. The interaction state is the sole observed hole.

## 3. Risks and next action

Production routing, N3 backend attachment, platform/source qualification, HTTPS, and activation remain deferred and were not evaluated.

Correct P2-1 and add the causal interaction-without-resume regression. A focused State re-verdict can then confirm the transport deadline closure without reopening the accepted admission, pin, or shutdown analysis.
