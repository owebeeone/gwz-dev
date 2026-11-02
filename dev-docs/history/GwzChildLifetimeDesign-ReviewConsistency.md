# Cancellation design B r1 — Consistency-AXIS REVIEW

**Review object:** /Volumes/projects/limbo/gwz-dev/dev-docs/GwzChildLifetimeDesign-DRAFT.md (hash at start `48b992d945f9570730103ae80f5c4a438dcaf504`, at end `48b992d945f9570730103ae80f5c4a438dcaf504`, unchanged) | **Date:** 2026-11-01 | **Axis:** I checked the design against D-V, the plan, the 2.3 plan, r1, the Safety review, the parked design, TR1.6 and the code at the pinned commits. I also checked its §8 table, whether its rows can run on M, L and W, and its hand-offs to A, C and D.

**Verdict: NO-GO** — 0 P0, 0 P1, 2 P2, 8 P3. **Pre-commit:** GO on a revision that closes P2-1 and P2-2 with the closure tests given below and adds no new P0-P2. The P3s are recommended but do not block.

## 0. Evidence base

- **Checkouts.** Every member checkout matches the design's pin with a clean tree: root `b4b827d6`, gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-sspi `6f83551`, git2-rs `d13951f`.
- **r1.** Read from blob `3448c561` with `git cat-file`.
- **Toolchain sources.** Rust std 1.95.0 from `~/.rustup/toolchains/1.95.0-aarch64-apple-darwin/lib/rustlib/src/rust/library/std/src`. tokio 1.53.1 from `~/.cargo/registry/src/index.crates.io-*/tokio-1.53.1`.
- **1.0.18.** Checked at gwz-core tag `v1.0.18`.
- **Design A.** I read `GwzCancelInterruptDesign-DRAFT.md`, an untracked draft, only to check B's hand-offs. I did not review it.
- **Method.** Every row about behaviour was checked by reading only. I ran no builds or tests.

## Problem, requirement provenance, and scope

B covers three things for gwz-core: the child registry, the Unix reaper and Windows jobs, and how each kind of process gwz-core starts is ended.

Requirements B1-B6 trace to the following D-V text, and the quotes are exact:

| Req | Plan lines | What it traces to |
|---|---|---|
| B1 | :1005-1008 | OQ2 |
| B2 | :1007 | death detection |
| B3 | :1039-1043 | resolution (2) |
| B4 | :1008 | "no … partial state" |
| B5 | :1035-1037 | resolution (1) |
| B6 | :1043 | resolution (3) |

B7 traces to the standing rules and to r1 OD-C1.

- **Mechanisms that need their own provenance.**
  - The f9 change in §3.6 is backed by no B requirement (P3-6).
  - B5 states as settled what A's O2 still offers as an alternative (P3-8).
  - §8 row 5 settles a C mechanism (P3-3).
- **Scope creep.** I found no scope creep into new subsystems.
- **Narrowing of the requirement.**
  - OQ2's "no orphaned process" after Ctrl-C is narrowed without saying so for children the registry does not start (P2-2).
  - On Windows, OD-B1 (a) changes 1.0.18 behaviour without stating the cost (P2-1).
- **Placement.** It matches D-V (3) and the 2.3 plan:
  - 2.3r-1 and 2.3r-2 come before 2.3j.
  - The waves fit three lanes.
  - The new files do not overlap any file owned by 2.3e-2.3l (2.3 plan :240-242) or by the Phase 1 trunk (plan :324-326).
  - Wave 1 has landed and wave 2 has not started (gwz-core log), so the wave-2 slot is still free.

## 1. Findings

### P2-1 OD-B1 (a) leaves out its Windows cost; R-B10's recognizer works only on Unix

- **Location:** §3.2 Windows ("`Group` adds `CREATE_NO_WINDOW`"); §3.6 row "`git commit`, `git tag`" (`Group`, `Wait`); §6 OD-B1 (a); §7 R-B10; row B15 W leg.
- **Violated invariant:** an operator decision must list its cost on every platform it names, so the operator decides on complete information (B7 parity). Each residual must also have a recognizer.
- **Root cause:**
  - On Windows, OD-B1 (a) starts the Git CLI route's `git commit`/`git tag` and their hooks with `CREATE_NO_WINDOW`, so they are not attached to gwz's console. That is exactly why B15's W leg ("CTRL_BREAK reaches neither") passes.
  - In 1.0.18 the same `git commit`/`git tag` ran through plain `Command::output()` (verified at `v1.0.18:src/git/gitbackend/repository.rs:301` and `refs.rs` `tag_create`). It shared gwz's console, so a console prompt reached the user.
  - The cost text in OD-B1 (a) covers only Unix: SIGTTIN, and the `ps` `T` state in R-B10.
  - Unix also has a further stop case. A passphrase prompt that turns off echo calls `tcsetattr` and stops on SIGTTOU before reading. This does not change the conclusion.
- **Reproduction:**
  - Windows, candidate build after 2.3r-2.
  - A signing setup with no agent, for example `ssh-keygen -Y sign` with a passphrase-protected key, or a `pre-commit` hook that reads `CONIN$`.
  - Run `gwz commit -m x`. The prompt goes to a console that is not gwz's and that nobody can see. The command waits with no bound, or fails if the program finds no console.
  - Because the child is a `Wait` entry, the forced exit also waits for it (A step 5, "waited for by every cancel and by the forced exit"), so Ctrl-C cannot end the command.
  - No Windows recognizer exists.
- **Impact:** a change from 1.0.18, the parity reference under D-V, on Windows, with no cost stated, no recognizer and no row. Diagnosability and parity defect.
- **Correction:**
  - State the Windows cost in OD-B1 (a): a console prompt is invisible, and Ctrl-C cannot end the wait.
  - Give R-B10 a Windows recognizer.
  - Optionally offer a Windows variant for the operator: keep the console and add `CREATE_NEW_PROCESS_GROUP`, so CTRL_C is disabled but a prompt is visible. State its CTRL_BREAK/CTRL_CLOSE cost, the difference §3.2 already describes.
- **Closure test:** OD-B1 and R-B10 name the Windows behaviour. B15 gains a W leg with a hook that reads the console and either records the observed hang or failure as the accepted cost, or shows the alternative's prompt reaching the console.

### P2-2 The residuals for children the registry does not start consider only gwz's death, not Ctrl-C or the forced exit

- **Location:** §1 B1, B5; §3.6 "Not gwz-core's children" ("native route helpers … stay as they are"); §6 OD-B2 (a); §7 R-B5 and R-B13 (both "When: gwz is SIGKILLed").
- **Violated invariant:**
  - D-V OQ2 (:1005-1008): "Ctrl-C cancels … every helper process it started … no orphaned process … or partial state".
  - B5: a write child is not killed while gwz lives.
  - D's residual list must be exact, because the operator accepts what it says (D-V split, :1052).
- **Root cause:** the residual analysis of unregistered children only asks what happens when gwz dies. While gwz lives, the terminal's interrupt and A's forced exit (`_exit` at A step 8) act on the same children, and B names neither.
- **Interleavings:**
  1. **Native route helper, Windows.** The transport is off, or the build is ordinary. git2-rs starts the helper with `CREATE_NO_WINDOW` (`git2-rs/src/cred.rs:391-392, :408-410`), so a console Ctrl-C never reaches it.
     - The user presses Ctrl-C, then Ctrl-C again. A's Force exits the process (A R10: "Force at the bound ends it").
     - A Git Credential Manager prompt or a helper waiting for the user keeps running with no parent. This is the very case OQ2 refuses.
     - R-B5 lists only SIGKILL, and A's R10 does not name the leftover helper either.
  2. **A gate with no registry: the non-candidate build, OD-B2 (a)'s hostless backends, and gwz-cli/gwz-py until 3.1/3.2.** The Git CLI route's `git commit` runs in gwz's own process group, as `SpawnGate::output` does today with no group set. The terminal's SIGINT reaches `git` and its hooks in the middle of the write while gwz is alive, which contradicts B5. R-B13 lists only SIGKILL.
- **Impact:** the residuals handed to D understate what is left. The operator would accept an orphan after Ctrl-C and a torn hook write that are not on the list. This narrows OQ2 without saying so.
- **Correction:**
  - Extend the When column of R-B5 and R-B13 to cover Ctrl-C and A's forced exit.
  - Add to OD-B2 (a)'s cost that the terminal's interrupt reaches a registry-less `git commit` and its hooks.
  - Make B1 list its exceptions instead of saying "every".
  - Or put the CLI route into its own group even without a registry, which is independent of registration.
- **Closure test:** R-B5 and R-B13 name the Ctrl-C and Force paths with their platforms, and OD-B2 states the cost. Optionally, a row: a registry-less gate, SIGINT to the harness's group during a sleeping `pre-commit`, with the observed outcome recorded as the accepted residual.

### P3-1 §8 row 8 adds a false "only" to the contract

- **Location:** §8 row 8 (amending `GwzConnectionApiContract.md:289`): "the child registry, the only starter and kill authority of gwz-core's child processes".
- **Violated invariant:** an amended contract clause must agree with the design that amends it.
- **Evidence:** B itself keeps several starters outside the registry:
  - the Windows helper's own launch (§3.2, §3.6 "Windows arm unchanged");
  - the gwz-sspi worker (its supervisor);
  - native route helpers (git2-rs);
  - every gate with no registry (OD-B2 (a), R-B13).
- **Impact:** C and D would read a frozen contract that claims more coverage than exists.
- **Correction:** "the starter and kill authority of every child a gate with a registry starts, except those §3.6 lists".
- **Closure test:** row 8's text names the exceptions or points to §3.6.

### P3-2 On Windows, the helper's "`Group`, `Kill`" row is not a registry entry, so `kill_killable()` misses it

- **Location:** §3.6 table, helper row; §3.7 "`kill_killable()` kills every live `Kill` entry"; §1 B7 ("except where §3.2 says").
- **Root cause:** the helper's Windows arm keeps its own job (`process_tree/windows.rs:54-62`), so the registry does not hold it. A's Force step 4 (A :214) relies on `kill_killable()` to end every `Kill` child at once.
- **Impact:** on Windows the helper ends at the lookup's own cancel or when the process exits, not at step 4. That is a platform difference §3.2 does not list. The impact is limited because Cancel comes before Force.
- **Correction:** state the difference in §3.2 and §3.7, or let the registry also kill jobs the helper's owner holds.
- **Closure test:** the text names the difference, or B22's W leg includes a Windows helper.

### P3-3 §8 row 5 settles C's mechanism, and cannot be built at 2.3k's place in the order

- **Location:** §8 row 5 (2.3k, background copy through "`Session` … `Detach`") against §0 ("which child takes which target and ending … C") and §3.7 ("Which child takes which … are C's").
- **Root cause:** this is an internal contradiction, and the plan amendment cannot be built as written:
  - 2.3k is a gwz-cli step in wave 4, before 3.1, so gwz-cli's `run` has no registry yet.
  - `ProcessGate` exposes no spawn, `SpawnGate` is `pub(crate)`, and the minting check allows `ChildRegistry::create` only in `composition.rs`.
- **Correction:** drop row 5 and leave a hand-off note for C. Or state the public spawn path and which registry 2.3k uses.
- **Closure test:** the §8 rows decide nothing that §0 assigns to C.

### P3-4 "Their callers do not change" contradicts the per-site specs and 2.3r-2's file list

- **Location:** §3.1 "Where it comes from"; §3.6 table; §4 2.3r-2 Owns (`commit_log/mod.rs (:330-358)`).
- **Root cause:**
  - `SpawnGate::spawn`/`output` take no `Spec`, but `rev-list` must be (`Group`, `Kill`) and `git commit` (`Group`, `Wait`).
  - `commit_log/mod.rs:354-356` calls std's `Child::wait_with_output` on what `spawn` returns, so a registry `Child` changes the call site.
- **Correction:** state how `spawn` and `output` map to a spec, or add a `Spec` parameter and list the callers that change.
- **Closure test:** §3.1 and §4 agree on which callers change.

### P3-5 Two rows that claim W cannot run as written on Windows (PLAUSIBLE for B21)

- **Location:** rows B10 and B21; §3.1 "Capacity".
- **B10 (M, L, W):**
  - It uses words that exist only on Unix: tokens, unregister records.
  - Capacity is defined against "the reaper's preallocated table", which Windows does not have. Its W leg is undefined.
- **B21 W "`IsProcessInJob` false":**
  - With a NULL job handle, this asks whether the process is in any job.
  - Under a harness that itself runs in a job (likely on hosted CI runners), the child inherits that job and the leg fails although the design holds.
- **Correction:**
  - Define capacity and B10's W leg for Windows.
  - Make B21 check that the child is not in a job the registry made.
- **Closure test:** each W leg can be stated with Windows mechanisms only.

### P3-6 The f9 change has no B requirement, and the f9 report's path is wrong

- **Location:** §2 "Cleanup classification (f9)"; §3.6 "Ending never changes the outcome".
- **Provenance:**
  - Changing `terminate` so it returns `Cancelled` or `Timeout` instead of `CleanupPending` (that is, `ErrorCode::Io`) traces to none of B1-B7.
  - It belongs with A's "what a cancelled member reports" (§0). A depends on it (A :110-111, step 3.0e), so the hand-off is consistent, but B gives no reason it is in B.
  - Unlike the TR1.6 lines, the superseded review clause (`ReviewCode-2.md:66`) is noted only in prose, not as a table row.
- **Citation:** under B's own path rule, `build-targets/winflk-logs/f9-report.md` resolves inside gwz-core, where it does not exist. It is at `/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md` (:140-147, which I verified).
- **Correction:**
  - Add a requirement row that traces f9 to A's requirement and OQ2's "cancels end to end".
  - Fix the path.
- **Closure test:** §1 traces §3.6's f9 bullet, and the path resolves.

### P3-7 OD-B2's example is not an instance, and a 1.0.18 claim is cited at main

- **OD-B2:**
  - OD-B2 (a) offers `create_workspace.rs:18` as an example of a gwz-core hostless backend "that commit[s] by the CLI route or walk[s] path-filtered history". That handler does neither (verified at `:14-40`).
  - The design does not list which gwz-core-internal hostless backends (18 production files) reach the two spawn sites, so the operator cannot judge how large R-B13 is.
- **The 1.0.18 claim:** OD-B1's "1.0.18 ran it in gwz's group" cites `spawn_gate.rs:431-436` at main. The claim is true, but at `v1.0.18:src/git/gitbackend/repository.rs:301` and `refs.rs`.
- **Correction:** list the internal sites that reach the spawns (or state that none do), and cite the v1.0.18 lines.
- **Closure test:** OD-B2's example spawns, and the 1.0.18 citation is at the tag.

### P3-8 B5 states as settled what A's O2 still offers as an alternative

- **Location:** §1 B5; §6 (no operator decision covers it); A §8 O2 ("Alternative: kill them at Force step 4").
- **Root cause:**
  - Resolution (1) says both "the forced exit kills children … at once" and "a local write sequence … runs to its next safe point".
  - B5 cites only the second half and treats it as a requirement, while A presents killing at step 4 as an operator alternative.
- **Impact:** if the operator picks A's alternative, B's `Wait` ending, `kill_killable`, B15 and B22 all change, and B has no fallback.
- **Correction:** mark B5 as depending on A O2 (or OD-B1), or cite the whole of resolution (1) and the reading B takes.
- **Closure test:** B5's provenance names the decision it depends on.

## 2. Verified citations

These were accurate at the pinned commits unless a finding says otherwise.

- **Plan and D-V.**
  - Plan :999-1054 is D-V; the quotes at :1005-1008, :1035-1043 and :1046-1054 are exact.
  - The plan's 3.1, 3.2 and 3.4 are at :721-728, :729-735 and :739.
  - The Phase 1 trunk's file list is at :324-326, and "ask, do not edit" at :330-331.
- **2.3 plan.**
  - 2.3j "After: 2.3g" is at :202, the 2.3k background copy at :212-213, 2.3l at :220-230, and waves 2 and 3 at :240-241.
  - "Review: dual on a, d, j, l" and the totals are at :249-250. The new totals check: 4,300+850=5,150; 4,750+860=5,610; 12+2=14.
- **r1.** r1 :148-149 (Windows helper job); OD-C1 :241-244; OD-C2 (a) :245-246 is quoted exactly; OD-C3 :248; OD-C6 :257-258.
- **Safety review.** P2-3 is at :92 (with the `tee` closure at :107), P2-4 at :109, and P3-3 at :256-263, whose correction "every registered child gets a job" matches B3.
- **Parked design.** :808 (Unix `setsid`; Windows `DETACHED_PROCESS` and a handle list), :809 (stdio child ignores CTRL_BREAK), :1212, :1218 (SIGINT and SIGQUIT ignored) and :1224 (end of pipe).
- **TR1.6.** :75 (a group per helper), :76 ("Its cost"), :82 (success leaves the child running), :398 (OQ2). `ReviewCode-2.md:66` holds the CleanupPending sentence.
- **Credential helper code (gwz-core).**
  - `process_tree.rs`: tokio Child at :42, `process_group(0)` at :71, kill at :79-86, a no-op `retire` at :89-90, spawn at :57-75.
  - `runner.rs:110`; `owner.rs` `terminate` at :253-277 (CleanupPending at :275) and `retain_pending` at :298.
  - `https_auth.rs` (500 ms grace at :28; Io mapping at :190); `https_worker.rs:377` (Interaction).
  - Windows arm: `windows.rs:54-80` and :115-118; `windows/launch.rs:1-3` and :224 (`CREATE_NO_WINDOW`).
- **Other spawn sites and composition (gwz-core).**
  - `spawn_gate.rs`: :1-11, :153-155, :431-436, :438-446, :450-454.
  - `composition.rs`: :30-35, :37-40.
  - `lib.rs:47-51`; `backend.rs`: :69, :114, :118-125.
  - `commit_log/mod.rs`: :330-358, :337, :347, :354.
  - `transport_support.rs:313-321`; `handshake_local_rows.rs:447`.
  - `Cargo.toml`: :50 (`crates/*`), :132-139, :152.
  - The three test files that use `tokio::process`, and `endpoint/mod.rs:130-137` (test-only).
  - `diff/output.rs:271` (test-only).
  - The only production spawns are the three sites named. gwz-core's crates have no production spawn.
  - Small gap: §2's list of `spawn_gate()` callers leaves out `transport_host/driver_only.rs:61`.
- **Other members.**
  - gwz-git `native_commit.rs:88-95` (stdin null, output piped) and `native_commit/cli.rs:38-44` (`commit -m`, so no editor).
  - gwz-cli `lib.rs:160-163`, `pager.rs:157-170`, `forall.rs:231-262`, `hook/env.rs:158-348`, `hook/fallback.rs:21-25`. The five hostless backend lines are at `dispatch.rs:93`, `log_exec.rs:118` and `hook/family.rs:63, :149, :187`.
  - gwz-py: 8 lines name `Git2Backend::new()`.
  - gwz-sspi `launch.rs:205-280` (`CREATE_SUSPENDED|CREATE_NO_WINDOW` at :212-213, job at :276-280), `platform.rs:1-9`, `handles.rs:50-56`.
  - git2-rs `cred.rs:124, :387-420`.
- **Rust std 1.95.0.**
  - `unix.rs`: `setpgid` :342; SIGPIPE reset :359-372; signal mask inherited :352-353; `pre_exec` closures :383-385; `execvp` :408; a closure forces the fork path :463; reap on exec failure :141-149.
  - `pipe/unix.rs`: :28 and :34-40.
  - Unstable APIs: `setsid` (`os/unix/process.rs:220-221`), `main_thread_handle` (:445-449), raw attribute lists (:342), and `inherit_handles` (:380).
  - Stable APIs: `creation_flags` :181-182 and `AsRawHandle` :36-39.
- **tokio 1.53.1.**
  - `process/unix/mod.rs:123-137` uses pidfd on Linux, otherwise a SIGCHLD handler.
  - `net/unix/pipe.rs` :419 and :967 are the `from_owned_fd` calls, which need only `net`.
  - No other manifest enables tokio's `process` (only gwz-core's Cargo.toml and `gwz-py/Cargo.toml:24` with rt-multi-thread and time), so B17's `no-dev` claim holds.
- **Other files.**
  - Contract `GwzConnectionApiContract.md:289`; `gwz-py/README.md:163`.
  - `BUILD.bazel` lists are at :18-32 and :36-41. B says :12-40, which is close enough.
  - `scripts/checks/{check_spawn_gate_minting.py, check_process_globals.py, process_globals_allowlist.json}` exist.

## 3. Risks and next action

- **Revision needed.** Revise B to r2 to close P2-1 (OD-B1's Windows cost and recognizer) and P2-2 (the Ctrl-C and Force paths for children outside the registry). The P3s are cheap edits to the text.
- **Rows I checked for satisfiability:**
  - B1-B9, B11-B16, B20, B22-B24 can run as written on their platforms. The `waitid(…WNOWAIT)` behaviour on macOS is a hypothesis that B3's M leg will test.
  - B10 and B21's W legs need P3-5's fixes.
- **The superseded-clause table.**
  - Rows 1-4 and 6-7 are exact against their anchors.
  - Row 5 conflicts with B's hand-off to C (P3-3), and row 8 overstates coverage (P3-1).
  - Rows 6 and 7 build in OD-B3 (a) and OD-B2 (a). Applying them "at acceptance" assumes the operator picks the recommended options; the table should say so.

## 4. Out-of-scope observations

- 2.3i's server installs a SIGINT/SIGTERM and Ctrl-Break handler in gwz-core (2.3 plan :185-186). r1 OD-C1 says gwz-core installs none. This is for A or C to reconcile; it is not B's defect.
- Under OD-B3 (a), the reaper is a forked copy with gwz's own command line, so `pgrep gwz` or `pkill -9 gwz` match it. If the reaper dies first, R-B2 follows. This is a hypothesis about how users and scripts behave, worth one line in D or a user note.
- §3.4 closes descriptors one by one "up to the limit" on macOS. If an embedding raises `RLIMIT_NOFILE` very high, the reaper's startup cost grows. This is a robustness question for 3.4's measurement, not this axis.

## SURFACE

User-visible behaviour B fixes:

- **Error classification.**
  - A cancelled or timed-out HTTPS helper lookup whose cleanup outlasts 500 ms now reports `cancelled` or `timeout` (with `SetupFailureCause::Interaction`) in `--json`, instead of `io`.
  - `RegistryFault` (`ReaperGone`, `Capacity`, `ForkedProcess`, `Closed`, `Unsupported`) reaches users as "a message naming the reaper", but no error code, `--json` code or text is given for it. It should be specified before 2.3r-1.
- **Processes.**
  - With OD-B3 (a), every gwz command, and every Python process that imports gwz-py's composition, has one extra process: the reaper, in its own session, holding `/dev/null` and the lifeline.
  - With OD-B1 (a), the Git CLI route's commit and tag hooks no longer receive the terminal's Ctrl-C. A hook or signer that reads the terminal stops (Unix) or prompts invisibly (Windows, P2-1), and `gwz commit` waits with no bound.
- **Documentation.** §8 row 7 freezes the gwz-py README text: the helper process, refusal after `os.fork()` without exec, the advice "use `spawn` or `forkserver`", and the warning about `waitpid(-1)` and SIGCHLD.
  - The `forkserver` advice holds only if gwz-py is not preloaded into the fork server. With a module-level composition created at import, a preloaded server would hold the registry, and its workers would be forked copies (R-B3).
  - This depends on whether step 3.2 creates the composition at import or later (PLAUSIBLE).
- **Exit status and defaults.** B fixes no exit status (that is A's). The one documented default is the registry capacity, 256, which §6 leaves as an internal choice open to review.
