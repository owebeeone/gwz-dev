# Shared SSH Endpoint Worker and Destination Routing — State-Axis Review

**Review object:** Shared-worker checkpoint at root `dc6480116755f95a31ffae0b3dada1ce3d26c2a9`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshWorker.md` at `59ac059c8bfe4052d8a5a16d821f314a3985bcc8`, status “implemented; aggregate review pending; production activation pending,” dated 2026-09-21.  
**Baseline:** root `dc6480116755f95a31ffae0b3dada1ce3d26c2a9`; gwz-core `59ac059c8bfe4052d8a5a16d821f314a3985bcc8`; gwz-core-evidence `53ab4831a04fc7ab67975456737ae5ed20dc8174`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show <SHA>:<path>`.  
**Date:** 2026-09-21  
**Axis:** Worker state transitions, bounded admission, cancellation, deadlines, physical ownership, routing admission, and fail-closed recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks acceptance. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete review prompt; `GwzRemoteTransportSshWorker.md` lines 1–131; its cited accepted stream, pool, channel, adapter, and per-remote contracts; `EVIDENCE.md`; the current checkpoint; and the evidence archive’s rules.

Implementation inspection covered:

- `ssh_worker.rs` lines 28–462, including admission permits, request/reply lifetime, worker wakeups, queue draining, pending checkout, active transfer, shutdown, attachment, release, and timer calculation.
- `ssh_destination.rs` lines 1–162 and `ssh_endpoint.rs` lines 1–40.
- The five-line pump clock-boundary change and its exact-expiry regression.
- All new destination, route, worker, agent-wait, and pump tests, plus the unchanged pool/channel/bridge tests.
- Private worker-a inputs, exact source hashes, development failure logs, README, and final raw output at evidence commit `53ab4831a04fc7ab67975456737ae5ed20dc8174`.

I reran the permitted Rust 1.95 offline locked gate with the external target directory. It passed 35 tests, with the one deliberately ignored agent child invoked by its passing parent. Native push, clone, second push, and fetch reused one injected authenticated connection; concurrent service failure left another active stream readable.

The exact tuple matched at both review boundaries. Final status contained only the two explicitly excluded owner-generated prompt files.

## 1. Findings

### [P2-1] A queued deadline is falsely reported as endpoint shutdown

**Location:** `gwz-core/src/git/endpoint/ssh_worker.rs` lines 90–92, 194–220, and 288–315.

`OpenRequest::expired` combines caller cancellation and absolute deadline expiry. The receive-queue path then combines `stopping_at.is_some()` with `request.expired(now)` and completes both cases using `stopped()`, which is `BrokenPipe: SSH endpoint stopped`. By contrast, the pending-checkout path correctly maps expiration to `TimedOut`.

A concrete sequence is:

1. Construct an endpoint with a positive connect timeout, so `open` records an absolute request deadline and waits with `recv_timeout`.
2. Queue a valid request while the worker is processing its bounded request or active-exchange turn, or while the worker thread is descheduled.
3. The worker dequeues that request at or after its deadline while `stop == false` and `stopping_at == None`.
4. Lines 292–294 classify the request as `BrokenPipe` and claim the shared endpoint stopped.
5. Because the absolute deadline is calculated before `recv_timeout` begins, there is a real interval in which the worker can publish this result while the caller is still waiting. A later request can succeed on the same endpoint.

This violates exact timeout classification and invents a terminal endpoint state. A caller may discard a healthy shared endpoint and its reusable pool, suppress a timeout-specific retry, or misattribute scheduler/admission delay as worker failure.

**Required correction:** separate shutdown from request expiry. Only `stopping_at` or actual channel/worker loss may return `stopped()`. A queued request whose deadline has elapsed must return `TimedOut`; cancellation should retain its own explicit reason or, for the current caller-timeout mechanism, clean up without manufacturing shutdown.

**Closure test:** deterministically hold a request in the receiver queue past its absolute deadline while the worker remains live. Assert that it returns `TimedOut`, its permit is released, the endpoint stop flag remains false, and a subsequent request is serviced. Cover the exact-deadline boundary (`now == deadline`).

## 2. Invariant analysis

**Admission and lifetime otherwise held.** The atomic permit spans queued and pending requests and is released before reply publication. Send failure, receiver loss, timeout cleanup, and queue destruction each drop exactly one permit. Active streams leave that admission ledger only after the pool owns their physical capacity. Endpoint clones share the worker, and final shared-state Drop signals, unparks, and joins it.

**Shutdown and physical ownership held.** Shutdown first rejects admission, completes pending requests, disconnects active callers, and shuts down the pool. Leases then drive discard and retain capacity until physical disposal. The cleanup deadline gets a final host step at exact expiry; `PoolHost` Drop supplies bounded forced disposal if graceful shutdown is incomplete. A worker error drops pending/queued requests, disconnects active streams, marks the endpoint stopped, and disposes all physical owners.

**Cross-stream isolation held.** Each transfer error cancels only its pump and peer, then discards that lease. Other active entries continue through the worker. Successful reuse requires terminal stream completion plus `ChannelResource::reclaim`; failure to recover the entire connection forces discard. The native concurrent test reproduced this isolation.

**Timer ordering held after the pump correction.** Worker and pool use one monotonic origin. Active and pending work force millisecond polling; idle waits are capped at one second and respect the next pool deadline. The pump advances time before ingesting messages, so a Close arriving at an already-expired network deadline cannot switch to the cleanup clock and conceal the timeout. A zero connect timeout intentionally leaves setup pending until explicit shutdown, which wakes the blocked caller.

**Destination and identity admission held.** Recognized SSH spellings fail closed. Raw embedded credentials, malformed authorities, invalid ports, query/fragment forms, controls, invalid UTF-8 escapes, and ambiguous IPv6 are rejected without echoing input. URL escapes are decoded once; SCP operands remain literal; repository paths do not alter pool identity; host aliases and effective ports are preserved. Route parsing precedes identity resolution, and authority is resolved on every open before checkout.

**Claim boundaries held.** The agent test accurately demonstrates that native agent reply waiting exceeds the session timeout despite nonblocking mode. It uses an isolated socket and reaped child and makes no bounded-authentication claim. Production credential setup, callback activation, platform qualification, and selected-source reconstruction remain explicitly pending.

## 3. Risks and next action

The worker still uses polling because no socket-readiness integration exists, and credential setup remains an injected seam. Those limits are stated accurately and belong to the next bounded package.

Correct P2-1 and add the focused queued-expiry regression. The re-verdict need only inspect that classification split and its interactions with permit release, shutdown, and subsequent endpoint use.
