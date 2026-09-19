# GWZ Remote Transport SSH Channel — State Review

**Date:** 2026-09-20  
**Axis:** State  
**Verdict:** **NO-GO**  
**Findings:** P0: 0, P1: 0, P2: 1, P3: 0

## Reviewed tuple

Verified unchanged at review start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `823ffdf50686bae2a222f80d673e7ad122780684` | `9dc6c9ff472e6b98e5ec81bbd9e884eaac2c4311` |
| core | `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e` | `3b4fe1ef5888c88c3feb1081a6a64febe29618e5` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Object reviewed: core diff `60daef8ca7a471d3e4d1acfd653678676ae1fef9..b77f4fef5dbb6958789dd8160fbb74eb67a3f47e` and the root Phase 3c checkpoint. I did not read the current peer report.

## Evidence

The permitted loopback gate passed both tests:

- `receive_pack_reuses_one_authenticated_connection_and_quotes_repository`
- `upload_pack_advertisement_and_abort_keep_cleanup_explicit`

The prescribed Rust 1.95 formatting check passed.

Inspection covered `ssh_channel.rs`, its native fixture, the controlling candidate, checkpoint, README, and ssh2 0.9.6’s channel/session destruction paths.

## Finding

### P2-1 — Abort/drop can leak the native session when teardown would block

**Location:** `gwz-core/src/git/endpoint/ssh_channel.rs:23-35,164-167`; cleanup contract at `dev-docs/GwzRemoteTransportSshChannel.md:24-28` and `tests/transport_ssh/README.md:42-48`.

`abort` only changes the phase to `Failed`; there is no explicit disposal state or `Drop` implementation. Destruction therefore relies on ssh2 0.9.6’s destructors while the session remains nonblocking. In ssh2 0.9.6, `ChannelInner::drop` calls `libssh2_channel_free` once and discards its return, and `SessionInner::drop` similarly calls `libssh2_session_free` once and discards its return. Both native operations can return `LIBSSH2_ERROR_EAGAIN`.

Concrete sequence:

1. A channel is active while the peer stops reading and the outbound socket becomes non-writable.
2. Cancellation calls `abort`, then drops `SshChannel`.
3. Native channel free attempts EOF/close and returns EAGAIN.
4. Rust discards that result and loses the channel handle.
5. Native session free retries channel cleanup, can again return EAGAIN, and its result is also discarded.
6. The Rust-held socket closes, but the native channel/session allocations are no longer reachable. Repeated stalled cancellations accumulate process memory.

This violates the stated bounded teardown/disposal invariant. The passing abort test exercises extraction refusal, but does not force teardown EAGAIN or prove native destruction.

**Required correction:** give the owner a bounded abort-disposal path that cannot lose an EAGAIN native teardown—for example, retain a socket-shutdown capability, force transport termination at the cleanup deadline, and ensure native objects are then freed. Add deterministic fault or stalled-peer coverage that makes teardown initially return WouldBlock and proves final native session/channel disposal before capacity is reclaimed.

## Invariant analysis

Normal operation preserves Open/Exec state across WouldBlock, drains stdout and stderr independently, distinguishes EOF from WouldBlock, gates close on sent EOF plus both drained streams, waits for close acknowledgement before extraction, and poisons non-WouldBlock failures. Early extraction and explicit abort refuse reuse. Command quoting and fixture trust-before-authentication hold. Exit status is documented and implemented as an observation rather than Git success.

Production connection, authentication, agent/trust policy, pool integration, platform parity, publication, and network activation remain deferred and were not assessed.

I pre-commit to **GO** on a revision that resolves P2-1 as specified, provided changed-range interaction review finds no new blocking defect.
