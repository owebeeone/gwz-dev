**Verdict: NO-GO**

# Lifetime requirements r3: Testability-axis review, re-verdict round 2

## 0 Evidence

- Object: `/Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md`, revision 3 ("DRAFT, revision 3,
  2026-11-03 (round-1 remediation)", :5). It is modified and uncommitted in the lane on top of `a874eddc`, which holds revision 2.
- Pin: `git hash-object` returned `ef534435f63eb52d6b0ba17aaf6b86632c267907` at the start and the same value at the end.
  The object did not change during the review. It matches the blob the RemPlan names.
- Read in full:
  - the object (:1-912);
  - my round-1 report (`GwzLifetimeRequirements-ReviewTestability.md`);
  - the RemPlan (`GwzLifetimeRequirements-RemPlan.md`).
- I compared against revision 2 (`git show a874eddc:...`) for E05, E06, E17, E28 and the event assignment.
- Peer-blind: I did not open `GwzLifetimeRequirements-ReviewCompleteness-2.md`. It does not exist in the lane at the time of writing.
- Sources re-read:
  - Plan:164-182 (R1-R9) and :276-292 (pass rules; R7 is :286, R8 :287, rounds :289). The RemPlan's citation corrections
    are confirmed.
  - B (`GwzChildLifetimeDesign-DRAFT.md`):138-148, :236-240, :355 (R-B3: "The reaper (a copy of gwz or Python in its own
    session)"), :383 (README: the reaper "is a fork of Python"). The cover is fork-without-exec.
  - Root `gwz-dev/dev-docs/GwzServerCore-Step2.3-Plan.md:154`, the operator decision the user's "go with (a), canonical
    path" refers to. LR-OWN-9, LR-OWN-10 and DEP-4 apply it faithfully: the canonical-path address, an identity-keyed member
    lock and start lock, and grants that fail closed on a move.
- Code re-read:
  - `gwz-core/src/transport_host/connection_host.rs:1-60` (the epoch moves on a drift above the idle interval; drifts
    accumulate);
  - `gwz-transport/crates/gwz-connection/src/ports.rs:42-50` ("A clock that cannot see either returns a constant").
- Platform facts used, marked (ext): they are not verified against a local primary source in this session, and each
  needs an LR-TEST-10 O row.
  - Windows `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE` acts when the **last** handle to the job closes.
  - Windows `IsProcessInJob(h, NULL)` is true for membership in any job.
  - Child processes inherit their parent's job unless they break away.
  - Linux `/proc/<pid>/environ` reads the exec-time block `env_start..env_end`, not the live `environ` array.
  - macOS `KERN_PROCARGS2` reads the same exec-time area from the process's stack.
  - A process forked without exec carries both unchanged.
- Nothing was modified. No git mutation and no build was run. The only file written is this report.

## Prior-finding closure table

"Counterexample re-run" means I re-applied the round-1 check to revision 3.

| ID | Sev | Status | Evidence (revision 3 lines) |
|---|---|---|---|
| T-1 | P2 | CLOSED | LR-LIVE-1 (:359) is limited to L1-L5, L7, L8, L10. LR-LIVE-9 (:367) gives L6 an owned join or channel and L9 atexit. L2/W reads "no instance" (:347). Every remaining cell can be killed by a P test. The NG-27 exception cell is dangling, which is new and filed as T2-4 |
| T-2 | P2 | CLOSED, with a new W defect | LR-PROC-2 (:374) is event-ordered. LR-PROC-16 (:388) is a SHOULD with an E row. B-COVER is measured (:485). LR-BND-3 (:446) is fixed. No MUST cell reads "within B-x after". The W half of the new verify cell ("the job's process list empties") defeats itself: T2-2 |
| T-3 | P2 | CLOSED for recognition | LR-REC-1 (:412) uses seeded states. LR-TEST-6 (:574) makes file-event kills Z. The failed-write half of libgit2 and Git writes (LR-REC-6) is now conditional: T2-6 |
| T-4 | P2 | CLOSED | LR-PROC-5 (:377) excepts dispositions a class declares (Unix `ssh`: SIGINT and SIGQUIT ignored), and its verify reads dispositions per class. It does not contradict LR-PROC-9 |
| T-5 | P2 | CLOSED | E06 (:195) gives the `ssh` carrier tier O pending OQ-24. E08 (:197) adds `ssh` SIGTSTP. OQ-24 (:902) bears on E06 and E08. LR-PROC-9 (:381) has the pty hang-up row over a real `ssh`, and LR-TEST-10 has the OpenSSH O row |
| T-6 | P2 | CLOSED | E17 (:211), LR-LIVE-2 (:360), NG-28 (:563) and OQ-33. The F cell asserts NG-28's outcome. A black-holing proxy can realize it on all three platforms |
| T-7 | P2 | CLOSED, with a P3 residue | LR-CLEAN-2 (:395) has one bound per owner kind, and §4 (:261) applies B-STOP to the server forms with U-waits excluded. The verify example's oracle is numerically wrong, and four scope kinds are unmapped: T2-7 |
| T-8 | P2 | CLOSED | LR-OWN-11 (:301) matches R1's pass rule (Plan:280): creations ≤ min(CAP, demand), peak ≤ CAP, later phases create none |
| T-9 | P2 | CLOSED, with a P3 residue | LR-REC-3 (:414) states W write-through. LR-REC-11 (:422) drops W. A U fake filesystem records the barrier per platform, and the U row on W/NTFS emits no warning. The O row "shows that write-through ... hold" has no method that can fail: T2-10 |
| T-10 | P2 | CLOSED | LR-BND-7 (:450) restates CT:670-672. B-SUSPEND (:477) agrees with `connection_host.rs:11-13` and drifts accumulate. Matrix cells are F U O, the U rows are B-SUSPEND ±1 ms, and the O row asks whether the clock counts suspend |
| T-11 | P2 | CLOSED | R7 P cell (:296, :595); §10.1 R7-R9 (:792-794); LR-CAN-1 (:310) includes `ssh`; LR-TEST-13 (:581) |
| T-12 | P2 | CLOSED, with a P3 residue | Code I (:74); LR-TEST-14 (:582); LR-TEST-4 (:572) restated. Every LR-EXIT matrix row has I. §0 defines I as "the same row", which is infeasible for seam-dependent rows: T2-9 |
| T-13 | P2 | PARTIAL | The memory-scan half closed: L and W scan, M self-scans, and U checks ordering. The environment half now names `KERN_PROCARGS2` and `/proc/<pid>/environ`. Both read the exec-time block, which a fork-only cover inherits verbatim. The check false-fails or is vacuous, and LR-CRED-3 contradicts NG-9: T2-3 |
| T-14 | P2 | PARTIAL | LR-REC-3 (:414) has one before-effect trigger set, and OQ-32 holds the wider options. A second, stop-time "recorded" obligation for every multi-file sequence remains in E02 (:191), E20 (:220), §4 O-forced (:250, :269) and LR-REC-5 (:416). Under LR-REC-3's first sentence it re-widens the trigger set. Under ENOSPC it cannot be met: T2-5 |
| T-15 | P2 | CLOSED, with a P3 residue | LR-CAN-12 (:321) is a MUST with B-CANCEL. The matrix row is E (:609), but the verify cell also asks for "a U/P regression row at B-CANCEL" with no defined oracle: T2-8 |
| T-16 | P3 | CLOSED | LR-LIVE-5 (:363) uses a wake counter; LR-TEST-5 (:573) admits F; LR-TEST-3 (:571) and code E cite Plan:289ff |
| T-17 | P3 | PARTIAL | LR-OWN-3: U recording port (closed). LR-CRED-2: secret-type S check (closed). LR-REC-12 (:423): the S check finds only functions that carry its marker, so an unmarked recovery sweep, the case that matters, is invisible: T2-11 |
| T-18 | P3 | CLOSED | LR-PROC-5 W clause and matrix W P (:630); LR-CONC-7 (:512) adds `GenerateConsoleCtrlEvent` to group 0 |
| T-19 | P3 | REOPENED as P2 | LR-TEST-7 (:575) now makes session or process group (Unix) and job (W) the primary marker. Those markers miss exactly the setsid'd and jobless gwz processes: the cover, the stdio core, the background server and, on W, the S child: T2-1 |
| T-20 | P3 | CLOSED | LR-LIVE-4 (:362) reads "the cover's own fork"; the M CLOEXEC cell is pending OQ-12 (:621) |
| T-21 | P3 | CLOSED | Matrix LR-OWN-9 U P O (:597); OQ-34 (:912) |
| T-22 | P3 | CLOSED | LR-OBS-7 (:497): one remedy per named shell, or a `gwz` subcommand |
| T-23 | P3 | CLOSED | §9.3 (:714) |

Summary: 17 CLOSED (4 of them with P3 residues), 3 PARTIAL (T-13, T-14, T-17), 1 reopened (T-19). Four new defects
(T2-2, T2-4, T2-6 and T2-12) came from the remediation delta or were exposed by it.

**Self-correction.** T2-1, T2-2 and T2-3 each follow a correction I proposed in round 1: T-19's session/job marker,
T-2's "the job's process list goes empty" and T-13's `KERN_PROCARGS2`/`/proc` environment read. The drafter applied them
faithfully. They are defects of my round-1 corrections, not of the remediation's fidelity. They still block, because the
text as written yields false passes or false fails.

## 1 Findings

### T2-1 (P2) The residue census cannot see the gwz processes most likely to leak (T-19 reopened)

- **Location:** LR-TEST-7 (:575): "no process carrying the test's marker (session or process group on Unix, job on W;
  an environment tag only if it is on the scrub allow-list in test builds ...)". Also LR-PROC-2's X cell (:374) and every
  X cell of §8.
- **Violated invariant:** X is the executable form of the operator's "no orphaned process" (OQ2). Its marker must reach
  every process gwz creates.
- **Reproduction:** each of these is a gwz-created process outside the marker as written.
  - The cover is "in its own session" (B:355, R-B3).
  - The stdio core is "in its own session" (E08, :197).
  - The background server (class D) detaches from the starter.
  - On W, an S child is "in no job" (LR-PROC-7, :379).

  A defect such as R-C3 (a stdio core outlives its client) or R-B20 (the cover outlives its entries) leaves a live
  process. A census keyed by session/process group (Unix) or job (W) does not list it, and the test passes.
- **Impact:** a false GO on the central lifetime requirement, for exactly the processes the residual lists (R-C3, R-B3,
  R-B20, NG-12) say can outlive gwz.
- **Required correction:** require a marker that survives `setsid`, re-parenting and job absence, and name the processes
  it must reach (the cover, the stdio core, the background server and every class).
  - **Linux:** the harness is a child subreaper (`PR_SET_CHILD_SUBREAPER`), so every descendant stays its descendant.
    A per-test cgroup is an alternative.
  - **macOS:** an environment or argv tag on the test-build scrub allow-list. The fork-only cover inherits it. Read it
    with `KERN_PROCARGS2`.
  - **W:** the same tag through the PEB, or a harness job that gwz is started in. If you use a harness job, see T2-2 for
    its interaction with LR-PROC-7.

  Drop "session or process group" as a sufficient marker.
- **Regression test:** a census self-test seeds one leaked process of each kind and asserts that each is reported:
  - a `setsid` child;
  - a forked copy that changes session;
  - a W child created outside every gwz job;
  - a background server.

### T2-2 (P2) LR-PROC-2's Windows verification keeps the job alive that it is waiting on

- **Location:** LR-PROC-2 verify (:374): "wait on each K child's pinned exit (...; on W, the job's process list
  empties), under a hang guard only". Interacts with LR-PROC-7's verify (:379, "K and W in a job, S in none") and
  LR-TEST-7's W marker.
- **Violated invariant:** LR-TEST-1 and LR-TEST-2 require a deterministic, failing-capable test that passes a correct
  implementation.
- **Reproduction (W, PF ext):**
  1. To read a job's process list the test needs a job handle (`QueryInformationJobObject`, `JobObjectBasicProcessIdList`).
  2. `KILL_ON_JOB_CLOSE` acts only when the last handle closes. The test's handle keeps gwz's job open after gwz is
     terminated.
  3. The K children therefore live on, the list never empties, and the hang guard fires on a correct implementation.

  Reading "the job" as a harness job that gwz starts inside does not fix it:
  - That job's list also contains F, W and S children, which are not under test.
  - Every child inherits the harness job, so LR-PROC-7's `IsProcessInJob(h, NULL)` check reports the S child in a job
    and false-fails "S in none".
- **Impact:** the W cell of a central MUST cannot pass as written, or a test author substitutes a looser check.
- **Required correction:**
  - W: open each K child's process handle (`SYNCHRONIZE`, which pins it) at a handshake before the kill, and wait on
    those handles. A process handle does not keep a job alive.
  - State LR-PROC-7's membership check as "in gwz's job" (`IsProcessInJob` with gwz's job handle, read in-process
    through a test seam), not "in any job".
  - If LR-TEST-7 uses a harness job, the job checks must exclude it.
- **Regression test:** W P row:
  1. Start a K child under gwz inside a harness job.
  2. Pin it by process handle.
  3. `TerminateProcess(gwz)`.
  4. Assert the K child's handle is signalled, and that the S child is not in gwz's job.

### T2-3 (P2) LR-CRED-3's environment check cannot judge a fork-only cover, and the clause contradicts NG-9 (T-13 partial)

- **Location:** LR-CRED-3 (:432): "The cover ... runs with an environment that carries no credential variable and no
  agent socket, whatever gwz inherited", verified by "inspect the cover's and the stdio core's environment (M
  `KERN_PROCARGS2`, L `/proc/<pid>/environ`, W the PEB)". NG-9 (:544): "secrets ... the inherited environment held
  before gwz composed, including their copy in a forked cover's memory".
- **Violated invariant:**
  - A MUST must be satisfiable.
  - Its check must separate a correct implementation from an incorrect one.
  - A MUST must not forbid what a §6 limit accepts.
- **Reproduction:**
  1. The cover is a fork without exec (B:355, :383).
  2. The test launches gwz with `SSH_AUTH_SOCK` set. R7 and every `ssh` row need it, and most developer and CI shells
     have it.
  3. The cover then holds the exec-time environment block verbatim. Clearing `environ` after the fork changes the
     pointer array, not that block. `/proc/<pid>/environ` and `KERN_PROCARGS2` read the block (PF ext), so the check
     reports `SSH_AUTH_SOCK` in the cover.
  4. Outcomes:
     - The check fails every correct fork-only cover that clears its environment.
     - It passes only if the harness scrubs gwz's own environment, and then it tests nothing.
     - The MUST can only be met by overwriting the inherited strings in place, which NG-9 says need not happen.

  The stdio core and the background server are exec'd, so the check is valid for them.
- **Impact:** a gate that is red by construction on M and L, or vacuous. Two rows give the inherited environment in the
  cover opposite verdicts.
- **Required correction:**
  - Restate the cover clause as behaviour:
    - the cover execs nothing;
    - it passes no environment on;
    - it holds no credential gwz obtained (the ordering already verified by U).
  - Move the inherited block explicitly under NG-9, or require an in-place wipe as a PROP clause with an OQ.
  - Keep the exec-time environment read for exec'd processes only (the stdio core, the background server, every child
    class).
- **Regression test:** P (M, L):
  - Launch gwz with a credential variable set. Assert the cover's process has no credential gwz obtained (canary absent
    from the self-scan) and starts nothing.
  - Assert the stdio core's exec-time environment lacks the variable.

### T2-4 (P2) LR-LIVE-1's exceptions name link cells that do not exist, so the gate fails the operator's interim L7 polls

- **Location:** LR-LIVE-1 (:359), "except the cells that name NG-4, NG-21, NG-25 or NG-27", against the link table
  (:344-355); NG-27 (:562); IL decision 4 (§10.1 :782).
- **Violated invariant:** a MUST must not forbid an operator decision (IL decision 4 keeps the HTTPS 2 ms step and the
  SSH bounded park as interim). Its verification must be able to pass on behaviour the operator accepted.
- **Reproduction:**
  - No link cell names NG-27 (grep: NG-27 appears only at :359, :562, :683, :782, :895). The L7 cell names only NG-21.
  - No cell names NG-4 either.
  - NG-27's own rationale says it is "not a waiver of LR-LIVE-1".

  The LR-LIVE-1 P row for L7 over HTTPS (§8 :619: "frozen clock"):
  1. Close the peer of an idle pooled HTTPS connection with the clock frozen.
  2. If the supervisor steps on the host clock, it never notices the loss, and the hang guard fires.
  3. If it steps on real time, it notices, but by polling, which LR-LIVE-1 forbids.

  Either way the gate is red until the separate HTTPS fixed-cost change lands.
- **Impact:** LR-LIVE-1 turns a change the operator scheduled separately into a prerequisite (standing scope rule), or
  the gate is waived informally.
- **Required correction:**
  - Name NG-27 in the L7 cell, for the HTTPS idle supervisor and the SSH idle watch.
  - Reword NG-27's rationale: it is the operator's exception, accepted by IL decision 4.
  - Drop NG-4 from LR-LIVE-1's list, since a hung peer is not a death. Alternatively, give it a cell.
  - Give L7's interim rows their own oracle: the loss is noticed within one step on a stepped clock.
- **Regression test:**
  - A script that maps every NG named in LR-LIVE-1's exception clause to at least one link-table cell.
  - A U row: an L7 HTTPS idle loss is noticed after exactly one supervisor step on a stepped clock.

### T2-5 (P2) A second, stop-time "recorded" obligation re-widens LR-REC-3's trigger set and cannot be met under ENOSPC (T-14 partial)

- **Location:**
  - E02 (:191): "a multi-file sequence may stop at a file-granular safe point. That state is recorded";
  - E20 (:220): "a stopped multi-file sequence is recorded";
  - §4 O-forced (:250, :269): "only as **recorded** partial state";
  - LR-REC-5 (:416): "is recorded and handled by the same recovery path";
  - against LR-REC-3 (:414): "Where gwz's recovery of a sequence uses a record ..., the record covering an effect
    reaches stable storage ... before the effect begins ... No other sequence is required to use the record route here".
- **Violated invariant:** one normative trigger definition (T-14's check), and every MUST feasible.
- **Reproduction:**
  1. Force stops a checkout (R1) at a file-granular safe point.
  2. LR-REC-5 requires a record, and the recovery path uses it.
  3. LR-REC-3 then requires that record to have been durable before the effect began. For a sequence outside the
     trigger set, that is a before-effect intent record for every Force-stoppable multi-file sequence: checkout, fetch
     (R18), clone. This is OQ-32 (b) and (c), which the trigger set excludes.
  4. If instead "recorded" means a record written at stop time, LR-REC-3's ordering forbids it, and nothing states that
     record's durability.

  E20 under ENOSPC is a direct infeasibility: the write fails because the disk is full, and E20 then requires writing a
  record to the same disk.

  The RemPlan's T-14 check found these occurrences (:191, :220) but did not treat them as a trigger definition.
- **Impact:**
  - Two oracles for one stopped sequence.
  - An unsatisfiable O-tier claim (E20).
  - A silent widening toward a per-sequence journal with a full-flush cost on R1/R2's path (OQ-32 (c)), without the
    operator's decision.
- **Required correction:** define the Force/error-path record once, in LR-REC-5, as one of:
  - (a) the report's safe-point label (LR-OBS-1), with the state recognized by shape (LR-REC-1);
  - (b) a stop-time marker that LR-REC-3's before-effect ordering does not govern, with its own stated durability and a
    shape fallback when it cannot be written (ENOSPC).

  E02, E20 and §4 should cite LR-REC-5 rather than restate it. If (b) is chosen, mark it PROP and add it to OQ-32.
- **Regression test:**
  - F: ENOSPC on the third file of a multi-file sequence, with the record write also refused. Assert:
    - no partial file is published;
    - the report names the safe point;
    - the next command recognizes the state by shape.
  - Grep: "recorded" for multi-file state occurs only through LR-REC-5.

### T2-6 (P2) LR-REC-6 binds libgit2 and Git writes, but on M and W that half has no gating test

- **Location:** LR-REC-6 (:417); LR-TEST-6 (:574): "for libgit2 and Git writes those faults run as O or E rows on a
  fault-injecting filesystem **where the CI host provides one**"; matrix LR-REC-6 F | F | F O (:644); E20 tier O (:220,
  :276); E39 (:242).
- **Violated invariant:** LR-TEST-1: every MUST has a failing-capable test on each platform §8 names.
- **Reproduction:**
  1. libgit2 (its indexer or ODB) or a Git child hits ENOSPC mid-write.
  2. LR-REC-6 and E20 (tier O, "its temp is removed ... Git locks are released") claim no residue.
  3. The F port does not reach libgit2 or a child process, so the F cells pass whatever libgit2 leaves.
  4. LR-TEST-6 makes the only route conditional on host capability, so on a host without one there is no row at all.

  Feasible routes exist, but the text does not name them:
  - L: a size-limited tmpfs in an unprivileged user and mount namespace;
  - M: a small disk image attached with `hdiutil` without root;
  - W: no unprivileged ENOSPC route is evident (a VHD mount needs admin).
- **Impact:** the O-tier claim for E20 and E39 on libgit2/Git writes is untested, and the gate passes regardless.
- **Required correction:** choose one of:
  - scope LR-REC-6 to gwz-owned writes, and make the leftovers of a failed libgit2 or Git write LR-REC-1 enumerated
    states, verified by seeded state and moving E20's libgit2/Git part to "recognized";
  - name the per-platform ENOSPC route as a gating P row, with W stated as an open question or a W-specific limit.

  Remove "where the CI host provides one" from a MUST's verification.
- **Regression test:** an LR-TEST-1 matrix-audit row: every MUST clause that binds libgit2/Git writes maps to a seeded-state
  row or a named per-platform fault route.

### T2-7 (P3) LR-CLEAN-2's verify example has the wrong oracle, and four scope kinds map to no bound

- **Location:** LR-CLEAN-2 verify (:395), "a worker that ignores cancel for 30 s is retained at B-CLOSEW for a session";
  §0 Terms "Scope" (:103).
- **Problem:**
  - B-CLOSEW is 60 s (:466). A 30 s worker ends before the bound and is never retained. A test written to the cell
    fails a correct session close.
  - The RemPlan's check ("every owner kind of §0 Terms maps to one bound") does not hold literally: operation or call,
    binding, exchange and temporary map to nothing. B-SETTLE covers only a settling exchange.
- **Correction:**
  - Use a worker that ignores cancel past the bound, for example 90 s: retained at 60 s (session) and at 5 s (host).
  - State that inner scopes run within their enclosing owner's single bound.
- **Regression test:** the U row with a 90 s worker.

### T2-8 (P3) LR-CAN-12's "U/P regression row at B-CANCEL" has no deterministic oracle and disagrees with the matrix

- **Location:** LR-CAN-12 verify (:321); matrix E only (:609); LR-BND-3 (:446).
- **Problem:**
  - A P row that asserts real elapsed interrupt-to-effect time ≤ B-CANCEL is load-dependent. Under LR-TEST-3 it cannot
    be retried, so it is a flaky gate by construction.
  - A U row on a stepped clock cannot measure a real-time figure.
- **Correction:**
  - Gate the figure by E only, under the rounds-and-pairing rules.
  - Define the U regression as "the cancel path reaches all network work and K children with zero stepped-clock
    advance".
  - Apply the same to LR-CLEAN-11's "U/E regression at B-RSS".

### T2-9 (P3) Code I says "the same row", which seam-dependent rows cannot run on the shipped binary

- **Location:** §0 code I (:74); LR-TEST-14 (:582); matrix I cells on LR-CAN-11/LR-EXIT-3 ("Force at every gate state",
  :608) and LR-EXIT-2 ("1, 2, 3 interrupts per phase", :612).
- **Problem:** the shipped executable has no step points or handshakes. "The same row" against it pins nothing (LR-TEST-2).
- **Correction:** define I as LR-TEST-14's smoke subset (dispositions at start, exit status, `user32`, console, panic
  strategy), triggered at a readiness line, not at a gate state.

### T2-10 (P3) The W and M durability O rows cannot fail, yet LR-REC-11 makes W's status depend on one

- **Location:** LR-REC-3 O (:414); LR-REC-11 (:422), "while LR-REC-3's O row shows that write-through create and rename
  hold"; OQ-7.
- **Problem:** durability across power loss is not observable without a real cut. With no method stated, the O row
  documents the fact and cannot show a gap.
- **Correction:**
  - Cite primary documentation as PF [V] for the O fact.
  - Make the falsifiable rows the U barrier row and the E power-cut row. Name the E host, for example a VM with the
    host write cache disabled and a hard reset.

### T2-11 (P3) LR-REC-12's S check sees only functions that carry its own marker (T-17 partial)

- **Location:** LR-REC-12 verify (:423).
- **Problem:** an unmarked sweep that deletes an earlier process's residue passes the check, which is exactly the case
  "a mechanism outside the list is refused" exists for.
- **Correction:** check deletion and rewrite call sites (`remove_file`, `remove_dir_all`, rename-over) in workspace and
  temp paths. Each must sit in a marked, listed function or on an allow-list.

### T2-12 (P3) New LR-OWN-8 entries have no surface; OQ-4's claim is stale

- **Location:** LR-OWN-8 (:298), "a status surface" for every entry; LR-OBS-5 (:495); OQ-4 (:882).
- **Problem:**
  - The admission thread stuck in resolution (new; NG-26) appears neither on LR-OBS-5's user surfaces nor among the
    LR-OBS-9 counters, so "U per entry" has nothing to observe.
  - OQ-4 says no-keepalive is "the one link where LR-LIVE-2 cannot hold". NG-21, NG-25 and NG-28 now also except it.
- **Correction:**
  - Add the admission thread to a counter, or to `server status`.
  - Reword OQ-4.

Pre-commitment: I will return GO on a revision that resolves T2-1 to T2-6 with no new P0-P2. The P3s do not block.

## 2 Invariant analysis

- **Sleeps and chance cuts.**
  - No MUST verify cell uses a sleep or "within B-x after" as its pass condition.
  - File-event kills are Z only (LR-TEST-6).
  - Fork-to-exec and PID-reuse windows are reached through injected ports (LR-PROC-3, -5; LR-LIVE-4), with Z as a
    supplement.
  - Remaining timing-shaped oracles: LR-CAN-12 and LR-CLEAN-11's regression rows (T2-8, P3), and E17b's real `ssh`
    keepalive. E17b is event-ordered (ssh exits) under a hang guard, which is acceptable.
- **Feasibility per platform.**
  - The M memory read is fixed (T-13).
  - Remaining infeasibilities:
    - W job-list wait (T2-2);
    - fork-only cover environment read on M and L (T2-3);
    - libgit2/Git ENOSPC on W (T2-6).
  - Real-`ssh` rows depend on LR-TEST-13's feasibility spike. That is correctly made a precondition.
- **Bounds.**
  - Every bound has a value and an owner, or sits in OQ-28 as a timer to fix or a measured figure.
  - B-SUSPEND matches the code.
  - B-COVER, B-CANCEL and B-RSS are correctly ownerless.
- **Failure events to tiers and requirements.**
  - E36-E40 are mapped and tiered.
  - E20's O tier is overclaimed for libgit2/Git writes (T2-6) and infeasible under ENOSPC for multi-file records (T2-5).
  - E05 and E06's `ssh` path stay pending OQ-24 and are disclosed.
  - E28's O tier is honest after LR-BND-7's restatement.
- **Stuck, orphan and partial states the text permits against operator decisions.**
  - Orphans: the text forbids them, but the census cannot see the setsid'd or jobless ones (T2-1). This is a
    verification gap, not a permission.
  - Partial state: the Force/error "recorded" obligation is ambiguous (T2-5).
  - Stuck states: E27b and NG-26 are disclosed with OQs.
  - The operator's 2.3f option (a) is applied without remainder (LR-OWN-9 and LR-OWN-10 agree with P23:154). Its
    verify cells are deterministic: fake identity, a second spelling refused naming the first address.
- **Architectural root cause.** None of the new findings is architectural. Each is a verification-method or
  definition defect in the text, correctable without changing a requirement's intent.
  - T2-1 and T2-2 share one cause: a process-identity mechanism chosen for the test without checking how the cover and
    the S class are placed. The single fix is a class-aware test marker.

## 3 Risks

- **Round-1 corrections that themselves had to be undone:** three of this round's P2s (T2-1, T2-2, T2-3) come from
  corrections I proposed in round 1. The cell-level mechanism names (job lists, `/proc` environ, session markers)
  should be checked by the LR-TEST-13-style feasibility spike before the matrix is frozen. That spike should cover all
  of §8, not only `ssh`.
- **W CI capability:** hosted Windows runners in session 0 limit console-event and ENOSPC routes. A W gap list should be
  explicit rather than left to "where the CI host provides one".
- **OQ load:** 34 open questions and 39 PROP rows gate normativity. Several MUST oracles (E05/E06 under OQ-24, the L1 W
  cell under OQ-2, LR-LIVE-4 M under OQ-12) cannot be finalized until the operator answers them.
- **Line-cite drift:** revision 3 cites a root P23 commit (`8aa5f800`) that this lane does not contain yet. A review in
  the lane alone cannot re-check it until the lane merges past that commit.
