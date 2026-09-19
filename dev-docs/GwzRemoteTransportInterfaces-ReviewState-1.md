# GWZ Remote Transport Interfaces — State Re-verdict 1

**Date:** 2026-09-19  
**Axis:** State  
**Verdict:** **GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Object and tuple

Focused remediation review of the Phase 1/2 transport interface candidate, limited to original State finding P3-1 and changed-range interactions.

| Repository | Commit | Tree |
|---|---|---|
| root | `9d0dc7ef5c616d64d52c296ea2fa34d83d21d73e` | `3d4f420b0b6300eca187183a63f5ce97b7cc93a0` |
| `gwz-core` | `ace269896ad80aee923e2e8fd31e565c43de57ed` | `fdf7e5020f3e67c273dfdb5a475cc41064202503` |
| `gwz-transport` | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| `taut` | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

The tuple matched at review start and end. Member repositories remained clean. Root contained only untracked current-round prompts; no peer prompt or report was read.

## Evidence

I read the committed remediation plan and revised checkpoint, inspected transport diff `163feebe..28f5afb`, core diff `ce3bb967..ace26989`, and traced the affected pool/stream clock, async-waker, cleanup and consumer-host paths.

Focused Rust 1.95 execution passed **27 tests** across:

- `wake_amplification`: 3
- `pool_async`: 6
- `io_clock`: 6
- `io_clock_async`: 3
- `pool_io_clock`: 2
- `network_timeouts`: 7

Both changed ranges passed `git diff --check`. The revised checkpoint records the complete 89-test owner suite, generation checks and 18-test isolated archive proof; those broader gates were not rerun.

## Prior-finding closure

| Finding | Status | Exact closure |
|---|---|---|
| **P3-1 — clock-only mutations wake every unrelated async waiter** | **CLOSED** | `gwz-transport/src/pool/clock.rs:6-76` no longer increments readiness revision for quiet ticks. It emits a revision only for real transitions or when a sent cleanup crosses its deadline. `begin_interaction`/`end_interaction` at `:116-162` change timer bookkeeping without waking checkout callers. `src/stream/machine.rs:281-315` likewise changes classification and resets progress deadlines without touching readiness revision. |

The original pool counterexample now leaves the checkout wake count unchanged across ticks at 10, 20, 50 and 99 ms and across helper begin/end; exact expiry at 100 ms wakes the checkout and makes `CancelConnect` runnable. Stream peer progress and `Network`→`Backpressure` update deadlines without waking blocked read or dispatcher futures; actual Data delivery wakes both.

Cleanup remains live: both `CancelConnect`→`AbortConnect` and `Close`→`Abort` become runnable exactly at their cleanup deadline. Real pool work still wakes a pending driver.

`PoolDriver::next_action` holds `&mut self`, so public code cannot retain that receiver while invoking driver clock/helper methods. The host cancels the receiver, mutates/recomputes the timer, then polls again. The tests do not claim an impossible simultaneous borrow; revision gating and direct action tests cover the relevant behavior.

## Changed-range invariant analysis

- Exact and late deadline ordering remains `now >= deadline`; backward ticks remain inert.
- Removing clock-only revisions does not suppress terminal wakes: stream timeout still calls `fail`, and pool request expiry still changes request/cleanup state.
- Helper pause/resume preserves network allowance, including disabled network timing, and repeated helper periods remain cumulative.
- Zero-byte progress remains inert; positive peer progress resets only the network allowance.
- Close retains its independent cleanup deadline and prevents I/O-clock rearming.
- Received prefixes, first terminal cause, mixed-direction host classification, lease discard, and disposal-before-capacity-reclaim behavior are unchanged.
- The pool cleanup-crossing predicate requires `sent && !aborted` and a strict before/at crossing, preventing quiet repeated wakes while preserving one deadline transition.
- Waiter, buffer, request and connection bounds remain unchanged.

## New findings

None.

## Residual risks and next action

Physical delivery, production host timer/dispatcher behavior, adapters, native-platform qualification, publication and remote CI remain deferred as declared. This State re-verdict supports accepting the remediation tuple; combine it with the independent current Code and Surface verdicts before recording either interface freeze.
