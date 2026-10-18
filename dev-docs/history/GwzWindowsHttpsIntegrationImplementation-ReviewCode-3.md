# Windows HTTPS WH1 round 3 — CODE-AXIS REVIEW

**Review object:** The WH1 remediation round-3 correction, committed on 2026-10-04 and awaiting re-verdict by the round-2 reviewers. This is the final round and is limited to non-architectural corrections.
- gwz-core: `f76cf4cc861f5697817ef43be784c6c88558b26e..21f9e15ed4360c031f6d2c803224b173f82b3111`, one commit.
- gwz-transport: `966763e429c6c894da52e498b6f95ffd3d5ab5b5..cd007b6868905543caa212155ad8ec99b3a4052c`, one commit, tests only.
- gwz-core-evidence: `56d93909781cc06626054d1210307f483efb72f8..1add0746a1ffc50cd4f6cbe479c630b9cc2babeb`. This covers `portable-rem3/`, the `wh1-rem3-before-v1` snapshots and the README's round-3 section.
- Root: `1513c214..c8ebae9a`.
  - `bf0eab2` adds documents only: Code-2, State-2, Verdict-2 and RemPlan-3.
  - `10b048e`, `19beeeb` and `c8ebae9` move member pointers.
  - `19beeeb` has a placeholder message but makes the correct lock move. It is out of scope as a defect.
- Controlling plan: `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-3.md` at root `c8ebae9a`.
- Lane: `/Volumes/projects/limbo/gwz-dev-wh1-rem2`.

**Baseline:**

| Repository | SHA (start = end) |
|---|---|
| root | `c8ebae9a5dee0e876a96484607af3b92ae0298d3` |
| gwz-core | `21f9e15ed4360c031f6d2c803224b173f82b3111` |
| gwz-transport | `cd007b6868905543caa212155ad8ec99b3a4052c` |
| gwz-core-evidence | `1add0746a1ffc50cd4f6cbe479c630b9cc2babeb` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| git2-rs (build input, unchanged) | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |

- **How sources were read:** with `git show <sha>:<path>`, `git diff` by SHA and blob-hash comparison.
- **Tuple checks:** the tuple matched at the start and the end of the review.
- **Untracked items:** only the excluded items were present (root prompt and design drafts, `candidate-target*/`, the gwz-core bug report, the evidence `alpha-setup-timeout/` run). None was read.
- **Tracked files:** every tracked file was clean, so tests run in the lane ran on the tuple's bytes.

**Date:** 2026-10-04

**Axis:** Code. It covers the routing change's call graph, interfaces and visibility against the endpoint contracts; closure of Code-2 P3-1; the State-1 P2-3 regression; and scope and budget. This is a re-verdict by the round-2 Code reviewer, with its context intact. Independent, adversarial, read-only. State-2 is used as an input; nothing here relies on the State axis's round-3 report. Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0, 0 P1, 0 P2, 0 P3.
- Code-2 P3-1 is closed. I re-ran the original split-body counterexample independently.
- State-1 P2-3 remains closed.
- On the Code axis, the State-2 P2-4 routing change is consistent with the existing stale-input contract. Its closure is the State axis's verdict.
- No new architectural root cause.
- This verdict accepts the round-3 correction only. Limited WH1 acceptance also needs RemPlan-3's exact-source native refresh, which the evidence records as not yet done.

---

## Prior-finding closure table

| ID | Original counterexample | Re-traced or re-run on this tuple | Status |
|---|---|---|---|
| **Code-2 P3-1**: `send_if`'s admit answer and its queueing were not proven to happen in one critical section | Splitting the body into an admit-only acquisition and a separate send acquisition passed all six transport tests and all three Session tests. | • New test `send_if_admits_and_queues_in_one_critical_section` (`gwz-transport/tests/mux_async.rs:292–330`) passes 7/7 on the committed body in my run.<br>• I re-ran the split body on an external copy built from `git show` blobs. My diff is identical to `transport-send-if-mutation-split-v1.diff`. Only the new test fails, with probe `[None]`; the other six pass.<br>• No lane file was touched. | **Verified closed** |
| **State-1 P2-3**: final native check before the mux lock | The guard passes at D−ε; the lock is acquired after D; Opened publishes. | • Round 3 does not touch the publication path: the `pump.rs` diff is confined to the incoming-routing predicate (`:53–69`), the handoff loop is unchanged, `https_endpoint.rs` gains one module line, and transport `src/mux/asynchronous.rs` is blob-identical (`31603ac8…`).<br>• My runs on this tuple: `native_publication_session_` 3/3 and `https_cancel_mux_tests::` 9/9. | **Remains closed** |
| **State-2 P2-4**: a stale admitted action closes an HTTPS-only endpoint Session | 1. A retired HTTPS entry is removed.<br>2. The stale non-Open action goes to the absent SSH engine.<br>3. `InvalidRequest` triggers `close_state`. | • Traced through the call graph below: on a Session without an SSH engine, the action now reaches `HttpsEndpoint::accept`, which returns `Ok` for an unknown key (`https_endpoint.rs:183–219`).<br>• I hash-verified `stale-actions-red-v1` against the inputs it claims: the `f76cf4cc` pump `22ade397…` plus the final tests and endpoint. In it, the three P2-4 cases fail with "a stale action closed the HTTPS-only Session" and the identity-check control passes.<br>• My run: `stale_action_tests::` 4/4. | Code trace consistent; **closure is State's verdict** |
| Round-1 Code P2-1/P2-2 and State P2-1/P2-2 | As filed in round 1 | `mod.rs`, `session.rs`, `session/capacity.rs`, `https_endpoint/poll.rs`, `retry.rs`, `qualification_tests.rs`, `https_worker.rs`, `session/requests.rs` and `request.rs` are blob-identical from `f76cf4cc` to `21f9e15e`. Endpoint/mux 9/9 in my run; qualification 4/4, paired constructor/capacity 16/16 and cleanup 2/2 in the evidence. | **Remain closed** |

## Changed-range analysis

**gwz-transport `966763e4..cd007b68`** (`tests/mux_async.rs`, +39):
- The `Probe` waker (`:291–309`) and one test.
- No change under `src/`, to `Cargo.toml`/`Cargo.lock` or to `protocol/`.

**gwz-core `f76cf4cc..21f9e15e`** (4 files, +480/−3):
- **`session/driver/pump.rs` (+10/−1):** one new routing clause with a comment, at `:53–69`.
- **`https_endpoint.rs` (+1):** `mod stale_action_tests;` at `:554`, inside the existing `cfg_if! { if #[cfg(all(test, unix))] { … } }` block at `:548–555`.
- **`https_cancel_mux_tests.rs` (+2/−2):** `small_limits` and `open_for` become `pub(super)`.
- **`https_endpoint/stale_action_tests.rs` (+467, new):**
  - case (a): `:307–363`;
  - case (b) with a pending native Opened: `:369–404`;
  - case (b) with a published POST: `:409–442`;
  - the identity-check control: `:447–467`.

**Evidence and root:** evidence files only; root documents and lock pointers only.

Everything is within RemPlan-3. None of the following was added: an API, owner, dependency, schema, wire, activation, `#[path]` edge, candidate-switch site, process global, or `#[cfg]` on an unbraced declaration.

**NEW ARCHITECTURAL ROOT CAUSES: none.** The routing change applies the SSH engine's existing stale-input contract to sessions without an SSH engine. It is non-architectural, as RemPlan-3 requires for the final round.

## 0. Evidence base

**Documents read** (at `c8ebae9a`): RemPlan-3; Verdict-2; State-2, including [P2-4] and its interleaving; my filed Code-2.

**Sources read:**
- The complete round-3 diffs.
- `pump.rs:1–110`.
- `https_endpoint.rs`: `:172–219` (`owns`, `accept`), `:367–410` (`cancel_request`, `shutdown`) and `:545–555`.
- `https_endpoint/poll.rs:184–237` (retain, `take_outbound`).
- `session/requests.rs:37–104` (cancel, seal).
- `endpoint_environment.rs:43–83` (`ssh` is `None` outside Unix).
- `session.rs:371–415` (the SSH engine is built only under `cfg(unix)`).
- `mod.rs:200–240`.
- `placement_endpoint/admission.rs:1–40`.
- gwz-transport `mux/routing.rs:1–100, 257–300`, `mux/mod.rs:467–515` (`Mux::cancel`) and `binding.rs:104–130`.
- The full new test file and the new transport test.

**Evidence inspected:**
- The README round-3 section, `commands.json` and `implementation-status.json`.
- RED: `stale-actions-red-v1` with its `.inputs.sha256`. All five inputs match the blobs: pump `22ade397…` = `f76cf4cc`; endpoint `7c986628…`, tests `4ad3edfd…` and `ad70c292…` = `21f9e15e`; transport `31603ac8…`.
- GREEN: `stale-actions-green-v1/v2`, `repeat20-v1` (20/20).
- Mutation: `stale-actions-mutation-checkidentity-v1` (`.diff` and `.log`). Only the control fails, with "no progress" at `stale_action_tests.rs:269`.
- Transport: `transport-send-if-atomic-green-v1` (7/7) and `transport-send-if-mutation-split-v1` (`.diff` and `.log`, `[None]` at `mux_async.rs:329`, the final line).
- Core controls: `publication-session-green-v1` (3), `endpoint-mux-green-v1` (9), qualification (4), paired controls (16), cleanup (2).
- Gates: lane gate (ok at both commits), cfg guard, inventory (50 sites), process globals, checked-artifact, format (52 files), whitespace, `check --tests` in both switch shapes, transport clippy and Windows MSVC check, package.
- Base failures: `candidate-failures-at-base-v1` shows the same six failures on the round-3 base.
- Hash manifests: `final-sources-v1.sha256` and `sha256-before.txt`, plus the `wh1-rem3-before-v1` snapshots, all match the blobs. `changed-files.json` has SHA-256 `4c47c0f6…`, as the README states.

**Executed by this reviewer** (all with `CARGO_INCREMENTAL=0` and targets under `/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-code/`):

1. **Transport, committed body.**
   ```
   cd …/gwz-transport && CARGO_TARGET_DIR=…/transport-target-r3 cargo +1.95.0 test --locked --test mux_async
   ```
   Result: 7 passed, exit 0.

2. **Split-body counterexample.**
   - I rebuilt the transport tree at `cd007b68` in `…/scratchpad/review-code/r3/transport-split` with a script that only calls `git show`. The extracted `asynchronous.rs` and `mux_async.rs` hashed equal to the committed blobs.
   - I applied Code-2's split body there only.
   - Command: `CARGO_TARGET_DIR=…/transport-split-target cargo +1.95.0 test --locked --test mux_async`.
   - Result: 6 passed; 1 failed (`send_if_admits_and_queues_in_one_critical_section`, left `[None]`); exit 101.

3. **Core tests.**
   ```
   cd …/gwz-core && RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=…/core-target-r3 \
     cargo +1.95.0 test --locked --manifest-path …/scratchpad/wh1-rem3/cand/Cargo.toml --lib <filter>
   ```
   Results: `stale_action_tests::` 4 passed (the build took 2m36s); `native_publication_session_` 3 passed; `https_cancel_mux_tests::` 9 passed. Exit 0 each. The prepared candidate's manifest and lock are byte-identical to round 2's.

4. **Conditional-compilation guard.** `python3.13 -B gwz-core/scripts/checks/check_cfg_boundaries.py`: 1,390 files, nothing new.

5. **Formatting check.** `rustfmt +1.95.0 --edition 2024 --config skip_children=true --check` on the five round-3 Rust files: exit 0. Disclosed: this is read-only and outside the listed command set.

After the runs, the status of every member was unchanged.

## 1. Findings

None.

## 2. Invariant analysis

### Routing change: call graph against the contracts
- **Where the predicate applies.** The pump routes to HTTPS when any of these holds (`pump.rs:57–69`):
  - an Open with an HTTPS scheme;
  - HTTPS `owns` the `(request, stream)` key;
  - **new:** there is no SSH engine and the kind is neither Open nor CheckIdentity.

  The predicate is consulted only on endpoint sessions (`:70`), so driver sessions are unaffected.
- **Which sessions take the new clause.** `state.engine` is `None` only when `EndpointSettings.ssh` is `None`.
  - In production that is Windows: `endpoint_environment.rs:56–63`, and the engine is built only under `cfg(unix)` (`session.rs:390–415`). That is the qualification route.
  - Unix production always builds the SSH engine, or construction fails. So SSH-present sessions are unchanged.
- **Unchanged routing for other inputs:**
  - an owned key routes to HTTPS as before;
  - Opens still route by scheme;
  - a session with neither engine still reaches `InvalidRequest` (through the absent HTTPS engine instead of the absent SSH engine).
- **What can arrive as an action.** On the endpoint side, Open and CheckIdentity create routes. Follow-ups are Data, Window, Flush, Flushed, EndWrite, Failed, Close and Cancel. They are admitted only for a route that was live at admission (`routing.rs:34–100, 286–297`); frames for retired or unknown routes never become actions (`:86–91`).
- **Parity with the SSH engine.**
  - `HttpsEndpoint::accept` checks shutdown first, then returns `Ok` (no work) for an unknown non-Open key (`https_endpoint.rs:180–219`). This matches `PlacementEndpoint::accept` (`admission.rs:20–39`).
  - The pump's exclusion list `Open | CheckIdentity` is exactly the SSH engine's set of route-creating kinds.
  - Without the CheckIdentity exclusion, HTTPS `accept` would silently swallow a CheckIdentity. The exclusion keeps it fatal, and the control together with its mutation evidence shows the control discriminates.
- **No live-stream input becomes silent success.**
  - A live HTTPS stream is owned, so it routes exactly as before.
  - The only admitted inputs an HTTPS-only session cannot serve are CheckIdentity and non-HTTPS Opens, and both stay fatal.
  - This satisfies RemPlan-3's constraint.
- **No-work never strands a stream.** An entry is removed (`poll.rs:184–186`) only after it is retired, and `retired` is set in exactly four situations:
  - `take_outbound` takes a terminal for publication (`poll.rs:222–228`), so the terminal is in flight;
  - `fail_publication` converts the message in place, so the converted terminal is in flight;
  - `cancel_request` runs after `Session::cancel` seals the registration (`requests.rs:37–50`); the mux then owns the Cancelled terminal (`Mux::cancel`, `mod.rs:467–512`);
  - shutdown.

  So treating a later stale action as no work never leaves a live route without a terminal.
- **Locking and ownership.** The change is a pure predicate under the session lock. It adds no lock, callback, allocation or panic path, and the lock order from round 2 is untouched.

### Interface and visibility
- **No interface changes.** Transport `src/` is blob-identical, and the only core production change is the predicate.
- **Helper visibility.**
  - `small_limits` and `open_for` are `pub(super)` inside `https_cancel_mux_tests`, a `cfg(all(test, unix))` child of `https_endpoint`. They are reachable only from `https_endpoint` and its descendants; the sibling `stale_action_tests` uses them. Nothing is `pub(crate)`.
  - Both helpers predate the WH1 base (`c011aaee:https_cancel_mux_tests.rs:13,51`).
- **Module declaration.** `mod stale_action_tests;` sits inside the existing explicit `cfg_if` boundary and resolves to `https_endpoint/stale_action_tests.rs`, as `retry_tests` does. There is no `#[path]`.
- **New test file content.** It has no `cfg`, global, `unsafe` or `pub` item.
- **What the tests use.** Only existing test accessors and existing public transport API (`Mux::message`, `send`, `cancel`, `check_identity`).

### P3-1 test soundness
- `Probe` (`mux_async.rs:294–309`) registers as a waiter through one pending `next_message()` poll.
- `Shared::change` fires wakers only after it releases the mutex (`asynchronous.rs:33–35`). The probe's `wake` can therefore poll a fresh `next_message()` without deadlocking.
- **Committed body:** one critical section, one wake, and the Opened is observed: `[Some(..)]`.
- **Split body:** the admit-only acquisition takes and fires the probe's waker before the send. The probe sees nothing, and the send acquisition finds no waker to fire: exactly `[None]`.
- The test is deterministic in both directions, with no timing dependence. This matches the closure test Code-2 specified.

### Budget recount
From the bases (core `c011aaee`, CLI `6a9c0dac`, Python `e0c5af10`, transport `8a2ec7fc`) to the tuple:

| | Files | Added | Deleted |
|---|---|---|---|
| Cumulative, non-inventory | 55 | 2,438 | 279 |
| Cumulative, including the 3 inventories | 58 | 2,467 | 279 |
| Round 3 alone | 5 | 519 | 3 |

- Cumulative non-inventory files by member: core 40, CLI 7, Python 6, transport 2.
- Round 3 by file: pump 10/1, endpoint 1/0, new test 467/0, test helpers 2/2, transport test 39/0.
- Reconciliation: 1,919 + 519 = 2,438 added, and 276 + 3 = 279 deleted. All three deleted lines predate the WH1 base (`c011aaee:pump.rs:60`, `https_cancel_mux_tests.rs:13,51`).
- My recount is byte-identical to `cumulative-numstat-committed-v1.tsv`.
- Against the ceilings: exactly at the 55-file ceiling (not exceeding it) and 162 lines under the 2,600-line ceiling.

### Compatibility and Windows
- There is no wire, schema or API delta.
- The CI transport pin (`a24e70a`) is the pre-existing push-ordering obligation recorded in Code-2, unchanged.
- The routing change is exactly the Windows qualification production path, because the SSH engine is always absent there. It is unconditional code, compiled identically by the macOS candidate build. The new tests exercise the same session shape portably with `ssh: None`.

## 3. Risks and next action

1. **Native refresh before acceptance.** RemPlan-3 requires an exact-source native refresh at this tuple before acceptance: MSVC build, qualification runner, provisioned CLI and installed wheel, with refusals. The evidence states it is **not done** (`implementation-status.json` deviations). The cumulative count sits at the 55-file ceiling, so any Windows-only fix the refresh forces into a new file needs a new disposition. Any source change re-opens review of the tuple.
2. **Disclosed REDs and known failures.** Strict Clippy RED45 and the owner-IR pin mismatch are unchanged and not waived. The six candidate-leg failures are identical on the round-3 base.
3. **Next action.**
   - Merge with State's round-3 verdict on P2-4.
   - If the merged gate is GO, run and archive the native refresh.
   - Then file limited WH1 acceptance on that receipt set.
   - Ordinary Windows activation, WH2, WH3 and release remain separate NO-GO gates.
