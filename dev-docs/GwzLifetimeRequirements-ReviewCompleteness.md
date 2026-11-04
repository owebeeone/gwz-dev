**Verdict: NO-GO**

# Lifetime requirements r1 — Completeness-AXIS REVIEW

## 0 Evidence

- **Object pin.** `git hash-object /Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md` returned `34680949ed430a85ce1710064c0b2ef46c067ecc` at the start and the same hash at the end. The object did not change during the review.
- **Revision label.** The task calls the object "revision 1". The file's own status line (:5) says "DRAFT, revision 2, 2026-11-03 (completeness fix pass on revision 1)". This review covers the pinned blob above, whatever it is called.
- **Tree.** Root lane `gwz-dev-docs-req` at `e63509c2`, with the object untracked. The C/D lane `gwz-dev-docs-cd` is at `a977d241`.
- **Method.** I read the object in full (851 lines). I checked it against these sources:
  - Plan §1, §3 and §10, D-A to D-V (:937-1110).
  - CT §1-§11. SD §1-§10 and §16.
  - SCC §1, §3.1-§3.8 and §4.1-§4.4.
  - SRV (parked) :301-318, :530-616, :1078-1118 and :1210-1226.
  - A §0-§3.8, §7 and §9. B §1, §3.1-§3.8, §6 and §7.
  - C §3.1-§3.2, §4 and §7. D §4, §8 and §9.
  - AM §1, §6 and §7. TR1.6 §3.2-§3.3 and §12.
  - TR18 decision table. IL §5, §9 and §10. ACD §13. SBC §11.
  - CP :229-231, :266-270 and :326-329. P23 :145-180.
  - `GwzCancellationClauseLog.md`.
  - The process-globals allowlists in gwz-core and gwz-py.
- **Mechanical checks** (Python over the object):
  - 141 requirement IDs (134 MUST). Every non-TEST ID appears in §8. No reference to an undefined LR, OQ or NG ID.
  - NG-9 and NG-25 are absent from §10.
  - OQ-14, OQ-15, OQ-17 and OQ-18 are referenced only inside §11.
- **Quotes verified verbatim:**
  - OQ2 "Ctrl-C leaves no orphaned process, stale lock or partial state behind" (Plan:1040-1041).
  - "a pipe closing or another mechanism" (Plan:1039).
  - Resolution (1) "each such residue gets a named recognizer in the next command" (Plan:1069-1070).
  - "a documented constraint" (Plan:1073).
  - "The parity reference is now gwz 1.0.18" (Plan:1034-1035).
  - Goal 3 "SSH setup is paid once per connection, not once per repository" (Plan:47).
  - R5 "cleanup reports name what the client still owns" (Plan:170).
  - "A new blocker is reported with evidence ... never absorbed" (Plan:179-180).
  - AM "Ctrl-C must not leave anything behind" (AM:28-29).
  - TR1.6:82 "nothing kills that child on success".
  - IL "before any request byte" (IL:297).
  - ACD "Park, give up after 2" (ACD OQ12).
  - SBC 250 ms (SBC §11 Q7).
  - `GwzOpenDecisions.md` D8 "300 s wait default".
- **Code anchors confirmed:** `gwz-connection/src/pump.rs`, `connection_host.rs:330-340` (retain refusal), `carrier/stream/inbound.rs`, `session-host/src/service.rs:784-786` (`SendError::Full` closes the channel).

## 1 Findings

### P2

**C-1 (P2). LR-PROC-7 (:354) and OQ-27 (:848) misstate accepted design B. One source key, "M-B9", does not exist.**
- **Problem.** LR-PROC-7 requires "every K, W and S child is inside a job from its first instruction". It then says "B's registry jobs end every member at close (M-B9), which conflicts for S and W". OQ-27 builds on that: "gwz's death kills a stdio core (against LR-EXIT-9, E09 and C:123)".
- **Evidence:**
  - B:128-129: "A `Kill` or `Wait` child is an entry … given a kill-on-close job (Windows). An `OwnInput` child is tracked for its handle only".
  - B row B21 (B:326): an `OwnInput` child is "in none of the registry's jobs … at the harness's SIGKILL or `TerminateProcess` it exits 0 at end of file".
  - B:271 (to C): "`OwnInput` registers nothing and gets no job". C:121 gives the stdio core `Session, OwnInput`.
  - "M-B9" appears nowhere in either lane: grep over both `dev-docs` trees matches only the object.
  - For W children, B OD-B4 (B:343) says "Windows stays (a) under every choice". Yet OQ-2 and OQ-27 present the W-platform outcome as open.
- **Impact.** The operator is asked to decide a conflict that accepted B does not have. Options (a) and (c) of OQ-27 would add a job to the S class, or accept a tear, on a false premise.
- **Correction:**
  - Drop "S" from LR-PROC-7's job clause, or mark it PROP against B:129 and B21.
  - Delete "M-B9" and cite B:128-129 and B:343.
  - Rewrite OQ-27 to cover the W child only, recording that B already fixed W to (a) under OD-B4.
  - In OQ-2, note that only the Unix case is open.
- **Check.** No "M-B9" in the object. LR-PROC-7, OQ-2 and OQ-27 agree with B:129, B21 and B:343.

**C-2 (P2). LR-BND-2 (:418) contradicts the accepted contract and TR1.6, and omits authorized clock transitions.**
- **Problem.** "A deadline is minted once … Clocks keep running through a credential prompt or agent confirmation (Pageant)." The only pause it allows is CP's connect-clock pause.
- **Evidence:**
  - CT:683: "Interaction allowance | 120 s, cumulative; distinct from network time".
  - CT:707-708: "Preserve only explicitly authorized charging transitions: setup interaction pauses under SetupClock, and D19's helper-Basic pause".
  - TR1.6:79: "The network clocks do not run during it" (the helper interaction allowance).
  - TR18 D5 (b) keeps clocks running for the Pageant confirmation only. It also says "nothing in the accepted SSH clock amendment changes".
  - TR18 D42 and D45 authorize a "producer-busy" `Backpressure` charge with a 600 s ceiling. This is missing from LR-BND-2 and from the bounds table (:427-455).
  - CP:9 (SSH pump stall fix): "consumer backpressure and post-pack local work no longer run the stall clock".
- **Impact.** A design that follows LR-BND-2 would remove the accepted interaction pause. A helper sign-in window would then be charged to the 9 s network stall.
- **Correction:**
  - Restate LR-BND-2: deadlines are never reset or extended. Charging transitions are only those authorized: the SetupClock interaction pause, D19's helper-Basic pause, CP's local-wait pause, and D42/D45's producer-busy charge (600 s). The Pageant confirmation runs in Network (D5 (b)).
  - Add B-PRODUCER 600 s (TR18 D45) to the bounds table.
  - Cite CT:704-713 for what it actually says.
- **Check.** LR-BND-2 names each CT:707-708 and TR18 D42/D45 transition. No clause says clocks run through a helper prompt.

**C-3 (P2). There is no register of contradictions with accepted documents, and several requirements depart from accepted text silently.** The axis requires contradictions to be declared, and the object has no such section. Grep for "contradict", "conflict" and "supersed" finds only NG-18's internal conflict. Undeclared departures:
- **(a) First interrupt in the served form.**
  - LR-EXIT-2 (:305) and E02 (:172) bound Cancel at B-CLEAN (5 s) for every CLI command, `--server` included.
  - SCC:242 (accepted) says: "first Ctrl-C sends `operation.cancel` and waits up to the close bound; the second disconnects (server design §7)".
  - A:138 replaces this only provisionally ("the served core's response and bound are C's (until then the local rule)"), and C is NO-GO.
- **(b) gwz-py exit and close bounds.**
  - LR-EMB-3 (:490) closes owners "within B-CLEAN".
  - SD:511-513: the bridge finalizer joins the pump "waiting at most the close bound" (60 s).
  - SCC:285: `SocketCoreBridge.close()` runs until "the close report, end of stream or the close bound".
- **(c) Stdio child termination.**
  - LR-EXIT-10 (:314) "never kills a stdio core" cites SRV:1116, which says the client "waits up to 10 seconds for the child to exit, then kills it".
  - LR-EXIT-9 (:312) "finishes its own local writes to a safe point" cites SRV:1080-1084, which says "It does not wait for detached workers: they end with the process".
- **(d) Server address key.** LR-OWN-9 (:276) binds "server address" to one filesystem-identity key. SCC §1 (accepted) fixes `ws-<key>` as "SHA-256 of the core's canonical absolute workspace root".
- **(e) Force during server shutdown.** LR-EXIT-8 (:311) "A Force at **any** step ends every pending wait". SRV:590 and SCC:239 let "a second signal or `stop --force`" end only the step-3 detached-worker wait.
- **(f) and (g).** See C-5 and C-4.
- **Correction.** Add a "Departures from accepted documents" table: for each, the clause, the requirement, and whether the requirement supersedes it (and on whose authority) or yields to it. Route each departure that has no operator source to §11.
- **Check.** Every item (a)-(g) appears in the table, with an authority or an OQ.

**C-4 (P2). LR-OWN-2's closed allow-list (:269) is contradicted by accepted SD §5.7 and by the recorded debt. Its S check is infeasible as specified.**
- **Problem.** The allow-list has three entries: the interrupt module, the W spawn gate, and the PYM composition object. The only recorded debt it names is gwz-transport `NEXT_POOL`. The check "fails on any entry outside the allow-list".
- **Evidence:**
  - SD §5.7 (accepted) keeps `permanent` entries: "ID counters … Kept … `permanent` allowlist entries", and libgit2-imposed state.
  - `gwz-core/scripts/checks/process_globals_allowlist.json` holds 5 `permanent` entries (`TIMEOUT_STATE`, two libgit2 timeout options, the `runtime_env.rs` capture, the `process_tree.rs` spawn) and 12 `debt` entries. The debt includes statics `NEXT_WORKER` and `SERIAL` and thread-local `V1_PRESERVATION_IMAGE_CAPTURES`.
  - The gwz-py allowlist holds resource-holding statics `diff_logs.rs REGISTRY`, `log_outputs.rs REGISTRY` and `operations.rs STORE`. That is the completed-result store LR-OWN-8 and LR-EMB-7 discuss.
  - Operator decisions on the `Hub` statics (ACD OQ9; CP:269) are not reflected.
- **Impact.** The specified S check would fail on SD-accepted `permanent` entries. "Recorded debt" understates what MEM `surface-structural-debt` requires to be surfaced.
- **Correction:**
  - Either admit SD §5.7's `permanent` class into LR-OWN-2 (with SD as the source) or declare the departure (C-3).
  - List every current `debt` entry by allowlist (or cite the three allowlists as the inventory) under OQ-22.
  - Reflect ACD OQ9 and CP:269.
- **Check.** Run the specified check, mentally, against the three allowlists. Every entry is either allowed by a cited decision or listed in OQ-22.

**C-5 (P2). The hung admission thread of the accepted session design is missing from the failure model and contradicts LR-BND-1 and LR-OWN-8.**
- **Evidence.**
  - SD:213 and SD:684: "A hung workspace resolution delays later admissions in its session". Resolution is not a gate crossing, so cancellation does not reach it.
  - SD:428 (close step 2): "It does not wait beyond the close bound for an admission thread stuck in resolution".
  - The thread therefore outlives the session.
- **Gaps:**
  - LR-BND-1 (:417) admits only bounded waits, cancellable waits, U1-U3, and the §6 waits. This wait is none of them.
  - LR-OWN-8 (:275) is a MUST closed list of what "may outlive the scope that created it", and does not include this thread.
  - No E-event covers a hung resolution, for example a filesystem stall during workspace resolution.
- **Correction.** Do one of the following, citing SD:213, :428 and :684:
  - add an E-event, plus a U4 or NG row and an LR-OWN-8 entry; or
  - declare that LR-BND-1 supersedes SD here, and record it under C-3.
- **Check.** SD's admission thread appears in §3, in LR-BND-1 or §6, and in LR-OWN-8.

**C-6 (P2). LR-LIVE-1's "no polling" (:335) has no operator source and contradicts an operator decision that is not reflected or superseded.**
- **Sources cited:**
  - OQ2 says "a pipe closing or another mechanism". It does not forbid polling.
  - AM C3 :46-47 is the retired draft's requirement text, not an operator decision.
  - CT:335-340 is about `Progress` delivering streams independently of application calls. It says nothing about death detection.
- **The operator decision.** IL decision 4 (operator 2026-10-07, IL:299-300): "The HTTPS 2 ms step and the SSH bounded park stay; a separate HTTPS fixed-cost change replaces the 2 ms polls". IL §5.2 (:139) confirms that HTTPS idle-loss notice "is bounded by that tick, not by readiness" (link L7).
- **The gap.** §9.1 lists other polls to collapse, but not these. OQ-17 does not ask about them. §10.1 maps IL only to LR-LIVE-7 and NG-21.
- **Correction:**
  - Mark the no-polling clause PROP.
  - Map IL decision 4 in §10.1: either name it as an interim exception on L7 (an NG row) or record that LR-LIVE-1 supersedes it and put that to the operator (extend OQ-17).
  - Replace the CT:335-340 citation.
- **Check.** §10.1 contains IL decision 4. LR-LIVE-1's sources support each of its clauses.

**C-7 (P2). A normative "durable intent" mandate is untraceable and is §9's design.**
- **Problem.**
  - LR-OWN-3 (:270): "… a durable intent before a multi-file effect".
  - §1 phase L3 (:131): "a durable intent for every multi-file or Git-locking sequence (LR-REC-3)".
  - None of LR-OWN-3's sources (SD O7, A:95-102, B:103-104, resolution (2)) says this, and the clause is not PROP.
  - Resolution (1) requires a named recognizer, not an intent. LR-REC-1 allows recognition "from a durable record or from the state's own shape".
  - The clause is §9.1 P3's journal mechanism promoted into the normative tiers.
- **Impact.** It forces a durable, fsynced record before every Git-locking sequence (every ref update) with no operator authority. It also narrows LR-REC-1 silently.
- **Correction.** Remove the clause from LR-OWN-3 and L3, or mark it PROP with its own OQ, and align it with LR-REC-1's record-or-shape rule.
- **Check.** The intent clause either traces to a source or is marked PROP and appears in §11.

**C-8 (P2). Some normative clauses rest only on C/D r1 or the retired amendment and are not marked PROP.** §0 says C and D are "evidence, not authority", and that a requirement sourced only by evidence is PROP. Unmarked cases:
- LR-EXIT-10: "never kills a stdio core". Only C:121-130 supports it, and the cited SRV:1116 says the opposite.
- LR-EXIT-9: "finishes its own local writes to a safe point". This comes from C. SRV:1082 says the opposite. Only the own-signal clause is marked PROP.
- LR-PROC-2 (:349): the S clause "once its own writes reach a safe point" (C).
- LR-EXIT-8 (:311): "at any step". This derives from the CS S-P2-1 and CC P2-3 reviews. Decision (i) governs the CLI's Force, and A O1 only extends the module to the server.
- LR-LIVE-1: no polling (C-6).
- **Impact.** OQ-16 ("confirm each PROP") does not reach these clauses, so the operator is never asked.
- **Correction.** Mark each clause PROP, or cite an operator source.
- **Check.** No normative clause has only C, D, AM or review sources without a PROP mark.

**C-9 (P2). Reporting in the served form is infeasible as specified, and no limit names it.**
- **Problem.** §4 (:250) promises the O-forced report is "complete, marked forced". LR-OBS-1 (:461) requires each member's "local writes completed, or the safe point at which they stopped".
- **Evidence.**
  - In the served form, Force closes the carrier and exits while the core still runs local phases.
  - C:101-104 gives the only notice possible: "the server cancels what it was still running".
  - SD §13 and §8 step 8 carry `CleanupReport{pending_local_work, peer_cleanup_confirmed}`, and SD:438-439 says close "guarantees that no core thread is writing the workspace only when `pending_local_work` is 0". No requirement surfaces this to the user.
  - E18's surviving core is assigned O-clean (:253-255), although NG-23 says a served detached write can be torn before 3.0g.
- **Correction:**
  - Add a requirement that a served Cancel or Force report states the core's unconfirmed local work (`pending_local_work`, `peer_cleanup_confirmed`), or add an NG row that accepts an incomplete client report in the served form.
  - Qualify E18 (core side), E05 and E17b with NG-23 until 3.0g.
- **Check.** §4's "Report" row and LR-OBS-1 are achievable for E02 under `--server`, and E18's tier cites NG-23.

### P3

**C-10 (P3). SD-F8 is mis-mapped.**
- Source key :54 and §10.1 :739 map the "session `cancellation` field 8" to LR-CAN-1 and LR-CAN-8.
- SD §13: "`cancellation` (8, bool …) true when a running network operation can be cancelled (§5.8). Absent or null means cancellation is not supported". It is a capability bit for builds that cannot cancel.
- No requirement says a client learns whether its core can cancel promptly (an ordinary-build core, NG-15).
- **Correction.** Re-map SD-F8 and add or mark a requirement (or note) for advertising cancel support.
- **Check.** The §10.1 row matches SD §13.

**C-11 (P3). Some session-design lifecycle objects and some failure events are missing.**
- **Gwz-py bridge pump.** SD:488-499 ("One daemon thread per session"; "never exits silently": closes the channel, fails every outstanding call). It is not an actor or resource, and its death is not an E-event.
- **Eviction.** Operation-table eviction, `operation_expired`, and the bridge's bounded result view (SD §5.4, §10) appear only in the R-SESS prose.
- **Missing failure events:**
  - a child or gwz thread in uninterruptible sleep (a SIGKILL that does not complete; reaping waits without bound);
  - descriptor or process-limit exhaustion (EMFILE, RLIMIT_NPROC or EAGAIN at spawn, lifeline creation or thread creation) beyond E25's ENOMEM;
  - a workspace or member directory removed, renamed or unmounted during an operation (E34 covers only the server's own address).
- **Correction.** Add the actor and resource rows and the E-rows, each with a tier.
- **Check.** §1, §2 and §3 list them.

**C-12 (P3). Operator decisions that bear on lifetime are unmapped.**
- TR18 D24: the gwz-sspi inheritance window; it bears on LR-LIVE-4 and LR-PROC-7.
- TR18 D47: the Client's gate covers gwz-py's non-network `git commit` and `git tag`; it bears on R-GATE.
- TR18 D52: the spawn gate fails open. Private gates are minted by `Git2Backend::new()` and similar. There is a source check in 1.1.0 and a required gate in 1.2.0. This bears on R-GATE, LR-OWN-2 (b) and NG-16.
- TR18 D42/D45: see C-2.
- SBC Q5 (revision 3): late close failures are reported to nobody, with no counter. This bears on LR-OBS-9 and LR-OWN-4.
- **Correction.** Add §10.1 rows (or "not lifetime-relevant" with a reason).
- **Check.** Every TR18 decision about spawn, inheritance, clocks or close appears in §10.1.

**C-13 (P3). Cross-section claims are not met.**
- §6 (:500-501) says "Every row is also a §10 target". NG-9 and NG-25 appear in no §10 table.
- OQ-14 (W server breakaway, CS S-P3-4) is referenced by no requirement or NG, although it bears on NG-2.
- §0 (:25-26) says each PROP "is listed in §11". OQ-16 only says "per row", with no list (about 30 rows).
- **Correction.** Map NG-9 (H residuals) and NG-25. Reference OQ-14 from NG-2. Enumerate the PROP rows in OQ-16.
- **Check.** Mechanical: every NG and OQ is referenced outside §11, and OQ-16 lists the IDs.

**C-14 (P3). Closed lists are incomplete.**
- LR-OWN-8 and R-PROC "May outlive" omit the cover process itself, which outlives gwz after Tier C until it has ended its entries (B:219-221; R-B20).
- LR-PROC-1 (:348) says "An undeclared spawn is refused", but these spawns have no class: `gwz hook`'s utilities (`gwz-cli/src/hook/env.rs` df, stat, mount, cp, fsutil; `hook/fallback.rs` git) and `git worktree add` (R-B9).
- **Correction.** Add the entries and classes, or exempt them by OQ-13.
- **Check.** Every production spawn site maps to a class or a named exemption.

**C-15 (P3). Acceptance row R8 is omitted.**
- The source key (:33) cites "acceptance rows R1-R7 :164-181". R8 (Plan:173), the two-machine `ssh … gwz server stdio` run per direction, is the only acceptance evidence for link L5 and E17b.
- LR-TEST-11 requires only the two-processes-on-one-machine form.
- **Correction.** Trace R8, and add a two-machine row for L5, LR-CAN-9 and LR-EXIT-9 (remote).
- **Check.** R8 appears in §10.1.

**Pre-commitment.** I will return GO on a revision that resolves C-1 through C-9 as stated and introduces no new P0-P2 on this axis. C-10 through C-15 are recommended, not blocking.

## 2 Coverage map checked

| Source set | Result |
|---|---|
| Plan D-A to D-U | All present in §10.1 (D-C, D-F, D-G, D-J, D-K, D-Q, D-T and D-U are justified as not lifetime-relevant). |
| D-V sub-decisions: OQ2, OQ4 (d), OQ6 slots, parity 1.0.18, OD-C1 to C6, resolutions (1)-(3), split, (i)-(iii), O4 (a), O5, EXIT | All mapped. OQ1 (b) and OQ7 (1) are not lifetime-relevant; OQ1 (b) is not declared so (trivial). |
| OD-C7 (a)-(e), OD-C8 (a) and (b), OD-C9 | Mapped (open items routed to OQ-3, OQ-5 and OQ-19). |
| OD-B1 to OD-B4 | Mapped. OD-B4's W fixing is misstated (C-1). |
| OD-D1 to OD-D18 | All 18 mapped (§10.5). |
| A R1-R18, R-A1 (D's label) | All mapped (§10.2). A's §9 "beyond" and narrowing list matches. |
| B R-B1 to R-B20 | All mapped (§10.3). |
| C R-C1 to R-C13; D R-D1 to R-D3; AM R1 to R11, R1a, R1b, E1-E6 | All mapped (§10.4). |
| TR1.6 OQ1 to OQ7; TR18 D5, D14, D18, D25, D26, D31, D32, D44 | Mapped. D24, D42, D45, D47 and D52 are missing (C-2, C-12). |
| IL decisions 1-6 | 3 and 6 mapped; 4 missing (C-6). |
| ACD OQ3, OQ5, OQ6, OQ12/13 | Mapped. OQ9 (statics) missing (C-4). |
| SBC Q2, Q4, Q7, revision 3 | Mapped. Q5 missing (C-12). |
| CP :231, :268-270, :328 | Mapped. CP:269's `Hub` decision is missing (C-4). |
| CT lifecycle objects: HostOwner/Handle, CleanupDomain/Retention, Admission/Permit, Operation, Opening, Exchange, ticket, endpoint instance, idle entries, tombstones, AuthContext, fork | Covered by R-CONN, R-CRED, LR-OWN-1/4/7, LR-CLEAN-1/2/10 and E31. |
| SD lifecycle objects: call record, token, gate, worker, direct workers, table, event ring, logs and spools, held reads, session and host context, close report, bridge pump, admission thread | Covered except the admission thread (C-5), the pump and eviction (C-11), and the close report fields (C-9). |
| SCC: carrier, stall, address, server lifecycle, stdio/ssh forms, scrub | Covered. The first-interrupt bound and the address key depart from SCC (C-3). |
| §8 matrix completeness | Mechanically complete: every non-TEST ID has a row. |
| Normative-vs-design | Mostly requirements. Exceptions: C-7. LR-TEST-4 and -7 prescribe harness mechanisms (acceptable for testability). |

## 3 Risks

- **Line-number drift.** Citations are pinned to `e63509c2`, while the clause log and further lane merges keep moving plan lines (A §9 already cites D-V at :1055-1069, the object at :1086-1100). A traceability re-check is needed at each revision.
- **Unstable evidence base.** C and D are r1 NO-GO. Several requirements (C-8) and the residual mapping rest on their text, which may change.
- **Revision mismatch.** The object calls itself revision 2 while the task calls it r1. Later verdicts must cite the blob hash, not the revision number.
- **Bound conflicts.** If C-3 (a) and (b) stay undeclared, the served-form design (C) and gwz-py step 3.2 will meet two incompatible bounds (5 s and 60 s) with no recorded authority choosing between them.
- **Self-imposed scope.** C-4 and C-7 would bring in work (allowlist reduction, a per-sequence journal) whose provenance is self-imposed, not the operator's. The standing scope rule requires those to be surfaced, not assumed.
