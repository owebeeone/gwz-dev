# Phase 1 (step 1.8) — remediation plan, round 1

**Status:** plan, 2026-10-31.

**Object:** the Phase 1 slice at root `bcfa8ac7` / gwz-core `a7b965c5` / gwz-transport `0a253639` / evidence `c30d49b3`.

**Reviews:** `GwzPhase1-ReviewCode.md` and `GwzPhase1-ReviewState.md`.
- Both verdicts were withheld because the root moved during the review, to `888de752`. The lane owner committed the HTTPS draft at root during the review; that was a lane-owner error. The commit changed no controlling document and no member.
- On the unchanged object the findings stand, and they amount to NO-GO.
- Code pre-committed to GO on P2-1 and P2-3 fixed, with P2-2 and P2-4 fixed or owned. State pre-committed to GO on P2-1 closed and its P3s dispositioned.

**Blind convergence:** both axes independently found P2-1, the driver's timer not re-armed by caller-driven pumps. State reproduced it on the real HTTPS adapter: the 30 s allocation bound was still undecided at 45 s.

**Operator decisions (2026-10-31):**
- fix BOTH architectural findings (Code P2-2, P2-4) now, inside 1.8;
- fold the HTTPS authentication model (`GwzHttpsAuthModel-DRAFT.md`) into this remediation. Within it:
  - the operation holds the credential (option b2);
  - the Windows NTLM/Negotiate placement is fixed before 3.1;
  - the credential key includes the service class;
- the re-verdict is a NEW round with fresh reviewers, each carrying the prior-finding closure table.

**Architectural root causes counted:** two (Code P2-2, P2-4). Under the cap, a third new architectural root cause found in round 2 stops the lane for an operator redesign-or-accept decision.

**Process:** the steps below are one remediation. They land as separate lane commits, and are re-reviewed together on one settled tuple. During the re-review nothing is committed to root, and the briefs say so.

## Dispositions

| ID | Finding | Disposition | Step | Closure test |
|---|---|---|---|---|
| Code P2-1 = State P2-1 | Caller-driven pumps do not re-arm the driver's timer: allocation, idle and proof deadlines go unenforced on a quiet host | Fix: notify the driver's `Signal` after every caller-driven pump (`submit`, `settle`, `release`, `abandon`, `cancel_binding`); never from `drive` | R1 | Crate rows PROBE-ALLOC and PROBE-PROOF (driver parked first) and the p1c-crate idle repro, all expiring at their bound. Sustained rows restored to assert idle EXPIRY. gwz-core real-adapter row: HTTPS per-host-1 second opening ends `AllocationTimeout` within bound plus slack |
| Code P2-3 | Unbound endpoint instances are never retired; the 17th profile is refused forever | Fix: at the limit, LRU-retire an instance with no bindings, openings or leases, in one registry transition; its adapter is disposed under the cleanup domain | R1 | The reviewer's probe as a row (17th profile admitted, retired adapter disposed); an attach-versus-retire race row |
| Code P3-1 | R3 and §13.2 rows narrower than their claims (name-keyed cancellation and unpruned scope tokens stay green) | Fix: R3 row uses one operation name for A and B; sustained workload uses a fresh scope per operation and more than 16 profiles; per-operation finish dispositions asserted | R1 | Mutations R3-M2 and R6-M1 go red |
| Code P2-4 (ARCH) | Contract §9's stream and effect surface was never built | Fix in two steps. R2a: crate ports `poll_read`/`poll_write`/`poll_shutdown_write` with `Context`; async `MessageStream` (`read`, `write_all`, `flush`, `shutdown_write`, `progress()`); `Exchange::response_head`; `Effect` on local `OpenFailure`/`ExchangeEnd`. R2b: the SSH and HTTPS adapters implement the poll ports; gwz-core consumers drop their sleep-polls | R2a, R2b | Crate rows: a pending read wakes on data with no sleep; `write_all`/`progress` after a drop; POST half-close; `response_head` once, then `WrongState`, and `WrongScheme` on SSH; local `OpenFailure` carries `Effect`. gwz-core: no `thread::sleep` on `WouldBlock` remains in `connection_host` tests (source grep row) |
| Code P2-2 (ARCH) | Production `EndpointFactory` ignores `EndpointSpec`; instances take the runtime's inputs and `NativeCaller` | Fix: build each adapter from `spec.profile.inputs()` (`EndpointInputs` gets its production consumer); the native caller moves into the operation's `AuthContext`; `FactoryEnv::domain()` setups draw on the cleanup domain | R3 | gwz-core rows through `ConnectionHost` + `CoreFactory`: two captures differing in `SSL_CERT_FILE` against a fixture signed by CA B (B succeeds, A's trust refuses); two differing in `SSH_AUTH_SOCK`; native caller per operation (Windows) |
| HTTPS model (operator) | Credential need recorded on the connection; POST pre-auth depends on pool placement and a single slot (p1c-https Code and State P3-2); Windows logon-session push placement | Fix per the reviewed `GwzHttpsAuthModel-DRAFT.md`: credential need and the credential itself on the operation, keyed by host, port, path and service; zeroized at operation end; NTLM/Negotiate connection-bound with a pool placement hint | R4 (after R3 and the doc's review) | The draft's rows, including PR5/PR6 turning green, a cross-binding no-leak row, and an NTLM-style connection-bound push succeeding |
| State P3-1 | A registered launch has unbounded precedence over blocking spawn-gate holds (reverse starvation) | Fix: bound the precedence (a blocking hold waiting past a bound, or behind K launches, is served next), or one FIFO ticket order | R5 | Saturation probe asserts each blocking hold is admitted within the stated bound; `gate_fairness_tests` still within 2 s and `2*TAKERS` |
| Code P3-5 | Cross-Client fairness residual unrecorded; gwz-py doc comment overclaims | Fix the record: §15 1.3a row and plan 3.2 name it (one composition-owned gate per module or command is the remedy, an operator call under D-E); correct gwz-py `client_host.rs:33-38`; plan :449 stale text | R5 (comment), R6 (records) | Re-read |
| Code P3-2 = State P3-2(a) | No maximum connection age (§10, 10 min checked at release) | Fix: stamp birth, check at release | R1b | Row: a connection reused past its age is retired at release, never leased |
| Code P3-3 | `Clock::epoch` never read (suspend rule) | Fix: consume the epoch in `Instance::pump` and before `submit` leases (expire idle; R1b). gwz-core's `DriverPorts::epoch` returns 0: a real source (wall versus monotonic drift past the idle interval, interior state, no globals) moved to R3 | R1b, R3 | Rows: epoch change retires idle entries; an epoch change is seen before a lease; R3 row: drift bumps the epoch |
| Code P3-4 | Local terminal receipts always say `CleanupStatus::Complete` | Fix: `Pending(ticket)` until `Completion::Closed` | R1b | Fake adapter holding `Closed` yields a pending `ExchangeEnd` that confirms afterwards |
| State P3-2(b) | `MessageStream` surface | = Code P2-4 | R2a | — |
| State P3-2(c) | §6.1 ordered seam not on the stream-data path; §13.2 sequenced-profile conformance and seeded owner Monte Carlo | **Re-dispositioned 2026-10-31 (lane owner, R2a design stop):** the neutral ordered-stream kernel does not exist (contract §6.1 "not existing code"; `src/sequenced.rs` is the profile-3 envelope kernel behind `unstable-sequenced`), about 850 lines with the seeded run. Recorded with an owner: plan Phase 4 (settle the library), a new step there; the Phase 1 report must not claim §6.1/§13.2 sequenced conformance | Phase 4 | That step's rows |
| State P3-2(d) | Adapter setups do not draw on the `CleanupDomain`'s budgets | = Code P2-2 correction | R3 | Row: setup budget exhaustion refuses typed |
| State P3-2(e) | Pool `network_deadline` not in `ConnectCommand` | Fix: carry it | R2b | Row: a stalled connect ends at the network deadline |
| State P3-2(f) | Two last-lessee records | Fix: unify. The crate has one (`Setup.lessee`); the second is gwz-core's (`ssh_pool.rs` `entry.lessee`, `https_pool.rs` `serves_binding`, `ssh_worker/runner.rs`), so it moved to R3 (2026-10-31) | R3 | Existing rows green; one record |
| State P3-2(g) | No certificate or security-key proof rows (SSH reuse authorization) | Fix: rows on the SSH fixture | R3 | Rows green, red under a mutation that accepts the proof unconditionally |
| State P3-2(h) | Native caller per instance | = Code P2-2 | R3 | — |
| Code P3-6 / State §1 | Closeout lanes and Linux evidence on the merged tree unrecorded | Fix: Phase 1 report records p1c-crate, p1c-fair, p1c-https, the pin and the WSL run of the merged tuple (run `root-p1-20261031-a`) | R6 | Re-read |
| Code §5 / State §5 | `host.rs` doc comment reassociated by `tables()` | Fix | R1 | Re-read |
| Code §5 / State §5 | `static NEXT_POOL` in gwz-transport (allowlisted debt, owner CS7.2–CS7.6) | Record in the Phase 1 report as a stated limitation; no change | R6 | — |
| State §3 | Executor and retainer double refusal leaves an unowned close | Record (observation, not filed) | R6 | — |

## Steps

Each step is one lane and one goal, aimed at 500 lines or fewer. TDD: every row is seen red first. Gates follow the standing lane briefs. Lanes are cut from root main.

| Step | Goal | Owns | Depends on | Review |
|---|---|---|---|---|
| **R1** | Driver re-arm (P2-1), instance LRU retirement (P2-3), stronger proof rows (Code P3-1), `host.rs` doc | gwz-transport `crates/gwz-connection/src/{instance,host,pump}.rs`, `sustained_tests.rs`, `operation_tests.rs`, `host_tests.rs`; gwz-core real-adapter allocation row in `connection_host/https_tests.rs`; the gwz-core pin | — | round 2 |
| **R1b** | Maximum connection age, suspend epoch, truthful cleanup receipts, one last-lessee record | gwz-transport `src/pool/*`; crate `exchange.rs` receipt paths | R1 | round 2 |
| **R2a** | Contract §9 surface in the crate, split (design stop 2026-10-31): **R2a-1** poll ports with transitional defaults, async local `MessageStream` (`read`, `write`, `write_all`, `flush`, `shutdown_write`, `progress()` with optional `peer_confirmed`), sync calls renamed `try_read`/`try_write`; **R2a-2** `response_head`, `Effect` on `OpenFailure`/`ExchangeEnd` (wire `Effect` reused; `ResponseStarted` recorded missing in §15). The remote stream wake and the 5 ms tick (`remote/receiver.rs:370`) go to plan 2.3g | crate `ports.rs`, `exchange.rs` (stream half), `operation.rs` (`Effect`), `fakes.rs`, new `stream_tests.rs` | R1 merged (shares `exchange.rs`) | round 2 |
| **R2b** | SSH and HTTPS adapters implement the poll ports; `network_deadline` in `ConnectCommand`; consumers drop sleep-polls | gwz-core `ssh_adapter*`, `https_adapter/pipe.rs`, `connection_host*` tests | R2a | round 2 |
| **R3** | Factory built from the operation's profile; native caller in `AuthContext` (R3 is the single owner of the caller move, including removing `HttpsInputs.native`/`Common.native`; draft r3 §3.2, U6); caller identity in the scope digest with a row (only a non-secret identifier digest; an unreadable identity gives a scope value unique to the capture, never a constant); cleanup-domain budgets; SSH proof rows | gwz-core `transport_host/{connection_factory,mod,endpoint_environment}.rs`, `https_worker/native/sspi.rs` caller capture, `https_adapter.rs`/`https_adapter/{exchange,auth}.rs` (caller fields; `auth.rs:42` switches to the context's caller), `https_auth.rs` (caller in `OperationAuth`), new rows | R2b for the adapter constructors, or before R2b with disjoint constructors. Lane owner decides at R2b's brief. | round 2 |
| **R4** | HTTPS authentication model, as the group of sub-steps H1a-H1e, H2a-H2b and H3a-H3d with the file lists of `GwzHttpsAuthModel-DRAFT.md` r5 §5 (the accepted revision; about 3,805 lines added in eleven sub-steps, each ≤500; amended 2026-10-31 per that draft's Consistency P3-7 / U1). H1e and H2b edit gwz-transport `crates/gwz-connection` (single admission; affinity), H2a the gwz-transport pool (`prefer`, bounded settling wait) | gwz-core `https_adapter*`, `https_auth*`, `connection_host/https*` tests; gwz-transport `crates/gwz-connection` (H1e, H2b) and `src/pool` (H2a); contract §4.1/§7/§7.2/§8/§9/§10/§11/§15 and plan text (A1-A13); `connection_host.rs` mod lines for new test files | R3; the draft's Consistency + Safety re-verdict | round 2 (and the doc's own dual) |
| **R5** | Spawn-gate bounded precedence; gwz-py doc comment | gwz-core `spawn_gate*`, gwz-py `native/src/client_host.rs` comment | — | round 2 |
| **R6** | The Phase 1 report: plan §3 Status claims as narrowed by the reviews, the closeout records, the carried items with their owners, §15 rows | root dev-docs | all above | round 2 |

**Parallelism:** at most three compiling lanes (2.3a is one).
1. Wave 1: R1 and R5. The HTTPS draft's Consistency and Safety review runs alongside, with no compile.
2. Wave 2: R1b and R2a (after R1).
3. Wave 3: R2b, then R3.
4. Wave 4: R4.
5. Wave 5: R6, then round-2 fresh reviewers on one settled tuple.

## Round 2
Fresh Code and State reviewers. Each carries this table as its prior-finding closure table, re-runs the original counterexamples (PROBE-ALLOC, PROBE-PROOF, the 17-profile probe, R3-M2, R6-M1, the saturation probe), and labels any new architectural root cause.
