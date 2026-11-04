**Verdict: NO-GO**

# Lifetime requirements r4: Testability-axis review, re-verdict round 3

## 0 Evidence

- **Object:** `/Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md`, revision 4. Its status line
  reads "DRAFT, revision 4, 2026-11-04 (round-2 remediation)". It is uncommitted in the lane on top of `a874eddc`.
- **Pin:** `git hash-object` returned `af4c6079428e25c9d8281aab2ac92a1f46661c29` at the start and the same value at the end.
  The object did not change during the review, and the hash matches RemPlan-2's blob.
- **Read in full:**
  - the object (:1-928);
  - my reports `-ReviewTestability.md` and `-ReviewTestability-2.md`;
  - `-RemPlan-2.md`, with `-RemPlan.md` as background.
- **Peer-blind:** I did not open `-ReviewCompleteness-3.md`.
- **Sources re-read:**
  - root `gwz-dev/dev-docs/GwzServerCore-Step2.3-Plan.md:154` (the operator's 2.3f decision (a));
  - root `gwz-dev/dev-docs/GwzSplitCommandChannel.md:230` (§4.2);
  - the lane's `GwzSplitCommandChannel.md` §3.8 (:204-210), in particular the **Children** bullet: "`ssh` keeps
    `SSH_AUTH_SOCK` for its own login" and "removes the list from every `sudo` and local stdio child".
- **Comparison:** `git show HEAD:` (revision 2) for LR-CRED-3, LR-REC-1, LR-REC-7 and LR-TEST-7.
- **Platform facts:** the ones used below are marked (ext). None was verified against a local primary source here.
- Nothing was modified except this report. No git mutation and no build was run.

## Prior-finding closure table (round 2)

Each row re-applies the round-2 check to revision 4.

| ID | Sev | Status | Evidence (revision 4 lines) |
|---|---|---|---|
| T2-1 | P2 | CLOSED, with P3 residues | LR-TEST-7 (:579) defines "no process that gwz created ... however it is placed ... or re-parented". It names the cover, the stdio core, the background server and the W S child, and says session, group and job are "never a sufficient marker". The marker is a tag the forked cover inherits. On L a subreaper backs it. A census self-test with four seeded leak kinds is a P row. The re-check passes: each of the four leak kinds carries the tag, so the census sees it. Residues: the census has no stated timing rule (T3-3), §0 X still lists "job" as a marker (T3-3), and a remote core under a localhost `sshd` carries no tag (T3-4) |
| T2-2 | P2 | CLOSED | LR-PROC-2 (:378): the test waits on the K child's own process handle (`SYNCHRONIZE`), "never the job's process list". LR-PROC-7 (:383): membership is checked "in gwz's own job" through an in-process seam, and the test holds no job handle. Re-check: a process handle does not keep a job alive (PF ext), and the S child is judged against gwz's job, so a CI runner's job or a harness job cannot satisfy the check |
| T2-3 | P2 | CLOSED for its reproduction; a **new P2 in the restated text** | The fork-only cover's exec-time block is now NG-9's (NG-9 :548, OQ-35), and the cover is judged by behaviour. The same restatement now says that every **child class** gets an environment with no agent socket. That contradicts SCC §3.8 for the `ssh` carrier, and the P check fails a correct `ssh` child: T3-1 |
| T2-4 | P2 | CLOSED | LR-LIVE-1 (:363) lists the exceptions NG-21, NG-25 and NG-27. The L7 cell (:356) names NG-27 and NG-21, and the L5 and L10 cells name NG-25. NG-4 is dropped. NG-27's rationale (:566) is the operator's exception. The Verify cell has a stepped-clock U oracle for L7 HTTPS, and matrix :623 has U and F |
| T2-5 | P2 | CLOSED for the stop-time record; the replacement clause opens T3-2 | LR-REC-5 (:420) reads "No record is written at stop time". E02 (:195), E20 (:224) and §4 (:254, :273) cite LR-REC-5. The ENOSPC F row exists. Its new last sentence ("reported at the stop only") contradicts LR-REC-1 and LR-REC-7: T3-2 |
| T2-6 | P2 | CLOSED | LR-REC-6 (:421) is scoped to gwz-owned writes, and libgit2 and Git leftovers are LR-REC-1 seeded states. LR-TEST-6 (:578) now says "no MUST depends on it". E20's tier is split (:224, :279-284). Matrix :652 |
| T2-7 | P3 | CLOSED | LR-CLEAN-2 (:399) uses a 90 s worker (retained at 60 s and at 5 s) and a 30 s worker (not retained), and inner scopes run under the enclosing owner's bound |
| T2-8 | P3 | CLOSED | LR-CAN-12 (:325) gates the figure by E only, and its U row checks ordering with zero stepped-clock advance. LR-CLEAN-11 (:408) uses counts, never RSS. Matrix :613 |
| T2-9 | P3 | CLOSED | Code I (:75) starts at a readiness line. LR-TEST-14 (:586) says a step-point row is never I. The matrix notes are at :612 and :616 |
| T2-10 | P3 | CLOSED | LR-REC-3 (:418): the O row cites vendor documentation and "cannot by itself show a gap", and the falsifiable rows are U and E (named host). LR-REC-11 (:426) and OQ-7 follow |
| T2-11 | P3 | CLOSED | LR-REC-12's S check (:427) covers every deletion or rewrite call site on workspace or temp paths |
| T2-12 | P3 | CLOSED | LR-OBS-5 (:499) adds a detached-thread count (PROP). OQ-4 (:897) is reworded |
| T-13 | P2 | via T2-3 | The environment half is closed for the cover. T3-1 is a new defect in the same row |
| T-14 | P2 | via T2-5 | There is one stop rule now. Its carve-out conflicts with LR-REC-1 and LR-REC-7: T3-2 |
| T-17 | P3 | CLOSED | via T2-11 |

Summary: all 12 round-2 findings and all 3 partials closed against their own reproductions. Two new P2s come from the
remediation delta: T3-1 from the T2-3 restatement and T3-2 from the T2-5/N-1 restatement. There are also four P3s.
Round 2's pre-commitment was GO on "T2-1 to T2-6 resolved with no new P0-P2". It is not met.

## 1 Findings

### T3-1 (P2) LR-CRED-3 forbids the agent socket to "every child class", which contradicts SCC §3.8 and LR-PROC-14, and its P check fails a correct `ssh` carrier

- **Location:** LR-CRED-3 (:436):
  - "A process gwz execs (the stdio core, the background server copy, **every child class**) gets a constructed
    environment with no credential variable and no agent socket".
  - Verify: "with a credential variable and `SSH_AUTH_SOCK` set in the starting environment, inspect the exec-time
    environment of each exec'd role (stdio core, background server copy, **every child class**; ...)".
- **Conflicts with:**
  - LR-PROC-14 (:390): "No credential environment reaches a child that does not need it".
  - Accepted SCC §3.8, Children bullet: "the client starts `ssh` with an allow-listed environment only: ...
    `SSH_AUTH_SOCK` ... `ssh` keeps `SSH_AUTH_SOCK` for its own login". It removes the scrub list only "from every `sudo`
    and local stdio child".
- **Violated invariants:**
  - A MUST must not misstate its source.
  - Two MUSTs must not contradict each other.
  - A verification must pass a correct implementation.
- **Reproduction:**
  1. Run the LR-CRED-3 P row as written, with `SSH_AUTH_SOCK` set, over the served `ssh://` form (LR-TEST-13, R8).
  2. The `ssh` K child is an exec'd child class. SCC requires that it hold `SSH_AUTH_SOCK`, and agent key authentication
     needs it.
  3. The `/proc/<pid>/environ` or `KERN_PROCARGS2` read of that child shows the variable, and the row fails a build that
     conforms to SCC and LR-PROC-14.

  The same applies to other children that need credentials by design:
  - credential helpers (`git credential fill` and the `credential-<x>` it runs; the `GCM_*` and `GIT_ASKPASS` names are on
    the scrub list);
  - a W child's signer (`ssh-keygen -Y sign` through the agent).

  A build that strips them, to pass the row, breaks agent login and helper lookups.
- **Origin:** revision 2 limited the clause to "a lifetime purpose (the cover process, the background server copy, the
  stdio core)". The T2-3 restatement widened its third sentence and its Verify cell to every child class.
- **Second, smaller defect in the same row:**
  - The requirement no longer says the cover clears its live environment. It says only that the cover "execs nothing and
    starts nothing, so it passes no environment on".
  - The Verify cell still asserts that the cover's live environment "lacks the variable".
  - A cover that leaves `environ` untouched meets the text and fails the test.

  Choose one:
  - require the cover to clear its live environment (cheap: it is the cover's own `environ` array, not the exec-time
    block);
  - drop the live-environment assertion for the cover.
- **Required correction:**
  - Restore the third sentence's scope to the lifetime-purpose roles: the stdio core and the background server copy.
  - State the children's rule once, by reference to LR-PROC-14 and SCC §3.8:
    - `ssh`: allow-list including `SSH_AUTH_SOCK`;
    - local stdio and `sudo` children: scrub list removed, and `SSH_AUTH_SOCK` removed for the local stdio child;
    - the other classes: only the variables their class needs.
  - Make the P row check each class against its own allow-list or scrub rule, not against "no agent socket".
  - Settle the cover's live environment one way.
- **Regression test:**
  - P (M, L, W): start gwz with `SSH_AUTH_SOCK` and `GH_TOKEN` set. Assert the `ssh` child holds `SSH_AUTH_SOCK` and not
    `GH_TOKEN`. Assert the stdio core and the background server copy hold neither.
  - S: the class-to-environment table in LR-PROC-14 has one row per LR-PROC-1 class.

### T3-2 (P2) LR-REC-5's "reported at the stop only" carve-out contradicts LR-REC-1 and LR-REC-7, and OQ-32's recommendation rests on it (T-14 lineage)

- **Location:**
  - LR-REC-5 (:420), last sentence: "A stopped state that LR-REC-1's enumeration cannot tell from the user's own edits is
    named in the enumeration and is then **reported at the stop only** (OQ-32) **PROP**".
  - Against:
    - LR-REC-1 (:416): "The next gwz command in the workspace recognizes **each** state, from a durable record or from the
      state's own shape";
    - LR-REC-7 (:422): "A working tree torn by Tier C, or stopped at a safe point by Force, cannot be staged or committed
      by gwz unseen. A later `stage`/`commit` of that member refuses, or warns with the state";
    - LR-REC-3's closed trigger set (:418);
    - OQ-32's recommendation (:925): "LR-REC-1 allows recognition by shape, which costs no barrier";
    - OQ-1's recommendation (:894): "(a) reported at the stop **and recognized by the next command** (LR-REC-5)
      suffices".
- **Violated invariants:**
  - One oracle per state.
  - No two MUSTs that no build can satisfy together.
  - No recommendation that relaxes an operator decision without saying so. Resolution (1) requires "a named recognizer in
    the next command" for every Tier C residue.
- **Reproduction:** take a Force mid-checkout (R1) at a file boundary.
  1. HEAD and the index are still old, and some working files are at the new content. LR-REC-7's own rationale says this
     "looks like the user's edits". Checkout is not in LR-REC-3's trigger set.
  2. LR-REC-5's carve-out applies: the state is reported at the stop only.
  3. A SIGKILL at the same file boundary leaves the same state (it is a step point of LR-REC-1's K row). LR-REC-1 then
     requires the next command to recognize it, and LR-REC-7's K row requires a later `commit` to refuse or warn.
  4. Without a record that is impossible, because by hypothesis shape cannot tell the state apart. So:
     - a build that follows the carve-out fails the K rows of LR-REC-1 and LR-REC-7;
     - a build that passes them must keep a record for checkout, so the carve-out never applies.

  The next command cannot know whether Force or SIGKILL produced the state, so the Force path cannot keep a weaker oracle
  than the Tier C path. OQ-32's recommendation (a) is presented as costing "no barrier". Under LR-REC-1 as written it
  either:
  - forces records (the cost of OQ-32 (b)) for every sequence with such a state; or
  - drops resolution (1) for that state, which the text does not say.

  OQ-1 (a) and OQ-32 (a) give opposite answers for the same state.
- **Impact:**
  - Two oracles for one state, and a MUST pair that no build can pass together.
  - The operator would approve OQ-32 (a) without seeing that it relaxes resolution (1) and voids LR-REC-7 for exactly
    the case LR-REC-7 exists for. OQ-8 recommends LR-REC-7 as a MUST.
- **Not architectural.** The root cause is that the record-versus-shape choice is a fixed list (the trigger set), decided
  apart from the recognizability LR-REC-1 demands. The fix is one rule plus a correctly framed question, inside the
  existing model.
- **Required correction:** choose one.
  - (i) Derive the trigger set. A sequence whose enumeration contains a state that shape cannot tell from the user's
    edits is in LR-REC-3's trigger set. Delete the carve-out. OQ-32 then asks only about the cost: list the sequences this
    rule pulls in (checkout at least) and give an E row for the barrier.
  - (ii) Keep the carve-out. Then:
    - LR-REC-1 and LR-REC-7 each name the exception explicitly, for the Tier C path too;
    - OQ-32 (a) is restated as a relaxation of resolution (1) for those states, which needs the operator's decision;
    - OQ-1's recommendation is reconciled with it;
    - LR-REC-7's MUST is marked conditional on that answer.

  Either way, delete the claim "costs no barrier" unless it holds for every enumerated state.
- **Regression test:**
  - Mechanical: no requirement text says "reported at the stop only" while LR-REC-1 or LR-REC-7 claims next-command
    recognition of the same state without an exception.
  - K: SIGKILL a checkout at a file boundary. Assert either recognition (i), or the named exception and its report (ii).

### T3-3 (P3) The census has no stated timing rule for processes that end by themselves after a K test, and §0 X still lists "job" as a marker

- **Location:** LR-TEST-7 (:579): "no process that gwz created during the test is still alive ... except one the test
  names as expected". Code X (:72): "no process carrying the test's marker **or job**".
- **Problem:**
  - Now that the census sees the cover and the S child (T2-1), a K test that SIGKILLs gwz meets several LR-OWN-8 entries
    that are alive at the end of the test step and end later without a further event:
    - the cover, "until it has ended its entries";
    - the stdio core, until its safe point and EOF;
    - an E37 child.
  - A snapshot census false-fails a correct build. Naming those processes as expected lets a cover that never exits (R-B20)
    pass.
  - LR-TEST-2 forbids a sleep, so a test author has no rule to follow.
  - X's definition contradicts LR-TEST-7's "never a sufficient marker".
- **Correction:**
  - The census waits, event-ordered, for every tagged process to exit, except the ones the test names as persistent (the
    background server, F programs). Under the hang guard, the recorded process tree is the failure report.
  - "Expected" is reserved for persistent processes.
  - Drop "or job" from code X.

### T3-4 (P3) The remote core under the localhost `sshd` of LR-TEST-13 carries no census tag

- **Location:** LR-TEST-7 (the tag reaches a process "by the test-build scrub allow-list"); LR-TEST-13 (:585); LR-EXIT-9
  ("The remote stdio core behaves the same").
- **Problem:**
  - `sshd` starts the remote `gwz server stdio` with its own environment.
  - SCC §3.8 has gwz pass `SetEnv=GWZ_NONE=1` and `SendEnv` only for allow-listed names, and `sshd`'s default `AcceptEnv`
    does not admit the tag (ext).
  - The census therefore cannot see a remote core that outlives its session (R-C3, remote form) on the localhost rows.
  - LR-EXIT-9's own P assertion still covers the case, so this is not a false GO on the MUST. It is a census blind spot.
- **Correction:** state the route. Choose one:
  - the test `sshd_config` accepts the tag, and the test build's `ssh` allow-list sends it;
  - a test-only remote command wrapper sets the tag.

  Add the remote core to the census self-test.

### T3-5 (P3) Small inconsistencies in the delta

- Matrix LR-REC-5 (:647), "F: ENOSPC with the record write also refused", against LR-REC-5 (:420), "No record is written
  at stop time" and "every further write refused". Use LR-REC-5's wording.
- LR-TEST-7 and LR-CRED-3 call the W PEB read the "exec-time environment block". The PEB's environment pointer tracks the
  live block after `SetEnvironmentVariable` (ext). On W the check is therefore of the live environment, and a role that is
  given a credential at creation and then scrubs itself passes on W but fails on M and L.
  - Name the W read as the live block.
  - Add it to LR-TEST-10's process-state O rows.

## 2 Invariant analysis

- **Sleeps and chance cuts.**
  - No MUST Verify cell uses a sleep, a timing figure or "within B-x after" as its pass condition.
  - Fork-to-exec, PID-reuse and file-event windows are reached through injected ports or seeded states. Z only
    supplements.
  - The census's timing (T3-3) is the one place a sleep could creep back in.
- **Feasibility per platform.**
  - The round-2 infeasibilities are gone:
    - the W job-list wait;
    - the fork-only cover's exec-time environment;
    - the conditional libgit2 ENOSPC route.
  - New infeasibilities:
    - LR-CRED-3's P row on every child class (T3-1);
    - the LR-REC-1/LR-REC-5/LR-REC-7 triple for shape-indistinguishable states (T3-2).
  - Cells that depend on host capability go through LR-TEST-10's spike and named-gap clause: the APFS case-sensitive image,
    W per-directory case sensitivity and real `sshd`.
- **Census sees the processes.**
  - The tag plus the L subreaper reach the cover, the stdio core, the background server and W S children, with a seeded
    self-test.
  - The tag also marks NG-1 descendants (gpg-agent, `credential-cache`). That over-approximation is safe, because it
    cannot yield a false GO and the tests name those processes.
  - Gaps: timing (T3-3) and the remote core (T3-4).
- **Bounds.**
  - Every bound has a value and an owner, or it is PROP (B-TURN, B-LINGER), or it sits in OQ-28 (B-SETTLE, B-FACTORY,
    B-EOF as timers; B-CANCEL, B-COVER, B-RSS as measured figures).
  - LR-BND-3 makes the unconfirmed ones non-normative, so LR-PROC-13's W B-EOF clause does not gate until OQ-28 is
    answered.
  - B-STOP agrees with LR-CLEAN-2.
- **Events to tiers.** E01-E40, E17b and E27b each appear in the event assignment (:279-285). E20 and E37 are tiered
  consistently with LR-REC-6 and LR-OWN-8.
- **Stuck, orphan and partial states against operator decisions.**
  - Orphans: LR-PROC-2, LR-PROC-15 and LR-OWN-8 now agree on the E37 child (one exception, one owner).
  - The 2.3f decision (a) is applied without remainder: LR-OWN-9, LR-OWN-10, LR-CLEAN-8, LR-CONC-5 and R-SRV match
    P23:154 and SCC:230, and the detached-worker clause is marked PROP.
  - Partial state: T3-2 is the one place where the text, through LR-REC-5's carve-out and OQ-32's recommendation, would
    leave a residue without the next-command recognizer that resolution (1) requires, and does not flag it as a
    relaxation.
  - Stuck states: E27b and NG-26 are disclosed with OQs.
- **Architectural root causes:** none. T3-1 is a scope slip in one row. T3-2 is a contradiction between a closed list and a
  derived obligation, fixed by one rule or one framed question.

## 3 Risks

- **Churn on the environment row.** LR-CRED-3 has now been rewritten in three rounds, and each rewrite moved the defect. The
  fix in T3-1 should be checked against SCC §3.8's Children bullet line by line, not against the round's finding alone.
- **OQ-32 load.** OQ-32, OQ-1, OQ-8 and LR-REC-7 form one decision. Presenting them separately invites inconsistent
  answers. §11 should cross-reference them as a group.
- **Unverified (ext) facts in gating cells:**
  - the `/proc` and `KERN_PROCARGS2` exec-time reads;
  - the PEB live block;
  - kill-on-close at the last handle;
  - `IsProcessInJob`;
  - OpenSSH's signal handling.

  Each has an LR-TEST-10 O row, but none has been spiked. The matrix should not freeze before the spike.
- **Citation drift:** the root P23:154 and SCC:230 cites (commit `8aa5f800`) were re-read at root and match. Re-check them
  when the lane merges.

Pre-commitment: I will return GO on a revision that resolves T3-1 and T3-2 with no new P0-P2. T3-3 to T3-5 do not block.
