# Python transport session v2 foundation — second remediation

Date: 2026-09-24. The corrected foundation at root `bf5c7270d9e8e098d131e20392713267160c1600`, core `eebd41bab1fe7a91249a55b48c39237d8c6c1671`, Python `fe9d6797148cf71315487817d36396d042214a6e` remains **NO-GO** after the [Code](GwzPyTransportSessionV2Foundation-ReviewCode-2.md) and [State](GwzPyTransportSessionV2Foundation-ReviewState-2.md) re-verdicts. The original queued-cancellation P1 is closed. This is the second consolidated foundation correction; candidate activation remains disabled.

| Finding | Correction | Closure proof |
| --- | --- | --- |
| Code P2-2 | Settle both direct and submit eight-slot refusals on the already-issued ID before returning. Audit every post-issuance early return. | Hold eight admission slots, refuse and inspect a ninth via each form; result and event waiting terminate with typed `TransportSessionFull`; release and retry beyond 64 refusals. |
| Code P2-4 | Construction panic must settle all issued but unstarted records before publishing `Closed`. | Reserve A, inject a construction panic through B, read A and B refusals, verify close idempotence and no later admission. |
| State P2-3 | A worker-spawn failure stores a typed pre-effect model refusal and returns the same typed projection, rather than a generic failed result. | Inject thread-launch failure; no `Accepted` or Git effect, typed refusal with effect none, waiter wakeup and slot recovery. |
| State P2-5 | An accepted worker panic overrides any staged success before publication. A success may publish only after transport finish. | Stage a success, inject cleanup panic, race cancel/close/read/release; retain one failure and conservative cleanup, no published success or further admission. |
| State P2-6 | Install and validate physical capacity before registering the lifetime request ID. Keep one deadline from arrival through installation. | Expire after leadership but before mutation, then retry the same ID. A failure after mutation closes the generation without registering that ID. |
| State P2-7 | Guard pool mutation through shared-authority and installed-policy publication; dropping the future after mutation closes and drains the generation. | Poll into retirement wait, drop the future, verify closed host and rejected later admission; verify successful paired policy. |

P2-6 and P2-7 change the capacity mutation boundary. The next settled-tree verdict therefore needs new independent Code and State reviewers under the review-loop rule for a changed architecture boundary. The full byte ledger, timer, public handles, rollover and platform gates remain separate stages, not waived by a foundation GO.
