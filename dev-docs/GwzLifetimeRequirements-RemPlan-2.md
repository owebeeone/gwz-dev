# GwzLifetimeRequirements: remediation plan, round 2

Date: 2026-11-04. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 4 of the lifetime
requirements document.** Drafted in the lane `/Volumes/projects/limbo/gwz-dev-docs-req`, writing only this file and the
document. No git or gwz operation was run (the blob hashes below come from the read-only `git hash-object`); nothing was
written in `/Volumes/projects/limbo/gwz-dev`. This is the last allowed remediation round: a third round may follow only for
non-architectural corrections.

- **Object:** `dev-docs/GwzLifetimeRequirements.md`, revision 3, blob `ef534435` (uncommitted in the lane), the blob both round-2
  re-verdicts pinned.
- **Re-verdicts (not edited):** [Completeness-2](GwzLifetimeRequirements-ReviewCompleteness-2.md), NO-GO: N-1..N-4 P2, N-5..N-7 P3,
  C-13 partial. [Testability-2](GwzLifetimeRequirements-ReviewTestability-2.md), NO-GO: T2-1..T2-6 P2, T2-7..T2-12 P3, T-13, T-14,
  T-17 partial. 23 open items, no P0 or P1. Round 1: [RemPlan](GwzLifetimeRequirements-RemPlan.md).
- **Result:** revision 4, edited in place, 928 lines (cap 1,200), status line "DRAFT, revision 4, 2026-11-04 (round-2
  remediation)", blob `af4c6079428e25c9d8281aab2ac92a1f46661c29` at the time of writing (a later edit by the lane owner changes
  it). Counts: 148 requirements (140 MUST, 8 SHOULD, unchanged), 28 non-goals, 35 open questions (was 34), 42 failure events,
  10 declared departures (was 7), 42 PROP requirement rows (was 39).
- **Dispositions:** fixed in text 23 of 23 (every P2, every P3, every partial). Disputed 0. Two citation corrections (§5). Where
  a fix differs from the reviewer's proposed correction, the row says so.

## 1 Operator input applied (and one misattribution removed)

The operator's 2.3f decision of 2026-11-03 (round-2 Code P2-1, option (a)) is read at root `GwzServerCore-Step2.3-Plan.md:154`
(commit `8aa5f800`) and root `GwzSplitCommandChannel.md:230` (§4.2 "One server per directory"). Exact content used:

- `ws-<key>` stays the first 16 hex digits of the SHA-256 of the canonical workspace root. A grant names one workspace path
  for its whole life.
- "filesystem identity keys only the member lock" (P23:154).
- The server also holds an exclusive start lock keyed by `workspace_identity` in the per-user directory for its whole life,
  `id-<identity key>.lock`, holding the first server's address; a second start through another spelling refuses as held and
  names that address; an inode reused after the first server exits finds the lock free, so the lock carries no grant.
- The decision does not key the **detached-worker record** by identity. Revision 3 attributed that clause to the decision (N-3).
  Revision 4 marks it **PROP** with its code evidence (`gwz-core/crates/session-host/src/detached.rs:26-27` keys the record by a
  `String`; `src/session_host/served.rs:265-298` passes the served root's display string) and lists it in OQ-16.
- The Windows production per-user directory (P2-4) belongs to step 2.3i (P23:154); the document does not touch it.

No other operator decision was invented. Everything that needs one is an open question with options and a recommendation.

## 2 Disposition table

"Where" gives revision-4 line numbers. Every row is FIXED. "+OQ" means the fix also raises or changes an open question or a
departure row the operator decides.

| ID | Sev | Finding | Disposition and where | Check | Result |
|---|---|---|---|---|---|
| N-1 | P2 | The narrowed record trigger set contradicts five "recorded" obligations (LR-REC-5, §4, E02, E20, OQ-1) | FIXED +OQ. One rule, defined once in LR-REC-5 (l.420): Force's partial state and a failed write's leftover are (a) reported at the stop with the safe-point label (LR-OBS-1) and (b) recognized by the next command, from a record where LR-REC-3's trigger set applies, otherwise by shape (LR-REC-1); no record is written at stop time. Restated to cite it: §4 O-forced (l.254), the Tier C definition (l.256, now "where LR-REC-3's trigger set applies"), both §4 table rows (l.273-274), E02 (l.195), E20 (l.224), R-TREE recognizer (l.172), OQ-1 (l.894). OQ-32 (l.925) now says what option (a) means for LR-REC-5 and LR-REC-7: a stopped state indistinguishable from the user's edits is reported at the stop only | `grep -n "recorded\|intent record\|from the intent"`: 16 hits at l.45 52 57 69 99 256 296 306 316 320 576 583 777 812 907 915, none a requirement of a record for partial state outside LR-REC-3's trigger set (l.256 is the qualified Tier C sentence; the rest are "operator rules recorded", "versions recorded", the latch's "recorded" cancel, "recorded debt", "unrecorded"); `intent record` and `from the intent`: 0 hits; LR-REC-5, §4, E02, E20 and OQ-1 now cite LR-REC-3/OQ-32 | pass |
| N-2 | P2 | Departures register incomplete; OQ-13 and OQ-14 misstate A and SRV:808 | FIXED +OQ. DEP-8 (l.98): A:96-97, :235-236, A:384 exclude `hook` and `claude-code-setup`; LR-EXIT-1 (l.332) now follows A and names OQ-13, LR-EXIT-4 inherits its scope. DEP-9 (l.99): SRV:808 has the W background copy leave the caller's job where the job allows it; NG-2 (l.541) follows SRV:808 and names OQ-14. DEP-10 (l.100): SRV:1115 "the child gets the client's environment" is amended by SCC:208-209; LR-PROC-14 (l.390) restated (no environment crosses the carrier; a local stdio core inherits the client's environment minus the scrub list and `SSH_AUTH_SOCK`). DEP-3 (l.93) extended with SRV:809 and :1107 (CTRL_BREAK), with B:162-165 and C:130; OQ-24 (l.917) cross-refers. OQ-13 (l.906) corrected ("A excludes them (A:96-97, :235-236; it took (b))"; DS S-P2-4 disputes D's premise that M1-M3 recognize what those commands leave). **OQ-14 (l.907): the recommendation changes from (b) to (a)**, because its stated reason, "no source asks for it", was false (SRV:808 asks for (a)); the dropped rule is now a declared departure only if the operator picks (b) | Sweep (§6, check 14): SRV adopted platform table :803-810, SRV §15 (:1068-1118), SRV §6 (:526-584) and A §3.1 (:90-128), A §3.6 (:231-257) against LR-EXIT, LR-PROC, LR-CRED and NG. Differences found: the four above. Examined and not departures: SRV "Its environment is the starting CLI's" for the background copy (amended by SCC:211's scrub); SRV §6 "the extension handles SIGTERM and SIGINT itself" for gwz-py (replaced by D-O, SCC:223); A:235 gives a `server` subcommand's exit status to C, and A:104-106 has C own "130 after any O4 (a) interrupt", which LR-EXIT-4 states; SRV's server-stop signal list names SIGTERM and SIGINT, A:104-105 adds SIGHUP; LR-EXIT-6's B-OUT drop matches A's Force step 8 (A:205-213, "up to 1 s for the writer"). Recommendations re-read against their sources: OQ-5, -13, -14, -30, -31 | pass |
| N-3 | P2 | LR-OWN-9 attributes the detached-worker clause to the operator; start lock missing from R-SRV, cleanup, census | FIXED. LR-OWN-9 (l.303): identity keys the member lock and the start lock `id-<identity key>.lock`; address stays canonical-path keyed; start lock "carries no grant and is free once its server exits"; detached-worker clause marked **PROP** with code evidence in the Source cell; source cell quotes P23:154 and cites root SCC:230. LR-OWN-10 (l.304) cites P23:154 and SCC:230 and adds the refusal naming the first address. Start lock added to: source key P23 (l.44), R-SRV (l.174: states, outlives, Tier C recognizer: the file stays, is not stale, its address is never trusted without LR-LIVE-8's proof), R-LOCK (l.170), LR-OWN-8 (l.302), LR-CLEAN-8 (l.405, with a K row: kill the server, the next start finds both locks free and replaces the address by proof), LR-CONC-5 (l.514), LR-TEST-7's expected file set (l.579), §10.1 row; OQ-16 lists the PROP clause | Every clause of LR-OWN-9 and -10 is in P23:154, in SCC:230, or marked PROP: read line by line. Quotes re-read at root P23:154 and SCC:230 (§6, check 15). `id-<identity key>.lock` appears in R-SRV, LR-OWN-8, -9, LR-CLEAN-8, LR-CONC-5, LR-TEST-7 (check 18) | pass |
| N-4 | P2 | E37 / LR-PROC-15 contradicts LR-PROC-2's exceptions, LR-OWN-8's list, O-clean and the census | FIXED. One rule: the killed-but-uncompleted K child is an LR-OWN-8 entry (l.302: owner the OS, ends when the blocked call returns, reported pending by identity) and the one named exception of LR-PROC-2 (l.378) and LR-PROC-15 (l.391). O-clean and O-forced (l.253-254) no longer say "nothing is left" absolutely: they say "except the entries of LR-OWN-8's closed list, each retained by a named owner or reported pending by identity", which is also what E36 and every bounded close already needed. E37 (l.244), R-PROC "may outlive" (l.165) updated. I did not move E37 to O-forced (the reviewer's alternative): the exit is not forced, and the redefinition makes the three lists agree for every retained item. Census exemption: none needed, because a real uninterruptible sleep cannot be made on demand (LR-TEST-2), so LR-PROC-15 has no P row (stated in its Verify cell) and the census never meets one | LR-PROC-2, LR-OWN-8, §4 and LR-TEST-7 jointly satisfiable: the only U row for E37 uses an injected wait port (no process, so no census); the exception lists name the same child in three places | pass |
| N-5 | P3 | §9.3 "NG-1 to NG-25"; E23 still says "close owners within B-CLEAN" | FIXED. §9.3 (l.727) "NG-1 to NG-28"; E23 (l.229) now equals LR-EMB-3 (`HostOwner` within B-CLEAN, each bridge pump within B-CLOSEW, DEP-2) | Check 19: the range text equals the defined maximum (28). E23 against LR-EMB-3 read | pass |
| N-6 | P3 | OQ-16 omits LR-EXIT-6 and the PROP bound B-TURN / B-LINGER (C-13 partial) | FIXED. OQ-16 (l.909) regenerated from the PROP scan: 42 requirement rows, plus the bounds row B-TURN / B-LINGER named as non-normative until confirmed, plus a sentence listing the four partly-PROP rows (LR-OWN-9, LR-EXIT-6, LR-REC-5, LR-OBS-5) and the clause that is PROP in each | Check 8: PROP scan over the requirement rows and the bounds table equals OQ-16's list (42 of 42; 0 listed that are not PROP; the bounds row found) | pass |
| N-7 | P3 | Verify codes missing from §8 matrix | FIXED. Matrix rows (l.600-671) gained the codes: LR-OWN-7 X, LR-OWN-9 S, LR-PROC-10 and -14 P (row split note), LR-CLEAN-2 S R (own row), LR-REC-12 S (own row), LR-CRED-2 S (own row), LR-CONC-3 U and -2 O (own rows), plus those the new text needs: LR-REC-1 U, LR-REC-5 F U, LR-REC-6 U K, LR-CAN-12, LR-CLEAN-11, LR-EMB-7 U, LR-LIVE-1/-2 U and F | Check 13: for every non-TEST row the Verify codes (E and Z excepted) are a subset of its §8 cells. 17 misses before, 0 after | pass |
| T2-1 | P2 | The residue census cannot see setsid'd or jobless gwz processes (T-19 reopened) | FIXED (churn; §3). LR-TEST-7 (l.579) restated: no gwz-created process survives however placed (own session, group, job or none) or re-parented, the cover, a stdio core, the background server and a W S child named; session, group and job are "never a sufficient marker"; the marker is a per-test tag that the test-build scrub allow-list carries into every class and a forked cover inherits, read from the exec-time environment block (M `KERN_PROCARGS2`, L `/proc/<pid>/environ`, W the PEB), plus on L a child-subreaper harness; a census **self-test** seeds one leak of each kind (a `setsid` child, a forked copy that changed session, a W child outside every gwz job, a background server) and asserts each is reported. The reads and `PR_SET_CHILD_SUBREAPER` are (ext) facts with LR-TEST-10 O rows (l.582) | Self-test specified, not run (the document authorizes no code); the W harness-job alternative is not used because of T2-2 | pass (text) |
| T2-2 | P2 | LR-PROC-2's W check waits on a job process list that the test's own handle keeps alive | FIXED (churn; §3). LR-PROC-2 Verify (l.378): wait on each K child's own process handle opened with `SYNCHRONIZE` at a handshake before the kill; "never the job's process list, because a handle a test holds on gwz's job keeps a kill-on-close job ... alive". LR-PROC-7 Verify (l.383): membership "in gwz's own job" (`IsProcessInJob` against the job handle gwz holds, read in-process through a test seam), never "any job", and the test never holds a handle on gwz's job from outside. LR-TEST-7 does not use a harness job | Read against the round-2 reproduction: the handle waited on is a process handle, which does not keep a job alive; the S child is judged against gwz's job, not a harness job | pass (text) |
| T2-3 | P2 | LR-CRED-3's environment check cannot judge a fork-only cover and contradicts NG-9 (T-13 partial) | FIXED +OQ (churn; §3). LR-CRED-3 (l.436) restated as behaviour: the cover execs nothing and starts nothing, so it passes no environment on, and is created before any credential; a process gwz execs gets a constructed environment with no credential variable and no agent socket; the server scrubs its own environment before any thread; the exec-time strings of a forked cover or a self-scrubbing server are NG-9's inherited copies and are not checked. Verify: exec-time environment read only for exec'd roles (stdio core, background server copy, child classes); the cover and a self-scrubbing server answer a test-seam query of the live environment; canary scan/self-scan kept. NG-9 (l.548) names the exec-time block. New OQ-35 (l.928): accept as NG-9 copies, or require an in-place overwrite (recommendation: accept). SCC:211 itself says a `--foreground` server removes the variables "from its own environment", which confirms that the self-scrubbing case exists | Quotes re-read at SCC:208-211 (§6, check 15); NG-9 and LR-CRED-3 give the inherited exec-time block one verdict | pass (text) |
| T2-4 | P2 | LR-LIVE-1's exceptions name cells that do not exist; the gate fails the operator's interim L7 polls | FIXED. LR-LIVE-1 (l.363): exception list is now NG-21, NG-25, NG-27 (NG-4 dropped: a hung peer is not a death; OQ-4 reworded, l.897); the L7 link cell (l.356) names NG-27 (the HTTPS 2 ms step and the SSH idle watch's bounded park); NG-27's rationale (l.566) reworded as the operator's exception under IL decision 4; Verify gains the L7 oracle "noticed after exactly one supervisor step on a stepped clock"; matrix l.623 gains U and F | Check 16: every NG in the exception clause appears in a link-table cell (NG-21, NG-25, NG-27: 3 of 3) | pass |
| T2-5 | P2 | A second stop-time "recorded" obligation re-widens the trigger set and cannot be met under ENOSPC (T-14 partial) | FIXED with N-1. The reviewer's option (a): the report's safe-point label plus recognition by shape; no stop-time marker, so option (b)'s PROP/OQ is not needed. LR-REC-5's Verify carries the reviewer's regression test: F, ENOSPC on the third file of a multi-file sequence with every further write refused: no partial file published, the report names the safe point, the next command recognizes the state by shape. E20 (l.224) no longer asks for a write on the full disk | `grep "recorded"` shows no multi-file obligation outside LR-REC-3/OQ-32 (N-1 check) | pass |
| T2-6 | P2 | LR-REC-6 binds libgit2 and Git writes that no gating test reaches ("where the CI host provides one") | FIXED (reviewer's first option). LR-REC-6 (l.421) scoped to gwz-owned writes; for a libgit2 or Git-child write the leftovers are LR-REC-1 enumerated states, recognized and reported with a remedy (LR-CLEAN-6). E20 (l.224) and the event assignment split the tier: O for gwz-owned writes, C for a libgit2 or Git leftover. LR-TEST-6 (l.578) no longer has "where the CI host provides one" in a MUST's verification: seeded state gates; a fault-injecting filesystem (O or E) only supplements; the S audit adds "every MUST clause that binds a libgit2 or Git write maps to such a row" | Grep: "where a host provides one" appears only as a supplement; the matrix LR-REC-6 row (l.652) is F U K, W adds O | pass |
| T2-7 | P3 | LR-CLEAN-2's example has the wrong oracle (30 s vs the 60 s B-CLOSEW); four scope kinds map to no bound | FIXED. LR-CLEAN-2 (l.399): "Inner scopes (call, binding, exchange, child, temporary) end within their enclosing owner's single bound; a settling exchange also has its own B-SETTLE"; Verify uses a 90 s worker (retained at 60 s by a session close, at 5 s by a host owner's close; a 30 s worker is not retained) | The U row now fails a session that retains a 30 s worker and a close that does not retain a 90 s one | pass |
| T2-8 | P3 | LR-CAN-12's "U/P regression row at B-CANCEL" has no deterministic oracle | FIXED. LR-CAN-12 (l.325): E gates the figure under Plan:289ff's rounds-and-pairing rules; the U row asserts ordering with zero stepped-clock advance. LR-CLEAN-11 (l.408): E regression at B-RSS, U on counts (threads, tasks, records, stale events), never RSS. Matrix l.613: E U | Check 13 (matrix has U); no MUST cell asserts elapsed real time | pass |
| T2-9 | P3 | Code I says "the same row", which seam-dependent rows cannot run on the shipped binary | FIXED. §0 code I (l.75) redefined as LR-TEST-14's smoke subset, started at a readiness line, never at a step point or handshake; LR-TEST-14 (l.586) says a row needing a step point is never an I cell; matrix notes for LR-CAN-11/LR-EXIT-3 and LR-EXIT-2 (l.612 and l.616) say what the I cell is | Check 14: every LR-EXIT MUST keeps its I cell on all platforms (LR-EXIT-5 on W) | pass |
| T2-10 | P3 | W and M durability O rows cannot fail, yet LR-REC-11 makes W's status depend on one | FIXED, variant. LR-REC-3 (l.418): the O row cites the vendor documentation of each primitive and "cannot by itself show a gap"; the falsifiable rows are U (barrier used) and E (a real power-cut run on a named host, for example a VM with the write cache off and a hard reset). LR-REC-11 (l.426) and OQ-7 (l.900) depend on the U row plus the cited documentation. **Not applied as proposed:** "cite primary documentation as PF [V]". [V] means verified against a local man page or source (§0 PF), and no such local source exists for NTFS; the fact stays (ext) and the O row must cite the vendor documentation | LR-REC-3's F U O E cells unchanged; LR-REC-11 names no row that cannot fail | pass |
| T2-11 | P3 | LR-REC-12's S check sees only functions that carry its own marker (T-17 partial) | FIXED. LR-REC-12 (l.427): the S check covers every deletion or rewrite call site (`remove_file`, `remove_dir_all`, rename-over, truncating open) on a workspace or temp path; each must sit in a marked, listed function or on an allow-list entry that says why it is not recovery. Matrix S added | Read; check 13 | pass |
| T2-12 | P3 | LR-OWN-8's admission thread has no surface; OQ-4's claim is stale | FIXED. LR-OBS-5 (l.499): detached-and-accounted threads (the SSH reaper, a stuck admission thread, NG-26) are visible through a fixed-size count that `server status` also shows, marked **PROP** and listed in OQ-16. OQ-4 (l.897) reworded (death-detection exceptions NG-21, -25, -27, -28; a keepalive would detect a hung peer, which is not a death) | Every LR-OWN-8 entry maps to a user surface or a test-visible count; OQ-4 matches NG-21/25/27/28 | pass |
| C-13 | P3 | partial: PROP list incomplete | Closed by N-6 | Check 8 | pass |
| T-13 | P2 | partial | Closed by T2-3 (environment half) | as T2-3 | pass (text) |
| T-14 | P2 | partial | Closed by T2-5 and N-1 | as T2-5 | pass |
| T-17 | P3 | partial | Closed by T2-11 | as T2-11 | pass |

### Reviewer notes that are not findings

| Item | Disposition |
|---|---|
| Completeness observation: §11's Recommendation column states no provenance | Done: a three-line note under the §11 heading (l.888) says the column is the drafter's, names the five recommendations re-read against sources (OQ-5, -13, -14, -30, -31) and does not claim more |
| Completeness risk: citation drift of root P23:154 / SCC:230 (commit `8aa5f800`, absent from this lane) | Open for the lane owner: re-check both cites when the lane merges past `8aa5f800` (§7) |
| Completeness risk: recommendation-driven approval; every recommendation needs a source re-check | Partly done: recommendations with line cites re-read (above). The others cite no line and give a reason from this document; they are not claimed re-read |
| Completeness risk: OQ-32 versus OQ-1 coupling | Closed by N-1 (OQ-1 and OQ-32 now agree) |
| Completeness risk: unstable evidence base (C and D are r1 NO-GO) | No change. The clauses resting on them are PROP and in OQ-16 |
| Testability risk: a feasibility spike should cover all of §8, not only `ssh` | Done: LR-TEST-10 (l.582) requires a spike for every cell whose method reads platform state, and a named gap reported to the operator for any infeasible cell |
| Testability risk: hosted Windows runners (session 0) limit console-event and ENOSPC routes | Covered by the same named-gap clause; with T2-6 the W ENOSPC route no longer gates |
| Testability risk: OQ load (34 OQs, 39 PROP) | Acknowledged: 35 OQs and 42 PROP rows now; no change is possible without the operator |

## 3 The churn cases: why each fix is true on every platform and deterministic

Round-2 T2-1, T2-2 and T2-3 came from round-1 corrections applied literally. The rule applied here: restate the requirement
as behaviour, then give the verification a method that cannot pass a leak or fail a correct build.

| Case | Round-1 correction that failed | What failed | Requirement now | Verification now |
|---|---|---|---|---|
| T2-1 | session/process-group/job as the marker | Misses the cover, stdio core and background server (own session) and the W S child (no job) | "no process gwz created is alive, however placed or re-parented" (platform-neutral) | A tag that survives fork and exec by the allow-list; read per platform; census self-test with four seeded leaks, so a blind census fails its own test |
| T2-2 | "the job's process list empties" | The test's job handle keeps `KILL_ON_JOB_CLOSE` from acting; "any job" membership is true under a harness job | LR-PROC-2 unchanged in intent; membership is "in gwz's own job" | Wait on pinned process handles; job queries go through gwz's own handle in-process; the test holds none |
| T2-3 | exec-time environment read for all roles | A fork-only cover holds the exec-time block verbatim, so a correct cover fails and a harness scrub makes the check vacuous | Behaviour: the cover execs nothing, passes nothing, holds no credential gwz obtained; inherited strings are NG-9 | Exec-time read only where gwz constructed the environment (exec'd roles); live-environment query for the cover and a self-scrubbing server; OQ-35 for the operator |

The other T2 items were definition or method defects of the same kind (T2-4 a dangling exception, T2-5 two oracles for one
state, T2-6 a MUST with a conditional test); each was fixed by choosing one rule and making every cell cite it, never by adding a
parallel mechanism clause.

## 4 IDs and open questions added or changed

- **Added:** DEP-8, DEP-9, DEP-10; OQ-35 (inherited exec-time environment strings). No LR, NG or E ID added or removed.
- **PROP rows added to OQ-16:** LR-EXIT-6 (it was PROP already; round 1 missed it), LR-REC-5 (stop-time clause), LR-OBS-5 (thread
  count). LR-OWN-9 was already listed; its PROP part is now the case clause and the detached-worker clause.
- **Changed questions:** OQ-1 (wording), OQ-4 (reason), OQ-7 (falsifiable rows), OQ-13 (A took (b); option labels), OQ-14 (options and
  recommendation changed to (a)), OQ-24 (bears on DEP-3), OQ-32 (consequence for LR-REC-5 and LR-REC-7), OQ-27 (wording only), OQ-16
  (regenerated).
- **Departure from a reviewer's proposed correction:** T2-10 ([V] not claimable); N-4 (O-clean redefined instead of E37 moved);
  OQ-14's recommendation reversed (a correction, not a proposal of the reviewer, who asked only that its stated reason be fixed).

## 5 Disputed findings and citation corrections

Disputed: none. Two citation corrections; neither changes a finding.

- N-2 (b) cites SRV:807 for the W background copy leaving the caller's job. In this lane's `history/parked/GwzCoreServerDesign.md` the
  row is at :808 (B and the lane's C review also say "parked :807/:808" in their own tree versions). Used: :808.
- N-2 (c) cites SRV:1113 for "the child gets the client's environment". It is :1115. Used: :1115.

## 6 Mechanical checks and results

Run with Python scripts over `GwzLifetimeRequirements.md` (928 lines, blob `af4c6079`); they live in the session scratchpad,
which is wiped on restart, so each check is restated here. Numbers 1-13 extend round 1's checks; 14-21 are new.

| # | Check (review line it implements) | Result |
|---|---|---|
| 1 | Every `LR-`, `NG-`, `OQ-`, `DEP-` and `E` ID referenced is defined. Defined: 148 LR, 28 NG, 35 OQ, 42 E, 10 DEP | pass: none undefined |
| 2 | Every NG appears in §10 | pass |
| 3 | Every NG is referenced outside §6 | pass |
| 4 | Every OQ is referenced outside §11 (OQ-35 is in LR-CRED-3, NG-9) | pass |
| 5 | Every non-TEST requirement appears in a §8 row, ranges expanded | pass |
| 6 | No §8 ID is undefined | pass |
| 7 | No MUST Verify cell cites B-COVER or "within B-x after" | pass |
| 8 | PROP scan (requirement rows and the bounds table) equals OQ-16's list (N-6, C-13) | pass: 42 of 42, 0 extra, bounds row named |
| 9 | "M-B9" and "Clocks keep running" absent | pass |
| 10 | Every `B-` bound used is in the bounds table | pass |
| 11 | Every code used in §8 is defined in §0 (I included) | pass |
| 12 | Table integrity: every row has its header's cell count | pass: 0 problems |
| 13 | Verify codes are a subset of the §8 codes, E and Z excepted (N-7) | pass: 0 misses (17 after the text edits and before the matrix edits) |
| 14 | Sweep of SRV :803-810, :1068-1118, :526-584 and A :90-128, :231-257 against LR-EXIT, LR-PROC, LR-CRED, NG (N-2) | 4 departures declared; 5 examined and not departures (N-2 row). Not swept: SRV §6 after :584, SRV §16, A §3.2-§3.5 beyond the Force sequence (A:204-213) |
| 15 | Quotes and cites added in this revision re-read at the cited lines, line wraps normalised: P23:154 (2 quotes), SCC:230, SCC:208-209, SRV:808, :809, :1082, :1107, :1115, :1116, A:96-97, :235-236, :384, B:162-165, B:355, C:130, IL:299-300, `detached.rs:26-27` | pass: 19 of 19 |
| 16 | Every NG named in LR-LIVE-1's exception clause is named in a link-table cell (T2-4) | pass: NG-21, NG-25, NG-27 |
| 17 | `intent record` and `from the intent` absent; "recorded" hits listed (N-1, T2-5) | pass: 0 and 16 non-normative hits (N-1 row) |
| 18 | The start lock `id-<identity key>.lock` is named in R-SRV, LR-OWN-8, -9, -10, LR-CLEAN-8, LR-CONC-5, LR-TEST-7 (N-3) | pass: 7 of 7 |
| 19 | §9.3's NG range equals the defined maximum (N-5) | pass: NG-1 to NG-28 |
| 20 | Size against the 1,200 target | pass: 928 lines |
| 21 | Status line carries "DRAFT, revision 4, 2026-11-04 (round-2 remediation)" | pass |

Manual checks, argued in the table above: N-4's joint satisfiability, T2-2's handle semantics, T2-3's per-role environment
reasoning, "every clause of LR-OWN-9/-10 is in the decision or PROP". Not run: the census self-test (T2-1) and every P, K, F, U
row: the document authorizes no code. Platform facts used by the new text are (ext) and each has an LR-TEST-10 O row: `/proc/<pid>/environ`
and `KERN_PROCARGS2` read the exec-time block, `PR_SET_CHILD_SUBREAPER`, W kill-on-close acting at the last handle's close,
`IsProcessInJob`. None was verified locally in this round.

## 7 For the lane owner

- Next review: pin the new blob and read the document, not this plan. Round 2 was the last full round; a third round may correct
  non-architectural defects only. Items most likely to draw a third-round finding: (a) the exec-time environment reads in
  LR-TEST-7 and LR-CRED-3 (ext, unverified here), (b) OQ-14's reversed recommendation (the operator may prefer (b); the text
  works either way), (c) the choice to redefine O-clean instead of moving E37.
- The document cites root P23:154 and root SCC:230 (commit `8aa5f800`); this lane has neither. Re-check both cites when the lane
  merges past that commit. This lane's SCC has 400 lines; the root's has 401, the added line being :230.
- The document stays uncommitted in the lane; this plan and the two re-verdicts are untracked beside it.
- Out-of-scope observations (not acted on): the detached-worker record is keyed by a display string today (`detached.rs:26-27`), which
  differs from the member lock's `canonicalize` key and from the operator's `workspace_identity`; whether the record should move to the
  identity is a design question the PROP clause in LR-OWN-9 now asks the operator through OQ-16, not a finding to implement here.
