# Gwz remote transport interfaces — Code re-verdict 1

## Object

- **Date:** 2026-09-19
- **Axis:** Code
- **Object:** Phase 1/2 interface candidate, remediation 1
- **Root:** `9d0dc7ef5c616d64d52c296ea2fa34d83d21d73e`  
  Tree `3d4f420b0b6300eca187183a63f5ce97b7cc93a0`
- **gwz-core:** `ace269896ad80aee923e2e8fd31e565c43de57ed`  
  Tree `fdf7e5020f3e67c273dfdb5a475cc41064202503`
- **gwz-transport:** `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`  
  Tree `bf275d0e746b8fc5104f7037cc33e8e685005bc1`
- **taut:** `733e8a78897a90f017f4726e4331aed95e8cb977`  
  Tree `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`

The tuple matched at review start and end. Member repositories were clean; root contained only the permitted untracked current-round prompts.

## Verdict

**GO**

| Severity | Open |
|---|---:|
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

No new finding arose from the corrected ranges or their inspected interfaces.

## Prior-finding closure

| Finding | Status | Verification |
|---|---|---|
| **P2-1 — negotiated Open admission reverted to defaults** | **Closed** | `gwz-transport/src/binding.rs:69-71` now calls `codec::admit_limited` with the installed binding’s receiver limits. The owner regression at `tests/policy.rs` proves 256/257-byte boundaries for operation, destination, and identity metadata. The consumer uses the same negotiated limits for typed admission and actual bounded encoded decoding (`admission.rs:17-38,453-465`). Its paired test covers all three fields and preserves a zero effect delta on rejection (`admission.rs:383-418`). The original 257-byte counterexample now returns effect-free `InvalidRequest`. |
| **P3-1 — native timeout composition evidence stopped before host policy** | **Closed** | The fake host captures connect, I/O, and helper policy; resolves them before its sole effect increment; and constructs exact pool/stream inputs only after successful resolution (`admission.rs:439-560`). The matrix at lines 563-687 independently rejects zero or lengthened requests under finite connect/I/O policy, accepts equal/shorter values, preserves zero and `i32::MAX` under disabled policy, verifies exact request/config values, and carries capped helper remainder. Rejections leave the effect count unchanged. Native policy remains host-owned as required. |

## Evidence

I read the merged remediation plan, revised checkpoint, exact transport/core diffs, corrected binding and consumer call paths, public contract changes, and the clock-wakeup ride-along interactions. Both changed ranges passed `git diff --check`.

Focused Rust 1.95 execution passed **24 tests** across:

- `binding`
- `policy`
- `network_timeouts`
- `pool_async`
- `wake_amplification`

The exact isolated archive proof verified:

- source revision `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`;
- archive SHA-256 `986033108eab2967028dc52c69f94e859ed6cbb78384648f03e88d9703383191`;
- offline locked consumption of the packaged crate;
- all **18 consumer tests**, including both original counterexamples.

The recorded 89-test owner suite and regeneration checks were inspected as prior qualification evidence and were not unnecessarily repeated. No peer prompt or report was read.

## Invariant analysis

Negotiated ingress is now safe at both layers: the public binding check cannot enlarge installed limits, while typed and encoded consumer paths apply identical receiver policy before effects. Bootstrap admission remains under its fixed pre-negotiation hard limits.

The host-policy fixture now distinguishes valid wire representation from endpoint authorization. Finite and disabled native network policies map correctly into pool and stream inputs, and helper time is transferred as remaining allowance.

The clock-wakeup ride-along removes bookkeeping-only notifications while retaining deadline expiry, cleanup work, data readiness, and terminal wakeups. Focused async and timeout tests found no lifecycle regression.

Schema artifacts and public signatures remain unchanged. No carrier, production dispatcher, adapter, credential flow, or CLI/core service entered the object.

## Residual scope and next action

This GO covers only the named schema/admission/message-handoff and stream/pool runtime interface candidate at the exact tuple. Physical delivery, aggregate host enforcement, production dispatch, real timers, SSH/HTTPS adapters, credentials, native-platform qualification, publication, and remote CI remain deferred.

The Code axis may now be recorded as GO for remediation 1.
