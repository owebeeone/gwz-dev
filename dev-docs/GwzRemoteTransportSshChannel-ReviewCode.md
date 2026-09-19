# GwzRemoteTransportSshChannel Code Review

## Verdict

**NO-GO**

Open findings: **P0: 0, P1: 0, P2: 1, P3: 0.**

I pre-commit to GO on a revision that resolves **P2-1** as specified.

## Reviewed object

- Root: `823ffdf50686bae2a222f80d673e7ad122780684`, tree `9dc6c9ff472e6b98e5ec81bbd9e884eaac2c4311`
- Core: `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e`, tree `3b4fe1ef5888c88c3feb1081a6a64febe29618e5`
- Transport: `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`, tree `bf275d0e746b8fc5104f7037cc33e8e685005bc1`
- Taut: `733e8a78897a90f017f4726e4331aed95e8cb977`, tree `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`
- Core range: `60daef8ca7a471d3e4d1acfd653678676ae1fef9..b77f4fef5dbb6958789dd8160fbb74eb67a3f47e`

The tuple and trees matched at review start and end. Core, transport, and taut remained clean. Root noise was limited to the authorized current-round prompt/report files. I did not read another current reviewer’s prompt or report.

## Evidence

- The authorized Rust 1.95 native gate passed: **2 passed, 0 failed**.
- The authorized formatting check passed.
- `git diff --check` passed.
- Inspection covered the channel phase machine, retry behavior, stdout/stderr separation, EOF and close ordering, command quoting, ownership/extraction, the native fixture, and locked `ssh2 0.9.6` / `libssh2-sys 0.3.3` behavior.

## Finding

### P2-1 — Abort and early drop can abandon nonblocking native cleanup

**Root cause and location:** `gwz-core/src/git/endpoint/ssh_channel.rs:164-167` makes `abort()` only set `Phase::Failed`; all disposal is delegated to ordinary field drop. The same applies to any early drop. In locked `ssh2 0.9.6`, `ChannelInner::drop` and `SessionInner::drop` each call the corresponding libssh2 free function once and discard its return value. Those native frees can return `LIBSSH2_ERROR_EAGAIN` in nonblocking mode.

**Violated invariant:** The candidate at `GwzRemoteTransportSshChannel.md:24-27` requires bounded channel disposal and says the pool must await actual disposal before reclaiming capacity. A destructor whose completion cannot be observed or retried does not establish that invariant.

**Counterexample:** Make an active channel’s socket non-writable, call `abort()`, then drop it. Channel free attempts close, receives EAGAIN, and returns without unlinking or freeing the channel. Session free immediately retries attached-channel cleanup and can receive EAGAIN again. Both Rust destructors discard the result, so the native channel/session allocation can remain while the last retry-capable owner is gone. Repeated cancellation can leak native resources, and the host has no disposal acknowledgement it can safely use for pool accounting.

The passing abort test at `tests/transport_ssh/tests/channel.rs:301-309` proves only that `into_session` refuses reuse; it cannot prove native disposal.

**Required correction:** Add an explicit retryable cancellation/disposal state that retains ownership across WouldBlock, exposes readiness interest, and reports completion only after the channel has been closed sufficiently for infallible local release and the session has been disposed. Pool capacity release must consume that completion signal. Plain nonblocking Drop may remain a last-resort fallback but cannot be the claimed cleanup mechanism.

**Closure test:** Through a deterministic native-call seam, force channel close/free to return WouldBlock at least twice, verify that ownership and phase are retained with no disposal acknowledgement, then allow success and verify exactly one terminal disposal acknowledgement and no reusable session.

## Invariant analysis and residual risks

Normal open/exec retries preserve the same channel and command. Read/write errors poison the primitive, empty-buffer reads do not fabricate EOF, stderr is separate, and reuse requires sent EOF, both drained streams, close, wait-close, and observed exit status. The fixed service command and single-quote escaping protect the repository operand, including the exercised shell-special path.

`ssh2::Session` is cloneable, so exclusivity remains a host-enforced precondition. That limitation is stated explicitly and is acceptable for this preactivation checkpoint; the later pool/session owner must keep clones inaccessible. Production connection, trust/authentication, pool integration, network activation, and platform parity remain correctly deferred.
