# Phase 1 (step 1.8) — State-AXIS PHASE REVIEW

**Review object:** the Phase 1 slice at root bcfa8ac7 / gwz-core a7b965c5 / gwz-transport 0a253639 / evidence c30d49b3

**Baseline:** Sources were read in place, read-only. Probes ran in `rsync -aL` copies under `/Volumes/projects/limbo/build-targets/review-1.8-state-*`, each with its own target dir. All copies and target dirs were deleted at the end.

**Date:** 2026-10-31

**Axis:** State (durable-state semantics and adversity). The review is independent, adversarial and read-only. The lane owner files it verbatim.

**Verdict: WITHHELD — the tuple moved during the review.** The brief says that if the tuple moves, the review reports the discrepancy instead of a verdict. This report therefore issues no GO or NO-GO.

**The discrepancy:**
- At the start, root was `bcfa8ac7`. At the end it was `888de752`, a descendant.
- The one new commit, "Draft the HTTPS authentication model … before plan step 3.1", adds only two files:
  - `dev-docs/GwzHttpsAuthModel-DRAFT.md`;
  - `gwz.conf/markers/01a18f00-….yaml`.
- The members did not move: gwz-core `a7b965c5`, gwz-transport `0a253639`, gwz-core-evidence `c30d49b3` (its only untracked entry is the inherited `runs/2026-09-22-alpha-setup-timeout/`).
- No controlling document changed between the two roots: the contract, the plan and AgentProcessRules are byte-identical.

**What the evidence shows on the unchanged object:**
- Open findings: **1 P2, 2 P3.**
- Under the severity contract, any open P2 means NO-GO. A re-issue on this object therefore could not be GO until P2-1 closes.
- **Pre-commitment:** I would give GO on a revision that closes P2-1 with the closure rows named below and that dispositions P3-1 and P3-2, either fixed or recorded with an owner.

## 0. Evidence base

**Tuple checks.**
- At start: root `bcfa8ac7`, gwz-core `a7b965c5`, gwz-transport `0a253639`, evidence `c30d49b3`.
- At end: the members were unchanged; root was `888de752` (see the verdict line).

**The crate alone.**
- Command: `CARGO_TARGET_DIR=…/review-1.8-state-crate cargo +1.95.0 test --locked -p gwz-connection`, in a copy of gwz-transport `0a253639`.
- Result: **189 passed, 0 failed.** This matches `2026-10-31-p1c-crate/raw/green-crate.log`.

**Crate probes.** One test file, `probe_tests.rs`, was added in the copy only, on the crate's own fakes (`Rig`, `ManualClock`, `ManualProgress`):
- **PROBE-ALLOC.** An operation with `OperationPolicy::new(4, 1, 3)` (per-host limit 1) holds exchange E1 and opens E2, which waits for capacity. The clock is advanced 30,001 ms and the executor is run.
  - Driver not yet run: E2 was decided at 31 s.
  - Driver run once before E2's open: E2 was **undecided at 31 s and still undecided at 631 s.**
- **PROBE-PROOF.** This is the existing row `p1_int_rem_the_openings_deadline_during_a_held_proof_ends_it_with_an_allocation_timeout` with one change: `rig.progress.run()` is called after A's connection goes idle and before B opens. B's opening was **undecided at its 30 s deadline**. It was decided only at the driver's stale 60 s idle wake, 90 s after it started.
- **PROBE-IDLE-ASYNC.** The fake holds the exchange end (`hold_finish`) and confirms it through the waker, as a real adapter does. After 60,001 ms: idle=0, closes=1. Expiry works on this path.

**gwz-core probes on a real adapter path.**
- Setup:
  - candidate shape: `TMPDIR=/Volumes/gwztmp RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --locked --lib review18_probe`;
  - a copy of gwz-core `a7b965c5` with its path siblings;
  - two rows appended to `connection_host/https_tests.rs`, using its `Case`: the real `HttpsAdapter` on the real `ConnectionHost` and `DriverThread`, with the fixture HTTPS server.
- **REVIEW18-IDLE.** Exchange finished `Reusable`, giving `Counts{idle:1}`. After a real 62 s sleep: `Counts{idle:0, leased:0}`. The HTTPS adapter's idle connection **does** expire.
- **REVIEW18-ALLOC.**
  - Setup: an operation with per-host limit 1 holds E1 (leased); after 1 s the second open's wait is polled for 45 s.
  - Result: `decided=false after=45.02s counts={leased:1}`. The contract's 30 s allocation bound is **not** enforced on a real adapter path.

**Spawn-gate probe.**
- Setup:
  - a row appended to `src/git/spawn_gate/tests.rs` in the copy, using `SpawnGate::with_exclusion(true)`;
  - N launcher threads each loop: `launching()`, poll `try_hold` every 1 ms, hold 3 ms, release, sleep `gap`;
  - one thread measures blocking `hold()` waits.
- Results:

| Launchers | Gap | Blocking holds | Median wait | Max wait |
|---|---|---|---|---|
| 2 | 30 ms | 40 | 375 ns | 8.2 ms |
| 4 | 30 ms | 40 | 416 ns | 9.1 ms |
| 4 | 10 ms | 40 | 18.3 ms | 137 ms |
| 8 | 20 ms | 3 in 4 s | **2.44 s** | **3.71 s** |
| 8 | 2 ms | the first blocking hold was **never admitted in about 9 minutes** (process killed) | | |

**Source checks** (gwz-core, python3.13), all exit 0 with "nothing new": `check_windows_parity.py`, `check_cfg_boundaries.py`, `check_process_globals.py`, `check_spawn_gate_minting.py`.

**Evidence read, verified against sources:**
- The readiness inventory, `build-targets/tools/p1-readiness-2026-10-31.md`.
- READMEs and raw logs of `2026-10-31-p1c-{crate,fair,https}`, including `finding-idle-expiry-repro.{rs,log}`: SINGLE variant 0 idle=0 closes=1; variants 1 and 2 idle=1 closes=0; MIN idle=2 closes=0.
- Receipts:
  - `p1c-fair-20261031-a`: gwz-core `f1d8c5a0` plus transport `0a253639`; hosted-like 4347 passed, 3 named gwz-py gaps; PASS.
  - `p1c-https-20261031-a` (`de03168e`) and `-b` (`68db19ae`): PASS, 3 gaps.
  - WSL `p1c-crate-remote-20261031-a`: gwz-core `34cdf51a` plus transport `0a253639`; OVERALL PASS.

**CI cited only as the owner gave it:** gwz-transport `0a253639` "Transport contracts" success, including "gwz-connection (ubuntu-24.04 / macos-latest / windows-2022)". The owner also reported win-flakes-20261031-d as PASS. The readiness inventory puts that receipt at gwz-core `0f5ec3e0`, which is not a descendant of the closeout commits.

## Problem, requirement provenance, and scope

**Requested outcome.**
- Plan §1, the operator's 2026-10-24 goal: connections are reused per host across repositories and phases, and owned by a client-side owner.
- Phase 1: "the connection owner, conforming to the contract".
- The slice delivers an owner (`gwz-connection`) with admission, per-operation bindings, profile partition, cross-binding proofs, per-binding limits, a driver-only builder and real SSH/HTTPS adapters.

**Authority for the 2026-10-31 closeout work.**
- p1c-crate's §13.2 rows and crate CI, p1c-https's C2(b), and p1c-fair's probe trace to plan 1.8's text and its carried-by-name list.
- The 4 s admission default traces to D-P.
- I found no scope expansion beyond these. The spawn-gate change (`SpawnGate::launching`, kernel entry behind the in-process lock) is the remedy for the closeout's hazard 3, which plan 1.8 names. Its necessity is demonstrated by the red probe (`raw/red-old-gate-runs.log`, 10 of 10 red).

**Out of scope, per the brief:** Phase 2 `remote/`, the 2.2c commits (except the Phase 1 invariants they touched; see §3), the Phase 5 delegations, CI/runner commits, and the operator drafts.

## 1. Exit and acceptance

**R1 (owner row: two operations, one fake connection).** Met with fakes.
- `operation_tests.rs` `finish_a_then_b_reuses_the_idle_connection` asserts `connects()==1`, and since `0a253639` both exchanges are on one fake connection.
- It ran green in my crate run on the Mac, and in the owner-reported CI on three OSes.
- The real-adapter equivalents are `connection_host/tests.rs:347` (SSH) and `https_tests.rs:309` (HTTPS), green in the Windows receipts.

**R3 (A and B overlap, cancel A, B completes, C reuses).** Met with fakes, partially on real adapters.
- Fakes: `operation_tests.rs:96` `r3_a_and_b_overlap_a_is_cancelled_b_completes_and_c_reuses_the_idle_connection`, made red by mutation M6. Green on the Mac (my run) and in CI on three OSes.
- Real adapters: no single A/B/cancel/C row. The legs exist separately:
  - `cancel_during_connect_ends_the_opening_and_disposes_of_the_setup`;
  - `a_binding_fault_closes_only_that_binding`;
  - `finish_a_then_b_reuses_…` (SSH and HTTPS).
- **Gap:** the report may claim R3 at owner level with fakes on three platforms, plus the real-adapter legs. It may not claim a real-adapter R3 row.

**R4 (changed identity, configuration or helper environment never reuses; no stale event).**
- Fakes: `host_tests.rs:593` `r4_…_no_stale_event_crosses`, red under M7 and M8.
- Core, real capture with a recording adapter: `endpoint_environment_tests/profile_partition.rs:159`.
- Real HTTPS adapter: `https_auth_tests.rs`, the changed `GH_TOKEN`, changed `GIT_CONFIG_GLOBAL` helper, no-helper and equal-context rows.
- Windows: `p1c-https-20261031-a` and `-b` PASS.
- **Gap:** Linux evidence for the C2(b) rows is missing. The only WSL matrix given, `p1c-crate-remote-20261031-a`, is at gwz-core `34cdf51a`, which predates `dabe7dc6` and `68db19ae`. No Linux run of the merged `a7b965c5` was given.

**R6 (10,000 operations; bindings never exhaust a host-wide bound; memory, tasks and native work bounded).** Met with fakes, at owner level.
- `sustained_tests.rs` passed green in my run. Evidence `green-sustained-nocapture.log`: 10,000 exchanges, 29,994 stale events with no table change, peak bindings 5, peak physical connections 8, retained charge 0, retired bindings dead.
- Mutations M1, M2, M4 and M5 turn it red.
- **Gaps:**
  1. The sustained rows were deliberately weakened to assert that idle records are "bounded" rather than "expired", because of the idle-expiry finding. Idle expiry, a §10 timer, is therefore not proven in R6. See P2-1.
  2. No real-adapter sustained row exists, so "native work bounded" is not shown with real adapters.

**Plan 1.8 list, row by row:**
- Portable §13.1–2 rows in the crate alone: present, except §13.2's "actual sequenced profile conformance" and seeded Monte Carlo of the owner. Only the pre-Phase-1 kernel suites exist (`tests/sequenced*.rs`, `monte_carlo.rs`), and the §6.1 seam is still not on the stream-data path (§15, 1.3b row).
- Fixture rows of 1.3 and 1.6: present (`connection_host/{tests,https_tests,https_auth_tests}.rs`, `ssh_tests/reuse_auth.rs`); Windows PASS receipts.
- Three-platform hosted-like receipt:
  - Windows: per-lane receipts PASS.
  - Mac: lane logs plus my focused runs.
  - Linux: only at `34cdf51a` for gwz-core.
  - No receipt at the merged `a7b965c5`. The fairness and C2(b) lanes changed disjoint files and neither changed a Windows-only file the other did, so under the lanes' own rule no merge receipt is required. The report must say this explicitly.
- Spawn-gate fairness probe: built, red and green on the Mac, Windows receipt `p1c-fair-20261031-a` PASS. Covers the clones of one gate only.

**Carried by name:**
- R4 fixture and hosted-like rows: present (above).
- C2(b) rows: present (`https_auth_tests.rs`).
- Fairness probe: present.

**Exit as written** ("R3, R4, R6 pass with fakes and on the hosted-like suite on three platforms; the driver-only builder exists"):
- Fakes on three platforms: met, per the CI evidence the owner gave.
- Driver-only builder: exists (`transport_host::DriverOnlyBuilder`, §15 1.7 row).
- Hosted-like suite on three platforms: met for Windows and Mac. For Linux, met only at the pre-closeout `34cdf51a`.
- Contract conformance of the timers the Phase is meant to deliver: not met (P2-1).

**The five judged items:**
1. **The idle-expiry finding.** The mechanism is real and is broader than idle expiry. It reaches a real adapter through the allocation deadline (REVIEW18-ALLOC: 45 s and undecided, against a 30 s bound).
   - On the HTTPS adapter the idle-expiry symptom itself does not reach (REVIEW18-IDLE: expired at 62 s). That adapter's end confirmation always arrives through the waker the pump passed, which notifies the driver.
   - On the SSH adapter it reaches only in a narrow race: the worker settles `Reusable` between `worker.unpark()` and `settle.poll(cx)` in `ssh_adapter.rs:411-426`.
   - It violates contract §4.1 (Progress "executes … timers independently of application calls"), §10 (allocation 30 s; idle expiry 60 s), §7.1 ("a request's own deadline is never extended") and §11 ("cannot silently strand waiters forever").
   - It does not violate R6's boundedness: idle entries are capped by the pool ceilings. See P2-1.
2. **Cross-Client spawn-gate fairness.**
   - D-E decided the *safety* scope: one named kernel object per process, the claim scoped to gwz callers, failing closed. Fairness across `Shared` states was never decided.
   - The residual is fail-safe: a starved launch ends `GateTimeout`, a typed and bounded refusal, with no credential effect.
   - Within D-E's safety decision this is a limitation, not a defect, but it is unprobed on Windows. The Phase 1 report must state it: fairness holds among clones of one gate (one gwz-py Client, one CLI command). Across Clients, and against the connection host's own `process_safety()` gate (`transport_host/mod.rs:390`), order is the kernel's.
   - A new reverse-starvation hazard of the fix is P3-1.
3. **Native (SSPI) caller per instance.** A hypothesis, not reachable in Phase 1.
   - `CoreFactory` is built once per `TransportRuntime` (`transport_host/mod.rs:364-391`), so each host serves one captured caller.
   - The caller is not a profile field (`endpoint_environment.rs` FIELDS, 17 entries).
   - It becomes reachable when Phase 3.2 shares a host across equal-profile Clients. Plan 3.2's red row covers the helper environment only. It needs an owner (P3-2).
4. **The Exit:** as above. Not fully met: P2-1, and Linux evidence on the closeout commits.
5. **Record integrity:**
   - The §15 rows for 1.3b, 1.3c/C2(b), 1.4, 1.5, 1.6, the integration fix-up and 1.7 match the code I checked:
     - the 4 s default (`host.rs` `DEFAULT_ADMISSION_BOUND_MS`, contract §10 amended);
     - `HttpsInputs` holds no helper (`https_adapter.rs:90-96`);
     - `scope_https` is unused by the crate;
     - `ConnectCommand` carries no deadline;
     - `FactoryEnv::domain()` has no non-test caller in gwz-core.
   - **Gap:** several items those rows carry "to the Phase 1 review" have no disposition (P3-2).

## 2. Findings

### P2-1 — Caller-driven instance pumps change the pool's timers without re-arming the host's driver

**Root cause** (one, not architectural):
- The driver task (`pump.rs` `drive`) sleeps only on the earliest deadline it computed on its own last pass. It is woken only by `Signal::notify`, which only the adapter's waker calls (`instance.rs` `Wakes::wake_by_ref`), or by that sleep.
- Every caller-driven pump mutates the pool and the in-flight tables under the instance lock, then wakes only the openings' waiters (`wake_waiters`), never the driver:
  - `Instance::submit` (`instance.rs:~370-388`);
  - `settle` (`:~392-404`);
  - `release` (`:~405-411`);
  - `abandon` (`:~414-448`);
  - `exchange.rs:cancel_binding` (`:250-266`).
- A deadline these pumps create or move earlier is therefore never armed if the driver last parked with no deadline or a later one. Such deadlines are a waiting request's allocation deadline, an idle entry's expiry, and an authorizing opening's deadline.

**Violated invariants:**
- Contract §4.1: Progress executes the host's timers independently of application calls.
- §10: allocation 30 s, idle expiry 60 s.
- §7.1: a proof never extends the request's deadline.
- §11: "Progress termination … cannot silently strand waiters forever".
- D-R's text ("ends in `AllocationTimeout` at its original deadline") holds only while other adapter events keep waking the driver.

**Reproduction:**
1. *Fakes (PROBE-ALLOC):* `prepared` with `OperationPolicy::new(4,1,3)`; register A; open and hold E1; `rig.progress.run()`; open E2 (pending); `rig.clock.advance(30_001)`; `rig.progress.run()`. E2 `is_ready()==false`, still false after `advance(600_000)`.
   - Control: the same row without the first `progress.run()` decides at 31 s.
2. *Fakes (PROBE-PROOF):* `idle_after_a`, then `rig.progress.run()`; `hold_reuse`; B opens (proof held); `advance(allocation)`; `run()`. B is undecided; it is decided only after `advance(60_000)`.
   - The existing rows `p1_int_rem_…held_proof…` and `…drivers_next_deadline…` pass only because the driver never ran before the opening. The second asserts `next_deadline()`, not that the driver sleeps on it.
3. *Real adapter (REVIEW18-ALLOC):* the same as (1) on `connection_host/https_tests.rs` `Case` (real `HttpsAdapter`, `ConnectionHost`, `DriverThread`). Undecided after 45.0 s, `leased:1`.
4. *Idle (p1c-crate repro):* idle=2 closes=0 after 60.001 s with the fake's synchronous finish. On real adapters only the SSH race named in §1 reaches it; HTTPS expired normally (REVIEW18-IDLE).

**Impact:**
- An opening waiting for its binding's per-host limit (`--max-per-host`), for the instance ceilings, or behind a scope saturation (D-R) is stranded on a quiet host. It is stranded until an unrelated adapter event or the release of the lease it waits behind, which can be a long fetch's whole duration.
- Its typed `AllocationTimeout` arrives late or not at all.
- A held proof's opening ends at a stale later wake.
- Idle connections can outlive 60 s on SSH.
- Fail direction: progress is lost and nothing is invented. No stale reuse occurs, because `submit` calls `pool.advance(now)` before matching (`instance.rs` submit), so an overdue idle entry is never offered.
- No production consumer exists until Phase 3, which is why this is P2 and not P1.

**Required correction:** after every caller-driven pump, notify the host's driver: `self.wakes.driver.notify()`, or notify when `next_deadline()` moved earlier. This applies at the end of `submit`, `settle`, `release`, `abandon` and `cancel_binding`.

**Closure tests:**
- PROBE-ALLOC and PROBE-PROOF as standing crate rows, each with the driver parked first.
- The p1c-crate idle repro as a row asserting expiry.
- The sustained rows restored to assert idle expiry, not just boundedness.
- A gwz-core real-adapter row: an HTTPS per-host-1 second opening ends `AllocationTimeout` within the bound plus slack.

### P3-1 — A registered launch has unbounded priority over blocking spawn-gate holds (reverse starvation)

**Root cause:** `SpawnGate::hold` (`src/git/spawn_gate.rs`, `while state.held || state.launches > 0`) defers to any registered `Launching` with no bound. Launches register before their first `try_hold` (`https_auth/runner.rs:195`) and poll at 1 ms. Under a sustained stream of overlapping launches, `launches` is rarely 0 when the gate is released, and a blocking `hold()`, which has no clock, waits for as long as the stream lasts.

**Violated invariant:** the fix's own claim that blocking holds are only deferred while a launch waits. The brief's requirement that the registration "never … wedge holds".

**Reproduction:** the §0 probe. Moderate duty cycles are fine. At gate saturation (8 launchers, 3 ms hold, 20 ms gap): median blocking wait 2.44 s, max 3.71 s. At 8 launchers with a 2 ms gap, no blocking hold was admitted in about 9 minutes.

**Impact:** Windows only. Off Windows the process gate does not guard, and only `with_exclusion(true)` test gates do. It requires helper launches to saturate the gate, for example many concurrent per-exchange helper lookups (C2(b) made lookups per exchange). Production reachability is a **hypothesis**. The consequence is concrete in overload: a blocking spawn waits unboundedly for the burst to end. These are a pathspec log entry, the Git CLI route's commit, or libgit2's credential helper "held whole".

**Required correction:** bound the precedence. Options: a blocking hold that has waited past a bound (or behind K launches) is served next; or one FIFO ticket order across both kinds.

**Closure test:** the saturation probe asserts each blocking hold is admitted within a stated bound, while `gate_fairness_tests` still meets its 2 s and `2*TAKERS` bounds.

### P3-2 — Items the contract carries "to the Phase 1 review" have no owner or decision

**Root cause:** plan 1.8's carried-by-name list (plan :447-449) names only R4's rows, C2(b) and the fairness probe. It omits the open items that contract §15's 1.3b and 1.6 rows (and the p1c-https Code review §4) addressed to this review. None of these has a plan owner (grep of the plan: no step names them).

**The items, at HEAD:**
- (a) **No maximum connection age anywhere** (§10, "10 min checked at release"; §7.1; reuse design §12: it "bounds how long the server's decision at setup is relied on"). There is no code in `src/pool` or the crate. A connection kept warm by reuse is never retired.
- (b) `MessageStream` has only `write` and `read`. It lacks `write_all`, `flush`, `shutdown_write` and `progress()`, and `Exchange` lacks `response_head` (§4, §9).
- (c) The §6.1 ordered seam is not on the stream-data path.
- (d) Adapter setups do not draw on the `CleanupDomain`'s 64/64/8 budgets: `FactoryEnv::domain()` has no gwz-core caller.
- (e) The pool's `network_deadline` is not in `ConnectCommand` (`ports.rs:131-144`).
- (f) Two last-lessee records.
- (g) No certificate or security-key proof rows.
- (h) The native caller per instance (judged item 3) has no Phase 3.2 row.

**Violated rule:** AgentProcessRules and the operator's standing rule that discovered shortcomings are dispositioned, never parked silently. The §3 status rule says a Phase report states what is proven.

**Impact:** the Phase 1 report could imply §10/§9 conformance that is absent. Item (a) is a §10 timer missing outright.

**Required correction:** the Phase 1 report lists (a)–(h), each with an owner step or an operator deferral recorded in plan §10. Contract §10's maximum-age row gains a "not built; owner …" note.

**Closure test:** the plan names an owner or decision for each of (a)–(h).

## 3. Invariant analysis

**Registration and revocation (§5).**
- `LocalPermit::register` publishes under the host lock or returns a terminal `NeverRegistered`.
- `seal` revokes claims exactly once (`claim.released`) and marks bindings cancelled and closing in the same critical section.
- The 2.2c rework kept that path intact. It wraps the path in `Admission`/`AdmissionPermit` kinds, and `closing_code` maps carrier-only reasons to 3, which a host binding never receives.
- No new stuck state found.

**Slots, admission and retained charge.** I examined the sustained rows; mutations M4 and M5 prove the charge returns and no binding is retained. The admission default of 4 s is below the mux bootstrap's 5 s (pinned by the row and by M9).

**Openings, proofs and settling.**
- A lease is never handed to another scope (misfit release `Reusable`, bounded by `misfit_bound`).
- Proofs are matched on `(connection, ProofId)` and late verdicts are dropped (`reused`).
- `abandon` returns the authorizing lease unused.
- `fail_in_flight` aborts settling and reports `Discarded`.
- The fail direction holds: nothing pools on an unknown end, and no completion is fabricated.
- The one liveness hole is P2-1.

**Idle and expiry.** `pool.advance(now)` precedes every match, so an overdue entry is never reused (fail-closed). Expiry *execution* depends on driver arming (P2-1). Maximum age is absent (P3-2a).

**Close and cleanup domain.**
- `seal` stops the driver, takes the adapters and disposes them under the close job.
- At the bound it pends to the retainer, and the close job keeps the work if the retainer refuses.
- Narrow observation, not filed: when the executor refuses the close job *and* the retainer refuses (`host.rs` `seal`, `let (ticket, _kept) = self.pend(adapters)`), the refused work is dropped and the ticket stays `Pending` forever. That is truthful (never a fabricated `Complete`), but the work is unowned. It is a double fault at executor loss.

**Locks.**
- Host lock, then instance locks, are never nested in reverse in the paths read (`seal`, `snapshot`, `tables`, `route`, `retire_if_done`).
- Adapter calls run under the instance lock, which is acceptable only because the real adapters queue to workers. SSH `poll_finish` polls an in-process stream close, nonblocking.

**Spawn-gate registration.**
- `Launching` is RAII inside the async `hold_spawn_gate`. A drop, a cancellation or a panic decrements it, with `notify_all` at zero. No leak path was found.
- A kernel `acquire()` failure now yields a `Local` hold. Before the fix it yielded `Nothing`, so this is strictly more exclusion.
- Hazard: P3-1.

**Cross-binding reuse after an identity change.** R4 holds (profile partition, then scope partition, then proof). C2(b) is enforced: no instance helper, a POST pre-authenticated only on its own connection and path, and an anonymous challenged POST refused with a typed `Authentication` and no replay. Deferral (a) is honestly recorded (§15 1.3c row; plan 3.1 prerequisite) and fails closed.

**Repository rules in the slice.**
- `cfg_if` boundaries are used: the crate's only bare `#[cfg(test)]` is the pre-existing `src/pool/setup_clock.rs:307`, which is outside the slice.
- No new globals: `NEXT_POOL` is pre-existing, allowlisted debt (see §5).
- No production code under `tests/` was found in the closeout.
- Braces: no violation found.

## 4. Risks and next action

**Next action:** close P2-1 with the closure rows. Re-confirm on a tuple whose root does not move, or the lane owner accepts the root move as documentation-only and re-issues the review request.

**What the Phase 1 report may claim in the §3 Status column:**
- **R1:** "owner level (fakes) passed; Phase 3 pending." It must not mark R1 met.
- **R3:** "passed at owner level with fakes on three platforms (crate CI); real-adapter legs separately." Only after P2-1 closes, because R3's "B completes" depends on bounded waits.
- **R4:** "passed, fakes and fixtures, Windows and Mac; Linux at pre-C2(b) `34cdf51a` only." The report must name that gap or add a Linux run of `a7b965c5`.
- **R6:** "owner level with fakes; idle expiry not proven (P2-1); native work not measured."

**What the report must not claim:**
- §10 conformance: P2-1, and the missing maximum age (P3-2a).
- §9 stream-surface conformance (P3-2b).
- §13.2 sequenced-profile conformance or Monte Carlo of the owner.
- Fairness across Clients.

**Other risks:**
- The Windows effect of P3-1 under per-exchange helper load is untested.
- No receipt exists at the merged `a7b965c5`. That is acceptable under the lanes' rule, but the report must say so.

## 5. Out-of-scope observations

- **Pre-existing, outside the slice:** `gwz-transport/src/pool/machine.rs:7` `static NEXT_POOL` is a process-global counter that mints every pool's `ConnectionId`, `RequestId` and `LeaseId` namespace.
  - The Phase 1 owner's connection numbers therefore come from a global counter. Contract §4.1 and §6 say "no global ID counter".
  - It is allowlisted debt (`process_globals_allowlist_gwz_transport.json`, owner "CS7.2–CS7.6"). This is a limitation to state, not a new defect.
- **Documentation:** `host.rs` (`0a253639`) inserted `tables()` under the doc comment "The instances the driver drives.", which belonged to `instances()`. `instances()` lost its doc. It is cosmetic, but it is the same reassociation pattern the repo rule warns about for attributes.
- **Process:** the readiness inventory's settled tuple (root `d140d0db`, gwz-core `34cdf51a`) is older than this review's tuple. Its R4 and receipt rows should not be reused verbatim in the Phase 1 report.
