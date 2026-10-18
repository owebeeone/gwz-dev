# GWZ Remote Transport SSH Channel — State Re-verdict

**Date:** 2026-09-20  
**Axis:** State, remediation round 1  
**Verdict:** **GO**  
**Open findings:** P0: 0, P1: 0, P2: 0, P3: 0

## Reviewed tuple

Verified unchanged at review start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `6076c6153f2b4fb74da5179b0ec6ffd2765d81ad` | `f14ca4667546c5bf91c652209cefafc3142ea7a6` |
| core | `f03f5f79bae73d378e575273af0b9ed2a87c052d` | `5ba118dfbc29a67ca27c9b789fdee5404f3e5e58` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Object: core diff `60daef8ca7a471d3e4d1acfd653678676ae1fef9..f03f5f79bae73d378e575273af0b9ed2a87c052d` and the root Phase 3c checkpoint. Current peer reports were not read.

## Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| State P2-1 — abort/drop could lose native teardown EAGAIN and leak the session | **Closed** | `SshConnection` retains the socket shutdown capability with the native session (`ssh_connection.rs:9-18`). `poll_dispose` retains both channel and connection across WouldBlock (`ssh_channel.rs:176-193`). `force_dispose` shuts down the socket before dropping channel and session owners, marks disposal only afterward, and retains ownership if shutdown fails (`ssh_channel.rs:195-209`). Early `Drop` uses the same forced ordering (`ssh_channel.rs:266-269`); idle and partial-open connection drops terminate before the native session destructor (`ssh_connection.rs:47-52`). |

The original sequence was reproduced by the stalled-peer test: the fixture-owned SSH process tree was paused, output drained to WouldBlock, and two `poll_dispose` calls returned WouldBlock without acknowledging disposal. `force_dispose` then terminated the socket and released native ownership; repeated forced disposal remained idempotent (`regression.rs:107-127`).

## Changed-range analysis

Healthy reuse now transfers the entire `SshConnection`, preserving the session/socket lifetime pair, and remains available only after EOF, both response streams, close, and wait-close complete (`ssh_channel.rs:129-166`). Abort, partial Open/Exec, non-WouldBlock native failures, and premature extraction cannot yield a reusable connection. A failed graceful close proceeds to forced termination; a failed socket shutdown returns an error while retaining owners and leaving `is_disposed` false.

The corrected `Write::flush` checks Active state and performs no native flush (`ssh_channel.rs:258-263`). This matches the wrapper’s lack of an application output buffer and avoids ssh2’s inbound-discarding `channel_flush_ex`. The regression reads one advertisement byte, calls flush, then verifies the complete advertisement byte-for-byte (`regression.rs:41-105`).

The process-pause guard records each stopped fixture-owned PID before stopping it and resumes recorded processes during normal return or unwind (`common/mod.rs:204-277`). Trust verification still precedes authentication, and fixture waits and diagnostic buffers remain bounded.

## Verification

The authorized Rust 1.95 test gate passed all five native tests: two lifecycle tests and three regressions, with zero failures or ignores. The exact prescribed rustfmt check and `git diff --check` passed.

No new architectural root cause or concrete State defect was found. Production connection/authentication policy, pool and message-pump activation, platform parity, publication, and network entry activation remain deferred; this verdict does not claim those outcomes.
