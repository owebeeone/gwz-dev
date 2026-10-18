# Windows HTTPS WH1 round 2 — CODE-AXIS REVIEW

**Review object:** The WH1 remediation round-2 correction for State-1 P2-3, committed on 2026-10-04 and awaiting fresh review.
- gwz-core: `261eaca55dca4067548027e8976ff0249a34d2f3..f76cf4cc861f5697817ef43be784c6c88558b26e`, one commit.
- gwz-transport: `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578..966763e429c6c894da52e498b6f95ffd3d5ab5b5`, one commit.
- gwz-core-evidence: `053121cc97664e46539c07d77cdad4effb481955..56d93909781cc06626054d1210307f483efb72f8`.
- Controlling DRAFT: `dev-docs/GwzWindowsHttpsIntegrationImplementation-RemPlan-2.md` at lane root `1513c214fbc9bc492875a8244c7ed62ed4169a3a`.
- Lane: `/Volumes/projects/limbo/gwz-dev-wh1-rem2`.

**Baseline:**

| Repository | SHA (start = end) |
|---|---|
| root | `1513c214fbc9bc492875a8244c7ed62ed4169a3a` |
| gwz-core | `f76cf4cc861f5697817ef43be784c6c88558b26e` |
| gwz-transport | `966763e429c6c894da52e498b6f95ffd3d5ab5b5` |
| gwz-core-evidence | `56d93909781cc06626054d1210307f483efb72f8` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| git2-rs (build input, unchanged) | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |

- **How sources were read:** with `git show <sha>:<path>`, `git diff` by SHA and blob-hash comparison.
- **Tuple checks:** the tuple matched at the start and at the end of the review.
- **Untracked items:** only the excluded items were present: root `dev-docs/GwzRemoteTransportSshN2b-Prompt*.md`, `GwzWorkspaceRouteMappingDesign.md`, `candidate-target*/`, the gwz-core bug report and the evidence `alpha-setup-timeout/` run. None was read.
- **Tracked files:** every tracked file in every member was clean. The executed tests therefore ran on bytes equal to the tuple.

**Date:** 2026-10-04

**Axis:** Code. It covers interface contracts against call sites, call graphs, lock order, ownership and visibility, compatibility reality, scope and budget, and evidence attribution. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0, 0 P1, 0 P2, 1 P3.
- The P3 is non-blocking: a test-coverage gap.
- State-1 P2-3 is closed on the Code axis.
- Round 1's Code P2-1 and P2-2, and State P2-1 and P2-2, remain closed.
- No new architectural root cause.
- This verdict accepts the round-2 correction only. It does not accept limited WH1, ordinary Windows activation or release.

---

## Prior-finding closure table

| ID | Original counterexample | Re-traced on this tuple | Status |
|---|---|---|---|
| **State-1 P2-3**: final native check precedes the mux publication lock | 1. Preparation finishes before D.<br>2. `before_handoff` passes at D−ε.<br>3. The pump waits for `Shared.inner` across D.<br>4. `Mux::send` queues Opened.<br>5. `handed_off` clears the guard and enables native serving. | Steps 1–2 are unchanged. For step 3 onward:<br>• `pump.rs:190–205` builds a `PublicationCheck` (only for an Opened whose entry carries native D) and calls `Owner::send_if`.<br>• `asynchronous.rs:96–102` runs `admit` inside the same `Shared::change` closure, under `Shared.inner`, before `Mux::send`.<br>• `admit` re-reads the token and a fresh clock (`https_endpoint.rs:465–483`, `now >= D` expired) and refuses. The result is `Ok(false)`; `Mux::send` is not called.<br>• `pump.rs:213–221` then runs `refuse_publication` → `fail_publication` (`https_endpoint.rs:488–502`) and re-queues. The next iteration sends OpenFailed(Timeout) by plain `send`.<br>• `handed_off` (`pump.rs:207–211`) is reached only for an admitted, successful send.<br>Executed evidence:<br>• RED-v3 (base pump and transport plus the seam alone) publishes Opened in both late cases. I verified its input hashes against the git blobs.<br>• My own rerun of the three Session tests: 3/3 pass. | **Verified closed** |
| **Code-1 P2-1**: late native Open publication (delayed collection; backpressured handoff) | Collection at or after D, and pending Opened handed off after D. | `https_endpoint/poll.rs` and `retry.rs` are blob-identical to `261eaca`.<br>The pre-lock conversion is statement-identical; it was factored into `fail_publication` (compared against `261eaca:https_endpoint.rs:303–333`).<br>A WouldBlock re-queue repeats both the pre-lock and the in-lock check on every attempt.<br>Endpoint/mux 9/9 in `endpoint-mux-green-v3.log` includes both original regressions. | **Remains closed** |
| **Code-1 P2-2 / State-1 P2-2**: engine-free SSH-only constructor | Public and shared construction without HTTPS succeeded. | `transport_host/mod.rs` is blob-identical.<br>`session.rs` changes only by two accessors inside the existing `cfg(test)`/`cfg(unix)` `cfg_if`; the constructor bytes (`session.rs:371–380`) are unchanged.<br>`qualification-green-v2.log`: 4/4. | **Remains closed** |
| **State-1 P2-1**: HTTPS-only capacity install required SSH | First non-default capacity on an HTTPS-only session. | `session/capacity.rs` and `qualification_tests.rs` are blob-identical.<br>Paired constructor/capacity 16/16 and qualification 4/4 in the final-bytes logs. | **Remains closed** |

## Changed-range analysis

**gwz-transport `8a2ec7fc..966763e4`** (2 files, +213/−1):
- `src/mux/asynchronous.rs:77–103` adds exactly one item, `pub fn send_if(&self, request: &str, message: &Envelope, admit: impl FnOnce() -> bool) -> Result<bool, Error>`, with its rustdoc.
- Nothing else changed: `impl Clone for Owner` already existed, and there is no visibility change, re-export, `Cargo.toml`/`Cargo.lock` change or `protocol/` IR change. The IR digest is identical to `8a2ec7f`.
- `tests/mux_async.rs` adds the `forward`, `drain` and `opening` helpers, three tests, and wider imports of existing exports (`Attachment`, `Error`, `Port`). There is no production code under `tests/`.

**gwz-core `261eaca..f76cf4cc`** (4 files, +382/−28):
- `session/driver/pump.rs` (+31/−2):
  - The unchanged sealed check and the `before_handoff` call come first.
  - Then the pump takes a `publication_check` and branches: `Some` goes to `send_if` with an `admit` that records its refusal reason; `None` goes to the old plain `send`.
  - There is a new `Ok(Some(code))` arm.
  - The `WouldBlock`, terminal-only `InvalidRequest` tolerance and close-on-error arms are unchanged.
- `https_endpoint.rs` (+113/−26):
  - New `clock` field.
  - `before_handoff` checks the message kind first, then calls `publication_refusal` and `fail_publication`.
  - New `publication_check` and `refuse_publication`.
  - New private-field `pub(super)` `PublicationCheck`.
  - `Clock` is defined in one `cfg_if!`: the arm for `all(test, unix)` is scriptable; the `else` arm is a zero-sized type that reads `tokio::time::Instant::now()`.
- `session.rs` (+9): two test-only accessors, gated by `cfg(test)` and `cfg(unix)` through `cfg_if`.
- `https_cancel_mux_tests.rs` (+229): `session_publication_case` and three tests.

**gwz-core-evidence:** evidence files only. **Root `22c1bbd..1513c214`:** lock-pointer updates only.

All of these changes fall within RemPlan-2 steps 1–5. None of the following was added: an API beyond `send_if`, an owner, a dependency, a schema or wire change, a `#[path]` edge, a candidate-switch site, a process global, or a `#[cfg]` on an unbraced declaration.

**Root-cause candidates:**
- P3-1, below: test coverage only, **not architectural**.
- **NEW ARCHITECTURAL ROOT CAUSES: none.** The remedy completes the existing publication-arbitration root inside its synchronization boundary. It does not add a mechanism.

## 0. Evidence base

**Authority read:**
- RemPlan-2, ReviewState-1, ReviewCode-1, Verdict-1, RemPlan-1, BudgetDisposition (including the round-2 entry), Acceptance and the Handoff.
- The `CurrentProgramCheckpoint` round-2 entry.
- Integration design: the spike/gate table and the review-gate paragraph.
- Composition §4 (fresh time, not pre-lock time; equality expired) and §8.
- AgentProcessRules L1-09/L1-10 and GwzProcessOptimization §4.
- AGENTS for root, core and transport.

**Source read at the tuple:**
- Transport: `asynchronous.rs` (all), `mux/mod.rs:334–396` (`Mux::send`) and the README mux section.
- Core endpoint: `pump.rs` (all); `https_endpoint.rs` (all; for `before_handoff` also the `261eaca` version); `https_endpoint/poll.rs` (all); `retry.rs` (start and admit).
- Core session: `session.rs` (accessors and constructors); `session/passes.rs`, `close.rs` and `requests.rs`.
- Worker: `https_worker/prepare.rs:101–117` (D is anchored only for the WindowsConfigured/WindowsDefault policies), `https_worker.rs:166–170, 232–238` and the `native.rs` test helpers.
- Tests: the new core and transport tests.
- CI: gwz-core `.github/gwz-transport.commit` and `.github/workflows/transport-candidate.yml:55–65`.
- tokio-util 0.7.19 `tree_node.rs` for `is_cancelled` and `cancel` locking.

**Evidence inspected** (`portable-rem2/` and the README round-2 section):
- Records: `commands.json`, `implementation-status.json`.
- RED logs: RED-v1, v2 and v3.
  - RED-v3 input hashes match the git blobs: base `pump.rs`, base `asynchronous.rs`, final `session.rs`, final tests, and the `wh1-rem2-red-v3` snapshot.
  - The snapshot equals base `https_endpoint.rs` plus the seam alone (diffed).
- GREEN logs: v4 and v5, repeat20-v2, `endpoint-mux-green-v3` (9), qualification-v2 (4), cleanup-v2 (2).
- Mutation logs: one core, one transport.
- Transport logs: RED (compile), GREEN, Windows MSVC check, doc, package, clippy, fmt base/lane.
- Other logs: `candidate-failures-at-base-v1.log`, cfg, inventory, process globals, lane gate, owner-IR.
- Hash checks: the `changed-files.json` SHA-256 (`0eee5a96…`), `final-sources-v2.sha256` and `sha256-before.txt` all match the recomputed blob hashes. The before-snapshots match the base blobs.

**Evidence attribution note (no finding):**
- Both mutation logs ran on pre-rustfmt test bytes; formatting does not change semantics.
  - The core pre-lock mutation fails at `https_cancel_mux_tests.rs:578` on the readings assertion, which is at line 587 in the final bytes.
  - The transport outside-lock mutation fails at `mux_async.rs:278`; the final line is 283.
- This matches their position in `commands.json`, between green-v2 and green-v3.
- The harness notes say which RED and GREEN runs used earlier forms, but not the mutation runs.

**Executed by this reviewer** (external target directories, `CARGO_INCREMENTAL=0`):

1. **Transport tests.**
   ```
   cd …/gwz-dev-wh1-rem2/gwz-transport && CARGO_INCREMENTAL=0 \
     CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-code/transport-target \
     cargo +1.95.0 test --locked --test mux_async
   ```
   Result: 6 passed (including all three `send_if_*` tests), exit 0.

2. **Core Session tests.**
   ```
   cd …/gwz-dev-wh1-rem2/gwz-core && CARGO_INCREMENTAL=0 RUSTFLAGS='--cfg gwz_transport_candidate' \
     CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/wh1-rem2-review-code/core-target \
     cargo +1.95.0 test --locked \
     --manifest-path /private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/wh1-rem2/cand/Cargo.toml \
     --lib native_publication_session_
   ```
   Result: 3 passed, 2,828 filtered out, exit 0. The build took 1m56s; the tests ran in 0.35s.
   - The prepared candidate links to the lane's `src`, `build.rs` and `tests`.
   - Its build script found no git repository, so it ran no git commands.
   - `--locked` wrote no lockfile.

3. **Conditional-compilation guard.** `python3.13 -B gwz-core/scripts/checks/check_cfg_boundaries.py`: 1,389 files, 433 listed occurrences, nothing new.

4. **Formatting check.** `rustfmt +1.95.0 --edition 2024 --config skip_children=true --check` on the six round-2 files: exit 0. Disclosed: this read-only check is outside the listed command set and wrote nothing.

5. **Lane state after the runs.** Member status was unchanged afterwards.

No other reviewer's material was accessed.

## 1. Findings

### [P3-1] No test proves that `admit`'s answer and the queueing happen in one critical section

**Location:**
- gwz-transport `tests/mux_async.rs:259–291` (`send_if_admits_once_under_the_mux_lock`), against the contract at `src/mux/asynchronous.rs:79–81`: "its answer and the queueing it allows are one step for every other user of this mux".
- Likewise gwz-core `https_cancel_mux_tests.rs:438–657`.

**Violated invariant:** The regression suite must fail if a publication decision and the queueing it authorizes run under separate acquisitions of the mux lock. That atomicity is what closes State P2-3.

**Reproduction (mutation, derived by inspection):** Replace the body of `send_if` with:
```rust
if !self.0.change(|_| admit()) { return Ok(false); }
self.0.change(|m| m.send(request, message)).map(|()| true)
```

Every test still passes:
- **Refusal test:** `Ok(false)`, one call, nothing queued, route still Opening.
- **Equality test:** results, drained queues and phase are identical in all four cases.
- **Lock test:** `admit` runs under the first acquisition with the contender blocked, returns true, and the second acquisition queues the Opened.
- **Session tests:** the readings are `[false, true]`, because the in-lock reading is taken under the first acquisition. Late cases refuse and the control publishes.

Yet between the two acquisitions the pump can be suspended across D and queue Opened. That is State P2-3 again.

**Impact:**
- There is no live defect: the committed body is one `Shared::change` closure (`asynchronous.rs:96–102`).
- But the atomicity that closes P2-3 rests on code inspection alone.
- A plausible refactor would reintroduce the gap with every test green.

**Required correction:** Add one deterministic transport test:
1. Before calling `send_if(.., || true)`, register a waiter by polling `cli_port.next_message()` once with a probing waker.
2. `Shared::change` wakes waiters only after it releases the mutex (`asynchronous.rs:33–35`). So the probe's `wake` can poll a fresh `next_message()` with a no-op waker and record whether the Opened is already queued.
3. With one critical section, the probe must observe the Opened.
4. A split body's only wake to the probe follows the admit-only section, and observes nothing.

**Closure/regression test:** That test passing on the committed body, plus a recorded mutation run in which the split body fails it.

## 2. Invariant analysis

### `Owner::send_if`
- **Runs once, under the lock:** `admit` runs exactly once, inside `Shared::change`'s closure, while the guard on `Shared.inner` is held (`:24–25`), and immediately before `Mux::send`.
- **Refusal:** returns `Ok(false)` without calling `Mux::send`, so nothing is queued and no route transitions.
- **Admission:** the result is `m.send(..).map(|()| true)`, which equals `send`'s result and effects by construction. `send_if_admitted_equals_send` confirms this for four cases.
- **Wakers:**
  - Waker semantics are those of every `change`: wakes are collected under the lock and fired after its release. No wake runs under the lock.
  - A refusal still wakes all registered waiters, as `phase()` does. This is spurious but harmless under the re-checking `wait()` loop.
- **Panics:** if `admit` panics, it unwinds before `send`. The poisoned mutex is recovered by the existing `unwrap_or_else(into_inner)`, and un-taken wakers remain registered.
- **Neutrality:** the transport stays neutral, with no time, authentication or policy.
- **Doc contract:** the doc (short `admit`, no callback into the owner or port, lock order "mux mutex, then whatever admit reads") is accurate and sufficient for the only call site. A re-entrant call may panic rather than deadlock under std's `Mutex`; the prohibition is what matters.
- **Users:**
  - `Owner` is used by gwz-core's Session (driver and endpoint roles) and by the transport tests.
  - gwz-cli and gwz-py do not use it.
  - The addition is purely additive and bypasses no validation, since it routes through `Mux::send`.

### Core pump
- **Which messages use `send_if`:** only an Opened whose entry exists and carries `publication_deadline`, the native D. D is set only for WindowsConfigured/WindowsDefault (`prepare.rs:101–117`).
  - Non-Opened messages, nonnative Opened and SSH traffic get `None` and keep plain `send`.
  - `(request, stream_id)` keys are unique per mux, so SSH messages cannot alias an HTTPS entry.
- **Admission:** `handed_off` runs only on `Ok(None)`, that is, after an admitted, successful send. Native advertisement serving starts only through `handoff = true` set there (`poll.rs:157–175`).
- **Refusal path:**
  1. The refusal converts the message in place through `refuse_publication` → `fail_publication`.
  2. It re-queues the message into `state.pending` with `moved = true`.
  3. The next iteration repeats the sealed check. `before_handoff` returns early for a non-Opened message, and `publication_check` returns `None`.
  4. The OpenFailed then takes the plain path with its terminal-only `InvalidRequest` tolerance and `WouldBlock` retry.
  - The entry cannot vanish between `publication_check` and `refuse_publication`: the session state lock is held for the whole pass, and `admit` touches only the cloned check.
- **Lock order:** session state → mux → token node (`is_cancelled` takes a brief tokio-util mutex) and the clock. No path takes these locks in reverse order:
  - mux wakes fire after release;
  - the pass waker only unparks;
  - all cancellers of the entry token run under the session lock;
  - tokio-util calls no user code while holding a node lock that `admit` could need.

### Other checks
- **`fail_publication`:** statement-for-statement identical to round 1's inline conversion: facts from the Opened, `failed_open`, cancel, revoke the route, clear output and prepared, handoff false, retired, disconnect the peer. The pre-lock path's decision order (cancellation, then `now >= D`) and its production clock are unchanged.
- **Clock seam:**
  - Production `Clock::now()` is exactly `tokio::time::Instant::now()`, through a zero-sized type with no allocation.
  - The scripted form exists only in the `all(test, unix)` arm, as a per-endpoint field: no global, no thread-local.
  - `poll.rs`'s collection-time check still reads the real clock. This is within RemPlan-2, whose seam is defined over the pre-lock and in-lock readings, and production reads one monotonic domain throughout. The test keeps real D about 10 s away (`connect_ms: 10_000`) and asserts real now < D before collection, so the mixed-domain model cannot produce a false result.
- **In-lock cancellation:** the in-lock cancellation read is untested but currently unreachable cross-thread. It is defense in depth, not a defect.

### Tests
- **Real publication path:**
  - The three Session tests drive `Session::endpoint_with_https_native`'s own pass thread, Owner and TransportPort with the real TLS server and native fixture. The initiator Mux is only the peer.
  - The tests replace the collected attempt atomically under the session lock before installing the script.
- **"Mutex held" probe:** the probe calls `other.phase()` on the session's own mux from another thread with a 100 ms bound. It is sound:
  - `held = true` requires the mux mutex to be held for the whole bounded wait, and the reading thread inside `admit` is the only long holder;
  - the pre-lock mutation yields `[false, false]`.
- **No false GREEN:**
  - Exact `[false, true]` in the late cases rules out a collection-time or pre-lock refusal.
  - The late cases assert one OpenFailed(Timeout) with `Effect::None`, authenticated facts, a revoked route, the charge held until the real `Connection` is dropped and then released, and nothing further published.
  - The control is pre-D success; the equality case is exactly D.
- **Transport tests:** they cover refusal, equality and "admit under the lock", subject to P3-1.

### Scope and budget
**My recount from the bases** (core `c011aaee`, CLI `6a9c0dac`, Python `e0c5af10`, transport `8a2ec7fc`):

| | Files | Added | Deleted |
|---|---|---|---|
| Cumulative, non-inventory | 54 | 1,919 | 276 |
| Switch inventories | 3 | — | — |
| Round 2 alone | 6 | 595 | 29 |

- Cumulative by member: core 40, CLI 8, Python 7, transport 2.
- The file list and per-file numstat are byte-identical to `cumulative-numstat-v2.tsv`.
- Reconciliation:
  - added: 1,350 (round 1) + 595 − 26 (round-1-added `before_handoff` lines rewritten) = 1,919;
  - deleted: 273 + 29 − 26 = 276.
- Both are within the ceilings of 55 files and 2,600 lines.

### Compatibility reality
- `.github/gwz-transport.commit` (`a24e70a`) is a **pre-existing push-ordering obligation, not a defect of this patch**.
- WH1's own base core `c011aaee` already uses transport `Failure.detail` (for example `c011aaee:src/git/endpoint/https_worker.rs:249`). That field first exists in transport `9f9f0dc`.
- Neither `9f9f0dc`, `8a2ec7f` nor `966763e` is on transport `origin/main` (`3547597` as last fetched).
- The transport-candidate CI job builds against the pinned checkout.
- Round 2 extends that obligation to `966763e` but does not create it.

### Windows
- The only platform branch in the changed production code is the `Clock` `cfg_if`. Its non-`all(test, unix)` arm is the arm every non-test build compiles, including the macOS candidate library built for integration tests by `check --tests` and `run_tests.py`.
- A Windows qualification build compiles that same arm and the same unconditional pump and endpoint code against a transport method that type-checks for x86_64-pc-windows-msvc.
- P2-3 is a platform-neutral lock-order defect, exercised through the real Session/Owner path. **Closing P2-3 needs no native compile or run.**

## 3. Risks and next action

1. **Native evidence for WH1 acceptance.** The design's build-closure row requires an exact-source Windows MSVC build.
   - The last native compile, the native qualification runner and the installed CLI/wheel hashes (`880f1acf…`, `3b8e0842…`) are of round-1 sources.
   - Native authenticated Opened publication now goes through `send_if`.
   - Before filing limited WH1 acceptance on this tuple, the lane owner should either refresh the exact-source native compile (library plus unit-test binary, ideally the five-test qualification runner) or record why the round-1 native evidence stands.
   - This is not a defect of the round-2 patch, which claims portable evidence only.
2. **CI pin bump.** When the lane is pushed, transport `966763e` must reach transport `main` before core. The pin must move to a commit containing it, in the same core commit as the IR pins. The disclosed owner-IR RED blocks that today.
3. **Disclosed REDs.** Strict Clippy (45, with unchanged per-file, per-lint counts) and the owner-IR pin mismatch remain unchanged by this patch and unwaived.
   - The six candidate-leg failures are shown identical on the base tree in `candidate-failures-at-base-v1.log`: the two SSPI validator target-dir tests, `corpus_byte_parity`, and three Python embedding tests (`windows_configured`). They are not re-litigated.
4. **Next action.**
   - Merge with the parallel State-axis report.
   - Optionally close P3-1 with the deterministic atomicity test and its mutation record. It is non-blocking.
   - Resolve item 1 before limited WH1 acceptance.
   - Ordinary Windows activation, WH2, WH3 and release remain separate NO-GO gates.
