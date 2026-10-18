# Crate map review — architecture (Code axis), round 2

Object: `dev-docs/GwzCoreSessionCrateMap.md` revision 1, 207 lines, SHA-256 `d0c822951d041cfb76b65b4e7deed21ded1ac828b4ff4486366c1623c32d570c`; diff `cratemap-r0-to-r1.diff`, 236 lines, SHA-256 `a86f992950cc91e11bcbfd4a1fd6930d6189c89a1510717fef3604580c4c1838`. Both verified at start and end. HEADs root `621f671`, gwz-core `4a0b7c8d`, gwz-py `a342b95`. Verdict: **GO**. P0 0, P1 0, P2 0, P3 4 (new, all bounded).

## Closure of round-1 findings

| Finding | Status | Evidence |
|---|---|---|
| P2-1 forbidden transport edges | CLOSED | §4 adds `gwz-endpoint-contract` (BlockingStream, GitService, HttpsOpenFailure; Engine, Reservations, InstancePort, HelperSpawn) and a dependency column; registry and instance reach each other only through `InstancePort`; both endpoint crates implement `Engine` from the contract; gwz-transport is classified pure (§1, §7); core binds the ports. Every listed edge obeys `role_edges`. Residual: N1. |
| P2-2 secrets leave core | CLOSED | §1 "Secrets stay in core where they can"; §3 core implements `HelperSpawn` and returns pipes and a kill handle, so `https_auth::Config.environment` never enters a crate; the two credential-holding APIs are named and keep per-step dual review. |
| P2-3 `NEXT_POOL` | CLOSED | §4: the pool takes its ID from its host at construction, gwz-transport keeps no first-party dependency; §5 and §7 carry it into CS7.2–CS7.6. Residual: N3. |
| P3-1 IdSource uniqueness | CLOSED | §2: a 64-bit OS-random prefix per context, drawn by core; §6.2 names each source's owner, replaces family-store's `File::create` with an exclusive create and retry, and re-pins `NEUTRAL_RAW_WRITE_FLOOR`. A collision is now a retry, not a clobber. |
| P3-2 `CROSSING` | CLOSED | §2: non-Clone controls; closures and callbacks receive only a `GateScope`; the cloneable view keeps `state()` and the token; a per-gate thread record panics on same-thread re-entry; the undetected path is stated. A `'static` callback can no longer own a crossing capability. |
| P3-3 omitted text | CLOSED | §7 lists O9, §5.7's counters row, GWZDesign's paired paragraphs, server §2 and §4, plan §3.0's ratchet rules, §5.4's list and CS3.5's files; §2's supervisor note matches CS3.1. Residual: N3. |
| P3-4 roles | CLOSED | gwz-server-os is an integration crate and reads the OpenSSL files; gwz-server-policy parses over text (§2, §5 CS8.2). `Authority` is decision-based (`try_reserve`, `counts`, guards over `Arc<Mutex<State>>`), so its pure placement holds. |
| P3-5 tooling | CLOSED | §6.1 grows `EXPECTED_CRATES` and the release bump list; §4 names a second gate root with its own inventory and the prepare.py change; §1 publishes the transport crates with gwz-transport before gwz-core 1.1.0; §6.5 and §8.3 count thirteen (6 + 6 + gwz-transport). Residual: N2. |
| P3-6 git2 suites | CLOSED; contest confirmed | §1 and §3 keep git2-driven suites in core's integration tests. repo-inspect's `[dev-dependencies]` are exactly `gwz-repo-contract` (contract-tests) and `tempfile`; its git2 is a production edge, so my "as repo-inspect does" was wrong. (The inventory does allow the classified harness `gwz-local-testrepo`, which carries git2, as its dev edge; the map's choice is legitimate either way.) Residual: N4. |

## New findings

**N1 (P3) — The HTTPS bridge sits on the wrong side of `Engine`.** §4 gives gwz-endpoint-instance "the HTTPS bridge" (`transport_host/https_endpoint.rs`), which imports `https_worker::{Budget, ChallengeLease, Client, Endpoint, Input, Prepared}` and `placement_endpoint::{EndpointError, Outbound}` (:4-9): implementation internals. Rule: integration depends only on contract and pure. Consequence: the extraction either widens `Engine` with worker internals or creates a forbidden edge; §4's parentheticals likewise assign CS7.7, CS7.13 and CS7.22 to the instance crate though their files (placement_endpoint, ssh_worker, https_worker, ssh_shutdown, agent_job) are the endpoint crates'. Correction: the bridge is the HTTPS `Engine` implementation, in gwz-https-endpoint; the step parentheticals follow the files' crates.

**N2 (P3, inferred) — Two path sources for gwz-ids in the candidate build.** §4: "prepare.py adds the crates to candidate builds by path". prepare.py symlinks `crates/` under a new root, so the candidate's `gwz-ids = { path = "crates/ids" }` and a transport crate's `../../crates/ids` are different normalized paths to one package. Consequence: Cargo resolves gwz-ids twice, and core cannot hand the registry an `IdSource` of the registry's type (or the lock refuses the duplicate). Correction: symlink `transport/` into the candidate root as `crates/` is, or take gwz-ids by version with a `[patch]`.

**N3 (P3) — The reuse design's §14 is not amended.** The pool-ID change is a gwz-transport pool step beyond T1–T5, and contract §1 (as reuse §13 amends it) excepts only "the pool changes of §14". §7 lists the plan's CS7.2–CS7.6 but not the accepted design. Consequence: the pool checkpoint's dual review meets an unlisted gwz-transport change. Correction: add reuse §14 (a T-step) to §7.

**N4 (P3) — Suites moved to core see only public API.** §1, §3. https_worker_tests.rs (911 lines) and tests/transport_ssh reach `pub(crate)` items today. Consequence: either the endpoint crates' "narrow" APIs grow for tests, or CS7.1's "movement-only commit" rewrites those suites. Correction: say which (the `Engine` port, or a `test-support` feature), and remove the rewrite from CS7.1's movement-only claim.

## Verified

- Every §4 edge, and every §2 edge, obeys `role_edges`; gwz-transport has no dependencies and no physical I/O.
- `shared_reservation::Authority` holds no pool handle (shared_reservation.rs:24-226).
- Thirteen names: six ordinary, six transport, gwz-transport (`publish = false` today).
- `check_crate_versions.py` `EXPECTED_CRATES = 14` and release.py's `crates/` walk are the tooling §6.1 names.
- repo-inspect's manifest matches the map's P3-6 statement.
