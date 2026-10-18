# SSH Local Integration Checkpoint — Code-Axis Review

**Review object:** SSH pump, physical pool host, and per-remote native Git composition at `gwz-core` `f39a6ed260332534aee8b0cf73955803b6a5bf81`; controlling DRAFT `dev-docs/GwzRemoteTransportSshIntegration.md`; 2026-09-21  
**Baseline:** root `d1273951ec5b2746f9215206440e5ffb56232293`; `gwz-core-evidence` `359d4fbf236182192f035ca8e42e3eb756c1a4ad`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show`.  
**Date:** 2026-09-21  
**Axis:** Code architecture, interfaces, call graphs, ownership, failure paths, and compatibility reality. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings. This accepts only the bounded local integration checkpoint; it does not accept production activation, platform parity, selected-source packaging, trust/credential policy, or general worker routing.

---

## 0. Evidence base

I inspected the controlling design and root checkpoint; the complete changed range; `ssh_pump.rs` lines 1–425, `ssh_pool.rs` lines 1–225, and `ssh_remote.rs` lines 1–100; retained `ssh_channel.rs`, `ssh_connection.rs`, and `stream_io.rs`; the relevant pinned `gwz-transport` stream and pool machines; all new fake and native fixture sources; the isolated manifest and lockfile; and the private campaign README, inputs, red logs, and final log.

All 16 SHA-256 fingerprints recorded in the final campaign inputs match the committed core blobs. The implementation meets its recorded ceilings: pump 425 lines, pool host 225, remote bridge 100, and new test support 1,144 lines.

I reran the sole permitted command with Rust 1.95, offline and locked. All 21 tests passed: 2 channel, 5 pool, 1 native pooled-remote, 8 pump, 3 regression, and 2 remote-bridge tests. The native result reported one physical open, five Git service channels, one authenticated session, and verified clone/push/fetch object IDs across two repositories.

The tuple was verified before and after review. Final trees were root `4edcc4e9076ffc7ff7efbef7f06a07e4276d0055`, core `2b79519ade4ccd749c0ce210007bb9a40b23446d`, evidence `534c180b0b0383359b64288196c214d602cb57ee`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Only the two permitted root prompt files were untracked.

## 2. Invariant analysis

The message pump validates envelopes through `MessageEndpoint` before native forwarding. Its forward mirror is bounded, and request bytes are consumed from the stream only after the SSH sink accepts the same prefix, preserving Window and Flushed meaning. Reverse output, stderr, and forward writes progress independently; reverse and retained stderr memory are bounded, and stderr work is capped per turn. Network progress is recorded only for actual backend bytes. Exact I/O expiry is checked before backend work, while Close switches to the cleanup clock. EOF, channel finish, nonzero service status, terminal errors, cancellation, and disposal all prevent reuse unless channel cleanup completed and the Closed message was drained.

The pool host retains each physical owner through connect cancellation, late completion, failed connect, idle close, and forced disposal. Capacity is acknowledged only after the resource reports disposal. Deadline advancement precedes completion polling, so an exact deadline defeats same-turn readiness. Reuse requires both the transport disposition and the concrete resource’s idle/reusable state; stale leases cannot reach resources. Dropping the host invokes forced disposal and then relies on the Resource drop contract for final socket termination.

The remote adapter creates owned state per remote, selects stateful smart transport, opens one stream for discovery and transfer, rejects a second concurrent service, waits for endpoint cleanup on close, and cancels without blocking on drop. Open errors remain network errors and do not invoke another transport. The native test exercises two clones, two pushes, and a fetch through five SSH Git commands while reusing one authenticated connection across two repositories.

The new modules remain preactivation-only and introduce no public API, wire format, production dependency, global registry, subprocess fallback, trust decision, or credential resolver.

## 3. Risks and next action

The remaining risks are the explicitly deferred platform and selected-source qualification batch and later production trust, credentials, URL routing, and worker activation. Sequential loopback evidence does not establish those properties. The next action is to accept this local composition checkpoint and subject each deferred activation boundary to its planned separate review and evidence gate.
