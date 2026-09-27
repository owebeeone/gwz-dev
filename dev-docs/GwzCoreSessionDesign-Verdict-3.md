# GWZ core session contract — revision 3 verdict

Date: 2026-09-26. Status: **NO-GO at root `5a5d6cf`, gwz-core `e8509c32`, gwz-py `851c2e2` and gwz-transport `a7a36ae`: [Consistency](GwzCoreSessionDesign-ReviewConsistency-3.md) reported GO and [Safety](GwzCoreSessionDesign-ReviewSafety-3.md) NO-GO on one blocking finding. Revision 2's acceptance as a design contract ([Verdict-2](GwzCoreSessionDesign-Verdict-2.md)) stands; this verdict adds no acceptance to it**.

The reviewed committed tuple is contract revision 3:

| Repo | Commit |
| --- | --- |
| root | `5a5d6cffac57558681ab9f712ebf88bcffc3bd11` |
| gwz-core | `e8509c321cdf646d9ab0946f6db3dbfc2778ca74` |
| gwz-py | `851c2e2966b66ba45dbfc522e30c25400b72ffb2` |
| gwz-transport | `a7a36aec0ec6d31e38647b61567166d612f5d2c5` |

Revision 3 applied the ten nonblocking findings Verdict-2 carried forward, at the operator's direction, and moved gwz-transport's process-global check into gwz-core. This round is the third, which the cap permits for non-architectural corrections.

The round-2 reviewers could not be resumed, so two fresh reviewers ran this round with the round-2 reports and Verdict-2 as inputs.
- Both verified the exact tuple at the start and end, and read every file from the commits.
- Neither saw the other's current-round report.
- Neither ran the checker or any test. Consistency ran one `python3` one-liner to count allowlist entries.

| Axis | Verdict | Prior findings | New blocking | New nonblocking |
| --- | --- | --- | --- | --- |
| Consistency | GO | P3-23 to P3-26 closed; P3-27 superseded | none | P3-28 to P3-33 |
| Safety | NO-GO | P3-23 to P3-27 closed | P2-9 | P3-28 to P3-31 |

Every round-2 finding is closed. Consistency P3-27, the unpinned gwz-core checkout in gwz-transport's CI, is closed by removal of the arrangement it attacked. Both axes numbered their new findings from P3-28, so P3-28 to P3-31 each name two different findings; this verdict always cites them with the axis name.

**Classification.** Both reviewers classified every new finding as a bounded correction; none is an architectural root cause. The [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop is not triggered.

## Blocking root

### B11 — the gwz-transport check runs in no CI (Safety P2-9, blind convergence with Consistency P3-31)

Both reviewers independently found the same defect in the transport-check move. They rated it differently: Safety P2, Consistency P3. This verdict takes the P2, because revision 3 is weaker here than the revision it amends. Revision 2 ran the check in gwz-transport's CI on every push; revision 3 runs it nowhere automatically.

- gwz-transport's CI no longer runs the checker, by design.
- No gwz-core workflow checks out gwz-transport. The boundary job runs the checker over gwz-core alone.
- `run_tests.py` skips the gwz-transport check with a printed note when no sibling checkout exists, and the run stays green. Every CI job that runs it is a single-repository checkout, so it always takes that branch.
- O9, §5.7, §15.8, GWZRequirements and GWZDesign state the check as enforced ("MUST fail gwz-core's test run"). Only a developer's workspace run delivers it.

A new static in gwz-transport can therefore merge and publish unseen. Both reports give the same shape of correction: fail closed locally, add a pinned CI run on gwz-core's side, and state the enforcement in the text. Both keep the operator's dependency direction: the consumer checks its dependency.

## Nonblocking findings

### Convergence: the make-room log release (§5.4)

Four findings attack revision 3's new make-room release from different sides. One rewrite of that bullet settles all four.
- **Consistency P3-30.** §15.7's "With 64 logs open, the next open is refused" contradicts the make-room test beside it.
- **Consistency P3-32.** Only *sealed* logs can be released, though the producer's other terminal action, *close*, finishes a log too. 64 closed, unread logs still refuse the next open.
- **Consistency P3-33.** Only a make-room release gets `operation_expired` on a later read. The other release ways leave today's `invalid_request`, which the contract retired for the analogous cancel case.
- **Safety P3-28.** Releasing a freshly sealed log that has never been read can delete the spool of a log whose reader is about to open. The operation table evicts only *delivered* records.

### Single-axis findings

- **Consistency P3-28.** §15.1 still says a stopped-waiting call's reply is dropped, contradicting §10's rule that result and response replies never are.
- **Consistency P3-29.** The libgit2 own-reads exception is scoped to ordinary builds in O9, §5.6, §16 and GWZRequirements. §5.8 and GWZDesign also extend it to the remotes libgit2 handles natively in transport builds. §15.8's `HOME` test is unsatisfiable for such a remote.
- **Safety P3-29.** The stated remedy for the credential-helper debt, core running `git credential fill`, does not rule out prompting. Git prompts through `GIT_ASKPASS` or the terminal when helpers return nothing, and IDE terminals set `GIT_ASKPASS`. The text also states no kill-on-cancel or bound for that child, and no secrecy rule for the helper's output.
- **Safety P3-30.** The bridge drops a held result when *any* call on that operation reports `operation_expired`. A late cancel after the host evicted the record therefore discards the last copy.
- **Safety P3-31.** The checker flags only the `Cred::credential_helper` spelling. Switching to git2's public `CredentialHelper::new(..).execute()` keeps the spawn, turns the entry STALE and reads as debt paid.

### Residual risks below the finding bar

- The workspace registry's check-and-record must be one atomic step. A session's live records must survive its close as detached records.
- The registry does not record fetches. A fetch step in one session may run beside a W operation in another, while the same pair is excluded within one session. All statements agree on this scope.
- A new `fork` inherits the "created once" host context with a dead supervisor thread. That is the same exposure as today's process-global supervisor.
- "The host can tell an unseen `call_id` from a spent one" holds only for contiguous IDs.
- The Windows WTF-8 capture rule has no §15 test.
- Today's `DiffLog` parks each held read on a blocked thread. §5.1's parked-waiter rule needs a different completion mechanism, which the contract does not call out.
- Verdict-2's residuals are unchanged and none is made worse: the eight direct methods, the `(0, true)` cancel report, the worker-ended signal, zero or two cancel targets, the `CleanupReport` name, and the stuck admission thread's close-report line.

## Stop-rule accounting and next action

- The object has now used both remediation rounds and one further round confined to non-architectural corrections.
- Round 1 found B1–B9, round 2 found B10, and round 3 found B11.
- No reviewer has classified any finding in any round as architectural.

Next: [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) maps B11 and the ten new P3 findings to one patch, revision 4, with a closure test for each. Safety pre-committed to GO on a revision that resolves P2-9 as specified. Consistency asked for no further Consistency round, but a patch that also applies its P3 corrections would get a focused check from the same reviewer. Whether to apply the plan, and whether the P3 corrections go into revision 4 or into the implementation plan, is the operator's decision.
