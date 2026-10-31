# GwzChildLifetimeDesign — remediation plan, round 1

Date: 2026-11-01. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 2 of design B.**
Drafted read-only except for this file and the design; no git operation was run (the lane owner commits).

- **Object:** `dev-docs/GwzChildLifetimeDesign-DRAFT.md`, revision 1, blob `48b992d9`, as committed at root `d146bc9a`
  (unchanged at `8ade43ac`).
- **Reviews:** [Consistency](GwzChildLifetimeDesign-ReviewConsistency.md), NO-GO, 2 P2 and 8 P3;
  [Safety](GwzChildLifetimeDesign-ReviewSafety.md), NO-GO, 5 P2 and 10 P3. No P0 or P1. IDs below keep each review's
  own numbering with a prefix: `C-` Consistency, `S-` Safety.
- **Architectural count: 0.** Safety: "None of the root causes is architectural"; Consistency names none; the
  operator's record agrees ("B: none", plan :1055-1056). Every finding closes in the design text, a platform module,
  a residual or a decision's stated cost.
- **Result:** revision 2, edited in place, 399 lines (the cap is 400), status "revision 2 (2026-11-01) … for round-2
  review by fresh reviewers". Section references below are revision 2's; its §9 maps every ID to its sections.
- **Verification.** Every fact revision 2 adds was re-read at gwz-core `b745f1fa` (and tag `v1.0.18`), gwz-cli
  `c0ba717`, git2-rs `d13951f`, Go 1.26.4 (`/opt/homebrew/Cellar/go/1.26.4/libexec/src/os/wait_waitid.go:5-6`) and the
  macOS SDK headers (`sys/event.h` `NOTE_EXIT`, `sys/proc_info.h` `PROC_PIDLISTFDS`). Two facts revision 2 relies on
  are not in any file and are marked **(OS)** with a row: kqueue `EVFILT_PROC` registration on a zombie (B25) and
  `proc_pidinfo` being usable in the forked reaper (B6). Reviewer facts that did not hold or were refined: S-P3-2's
  "`/proc/self/fd` descriptor opened before the fork" would list the parent's table, so revision 2 reads it in the
  child; C-P3-7's 1.0.18 line is `repository.rs:299-305` (`Command::new` at :299, `output()` at :305), not :301 alone.

## Blind convergences

The reviews were peer-blind. They converge on:

1. C-P2-1 = S-P2-4 (part): OD-B1 (a)'s stated cost leaves out Windows' console (a hidden prompt, an unbounded wait).
2. C-P3-4 = S-P3-6: "their callers do not change" is false; the gate seam must name the ending and stop std reaping.
3. C-P3-6 = S-P3-8: the `terminate` (f9) change has no B requirement, and the f9 report's path does not resolve.
4. Consistency's out-of-scope note on `pkill -9 gwz` matching the reaper = S-P3-5.
5. Consistency's out-of-scope note on the descriptor-closing loop "up to the limit" = S-P3-2.

## The operator's decisions (plan §10 D-V, "Designs A and B, round 1", :1055-1062) and how revision 2 applies them

- **(i) Force never exits mid-write.** B5 now cites (i) and (ii) (closing C-P3-8). `close()` waits without a bound for
  every live `Wait` entry instead of killing it (§3.4 "Its end"; this also closes S-P2-3's guarded paths);
  `kill_killable()` refuses later `Kill` spawns so children end at once (§3.7). B had no mechanism that existed only to
  cope with exiting mid-write; revision 1's "A calls `close()` only after the last write sequence has ended" becomes
  a property of `close()` itself.
- **(ii) `git commit`/`git tag`, hooks and signers finish in their own group.** OD-B1 (a) is recorded as decided with
  its full cost on both platforms (§6); Safety's option (c) and Consistency's Windows variant are recorded as not
  chosen. Only gwz's death ends a hung write (R-B4, R-B10).
- **(iii) OD-B2 (a), OD-B3 (a)** recorded as decided with their costs (§6, §3.4 "Cost", R-B13, R-B16); §8 rows 5-6 no
  longer assume an answer. A's O1 and O3 / OD-C7 are A's; B cites O3 (a) only where it ends gwz (OD-B1, R-B4).

## Hand-off points relied on in design A revision 1 (A is being revised in parallel)

1. Force calls `kill_killable()` as it begins (A §3.4 step 4), `close()` only after its wait for write sequences (steps
   5-6), and exits only after `close()` returns (step 8). Revision 2's `close()` also waits for `Wait` entries, so B
   stays correct if A reorders steps 5-6.
2. A gwz-cli command waits for its `Wait` children before it returns (A §3.4, R8), so gwz-cli's orderly exits
   (`gwz-cli/src/lib.rs:326, :344`) need no `close()`.
3. Cancel cancels the request's token (A §3.2), which ends the Windows helper through its lookup's cancel (B §3.6).
4. A reports the retained helper tree as `pending_local_work` and the member as `Cancelled` from B's `terminate`
   outcome (A §1 A5, §3.3, step 3.0e after 2.3r-2).
5. After Force begins, A restores the default dispositions (A §3.1, O3 (a)); B treats the resulting death as gwz's
   death (R-B4).
6. The process-globals `signal` kind is A's; B's two forked-child entries stay B's (A §3.1 "Check").
If A revision 2 changes points 1, 2 or 4, B §3.7 and §3.6 change with it; no other B text depends on A.

## Finding disposition

Dispositions: **Fix** (design change), **Fix-text** (text, cost or residual only), **Dispute** (with evidence),
**Record** (named residual or hand-off, no mechanism).

| ID | Finding | Disposition | Closure test | Closed in (rev. 2) |
|---|---|---|---|---|
| C-P2-1 | OD-B1 (a) omits its Windows cost; R-B10 has no Windows recognizer | Fix-text. Windows cost stated (hidden console prompt, unbounded wait, only gwz's death ends it); the kept-console variant recorded as not chosen under decision (ii) | OD-B1 and R-B10 name the Windows behaviour; B15's W leg records a hook reading `CONIN$` hanging | §6 OD-B1; §7 R-B10; §5 B15 |
| C-P2-2 | Residuals for unregistered children consider only gwz's death, not Ctrl-C or Force | Fix-text. R-B5 adds Force and the Windows console; R-B13 adds the terminal's or console's interrupt mid-write (M, L, W); OD-B2 states that cost; B1 lists its exceptions; B5 is scoped to registered `Wait` entries. Own-group-without-registry not taken: OD-B2 (a) is decided as "as today" | R-B5 and R-B13 name the Ctrl-C and Force paths with platforms; OD-B2 states the cost | §1 B1, B5; §3.6; §6 OD-B2; §7 R-B5, R-B13 |
| C-P3-1 | §8 row 8 claims the registry is the "only" starter | Fix-text. Now "of every child a gate with a registry starts", pointing to §3.6's exceptions | The contract row names the exceptions | §8 row 7 |
| C-P3-2 | The Windows helper is no entry, so `kill_killable()` misses it | Fix-text. The difference is stated with how the helper ends (its lookup's cancel, its job at exit) | §3.6 names the difference | §3.6 "The helper on Windows" |
| C-P3-3 | §8 row 5 settles C's 2.3k mechanism and cannot be built before 3.1 | Fix. Row removed; §3.7 hands 2.3k's registry access to C | No §8 row decides anything §0 assigns to C | §3.7; §8 note |
| C-P3-4 | "Their callers do not change" contradicts the per-site specs | Fix. The gate's `spawn`/`output` take a `Spec` and return the crate's `Child`; the three changing callers are named | §3.1 and §4 2.3r-2 list the same callers | §3.1; §4 |
| C-P3-5 | B10's and B21's W legs cannot run as written | Fix. Capacity is a live-entry count on every platform; B10's token clause is M, L only; B21 checks the registry's own jobs | Each W leg uses Windows mechanisms only | §3.1 "Capacity"; §5 B10, B21 |
| C-P3-6 | The f9 change has no B requirement; the f9 path is wrong | Fix-text. New B7 traces it to A's A5 and OQ2; B keeps the code because 2.3r-2 owns `owner.rs`; the path is given as beside the root checkout | §1 traces §3.6's bullet; the path resolves (`/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md`) | §1 B7; §2; §3.6 |
| C-P3-7 | OD-B2's example is not an instance; the 1.0.18 claim is cited at main | Fix-text. §2 lists gwz-core's six production hostless backends and shows none reaches a spawn site (commit, tag and log handlers take their caller's backend); 1.0.18 cited at the tag | OD-B2's statement is checkable; the 1.0.18 citation is at `v1.0.18` | §2 "The gate"; §6 OD-B1, OD-B2 |
| C-P3-8 | B5 settles what A's O2 offered as an alternative | Fix-text. B5 cites decisions (i) and (ii), which settle it | B5's provenance names the decision | §1 B5 |
| S-P2-1 | macOS `waitid(WNOWAIT)` returns for a stopped child | Fix. Linux keeps `waitid`; macOS waits on kqueue `NOTE_EXIT` (no reap), with an `ESRCH` path confirmed by `si_code`; a stop is never an exit | B25 (M, L): SIGSTOP and SIGTTIN under a pty, no retire, no spin, reaper still holds both, SIGKILL while stopped ends both; an immediately exiting child | §2; §3.3 "Exit observation"; §5 B25 |
| S-P2-2 | An embedding application's reaper lets gwz's own kills reach a reused ID while gwz lives | Fix-text (correction (a)). R-B1, §3.3 and B4 state that gwz-side kills are exposed too; the README calls process-wide reaping unsupported. Not taken: (b) a fresh `WNOHANG` check before each kill is itself check-then-kill and does not close the race; (c) refusing on `SIG_IGN`/`SA_NOCLDWAIT` loses function 1.0.18 had and misses a disposition changed after creation | R-B1 and the README text name gwz-side kills; B11 claims only what holds | §1 B4; §3.3; §7 R-B1; §8 row 6; §5 B11 |
| S-P2-3 | An orderly exit or `close()` SIGKILLs a live `Wait` entry mid-write | Fix. `close()` (and the last drop) waits for every live `Wait` entry; gwz-py's close runs it; unguardable exits (`os._exit`, a foreign `exit()`, death after Force) join R-B4; R-B17 names the later members `close()` refuses. A gwz-cli exit row is not added: every gwz-cli `exit` follows the command's return, by which every `Wait` child was waited for (`gwz-cli/src/lib.rs:326, :344`; A R8) | B26 (M, L, W): a daemon-thread commit and `sys.exit(0)` leave no `index.lock`; B15 checks `close()` returns after the commit | §3.4 "Its end"; §3.8; §7 R-B4, R-B17; §5 B15, B26 |
| S-P2-4 | OD-B1 (a)'s cost omits hung hooks, the unbounded Force wait and the Windows console | Fix-text. OD-B1 restated with hung hooks and signers, the wait through Cancel and Force, gwz's death as the only end, `index.lock` versus 1.0.18's SIGINT cleanup, and Windows; Safety's (c) recorded as not chosen under decision (ii) | B15: a never-exiting `pre-commit` is ended only by SIGKILL and `index.lock` remains (recorded); W console hook recorded | §6 OD-B1; §7 R-B4, R-B10; §5 B15 |
| S-P2-5 | The reaper's cost in an embedding process is unstated (copy-on-write, `ENOMEM`, OOM) | Fix-text. Cost bound, `ENOMEM` fail-closed with its message, and OOM exposure stated; gwz-py measurement added to 3.4; R-B16; README. `oom_score_adj` not taken (Record): it can raise the reaper above its parent, the direction the reviewer warns of | B5 injects `ENOMEM` (harness and gwz-py) and checks the message; 3.4 records the reaper's unique memory for a 2 GiB gwz-py heap | §3.1 API; §3.4 "Cost"; §4 3.4; §7 R-B2, R-B16; §8 rows 5-6; §5 B5 |
| S-P3-1 | Windows: gwz terminated between creation and job assignment leaves a suspended orphan | Fix. `close()` takes the gate's hold, so A's own exit never lands in the window; termination from outside is R-B14. The helper's job-list launch is not generalized (it is the helper's private secret-carrying owner, `windows/launch.rs:1-4`); the one-thread rule stays fail-closed, the security-software case being a hypothesis | B12: `close()` during a creation held at a seam returns after the assignment; R-B14 has a recognizer | §3.2; §7 R-B14; §5 B12 |
| S-P3-2 | Descriptor closing is unbounded by `RLIMIT_NOFILE` | Fix. Descriptors are listed, not counted to the limit: `close_range`, else `/proc/self/fd` read in the child (Linux); `proc_pidinfo(PROC_PIDLISTFDS)` repeated (macOS) | B4 with `ulimit -n` at its hard maximum (L without `close_range`, M) | §3.4 "Launch contract"; §5 B4 |
| S-P3-3 | The lifeline protocol is under-specified | Fix. 64-bit tokens never reused; `EINTR` retried; a full table makes the reaper `_exit(1)` killing nothing; the core owns the write end and a writer-thread death closes nothing | B7: writer-thread panic kills no child and refuses later spawns; token counter near its wrap | §3.3 "The writer thread"; §3.4 "Records", "Loop"; §5 B7 |
| S-P3-4 | The fork check misses `Drop`, `kill`, `kill_killable`, `close` and precedes no lock | Fix. The PID check runs first, before any lock, in every method and `Drop`; drops are inert in a copy | B9 extended: drops and calls in a copy forked while another thread holds the lock | §3.1 "Fork check"; §5 B9 |
| S-P3-5 | `pkill -9 gwz` kills the reaper with gwz | Record. Added to R-B2's "When" with the OOM case; `PR_SET_NAME` not taken (Linux only, renames only the short name) | R-B2 names it | §7 R-B2 |
| S-P3-6 | The gate seam is implicit; std reaping in `commit_log` | Fix (with C-P3-4). Named `Spec` per call site; the crate's `Child`. Dispute the source-check part: the crate's `Child` is its own type with no std `wait`, so a std wait on a registry spawn does not compile; no check is needed | §3.1 states the type; B14 runs `rev-list` through it | §3.1; §4 2.3r-2; §5 B14 |
| S-P3-7 | The reaper kills `Session` entries by PID only | Fix. `kill(-pgid)` for `Group` and `Session` (a session leader leads its group), at the reaper and at release | B24: a registered `Session` child and its grandchild end within 1 s of the harness's SIGKILL | §3.3 "Order"; §3.4 "Loop"; §5 B24 |
| S-P3-8 | The `terminate` change is untraced, overlaps A, has an unsourced 30 s and a wrong path | Fix-text (with C-P3-6). B7 traces it; the pending count is A's `pending_local_work`; the 30 s is the existing rows' `reap_pending` deadline (`src/git/endpoint/https_auth/runner_tests.rs:299`); path fixed. Not moved to A: 2.3r-2 owns `owner.rs` and A revision 1 relies on B for it (A §2, §4) | B16 cites its bound's source | §1 B7; §3.6; §5 B16 |
| S-P3-9 | The reaper's snapshot can hold credentials, notably in the interim per-call registries | Fix. "Creation precedes credentials" is an invariant; `spawn_gate()` carries no registry, so the interim per-call forks are gone (the old entries spawn unregistered until 3.3, as today) | The minting check allows `create` only in `composition.rs` (B17); 2.3r-2's dual-review brief names the invariant | §3.1; §4 "Interim" |
| S-P3-10 | macOS std pipe window can delay an `OwnInput` child's end of file | Record. R-B15 handed to C, who decides each `OwnInput` use | R-B15 states the condition; §3.7 hands it to C | §3.5; §3.7; §7 R-B15 |

Counts: Fix 12 (C-P3-3, C-P3-4, C-P3-5, S-P2-1, S-P2-3, S-P3-1, S-P3-2, S-P3-3, S-P3-4, S-P3-6, S-P3-7, S-P3-9);
Fix-text 11 (C-P2-1, C-P2-2, C-P3-1, C-P3-2, C-P3-6, C-P3-7, C-P3-8, S-P2-2, S-P2-4, S-P2-5, S-P3-8); Record 2
(S-P3-5, S-P3-10); Dispute 0 as a whole-row disposition (S-P2-2's options (b)-(c), S-P3-6's source check and S-P3-1's
generalized launch are declined with evidence inside their rows).

## Other review notes

- **Consistency SURFACE:** `RegistryFault` now has a member code and message (§3.1, `ErrorCode::Io`); the README's
  `forkserver` advice now says "without preloading gwz" (§8 row 6); exit status stays A's.
- **Consistency §2 gap:** `driver_only.rs:61` added to `spawn_gate()`'s callers (§2).
- **Consistency out-of-scope, 2.3i's handler:** settled by decision (iii), A's O1 (one site; 2.3i uses A's module).
- **Safety out-of-scope, `HelperJob::drop` in a forked copy** (`owner.rs:288-291`): a plausible defect in existing
  code, unconfirmed; its Unix arm is covered by §3.1's fork check once 2.3r-2 moves the helper onto the registry. Until
  then it is outside B's scope; recorded here for the lane owner, not fixed.
- **Safety hand-offs:** to A, the gate before `ExitProcess` is now in B's `close()` (§3.2); to C, the `ssh`
  `ProxyCommand` row stays C's (R-B7).

## Re-review

Fresh Consistency and Safety reviewers review revision 2 against D-V (:999-1062), this plan and design A's current
revision. Exit: GO on both axes with no new P0-P2. Open for round 2: the two **(OS)** facts (B6, B25), and whether
A revision 2 keeps hand-off points 1, 2 and 4.
