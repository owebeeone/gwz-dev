# Windows HTTPS WH1 round 3 — STATE-AXIS RE-VERDICT

**Review object:** The round-3 correction, the final round of non-architectural corrections, under `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-3.md` at root `c8ebae9`. It is committed and awaiting re-verdict by the round-2 State reviewer. The ranges are:
- gwz-core `f76cf4cc..21f9e15e`, the State-2 P2-4 correction;
- gwz-transport `966763e4..cd007b68`, Code-2 P3-1's test only;
- gwz-core-evidence `56d93909..1add0746`, `portable-rem3/`;
- root `1513c214..c8ebae9`: the filed round-2 reports, Verdict-2, RemPlan-3 and the lock moves.

The review covers limited WH1 only.

**Baseline:**

| Repository | SHA |
|---|---|
| root | `c8ebae9a5dee0e876a96484607af3b92ae0298d3` |
| gwz-core | `21f9e15ed4360c031f6d2c803224b173f82b3111` |
| gwz-transport | `cd007b6868905543caa212155ad8ec99b3a4052c` |
| gwz-core-evidence | `1add0746a1ffc50cd4f6cbe479c630b9cc2babeb` |
| gwz-cli / gwz-py / gwz-sspi | `6ab16d46…` / `5df15766…` / `582ec001…`, unchanged |

How sources were read:
- I read sources with `git show`/`git diff` by SHA.
- The tuple and root `gwz.conf/gwz.lock.yml` matched at the start and the end of the review.
- The only dirt was the excluded untracked files and the lane-root build directories, and tracked trees were unchanged after my runs.
- Root `19beeeb`'s placeholder message is out of scope as directed. Its content is the core lock move to `21f9e15e`, which I confirmed.

**Date:** 2026-10-04

**Axis:** State. This review covers:
- closure of P2-4's original interleavings;
- an attack on the new endpoint routing: invalid input, stale input for live keys, sealed-route deadline Cancels, stuck states, and ordering against retirement;
- continued closure of P2-3.

It is independent, adversarial and read-only. Nothing here relies on the parallel axis. The lane owner files it verbatim.

**Verdict: GO.**
- State-2 P2-4 is closed.
- State-1 P2-3 remains closed, and so do round 1's State P2-1/P2-2 and Code P2-1/P2-2.
- No P0–P3 is open.
- **NEW ARCHITECTURAL root causes: none.**

This GO does not satisfy RemPlan-3's separately required exact-source native Windows refresh. The evidence README records that refresh as still owed, so limited WH1 acceptance must wait for it.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State-2 P2-4: a stale admitted action for a retired HTTPS stream closes an HTTPS-only endpoint Session | <ul><li>Without an SSH engine, non-Open, non-CheckIdentity actions go to HTTPS, whose `accept` treats an unknown or retired key as no-work.</li><li>Opens stay routed by scheme, and CheckIdentity stays fatal.</li><li>Sessions with SSH are unchanged.</li></ul> | <ul><li>I re-traced all three original interleavings (below). Each now ends in `HttpsEndpoint::accept`'s unknown-key `Ok(())` (`https_endpoint.rs:184-218`), not in `close_state`.</li><li>`stale-actions-red-v1` used the base pump (`22ade397…`, the `f76cf4cc` bytes) with the final tests, and I verified those input hashes against git.</li><li>On that RED, exactly the three P2-4 cases fail with "a stale action closed the HTTPS-only Session", and the identity-check control passes.</li><li>The mutation that drops CheckIdentity from the exclusion fails only the control.</li><li>My rerun: 4/4, then 20/20 repeats.</li></ul> | **Closed** |
| State-1 P2-3: the final native publication check preceded the mux lock | Unchanged since round 2 | <ul><li>The round-3 production diff is the one hunk at `pump.rs:53-69`.</li><li>The handoff loop (`pump.rs:162-253`) is unchanged: the seal check, `before_handoff`, `publication_check`, `send_if`, and the refusal and WouldBlock branches.</li><li>`https_endpoint.rs` gains only the test-module line.</li><li>Transport `src/` is byte-identical: `asynchronous.rs` is `31603ac8…`, as at `966763e4`.</li><li>My reruns: `native_publication_session_` 3/3 then 10/10; `https_cancel_mux_tests::` 9/9; transport `mux_async` 7/7.</li></ul> | **Remains closed** |
| Round 1's State P2-1 and P2-2, and Code P2-1 and P2-2 | Unchanged | Capacity, constructor, collection and qualification code is untouched. Recorded results: qualification 4/4, paired constructor/capacity 16/16, cleanup 2/2. I reran endpoint/mux: 9/9. | **Remain closed** |

**P2-4's original interleavings, re-traced at `21f9e15e`.** The setup is an HTTPS-only endpoint Session: `state.engine == None`, per `session.rs:390-415`.

**1. Refusal, backpressure and a racing Cancel.**
- **Pass N:**
  - The in-lock admit refuses S's native Opened, and `fail_publication` retires S.
  - The converted OpenFailed's send returns WouldBlock, so S's mux route stays Opening.
  - The initiator's Cancel(S) is admitted to `actions`.
- **Pass N+1:**
  - `step` removes the retired entry (`poll.rs:184-186`).
  - The incoming loop takes Cancel(S). It carries no Open and `owns` is false, but `state.engine.is_none()` holds and its kind is neither Open nor CheckIdentity. So `https` is true (`pump.rs:57-69`).
  - The action goes to `HttpsEndpoint::accept`, which finds no entry and returns `Ok(())`. The session stays open.
- The pending OpenFailed(Timeout) is sent once the queue drains, and it is the stream's one terminal.

Test (a) runs exactly this sequence:
- four filler Opened fill the outbound queue;
- scripted readings of D−1ns, then D+1ms;
- Cancel(S) is delivered while S is still Opening.

It asserts:
- the session stays open;
- the stream has one terminal: OpenFailed(Timeout, `Effect::None`) with authenticated facts;
- the clock was read twice and the route was revoked;
- the charge stays at (1,1) with `try_reserve` returning None until the real Connection drops, then falls to (0,0);
- nothing further is published;
- a second request on the same Session opens and completes cleanly.

**2. The same race without backpressure.**
- Cancel(S) is admitted between pass N's last `next_action` and the OpenFailed's enqueue.
- Routing in pass N+1 is the same.
- This variant is not separately executed, but it takes the same kind-agnostic path.

**3. A deterministic endpoint-side request drop with an idle HTTPS entry on a live route.**
1. `ClientRequest::drop` (`request.rs:346-351`) calls endpoint `Session::cancel`.
2. `Mux::cancel` enqueues Cancel actions and sets `route.cancel`, and `cancel_request` retires the entry.
3. The next pass removes the entry, and its Cancel becomes no-work.
4. Any pending item for the sealed request is dropped by the seal check (`pump.rs:184-191`).
5. The mux synthesizes the Cancelled terminal.

Tests (b) run both idle states:
- a native Opened admitted under the lock but held by WouldBlock;
- a published POST awaiting its first Data.

They assert the session stays open, there is exactly one Cancelled terminal (OpenFailed or Failed), nothing further is published, and, in the native case, the charge is held until real disposal.

The remaining triggers named in State-2 hit the same disjunct: collection-time and pre-lock refusals, and ordinary endpoint terminals.

## Changed-range analysis

**gwz-core** (4 files, +480/−3):
- **Production:** one change. `pump.rs:65-69` adds a third disjunct to the HTTPS routing flag. The flag is read only in the endpoint branch (`pump.rs:70-95`); driver sessions never read it.
- `https_endpoint.rs` declares `mod stale_action_tests;` inside the existing `cfg_if!` `all(test, unix)` boundary. This complies with the explicit-boundary rule.
- `https_cancel_mux_tests.rs` makes two helpers `pub(super)`.
- `stale_action_tests.rs` is new and test-only, with no item-level cfg.

**gwz-transport** (+39): Code-2 P3-1's probe test only.

**Root and evidence:** documents, lock moves and `portable-rem3/`.

**Scope.** All changes are within RemPlan-3. I recounted from the cumulative bases:
- 58 files: 55 source/test/build files, which is at the ceiling but not over it, plus 3 inventories;
- 2,438 gross non-inventory added lines and 279 deleted, within 2,600.

There is no change to any API, owner, dependency, schema, wire or activation, and no dummy SSH owner.

**NEW ARCHITECTURAL root causes: none.** The change gives HTTPS-only endpoints the existing stale-input contract of `PlacementEndpoint::accept` (`admission.rs:34-39`).

## 0. Evidence base

**Documents:**
- RemPlan-3 and Verdict-2;
- the filed State-2 (its headings and P2-4);
- the evidence README's round-3 section and `portable-rem3/commands.json`;
- the stale-action RED, GREEN, repeat and mutation logs, and the mutation diff;
- the transport atomic GREEN, the split-body mutation log and its diff;
- the control logs;
- `final-sources-v1.sha256`, `sha256-before.txt` and `stale-actions-red-v1.inputs.sha256`, all checked against `git show` bytes.

**Sources:**
- the complete round-3 diffs;
- full `pump.rs` at `21f9e15e`;
- `https_endpoint.rs:175-219,367-386,488-502`, `poll.rs:184-237`, `requests.rs:37-104` and `request.rs:324-351`;
- `placement_endpoint/admission.rs:1-80`;
- gwz-transport `mux/routing.rs`, `mux/mod.rs:377-668` and `binding.rs:104-133`.

**Commands.** All used `CARGO_INCREMENTAL=0`, with targets under `/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-state/`:
1. `gwz-transport`: `cargo +1.95.0 test --locked --test mux_async` gave 7 passed.
2. The recorded `stale-actions-green-v1`, `publication-session-green-v1` and `endpoint-mux-green-v1` core commands, with `--manifest-path …/scratchpad/wh1-rem3/cand/Cargo.toml` and `CARGO_TARGET_DIR=…/core-target`, gave 4, 3 and 9 passed. The log is `core-rem3-v1.log`.
3. The built binary passed `stale_action_tests:: --test-threads=4` 20/20 and `native_publication_session_ --test-threads=3` 10/10.

I made no file or git mutation in any repository and read none of the parallel reviewer's materials.

## 1. Findings

None.

## 2. Invariant analysis

Each attack on the new routing failed, as follows.

**Invalid input becoming silent success: it cannot.** The disjunct affects only actions that no HTTPS entry owns, on endpoint Sessions with no SSH engine. Before the change, those actions went to the absent engine and closed the session. The new disjunct cannot let invalid input through:
- The mux admitted every such action against a live route and a valid transition (`routing.rs:86-99,265-302`).
- Protocol-invalid input still disconnects at the mux, and bootstrap (stream 0) never becomes an action.
- Opens are still routed by scheme. An SSH Open still closes the session.
- CheckIdentity stays fatal, as the control and its mutation show.
- A non-Open action with `state.https == None` still yields `InvalidRequest`.
- Owned keys route exactly as before. That includes input a live stream rejects: a peer `deliver` error becomes `Protocol` and closes the session.
- A non-Open `accept` never returns WouldBlock, so `state.incoming` gains no new occupant.

**Stale input for a live key: it does not misbehave.** This path is unchanged. When a retired entry is still awaiting preparing or serving work:
- **Cancel:** it reaches `cancel_entry`, whose output `take_outbound` never takes for a retired entry. No second terminal results.
- **Other kinds:** the entry's token is already cancelled, or delivery is skipped for retired entries (`https_endpoint.rs:196-216`).
- **No serving spawn on a retired entry.** The only retirement that keeps `prepared` is `take_outbound` of a peer terminal. That leaves preparing and serving as None, so `step` removes the entry before the next incoming loop.

**A sealed-route deadline Cancel: handled.** On expiry the mux queues its own terminal and a Cancel action, then removes the route (`mod.rs:530-600`).
- If the entry is still live, `cancel_entry` handles it as before.
- If the entry was removed, the Cancel is now no-work; before the change it closed the session.

The same holds in two further cases. In both, the mux synthesizes that route's Cancelled terminal.
- an Open that `Session::cancel` discarded from `state.incoming` (`requests.rs:51-57`);
- an action that `Mux::cancel`'s retain removed from the actions queue.

**New stuck states: none.**
- Every retirement either produced the stream's terminal (`fail_publication` into `state.pending`, or `take_outbound` of a terminal) or sealed the request, in which case the mux synthesizes the terminal. Dropping a stale action therefore never strands a live route.
- The WouldBlock retry and InvalidRequest tolerance are unchanged.

**Ordering against `step`'s removal: none.** The outcome is now independent of order:
- an action drained before the entry is removed meets a retired entry and is no-work;
- an action drained after meets an unknown key and is no-work.

Stream ids are unique within a session, so a removed key is never reused.

**Unchanged shapes:**
- Sessions with SSH: the disjunct is false.
- Driver sessions: the flag is unused.
- The publication path (P2-3) is untouched.

**Lock order:** unchanged. Code-2 P3-1's probe also tests wake-after-release: its waker re-enters `next_message` and would deadlock if woken under `inner`. The split-body mutation fails the probe with `[None]`.

**Test fidelity:** adequate.
- The RED is the base pump plus the final tests, verified by hash.
- RED failing on case (a) itself proves the full-queue precondition: had S's terminal been enqueued first, the mux would have dropped the Cancel and the test would pass.
- Case (a)'s second request is anonymous, because the native fixture authenticates only once. That suffices: the property under test is the same Session's liveness.

**Fail-closed direction:** held. The change only suppresses a session close for input that has no remaining endpoint work, and it never publishes anything.

## 3. Risks and next action

- **The native refresh is still owed.** RemPlan-3 requires the exact-source native Windows refresh before acceptance: the MSVC build, the qualification runner, and the provisioned CLI and installed wheel normal paths with their refusals. No `raw/wh1-rem3-*` receipt exists yet. If the refresh forces a source change, the tuple changes and needs a re-verdict.
- **Two variants were not separately executed:** the no-backpressure race, and stale actions of kinds other than Cancel. Both go through the same kind-agnostic disjunct as the executed cases.
- **CheckIdentity stays session-fatal on the Windows route, by design.** `Bound::check_identity` (`binding.rs:104-133`) does not exclude it on an HTTPS-only binding. The qualification driver never sends identity checks, so only a misbehaving peer can trigger it, and only against its own session.
- **The patch is at the 55-file ceiling.** Any further file needs a new disposition.
- **Deferred REDs:** strict Clippy RED45 and the owner-IR pin mismatch are unchanged, as recorded. I did not rerun them.
- **Next action:** perform the native refresh at this tuple, then file the merged verdict.
