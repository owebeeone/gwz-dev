# GwzChildLifetimeDesign — remediation plan, round 2

Date: 2026-11-03. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 3 of design B.**
Drafted in the lane `gwz-dev-docs-cancel`, writing only this file and the design; no git operation was run.

- **Object:** `dev-docs/GwzChildLifetimeDesign-DRAFT.md`, revision 2 (hash `9724eae6`, both reviews' pin), as committed
  at root `6d20ef6f`.
- **Reviews:** [Consistency](GwzChildLifetimeDesign-ReviewConsistency-2.md), NO-GO, 1 P2 and 9 P3;
  [Safety](GwzChildLifetimeDesign-ReviewSafety-2.md), NO-GO, 2 P2 and 6 P3. No P0 or P1. Each review numbers its new
  findings `N-…`; IDs below add a prefix: `C-` Consistency, `S-` Safety (so `C-N-P2-1`, `S-N-P3-4`).
- **Architectural count: 0.** Both reviews name no new architectural root cause; the operator's record agrees ("B: 3
  P2, count 0", plan :1063-1064). This is the last round under the review cap, so revision 3 adds no mechanism: each
  finding closes by narrowing text, an ordering inside an existing step, a named residual, an Owns list, or a hand-off
  (OD-B4 to design D, which puts it to the operator).
- **Result:** revision 3, edited in place and then by the fix pass below, 399 lines (the cap is 400), status "revision 3 (2026-11-03) … for round-3
  review". To fit, §1 (requirements), §5 (rows) and §6's decided options became tables; no row or requirement was
  dropped. Section references below are revision 3's; its §9 maps every ID to its sections.
- **Verification.** Every citation revision 3 adds was re-read at its pin: gwz-core `b745f1fa`
  (`https_auth.rs:106-167, :175, :190`; `runner.rs:115-121`; `commit_log/mod.rs:296-300, :359-361`;
  `src/git/verify_checkout_state.rs:124-129`; `composition.rs:20-28`; `scripts/checks/check_crate_versions.py:85`
  `EXPECTED_CRATES = 18`; `check_process_globals.py:103-104, :540-541`, where `libc::chdir`, `signal`, `sigaction` are
  kind `env`; `local_clone_inventory.json`'s LBT-001 description), gwz-git `9bc4ea9` (`src/native_commit.rs:93-95`,
  `src/native_commit/cli.rs:17-21`), std 1.95.0 (`sys/process/unix/unix.rs:93-94` the fork with no signal blocked,
  `:352-353` "Inherit the signal mask", `:463` closures rule out `posix_spawn`, `:990-996` `kill` after the reap returns
  `Ok`), root `6d20ef6f` (`MODULE.bazel:73-96`, unchanged since `8ade43ac`; plan :1006, :1063-1069, its lines before
  :1063 unchanged since `8ade43ac`), blob `3448c561` (:245-246) and A revision 2 (:91-92, :179-180, :183-184, R9, R14,
  A6). The fix pass's citations were re-read too: gwz-core `b745f1fa` `runner.rs:115-121, :141, :145` (a wait error
  becomes `AuthError::Io`), `https_auth.rs:142` (`process wait failed`); A revision 3 as it stood in the lane on
  2026-11-03, cited by section: §3.1 "Unix" (SIGQUIT keeps its default) and "Check" (B's 2.3r-1 and 2.3r-2 merge after
  2.3s), §3.2 (the host close), §3.3 (`Pending`), §4 point (6), its 2.3s row ("before 2.3k, 2.3r-1, 2.3r-2"), §8 row 7,
  row 6.1, R14 ("SIGQUIT at any time"); macOS `sigaction(2)`: under `SIG_IGN` "current and pending instances … are
  ignored and discarded" (marked **(OS)**; B8 verifies it on M and L). Reviewer facts refined: C-N-P3-7's OD-C2 (a) is at `3448c561` :245-246, not :244-245 (the bullet starts at
  :245); C-N-P2-1's `verify_checkout_state.rs` is `src/git/verify_checkout_state.rs`.

## The operator's round-2 answers (plan §10 D-V, "Designs A and B, round 2", :1063-1069) and how revision 3 applies them

- **O4 (a): SIGINT, SIGTERM, SIGHUP and Windows' console events take the interrupt path, exit 130.** For B this removes
  gwz-cli's SIGTERM and SIGHUP from the deaths that tear a commit: they now reach Cancel and Force, which wait for the
  `Wait` child (OD-B1, R-B4, §3.7). Console events still never reach a `CREATE_NO_WINDOW` `git` (§3.2); after
  CTRL_CLOSE, LOGOFF or SHUTDOWN Windows ends gwz itself a short time later (A R9), its jobs close and the commit is torn
  (R-B4, W).
- **O5: OD-C7 (a) kept.** A further interrupt once Force has begun ends gwz by the OS default (A R14). B lists it among
  R-B4's triggers and OD-B1's cost; decision (i) governs gwz's own exit, which B's `close()` and `kill_killable()`
  already serve (§3.4, §3.7).
- Neither answer changes a mechanism of B. Decision (i)'s "only SIGKILL ends a hung write" concerns Force; what the
  reaper sends a `Wait` group when gwz is gone was never put to the operator, so revision 3 names it OD-B4 (S-N-P2-1)
  and hands it to design D (G5).

## Blind convergences

1. C-N-P3-3 = S-N-P3-5 (first half): after a writer-thread death the waiter's release order is undefined (block for
   ever, or reap without unregistering).
2. C-N-P2-1 and S-N-P2-2's closure test meet in the same gap: no `RegistryFault` reaches the member as written, so the
   documented messages (B19, R-B2, R-B16, and the new per-spawn `ENOMEM`) cannot be observed. One fix serves both: fixed
   messages, one transport (`io::Error::other`) and a per-site table (§3.1).
3. Consistency's SURFACE note (the `ReaperGone` cause depends on which observer sees the death first) is closed by the
   same fix: messages carry no variable part; a failed fork is its own variant, `ReaperNotStarted`.

## Hand-off points relied on in design A revision 2 (A is being revised to revision 3 in parallel)

1. Force step 4 calls B's `kill_killable()` (A :179-180). B now names the public surface: `ProcessGate::kill_killable()`
   (§3.1, §3.7; C-N-P3-2). A revision 3 should name it.
2. A refused `Kill` spawn's member reports `Cancelled` (A :179-180). B's helper mapping gives `AuthError::Cancelled` for
   `Closed` (§3.1); a refused `rev-list` step degrades `HistoryUnreadable` inside a member Force stops anyway.
3. A calls no `close()` of B's (A :183-184); B §3.7 keeps the gwz-cli exit-site argument that none is needed. gwz-py's
   close calls `ProcessGate::close_children()` (§3.8).
4. `run` creates B's registry before A's install, before any thread (A §3.1 "Install", :86-87; B §4 3.1).
5. A's handler returns at once in a forked child (PID check, A :91-92). B no longer relies on it: revision 3 blocks
   every signal around each spawn and resets caught handlers before unmasking (§3.2, S-N-P3-1), which also covers
   Python's and an embedding application's handlers; the fix pass (G1) adds a `SIG_IGN` of SIGINT, SIGQUIT, SIGTSTP and
   SIGHUP first, discarding one sent to gwz's group before the child left it. The two are consistent.
6. O4 (a), O5 as A states them: R-B4 cites A R9 (Windows ends gwz after console close, logoff, shutdown) and A R14 (an
   interrupt after Force began). If A revision 3 renumbers them, B's citations follow.
7. The gwz-py CLI's SIGTERM and SIGHUP (A §3.8, R15). B's R-B4 names "a gwz-py host's default SIGTERM or SIGHUP"; if A
   revision 3 routes the gwz-py CLI's through its interrupt path under O4 (a), R-B4 then covers embedding applications
   (and what A leaves at the default) only; no B text changes.
8. BR7 cites A's A6 (honest member reports; revision 2's A5 is status and marking). A revision 3 keeping A6 keeps it.
9. **Changed by C-N-P3-1:** the crate's allowlist entries move from 2.3r-2 to 2.3r-1, which merges after A's 2.3s so the
   `sigaction` and `signal` entries take A's `signal` kind (§4, §8 row 1). **Taken by A revision 3** (G4): its §3.1
   "Check", §4 point (6), 2.3s row and §8 row 7 now say 2.3r-1 and 2.3r-2 merge after 2.3s; its earlier "either order"
   text is gone, so the designs agree and A re-kinds no B entry. 2.3r-1 and A's 3.0d (`crates/write-gate`) both bump `EXPECTED_CRATES` and add a
   manifest to root `MODULE.bazel`: merged serially, 2.3r-1 first (Phase 2 before Phase 3).

10. **A revision 3 §4 points 2, 5, 8** (fix pass): B §3.6 now names A r3's host close (§3.2) and `Pending` (§3.3) in
    place of "A's Cancel token" and `pending_local_work`; B §8 row 4 says A §8 row 7's totals sentence supersedes its
    clause. B's A citations by line (":179-180", ":183-184") stay valid: they are pinned to A revision 2 at root
    `6d20ef6f`; A r3 is cited by section.

If A revision 3 changes points 1-3, 9 or 10, B §3.1, §3.6, §3.7, §4 and §8 change with it; no other B text depends on A.

## Finding disposition

Dispositions: **Fix** (design change within an existing mechanism), **Fix-text** (text, cost, Owns or residual only),
**Dispute** (with evidence), **Record** (named residual, alternative or hand-off, no mechanism).

| ID | Finding | Disposition | Closure test | Closed in (rev. 3) |
|---|---|---|---|---|
| C-N-P2-1 | The frozen `RegistryFault` contract reaches the member at none of the three spawn sites; B19 cannot pass; R-B2's and R-B16's recognizers fail for the helper | Fix. `RegistryFault` becomes a `Copy` enum with one fixed message per variant (no cause, no capacity number), so it fits `AuthError`, which is `Copy` with fixed messages (`https_auth.rs:106-167`); a failed reaper fork is its own variant `ReaperNotStarted`. It travels through every `io::Result` interface as `io::Error::other(fault)`; `spawn`, `output` and `exited()` return `io::Result`, std's own errors unchanged (G3). Per site: the helper's `runner.rs:115-121` downcasts it (`Closed` → `AuthError::Cancelled`; any other → new `AuthError::ChildProcess(fault)`, the transport's `ErrorCode::Io`, the fault's message); the CLI route keeps gwz-git's `failed to run <what>: <message>` as `GitCommandFailed` (gwz-git is reference-only; accepted and stated); `rev-list` keeps its `HistoryUnreadable` degradation with the message. "`ErrorCode::Io`" is the transport's (`gwz_transport::protocol::ErrorCode`), named per site. `https_auth.rs` and `runner.rs` join 2.3r-2's Owns | §3.1's table lists each site's code and message; B27 (2.3r-2; M, L): with the reaper SIGKILLed a helper lookup, a CLI-route commit and a path-filtered log each show `child-process reaper unavailable` with the stated code; B19 expects the same `Io` text in gwz-py; B5 checks every message exactly | §3.1 "API", "Faults" (with its exit row, G3); §4 2.3r-2; §5 B5, B10, B19, B27; §7 R-B2, R-B16 |
| C-N-P3-1 | 2.3r-1 cannot pass the gates with the files it owns; "shares only `Cargo.lock`" is false | Fix-text. 2.3r-1 owns root `MODULE.bazel` and `MODULE.bazel.lock` (the manifest in `@gwz_core_crates`, `MODULE.bazel:73-96`), `scripts/checks/local_clone_inventory.json` (its entry and edges; LBT-001), `check_crate_versions.py` (`EXPECTED_CRATES`, :85) and the allowlist entries (`sigaction`, `signal` under A's `signal` kind, `chdir` under `env`; it merges after A's 2.3s); 2.3r-2 owns gwz-core's inventory edge to the crate. The shared-file sentence and §8 rows 1, 3, 4 restated | §4 and §8 row 1 list these files; §4 "Waves" names them as shared and merged serially | §4; §8 rows 1, 3, 4 |
| C-N-P3-2 | `close()` "takes the gate's hold" contradicts §3.1 (the crate cannot take gwz-core's gate); no public control path | Fix. The registry has its own creation lock, held by every Windows creation from `CreateProcess` through its resume; `close()` takes that lock (B12 tests it in the crate's harness). 2.3r-2 adds public `ProcessGate::kill_killable()` and `ProcessGate::close_children()` in `composition.rs` (already owned), no-ops without a registry | §3.1, §3.2 and §3.7 name one lock and one public surface; A's step 4 and 3.2's close resolve to it (hand-off 1, 3) | §3.1 "Value", "Where it comes from"; §3.2; §3.7; §3.8; §4; §8 row 5 |
| C-N-P3-3 | A writer-thread death leaves the release order undefined | Fix (with S-N-P3-5). A waiter reaps only after its acknowledgement or after the watch thread has reaped the reaper (nothing can then kill); otherwise its zombie stays pinned until gwz exits (R-B1 at that exit), and `close()` counts an observed exit as ended | B7: a writer panic with a live `Wait` and a live `Kill` entry: no child killed, both exits observed, `close()` returns, no zombie reaped while the reaper lives | §3.3 "The writer thread", "Why no kill…"; §7 R-B1; §5 B7 |
| C-N-P3-4 | Requirement IDs and row IDs share one namespace; B4's quote and line | Fix-text. Requirements renamed BR1-BR8; rows keep B1-B29 (A cites row B22, §8 cites row ranges). BR4 now quotes D-V :1006 | Each ID has one meaning; BR4 cites :1006 | §1; §2; §3.3-§3.6 |
| C-N-P3-5 | B18's and B19's legs marked **(reg)** are red before their step | Fix-text. **(reg)** only on the Windows helper legs (the helper has its own job today); B3's W leg unmarked (G6) | Every (reg) leg passes at its step's parent | §5 B3, B18, B19 |
| C-N-P3-6 | §8 row 6 applies README text "at acceptance", §4 assigns it to 3.2 | Fix-text. Row 6 and §8's heading say step 3.2 applies it; 3.2 owns `gwz-py/README.md` | One owner, one time | §8 heading, row 6; §4 3.2 |
| C-N-P3-7 | The superseded-clause note omits r1 OD-C2 (a)'s pricing | Fix-text. The note names it (`3448c561` :245-246), superseded by OD-B3 (a) | The note names it | §8 note |
| C-N-P3-8 | OD-B2 (a) narrows decision (ii) for registry-less gates without recording the alternative | Record. OD-B2's "Not chosen" adds an own group without a registry, with its cost: no terminal Ctrl-C reaches `git`, which then outlives gwz to finish its write with nothing to end a hung one | OD-B2 names it | §6 OD-B2 |
| C-N-P3-9 | The Windows `Process` target differs from Unix in two unstated ways | Fix-text. §3.2 states that `CREATE_NEW_PROCESS_GROUP` disables CTRL_C whatever `ignore_interrupts` says, and that a `Process` entry's descendants end with its job on W while M and L kill its PID alone; R-B7's recognizer is per platform | §3.2 and R-B7 name both | §3.2 "Windows targets differ"; §7 R-B7 |
| S-N-P2-1 | At gwz's death the reaper SIGKILLs a running `git commit` that 1.0.18 let finish; R-B4 and OD-B1 omit the common triggers and a less destructive kill | Fix-text, and Record (OD-B4 handed to D, G5). OD-B1's cost and R-B4's "When" now name every death that tears a commit, hung or not: SIGKILL, a crash, the OOM killer, an interrupt once Force has begun (O5), SIGQUIT (its default, a terminal Ctrl-\, G2), a gwz-py host's default SIGTERM or SIGHUP, `os._exit`, a foreign `exit()`, an `exec` of the host, and Windows ending gwz after console close, logoff or shutdown (A R9); with the 1.0.18 comparison (a death signalled to gwz alone let the commit finish; a group signal let `git` remove its locks). Under O4 (a) gwz-cli's SIGTERM and SIGHUP leave the list. New **OD-B4** (a §6 row) names the reaper's signal for a `Wait` group; D-V does not decide it, B is written and complete as (a), and D, which owns R-B4's `*.lock` files and the accepted-residual list, puts it to the operator (G5): (a) SIGKILL, as written; (b) **recommended**, SIGTERM then SIGCONT to each `Wait` group, so `git` removes its locks as under 1.0.18's Ctrl-C, a hook that ignores SIGTERM outliving gwz (R-B18); (c) leave `Wait` groups to finish (against BR1). (b) still sends every signal at end of file, at once, so §3.3's reuse argument stands; a follow-up SIGKILL after a bound was not offered, as it would signal a group ID that `init` may have freed and reused (BR4). Windows is (a) under every choice. (b) is a new mechanism (an ending bit, a second signal), so it is not adopted under the cap; if the operator takes (b) or (c), D amends §3.4 "Records", "Loop", R-B4, R-B18 and B29 and reviews that in its own loop | B29 (3.2; M, L, W): a gwz-py commit with a sleeping hook gets SIGTERM; `git`'s group is gone within 1 s and `index.lock` remains, R-B4's residue under (a) (Safety's closure test, "Under (a)"); W records. B18 (3.1): gwz's SIGKILL, and SIGQUIT to the terminal's group, mid-hook leave `index.lock` (R-B4, OD-B4 (a)) | §6 OD-B1, OD-B4; §3.4 "Loop"; §3.6; §7 R-B4, R-B18; §5 B18, B29 |
| S-N-P2-2 | The child-side registration forces a full `fork` of the host for every registered spawn; unpriced | Fix-text, with the reviewer's optional named fault taken. §3.4 "Cost" states it: the `pre_exec` closure rules out `posix_spawn` (`unix.rs:463`), so each spawn copies the host's page tables (a path walk slows with the host's size) and under strict overcommit is charged the host's private writable memory again; R-B19 is the sibling of R-B16; an entry spawn std refuses `ENOMEM` is `RegistryFault::OutOfMemory`, `child process not started: out of memory`; the README text says so; 3.4 measures per-spawn fork time and a path walk's step rate at a 2 GiB gwz-py heap (M, L). A `vfork`-style spawn is not taken now; it is the operator's if 3.4's numbers do not suit (no new mechanism in this round) | B5: an injected fork `ENOMEM` at an entry spawn after `create` gives `OutOfMemory` with its exact message; B19 (gwz-py): the same raises `Io` with it; the 3.4 evidence records per-spawn fork time | §3.1 "Faults"; §3.4 "Cost"; §4 3.4; §7 R-B19; §8 rows 5-6; §5 B5, B19 |
| S-N-P3-1 | On the fork path A's handlers run in a registered child before `exec`; §3.2 unmasks before it ignores | Fix (an ordering inside §3.2's existing closure). The spawning thread blocks every signal around the spawn (std's fork path blocks none, `unix.rs:93-94`); in the child, in order: `setsid` for `Session`; SIGINT, SIGQUIT, SIGTSTP and SIGHUP `SIG_IGN` for every target, discarding one pending from gwz's group (G1); every caught handler and those four `SIG_DFL` unless gwz ignored it; `SIG_IGN` for `ignore_interrupts`; the register write; and last the empty mask. The reaper also resets every caught handler before it unmasks and is forked with every signal blocked. A revision 2's PID check (A :91-92) already stops the double count byte; B no longer depends on it (hand-off 5) | B8: a child held between `fork` and its closure while SIGINT reaches the harness's group: the harness handler's pipe gets one byte; a `Process { ignore_interrupts }` child with SIGINT pending runs its program; a `Group` `Wait` and a `Session` child held there while SIGINT, SIGQUIT and SIGTSTP reach the group run to exit, never stopped (G1). B15: SIGINT inside that window reaches neither `git` nor the hook | §2 "std's spawn"; §3.2 "Unix"; §3.4 "Creation", "Launch contract"; §5 B8, B15 |
| S-N-P3-2 | An exit seen only as `ECHILD` has no specified status and does not disarm gwz's kills | Fix. Such an entry is unregistered at once; every later gwz kill of it does nothing; `exited()` and `output` return `RegistryFault::StatusLost`, never a made-up status. R-B1 notes that an inherited SIGCHLD `SIG_IGN` (kept across `exec` on Linux) makes every status `StatusLost`, as 1.0.18's std wait failed `ECHILD` | B11 traces every `kill(2)`: none follows the `ECHILD`; a `Wait` child's `output` returns `StatusLost`; B27: a helper's exit taken by a `waitpid(-1)` thread fails `Io` with `StatusLost`'s message (`runner.rs:145` downcasts, G3) | §3.1 exit row; §3.3; §3.6; §7 R-B1; §5 B11, B27 |
| S-N-P3-3 | The macOS `ESRCH` path says nothing about a child exiting but not yet a zombie (PLAUSIBLE, OS) | Fix-text. While `waitid(WNOHANG | WNOWAIT)` reports no exit after an `ESRCH`, the kqueue registration and the `waitid` are retried after 1 ms, doubling to 100 ms, until an exit is reported **(OS)**; no spin | B25 (M): a seam forces `ESRCH` while `waitid` reports nothing for 50 ms; the exit is seen within the backoff, under the CPU cap | §3.3 "Exit observation"; §5 B25 |
| S-N-P3-4 | A failed descriptor sweep leaves the reaper holding the lifeline's write end, silently losing BR1 | Fix. The reaper closes its copy of the write end by number before the sweep; `close_range` falls back to the `/proc/self/fd` listing on any failure, not by kernel version; if no method confirms only 0-3 remain it `_exit(1)`s, which the watch thread reports as `ReaperGone` | B4 (L): `close_range` refused by a seccomp filter and `/proc` hidden: the group ends within 1 s or `ReaperGone` is raised, never neither; the "L without `close_range`" leg names its seam (Consistency's risk note) | §3.4 "Launch contract"; §5 B4 |
| S-N-P3-5 | Writer-thread death and a full lifeline leave the waiter and `close()` unspecified | Fix (the writer half, with C-N-P3-3) and Record (the full lifeline). The lifeline stays blocking: making writes non-blocking with `EAGAIN` as `ReaperGone` would call a stopped reaper dead, against round 1's S-P2-1 closure and B25. R-B2 states that a reaper stopped by SIGSTOP blocks entry spawns and releases once about 4,096 records fill the lifeline, until it is continued; recognized by a reaper in state `T` | B7 as for C-N-P3-3; B25: with the reaper stopped, 5,000 entry spawns, where gwz blocks is recorded | §3.3 "The writer thread"; §6 "Choices open to review" (a blocking lifeline); §7 R-B2; §5 B7, B25 |
| S-N-P3-6 | The registry-less mode of the crate's `Child` ships but is unspecified | Fix-text. Without a registry the crate's `Child` is no entry (no record, no job) and spawns as today: `rev-list` and the CLI route stay in gwz's group (OD-B2 (a)), waited for by std's `output` and never signalled; the helper leads its own group (`process_tree.rs:71`) and keeps §3.3's observation and order, so its kills, group kill included, precede its reap. This differs from the reviewer's "exit is std's wait": a std wait reaps, after which the owner's group kill (`owner.rs:253-256`) would be skipped or would reach a freed group ID; §3.3's order keeps it before the reap. R-B13 adds the registry-less helper | B28 (2.3r-2; M, L): with no registry the helper leads its own group, a CLI-route commit and `rev-list` stay in the harness's group and are never signalled; a helper whose leader exited, then cancelled: its group kill precedes its reap and no signal follows the reap (traced) | §3.1 "Without a registry"; §3.6; §7 R-B13; §5 B28 |

Counts (18 rows): **Fix 7** (C-N-P2-1, C-N-P3-2, C-N-P3-3, S-N-P3-1, S-N-P3-2, S-N-P3-4, S-N-P3-5); **Fix-text 10**
(C-N-P3-1, C-N-P3-4, C-N-P3-5, C-N-P3-6, C-N-P3-7, C-N-P3-9, S-N-P2-1, S-N-P2-2, S-N-P3-3, S-N-P3-6); **Record 1**
(C-N-P3-8; the recorded halves of S-N-P2-1 (OD-B4, handed to D) and S-N-P3-5 (R-B2) sit in their rows); **Dispute 0** as a
whole-row disposition. Declined inside rows, with evidence: S-N-P3-5's non-blocking lifeline (it would make a stopped
reaper dead), S-N-P3-6's "std's wait" (it would order the group kill after the reap), S-N-P2-1's SIGKILL follow-up
after a bound (a freed group ID), and C-N-P3-7's line (:245-246).

No Fix adds a mechanism: the per-site mapping, the creation lock and the two `ProcessGate` methods expose or move what
revision 2 already had; the signal mask, the handler reset and the explicit descriptor close order steps of existing
launches; `StatusLost`, the backoff and the sweep's `_exit(1)` specify paths revision 2 left undefined. OD-B4 (b), B's
recommendation, would be a new mechanism (an ending bit in the record, a second signal in the reaper's loop): it is
not adopted in round 3; it is D's to put to the operator and, if taken, D's to amend and review (G5).

## Fix pass (lane owner's gaps G1-G7 on revision 3 and this plan)

| Gap | Disposition | Change | Closure test | Where (rev. 3) |
|---|---|---|---|---|
| G1 the fork-to-`setpgid` window: a pending SIGINT or SIGQUIT kills, a SIGTSTP stops, a `Group` or `Session` child in a group no `fg` continues | Fix (an ordering inside the existing closure) | In the child, after `setsid` for `Session`, SIGINT, SIGQUIT, SIGTSTP and SIGHUP `SIG_IGN` for every target, which discards a pending one **(OS)**; then every caught handler and those four `SIG_DFL` unless gwz ignored it (`sigaction`'s old action); then `ignore_interrupts`, the register write, the empty mask. A terminal signal in that window is gwz's alone (A's path) | B8: a `Group` `Wait` and a `Session` child held in the window while SIGINT, SIGQUIT and SIGTSTP reach the harness's group run to exit, never in state `T`; B15: SIGINT inside the window reaches neither `git` nor the hook | §3.2 "Unix"; §5 B8, B15 |
| G2 SIGQUIT missing from the deaths; OD-B1's "sends" misstates O4 (a) | Fix-text | OD-B1 and R-B4 add SIGQUIT (its default, A r3 §3.1 "Unix", row 6.1, R14): a terminal Ctrl-\ ends gwz alone, `git` in its own group, and the reaper kills `git`. OD-B1: "to which O4 (a) also routes SIGTERM, SIGHUP and console events". §3.7 adds "SIGQUIT at any time" | B18 adds a SIGQUIT-to-the-group leg (M, L): `index.lock` remains | §3.7; §5 B18; §6 OD-B1; §7 R-B4 |
| G3 std's other spawn errors and exit-time faults have no route | Fix-text | `spawn`, `output`, `exited()` return `io::Result`: std's errors pass unchanged (`runner.rs:116-118`'s `MissingExecutable`, `ConfigurationRefused` kept), a fault is `io::Error::other(fault)`. The per-site table gains "Helper, at exit": `runner.rs:145` downcasts `StatusLost` to `AuthError::ChildProcess`, not `AuthError::Io`'s `process wait failed` (`https_auth.rs:142`); 2.3r-2 owns `runner.rs:145`; §3.6 keeps `:123-144` unchanged | B10: a missing program refused with std's `NotFound`; B27: a helper's exit taken by `waitpid(-1)` fails `Io` with `StatusLost`'s message, a missing helper still `MissingExecutable` | §3.1 "API", "Faults"; §3.6; §4 2.3r-2; §5 B10, B27 |
| G4 merge order of 2.3r-1 against A's 2.3s | Fix-text (no B change of rule) | A revision 3 now states B's rule in §3.1 "Check", §4 point (6), its 2.3s row and §8 row 7 (after 2.3s; no re-kinding of B entries). B §4's "After" cites those sections; hand-off 9 records the agreement | The two designs state one order: 2.3s, then 2.3r-1 and 2.3r-2 | §4 2.3r-1; §8 row 1; hand-off 9 |
| G5 OD-B4 open in the last round; (b) is a new mechanism; no review route; GO with B29 hanging | Record (hand-off to D) | OD-B4 becomes a §6 row: D-V does not decide it; B is written, reviewed and complete as (a); D (owner of R-B4's locks and the residual list, §0) puts it to the operator; (b) is named a new mechanism and not adopted under the cap; if (b) or (c) is taken, D amends §3.4 "Records", "Loop", R-B4, R-B18 and B29 and reviews that in its own loop. B29 asserts (a) only; R-B18 is conditional on D taking (b) | Round 3 can return GO on (a): no row or residual depends on an unanswered decision | §6 OD-B4; §5 B29; §7 R-B18 |
| G6 B3's W leg marked (reg) at 2.3r-1 | Fix-text | The mark is removed: the crate's harness does not exist at 2.3r-1's parent | Every (reg) leg passes at its step's parent | §5 B3 |
| G7 §9 omits sections that close findings | Fix-text | §9 adds §3.6 (S-N-P2-1, S-N-P3-2, S-N-P3-6), B19 (S-N-P2-2), §8 row 5 (C-N-P3-2), B3 (C-N-P3-5), B15 (S-N-P3-1), the exit row and B27 (S-N-P3-2), and A r3's §4 for §3.6 and §8 row 4 | §9 names every section this plan's last columns name | §9 |

Line budget: G1-G3 and G5 added text; OD-B4 moved from a 7-line bullet to a §6 row and four paragraphs were tightened,
so revision 3 stays at 399 lines.

## Other review notes

- **Consistency SURFACE.** The five messages are now eight fixed strings (§3.1); the `ReaperGone` cause race is gone.
  Process and commit surfaces are unchanged except OD-B4, which D puts to the operator.
- **Safety's C-P3-4 note** ("`output` … return the crate's `Child`"): §3.1 now says `output` returns the crate's `Output`.
- **Consistency risk notes.** B4's "L without `close_range`" leg names its seam. The (OS) facts stay hypotheses until
  their rows run: kqueue registration on a zombie and the `ESRCH` backoff (B25), `proc_pidinfo` in the forked reaper
  (B6), glibc's `posix_spawn` as a `CLONE_VM | CLONE_VFORK` clone (3.4's measurement).
- **Hand-off watch.** A revision 2 dropped revision 1's :216-217 sentence (refused `Kill` spawns now report
  `Cancelled`, A :179-180), closing round 1's hand-off 7.

## Unresolved, for the lane owner

1. **OD-B4 is the operator's, through design D** (G5). It no longer blocks B: revision 3 is complete as (a), B18 and B29
   assert (a), and (b) or (c) would reach B only as an amendment D drafts and reviews.
2. **A revision 3** was still being edited in the lane while this pass ran. At the time of writing it keeps hand-offs 1-3
   and states hand-offs 9 and 10 as B does; a later change to A's §3.1 "Check", §4 or §8 row 7 reopens G4.
3. The (OS) facts above, and G1's discard of a pending signal under `SIG_IGN`, are verified only when their rows run.

## Re-review

Fresh Consistency and Safety reviewers review revision 3 (with this fix pass) against D-V (:999-1069), this plan and
design A's current revision, OD-B4 taken as (a). Exit: GO on both axes with no new P0-P2. An architectural finding in
round 3 stops the design under the cap.

## Closure pass (round 3 pre-commitments)

Date: 2026-11-03. Drafted in the lane `gwz-dev-docs-cancel`, writing only this file and the design; no git operation
was run. It authorizes nothing beyond revision 3.1 of design B.

- **Object:** revision 3 (hash `0cbb9081`, both round-3 reviews' pin) edited in place to revision 3.1 (hash
  `4d470544`), 399 lines (under the 400 cap). No mechanism is added: the creation lock exists in revision 3 and only
  its stated scope widens; the rest is an ordering inside the existing closure, a named residual, Owns lists, rows and
  §8 cells. To fit, §3.7 and §9 became tables and the helper's Windows bullet moved into §3.6's table; nothing dropped.
- **Reviews:** [Safety](GwzChildLifetimeDesign-ReviewSafety-3.md), NO-GO, 1 P2 and 3 P3, pre-commit: GO on a revision
  that closes N-P2-1 as its correction states, adds no new P0-P2, and resolves N-P3-1 to N-P3-3 or defers each with an
  owner; [Consistency](GwzChildLifetimeDesign-ReviewConsistency-3.md), GO, 6 P3 (text edits, no re-review needed on
  that axis). IDs below add `S3-` (Safety round 3) or `C3-` (Consistency round 3) to the reviews' own `N-…` IDs.
- **Architectural count: 0** (both reviews).

| ID | Finding | Disposition | Change | Closure test | Where (rev. 3.1) |
|---|---|---|---|---|---|
| S3-N-P2-1 | On Unix `close()` and `kill_killable()` do not exclude an entry spawn in flight: the reaper can SIGKILL a `Wait` child while gwz lives, or the child writes its record into a reused descriptor | Fix-text, exactly the correction (the existing creation lock, stated for every platform) | New §3.2 bullet "The creation lock": an entry spawn holds it from its `Closed` and `Capacity` checks until std's spawn returns and the entry is recorded live (Windows: through its resume); a failed spawn unregisters its token; `close()` and `kill_killable()` take it only to set `Closed` and snapshot the live entries; `close()` closes the write end only after that; the child takes no lock (the fork check precedes any lock; the closure takes none); spawns already serialize under the gate's hold. §3.2 "Windows" refers to it; §3.4 "Its end", §3.7 (A), §3.8 follow. Consequence named, not new: an entry spawn blocked on a full lifeline (R-B2's stopped reaper) holds the lock, so `close()` and `kill_killable()` (A's Force step 4) wait too; R-B2 says so | B22 (M, L, W): a seam holds an entry spawn after its check, and separately between `fork` and `exec` (W: before its resume), while `close()` or `kill_killable()` runs: refused `Closed` before its fork, or an entry they act on (a `Wait` child runs to exit and `close()` returns after it; a `Kill` child is killed); no reaper kill of a `Wait` token; no write outside the lifeline (traced). B26 (M, L): the same seam on gwz-py's daemon-thread commit: refused `Closed` or completed, never killed | §3.2 "The creation lock", "Windows"; §3.4 "Its end"; §3.7; §3.8; §7 R-B2; §5 B22, B26 |
| S3-N-P3-1 | The pre-`exec` ignore set omits SIGTERM, which O4 (a) makes an interrupt source | Resolved (Fix-text, the window's existing set) | SIGTERM joins SIGINT, SIGQUIT, SIGTSTP and SIGHUP in the window's `SIG_IGN` set, with the same "unless gwz ignored it" restore ("those five"); a group SIGTERM in the window is gwz's alone; any other signal keeps its default (the reviewer's optional sentence) | B8: the `Group` `Wait` and `Session` legs add SIGTERM to the harness's group inside the seam; the child runs to exit | §3.2 "Unix"; §5 B8 |
| S3-N-P3-2 | "Last, an empty mask" departs from std and 1.0.18, which pass on the spawning thread's mask | Resolved (Fix-text) | The spawning thread saves its mask before blocking; the closure's last step restores that saved mask, as std (`std/sys/process/unix/unix.rs:352-353`) and 1.0.18 pass it on. The reaper keeps its empty mask (not part of the finding) | B8: a spawn from a thread with SIGINT and SIGPIPE blocked gives a child whose mask equals that thread's (both blocked) | §3.2 "Unix"; §5 B8 |
| S3-N-P3-3 | Every fork B makes runs the host's `pthread_atfork` handlers: the launch contract is false in an embedding host, a blocked handler leaves an unrecognizable reaper, and the reaper keeps the host's mappings (PLAUSIBLE, OS) | Resolved as text, the Linux fork call **deferred, owner design D** | §3.4 "Launch contract" now starts "after `fork` returns, R-B20"; §3.4 "Cost" names the handlers (`std/sys/process/unix/unix.rs:187-189`, std's `libc::fork()`; glibc's `posix_spawn` runs none, **(OS)**); new R-B20 (a reaper that never reads; the host's file mappings and SysV attachments; recognized as a reaper alive and not stopped after gwz died, `lsof` listing the host's mapped files); BR1 lists it; §3.8 and §8 row 6's README sentence name the handlers and the mappings. **Deferred:** reforking the Linux reaper through `_Fork` (glibc 2.34+) or a raw `clone(SIGCHLD)` changes the reaper's fork call, which this capped round does not take; D, which owns the accepted-residual list (§0), puts it to the operator with R-B20, as with OD-B4; if taken, D amends §3.4 "Creation", R-B20 and B7 and reviews that in its own loop | B7 (M, L): with a blocking `pthread_atfork` child handler registered before `create`, whether the reaper still ends a child at the harness's SIGKILL is recorded, not asserted (the reviewer's Linux assertion follows only if D's operator answer takes `_Fork`) | §1 BR1; §3.4 "Launch contract", "Cost"; §3.8; §7 R-B20; §5 B7; §8 row 6 |
| C3-N-P3-1 | 2.3r-2 needs a `pub use` in the trunk's `transport_host/mod.rs` and its checks' tests; "Waves" says its files are outside the trunk's | Fix-text (cells) | 2.3r-2's Owns adds `src/transport_host/mod.rs` (one `pub use` of `process_safety_with_children` beside `:32`, asked of the trunk, plan :330-331) and `scripts/checks/test_check_spawn_gate_minting.py`, `test_check_process_globals.py`; "Waves" names the one exception (the trunk's, plan :325; 2.3g's and 2.3k's, 2.3 plan :246) and asks for its line | §4 lists the three files; the placement sentence names the trunk file as asked for | §4 2.3r-2, "Waves" |
| C3-N-P3-2 | §2's `sspi.rs:77` claim is false; gwz-py's route to the three spawn sites is unnamed; "8 lines" is wrong | Fix-text (cells) | §2 "The gate": `NativeHost::backend()` (`sspi.rs:76-80`) carries the gate `NativeHost::new` takes from the registry-less `process_safety()` (`:32-34`) to gwz-py's commit, tag and log (`gwz-py/native/src/client_host.rs:46, :52-53`; `shims.rs:74`); gwz-cli's network route calls `NativeHost::new` too (gwz-cli `src/globalargs/dispatch.rs:61`); gwz-py's hostless production lines are the two non-candidate ones (`client_host.rs:66`, `route/native.rs:46`). §4 3.2: the Client's `NativeRoute` is built with `NativeHost::with_process_safety(worker, &gate)` on the registry's `ProcessGate`; the two non-candidate lines stay unregistered ("Interim"). §4 3.1: the host through `NativeHost::with_process_safety` in place of `dispatch.rs:61`'s `NativeHost::new`. OD-B2's premise and §8 row 5 restated | §2 and §4 3.2 name `client_host.rs:46` and the `NativeHost` route; B19's commit and log legs trace to the composition's registry (row text unchanged) | §2 "The gate"; §4 3.1, 3.2; §6 OD-B2; §8 row 5 |
| C3-N-P3-3 | BR1's and BR4's exception lists are narrower than §7 | Fix-text (cells) | BR4: "except in R-B1's cases" with the three cases listed; BR1 adds R-B2, R-B3, R-B14, R-B15 (and the new R-B20). §3.3 "Why no kill…" cites BR4 | Every §7 residual in which a registered child outlives gwz, or a kill can reach a reused ID, is cited by BR1 or BR4 | §1 BR1, BR4; §3.3 |
| C3-N-P3-4 | R-B4 and OD-B1 omit a gwz-py host's default CTRL_BREAK (A R15) | Fix-text (cells) | R-B4 and OD-B1 add "a gwz-py host's default CTRL_BREAK (A R15)"; B29's W leg records CTRL_BREAK and the console close, not asserted; §8 row 6's README sentence adds Windows CTRL_BREAK | R-B4 cites A R15 for Windows; B29's W leg records CTRL_BREAK | §6 OD-B1; §7 R-B4; §5 B29; §8 row 6 |
| C3-N-P3-5 | The crate's inventory classification is unstated | Fix-text (cell), a proposal for lane C | 2.3r-1's Owns names the entry: `gwz-children`, directory `children`, role `integration` (documented concrete OS dependencies; it implements no named contract, so not `implementation`), owner `CS` (as `gwz-session-host`), a one-line rationale, edges `libc`, `windows-sys`, `tokio`; lane C reviews the entry (LBT-012, the inventory's description), named in the brief | §4 2.3r-1 names the role and owner | §4 2.3r-1 |
| C3-N-P3-6 | The 2.3 plan's critical path (:234) and "about 4,300" (:71) are left stale; §8 row 5's operation is ambiguous | Fix-text (cells) | §8 row 3 amends :234 (the longer of the old path and 2.3e, 2.3r-1, 2.3r-2 (after 2.3f, 2.3s), 2.3j, 2.3l; off it 2.3a, 2.3b, 2.3c); §8 row 4 amends :71 to point at §4's totals sentence (so A §8 row 7's totals carry through in either order); §8 row 5 says "Append to" for 3.1, 3.2 and 3.4 | §8 names :234 and :71; row 5 says "Append" | §8 rows 3, 4, 5 |

Counts (10 rows): Fix-text 9; resolved in part and deferred in part 1 (S3-N-P3-3, owner D). No Dispute. Not taken, as
not findings: Safety §4's optional notes (a bounded Force wait for the killed `Kill` entries' unregistration; a
SIGCHLD `SIG_DFL` reset in `run`, which needs A's `signal` kind); Consistency §4's notes that A r3 cites B by stale
line numbers (A's fix) and that the new crate's first crates.io publish belongs in the 2.3r-1 brief.

- **Verification.** Every citation this pass adds or changes was re-read: gwz-core `b745f1fa` (`git show`):
  `src/git/endpoint/https_worker/native/sspi.rs:32-34` (`new` calls `with_process_safety(worker,
  &process_safety())`), `:38-49` (`with_process_safety`), `:76-80` (`backend()` sets the host's gate);
  `src/transport_host/mod.rs:12` (`pub(crate) mod composition`), `:32` (`pub use composition::{ProcessGate,
  process_safety}`); `scripts/checks/test_check_spawn_gate_minting.py` and `test_check_process_globals.py` exist;
  `local_clone_inventory.json` (roles, `role_edges`, `gwz-session-host` owner `CS` role `integration`, the LBT-012
  sentence). gwz-py `2185cc6`: `native/src/client_host.rs:45-47` (`NativeHost::new`), `:52-53` (`own_backend` returns
  `route.backend()`), `:66` (non-candidate `Git2Backend::new()`); `shims.rs:74`; `route/native.rs:1-3` (a build
  without the transport), `:46`. gwz-cli `c0ba717`: `src/globalargs/dispatch.rs:61` (`NativeHost::new`). Root
  `6d20ef6f`: 2.3 plan :71, :202, :229 (2.3l after 2.3j), :234, :246; plan :324-331. std 1.95.0:
  `sys/process/unix/unix.rs:187-189` (`libc::fork()`), `:352-353`. A revision 3 in the lane (hash `3760dd61`,
  unchanged during this pass): §3.1 "Unix" (SIGTERM caught under O4 (a)), §3.4 "Force" step 4, §3.8 and R15
  (SIGTERM, SIGHUP, CTRL_BREAK, CTRL_CLOSE keep Python's default), §4 points (1), (6).
- **Hand-offs to design A** (no A text needs to change): (1) A's Force step 4 "every `Kill` child ends now" now holds
  for a `Kill` spawn in flight too, because `kill_killable()` waits for it under the creation lock; the wait is bounded
  by std's spawn except under R-B2's stopped reaper, which R-B2 already names "gwz blocked". (2) B now cites A R15 for
  a gwz-py host's CTRL_BREAK (R-B4, OD-B1, B29). (3) A §4 point (6)'s PID check and B's widened ignore set are
  consistent: B still relies on neither A's handler nor its PID check in the child. (4) A cites B by section; B's
  section names are unchanged (§3.2 gains a bullet, §3.7 is a table with rows "A" and "C").
- **Re-review.** Safety's pre-commit: a reviewer confirms revision 3.1 against it (N-P2-1 closed as its correction
  states, no new P0-P2, N-P3-1 to N-P3-3 resolved or deferred with an owner). Consistency needs no re-review for its
  P3s. If the operator takes OD-B4 (b) or (c), or the Linux reaper fork under R-B20, D amends B in its own loop.
