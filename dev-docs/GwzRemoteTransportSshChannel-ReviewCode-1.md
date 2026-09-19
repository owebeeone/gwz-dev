# GwzRemoteTransportSshChannel Code Re-verdict — Round 1

## Verdict

**GO**

Open findings: **P0: 0, P1: 0, P2: 0, P3: 0.**  
No new architectural root cause was found.

## Reviewed object

- Root: `6076c6153f2b4fb74da5179b0ec6ffd2765d81ad`, tree `f14ca4667546c5bf91c652209cefafc3142ea7a6`
- Core: `f03f5f79bae73d378e575273af0b9ed2a87c052d`, tree `5ba118dfbc29a67ca27c9b789fdee5404f3e5e58`
- Transport: `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`, tree `bf275d0e746b8fc5104f7037cc33e8e685005bc1`
- Taut: `733e8a78897a90f017f4726e4331aed95e8cb977`, tree `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`
- Correction range: `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e..f03f5f79bae73d378e575273af0b9ed2a87c052d`
- Full object range: `60daef8ca7a471d3e4d1acfd653678676ae1fef9..f03f5f79bae73d378e575273af0b9ed2a87c052d`

The tuple and trees matched at review start and end. Core, transport, and taut remained clean. Root noise was limited to the authorized round-one prompts. I did not read another current reviewer’s prompt or report.

## Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| P2-1 — abort/early drop could lose native cleanup after EAGAIN | **Closed** | `ssh_channel.rs:176-205` retains the channel and connection when `close()` returns WouldBlock. `force_dispose` shuts down the owned socket before dropping the channel and session, and records `Disposed` only afterward. `ssh_connection.rs:36-52` supplies idempotent socket termination before session destruction. `regression.rs:108-127` recreates a stalled peer, observes two close WouldBlock results without disposal, then forces disposal and verifies idempotence. |

This directly retraces the original locked-dependency counterexample: the retry-capable Rust owners now survive EAGAIN, while deadline fallback makes subsequent libssh2 close/free observe a terminated socket instead of losing an indefinitely blocked native allocation.

## Changed-range analysis

`SshConnection` now owns both the `ssh2::Session` and a socket handle capable of terminating the shared connection. Successful `into_session` transfers that complete owner only after the normal EOF, close, wait-close, and status sequence. Disposed or failed channels cannot yield it.

The disposal paths cover active, partially opened, failed, and early-dropped channels. `poll_dispose` preserves ownership and readiness state on WouldBlock; other close failures proceed through forced socket termination. `force_dispose` retains ownership if shutdown itself fails, performs channel-before-session destruction after successful shutdown, and is idempotent. `Drop` uses the same forced path as a final fallback. `block_directions` safely returns `None` after disposal.

The independent flush correction is consistent with the Rust `Write` contract: this wrapper has no outgoing application buffer, while locked `ssh2 0.9.6` maps native channel flush to incoming-data discard. The new no-op preserves active-state validation. The native regression reads one advertisement byte, calls `flush`, then verifies the complete advertisement byte-for-byte.

The paused-process fixture arms its resume guard before each stop and resumes during unwind. Its evidence remains correctly limited to the owned macOS loopback process tree.

## Verification and residual scope

The authorized Rust 1.95 gate passed all **5 tests**: two lifecycle tests and three regressions, including twice-blocked disposal, graceful disposal, idempotence, extraction refusal, and advertisement preservation. The authorized formatting check and `git diff --check` passed.

Session/channel exclusivity remains an explicit host precondition because `ssh2` handles are cloneable. Production connection policy, trust/authentication, pool/message-pump integration, network activation, and cross-platform parity remain deferred and are not accepted by this verdict.
