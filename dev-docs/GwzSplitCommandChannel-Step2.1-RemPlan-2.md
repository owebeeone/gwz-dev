# Step 2.1 carrier: remediation plan (round 2)

Date: 2026-10-26. Lane owner's plan after the round-2 re-verdicts on gwz-core `49e79493` (lane p2-carrier): [Code](GwzSplitCommandChannel-Step2.1-ReviewCode-2.md) GO (1 new P3), [State](GwzSplitCommandChannel-Step2.1-ReviewState-2.md) NO-GO (1 new P2, 2 new P3; GO committed on N-P2-1's fix and closure row, no further round needed). No new architectural root cause (count on this object stays 1). This is round 2 of 2.

Convergence: Code N-P3-1 = State N-P3-2 (Unix `turn_away` drain bounded by bytes, not time), found blind by both axes.

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| State N-P2-1 | `try_recv_binding` and `recv_binding_timeout` return a binding-level terminal for a faulted, retired or unknown binding (e.g. `BindingClosed(BindingFault \| Retired)`); `CarrierLink` ends only that join: disconnect its port, record the reason, leave other joins and the carrier running. | `carrier/stream.rs:507–512`, `stream/state.rs:207`, `stream/writer.rs:205–227`, `link.rs` `inbound` | Row: scoped link over bindings `a` and `b`, `a` overrun → `a`'s port disconnected within a bound with the fault recorded, `b`'s port keeps running; plus the retired and unknown-binding variants. Mutation: `try_recv_binding` returns `Ok(None)` for a faulted binding → row fails. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| Code N-P3-1 = State N-P3-2 | One overall 1 s deadline for the Unix drain (as Windows `windows.rs:696`). Row: a refused client trickling one byte every 100 ms; `accept` returns within ~1.2 s and the next client is accepted. | `local/unix.rs:344–362` |
| State N-P3-1 | Carried to 2.2c (memory amplification is already deferred there): derive `max_streams` from the binding's concurrent-stream limit or add a carrier-wide hard cap faulting the largest backlog. Not this patch. | plan (lane owner) |
| Observations | Linux pinning vs PID reuse → 2.3 (use `SO_PEERPIDFD` or compare start time with connection time); `SERVER_PEER_REFUSED = 82` provisional → reconcile with 2.2b's tail codes at merge. Not this patch. | plan / merge |

## Process

One patch on `49e79493`; focused carrier rows (candidate shape) + clippy; remote matrix `p2-1-rem2-20261026-a` and Windows receipt `p2-1-rem2-20261026-a` (unix.rs and the link are platform-relevant), both detached; evidence appended to run `2026-10-25-p2-1-rem1` as `rem2/`; the lane owner commits; State re-verdicts N-P2-1 only (Code optional for its P3).
