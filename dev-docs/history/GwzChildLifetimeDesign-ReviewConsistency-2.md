# Cancellation design B r2 — Consistency-AXIS REVIEW (round 2)

**Review object:** /Volumes/projects/limbo/gwz-dev/dev-docs/GwzChildLifetimeDesign-DRAFT.md, revision 2. Hash at start `9724eae66f13de48959d7bc3dcdad1d4dcd6d423`; hash at end `9724eae66f13de48959d7bc3dcdad1d4dcd6d423` (unchanged). | **Date:** 2026-11-01 | **Axis:** Consistency. This covers internal contradictions; agreement with D-V (all bullets) and every document and code line it cites; the §8 superseded-clause table; whether the rows can be met on M, L and W; placement against the plan and the 2.3 plan; and the hand-offs to A, C and D.

**Verdict: NO-GO**: 0 P0, 0 P1, 1 P2, 9 P3. No new architectural root cause (the count for B stays at 0).

Round-1 status: all 25 round-1 findings are closed. S-P2-5 is closed except for one part: its user-visible message test (row B19) fails because of new finding N-P2-1.

**Pre-commit:** I will give GO on a revision that closes N-P2-1 with the closure test below and adds no new P0-P2. The P3s are recommended but do not block.

## 0. Evidence base

- **Pin.** `git hash-object` gave the same value at start and end (header).
- **Member checkouts.** Each is at the design's pin with a clean tree:
  - gwz-core `b745f1fa`
  - gwz-cli `c0ba717`
  - gwz-py `2185cc6`
  - gwz-git `9bc4ea9`
  - gwz-sspi `6f83551`
  - git2-rs `d13951f`
- **Root documents.** The root is at `48feeaad`. The plan, the 2.3 plan and A have not changed since `8ade43ac` (`git diff --stat 8ade43ac HEAD` shows only B and its RemPlan). B and the RemPlan carry uncommitted edits.
- **Documents read in full:**
  - B r2
  - both round-1 reviews
  - the RemPlan
- **Documents read in part:**
  - plan §10 D-V (:999-1064)
  - plan §4: :305-331, :654-682 and :712-742
  - 2.3 plan :181-250
  - A r1, hand-off points only (:36-50, :140-150, :152-162, :200-230, :262-275, :350-400)
  - the amendment r1 blob `3448c561` :240-258
  - `history/GwzCancellationAmendment-ReviewSafety.md` :90-112 and :254-264
  - parked :808, :809, :1212, :1218, :1224
  - contract :286-291
- **Code read:**
  - gwz-core:
    - `spawn_gate.rs` :1-45, :140-180, :415-470
    - `composition.rs` :20-45
    - `driver_only.rs` :58-63, :150-154
    - `backend.rs` :66-70, :112-126
    - `commit_log/mod.rs` :296-304, :330-372
    - `https_auth/runner.rs` :100-145
    - `process_tree.rs` :18-112
    - `process_tree/windows.rs` :40-62, :150-185
    - `owner.rs` :75-98, :253-327
    - `https_auth.rs` :27-29, :109-190
    - `https_worker.rs` :300-305, :375-378, :397-398
    - `runner_tests.rs` :295-301
    - `endpoint/mod.rs` :128-138
    - `verify_checkout_state.rs` :124-134
    - every production `Git2Backend::new()` and `without_credential_helpers()` site
    - `Cargo.toml` :40-60, :125-160
    - `BUILD.bazel` :1-45
    - `scripts/checks/check_process_globals.py` :95-125, :530-565, and its allowlist (roots and entries)
    - `check_local_clone_boundaries.py` :1-60
    - `local_clone_inventory.json` keys
    - `v1.0.18:repository.rs:297-306` and `refs.rs:170-184`
  - Other members and toolchains:
    - gwz-cli `src/lib.rs:155-352`, and every `Git2Backend::new()` site
    - gwz-git `native_commit/cli.rs:1-60` and `error.rs:100-112`
    - root `MODULE.bazel` :13-97, and `git log` for it
    - std 1.95.0 `sys/process/unix/unix.rs` :138-150, :336-412, :455-468; `sys/pipe/unix.rs:28, :34-40`; `os/unix/process.rs:219-222`; `os/windows/process.rs:37-39, :181-182`
    - tokio 1.53.1 `net/unix/pipe.rs:419, :967`
    - Go 1.26.4 `os/wait_waitid.go:5-6`
    - the f9 report's section list
- **Method.** Reading only. Nothing was built or run, no file was modified, and no git mutation was made.

## 1. Prior-finding closure table

| ID | Finding (round 1) | Status | What I checked |
|---|---|---|---|
| C-P2-1 | OD-B1 (a) leaves out its Windows cost; R-B10 has no Windows recognizer | **Closed** | §6 OD-B1 now states the hidden console prompt, the wait through Cancel and Force, and gwz's death as the only end, with 1.0.18's console sharing cited at `v1.0.18:repository.rs:299-305` and `refs.rs:172-183` (verified: plain `Command::output()`). R-B10's W recognizer names the reader in `git`'s own job. B15's W leg records, without asserting, a hook reading `CONIN$`. Decision (ii) cites "B OD-B1 (a)", and r1's (a) already carried "(no console on Windows)" (blob `48b992d9` §6), so the choice is the operator's. |
| C-P2-2 | Residuals for unregistered children cover only gwz's death | **Closed** | R-B5 now lists Force and the Windows console. R-B13 lists the interrupt mid-write (M, L, W). OD-B2 states that cost. B1 lists its exceptions, and B5 names its registry-less exception. §2, §3.1, §3.7, §4 "Interim", OD-B2 and R-B13 name the same registry-less set: the non-candidate build, the old entries, and `DriverOnly` (`driver_only.rs:61` and `:152` verified). See N-P3-8 on how this sits with decision (ii). |
| C-P3-1 | §8 contract row claims "only" starter | **Closed** | §8 row 7 now reads "every child a gate with a registry starts … (§3.6 lists the children it does not start)". |
| C-P3-2 | Windows helper missed by `kill_killable()` | **Closed** | §3.6, "The helper on Windows"; B8 now lets §3.6 name a platform mechanism. |
| C-P3-3 | §8 row settled C's 2.3k mechanism | **Closed** | The row is gone. §3.7 hands 2.3k's registry access to C, and the §8 note says the same. |
| C-P3-4 | "Callers do not change" was false | **Closed** | §3.1 names three changed callers, and §4 2.3r-2 owns their files. A grep confirms they are the only gate spawn and output callers (`commit_log/mod.rs:354` and `spawn_gate.rs:452`; gwz-git's trait sits behind the latter). |
| C-P3-5 | B10's and B21's W legs could not run | **Closed** | Capacity is now a count of live entries on every platform. B10's token clause applies to M and L only. B21 uses `IsProcessInJob` against each of the registry's own jobs. |
| C-P3-6 | f9 change untraced; wrong path | **Closed** | The new requirement B7 traces it (A5, OQ2, the f9 report). The path resolves at `/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md`, and its section "Product classification limitation" is at :296. |
| C-P3-7 | OD-B2 example was not an instance; 1.0.18 cited at main | **Closed** | §2 lists six gwz-core production hostless backends. Every other site is `cfg(test)`: I checked `status/workspace_path.rs`, `historical_identity.rs`, `conf_gate.rs`, `git_config.rs`, `handle_remote_identity.rs` and `transport_capabilities.rs`. None of the six reaches a spawn site. 1.0.18 is now cited at the tag. |
| C-P3-8 | B5 settled what A's O2 offered | **Closed** | B5 cites decisions (i) and (ii) (:1056-1060). |
| S-P2-1 | macOS `waitid(WNOWAIT)` returns on a stop | **Closed** | §3.3 uses `waitid` on Linux and kqueue `NOTE_EXIT` on macOS, with an `ESRCH` path; "a stop … is never an exit". §3.4's watch thread uses the same method. B25 and R-B2 cover it. Go's comment is verified. |
| S-P2-2 | External reaper exposes gwz-side kills | **Closed** | §3.3, R-B1 and B4 now name gwz's own kills. The README row says "unsupported". B11 claims only what holds. |
| S-P2-3 | Orderly exit SIGKILLs a `Wait` entry | **Closed** | `close()` and the last drop now wait for `Wait` entries. R-B4 adds `os._exit` and a foreign `exit()`. R-B17 and B26 are present. §3.7 cites gwz-cli's 16 exit sites; I checked each at `c0ba717` (`:162, :175-177, :187, :207, :350` come before any command; the rest come after its return). |
| S-P2-4 | OD-B1's cost incomplete | **Closed** | OD-B1 lists hung hooks and signers, the wait through Force, `index.lock` against 1.0.18's SIGINT cleanup, and Windows. Safety's (c) is recorded as not chosen under (ii). B15 has the never-exiting `pre-commit` leg. |
| S-P2-5 | Reaper cost in embedders unstated | **Closed, except the message leg** | §3.4 "Cost", R-B16, the 3.4 gwz-py measurement and README row 6 are present. The closure test's user-visible message in gwz-py (B19) cannot pass as designed; that part is carried as **N-P2-1**. |
| S-P3-1 | Windows suspended orphan window | **Closed** (listing) | R-B14 and B12 are present. The mechanism text conflicts with §3.1 (N-P3-2). |
| S-P3-2 | Descriptor closing unbounded | **Closed** | §3.4 lists descriptors with `close_range`, `getdents64` or `PROC_PIDLISTFDS`. B4 covers it. |
| S-P3-3 | Lifeline protocol under-specified | **Closed** | 64-bit tokens never reused; `EINTR` retried; a full table makes the reaper `_exit(1)`; the core owns the write end. B7 covers it. A related gap remains: N-P3-3. |
| S-P3-4 | Fork check incomplete | **Closed** | §3.1: the check comes first, before any lock, in every method and every `Drop`. B9 is extended. |
| S-P3-5 | `pkill -9 gwz` kills the reaper | **Closed** | R-B2's "When" covers it. |
| S-P3-6 | Gate seam implicit | **Closed** | `Spec` is named at each site. The crate's `Child` has no std `wait`. |
| S-P3-7 | `Session` killed by PID | **Closed** | §3.3 and §3.4 use `kill(-pgid)` for `Group` and `Session`. B24 covers it. |
| S-P3-8 | f9 untraced, 30 s unsourced | **Closed** | B16 cites `runner_tests.rs:299` (verified: `Duration::from_secs(30)`) and `owner.rs:77` (verified). |
| S-P3-9 | Reaper snapshot of credentials | **Closed** | §3.1 states "Creation precedes credentials". `spawn_gate()` carries no registry. The minting check and the 2.3r-2 brief cover it. |
| S-P3-10 | macOS `OwnInput` end-of-file window | **Closed** (recorded) | R-B15 is handed to C. |

## 2. New findings

### N-P2-1: The frozen `RegistryFault` contract does not reach the member at any of the three spawn sites; B19 cannot pass; the R-B2 and R-B16 recognizers fail for the helper

- **Root cause (not architectural):**
  - §3.1 freezes this rule: "A `RegistryFault` reaches the member as `ErrorCode::Io` with exactly one message".
  - B never says how the fault crosses the `io::Result` interfaces that sit between the registry and the member.
  - It leaves the existing error mapping outside 2.3r-2's Owns.
- **Location:** §3.1 "API"; §3.6 ("`runner.rs:123-141` … unchanged"); §4 2.3r-2 Owns; row B19; R-B2 and R-B16 recognizers ("Spawns fail naming the reaper", "The refusal names the reaper and `fork`").
- **Violated invariant:** a frozen SURFACE claim must be deliverable by the owned changes. Each residual needs a working recognizer. Each row must be satisfiable.
- **Evidence:**

  | Spawn site | Code at the pin | What the user sees | Who owns the mapping |
  |---|---|---|---|
  | Helper | `runner.rs:115-121` maps any `process_tree::spawn` error except `NotFound` and `ArgumentListTooLong` to `AuthError::SpawnFailed` | `ErrorCode::Unavailable` (`https_auth.rs:175`), message "HTTPS credential helper could not be started" (`:141`); the reaper is never named | runner.rs is not in 2.3r-2's Owns |
  | CLI-route `git commit`/`git tag` | gwz-git `cli.rs:17-21` wraps the error as `Error::operation("failed to run git commit: …")`; `verify_checkout_state.rs:124-129` turns that into `ErrorCode::GitCommandFailed` | `GitCommandFailed`, not `Io` | gwz-git is reference-only |
  | `rev-list` | `commit_log/mod.rs:358-361` turns it into a `HistoryUnreadable` degradation | a degradation, not a member error | — |

- **Reproduction:**
  1. Build B as written.
  2. In gwz-py on M or L, SIGKILL the reaper.
  3. Run a fetch that needs a helper.
  4. Python receives `Unavailable`, "HTTPS credential helper could not be started". B19 expects `ErrorCode::Io` with "child-process reaper unavailable: exited".
- **Impact:**
  - A misleading diagnosis in exactly the cases B prices: strict overcommit (R-B16), a reaper killed by name (R-B2), and OOM.
  - The SURFACE contract is frozen yet cannot be delivered.
  - B19 is red with no owned fix.
  - "Exactly one message" is also ambiguous about which `ErrorCode` is meant: the transport `ErrorCode::Io` (`https_auth.rs:190`) or the model `ErrorCode::IoError` (`verify_checkout_state.rs:133`).
- **Correction:**
  - State how a `RegistryFault` travels: for example `io::Error::other(fault)`, downcast at each mapping.
  - Add `runner.rs`'s spawn-error mapping to 2.3r-2's Owns, with a new `AuthError` variant that maps to `Io` and carries the fault's message.
  - State, per site, what the member receives (code and message). For the CLI route through gwz-git, either accept the prefixed `GitCommandFailed` and say so, or have `model_error` recognise the fault. For `rev-list`, state the degradation.
  - Say which `ErrorCode` enum §3.1 means.
- **Closure test:**
  - §3.1 lists each site's code and message.
  - A 2.3r-2 row (M, L): with the reaper SIGKILLed, a helper lookup, a CLI-route commit and a path-filtered log each show the fault's text with the stated code.
  - B19's expectation matches that table.

### N-P3-1: 2.3r-1 cannot pass the existing gates with the files it owns; "shares only `Cargo.lock`" is false

- **Location:** §4 2.3r-1 Owns and "Waves"; §8 row 1; §3.7 ("one entry each for `crates/children`").
- **Evidence:**
  - Root `MODULE.bazel:73-96` lists every `gwz-core/crates/*` manifest for `@gwz_core_crates`; the last crate addition, `1ccb5c1b`, edited it. A new crate's `all_crate_deps()` needs `MODULE.bazel` and `MODULE.bazel.lock`, which are root-repository files shared by every lane.
  - `check_local_clone_boundaries.py` LBT-001: "every package directory under `crates/` is classified" in `local_clone_inventory.json`, with role and allowed third-party edges (here `libc`, `windows-sys`, `tokio`).
  - `check_crate_versions.py` requires the lockstep version and publish metadata for every crate under `crates/`.
  - `check_process_globals.py` already roots at `crates/*/src/lib.rs` and flags `libc::signal`, `libc::sigaction` and `libc::chdir` (`LIBC_FUNCS`, :103-104). The reaper and child setup call all three. The allowlist, however, is owned by 2.3r-2, so 2.3r-1's own commit fails that gate. "One entry each" also undercounts: the reaper's `chdir("/")` needs an entry, and entries are keyed by path, kind and name.
- **Impact:** a red per-commit lane gate at 2.3r-1, and undeclared shared files in wave 2.
- **Correction:**
  - Add to 2.3r-1's Owns: root `MODULE.bazel` and `MODULE.bazel.lock`, `scripts/checks/local_clone_inventory.json`, and the crate's `process_globals_allowlist.json` entries.
  - Add gwz-core's edge to the crate (inventory) to 2.3r-2.
  - Restate the shared-file sentence and §8 rows 1 and 3.
- **Closure test:** §4 and §8 row 1 list these files, and the shared-file sentence names them.

### N-P3-2: `close()` "takes the gate's hold" contradicts §3.1, and the registry has no stated public control path

- **Location:** §3.1 "Value" (the gate holds the registry, "so no cycle forms"; the crate never depends on gwz-core); §3.2 ("`close()` takes the gate's hold"); B12 (a crate harness row); §3.7 and §3.8 (A, gwz-cli and gwz-py call `kill_killable()` and `close()`).
- **Root cause:**
  - `crates/children` cannot take gwz-core's `SpawnGate` hold. B12 runs in the crate's harness, where no gwz-core gate exists.
  - `ProcessGate`, which `process_safety_with_children()` returns, exposes only a `pub(crate) gate()` (`composition.rs:20-28`). Nothing gives gwz-cli (A's Force) or gwz-py (3.2's close) a public path to `kill_killable()` or `close()`.
- **Correction:**
  - State that `close()` takes the registry's own creation lock, which every Windows creation holds through its job assignment. That is what B12 can test.
  - Name the public methods 2.3r-2 adds to `ProcessGate` (in `composition.rs`, already owned) for `kill_killable()` and `close()`.
- **Closure test:** §3.1, §3.2 and §3.7 name one lock and one public surface. A's call sites (steps 4 and 6) and 3.2's close resolve to that surface.

### N-P3-3: A writer-thread death leaves the release order undefined

- **Location:** §3.3 "Order" (the waiter "has the writer thread write the unregister record, waits for its acknowledgement, and reaps"); §3.3 "The writer thread" (its death "sets `ReaperGone` and closes nothing"); §3.4 "Its end" (`close()` waits without a bound for `Wait` entries); B7.
- **Root cause:** after the writer dies, no acknowledgement can arrive. B does not say whether the waiter then:
  - blocks, so zombies accumulate and `close()` and Force step 6 can hang; or
  - reaps without unregistering, which leaves the still-live reaper holding a freed ID and breaks B4.
- **Correction:**
  - State the rule. For example: the waiter never reaps an entry whose unregister record was not written, and that entry counts as released for `close()`. Name the residual for gwz's later exit.
  - Extend B7's panic leg to a live `Wait` entry and a live `Kill` entry.
- **Closure test:** B7 asserts that after a writer panic, `close()` returns and no kill reaches a reaped ID.

### N-P3-4: Requirement IDs and row IDs share one namespace

- **Location:** §1 B1-B8 (requirements); §5 B1-B26 (rows); §2, §3.3, §3.4, §3.6 and §9 references.
- **Evidence:** "B7" is the cleanup-outcome requirement in §2 and §3.6, and the descriptor-0-2 row in §9 (via S-P3-3). "B5" is "Writes finish" in §3.4, and the `ENOMEM` row in §9 (via S-P2-5). B4 quotes "B1's 'every helper process it started'", which B1 no longer says; the phrase is D-V :1006, not :1008.
- **Correction:** prefix the rows (for example `L1`-`L26`) or the requirements, and fix B4's quote and line.
- **Closure test:** each ID has one meaning in the document.

### N-P3-5: Some legs of B18 and B19 are marked **(reg)** although they are red before their step

- **Location:** rows B18 and B19; the §5 legend ("red first unless a leg is (reg)").
- **Evidence:**
  - Before 3.1, gwz-cli's backends are hostless and unregistered (§4 "Interim"). SIGKILL of gwz leaves a sleeping hook and `rev-list` running on M and L, and leaves `git commit` and `rev-list` running on W (no job). B18's legs are red first.
  - The same holds for B19's M and L SIGKILL leg before 3.2.
  - Only the Windows helper legs (its own job today) are true regressions.
- **Correction:** mark **(reg)** only on the W helper legs.
- **Closure test:** every (reg) leg passes at the step's parent commit.

### N-P3-6: §8 row 6 applies README text "at acceptance", while §4 assigns it to 3.2

- **Location:** §8 heading ("applied by the lane owner at acceptance"), row 6; §4 3.2 ("README text").
- **Impact:** applied at design acceptance, gwz-py's public README would describe a reaper and refusals that do not exist until 3.2.
- **Correction:** mark row 6 "applied by step 3.2", or move it out of §8.
- **Closure test:** one owner and one time for the README text.

### N-P3-7: The superseded-clause note omits r1 OD-C2 (a)'s reaper pricing

- **Location:** §8 closing paragraph.
- **Evidence:**
  - D-V :1030-1031 adopts OD-C1 to OD-C6 as recommended. OD-C2 (a) (`3448c561` :244-245) prices the reaper as "one extra process per gwz process that starts children".
  - OD-B3 (a), decided at (iii), replaces that with "every command". B's r1 said so; r2 dropped the statement.
- **Correction:** add OD-C2 (a)'s pricing to the history-unedited list, as superseded by OD-B3 (a).
- **Closure test:** the note names it.

### N-P3-8: OD-B2 (a)'s "as today" narrows decision (ii) for registry-less gates without recording the alternative

- **Location:** §1 B5 "Not covered"; §6 OD-B2; R-B13.
- **Evidence:**
  - Decision (ii) (:1059-1061) says `git commit` and `git tag` "are let finish in their own process group", with no condition.
  - B reads OD-B2 (a)'s "spawn as today" as also keeping gwz's group. Under that reading the terminal's Ctrl-C reaches `git` mid-write in the non-candidate build and under `DriverOnly`.
  - r1's framing supports B's reading, so this is not a contradiction. Still, an own group without a registry (round-1 C-P2-2's alternative) would meet (ii) literally. Its cost is a `git` that outlives gwz to finish its write, with no reaper.
  - The RemPlan declines it, but §6 does not record it.
- **Correction:** add it to OD-B2's "Not chosen" with its cost, so the operator sees the trade.
- **Closure test:** OD-B2 names it.

### N-P3-9: The Windows `Process` target differs from Unix in two ways B does not state

- **Location:** §3.2 ("`Process` and `Session` add `CREATE_NEW_PROCESS_GROUP`"; "Windows `ignore_interrupts`"); R-B7.
- **Evidence:**
  - `CREATE_NEW_PROCESS_GROUP` disables CTRL_C whatever `ignore_interrupts` says, so `Process { ignore_interrupts: false }` ignores Ctrl-C on W but not on Unix.
  - A registered `Process` entry's descendants are in its kill-on-close job on W, while Unix kills by PID only. R-B7's recognizer ("In no group or job of gwz's") is therefore wrong on W for, say, a registered `ssh`'s `ProxyCommand`.
- **Correction:** state both differences in §3.2, and scope R-B7's recognizer to M and L for `Process` descendants.
- **Closure test:** §3.2 and R-B7 name them.

**New architectural root causes: none.** Each finding closes with text, an Owns list, or a local error mapping.

## 3. Verified citations

At the pins, unless a finding above says otherwise:

- **D-V:**
  - :999-1062; OQ2 :1005-1010
  - resolutions (1)-(3) :1035-1045, with (2) at :1039-1043 and (3) at :1043-1045
  - the split :1046-1054
  - decisions (i) :1056-1059, (ii) :1059-1061, (iii) :1061-1062
  - B4's :1008 should be :1006 (N-P3-4)
- **Plan:** §4 trunk ownership :324-331 (Phase 5 owns `native_commit`, `refs.rs` and `repository.rs`, none of which 2.3r-2 touches); 3.1 :721-728; 3.2 :729-735; 3.3 :736-738; 3.4 :739.
- **2.3 plan:**
  - 2.3j "After: 2.3g" :202; 2.3l :220-230; waves :237-242; review and totals :249-250
  - 5,250 = 4,300 + 600 + 350; 5,710 = 4,750 + 600 + 360; 14 steps
  - No 2.3 sub-step names any file in 2.3r-2's Owns. 2.3h edits `remote_lane.rs`, `served/*` and `service.rs`, not `composition.rs`.
- **History:**
  - Safety (amendment) P3-3 :256-263 and P2-4 :109
  - parked :808, :809, :1212, :1218, :1224
  - contract :289; gwz-py README "Platform And Status" near :163
- **gwz-core:**
  - `runner.rs:110` (and :108-121, N-P2-1)
  - `process_tree.rs` :42, :57-75, :71, :79-86, :89-90, :18-24
  - `windows.rs` :44-48, :54-62, :153-183
  - `owner.rs` :77, :97, :253-277, :298
  - `https_auth.rs` :28, :190; `https_worker.rs` :303, :377
  - `spawn_gate.rs` :20-21, :431-436, :438-446, :450-454
  - `composition.rs` :30-35, :37-40
  - `lib.rs:47-51`
  - `driver_only.rs` :61, :152
  - `local_command.rs:79`; `cancellable.rs:238`
  - `backend.rs` :69, :114, :118-125
  - `commit_log/mod.rs` :330-358, :337, :347, :354-356
  - `Cargo.toml` :50, :139, :151-152
  - `endpoint/mod.rs:130-137` (`ssh_controlled_server` is `cfg(test)`)
  - the three test-only `tokio::process` files
  - only `gwz-py/Cargo.toml:24` also names tokio, without `process`
- **Members:**
  - gwz-cli `lib.rs` exit sites (all 16)
  - gwz-cli hostless sites `dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187` (complete)
  - `v1.0.18` `repository.rs:299-305` and `refs.rs:172-183`
- **Toolchains:**
  - std `unix.rs` :141-149, :342, :352-372, :383-385, :408, :463
  - std `pipe/unix.rs` :28, :34-40
  - std `os/unix/process.rs:220-221` (unstable `setsid`)
  - std `os/windows/process.rs` :37-39, :181-182
  - tokio `pipe.rs` :419, :967
  - Go `wait_waitid.go:5-6`
- **A r1 hand-off points:**
  - Force order :206-209 (steps 4, 6, 8) matches §3.7.
  - :216-217 differs on `Kill` spawns after step 4 (§4 below).
  - A §4 :271-273 matches B §3.7, "no disposition".
  - A §3.3 :155-160 relies on B's `terminate` and `owner.rs:298`.

## 4. Risks and next action

- **Revision needed.** r3 closes N-P2-1:
  - the fault's transport and the per-site code and message table;
  - `runner.rs`'s mapping added to 2.3r-2's Owns;
  - B19 aligned with that table.

  The P3s are cheap text and Owns edits. N-P3-1 avoids a red gate at 2.3r-1, so do it before briefing that lane.
- **Hand-off to A.** A's current file (r1 :216-217) still says a `Kill` child started after step 4 "is ended by B's reaper when the process exits". B's `Closed` refusal follows decision (i), "at once", so the correction belongs to A r2 (RemPlan hand-off 7). The hand-off is clean only when A r2 drops that sentence and states the member report.

  Whatever A r2 says about this, the parked thread blocks for good under A's `outside` (A :186), so the report is moot for gwz-cli.
- **Open (OS) facts.** The B6 and B25 legs on macOS (`proc_pidinfo` in the forked reaper; kqueue registration on a zombie) are hypotheses until those rows run.
- **Row satisfiability.**
  - B1-B17 and B20-B26 can be met on the platforms they name, given N-P2-1 (B19) and N-P3-5 (B18 and B19 labels).
  - B4's "L without `close_range`" needs a test seam on modern kernels (WSL and weftpi have `close_range`). It should be named.

## SURFACE

What B freezes that users see:

- **Errors.** Five `RegistryFault` messages under "`ErrorCode::Io`":
  - `child-process reaper unavailable: <cause>`
  - `child-process limit reached (256)`
  - `child processes refused in a forked copy`
  - `child processes refused: closing`
  - `child-process mode unsupported on this platform`

  Not deliverable as written (N-P2-1). The `ReaperGone` cause (`exited` or `lifeline: <e>`) depends on whether the watch thread or a spawn's `EPIPE` sees the death first.
- **Outcome classification (f9).** A cancelled or timed-out helper lookup whose cleanup outlasts 500 ms reports `Cancelled`, or `Timeout` with `Interaction`, instead of `Io`. The pending tree is counted, not reported as the member's error.
- **Processes.**
  - Every candidate gwz command, and every Python process that makes gwz-py's composition, has one extra process: a fork in its own session with the parent's name, so `pgrep` and `pkill gwz` match it.
  - On Windows, every registered child sits in a kill-on-close job.
- **Commit and tag (OD-B1 (a)).**
  - Hooks and signers no longer receive the terminal's Ctrl-C.
  - Unix: a terminal reader stops (SIGTTIN or SIGTTOU).
  - Windows: hooks and signers have no console.
  - `gwz commit` waits through Cancel and Force. An interrupt after Force, or SIGKILL, leaves `index.lock`, where 1.0.18's Ctrl-C let `git` clean up.
- **gwz-py.**
  - The registry's close waits for a running commit, and later members are refused with `Closed`.
  - Spawns are refused after `os.fork()` without exec, and under strict overcommit.
  - The README text of §8 row 6 is fixed, including "`forkserver` without preloading gwz".
- **Exit status:** none fixed by B (that is A's).
