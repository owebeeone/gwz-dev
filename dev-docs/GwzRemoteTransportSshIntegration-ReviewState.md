# SSH Local Integration Checkpoint — State-Axis Review

**Review object:** SSH pump, physical pool host, and per-remote local composition checkpoint at root `d1273951ec5b2746f9215206440e5ffb56232293`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshIntegration.md` at `f39a6ed260332534aee8b0cf73955803b6a5bf81`, status “implementation authorized; acceptance pending,” dated 2026-09-21.  
**Baseline:** root `d1273951ec5b2746f9215206440e5ffb56232293`; gwz-core `f39a6ed260332534aee8b0cf73955803b6a5bf81`; gwz-core-evidence `359d4fbf236182192f035ca8e42e3eb756c1a4ad`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from committed objects with `git show <SHA>:<path>`.  
**Date:** 2026-09-21  
**Axis:** Durable-state transitions, ownership, teardown, timeout ordering, and fail-closed recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

I read the complete review prompt; `GwzRemoteTransportSshIntegration.md` lines 1–99; its cited stream, pool, SSH-channel, blocking-adapter, and per-remote contracts; `EVIDENCE.md`; the evidence-member instructions; and the current checkpoint.

Implementation inspection covered:

- `ssh_pump.rs` lines 13–425, including ingress admission, forward consumption, reverse/stderr work, close completion, invalidation, disposal, and ownership return.
- `ssh_pool.rs` lines 15–225 and the pinned gwz-transport pool machine/async lifecycle, including pool-qualified connection IDs, lease generations, connect/cancel races, cleanup deadlines, and physical-close acknowledgements.
- `ssh_remote.rs` lines 13–100 and unchanged `stream_io.rs`, `ssh_channel.rs`, and `ssh_connection.rs`.
- All new focused tests: pump, pool host, remote bridge, native pooled remote, and the 313-line controlled worker.
- Private evidence inputs, hashes, README, development failures, and final raw log at evidence commit `359d4fbf236182192f035ca8e42e3eb756c1a4ad`.

I reran the sole permitted focused command with the external target directory. All 21 tests passed: 2 channel, 5 pool-host, 1 native composition, 8 pump, 3 regression, and 2 remote-bridge tests. The native test again reported one physical open, five Git service channels, one authenticated session, and verified clone/push/fetch object IDs across two repositories.

The exact tuple matched at both review boundaries. Final status contained only the two explicitly excluded owner-generated prompt files.

## 2. Invariant analysis

**Ingress and flow control held.** Data is size-bounded before copying and is delivered to the stream state machine before any SSH write. Invalid identity, ordering, shape, or limits therefore fail and dispose the channel without native forwarding. Accepted request bytes remain in the bounded mirror until the native sink reports an exact accepted prefix; only then does `Stream::read` consume the same count. Consequently Window and Flushed eligibility cannot advance ahead of the SSH sink. Zero writes, mismatched reads, and post-side-effect stream failures invalidate the pump rather than replaying uncertain bytes.

Reverse data is retained within the configured mirror, while stderr retention and per-turn drain work are independently bounded. Reverse backpressure does not stop stderr work. EOF is distinct from WouldBlock, outgoing EOF waits for all accepted request bytes, response EndWrite is transferred before service finish, and reusable completion requires successful zero exit, endpoint close completion, Closed-message ownership transfer, and recoverable native connection ownership. Cancellation, transport failure, timeout, and nonzero service status clear mirrors and prevent reuse.

**Clock ordering held.** Pump time advances before backend work, so an exact expiry prevents a new native side effect. Active I/O reporting stops when Close starts and the close-cleanup deadline becomes authoritative. Pool time likewise advances before connection completions; exact connect expiry wins over simultaneous readiness. Backpressure pauses the appropriate network clock without being misreported as peer progress.

**Physical ownership held.** A `PoolHost` exclusively owns each connecting, ready, or disposing resource. Connection IDs include a process-local pool identity, and leases add a generation, so foreign or stale leases cannot reach an entry. Cancellation and late completion move the physical resource to disposal; they cannot promote it to idle. Capacity remains charged until `poll_dispose` succeeds and `driver.closed` acknowledges the removal. A disposal error records diagnostics, switches to forced cleanup, and retains the entry for retry. Host loss force-polls every resource and then relies on each resource’s socket-terminating Drop fallback; it never acknowledges reuse.

Reusable release requires both a live lease and the resource’s independent idle/authenticated/fully-cleaned predicate. In the native resource, that predicate is true only when the pump is absent and the whole `SshConnection` owner has returned.

**Per-remote composition held.** Each callback factory owns one `RemoteTransport`; discovery and negotiation retain one cloned `BlockingStream`. A second service cannot open while that stream is active. Explicit close removes the active handle and waits for EndWrite, response drain/discard, endpoint cleanup, and close facts. Drop only cancels and makes no reuse claim. Open refusal becomes a network-class error and has no fallback path.

The loopback fixture verifies host trust before authentication, injects exactly one authenticated connection, binds only exact fixture URLs, and uses libgit2 as the client. Two clones, two pushes, and one fetch used five channels over that connection across two repositories. Object IDs and the shell-injection marker were checked. The worker’s bounded queues, message turns, watchdog, shutdown, and join keep fixture failures observable.

## 3. Risks and next action

This checkpoint deliberately leaves the three production modules outside production module wiring. The controlled worker injects an already trusted session and proves sequential local composition; it does not establish credential policy, URL routing, production scheduling, concurrent native connections, platform parity, or selected-source distribution.

Production wiring must preserve the documented private preconditions: the pump mirror must cover the negotiated receive window, leases must remain paired with their originating host, and the worker must continue servicing both stream and pool deadlines independently. The next action is the separately bounded production endpoint-wiring package, followed by the operator-deferred integrated platform/source qualification batch.
