# Windows HTTPS WH1 round 2 — STATE-AXIS REVIEW

**Review object:** The round-2 correction for State-1 P2-3, as committed. It is awaiting fresh review. Ranges:
- gwz-core `261eaca55dca4067548027e8976ff0249a34d2f3..f76cf4cc861f5697817ef43be784c6c88558b26e`;
- gwz-transport `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578..966763e429c6c894da52e498b6f95ffd3d5ab5b5`;
- gwz-core-evidence `053121cc97664e46539c07d77cdad4effb481955..56d93909781cc06626054d1210307f483efb72f8`.

The controlling document is `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-2.md` at root `1513c214`. The review covers limited WH1 only.

**Baseline:**

| Repository | SHA |
|---|---|
| root | `1513c214fbc9bc492875a8244c7ed62ed4169a3a` |
| gwz-core | `f76cf4cc861f5697817ef43be784c6c88558b26e` |
| gwz-transport | `966763e429c6c894da52e498b6f95ffd3d5ab5b5` |
| gwz-core-evidence | `56d93909781cc06626054d1210307f483efb72f8` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |

How sources were read:
- I read sources with `git show <sha>:<path>` and `git diff <base>..<sha>`.
- The tuple and root `gwz.conf/gwz.lock.yml` matched at the start and the end of the review.
- The only dirt was the excluded untracked files and the lane-root build directories.
- Tracked trees were unchanged after my test runs.

**Date:** 2026-10-04

**Axis:** State. This review covers:
- publication arbitration under the mux lock;
- the refusal outcome;
- races and stuck states after a refusal;
- lock order;
- the fail-closed direction;
- test fidelity.

It is independent, adversarial and read-only. The other axis runs in parallel, and nothing here relies on it. The lane owner files it verbatim.

**Verdict: NO-GO.** One P2 blocks: P2-4.
- P2-4 is a pre-existing WH1 routing defect, outside the round-2 changed range. The round-2 refusal outcome reaches it.
- State-1 P2-3 is closed.
- State P2-1/P2-2 and Code P2-1/P2-2 remain closed.
- There is no P0, P1 or P3.

I pre-commit to GO on a revision that resolves P2-4 as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State-1 P2-3: the final native publication check preceded the mux publication lock | A native-D Opened goes through `Owner::send_if`. Its admit re-reads cancellation and fresh time under `Shared.inner`, and equality counts as expired. A refusal goes through `fail_publication`. The pre-lock check stays as an early exit. | <ul><li>I re-traced the original interleaving on this tuple; the trace follows this table.</li><li>The RED-v3 input hashes match the inputs they claim to be: base pump `0458bd43…`, base transport `9e442478…`, seam-only endpoint `234a7a42…`, final session `5b9617b5…` and final tests `2f869437…`.</li><li>On RED-v3, the crossing and equality cases publish Opened, and the pre-D control passes.</li><li>Both mutation logs fail the tests that should catch them.</li><li>My rerun: transport `mux_async` passed 6/6, and the core `native_publication_session_` tests passed 3/3, then 20/20 repeats.</li></ul> | **Closed** |
| State P2-1: HTTPS-only capacity installation | Unchanged by round 2 | Capacity, construction and qualification sources are untouched; the round-2 core diff covers only four files. Recorded evidence: `qualification-green-v2` 4/4, paired constructor/capacity 16/16, cleanup 2/2. I did not rerun these. | **Remains closed** |
| State P2-2: the empty constructor advertises HTTPS | Unchanged by round 2 | The constructor guards are untouched. Recorded evidence: `qualification-green-v2` 4/4. | **Remains closed** |
| Code P2-1: late native Open publication, at collection and under backpressure | The collection check is unchanged. `before_handoff` was refactored. | <ul><li>The collection check (`poll.rs:27-50`) is unchanged.</li><li>`before_handoff` (`https_endpoint.rs:308-319`) plus `fail_publication` (`:488-502`) is semantically identical to round 1's inline block: cancellation first, `now >= D`, and the same seven effects.</li><li>The production clock is `tokio::time::Instant::now()`, as before.</li><li>`endpoint-mux-green-v3` passed 9/9, including both round-1 regressions.</li></ul> | **Remains closed** |
| Code P2-2: an engine-free constructor succeeds | Unchanged by round 2 | Untouched. Recorded evidence: `qualification-green-v2`. | **Remains closed** |

**Re-trace of State-1 P2-3's original interleaving at `f76cf4cc`/`966763e4`:**

1. **Preparation completes before D.**
   - Collection reads fresh time below D (`poll.rs:29-38`).
   - It keeps D in `entry.publication_deadline` (`poll.rs:27`).
   - Because D is set, serving is deferred (`poll.rs:111-123`).
2. **The pre-lock check passes at D−ε.**
   - The pump calls `before_handoff` (`pump.rs:183-185`).
   - `publication_refusal` returns `None`, so the Opened is unchanged.
3. **The thread is suspended or contends before the lock.**
   - `publication_check` (`pump.rs:190-193`, `https_endpoint.rs:324-338`) only captures D, a token clone and the clock. It reads no time.
   - The thread then waits for `Shared.inner` in `send_if` (`pump.rs:197-202`, `asynchronous.rs:90-103`, `22-37`).
4. **It acquires the mutex after D.**
   - The admit closure calls `PublicationCheck::refusal` (`https_endpoint.rs:465-483`).
   - That reads the token, then fresh `Clock::now()` (production `tokio::time::Instant::now()`, `:515-527`).
   - With `now >= D`, it returns Timeout and admit returns false.
   - `send_if` returns `Ok(false)` without calling `Mux::send`. Nothing is queued and the route stays Opening. The transport refusal test checks this.
5. **The refusal is converted to one terminal.**
   - `Ok(Some(Timeout))` (`pump.rs:213-221`) calls `refuse_publication`, then `fail_publication`.
   - The Opened becomes OpenFailed(Timeout, `Effect::None`) and keeps the observed facts.
   - The token is cancelled and the authenticated route is revoked. The route is revoked before `prepared` (the lease) is dropped.
   - `output` and `prepared` are cleared, `handoff` is set false, the entry is retired and the peer is disconnected.
   - `handed_off` is never called with the Opened, so `opening_published` stays false and the serving spawn (`poll.rs:157-175`) never fires.
6. **The terminal is published through the normal path.**
   - The converted terminal is placed in `state.pending` and sent by plain `send` on the next iteration.
   - The existing WouldBlock retry and InvalidRequest tolerance apply (`pump.rs:222-238`).

The original steps 5 and 6 (Opened queued, route moves to Stream, guard cleared, serving eligible) cannot occur. The only enqueue (`Mux::send`, `mod.rs:377-395`) runs inside the same `inner` critical section as the fresh read. A preemption between that read and the enqueue is invisible to every other mux user, so the read is the linearization point. No window remains between the in-lock decision and queueing.

## Changed-range analysis

**gwz-transport** (2 files, +213/−1):
- It adds one method, `Owner::send_if` (`asynchronous.rs:77-103`). The method runs `admit` once inside `change`, returns `Ok(false)` without touching the mux on refusal, and otherwise returns the send's result.
- `send` is unchanged.
- `change` still collects wakers under `inner` and wakes them after releasing it.
- The method has no notion of time or policy. It matches the operator's one approved interface.

**gwz-core** (4 files, +382/−28):
- `pump.rs:186-221`: `send_if` is used only when `publication_check` returns Some, which means an Opened whose entry carries native D. Every other message keeps `send`.
- `https_endpoint.rs`:
  - adds a `clock` field;
  - makes `before_handoff` an early exit for non-Opened messages, with no behavioural change;
  - factors the failure conversion into `publication_refusal` and `fail_publication`;
  - adds `PublicationCheck` and `refuse_publication`;
  - defines `Clock` inside one `cfg_if!` boundary. Production gets a zero-sized type that reads `tokio::time::Instant::now()`; the scripted seam exists only under `cfg(all(test, unix))`.
- `session.rs`: two test-only accessors, inside `cfg(test)` and then `cfg(unix)`.
- Tests: three Session-level cases.

**Evidence:** recorded under `portable-rem2/`.

Everything is within RemPlan-2 steps 1–5 and the round-2 budget disposition. My recount gives 57 cumulative files, which is 54 source/test/build files plus 3 inventories. The added lines fall under the 2,600 ceiling. No other API, owner, dependency, wire or schema change appears.

**NEW ARCHITECTURAL root causes: none in the changed range.**

The new P2-4 lies outside the range:
- It was introduced by the original WH1 implementation at core `398158b`.
- It is **not** architectural.
- It is not caused or materially widened by `send_if`. The refusal adds one mux-lock release before the converted terminal's enqueue, inside a window that already existed.

## 0. Evidence base

**Documents read**, at root `1513c214`:
- RemPlan-2, ReviewState-1, ReviewCode-1, Verdict-1, RemPlan-1 and Checkpoint-1;
- the budget disposition, including its round-2 entry;
- composition §1, §4 and §8;
- WH1 design §2–§4 and its qualification table;
- the handoff;
- AgentProcessRules §14.4 and GwzProcessOptimization §4.1, the cap rule.

**Sources inspected:**
- the complete round-2 diffs of the six files;
- full `pump.rs`, `https_endpoint.rs`, `https_endpoint/poll.rs`, `session.rs:200-470`, `session/{requests,close,passes,port,local_link}.rs` and `request.rs:200-351`;
- the publication helpers in `https_worker.rs` and `https_worker/native.rs` (`Authenticated`, `logical_deadline`), and `https_worker/prepare.rs:101-125`;
- `placement_endpoint/admission.rs:1-80`;
- the stale-input tests in `fault_tests.rs:413-489`;
- the WH1 base `c011aae` and the original `398158b` diff of `pump.rs`;
- gwz-transport `mux/asynchronous.rs` (all), `mux/mod.rs:1-170,330-668` and `mux/routing.rs`;
- tokio-util 0.7.18 and 0.7.19 `CancellationToken`: `is_cancelled` and the handle refcount take the node mutex, and `cancel` notifies waiters.

**Evidence inspected:**
- the README round-2 section;
- `commands.json` and `implementation-status.json`;
- `publication-session-red-v1/v2/v3` and `green-v1..v5`;
- both mutation logs;
- `transport-send-if-red/green`;
- `endpoint-mux-green-v3`;
- both repeat-20 logs;
- the RED-v3 seam snapshot, diffed against the base and the final;
- `final-sources-v2.sha256` and `sha256-before.txt`, both checked against git bytes.

**Commands I ran.** Both used `CARGO_INCREMENTAL=0` and targets outside the lane:
1. `cd gwz-transport && CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-state/transport-target cargo +1.95.0 test --locked --test mux_async`: 6 passed, exit 0.
2. The recorded `publication-session-*` core command, with `RUSTFLAGS='--cfg gwz_transport_candidate'`, `--manifest-path …/scratchpad/wh1-rem2/cand/Cargo.toml --lib native_publication_session_` and `CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-state/core-target`:
   - 3 passed, exit 0;
   - the log is `…/wh1-rem2-review-state/core-publication-session-v1.log`;
   - 20 further runs of the built binary with `--test-threads=3` passed 20/20.

I made no file or git mutation in any repository. I did not read the parallel reviewer's materials.

## 1. Findings

### [P2-4] A stale admitted action for a retired HTTPS stream closes an HTTPS-only endpoint Session

**Provenance and classification:**
- **Pre-existing.** Core `398158b` changed the endpoint incoming routing.
  - Before: `if state.engine.is_some()` with `.expect("endpoint")`. On the base, stale input always reached the SSH engine, which tolerates it.
  - After: `if state.endpoint_config.is_some()` with `.ok_or(EndpointError::InvalidRequest)`.
- Rounds 1 and 2 left it unchanged.
- **NOT a new architectural root cause.** It is an HTTPS-only omission of an existing endpoint contract, in the same family as State P2-1. The two-round cap is not triggered. GwzProcessOptimization §4.1 permits a third round limited to non-architectural corrections.

**Location** (gwz-core):
- `src/transport_host/session/driver/pump.rs:52-84`:
  - a non-Open action is routed to HTTPS only while an HTTPS entry `owns` it;
  - otherwise it goes to `state.engine`, and `None` yields `InvalidRequest`, which calls `close_state`.
- `session.rs:390-415`: the SSH engine is built only under `cfg(unix)`, so on the Windows qualification route it is always `None`.
- `https_endpoint/poll.rs:184-186`: `step` removes retired idle entries. `step` runs before the incoming loop (`pump.rs:28-32`).

Location (gwz-transport):
- `mux/routing.rs:86-99` and `283-289`: an action admitted while the route is live stays queued.
- `routing.rs:304-319`: the endpoint's terminal removes the route.
- `mux/mod.rs:397-399`: `next_action` pops without any route check.

**Violated invariant:** Stale admitted input must never close a session, and one stream's cancellation must never fail unrelated streams. The existing code establishes this in three places:
- the SSH endpoint implements it: `placement_endpoint/admission.rs:33-39`, "stale admitted input has no remaining endpoint work";
- the driver side tests it (`fault_tests.rs:413`, `:452`);
- WH1 design §4 carries "the existing independent transport delivery contract" unchanged.

In addition, a refusal's single terminal must be published unless the mux's own terminal replaces it.

**Interleaving through the round-2 refusal.** The setup is an HTTPS-only endpoint Session: the Windows qualification route, or the round-2 fixture with `ssh: None`.

1. **Pass N refuses the Opened.**
   - Pass N drains its actions.
   - In the handoff loop, the in-lock admit refuses stream S's native Opened (Timeout).
   - `fail_publication` retires S's entry, which has neither preparing nor serving work.
   - The converted OpenFailed's `send` returns WouldBlock because the control queue is full. It waits in `state.pending`, and S's mux route stays Opening.
2. **The initiator cancels.**
   - The initiator's request is cancelled, through the caller's token or a request drop.
   - Its mux synthesizes Cancel(S) for the live route (`mod.rs:417-436`), and the carrier delivers it.
   - Because S is still Opening, the endpoint mux admits it to `actions` (`routing.rs:283-289,98`).
3. **Pass N+1 misroutes the Cancel.**
   - `step` removes S's retired entry.
   - The incoming loop takes Cancel(S). It carries no Open and `owns` is false, so it is sent to `state.engine`, which is `None`.
   - That yields `InvalidRequest`, which calls `close_state` (`pump.rs:68-84`).
4. **The session closes.**
   - `close_state` clears `state.pending` (`close.rs:41`), discarding the Timeout and its authenticated facts.
   - It disconnects the port (`close.rs:21-23`), which clears the mux's outbound queue and routes.
   - The local link then disconnects the driver port too (`local_link.rs:29-42`).

Without backpressure, the same thing happens whenever Cancel(S) is admitted between pass N's last `next_action` and the converted terminal's enqueue.

Other triggers with the same root cause:
- the pre-lock and collection-time refusals;
- any endpoint OpenFailed, Failed or Closed;
- deterministically, with no race: dropping a Local-placement request while its HTTPS entry is idle on a live route, such as a pending Opened, a published POST awaiting its first Data, or an open held for retry. The path is:
  1. `ClientRequest::drop` (`request.rs:346-351`) calls endpoint `Session::cancel`.
  2. `Mux::cancel` enqueues Cancel actions (`mod.rs:487-511`).
  3. `cancel_request` retires the entry (`https_endpoint.rs:367-386`).
  4. The next pass removes the entry, then drains the Cancel to the absent engine.

On Unix with SSH present, `PlacementEndpoint::accept` absorbs the misrouted action, so only HTTPS-only endpoint sessions, which are WH1's qualification route, are affected.

**Impact:** The direction is fail-closed: progress is lost, and none is invented. It is still a concrete recovery and isolation defect:
- a benign cancellation race or request drop closes the runtime's shared in-process transport;
- every concurrent request's streams fail with CarrierLost;
- a refused Opened's single truthful terminal can be discarded;
- later Local requests reuse the closed `local_endpoint` (`mod.rs:376-416`) until the runtime is recreated.

Neither the round-2 Session test (no initiator Cancel, no backpressure on the converted terminal) nor the normal-path native receipts exercise this.

**Required correction:**
- On HTTPS-only endpoint sessions, treat stale admitted input for an HTTPS stream as no-work, as `PlacementEndpoint::accept` already does. Do not add a dummy SSH owner (WH1 design §2–§3).
- For example, route non-Open actions to the HTTPS endpoint when no SSH engine exists; its `accept` already returns `Ok` for an unknown key (`https_endpoint.rs:184-218`). Alternatively, keep HTTPS ownership of a retired key until its mux route has retired.
- Keep Opens routed by scheme, and keep genuinely invalid input fatal.

**Closure and regression test:** Use a portable HTTPS-only Session (`ssh: None`) through the real pump and mux.
- **Case (a), refusal plus racing Cancel.** Refuse a native Opened under the lock, hold the converted OpenFailed with a full control queue, and admit the initiator's Cancel for that stream before the enqueue. Once a later pass has removed the retired entry, assert:
  - the session stays open;
  - at most one terminal for the stream is published: the converted OpenFailed, or the mux's sealed-cancellation terminal;
  - the physical charge is kept until real disposal;
  - a second request on the same session opens and completes.
- **Case (b), endpoint-side cancel of an idle entry.** Call endpoint `Session::cancel`, or drop the `ClientRequest`, for a request whose HTTPS entry is idle on a live route (a pending native Opened, and a published POST awaiting its first Data). Assert the session stays open.
- **Fail-before and controls.** Both cases must close the session on `f76cf4cc` and pass after the fix. Retain the Unix SSH-present control.

## 2. Invariant analysis

**Arbitration under the publication lock: held.**
- The decision and the enqueue are one `inner` critical section, and the read is the linearization point.
- The equality case is expired.
- RED-v3 is a fair fail-before: base pump and transport, with only the seam added.
- The pre-lock mutation fails with readings `[false, false]`. The outside-lock `send_if` mutation fails as `Ok(false)`.

**Refusal outcome: held** apart from P2-4's later loss path.
- `fail_publication` runs exactly once.
- Facts are preserved and the route is revoked before the lease is released.
- `prepared` and `output` are discarded, and the charges stay until disposal. The test holds a real Connection: (1,1) and `try_reserve` returns None until it drops, then (0,0).
- `handed_off` is never called with the Opened, and serving never spawns.
- After the entry is removed and two more passes run, no further message is published.

**Post-refusal sends.**
- **Ok:** one terminal is published.
- **WouldBlock:** the terminal is retried. The entry may be removed meanwhile; the pending terminal does not need it, apart from P2-4.
- **InvalidRequest:** endpoint Opening routes have no mux deadline (`routing.rs:47,80`), so this is reachable only after a seal. The mux's own terminal wins and is the single terminal.
- **Sealed:** the pending item is dropped and the mux synthesizes the Cancelled terminal (`mod.rs:437-463`).
- **Two terminals are impossible.** Both the endpoint's terminal and the mux's synthesized terminal need the live route under `inner`, and either one removes it.

**Cancellation racing D:** no race. Production cancels the entry token only under the session lock (`https_endpoint.rs:370,429,491`), and the pump holds that lock across the check. The in-lock token read therefore equals the pre-lock read, and its cancel-first precedence matches `before_handoff`. Prepare and serve only read the token, and no `drop_guard` exists.

**Lock order and deadlock: held.**
- The order is session, then `inner`, then the token node mutex, including the dropped clone's refcount.
- Nothing takes these in reverse:
  - `cancel()` never runs under `inner`;
  - `change` and `wait` wake only after releasing `inner`;
  - the wakers are `ThreadWake` (unpark only) or tokio task wakers (schedule only).
- `Registration::drop` takes only `inner`.

**Unchanged behaviour: held.**
- Nonnative Opened and other kinds still use plain `send`.
- The WouldBlock and InvalidRequest branches, and the collection and backpressure checks, are unchanged.
- Constructor and capacity code is untouched.

**Seam scope.** This is consistent with composition §4 and RemPlan-2 step 5.
- The production `Clock` reads `tokio::time::Instant`, which is D's domain.
- Collection still reads the real clock, so production behaviour is identical.
- The tests cover every point where D can fall:
  - prepare to collection: round-1 delayed collection;
  - collection to the pre-lock check: round-1 backpressure;
  - the pre-lock check to the in-lock read: round 2;
  - both readings before D: the pre-D control.

**Test fidelity: adequate.**
- The scripted clock models suspension faithfully, because nothing between the guard and the lock reads time and the token cannot change.
- The real Owner, Session, Connection and route are used.
- The 100 ms held-detection is heuristic, but the mutation evidence shows it discriminates.

**Fail-closed direction: held.**
- No path publishes a native Opened without an in-lock reading below D.
- `refuse_publication`'s silent no-op for a missing entry is unreachable: one continuous session-lock hold spans `publication_check`, `send_if` and `refuse_publication`.

**Crash and kill points.** All of this state is in memory and there are no durable writes. Round 2 adds no panic sites.

## 3. Risks and next action

**Residual risks:**
- The in-lock cancellation read is unreachable in production. It is harmless.
- The latent `refuse_publication` no-op depends on the continuous session-lock hold. Any future change that releases the lock there must convert the message unconditionally.
- The test's held-detection can fail spuriously under heavy load.
- Round-2 evidence is portable only. There was no Windows compile or test of gwz-core, and the CLI and wheel were not rebuilt at `f76cf4cc`/`966763e4`.
- gwz-core's CI transport pin (`a24e70a`) predates `send_if`. This is disclosed.
- Strict Clippy RED45 and the owner-IR pin mismatch are deferred as recorded. I did not rerun them.

**Next action:** Make a bounded, non-architectural correction of P2-4 and add its regression. State-1 P2-3 needs no further change.
