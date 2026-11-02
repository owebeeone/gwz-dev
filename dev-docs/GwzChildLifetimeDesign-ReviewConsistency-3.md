# Cancellation design B r3: Consistency-axis review (round 3)

**Review object:** `/Volumes/projects/limbo/gwz-dev-docs-cancel/dev-docs/GwzChildLifetimeDesign-DRAFT.md`, revision 3, 399 lines. Hash at start `0cbb9081363ab26786635b13280df11550a8a0b7`. Hash at end `0cbb9081363ab26786635b13280df11550a8a0b7`, so the object did not change. | **Date:** 2026-11-03 | **Axis:** Consistency. This covers:
- internal contradictions;
- agreement with D-V (all bullets, including O4 and O5) and with every cited document and code line;
- the §8 superseded-clause table;
- whether the rows can be satisfied;
- placement against plan §4 and the 2.3 plan;
- the hand-offs to A, C and D.

**Verdict: GO.** Counts: 0 P0, 0 P1, 0 P2, 6 P3 (new). All 18 round-2 findings are closed: 10 from Consistency and 8 from Safety. No new finding is architectural, so design B's architectural count stays at 0. The P3s are recommended before the 2.3r-2 and 3.2 lanes are briefed, but they do not block. N-P3-1 and N-P3-2 matter most, because each misleads a lane brief.

## 0. Evidence base

- **Pin.** `git hash-object` gave the same value at start and end (see the header). Root checkout `gwz-dev-docs-cancel` is at `6d20ef6f`. B, A and both RemPlan-2 files are uncommitted working-tree edits.
- **Members.**
  - gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-sspi `6f83551` and git2-rs `d13951f` are checked out at the design's pins with clean trees.
  - gwz-core's checkout is at `7f571a18`. Every gwz-core citation was therefore read at the pin with `git show b745f1fa:<path>`.
- **Documents read in full:**
  - B r3;
  - both round-2 reviews;
  - `GwzChildLifetimeDesign-RemPlan-2.md`.
- **Documents read in part:**
  - plan §10 D-V :995-1071, all bullets, line numbers re-derived;
  - plan §4 :318-331 and :712-742;
  - 2.3 plan :194-250;
  - A r2 at `6d20ef6f`: :30-36, :86-92, :110, :122, :176-186;
  - A r3 working copy: header, §3.1 :92-127, §3.2-§3.3 :128-153, §3.4 Force :170-212, §4 :286-303, §5 :304-317, R9/R14/R15, §8 :369-386;
  - parked :808, :809, :1212, :1218, :1224;
  - contract :289;
  - blob `3448c561` :244-247;
  - TR1.6 :75, :76, :82, :398;
  - gwz-core `dev-docs/history/GwzTransportCredentialHelpers-ReviewCode-2.md:66`;
  - the f9 report :296.
- **Code read:**
  - gwz-core at `b745f1fa`:
    - `runner.rs` :105-240;
    - `https_auth.rs` :20-30, :100-195;
    - `owner.rs` :74-99, :250-305;
    - `https_worker.rs` :300-305, :372-380;
    - `process_tree.rs` :1-112;
    - `process_tree/windows.rs` :44-62, :113-118;
    - `windows/launch.rs` :1-3, :224;
    - `spawn_gate.rs` :1-60, :130-200, :271-302, :330-470;
    - `composition.rs` :15-50;
    - `transport_host/mod.rs` :12, :32, :300-395;
    - `driver_only.rs` :59-62, :150-153;
    - `backend.rs` :66-70, :110-126;
    - `commit_log/mod.rs` :292-365;
    - `lib.rs` :45-52;
    - `local_command.rs:79` and `cancellable.rs:238`;
    - `https_worker/native/sspi.rs` :25-110;
    - `handshake_local_rows.rs` :443-450;
    - `runner_tests.rs` :297-300;
    - `verify_checkout_state.rs` :122-134;
    - `Cargo.toml` :48-52, :128-156;
    - `scripts/checks/check_spawn_gate_minting.py` (header, ROOTS, ALLOWED);
    - `check_process_globals.py` :95-125;
    - `check_crate_versions.py` :1-30, :70-85;
    - `local_clone_inventory.json` (keys, roles, role_edges, package entries);
    - `publish_crates.py` (order derivation).
  - gwz-cli: `lib.rs` :150-215; `globalargs/dispatch.rs` :40-110; `log_exec.rs:118`; `hook/family.rs:63, :149, :187`.
  - gwz-py: `native/src/client_host.rs` :25-70; `shims.rs` :40-80, :145-165; `route/native.rs` :30-50; `dispatch/log.rs` :80-105; `README.md` :155-170; the tokio entries in its manifests.
  - gwz-git: `native_commit.rs` :86-96; `native_commit/cli.rs` :15-22, :45, :74, :87.
  - gwz-sspi: `supervisor/platform.rs` :1-9; `supervisor/windows/launch.rs` :276-280.
  - git2-rs: `cred.rs` :124, :391-392.
  - Root `MODULE.bazel` :70-98 at `6d20ef6f`.
  - std 1.95.0: `unix.rs` :93-94, :141, :149, :342, :352-353, :359-372, :383-385, :408, :463, :990-996; `pipe/unix.rs` :28, :34-40; `os/unix/process.rs` :219-222; `os/windows/process.rs` :37-39, :181-182, :342, :445-449.
- **Method.** Reading only. Nothing was built or run, no file was modified, and no git mutation was made.

## 1. Prior-finding closure table

| ID | Round-2 finding | Status | What I checked in r3 |
|---|---|---|---|
| C-N-P2-1 | The `RegistryFault` contract does not reach the member; B19 cannot pass | **Closed** | §3.1 "Faults" gives eight fixed messages, a `Copy` enum and transport through `io::Error::other`. The per-site table matches code: `runner.rs:115-121` (a missing helper maps to `MissingExecutable`, other errors to `SpawnFailed`), `https_auth.rs:175, :190, :142`, gwz-git `cli.rs:17-21` (with `"git commit"` at :45), `verify_checkout_state.rs:124-129` and `commit_log/mod.rs:296-300, :359-361`. `https_auth.rs` and `runner.rs` (`:115-121`, `:145`) are in 2.3r-2's Owns. B19, B27, R-B2, R-B16 and R-B19 quote the exact strings. `ErrorCode` is named as the transport's. |
| C-N-P3-1 | 2.3r-1 cannot pass the gates with the files it owns | **Closed** | §4 2.3r-1 owns root `MODULE.bazel` and its lock (verified that `@gwz_core_crates` lists every `crates/*` manifest, :73-96), the inventory (LBT-001), `check_crate_versions.py` (`EXPECTED_CRATES = 18` at :85, with 18 directories) and the allowlist entries. "Waves" names these as shared files merged serially, and §8 rows 1 and 3 match. The crate's role is still unstated: new N-P3-5. |
| C-N-P3-2 | `close()` takes the gate's hold; no public path | **Closed** | §3.1 has the registry's own creation lock, which §3.2's `close()` takes. Public `ProcessGate::kill_killable()` and `close_children()` are added in `composition.rs`, which `pub use` already exports as a type (`transport_host/mod.rs:32`). A r3's Force step 4 and §4 point (1) resolve to it. A related re-export gap is new N-P3-1. |
| C-N-P3-3 | Writer-thread death leaves the release order undefined | **Closed** | §3.3 "The writer thread": a waiter reaps only after its acknowledgement or after the reaper has been reaped, and `close()` counts an exit it observes. B7 has the panic leg with live `Wait` and `Kill` entries. |
| C-N-P3-4 | Requirement and row IDs share one namespace | **Closed** | Requirements are BR1-BR8 and rows are B1-B29. Every `BRn` reference in §2, §3.4 and §3.6 is a requirement; every bare `Bn` is a row. BR4 cites :1006, which is the "every helper process it started" line. |
| C-N-P3-5 | (reg) marks on legs that are red first | **Closed** | (reg) appears only on the W helper legs of B13, B18 and B19, where the helper has its own job today (`process_tree/windows.rs:54-62`). B3's W leg is unmarked. |
| C-N-P3-6 | README text applied "at acceptance" | **Closed** | §8's heading and row 6 say "applied by step 3.2", and §4 3.2 owns `gwz-py/README.md`. :163 is "## Platform And Status". |
| C-N-P3-7 | r1 OD-C2 (a) pricing is missing from the history note | **Closed** | The §8 note cites `3448c561` :245-246, which I verified as "one extra process per gwz process that starts children". |
| C-N-P3-8 | OD-B2 does not record the alternative | **Closed** | OD-B2's "Not chosen" records an own group without a registry, with its cost. |
| C-N-P3-9 | The Windows `Process` target differs from Unix in unstated ways | **Closed** | §3.2 "Windows targets differ" covers CTRL_C being disabled whatever `ignore_interrupts` says, and descriptors living in the job. R-B7's recognizer is now per platform. |
| S-N-P2-1 | The reaper SIGKILLs a commit that 1.0.18 let finish | **Closed (record)** | OD-B1's cost and R-B4's "When" name every listed death and the 1.0.18 comparison. OD-B4 is a §6 row: B is complete as (a), recommends (b), and hands the choice to D. B29 asserts (a). The operator has not yet decided OD-B4; D-V (:1063-1069) is silent on it (see §4). One trigger is still missing: new N-P3-4. |
| S-N-P2-2 | The per-spawn fork cost is unpriced | **Closed** | §3.4 "Cost" states the per-spawn page-table copy and strict-overcommit charging (`unix.rs:463` verified), and adds `OutOfMemory`, R-B19, the README text, the 3.4 measurement, and the B5 and B19 legs. |
| S-N-P3-1 | Handlers run in the child; unmask before ignore | **Closed** | §3.2 "Unix" order: the spawning thread blocks all signals; four signals are set to `SIG_IGN`, then each to `SIG_DFL` unless it was ignored; then `ignore_interrupts`; then the register write; the mask is emptied last. This is consistent with std's order (`setpgid` :342, SIGPIPE :359-372, closures :383-385). The reaper's launch contract matches. B8 and B15 have the seam legs. |
| S-N-P3-2 | `ECHILD` status unspecified; kills not disarmed | **Closed** | §3.3 says the entry is unregistered at once, later kills do nothing, and `StatusLost` is returned. The §3.1 exit row and the `runner.rs:145` downcast are in Owns. B11 and B27 cover it. R-B1 notes the Linux SIGCHLD `SIG_IGN` parity. |
| S-N-P3-3 | macOS `ESRCH` while the child is exiting | **Closed** | §3.3 retries with a 1 ms to 100 ms backoff (marked (OS)). B25 has the 50 ms seam. |
| S-N-P3-4 | A failed descriptor sweep loses BR1 silently | **Closed** | The §3.4 launch contract closes the write end by number first, falls back on failure, and calls `_exit(1)` if neither method confirms the sweep. B4 has the seccomp and `/proc`-hidden leg. |
| S-N-P3-5 | Writer death and a full lifeline | **Closed** | The writer half is closed with C-N-P3-3. The full lifeline is recorded in R-B2 (about 4,096 records = 64 KiB / 16 B), the blocking lifeline is listed among the "Choices open to review", and B25 has the 5,000-spawn leg. |
| S-N-P3-6 | Registry-less mode unspecified | **Closed** | §3.1 "Without a registry" and B28 cover it. The rejection of "std's wait" is justified by `owner.rs:253-256`'s group kill coming before the reap. |

## 2. New findings

None is architectural. Each one closes with text, an Owns entry or a §8 row.

### N-P3-1: 2.3r-2 needs a line in a trunk-owned file that it does not own, and §4 says its files are outside the trunk's

- **Root cause (not architectural).** The `composition` module is `pub(crate)` (`transport_host/mod.rs:12`). Only `ProcessGate` and `process_safety` are re-exported (`:32`). gwz-cli (3.1) and gwz-py (3.2) call the new `composition::process_safety_with_children()` from outside gwz-core, so it needs a `pub use` line in `transport_host/mod.rs`.
- **Ownership conflict.** That file is a Phase 1 trunk file (plan :325) and a shared 2.3 file ("`transport_host/mod.rs` … one export each in g and k", 2.3 plan :246-247).
- **Also missing from the Owns list:**
  - `scripts/checks/test_check_spawn_gate_minting.py`;
  - `scripts/checks/test_check_process_globals.py`.

  These are the unit tests of the two checks whose rules 2.3r-2 changes (B17).
- **Location:** §4 2.3r-2 Owns; §4 "Waves": "2.3r-2's files are outside … the Phase 1 trunk's (plan :324-326)".
- **Invariant:** a step's Owns covers every file its content needs. The placement statement must be true.
- **Reproduction:** brief 2.3r-2 from §4 and add `process_safety_with_children` in `composition.rs`. gwz-cli cannot call it, because `gwz_core::transport_host::composition` is private.
- **Impact:** an unplanned edit to a trunk file, or a red 3.1 build. No product defect.
- **Correction:**
  - Add "`src/transport_host/mod.rs` (one `pub use`, asked of the trunk, plan :330-331)" and the two test files to 2.3r-2's Owns.
  - Restate the "Waves" sentence.
- **Closure test:** §4 lists the three files, and the placement sentence names the trunk file as asked for.

### N-P3-2: §2's claim about `sspi.rs:77` is false, and gwz-py's actual route to the three spawn sites goes unnamed

- **Root cause (not architectural).** §2 lists `src/git/endpoint/https_worker/native/sspi.rs:77` among gwz-core's own hostless backends "which reach none of the three spawn sites". OD-B2 relies on the same premise ("(§2)"). The code says otherwise:
  - `NativeHost::backend()` (`sspi.rs:76-80`) is documented as "its Git CLI spawns (`git commit`, `git tag`) and libgit2's credential helper take the gate" (`:72-75`).
  - gwz-py's candidate `own_backend` returns it (`client_host.rs:52-53`, reached through `shims.rs:74`) for every non-network request: commit, tag and log.
  - Its gate comes from `NativeHost::new`, which calls the registry-less public `transport_host::process_safety()` (`sspi.rs:32-34`). gwz-py calls it at `client_host.rs:46`, and gwz-cli's old entry calls it at `dispatch.rs:61`.
- **The "8 lines" figure.** §4 3.2 and §8 row 5 direct the change to "gwz-py's 8 hostless backend lines". The 8 `Git2Backend::new()` matches in `native/src` include 3 comment lines (`shims.rs:4, :153`, `log.rs:86`) and 3 test lines (`shims.rs:162-163`, `log.rs:101`). Only `client_host.rs:66` (non-candidate) and `route/native.rs:46` are production.
- **Location:** §2 "The gate"; §6 OD-B2; §4 3.2; §8 row 5; §4 "Interim".
- **Invariant:** cited code facts are true, and the hand-off to 3.2 names the lines that carry the requirement.
- **Reproduction:** apply 3.2 as written, giving `with_process_gate` to the hostless backend lines and leaving `native_route()` as it is. A gwz-py commit with a sleeping hook then spawns `git` on `NativeHost::new`'s registry-less gate.
- **Impact:**
  - B19's "commit and log: one registry holds every spawn" and B26 (`close_children()` waits for the daemon thread's commit) fail in 3.2.
  - The rows catch it, and the fix lies inside 3.2's own files.
- **Correction:**
  - §2 names `NativeHost::backend()` as gwz-py's non-network route and `NativeHost::new` → `process_safety()` as a registry-less mint.
  - §4 3.2 and §8 row 5 say the Client's `NativeRoute` is built with `NativeHost::with_process_safety(worker, &gate)` on the registry's `ProcessGate`, or replaced by the HostHandle's backend, and list the production lines.
  - OD-B2's premise is restated.
- **Closure test:**
  - §2 and §4 3.2 name `client_host.rs:46` and the `NativeHost` route.
  - B19's commit and log legs trace to the composition's registry.

### N-P3-3: The §1 exception lists are narrower than §7

- **Root cause (not architectural).** §0 says "except §7's residuals", but BR1 and BR4 each enumerate their exceptions, and the lists are incomplete:
  - BR4 ("never the wrong process") excepts only "an application's process-wide reaper (R-B1)". R-B1, and §3.3 "Why no kill…", also leave open gwz dying between a child's exit and its unregistration, entries left registered by a dead writer, and std reaping a child after an `exec` failure.
  - BR1's "Not covered" list omits the registered or tracked children that §7 says can outlive gwz: R-B2 (reaper killed or stopped), R-B3 (fork without exec), R-B14 (W suspended child) and R-B15 (`OwnInput`).
- **Location:** §1 BR1, BR4; §3.3; §7.
- **Invariant:** a requirement and its residuals agree.
- **Impact:** D, which builds the operator's accepted-residual list from B, reads the requirement as covering more than the design delivers.
- **Correction:**
  - BR4: "except R-B1's cases".
  - BR1: add R-B2, R-B3, R-B14, R-B15.
- **Closure test:** every §7 residual in which a registered child outlives gwz, or a kill can reach a reused ID, is cited by BR1 or BR4.

### N-P3-4: R-B4 and OD-B1 leave out the Windows default death of a gwz-py host

- **Root cause (not architectural).**
  - A r3 R15 lists "SIGTERM, SIGHUP; Windows CTRL_BREAK, CTRL_CLOSE" as tearing a gwz-py CLI write by default. Python's default for CTRL_BREAK ends the process.
  - R-B4 and OD-B1 list a gwz-py host's default SIGTERM and SIGHUP (R15's Unix half). On Windows they list only "the OS ending gwz after CTRL_CLOSE, LOGOFF or SHUTDOWN (A R9)".
  - When CTRL_BREAK kills a gwz-py host, its jobs close and the `git commit` job is killed mid-write: the R-B4 residue.
- **Location:** §6 OD-B1 "Such deaths"; §7 R-B4 "When"; B29's W leg.
- **Invariant:** R-B4 names every death that tears a CLI-route write (S-N-P2-1's required correction), and B agrees with A's residual table.
- **Impact:** D and the operator decide OD-B4 against a list that is missing a common Windows stop path. Recognition is the same (`*.lock`), so diagnosability is unchanged.
- **Correction:** add "W: a gwz-py host's default CTRL_BREAK (A R15)" to R-B4 and OD-B1.
- **Closure test:**
  - R-B4 cites A R15 for Windows.
  - B29's W leg records CTRL_BREAK, without asserting a result.

### N-P3-5: The crate's library-boundary classification is unstated

- **Root cause (not architectural).**
  - Each entry in `local_clone_inventory.json` needs `role`, `owner`, `rationale`, first- and third-party edges and their dev counterparts, and `expected`.
  - The roles are defined: "implementation" means "implementing a named contract and running its conformance suite"; "integration" means composition through injected ports.
  - The inventory says role or edge changes go "through lane C with the affected lane's review (LBT-012)".
  - §3.1 and §4 give only the edges (`libc`, `windows-sys`, `tokio`). `crates/children` implements no named contract, so no role fits as written.
- **Location:** §3.1 "Crate"; §4 2.3r-1 Owns.
- **Invariant:** 2.3r-1 can pass LBT-001 with what the design fixes.
- **Impact:** the 2.3r-1 lane has to make a policy classification the design left open, possibly with a lane-C review it did not plan for.
- **Correction:** state the role, owner and one-line rationale, and whether lane C reviews the entry.
- **Closure test:** §4 2.3r-1 names the role and owner.

### N-P3-6: The 2.3 plan's ordering text is left stale, and §8 row 5's operation is ambiguous

- **Root cause (not architectural).** 2.3j now waits for 2.3r-2. 2.3r-2 waits for 2.3r-1, which follows 2.3e in lane 3, and for 2.3s, which follows 2.3f in lane 2. In wave 3, lane 3 then carries about 800 lines (r-2) plus about 550 (j) ahead of 2.3l. Despite that:
  - The 2.3 plan still says "Critical path: 2.3d, then 2.3g, then 2.3h, then 2.3l. Off it: … 2.3e, 2.3f" (:234), and "about 4,300 implementation lines" (:71). No §8 row of B or A amends either clause.
  - §8 row 5 gives 3.1 and 3.2 text without "Add" or "Append". Row 1 says "Add:", row 5's 3.4 text starts "and …", and A §8 row 4 also appends to plan 3.1. Applied as a replacement, row 5 would delete plan 3.1's content, including the HTTPS prerequisite.
- **Location:** §8 rows 3-5.
- **Invariant:** after §8 is applied, the 2.3 plan's ordering statements are true, and every §8 row states its operation.
- **Correction:**
  - Add a row amending :234, and :71 or a note.
  - Mark row 5's 3.1 and 3.2 text "Append".
- **Closure test:** §8 names :234, and row 5 says "Append".

## 3. Verified citations

These are correct at the pins unless a finding above says otherwise.

- **D-V (plan, root `6d20ef6f`):**
  - :999 header; OQ2 :1005-1008 with "every helper process it started" at :1006 and death detection at :1007;
  - resolution (2) :1039-1043; (3) :1043;
  - decisions (i) :1056-1059, (ii) :1059-1061 (B cites :1059-1060, which is inside it), (iii) :1061-1062;
  - round 2 :1063-1069: O4 (a), O5, and "B: 3 P2, count 0".
  - Shape agreement:
    - B lives in gwz-core: `crates/children` is a gwz-core member by `members = ["crates/*"]`, `Cargo.toml:50`.
    - The reaper is forked at `create`, and its loop reads the lifeline and calls `kill`.
    - It is shared by gwz-cli and gwz-py.
    - Fork without exec is documented (§8 row 6).
    - The sub-steps come before 2.3j.
    - Force does not exit mid-write; commit and tag run in their own group; OD-B2 (a) and OD-B3 are applied.
    - O4 (a) and O5 are applied through R-B4 and §3.7.
- **Plan §4:** trunk :325-326; asking for a file :330-331; Phase 5 owns `native_commit`, `refs.rs` and `repository.rs` (:329-330), none of which 2.3r-2 touches; 3.1 :721-728; 3.2 :729-735; 3.4 :739.
- **2.3 plan:** 2.3j :194, "After" :202; 2.3l :220-230; waves :240-241; review and totals :249-250. B's arithmetic holds: 4,300 + 600 + 380 = 5,280; 4,750 + 650 + 420 = 5,820; 12 + 2 = 14. A r3 §8 row 7 is consistent with this (+300, +300, 15 steps).
- **A r2 (`6d20ef6f`):** A6 :33; Force step 4 :179-180; "A calls no close()" :183-184; the PID check :89-91.
- **A r3 (working copy):**
  - §3.1 "Unix": SIGQUIT keeps its default.
  - "Check": 2.3r-1 and 2.3r-2 follow 2.3s.
  - :100 "first threads after the registry's", consistent with B §3.4's writer and watch threads.
  - §3.2 host close; §3.3 `Pending`.
  - §4 points (1)-(8); the 2.3s row "before 2.3k, 2.3r-1, 2.3r-2"; row 6.1; R9, R14, R15; §8 row 7.
- **History and parked:**
  - parked :808, :809, :1212, :1218, :1224;
  - contract :289;
  - Safety (amendment) :109, :256-263 (round-1 verified, unchanged);
  - blob `3448c561` :245-246;
  - TR1.6 :75, :76, :82, :398;
  - `GwzTransportCredentialHelpers-ReviewCode-2.md:66`;
  - f9 report :296.
- **gwz-core:**
  - `runner.rs` :109-110, :115-121, :123-144, :141, :145;
  - `https_auth.rs` :28, :106-167, :142, :175, :190;
  - `owner.rs` :77, :97, :253-303, :261-275, :298;
  - `https_worker.rs` :303, :377;
  - `process_tree.rs` :21-24, :42, :57-75, :71, :79-86, :89-90;
  - `process_tree/windows.rs` :44-48, :54-62, :115-118;
  - `windows/launch.rs` :1-3, :224;
  - `spawn_gate.rs` :431-436, :438-446, :450-454 (the gate is not re-entrant, :334-336; the Unix process gate does not exclude, :271-274);
  - `composition.rs` :20-28, :30-35, :38-40;
  - `driver_only.rs` :61, :152;
  - `backend.rs` :69, :114, :118-125;
  - `local_command.rs:79`, `cancellable.rs:238`;
  - `commit_log/mod.rs` :296-300, :330-358, :337, :347, :354-356, :359-361;
  - `lib.rs` :47-51;
  - `Cargo.toml` :50, :132-139, :139, :152;
  - `handshake_local_rows.rs:447`;
  - `runner_tests.rs:299`;
  - `verify_checkout_state.rs:124-129`;
  - `check_crate_versions.py:85`;
  - the `check_spawn_gate_minting.py` ROOTS include `crates/*/src/lib.rs`;
  - `check_process_globals.py` `LIBC_FUNCS` includes `chdir`, `signal` and `sigaction`.
- **Members:**
  - gwz-cli `lib.rs:160-163` and the exit sites; `dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187`;
  - gwz-git `native_commit.rs:88-95, :93-95`, `cli.rs:17-21`;
  - gwz-sspi `platform.rs:1-9`, `launch.rs:276-280`;
  - git2-rs `cred.rs:124, :391-392`;
  - gwz-py `README.md:163`; no `tokio` `process` feature anywhere in gwz-cli, gwz-py or gwz-transport (B17 is satisfiable).
- **Toolchains:** std `unix.rs` :93-94, :141-149, :342, :352-353, :359-372, :383-385, :408, :463, :990-996; `pipe/unix.rs` :28, :34-40; `os/unix/process.rs:220-221`; `os/windows/process.rs` :37-39, :181-182, :342, :445-449.
- **Exceptions:** the §2 claim about `sspi.rs:77` and the gwz-py "8 lines" (N-P3-2).

## 4. Risks and next action

- **OD-B4 is still open with the operator.** B is internally consistent as (a), and D owns putting the question. Until D runs, the ordinary stop of a gwz-py service (default SIGTERM, or CTRL_BREAK per N-P3-4) tears an in-flight commit that 1.0.18 would have finished. D-V does not settle this, so the hand-off has to stay visible in D's brief. If (b) or (c) is chosen after 2.3r-1 has merged, D needs a step that owns the crate amendment.
- **A r3 cites B by stale line numbers:**
  - "B :254-255", ":241", ":244", ":278", ":378", ":381";
  - and `runner.rs:226`, a closing brace; the `terminate` call is at :225.

  B's text now matches the substance of A's points 2, 5 and 8 (A r3's host close, `Pending`, and the totals supersession), so the hand-off is clean in substance. A should cite B by section; that is A's fix, not B's.
- **Release.** A new published crate (`gwz-children` presumably) changes the publish order. `publish_crates.py` derives the order from the manifests, so only `EXPECTED_CRATES` and a first crates.io publish of the new name are affected. Worth one line in the 2.3r-1 brief.
- **Next action.** Apply the P3s as text edits before briefing 2.3r-2 (N-P3-1) and 3.2 (N-P3-2). No re-review is needed for them under this axis.

## SURFACE (what B freezes that users see)

- **Messages, all with the transport's `ErrorCode::Io` unless noted:**
  - `child-process reaper could not be started`
  - `child-process reaper unavailable`
  - `child-process limit reached`
  - `child processes refused in a forked copy`
  - `child processes refused: closing` (the helper reports this as `Cancelled`)
  - `child-process mode unsupported on this platform`
  - `child process not started: out of memory`
  - `child-process exit status lost: reaped elsewhere`

  The CLI route shows these as `GitCommandFailed` `failed to run git commit: <message>`. `rev-list` shows them as a `HistoryUnreadable` degradation.
- **Outcome.** A cancelled or timed-out helper lookup whose cleanup outlasts 500 ms reports `Cancelled`, or `Timeout` with `Interaction`, instead of `Io`. The pending tree is counted separately.
- **Processes:**
  - one extra process per candidate gwz command and per gwz-py composition: a fork in its own session, matched by `pgrep gwz` or `pgrep python`;
  - on Windows, registered children sit in kill-on-close jobs;
  - `Detach` is refused on Windows.
- **Commit and tag (OD-B1 (a), OD-B4 (a)):**
  - hooks and signers no longer receive the terminal's Ctrl-C, Ctrl-\ or Ctrl-Z;
  - on Unix a terminal reader stops; on Windows it has no console;
  - `gwz commit` waits through Cancel and Force;
  - gwz's death mid-commit (SIGKILL, SIGQUIT, an interrupt once Force has begun, or a gwz-py host's default signal) leaves `index.lock`.
- **gwz-py:**
  - `close` waits for a running commit, and later members are refused;
  - spawns are refused after `os.fork()` without exec and under strict overcommit;
  - the README text of §8 row 6 is fixed;
  - process-wide reaping is unsupported.
- **Exit status:** B fixes none; that is A's.
