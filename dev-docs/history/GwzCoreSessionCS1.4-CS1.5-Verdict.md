# Core session plan CS1.4 + CS1.5 (the gate and context freeze) — first review verdict

Date: 2026-09-28. Status: **NO-GO on the nine files at the SHA-256 values in the lane owner's `cs1415-object.sha256` (diff SHA-256 `e927503e…`, 1912 lines): [Consistency](GwzCoreSessionCS1.4-CS1.5-ReviewConsistency.md) and [Safety](GwzCoreSessionCS1.4-CS1.5-ReviewSafety.md) each reported NO-GO, and each committed in advance to GO on a revision that resolves its blocking finding as specified.** This verdict accepts nothing.

The object was steps CS1.4 (contexts, gate, token and `open`) and CS1.5 (the endpoint environment snapshot) of the [core session plan](GwzCoreSessionPlan.md), uncommitted in gwz-core's working tree:
- `src/session_host/{mod,limits,gate,context,environment}.rs` and `src/session_host/environment/tests.rs`;
- `src/lib.rs`, `docs/RustApi.md` and `scripts/checks/process_globals_allowlist.json`.

The plan records the tier: a dual review, since the two steps freeze the gate and context interface together.
- Both reviewers read the object against gwz-core `bd538656`, root `4bf52e00`, gwz-cli `ebbea902`, gwz-py `0b535dc5` and gwz-transport `a7a36aec`.
- They used frozen copies of the plan, the contract and the reuse design, and the accepted server design at `61d8dfa7…`.
- Both verified the tuple at the start and the end, and both ran `cargo check --lib` and `cargo test --lib session_host` (30 passed).
- Neither read the other's report. The reports were filed only after both had finished.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | NO-GO | 0 | 0 | 1 | 0 |
| Safety | NO-GO | 0 | 0 | 1 | 6 |

## Blocking findings

| ID | Axis | Finding |
| --- | --- | --- |
| C-P2-1 | Consistency | `EnvironmentSnapshot::capture()` reads the process environment inside gwz-core, and the allowlist inventories that read as `permanent`. Contract §5.6 says core never reads the environment itself. No disposition the controlling documents define admits the read: `permanent` entries carry no session-relevant state, and `debt` entries are scheduled for removal. The checker also cannot see a call to `capture()` from session-path code. |
| S-P2-1 | Safety | `capture_takes_each_process_variable_once` compares two sets of hex-encoded `name=value` lines with `assert_eq!`. On failure it prints the whole live process environment, and the hex defeats CI secret masking. |

Neither reviewer classified a finding as architectural. The gate's states, the token's authority, the host context's lifetime, `open`'s contract and the snapshot's secrecy in production code all held. This was the first round; the two-round cap has one round left.

## Blind convergence

Reviewing blind, the two axes landed on the same defect:
1. **`capture()` in core.** C-P2-1 and S-P3-1 name one root cause from opposite sides: the capture point lives in core, the `permanent` label contradicts O9's definition, and the process-globals ratchet cannot see calls to the wrapper. Consistency rated it P2 as a contract breach; Safety rated it P3 as a blind spot in the O9 release gate. Both proposed the same correction: take the read out of core.

They also converged in their risk sections on four things:
- the read size against the frame size (S-P3-4);
- cancel callbacks that run on the reading thread;
- crossings that run under the gate's lock (S-P3-3);
- the approximation in the Windows name folding (S-P3-6).

## Confirmed by the reviews

- **The gate.** The three states refuse and ignore exactly what O8 and §5.6 require. Once `revoke` returns, no crossing is in flight, and nothing but the token cancels (§5.3). Every interleaving of `cancel` and `on_cancel` either runs a callback once or refuses it.
- **Limits, `open` and the extension points.** The limits carry §1's defaults and its one rule. `open` validates before any effect. `non_exhaustive` leaves room for `transport_off` and TR1.4b's registry and bounded `shutdown` without breaking a caller.
- **Naming.** `Limits` rather than `SessionLimits` is the consistent name: `lib.rs` re-exports every generated type at the crate root, and the server design's schema defines `SessionLimits`.
- **The snapshot in production code.** It has no `Debug`, `Display` or serialization of a value, and its errors name only an index. Its buffers are wiped on drop, and the spawn helper gives a child `env_clear()` plus the snapshot. WTF-8 handling can only make a byte-for-byte comparison refuse, never falsely accept.
- **Code rules.** Braces and conditional-compilation boundaries hold, every new file is under 500 lines, and both steps are within budget.

## Carried to later steps

These are notes the reviewers attached to later steps. They are not findings against this object. The plan's owners of those steps take them:

| Step | Obligation | Source |
| --- | --- | --- |
| CS1.2 | Channel queues are bounded by counters, never pre-allocated with `with_capacity(limit)`: `Limits` accepts counts up to `usize::MAX - 1`. | Safety risks |
| CS1.6 | Record whether handlers get the token through `OperationServices`, which 44 sites take today, or through `HandlerContext` once and for all. | Consistency risks |
| CS2.2, CS2.12 | A session context never drops with a live, uncancelled token. Once CS2.6 puts the operation table in the context, consider cancelling every token in `SessionContext::drop`. | Safety risks |
| CS2.4, CS2.5 | Split `gate.rs` (479 lines) with rust-split when the call's record joins `GateScope`. | Consistency risks |
| CS2.10 | Count each record's encoded size against `read_bytes`, and test that a reply at the limit fits in one frame (with S-P3-4's bound below). | Both |
| CS2.11, CS3.7, CS3.9 | A wait happens after its crossing, never inside one, and wakes on the token. Each step's review checks its wake-on-cancel row. | Consistency risks |
| CS2.12 | Measure close latency while a worker emits events in a loop: the mutex is not fair, so a busy reporter can delay `revoke`. | Safety risks |
| CS3.7 | Every `on_cancel` callback is non-blocking. Add a test with a deliberately slow callback and a concurrent `session.close`. | Both |
| Socket host (Phase 7) | Keep the server design's entry bound, since a snapshot's `insert` is quadratic in its entries. | Safety risks |
| Phase 1 exit | The Windows-only tests (WTF-8 equality, case-insensitive names, the child probe) are first observed in Windows CI. | Consistency risks |

## Next action

The [remediation plan](GwzCoreSessionCS1.4-CS1.5-RemPlan.md) gives every finding one disposition. The implementer applies it in one patch. The same two reviewers then re-verdict the revision with their context intact, each over its own findings and every changed range.
