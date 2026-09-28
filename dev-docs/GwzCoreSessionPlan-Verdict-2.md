# GWZ core session plan, revision 3 (TR1.4b) — review verdict

Date: 2026-09-28. Status: **revision 3 is accepted for TR1.4b's scope at SHA-256 `01adc2f0b1647c987f903c4704ced759c6f12599582048e7fb945e51012e0255` after [Consistency-2](GwzCoreSessionPlan-ReviewConsistency-2.md) and [Safety-2](GwzCoreSessionPlan-ReviewSafety-2.md) reported GO; this accepts the plan text only**. With revision 2's acceptance for TR1.4a's scope ([Verdict-1](GwzCoreSessionPlan-Verdict-1.md)), the whole plan is now accepted: this is the session plan's G1 for the rest. The acceptance authorizes no implementation, commit, activation or release.

The object was revision 3 of `GwzCoreSessionPlan.md`, which applies step TR1.4b of the transport release plan:
- it revises every step revision 2 left to TR1.4b;
- it adds the extension steps and two addenda;
- it adds Phase 7 (connection reuse, CS7.1 to CS7.27) and Phase 8 (the server, CS8.1 to CS8.32).

It is 1527 lines and uncommitted. The drafter's disclosures listed 28 choices, six conflicts in controlling documents, and the items not placed.

Before the review, the lane owner corrected one of the six conflicts as an erratum in the accepted server design: §12's Linux walk row now says that without `/proc` the client refuses. The review ran against frozen copies of the fourteen controlling documents, the corrected server design among them.
- Two fresh reviewers, Consistency and Safety, ran in parallel.
- Neither read the other's report. The Consistency report was held outside `dev-docs` until Safety had finished.
- Each verified the object, the five HEADs and the frozen controls at the start and the end.
- Each read the object in full, and checked every code fact revision 3 adds against the tree.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | GO | 0 | 0 | 0 | 5 |
| Safety | GO | 0 | 0 | 0 | 6 |

Neither reviewer classified a finding as architectural. Both cleared every P3 to land as a text correction after the GO, without a further round.

## Blind convergence

Reviewing blind, the two axes landed on the same text three times:
1. **The server release gate's `env` debt.** Revision 3 moved the lazy endpoint's `transport_binding.rs` entry from CS3.2 to CS7.24. The gate sentences, the Phase 6 exit's O9 claim and CS6.5's test did not follow (C-P3-1, S-P3-3).
2. **Reuse §15 rows with no owning step.** Neither item 6's cancellation sub-row ("cancelling A while B holds a lease") nor item 4's "a different explicit identity never shares a connection" has an owner (C-P3-2, S-P3-5; Consistency named item 4 among its §3 risks).
3. **The §4 shared-file list.** It claims the sketch orders every shared file, but omits pairs (S-P3-1; Consistency §3 names three more files).

## Post-GO corrections

Applied to revision 3 in one patch by its drafter. Each reviewer then confirms its own items on the corrected text, as TR1.3's reviewers did.

| ID | Correction |
| --- | --- |
| C-P3-1, S-P3-3 | The Phase 6 preamble and CS6.5 say that O9 holds except for the lazy endpoint's read in `transport_binding.rs`. That is the one remaining gwz-core `debt` entry, reachable only on a predicate miss, and CS7.24 retires it. CS6.5's check asserts exactly that set. §5.4's paragraph and the Phase 8 preamble say the server's release gate passes "once CS6.5 and CS7.24 have cleared gwz-core's `env` and `process` debt, and CS4.8 gwz-py's". |
| C-P3-2, S-P3-5 | CS7.9's test-first gains reuse §15.6's cancellation row: A's token is cancelled while B holds a lease on the same instance; B completes, A's lease closes, the idle connections survive, and B's next request reuses one. CS7.11's instance half gains §15.4's row: two sessions with equal configuration and different explicit identities share no connection. §5.9's rows for items 4 and 6 name those steps. |
| C-P3-3 | CS8.5, CS8.8 and CS8.13 depend on CS1.10, and CS8.14 on CS1.3, CS1.10, CS8.8 and CS8.9. §4's sketch gains the edges. |
| C-P3-4 | CS8.6's parenthetical now cites the server design's erratum of 2026-09-28, which corrected §12's row to match §3 and §14. |
| C-P3-5 | CS4.5's fork rule names the mechanism. The child retains the inherited default host context and sessions, unreachable from the bridge, and never closes or drops them: their Rust owners are leaked (`mem::forget`), since dropping would join a thread the child lacks and shut down a socket the parent shares. |
| S-P3-1 | §4 gives every unordered pair of steps that name one file an order edge, or names an L1-06 handoff. It also states that after CS7.1 the split owners replace the generic "owners of" names, and that the sketch is redrawn when CS7.1 merges, recorded in the program checkpoint. The shared-file list gains `src/gwz/errors.py` (CS4.4, CS8.27), gwz-core's `scripts/run_tests.py` (CS1.7, CS8.4) and `docs/RustApi.md` (CS1.4, CS1.9). Safety's suggested edges are:<br>• CS7.10 → CS7.20 → CS7.13 → CS7.22 → CS7.19, over the owners of `ssh_worker.rs`;<br>• CS7.12 → CS7.13, CS7.10 → CS7.14, CS3.4 → CS8.4;<br>• CS4.2 → CS5.2 → CS4.8, CS5.3 → CS8.30, CS7.25 → CS8.24, CS7.26 → CS8.3. |
| S-P3-2 | **Choice: the Windows trust arms and the socket host's file and stale-socket rules are reviewed dual.** CS8.17, CS8.18 and CS8.19 are dual, as re-freezes of CS8.8, CS8.14 and CS8.5; together they may form one Windows interface checkpoint (AgentProcessRules §6.1). CS8.9 is dual, alone or with CS8.10 as one socket-host checkpoint. §3.0's dual list and R2's counts follow. |
| S-P3-4 | The Phase 7 exit names the rows whose owners follow it, as it already does for CS6.7:<br>• items 1 and 3 through gwz-py and through a server;<br>• item 11's server-shutdown half;<br>• item 13's pooled-connection half.<br>These run at CS4.9 and CS8.32, and those steps' reviews carry them. The exit requires every other row. |
| S-P3-6 | Beside C10, a contract ambiguity is recorded, with CS3.4's working rule. The credential spawn's environment drops `GIT_DIR`, `GIT_COMMON_DIR` and `GIT_WORK_TREE` as it drops the prompt hooks, so `git credential fill` never reads a repository's local configuration. §5.8 today names only prompt hooks, so the contract's next revision states it. CS3.4 gains the test: with the snapshot's `GIT_DIR` naming a repository whose local configuration names a recording helper, the recorder never runs. |

## Residual notes taken with the patch

- **From Consistency §3:**
  - CS1.10's merge-order referent is made explicit;
  - CS7.2 states the constraint that keeps the candidate build compiling at every step: T1 gives `Request` an additive or defaulted API until CS7.10 plumbs the limits.
- **From Safety §3:**
  - CS8.13's closure test for its `permanent` spawn entries runs over a test program, so the entries land proven at CS8.13;
  - the registry option that lets tests attach several bindings to one instance is test-only (`cfg(test)` or CS2.1's hook switch), with a workflow-text test (L2-12);
  - the server's Unix arms name what a Unix that is neither Linux nor macOS gets: `server_unavailable`, before any connect or bind;
  - after the Phase 8 exit, CS8.4's no-debt mode stays on in `run_tests.py`;
  - CS3.7 states that an abandoned binding is disposed at the next call or at the host context's shutdown or drop, with a test that the next call disposes it.

## Plan text carried from the step verdicts

The implementation reviews of CS1.4 with CS1.5, and of CS1.7, deferred their plan-text impacts to this patch, because the plan was this review's object:
1. **CS1.4 and CS1.5** ([their Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)):
   - the step file lists: CS1.4 gains `gate/tests.rs` and `context/tests.rs`, and CS1.5 gains `environment/tests.rs` and gwz-core's `Cargo.toml`;
   - §5.4's list of what stays `permanent` gains `thread_local CROSSING` (gwz-core's allowlist: 31 entries, 18 debt and 13 permanent);
   - §3.0's ratchet sentence says what it means: state that carries session-relevant content is `debt`, naming the step that removes it; state that carries none may be `permanent`, with a reason, under review;
   - CS1.4's measured budget: 505 production lines against the aspirational < 450;
   - that verdict's table of obligations, added one clause each to the steps it names: CS1.2, CS1.6, CS2.2, CS2.4, CS2.5, CS2.9 to CS2.12, CS3.5, CS3.7, CS3.9, CS1.9's `shutdown`, the socket host, and the Phase 1 exit.
2. **CS1.7** ([its Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md)):
   - §3.0 records the check's scope: items and unbraced statements. It also records the counted exclusion of fields, variants, match arms, parameters, call arguments, struct-expression fields and tail expressions, which no explicit boundary can hold.
   - CS1.7's exit records that sibling coverage is local-only until gwz-cli's and gwz-py's own CI run the check. That is a follow-up once CS1.7 is on gwz-core's `main`.

## The drafter's conflicts in controlling documents

| # | Conflict | Disposition |
| --- | --- | --- |
| 1 | The release plan's Phase 7 exit names only gwz-core's checker; the server design's gate also covers gwz-py's allowlist. | The plan's gate names both, which satisfies both documents (Consistency). Recorded. The release plan's line changes only with the operator's sign-off. |
| 2 | Server §12's `/proc` row contradicted §3 and §14. | Corrected as an erratum before the review. The plan's parenthetical follows (C-P3-4). |
| 3 | The release plan's §6 sketch lags its Phase 7 text, and lacks revision 3's edges. | The plan follows the text. The program checkpoint records the new edges (On GO). |
| 4 | Contract §10 gives no exit bound for several open Clients. | C9's one process-wide finalizer satisfies §10 and reuse §7. Recorded for the contract's next revision. |
| 5 | The server design's working-directory amendment does not cover core's `git credential fill`. | C10's `/` satisfies the amended §5.6. The `GIT_DIR` gap is recorded with it (S-P3-6). |
| 6 | No document names the owner of the Windows agent-pipe rules. | CS8.19, on top of S4.3, consistent with both documents. Its review is now dual (S-P3-2). |

## On GO, after the corrections are confirmed

- The plan's status line takes the §7 form for TR1.4b's scope, with this verdict and the reviewers' confirmations.
- TR1.7's status edit to gwz-py's `GwzPyTransportDesign.md`: superseded by the contract's §9, §10 and §14, as TR1.2 and TR1.4b leave them. Its NO-GO closing condition becomes the release plan's §4.
- The program checkpoint records:
  - the acceptance;
  - each step's review tier, with S-P3-2's changes;
  - the new edges for the release plan's §6 sketch, CS1.10 and CS5.4 before the server's lane among them;
  - S-P3-1's rule that the sketch is redrawn when CS7.1 merges.

## Final state after the post-GO corrections

1. **The drafter's patch** applied every row above, the residual notes and the carried plan text: `232f702d…`, 1562 lines.
   - Both reviewers confirmed every item: [Consistency-2a](GwzCoreSessionPlan-ReviewConsistency-2a.md) and [Safety-2a](GwzCoreSessionPlan-ReviewSafety-2a.md), filed verbatim.
   - Both judged the drafter's departures sound:
     - item 1 of reuse §15 has no gwz-py row;
     - the frame limit stands in for an entry count;
     - the checkpoint shapes;
     - the extra edges and handoffs;
     - CS1.7's file list and counts;
     - where the local-only note sits.
   - The tiers are now 28 dual, 74 single-axis and 3 Surface.
2. **Each confirmation raised one new P3, and a few notes.** The lane owner applied them as text corrections, with the status sentence:
   - **Safety-2a P3-7.** §3.0's new route to `permanent` is only for state whose type holds no payload (a marker, flag or counter, as `thread_local CROSSING` is). Where a test can prove the reason, the step adds it. Anything else is `debt`, with a named remover.
   - **Consistency-2a.** CS8.4's standing no-debt mode covers gwz-core's and gwz-transport's allowlists in gwz-core's `run_tests.py`, and CS8.30 adds the same mode for gwz-py's allowlist in gwz-py's test run. The Phase 8 exit switches both on.
   - **Safety-2a, departure 4.** CS8.22 and CS8.29 depend on CS8.3, so no server a CLI starts serves a native route before those rows run through a server. CS8.3 shares `session_entry.rs` with CS7.26 under a handoff instead of an edge, so the CLI server lane does not wait for reuse's activation.
   - **Safety-2a, departures 2 and 3.** CS8.10's snapshot test uses the worst case: the most minimum-size entries a 64 MiB frame can carry. The Windows checkpoint re-runs CS8.5's, CS8.8's and CS8.14's counterexamples on Windows.
   - **Consistency-2a residual.** CS1.4's file list matches its accepted object. CS1.2 registers its modules in `session_host/mod.rs`, after CS1.4.
3. **The final text is SHA-256 `5d1dc819dea4b31018e6b8d4e834340231d961eb7b8764a3c34dc9363cd394c3`, 1569 lines.** The lane owner re-checked that the step graph is acyclic.

   One Consistency-2a residual is left as it stands. The CS4.2 → CS5.2 → CS4.8 order, which Safety suggested, makes the Phase 4 exit wait on lane E. The reverse order would decouple them, since CS5.2's change to `native/src/lib.rs` is additive. Either order is sound. Revisit it if the Phase 4 exit is ever waiting on lane E.

## Round count

| Round | Consistency | Safety |
| --- | --- | --- |
| TR1.4a, round 1 (revision 1) | GO, 5 P3 | NO-GO, 2 P2 |
| TR1.4a, round 2 (revision 2) | GO | GO |
| TR1.4b, round 1 (revision 3) | GO, 5 P3 | GO, 6 P3 |
