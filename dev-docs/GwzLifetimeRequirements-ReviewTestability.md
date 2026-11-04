**Verdict: NO-GO**

# Lifetime requirements r1 — Testability-AXIS REVIEW

## 0 Evidence

- Object: `/Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md`. It is untracked in the lane at `e63509c2`.
- Pin: `git hash-object` returned `34680949ed430a85ce1710064c0b2ef46c067ecc` at the start and the same value at the end. The object did not change during the review.
- Revision mismatch: the brief calls the object "revision 1". The file's own status line (:5) says "DRAFT, revision 2, 2026-11-03 (completeness fix pass on revision 1)". This review covers the pinned blob above, whatever its revision label. The report title keeps "r1" because the brief prescribes it.
- Read in full: the object (:1-851).
- Sources checked:
  - Plan §3 rows R1-R9 and pass rules (:160-182, :276-290).
  - D-V (:1030-1110).
  - CT §10 (:664-676) and :183-195.
  - SD §8 (:420-445) and :56.
  - SCC §4.3-§4.4 (:232-250).
  - SRV §16 item 7 (:1205-1230).
  - B (`GwzChildLifetimeDesign-DRAFT.md`) :139-147, :270, :313-340.
  - IL §10 (:292-304).
- Code checked:
  - `gwz-core/src/durable_fs.rs:15-21`
  - `gwz-core/src/filesystem/native.rs:134-145`
  - `gwz-core/src/checked_artifact/residue.rs:591-600`
  - `gwz-core/src/session_host/context.rs:75-80`
  - `gwz-transport/crates/gwz-connection/src/ports.rs:42-50`
  - `gwz-core/src/transport_host/connection_host.rs:11, :34-54, :307`
  - the Cargo profiles (no `panic` setting in any member).
- Platform facts:
  - [V] macOS `clock_gettime(3)`: CLOCK_UPTIME_RAW "does not increment while the system is asleep".
  - (ext) OpenSSH `clientloop.c`: it installs its own handler for SIGHUP, SIGINT, SIGQUIT and SIGTERM unless that signal was inherited as ignored, and quits on any of them.
  - (ext) Rust std `Instant` uses CLOCK_UPTIME_RAW on macOS and CLOCK_MONOTONIC on Linux; neither counts suspend.
  - (ext) macOS `task_for_pid` needs root or a debugger entitlement.
  - (ext) Linux fanotify, macOS Endpoint Security and Windows minifilters are the only synchronous file-operation interception points; inotify and FSEvents are asynchronous.
- Nothing was modified. No git mutation and no build was run.

## 1 Findings

### T-1 (P2) LR-LIVE-1 cannot hold for links L6 and L9, and cannot be tested there

- **Location:** LR-LIVE-1 (:335); link table L6, L9 (:327, :330); L2 W cell (:323).
- **Problem:** LR-LIVE-1 requires each side to detect the other's death "through an OS event that the death itself produces". Its test is "kill each party in each link".
  - L6 (session ↔ worker thread): detection is by a terminal report or a revoked gate. Neither is an OS event, and a thread cannot be killed.
  - L9 (PYM ↔ interpreter): detection is by atexit or finalization. Neither is an OS event. Only the SIGKILL half of L9 is excepted (NG-25 (c)).
  - L2 on W: the cover is a job, so "the cover's exit or EOF" has no W meaning, yet the matrix marks LR-LIVE-6 W as n/a.
- **Evidence:** the link table at :320-331; the exception clause at :335 names only NG-4, NG-21 and NG-25.
- **Correction:**
  - Limit LR-LIVE-1 to cross-process links (L1-L5, L7, L8, L10).
  - Give L6 its own in-process rule: a worker's end is observed through an owned join or channel handle, never by polling. Verify it with U.
  - Give L9 the atexit rule (LR-EMB-3), not an OS event.
  - Mark L2/W as "n/a: the job is the cover".
- **Check:** every remaining LR-LIVE-1 cell can be killed by a P test.

### T-2 (P2) "Within B-COVER" is a latency bound that no gwz timer enforces, has no value, and can only be asserted with a sleep

- **Location:** LR-PROC-2 (:349) and its verify cell ("X: no K marker process B-COVER after gwz's death"); E09 (:182); §4 table (:240); bounds table B-COVER (:455); LR-BND-3 (:419).
- **Problem:**
  - LR-BND-3 says each bound is "enforced only by its owner's timer". B-COVER is how quickly the reaper reacts after it sees EOF. That is OS scheduling, and no timer owns it.
  - The verify cell has to wait B-COVER and then look for absence. That is a sleep used as a pass condition, which LR-TEST-2 (:536) forbids.
  - B-COVER also has no value (OQ-28), so a MUST pass condition depends on a non-normative number.
- **Correction:**
  - State the MUST as event-ordered: each K child ends after gwz's death with no further event needed. Verify it with P: wait on each K child's pinned exit (pidfd or kqueue; on W, the job's process list goes empty), under a hang guard only.
  - Move the latency to an E row or a SHOULD measured against a baseline.
  - Remove B-COVER from LR-BND-3's owner-timer table.
- **Check:** no MUST verify cell contains "within B-x after" for a bound that no timer owns.

### T-3 (P2) The file-event crash cuts for libgit2 and Git writes are not deterministic

- **Location:** LR-REC-1 (:385); LR-TEST-6 (:540); matrix LR-REC-1… "file-event cuts" (:601).
- **Problem:** the harness "kills on file events [it] observes". inotify and FSEvents report events after the fact, so by the time SIGKILL lands the writer has moved on. Where the cut falls depends on load, which makes this a Z-class stochastic run passed off as K. The same applies to a Git child. Neither libgit2's posix layer nor Git offers a pause hook. Synchronous interception (fanotify permission events, Endpoint Security AUTH, minifilters) needs root, an entitlement or a driver on CI.
- **Correction:**
  - For writers gwz does not control, verify by **seeded state**: the test writes each enumerated leftover state directly (lock present, temp present, pack without refs, and so on), then asserts recognition and the remedy. That is U or K-recovery, and deterministic.
  - Reclassify file-event kills as Z, a supplement only.
  - Keep K at step points for gwz-owned writes.
- **Check:** every libgit2/Git sequence in LR-REC-1 has one seeded-state row per enumerated state, and no MUST depends on a file-event kill.

### T-4 (P2) LR-PROC-5 conflicts with the ssh dispositions that LR-PROC-9 and SRV require

- **Location:** LR-PROC-5 (:352) vs LR-PROC-9 (:356) and §1 SSH (:115).
- **Problem:** LR-PROC-5 says "A child starts with gwz-caught signals at their default". gwz catches SIGINT (LR-EXIT-1). SRV:1218 requires `ssh` to start with SIGINT and SIGQUIT **ignored**, because "OpenSSH's client loop keeps an inherited ignore". B already provides this as `Process { ignore_interrupts }` (B:313). A test written to LR-PROC-5 fails a correct ssh spawn; one written to LR-PROC-9 fails LR-PROC-5.
- **Correction:** amend LR-PROC-5 to "…at their default, except dispositions the child's class declares (Unix `ssh`: SIGINT and SIGQUIT ignored)". Verify with P: read the dispositions of the spawned ssh, and the default for every other class.
- **Check:** no two rows give one spawn contradictory dispositions.

### T-5 (P2) E06 (hang-up) is tiered O-clean, but on Unix it ends the ssh carrier exactly as E05 does

- **Location:** E06 (:178); event assignment (:253); E05 (:177); LR-PROC-9.
- **Problem:**
  - `ssh` shares gwz's foreground group. Only SIGINT and SIGQUIT are ignored.
  - On hang-up the shell or kernel sends SIGHUP to that group. OpenSSH's handler sets quit (ext: `clientloop.c`), so the session ends without cancel-and-wait.
  - This is exactly R-C4, which E05 marks "O (pending OQ-24)". E06 says "As E04" and is placed in O-clean without that caveat.
  - Because LR-PROC-5 resets caught signals to default, gwz itself guarantees ssh installs its SIGHUP handler.
  - The same applies to SIGTSTP (E08): ssh stops, which E08 does not mention.
- **Correction:**
  - Tier E06 for the served-ssh form as E05 (pending OQ-24), and add E06 to OQ-24's "Bears on".
  - Or, if the operator wants E06 O-clean, require SIGHUP (and SIGTERM) ignored in the ssh spawn and add an O row for OpenSSH's inherited-ignore behaviour.
  - Add ssh's SIGTSTP to E08.
- **Check:** a P (pty) row hangs up during a served fetch over a real `ssh` and asserts the tier's outcome.

### T-6 (P2) Silent loss of an idle connection is promised but no OS event exists for it

- **Location:** E17 (:194, "An idle connection lost is discarded before reuse", O-clean); LR-LIVE-2 (:336); LR-LIVE-7 (:341); link L7 (:328).
- **Problem:**
  - L7 Unix detects idle death by FIN or RST readiness. A peer that vanishes silently sends neither. Examples: a Wi-Fi or VPN switch leaving a stale source address, a NAT timeout, a remote power-off.
  - LR-LIVE-2 does not except L7/Unix, so it requires something no OS event can deliver.
  - At reuse, the first request bytes go into the kernel buffer, so LR-LIVE-7's retry is barred. The operation then fails `Stalled` at B-SETUP. E17's "discarded before reuse" and O-clean are not honest for this case.
  - The verify cells ("peer closes idle") test only FIN.
- **Correction:**
  - Add an NG or narrow E17 and LR-LIVE-2 to "loss the peer signals (FIN, RST, ICMP)".
  - State the silent case's outcome as a typed stall failure with no replay, bounded by B-IDLE exposure.
  - Add a P/F row with a black-holed peer (drop without RST).
- **Check:** an F network port drops packets silently; assert the stated outcome.

### T-7 (P2) LR-CLEAN-2's single B-CLEAN bound contradicts the session close_wait and the server stop bound

- **Location:** LR-CLEAN-2 (:368); §4 "gwz process exit within B-CLEAN" for server forms (:239, :251); B-CLOSEW 60 s, B-STOP 185 s (:439, :444); LR-EXIT-8.
- **Problem:**
  - LR-CLEAN-2 says each owner's close is bounded by one overall B-CLEAN (5 s).
  - Sessions are owners in the scope list (:87). SD §8 step 4 waits up to `close_wait` (60 s) before it detaches, and SCC 4.3 adds "one more bound" for detached workers.
  - Code agrees with the split: `CLEANUP_BOUND` of 5 s is the host context's own bound (`context.rs:75-80`), not the session's.
  - A U test with a stepped clock and a worker that ignores cancel for 30 s therefore has two oracles: retain at 5 s, or wait to 60 s.
  - LR-BND-8 already admits that "2 x close_wait" cannot fit a supervisor window.
- **Correction:**
  - Scope LR-CLEAN-2 to the host or composition owner.
  - State the session rule as "one overall B-CLOSEW, then detach (LR-OWN-4)".
  - Change §4's exit property for server forms to "within B-STOP, excluding U-waits".
- **Check:** every owner kind in :87 maps to exactly one named bound.

### T-8 (P2) LR-OWN-11 forbids the concurrent connections that R1 and D-M allow

- **Location:** LR-OWN-11 (:278); matrix "E U" (:562).
- **Problem:** "After a command's first exchange to a host, its later exchanges to that host create no connection, except replacements of idle-closed ones." A 16-member fetch runs exchanges concurrently up to CAP (per-host default 32, D-M), and each needs its own connection. R1's pass rule allows "creations ≤ min(CAP, members)", and none only "after the fetch has created its connections" (Plan:280). The U creation-counter test written to LR-OWN-11 fails a correct pool, while the E row passes.
- **Correction:** restate it as R1 does: creations ≤ min(CAP, concurrent demand); after the first phase that reached its peak, later phases create none, idle-closed replacements excepted.
- **Check:** the U oracle equals R1's pass rule.

### T-9 (P2) LR-REC-3 and LR-REC-11 rest on a wrong Windows fact, and every Windows command would warn

- **Location:** LR-REC-3 (:387, "on W no directory-sync primitive exists"); LR-REC-11 (:395, "W directory sync" in the degraded set, "warns once per command"); NG-3; OQ-7; matrix :603, :607.
- **Problem:** the code's Windows durability model is write-through, not "none":
  - `durable_fs.rs:17`: "Windows supplies the persistence barrier through `MOVEFILE_WRITE_THROUGH`".
  - `filesystem/native.rs:145` uses that flag.
  - `FILE_FLAG_WRITE_THROUGH` is used at `handles.rs:92` and `publication.rs:29`.
  - `residue.rs:591-595` cites W3 of `GwzWindowsMatrix-Classification.md`.

  Under LR-REC-11 as written, every Windows command prints a durability-degraded warning. OQ-7 also puts a question to the operator on a false premise. The listed O row ("W directory and rename durability") would only document the fact; it cannot fail if gwz drops write-through.
- **Correction:**
  - State Windows' primitive (write-through create and rename) as an O row first.
  - Remove W from LR-REC-11's degraded list unless that O row shows a gap.
  - Add a U row where the fake filesystem records the barrier primitive used per platform: F_FULLFSYNC on M (std `sync_all`), fsync on L, write-through on W. That row fails when the wrong primitive is used.
- **Check:** LR-REC-11's U row on W emits no warning on NTFS.

### T-10 (P2) LR-BND-7 claims more than a monotonic clock gives, and an F-only test cannot catch it

- **Location:** LR-BND-7 (:423); E28 (:214); matrix F only (:613).
- **Problem:**
  - "Deadlines use a monotonic clock. A suspend/resume … expires idle connections past their bound before any lease."
  - On macOS the monotonic clock std uses does not advance during sleep ([V] `clock_gettime(3)`: CLOCK_UPTIME_RAW). Linux CLOCK_MONOTONIC behaves the same (ext).
  - CT §10 (:670-672) and the code (`connection_host.rs:11, :34-54`) detect suspend only by a wall-versus-monotonic difference **above a threshold**. `ports.rs:47-49` adds: "A clock that cannot see either returns a constant."
  - A suspend below the threshold therefore leaves overage idle entries leasable. An F row with a fake clock cannot detect that the real clock omits suspend.
- **Correction:**
  - Restate it as CT does: a sleep-inclusive clock where available, otherwise a discrepancy above a named threshold.
  - Name that threshold as a bound.
  - Add an O row per platform (does the chosen clock count suspend?).
  - Add a U row at threshold − 1 and threshold + 1.
- **Check:** the bounds table names the discrepancy threshold.

### T-11 (P2) The acceptance matrix drops the operator's R7 cross-process proof and R8, and has no route to test the ssh form

- **Location:** matrix LR-OWN-6, LR-CRED-1 "U S" (:559); LR-CAN-1 verify (:287); §10.1 (no R8 or R9 row); LR-TEST-10 (:544).
- **Problem:**
  - R7's pass rule is "two processes, scrubbed core (§4 step 2.5); CI; `connect()` count in the core process = 0" (Plan:285), proven per platform. The matrix has no P cell for it.
  - R8 (installed artifacts; a two-machine run over `ssh … gwz server stdio`) is untraced.
  - LR-CAN-1's case set lists "in-process, local socket, stdio and a carrier fake" and leaves out the ssh form, which OD12 puts in scope.
  - LR-PROC-9, LR-PROC-6 (kill `ssh` at a prompt) and L5 depend on real OpenSSH behaviour. That needs an `sshd` on M, L and W CI hosts, or an O row for OpenSSH's signal handling plus a stub. No LR-TEST row requires either.
- **Correction:**
  - Add P cells for LR-OWN-6 and LR-CRED-1 per R7.
  - Trace R8 (and say R9 is not lifetime-relevant).
  - Add the ssh form to LR-CAN-1's case set.
  - Add an LR-TEST row for a localhost `sshd` per platform (or OS-fact rows for ssh dispositions plus a stub `ssh`).
- **Check:** §10.1 lists R1-R9; every OD12-form MUST has a real-ssh or O-backed cell.

### T-12 (P2) No row tests the shipped binary: every P test runs the re-exec test binary

- **Location:** LR-TEST-4 (:538); LR-TEST-9 (:543).
- **Problem:**
  - Exit status (LR-EXIT-4), dispositions at start (LR-EXIT-1), Windows `user32` loading (OQ-26), the panic strategy (LR-EXIT-7; no `panic` profile is set today, so a later `panic = "abort"` in `profile.dist` would turn E11 into E12) and console behaviour are all properties of the shipped `gwz` executable and the gwz-py wheel.
  - The re-exec harness exercises the test binary only.
  - Parity rows run the **old** shipped binary (LR-TEST-9), but no MUST row runs the **new** one.
  - LR-TEST-4 is also stated as exclusive ("use a re-exec harness"), yet rows need real `git`, `ssh`, `less`, bash, cmd/PowerShell and Python.
- **Correction:**
  - Add an artifact row: LR-EXIT-1, -4, -6 and LR-EMB-9 smoke rows against the installed CLI and wheel on M, L and W (R8's artifacts).
  - Restate LR-TEST-4 as "gwz's own roles re-exec; external programs are real, with their versions recorded".
- **Check:** each LR-EXIT MUST has at least one artifact cell per platform.

### T-13 (P2) LR-CRED-3's memory-marker check is infeasible on macOS

- **Location:** LR-CRED-3 verify "U, P (inspect environment and memory markers)" (:405); matrix "U P K" on M (:609).
- **Problem:** reading another process's memory on macOS needs `task_for_pid`, which needs root or a debugger entitlement (ext). That is not available to a CI test binary. The M cell cannot be built as written.
- **Correction:**
  - Verify the ordering instead: U with a credential-source port that records its first use after the cover-fork step point.
  - Inspect environments with `KERN_PROCARGS2` (M), `/proc/<pid>/environ` (L) and the PEB read (W).
  - Make memory scans L/W only, or a self-scan in the cover role using a derived (non-literal) canary.
- **Check:** every M cell in the matrix names a mechanism usable without privilege.

### T-14 (P2) §9 adds an intent-durability obligation that §5 does not state, and the trigger set is inconsistent

- **Location:** §1 L3 (:131, "multi-file or Git-locking"); LR-OWN-3 (:270, "multi-file"); LR-REC-3 (:387, no trigger); §9.1 P3 (:639, "multi-file, Git-locking **or temp-creating** sequence").
- **Problem:**
  - Three different trigger sets exist, and LR-REC-3 defines none, so "S: every named step point has a K row" has no defined scope.
  - §9 widens the set to every temp-creating sequence, which is every fetch, since libgit2 writes pack temps. That adds a full-flush barrier per member per fetch (F_FULLFSYNC on M) on R1/R2's measured path, with no requirement or operator source behind it.
- **Correction:**
  - Put one normative trigger set in LR-REC-3, with its source or a PROP mark.
  - Make §9 P3 cite it rather than extend it.
  - If temp-creating sequences are meant, mark that PROP and add an E row for its R1/R2 cost.
- **Check:** grep finds one trigger definition.

### T-15 (P2) LR-CAN-12 demotes part of R3's operator pass rule to a SHOULD

- **Location:** LR-CAN-12 (:298, SHOULD).
- **Problem:** R3's pass rule includes "cancel-to-effect ≤ `___` ms (gapsa method)" (Plan:282), and "a GO without the row's numbers is not a GO" (Plan:178). Making it a SHOULD mis-levels an operator gate and leaves the acceptance matrix without a gating row for it.
- **Correction:** make it a MUST once Phase 0 fixes the number, citing Plan:282's method; matrix E then U/P.
- **Check:** every numeric column of R1-R7 maps to a MUST row.

### T-16 (P3) Several verify cells contradict LR-TEST-2 or LR-TEST-5

- **LR-LIVE-5 (:339):** "flat CPU" is a timed measurement. Replace it with an observer wake counter that stays at 0 across a stop and continue pinned by handshake.
- **LR-TEST-5 (:539):** requires "every §3 event has a U, P or K row", but E13 (power loss) and E28 (suspend) have only F, O or E rows. Admit F there.
- **LR-TEST-3 (:537):** bans retries, but the E gates (LR-EMB-7's median-vs-p90, LR-CLEAN-11) are statistical. State that E rows follow the Plan's rounds-and-pairing pass rules (Plan:288ff), so noise is not a "defect".

### T-17 (P3) Some S checks cannot verify what they claim

- **LR-OWN-3 (:270):** "S for order" cannot see runtime creation order across call sites. Use a U recording port.
- **LR-REC-12 (:396):** "S: every recovery entry is on the list" needs a defined syntactic marker for a "recovery entry".
- **LR-CRED-2 (:404):** zeroizing storage is not covered by its verify cell. Add an S check that secret types are only `Zeroizing` or wrapper types.

### T-18 (P3) Windows analogues are marked n/a or missing

- **LR-PROC-5, W "n/a" (:592):** a W child inherits the CTRL_C-ignore flag, and CREATE_NEW_PROCESS_GROUP disables CTRL_C in it, so there is a disposition-inheritance rule to state.
- **LR-CONC-7 (:481):** worded for Unix only. `GenerateConsoleCtrlEvent(…, 0)` is the W equivalent of signalling the whole console, and the "S U" W cell has nothing named to check. Add "never GenerateConsoleCtrlEvent to group 0".

### T-19 (P3) The census marker can be scrubbed away

- **Location:** LR-TEST-7 (:541) vs LR-PROC-14 (:361).
- **Problem:** K children's environment is "scrubbed by class", so an environment-tag marker can be removed and the census then misses a leaked child.
- **Correction:** require the marker to be session ID (Unix) or job (W), or put the tag on the scrub allow-list in test builds only.

### T-20 (P3) LR-LIVE-4 reads as forbidding what NG-7 accepts

- **Location:** LR-LIVE-4 (:338) "fork-without-exec copies of gwz's own process" vs NG-7.
- **Problem:** the phrase can mean the application's fork, which NG-7 accepts, or the reaper fork, which must hold. A test author picking the first writes a test that fails by design.
- **Correction:** say "the cover's own fork". Mark the M cell for the CLOEXEC window as pending OQ-12, since NG-18 conflicts by the document's own statement.

### T-21 (P3) LR-OWN-9's matrix code and Linux floors are unstated

- **LR-OWN-9 (:276, :561):** the matrix says "U", but the cases need real case-sensitive APFS images, Linux defaults and W per-directory case sensitivity (`fsutil setCaseSensitiveInfo`, which needs the WSL component (ext)). The rows are environment-dependent P or O rows.
- **LR-TEST-11 (:545):** records the kernel version (pidfd 5.3, `SO_PEERPIDFD` 6.5) but sets no floor or fallback. Results then vary by CI kernel.

### T-22 (P3) A single LR-OBS-7 remedy cannot be paste-safe in both cmd and PowerShell

- **Location:** LR-OBS-7 (:467).
- **Problem:** quoting rules and wildcard handling differ (PowerShell needs `-LiteralPath`; `$` and backtick expand). A U test has no oracle unless the shell is named.
- **Correction:** emit one remedy per shell, or a `gwz` subcommand remedy.

### T-23 (P3) §9.3 counts R-B15 and macOS R-B3 as "Disappear" when that depends on an open question

- **Location:** §9.3 (:670).
- **Problem:** the disappearance presumes OQ-12 (b). It also ignores forks gwz does not own: NG-16 third-party spawns, and an embedding application's `popen` in PYM.
- **Correction:** move them to "Disappear if the operator chooses (OQ-12 (b)); NG-16 still applies".

Pre-commitment: I will return GO on a revision that resolves T-1 to T-15. The P3s are recommended but do not block.

## 2 Coverage map checked

- **Matrix completeness:** every MUST and SHOULD ID in §5.1-§5.12 appears in a §8 row, via its own row or a grouped one. Grouped placements: LR-CAN-6 with OWN-5; LR-CLEAN-8 with OWN-10; LR-CLEAN-11 with CAN-12; LR-CRED-1 with OWN-6; LR-EMB-8 with OWN-7. LR-TEST rows sit outside the matrix by design (:551).
- **Row-versus-matrix agreement:** checked for each row. The defects are T-11 (R7 P missing), T-13 (M memory scan), T-21 (OWN-9 coded U) and T-18 (W analogues). The other differences only add codes (for example LR-REC-2 gains F), which is harmless.
- **Tiers (§3, §4):** checked per event against platform facts. Mis-tiered or overclaimed: E06 (T-5), E17 (T-6), E28 (T-10), the server-form exit property (T-7). Honest: E15 and E16 (O-forced → C, `user32` disclosed), E05 (pending), E09, E10, E12, E13.
- **Feasibility per platform:** W console injection (GenerateConsoleCtrlEvent via a console-holder role; console close) is feasible. LOGOFF and SHUTDOWN by U and O are honest. Job-at-creation on W (PROC_THREAD_ATTRIBUTE_JOB_LIST, Windows 10 and Server 2016) is feasible within LR-TEST-11's floor. LR-LIVE-8's race can be closed by the server lock (SCC 4.3 "under the lock").
- **Traceability, testability side:** R1-R7 are mapped; R8 and R9 are not (T-11). R3's numeric gate is demoted (T-15).
- **§9:** checked for obligations it adds or decisions it presumes (T-14, T-23). P1, P2 and P4-P9 follow from their groups.

## 3 Risks

- **CI host capability:** several W rows need a console the harness can close and a process it can suspend, on hosted runners in session 0. Real-ssh rows need `sshd` with admin setup on M and W. These should be checked by feasibility spikes before the matrix is frozen.
- **std::process on W:** stable std cannot pass attribute lists, so LR-PROC-7's job-from-first-instruction requires gwz's own `CreateProcessW` path for every W spawn. That includes git2-rs's native-route helpers, which NG-15 leaves outside until OQ-3 is decided.
- **Unvalued bounds:** five bounds have no value (OQ-28), and LR-TEST-1 makes their rows gates. Until Phase-0-style baselines exist, those MUST rows cannot pass or fail, and the release gate count is open.
- **Full-flush cost on macOS:** the memory notes record that suites wait on serialized F_FULLFSYNC. Any widening of intent durability (T-14) directly lengthens the lifetime suites and the R1/R2 timings.
