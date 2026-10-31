# Step 2.2c rework — STATE-AXIS REVIEW (re-verdict, round 1)
**Review object:** step 2.2c (rework) remediation round 1, in lane `/Volumes/projects/limbo/gwz-dev-p2-rework`. Corrected tuple: gwz-transport `96938616`, gwz-core `09f4b8a8`, root `044dad8f`, evidence `33a0c9fa`.
**Baseline:** my round-1 tuple — gwz-transport `9d5e0866`, gwz-core `05ea42b5`, root `49504e6d`, evidence `b79e563d`. Report `GwzSplitCommandChannel-Step2.2c-ReviewState.md`. **Date:** 2026-10-30
**Axis:** State: durable-state semantics and adversity. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO** — all 8 round-1 findings are closed. No P0, P1 or P2 is open. One new P3 (P3-5) is open and does not block.

## Prior-finding closure table
| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P1-1 cancel while opening hangs `Opening::wait` | `cancel_all` on `Opening` sets `cancel_on_open` and `outcome = Err(Cancelled)`. `dropped` means only "no handle". `poll_open`/`RemoteFinish` answer a missing record with a terminal. | My round-1 probe now resolves `Ready` after settle and after carrier loss. An added probe drops the resolved wait, and `op.finish()` resolves with 0 core bindings. Code: `decide()` cannot overwrite a cancel. A late `opened` gets its machine cancelled. A late `open_failed` becomes `Ended` and is reclaimed once the wait (which sets `dropped`) or its `Live` goes. `is_ready()` and `poll` agree. Rows `cancel_rows.rs` and `a_cancel_while_an_open_is_unanswered_resolves_through_the_drivers…` cover it. | Closed |
| P2-1 every pending wait wakes itself | `Shared::with` wakes only waiters whose condition holds (`Wait` + `ready_wakers`). Acquire is woken at head-and-free or on loss. | Probe: 5 pending polls of `registered()` and of an `OpenWait` give **0** self-wakes (round 1: 5 each). Rows cover registered, open, finish and acquire, plus a whole operation driven by counting wakers and drivers with no `settle()`. Carrier loss is still woken through `holds()`. | Closed (a new defect in the fix: P3-5) |
| P2-2 client reports `Effect::None` for a forwarded push on `end_call`/fault | `terminate` uses `effect_after(service, forwarded)`, shared with `cancel_stream` and `fail`. | Probe: echo "pack-bytes" crossed, then `end_call` gives `failure: Some((Cancelled, Possible))`. Rows cover the client-side fault, owner host close and lane cancel. Fetch and unforwarded push stay `None`. | Closed |
| P2-3 stale `bind` sent after retire; carrier re-opens the binding | `purge_ended` drops a faulted binding's unsent messages at `flush`. `CarrierLane` remembers 1024 retired sessions and drops their `bind`. | Probe: "binds sent after retire: 0, client bindings 0, active ops 0". Every pending-end path (finish, cancel, `TimedOut`, fault, revoke) goes through `abort_all`, which sets `faulted`, so the purge applies. Purge and retire both run under the state lock, before `retire_finished`. gwz-core row and mutation logs are in the rem1 evidence (`green-core-remote-lane.log`). | Closed |
| P3-1 receiver outbox unbounded under a bind flood | The receiver stops taking from the lane while `outbox ≥ OUTBOX_ROOM`. | Probe: 20,000 injected binds, 64 held (round 1: 20,000). Against a conforming core this cannot deadlock: the core's pump always reads, and the carrier reader never blocks. | Closed |
| P3-2 refusal snapshot says `Open` after loss | `refusal(namespace, phase)`: `Closed` once lost. Docs say the counts are meaningless. | Code and row `a_refused_snapshot_says_closed_once_the_carrier_is_lost_and_open_before`. | Closed |
| P3-3 `registration()` can read `Pending` after `Registered` | The final `Registration` goes into `last: Arc<OnceLock>`, set under the lock in `retire_finished` before removal. | Code: removal happens only in `retire_finished`, after `last.set`. The fallback cannot be reached. Row `the_registration_never_reads_pending_after_registered…`. | Closed |
| P3-4 direct `#[cfg(test)]` in `receiver.rs` | Moved into a `cfg_if!` block. | Diff confirms it. `remote/` has no other direct `cfg`. | Closed |

## Changed-range analysis
- **What I read.** `9d5e0866..96938616` (17 files, +1299/−108) and `05ea42b5..09f4b8a8` (9 files, +296/−43), in full for production code and by row name for tests.
- **Changes beyond my dispositions** belong to the other axis's dispositions:
  - Code P2-2: `abort_all` maps `Cancelled` to `OpenFailure::Cancelled`.
  - Code P3-2: `OperationRef` admits 128 bytes.
  - Code P3-3: `bind_rejection` refuses `detail`, with the README wire note and a mux row (28 pass).
  - Code P3-4: carrier bind bounds.
  - Code P3-5: docs.
  - The pin moves.
  - None of these crosses into 1.3b/1.3c/1.4/1.5/1.6 code or 2.2b's `Service`.
- **`remote::BindBounds`** (new public type, not named in the plan).
  - **Necessity:** the carrier lives in gwz-core and cannot name gwz-connection's binding limits or a handle's `streams_per_binding`. §3.3's 2026-10-27 amendment assigns this derivation to 2.2c. Carrying two `usize`s across the crate boundary is the smallest public surface that does it.
  - **State semantics:** both are correct.
    - `receive_window` is the per-stream data bound. A conforming sender stays inside it, because `stream_config` clamps the window to `limits.receive_window`.
    - `streams` equals the receiving binding's mux bound: the core's `config.streams()` on the core end, and `served::STREAMS = 64` on the client end, which is also the client's endpoint mux `max_streams`. A peer cannot create queued state on more streams than that mux would accept.
  - **Hazard (not a defect today, no production caller):** `CarrierLane::core(…, BindBounds)` and `remote::handle_with(binding, RemoteConfig)` take the config independently, so step 2.3's composition must pass the same `RemoteConfig` to both. A smaller bound would fault healthy bindings; a larger one loosens the bound. I judge the type justified. I recommend 2.3 derive both from one value (for example, the handle exposing its own `BindBounds`).
- **New root causes.** One, inside the P2-1 fix: P3-5 below. It is not an architectural root cause; it is local to `wait_for`'s eviction rule. **No NEW ARCHITECTURAL root cause** found.
- **Re-attacked invariants:**
  - `abort_all` now overwrites a `cancel_on_open` outcome that has not been polled with `CarrierLost`/`Fault`. If the app already took `Cancelled`, the rewrite is never read. Harmless.
  - The `RemoteFinish` missing-record fallback reports `(Cancelled, Possible)` for any service. It over-reports, and is unreachable while the future holds its `Live`.
  - The retired-name list evicts after 1024 retirements. Session names are fresh serials, and the core-side purge is the primary guard, so this is defense in depth only.

## 0. Evidence base
- **Tuple.** I checked it at the start and the end. It was identical both times: root `044dad8f`, gwz-transport `96938616`, gwz-core `09f4b8a8`, gwz-core-evidence `33a0c9fa`. gwz-transport and gwz-core are clean; the only untracked files are the same out-of-scope ones as in round 1.
- **Read:**
  - `GwzSplitCommandChannel-Step2.2c-RemPlan.md`
  - the two diffs above
  - the rem1 evidence index and its gwz-core `test result` lines: `transport_host` 460 pass / 3 ignored, candidate rows 113 pass, remote_lane 10 pass.
- **Runs.** All used `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p2-2c-review-state` and `TMPDIR=/Volumes/gwztmp`.
  - Lane: `cargo +1.96.0 test -p gwz-connection --manifest-path gwz-transport/Cargo.toml --offline` gives 184 passed. `--test mux` gives 28 passed.
- **Reviewer probes.**
  - Setup: a `git archive 96938616` export to `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/gt2`, with my unchanged round-1 `probe.rs`. A copy is at `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/probe-r1.rs`.
  - All 5 round-1 probes pass (outputs quoted in the table).
  - Added probes:
    - `probe_cancel_during_opening_then_drop_lets_the_binding_retire`: passes.
    - `probe_operation_is_sync`: compiles, so `Operation` is `Sync`.
    - `probe_seventeen_waiters_on_one_registration`: fails, "wakes per waiter: [0, 1, 1, …, 1]" (P3-5).
- **Not run:** gwz-core's candidate build. I did not rebuild it because the machine is shared. My gwz-core claims rest on reading the code and on the rem1 evidence logs.

## Problem, requirement provenance, and scope
- **Authority:** the requested outcome and its authority are unchanged from round 1. The remediation plan, merged by the lane owner from both round-1 reports, is the authority for this patch.
- **Inside the plan:** every change maps to a plan row except `BindBounds`. That type is necessary for the Code P3-4 disposition the plan accepted, and is judged above.
- **Out of scope, and correctly recorded rather than implemented:**
  - idle polling;
  - the stream machine not sending a terminal for a peer cancel;
  - lock order for 2.3;
  - ignored `spawn` results;
  - the release-kind design question.
- No widening of scope was found.

## 1. Findings

### [P3-5] More than 16 concurrent waiters on one registration lose a wakeup
- **Location:** `remote/core.rs` `State::wait_for`. When `same.len() >= WAKERS_PER_WAIT` (16) for one `Wait` key, it removes the oldest waker (`self.waiters.remove(same[0])`), even when that waker belongs to a different, still-pending future.
- **Invariant:** a future that returned `Pending` is woken when its condition holds.
  - Round 1 woke every waiter on every pump, which masked this.
  - Now the driver wakes only the waiters still registered.
- **Reproduction:**
  - `Operation` is `Sync`, and `registered(&self)` can be polled by any number of tasks.
  - Seventeen `op.registered()` futures, each polled once with its own waker, then `bound` arrives (`settle`).
  - Probe `probe_seventeen_waiters_on_one_registration`: waiter 0 gets **0** wakes, the rest 1 each.
  - The same rule applies to `Opening`/`Ending` keys. Those futures have a single owner, and the newest waker is always kept, so in practice only `Registered` is exposed.
- **Impact:**
  - A task awaiting `registered()` alongside 16 others can hang until something else re-polls it.
  - It needs 17 or more simultaneous waiters on one operation, which is unlikely, so the impact is bounded.
  - The row `a_task_polled_with_a_fresh_waker_every_time_does_not_grow_the_waiters` pins the cap but not this case.
- **Remedy:** bound waiters per future, not per key.
  - Give each `Registered`/`RemoteOpenWait`/`RemoteFinish` a unique slot id when it is created.
  - Register under `(Wait, slot)`, replacing that slot's waker.
  - Drop the slot when the future is dropped.
  - Alternatively, keep the cap but on overflow wake the evicted waker instead of dropping it: a spurious wake is allowed, a lost one is not.
- **Closure test:** the probe as a row. 17 or more distinct waiters on one registration each get at least 1 wake on `bound`, while the existing fresh-waker row still bounds growth.

## 2. Invariant analysis (attacks that failed are part of the result)
- **Registration lifecycle.**
  - `Pending → Registered | NeverRegistered(ClientRefused|TimedOut|CarrierLost|Cancelled)` is unchanged and still monotonic.
  - `last` makes the read monotonic across retirement.
  - The stale-`bind` path is closed twice: by the core-side purge and by the lane's refusal to re-open a retired session.
  - No interleaving found that leaves an operation `Pending` forever. A pending binding still ends by `bound`, `bind_rejected`, the 5 s bootstrap bound, a cancel or finish, or a loss.
- **Cancel matrix.**
  - Queued: ended at once.
  - Opening: `Cancelled` at once; a late `opened` is cancelled; a late `open_failed` is reclaimed.
  - Open or Closing: the machine is cancelled.
  - Pending: `NeverRegistered(Cancelled)`, and the bind is purged.
  - Each resolves, with the matching rows present (`cancel_rows.rs`).
  - A cancel on an `Opening` that later loses the carrier reports `CarrierLost`, which is the more informative outcome.
- **Effects.**
  - Core-side `effect_on_loss` is unchanged (over-reports).
  - The client now reports `Possible` for a forwarded push on every abnormal end: cancel, call end, binding fault, owner stream failure, host close.
  - No path reports `None` after forwarded bytes.
- **Wakeups.**
  - Every `Wait` condition (`Registered`, `Opening`, `Ending`) is re-evaluated after the second pump in every `with`, including the driver's.
  - Carrier loss satisfies all three, and queued acquires are woken on loss.
  - The head acquire is woken when a slot frees in the same call.
  - The only lost wakeup found is P3-5.
- **Backpressure.** The receiver stops taking at 64 queued outbound messages. The carrier's reader never blocks and holds per-binding state within `BindBounds`, which a conforming core cannot overrun (window-bounded data, at most 4 per kind, at most 64 streams).
- **Locks.**
  - `purge_ended`, `retire_finished` and `CarrierLane::send`/`retire` all run under the remote state lock, so the purge-versus-retire-versus-send order is serialized.
  - `CarrierLane.retired` is a leaf mutex.
  - No new lock order.

## 3. Risks and next action
- **Next action:** fix P3-5 with the closure row, in this patch or the next one that touches `remote/core.rs`. It does not block GO.
- **Step 2.3 composition:**
  - Build `remote::handle_with` and `CarrierLane::core` from one `RemoteConfig`, so their `BindBounds` cannot diverge.
  - Implement `Calls` without taking a lock that the cancel path also holds; this is the round-1 lock-order note, still recorded in the plan.
- **Windows receipt:** still owed on the corrected gwz-core commit under a new label, as the plan states. Not a State-axis item.

## 4. Out-of-scope observations (follow-up proposals; not findings)
- **Stream-count mismatch (limitation, predates this patch).**
  - `RemoteConfig::streams_per_binding` accepts up to 1024, but the client's endpoint mux, and now the client carrier through `BindBounds::client()`, serves 64.
  - A core configured above 64 has its binding faulted at the 65th concurrent stream: the client mux returns `Capacity`, which `Served::receive` treats as a fault.
  - Proposal: clamp `streams_per_binding` to the client's 64, or carry the stream bound in the bind's limits.
- **Merge prerequisites:** the root-level items from round 1 (`58e506ee`, and `fb62344d` now noted by the plan) are unchanged and recorded in the plan.
