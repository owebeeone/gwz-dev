# Phase 1 (step 1.8) — Code-AXIS PHASE REVIEW
**Review object:** the Phase 1 slice at root bcfa8ac7 / gwz-core a7b965c5 / gwz-transport 0a253639 / evidence c30d49b3
**Baseline:** I read the member working trees, which matched the tuple and were clean at the start (evidence's only untracked path was the inherited `runs/2026-09-22-alpha-setup-timeout/`). I also used `git show`/`git diff`/`git log` on the slice ranges, and probes and mutations in `rsync -aL` copies under `/Volumes/projects/limbo/build-targets/review-1.8-code-*`. All copies and target dirs were deleted at the end.
**Date:** 2026-10-31
**Axis:** Code: architecture, interfaces, call graphs and compatibility reality. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NONE ISSUED: the tuple moved during the review.** At the end-of-review check, root was `888de752` instead of `bcfa8ac7`. The prompt says that when the tuple moves, the reviewer stops and reports the discrepancy instead of a verdict, so this report does that.
- **What moved:** `888de752` is a direct descendant of `bcfa8ac7`: "Draft the HTTPS authentication model … for review before plan step 3.1", committed 2026-10-31 09:49 +1100. It adds two files: `dev-docs/GwzHttpsAuthModel-DRAFT.md` and `gwz.conf/markers/01a18f00-….yaml`.
- **What did not move:**
  - gwz-core `a7b965c5`, gwz-transport `0a253639` and gwz-core-evidence `c30d49b3`.
  - `GwzConnectionApiContract.md`, `GwzTransportReleaseConvergencePlan.md` and `AgentProcessRules.md`; `git diff` between the two root commits shows no change to them.
- **Not read:** I did not read the new draft, which is deferral (a)'s design note.
- **Findings still stand:** every finding below is about unmoved member code or unchanged controlling text. The open count is **4 P2 and 6 P3**. The lane owner can re-pin the tuple to `888de752`, or confirm that the draft is outside the object, and then ask for the verdict.
- **Pre-commitment** (for whichever tuple is confirmed): I commit to GO on a revision that closes P2-1 and P2-3 by fix and closure row. P2-2 and P2-4 can close either by fix, or by a §15 amendment plus a plan record that names an owner step before 3.1. In that case the Phase 1 report must narrow its R4 and stream claims as §4 states.

## 0. Evidence base
- **Tuple checks.**
  - Start: root `bcfa8ac7`, gwz-core `a7b965c5`, gwz-transport `0a253639`, evidence `c30d49b3`.
  - End: root `888de752` (see the verdict line); every member unchanged.
- **Crate alone**, run in a copy: `CARGO_TARGET_DIR=…/review-1.8-code-crate cargo +1.95.0 test --locked -p gwz-connection`. Result: **189 passed, 0 failed** on macOS arm64. The load average was about 25–48.
- **Mutations** in the crate copy (each restored, and restoration checked with `diff -r` against the workspace):

  | Id | Mutation | Result |
  |---|---|---|
  | R3-M1 | `exchange.rs:631`: `Exchange::finish` ignores `cancelled` | **RED**: `r3_a_and_b_overlap_a_is_cancelled_b_completes_and_c_reuses_the_idle_connection` and `cancelling_a_leaves_bs_exchange_intact` (190 pass / 2 fail) |
  | R3-M2 | gwz-transport `src/pool/lifecycle.rs:124`: `cancel_operation` matches by `owner.operation` only, so cancellation is keyed by the external name | **GREEN** in the whole crate (192/192), including the R3 row and both sustained rows. Red only in root `tests/pool_owner_limits.rs` `releasing_one_owner_changes_neither_the_other_s_limit_nor_its_connections` |
  | R6-M1 | `instance.rs:153`: `prune_scope_tokens` returns at once, so scope tokens are never pruned | **GREEN** (192/192), including `no_host_wide_table_grows_with_the_number_of_operations` |

- **Probes** (my test module in the crate copy, against the fakes):
  - `probe_allocation_deadline_on_a_quiet_driver`. Setup: a binding limit of 1, the driver run once, a second opening waiting.
    - Not ready 30.001 s later, nor 630 s later.
    - It resolves only when the first exchange is released, as `Pool(AllocationTimeout)`.
    - Control: the same opening queued before the driver's first pass is ready at 30.001 s.
  - `probe_idle_expiry_with_deferred_finish_confirmation` (the real-adapter shape: `hold_finish`, ended later through the waker): before expiry idle=1 closes=0; after 60.001 s idle=0 closes=1.
  - `probe_unbound_instances_are_never_retired`:
    - After 16 profiles have each finished and gone idle-expired: instances=16, bindings=0, idle=0, closes=16.
    - The 17th profile's `acquire` then returns `InstanceLimit`.
- **gwz-core focused rows, candidate shape, macOS**, in a copy (gwz-core, gwz-transport, gwz-git, git2-rs, gwz-sspi):
  - Command: `TMPDIR=/Volumes/gwztmp RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --locked --lib -- connection_host profile_partition spawn_gate`.
  - Result: **63 passed, 0 failed** (3,999 filtered out):

    | Module | Rows |
    |---|---|
    | `connection_host::tests` | 19 |
    | `connection_host::https_tests` | 5 |
    | `connection_host::https_auth_tests` | 12 |
    | `profile_partition` | 4 |
    | `spawn_gate::tests` | 13 |
    | `spawn_gate_chain_tests` | 5 |
    | `gitbackend::spawn_gate_tests` | 5 |

- **Source checks** (`python3.13`, gwz-core), each reporting "nothing new" or ok:
  - `check_windows_parity`: 514 files.
  - `check_cfg_boundaries`: 1,730 files, covering gwz-core, gwz-cli and gwz-py.
  - `check_process_globals`: 17 allowlisted.
  - `check_spawn_gate_minting` and `check_driver_only_boundary`.
- **Sha audit.** Every gwz-transport and gwz-core sha named in contract §15 rows 1.3a–1.7, the closeout and the plan's Record resolves and is an ancestor of `0a253639` / `a7b965c5` (26 gwz-core, 14 gwz-transport). This includes `dc9de784`, `a681cf26` and `e0cb12f7`.
- **CI and receipts, cited as the lane owner gave them:**
  - gwz-transport `0a253639` "Transport contracts": success, including "gwz-connection (ubuntu-24.04 / macos-latest / windows-2022)".
  - Windows hosted-like PASS (3 named host gaps, the gwz-py message rows): `p1c-fair-20261031-a`, `p1c-https-20261031-a` and `-b`, `win-flakes-20261031-d`.
  - WSL matrix PASS on `p1c-crate-remote-20261031-a`.
- **Read:**
  - Contract §1–§16 in full, including every §15 row for 1.3a–1.7, the integration fix-up and 2.2c.
  - Plan §1, §3, §4 Phase 1 (with the Record block and 1.8), Phases 3–4, and §10 D-E, D-P and D-R.
  - The readiness inventory, and the `2026-10-31-p1c-{crate,fair,https}` READMEs with the idle-expiry repro.
  - The crate's `lib, ports, pump, host, instance, exchange, operation, admission, binding`, with fakes and tests.
  - The gwz-transport pool diff `a27817b..a22d43c`, and the 2.2c and closeout diffs on Phase 1 files.
  - In gwz-core: `connection_host.rs`, `connection_factory.rs`, `endpoint_environment.rs`, `ssh_adapter.rs` (+ worker), `https_adapter/{exchange,pipe}.rs`, `spawn_gate.rs`, `native/sspi.rs`, and gwz-py `client_host.rs`.

## Problem, requirement provenance, and scope
- **Requested outcome:**
  - Plan §1 (operator, 2026-10-24): connections reused per host across repositories, and Windows as a full peer.
  - Phase 1: "the connection owner, conforming to the contract" (contract §4–§14).
  - Exit: "R3, R4, R6 pass with fakes and on the hosted-like suite on three platforms; the driver-only builder exists".
- **Authority for this review:** the operator's 2026-10-31 go for 1.8.
- **Prerequisites added in the slice, and their authority:**
  - The 4 s admission default traces to D-P.
  - The spawn-gate `launching` registration traces to plan 1.8's named fairness probe and closeout §6.3.
  - C2(b) traces to the 1.3c obligation.
  - Each is authorized. I found no scope widening without authority.
- **Delivery in the slice:**
  - It delivers the owner, bindings, adapters, partitioning, reuse proofs, per-binding capacity and the driver-only builder.
  - It does not deliver the contract's §9 stream surface (P2-4), §10 instance retirement and timers (P2-1, P2-3), or a production factory that honours the profile (P2-2).
- **Reachability:** none of these four is reachable in Phase 1 production, because "nothing in production uses the host until Phase 3" (§15 1.3c row). All four sit on the path Phase 3 adopts, and R4/R6 are Phase 1 acceptance rows.
- **Classification of the open items:**
  - Confirmed defects: P2-1, P2-3 (both reproduced), P2-2 and P2-4 (code-read; each is a single unambiguous call site).
  - Limitations or hypotheses: P3-5 (cross-gate fairness, unconfirmed on Windows) and item (3) (folded into P2-2).
- **Deferrals honoured:** (a) to (d) are not re-found.
  - Deferral (a) fails closed. A 401 to a POST that was not pre-authenticated falls through `https_policy::classify` to a typed failure (`https_adapter/exchange.rs` `exchange`). It is pinned by `a_post_on_a_connection_nothing_authenticated_is_refused_after_one_anonymous_attempt`, which is green on the Mac in my run.

## 1. Exit and acceptance (R1 owner row, R3, R4, R6; the plan's 1.8 list; the five judged items) — row by row, evidence or gap

| Row | Evidence | Gap |
|---|---|---|
| R1 owner-level (fakes: two operations, one connection) | `operation_tests.rs:56` `finish_a_then_b_reuses_the_idle_connection`; green 189/189; crate CI on 3 platforms (as given) | None for the owner row. Real reuse is Phase 3. |
| R3 with fakes | `operation_tests.rs:96`; R3-M1 makes it red | It passes with name-keyed cancellation (R3-M2): **P3-1** |
| R3 on the hosted-like suite | Real-adapter legs only: `connection_host/tests.rs:476` and `https_tests.rs:376` (binding fault closes only that binding), `tests.rs:557` (cancel during connect), `tests.rs:347` and `https_tests.rs:309` (A then B reuse). Green on Mac (my run); Windows and WSL receipts as given. | No fixture row for the composite "A and B overlap, cancel A, B completes, C reuses". Under the strict reading of the Exit, this is a gap for the report to state, not a defect. |
| R4 with fakes | `host_tests.rs` `r4_a_changed_identity_configuration_or_helper_environment_reuses_nothing_and_no_stale_event_crosses` (M7/M8 red per the evidence README) | Partitioning is proven. |
| R4 on the hosted-like suite | Helper legs on the real HTTPS adapter: `https_auth_tests.rs:347,359,372,438`, green on Mac. HOME/agent/CA/proxy legs: `profile_partition.rs:159`, which uses a **test factory with a loopback adapter** (`profile_partition/host.rs:46-60`). | The production factory ignores the profile: **P2-2** |
| R6 with fakes | `sustained_tests.rs:369,449` (10,000 operations, two names, four scopes, one profile) | Blind to scope-token growth (R6-M1, **P3-1**) and to instance exhaustion (**P2-3**) |
| Driver-only builder exists | `transport_host/driver_only.rs` `DriverOnlyBuilder::new/with_transport_flag/build`; `check_driver_only_boundary: ok`; 1.7 GO | None |
| 1.8: 10,000 operations; no host-wide tombstones | As in R6 | P3-1, P2-3 |
| 1.8: fixture-server rows of 1.3 and 1.6 | 19 + 5 + 12 rows green on Mac (my run); Windows/WSL as given | None |
| 1.8: three-platform hosted-like receipt | Windows (given), WSL (given), Mac (my focused run) | None for the listed rows |
| 1.8: spawn-gate fairness probe | p1c-fair (Mac) and receipt `p1c-fair-20261031-a` (given); `spawn_gate*` rows green on Mac | Covers clones of one gate only: **P3-5** |
| 1.8 carried by name: R4 fixture and hosted-like rows | As in R4 | P2-2 |
| 1.8 carried by name: C2(b) rows | 12 `https_auth_tests` rows green on Mac; receipts `-a`/`-b` given | None |

**The five judged items.**
1. **Idle re-arm finding.**
   - The root cause is wider than the evidence states, and it does reach the real adapters through the allocation deadline (**P2-1**).
   - The idle-expiry manifestation reaches them only through a narrow race. My deferred-finish probe expires normally, and the SSH/HTTPS `poll_finish` first returns `Pending` with the driver's waker in the normal path.
   - It violates contract §10 (allocation 30 s; idle 60 s) and §4.1 ("timers independently of application calls"). It does not violate R6: all state stays bounded by the caps.
2. **Cross-Client gate fairness.**
   - Exclusion is within D-E as decided: one named kernel object per process.
   - D-E did not decide scheduling. The fairness registration lives in each `SpawnGate::process()` value's `Shared`, and `process_safety()` mints one value per `NativeHost` (per Client).
   - Across Clients the order is the kernel's. This is a liveness limitation, fail-closed through `GateTimeout`, unconfirmed on Windows, and recorded in no root document (**P3-5**).
3. **Native caller per adapter instance.**
   - The shape is confirmed: `HttpsInputs.native` (`https_adapter.rs:93`) is filled once per factory from the runtime (`mod.rs:375-383`).
   - It is unreachable in Phase 1, because there is one host per runtime.
   - It shares one root cause with P2-2 (the factory is configured per runtime, not per operation). The correction there must put the caller in the operation's `AuthContext`.
4. **1.8 Exit as written:**
   - The driver-only builder exists.
   - R3, R4 and R6 pass with fakes on three platforms (CI as given).
   - "On the hosted-like suite": R3 has legs only, R4's configuration legs run on a test factory (P2-2), and R6 has no real-adapter row. R6 with fakes also has the P2-3 and P3-1 blind spots.
5. **Record integrity.**
   - All cited shas resolve and are ancestors.
   - These claims hold in code:
     - 1.3a: name `Local\gwz-spawn-gate-<pid>` (`spawn_gate/kernel.rs:54`), the one minting site (`composition.rs:31`).
     - 1.3b: `ExchangeSpec::git` refusals (`operation.rs:304-322`).
     - 1.5: misfit release `Reusable` and resubmission under the original deadline, `abandon` by slot (`instance.rs` `openings`, `abandon`).
     - 1.6 and integration: verdict routing — `Revoked` asks fresh, `Unprovable` uses `release_unused` plus exclusion; `cancel_reuse` on abandon, seal and expiry; `NotConnected` treated as a lost lease.
     - 1.4: limits carried on every request.
     - Closeout: `DEFAULT_ADMISSION_BOUND_MS = 4_000`, with production using `HostLimits::default()` (`connection_host.rs:322`).
   - Gaps:
     - The 1.5 row's "the captured certificates are what the instance trusts" is true only by one-host-per-runtime (P2-2).
     - §15 is silent on P2-1, P2-3 and most of P2-4.
     - The closeout lanes are missing from the plan (P3-6).

## 2. Findings

**P2-1 — The host driver's timer is not re-armed by caller-driven pumps.**
- **Location:** `gwz-transport/crates/gwz-connection/src/instance.rs`:
  - `submit`, `settle`, `release` and `abandon` (`:349-451`), and `exchange.rs` `cancel_binding`, each call `Instance::pump` and then `wake_waiters` (`instance.rs:262-268`).
  - `wake_waiters` wakes only the application waiters, never `Signal::notify`.
  - The driver computes its sleep only inside its own pass (`pump.rs` `drive`).
- **Invariant:**
  - Contract §4.1: `Progress` executes the host's "timers independently of application calls".
  - §10: allocation 30 s and idle 60 s, and network timeout zero "never disables allocation … idle reaping".
- **Reproduction:** `probe_allocation_deadline_on_a_quiet_driver` (§0).
  - An opening that waits on its binding's limit, queued after the driver's first pass, is not failed at 30 s. It waits until something unrelated runs a pump (630 s+ in the probe).
  - It then ends `Pool(AllocationTimeout)`, not served.
  - The idle form is the evidence repro (`2026-10-31-p1c-crate/raw/finding-idle-expiry-repro.*`).
- **Reach to the real adapters:**
  - The allocation form needs no adapter event, so it reaches them whenever a host is used. The production `DriverThread` polls only woken tasks (`connection_host.rs` `run`).
  - The idle form reaches them only when the adapter's settlement ends between `poll_finish`'s notify and its `settle.poll` (`https_adapter/pipe.rs:201-211`; `ssh_adapter.rs:411-426`).
- **Impact:** with every connection leased longer than 30 s, or with stalled exchanges under a disabled network timeout, a queued opening's bound is unenforced and unbounded.
- **Correction:** each caller-driven entry point notifies the driver's `Signal` after its pump. `drive` itself must not, or the driver loops.
- **Closure test:** the allocation probe becomes a row (ready at 30.001 s with `Pool(AllocationTimeout)`); the p1c-crate repro becomes a row (idle=0, closes ≥1).

**P2-2 (ARCHITECTURAL) — The production `EndpointFactory` ignores `EndpointSpec`, so instances are configured from the runtime that built the host, not from the operation's profile.**
- **Location:** gwz-core `src/transport_host/connection_factory.rs:60-89`.
  - `create(&self, _spec, env)` clones `self.ssh`/`self.https`, which are built once per runtime in `mod.rs:356-385`.
  - `EndpointInputs` (`endpoint_environment.rs`, the struct after `endpoint_config_native`) has no production consumer, and `OperationCapture.profile` carries a `dead_code` allowance.
- **Invariant:**
  - Contract §4.1: the factory "creates endpoint adapters from validated immutable inputs".
  - §7: the profile is the instance's configuration, and the native caller is an original-entry, per-operation capture.
  - Plan R4 and 1.5.
- **State sequence:**
  1. One host is built from capture A.
  2. Operation B is admitted with a profile from capture B (other `HOME`, `SSH_AUTH_SOCK`, `SSL_CERT_FILE` or proxy).
  3. The registry correctly makes a second instance.
  4. `CoreFactory::create` gives it A's known_hosts/home, A's agent source, A's TLS trust and proxy, and A's `NativeCaller` (item 3).
- **Impact:**
  - No effect today, because there is one host per runtime.
  - Once a host serves more than one capture (plan 3.2's module-level owner), operation B authenticates and trusts with A's configuration. That is credential and trust confusion.
  - R4's configuration legs are proven only on a test factory whose adapter is a loopback (`profile_partition/host.rs:46-60`).
- **Correction:**
  - Build each adapter from `spec.profile.inputs()` (`EndpointInputs`), and carry the native caller in the operation's `AuthContext`.
  - Alternatively, record this as a named prerequisite of plan 3.1/3.2 in contract §15 and the plan, and narrow R4's Phase 1 claim to match.
- **Closure test:** a gwz-core row through `ConnectionHost` + `CoreFactory` with two captures differing in `SSL_CERT_FILE` against a fixture HTTPS server signed by CA B: B's exchange succeeds, and an instance with A's trust refuses. Add one more for `SSH_AUTH_SOCK`.

**P2-3 — Endpoint instances are never retired, so a long-lived host refuses every new profile after 16.**
- **Location:** `host.rs:472-484` (`attach_profile`).
  - It returns `AttachError::InstanceLimit` at `instances.len() + creating >= endpoint_instances`.
  - Nothing removes an instance before `seal`.
- **Invariant:**
  - Contract §10: "Endpoint instances | 16; coalesce equal profiles, LRU retire unbound".
  - §6: selection versus retirement is one registry transition.
  - R6: long-lived use is sustainable.
- **Reproduction:** `probe_unbound_instances_are_never_retired` (§0). With 16 instances, no bindings and no connections, the 17th profile gets `InstanceLimit`, permanently.
- **Impact:** R4's own scenario (changed HOME/agent/CA/proxy) exhausts a long-lived owner after 16 changes. Example: a Python service whose agent socket path changes per login.
- **Correction:** at the limit, LRU-retire an instance with no bindings, openings or leases, in one registry transition. Its adapter is disposed under the cleanup domain.
- **Closure test:** the probe's 17th profile is admitted; the retired adapter's disposal is confirmed; an attach-versus-retire race row.

**P2-4 (ARCHITECTURAL) — The §9 stream and effect surface was never built and no §15 row amends it.**
- **Location:**
  - `exchange.rs` `MessageStream` has only synchronous `read`/`write` returning `Io(WouldBlock)`.
  - `ports.rs` `ExchangeIo::read/write` take no `Context`, so a caller cannot be woken.
  - There is no `write_all`, `flush`, `shutdown_write` or `progress()`.
  - `Exchange::response_head` (§4, §7.2) is absent.
  - The local `OpenFailure` and `ExchangeEnd` carry no `Effect` (§9).
- **Invariant:** contract §4 and §9 ("asynchronous read … shutdown_write", "progress()", "Effect … is included in OpenFailure/ExchangeEnd") and §7.2 (`response_head`).
- **Evidence:** consumers sleep-poll: `connection_host/tests.rs:331-340` and `https_tests.rs:297` (`thread::sleep(1 ms)` on `WouldBlock`). §15 records only "ExchangeIo has no shutdown_write" (1.3b) and the 1.3c README's "errors carry no Effect".
- **Impact:**
  - Phase 3 and 5 consumers (the libgit2 bridge) will be built on polling.
  - A dropped write cannot be resolved by progress.
  - Replay safety has no typed effect.
- **Correction:** add a poll-based port (`poll_read`/`poll_write`/`poll_shutdown_write` with `Context`) and the async `MessageStream`, or amend §9 through §15 with an owner step before 3.1.
- **Closure test:** crate rows where:
  - a pending read wakes on data, with no sleep;
  - `write_all`/`progress` hold after a drop;
  - a POST half-closes;
  - `response_head` resolves once, then gives `WrongState`, and `WrongScheme` on SSH;
  - a local `OpenFailure` carries an `Effect`.

**P3-1 — The R3 and §13.2 proof rows are narrower than their claims.**
- **Root cause:** the sustained workload fixes its diversity (two names, four scopes, one profile) and asserts its own tables, not other operations' outcomes.
- **Evidence:** R3-M2 and R6-M1 stay green in the crate (§0), and P2-3 is invisible to the 10,000-operation row.
- **Consequence:** the "repeated external request IDs" adversity of contract §14 A3/A5 passes with cross-operation cancellation keyed by that ID. The only guard is a pool-level test.
- **Correction:**
  - Give A and B the same operation name in the R3 row.
  - Use a fresh scope per operation in the workload, and more than 16 profiles once P2-3 is fixed.
  - Assert each ring operation's finish disposition.
- **Closure:** R3-M2 and R6-M1 go red.

**P3-2 — No maximum connection age anywhere.**
- **Location:** no age field or check in `gwz-transport/src/pool` or the crate.
- **Invariant:** §10 (10 min) and §7.1 ("checked at release"). Carried to "the Phase 1 review" by name in the §15 1.6 row; no owner step since.
- **Consequence:** within one binding, connection reuse (which needs no proof) has no revocation or age bound.
- **Correction:** stamp a connection's birth, check it at release, and add a row; or add a §15 amendment with an owner step.

**P3-3 — `Clock::epoch` is never read.**
- **Location:** `ports.rs:49`; only the fakes implement it. gwz-core `DriverPorts::epoch` returns 0 over an `Instant` (`connection_host.rs`, `impl Clock for DriverPorts`).
- **Invariant:** §10's suspend/discrepancy rule has no path, and nothing records that.
- **Consequence:** after a suspend, idle entries that are stale by wall time are offered as fresh. The first exchange then fails or goes through the lost-lease retry.
- **Correction:** consume the epoch in `Instance::pump` (expire idle entries and recompute deadlines), or amend §10 through §15.

**P3-4 — Local terminal receipts always report `CleanupStatus::Complete`.**
- **Location:** `instance.rs` `settle()`, `exchange.rs` `discarded()` and `host.rs` `retire_if_done` hard-code `Complete`, including for a discarded lease whose `Close` the adapter has not confirmed.
- **Invariant:** §5 ("Every terminal receipt states … cleanup") and D4. Accounting stays correct: the pool keeps the entry in Closing.
- **Consequence:** a receipt claims cleanup the adapter has not confirmed, and no row can see it, because the fake adapter's `close` completes at once.
- **Correction:** report `Pending(ticket)` until `Completion::Closed`, or add a §15 amendment that per-operation receipts exclude physical disposal.
- **Closure:** a fake adapter that holds `Closed` yields a pending `ExchangeEnd`, which confirms afterwards.

**P3-5 — The cross-Client spawn-gate fairness residual is recorded in no root document.**
- **Location:**
  - `composition.rs:31` mints a new `ProcessGate` value per call.
  - `NativeHost::new` (`native/sspi.rs:32-33`) calls it per Client.
  - Registration and the one-kernel-waiter serialization live per value (`spawn_gate.rs` `Shared`).
- **Analysis (hypothesis, unconfirmed on Windows):**
  - Across values only the kernel semaphore orders.
  - With two Clients a polling launch wins only in release windows, so success is probabilistic.
  - With three or more, queued kernel waiters plus hand-off can keep it out until `GateTimeout` (fail closed).
- **Overclaims and stale text:**
  - gwz-py `client_host.rs:33-38` says the gate is "shared by every Client … so no Client makes a gate of its own", which is true for the kernel object only.
  - Plan line 449 ("which no run README records") is stale.
- **Correction:**
  - Record the residual in the §15 1.3a row and in plan 3.2. One composition-owned gate value per module or command is the remedy, and it is the operator's call under D-E.
  - Add a Windows probe with two `process()` gates.
  - Correct the doc comment.

**P3-6 — The closeout lanes have no root record.**
- **Location:** the plan's Record block (2026-10-31) names none of p1c-crate (gwz-transport `0a253639`), p1c-fair (gwz-core `44e532b2`, `1b6670a0`) or the pin `25039b78`.
- **Consequence:** the fairness change and its residual (P3-5) exist only in commit messages and an evidence README.
- **Correction:** the Phase 1 report adds these landing rows.

## 3. Invariant analysis
- **Holds:**
  - Owner, handle and operation split: `HostOwner` is not `Clone`; there are no public constructors.
  - Registration is a single locked transition with a terminal `NeverRegistered` result (`host.rs` `publish`/`seal`; the 2.2c refactor preserved it).
  - Revocation releases each claim exactly once.
  - IDs are monotonic and checked.
  - Routing uses (carrier, host, binding, session).
  - Stale frames create no state; the sustained row checks this every operation.
  - Per-binding limits follow the max(ceiling, limit) rule.
  - The verdict and deadline handling for reuse proofs matches §15.
  - The crate has no globals or thread-locals; `cfg` appears only in `cfg_if!`; the dependencies are `cfg-if` and `gwz-transport` only.
  - Pool changes are compatible with the older callers: `install_capacity` and `ActiveOperation` have no callers left in gwz-core, gwz-cli, gwz-py or gwz-git; a request with no limits uses the pool's own capacity.
- **Broken or absent:**
  - Timer independence (P2-1).
  - Profile-driven construction (P2-2).
  - Instance retirement (P2-3).
  - §9 surface (P2-4).
  - Age and suspend rules (P3-2, P3-3).
  - Truthful per-operation cleanup (P3-4).
- **Observations:**
  - `HostSnapshot`/`OperationEnd` lack §11's created/reused/discarded and exchange-total counters.
  - The shared proof job runs under the first waiter's deadline (already recorded).

## 4. Risks and next action (including what the Phase 1 report may and may not claim in the §3 Status column)
- **First, the tuple:** re-pin the review tuple (root `888de752`) or confirm that the draft is outside the object, then request the verdict.
- **Next:** remediate P2-1 and P2-3 in the crate (both are small: a notify; an LRU retire plus disposal) and decide P2-2 and P2-4 (fix, or §15 plus a plan owner step before 3.1).
- **The Phase 1 report may claim in the §3 Status column:**
  - R1: "owner-level row passes (fakes)".
  - R3: "passes with fakes on three platforms; real-adapter legs only".
  - R4: "partition passes with fakes; helper-environment legs pass on the real HTTPS adapter; configuration legs proven on a test factory only".
  - R6: "10,000 operations bounded on one profile, with fakes".
- **It may not claim:**
  - R4 as passed on the hosted-like suite, while P2-2 is open.
  - R6 as sustainable for a long-lived owner, while P2-3 is open.
  - Contract §9 conformance, while P2-4 is open.
- **Risk if this lands unchanged in Phase 3:** a module-level owner with mixed captures would configure or authenticate one Client with another Client's inputs (P2-2).

## 5. Out-of-scope observations
- **Root moved during the review:** `888de752`, the HTTPS auth model draft. It is not read here.
- **A global in gwz-transport:** `src/pool/machine.rs:7` `static NEXT_POOL` is a process-global counter that every gwz-connection instance now uses. It is pre-existing and allowlisted as debt (`process_globals_allowlist_gwz_transport.json`, owner CS7.2–CS7.6).
- **Guard coverage:** `check_cfg_boundaries.py` scans gwz-core, gwz-cli and gwz-py, not gwz-transport (including gwz-connection). The crate is compliant by inspection; `src/pool/setup_clock.rs:307` `#[cfg(test)] mod tests` is pre-existing.
- **Misplaced doc comment:** `host.rs`: "/// The instances the driver drives." now sits above `tables()` (closeout commit), not above `instances()`.
