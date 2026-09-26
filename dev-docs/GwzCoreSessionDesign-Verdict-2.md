# GWZ core session contract — remediation round 2 verdict

Date: 2026-09-25. Status: **accepted at root `58ea74b`, gwz-core `730e711`, gwz-py `685ecdc` and gwz-transport `67ed9b1` after the round-2 [Consistency](GwzCoreSessionDesign-ReviewConsistency-2.md) and [Safety](GwzCoreSessionDesign-ReviewSafety-2.md) re-reviews reported GO; this accepts the design contract only**.

The reviewed committed tuple is contract revision 2:

| Repo | Commit |
| --- | --- |
| root | `58ea74bd70b24d762409a8cf2852fedd94f9897d` |
| gwz-core | `730e7119baea6df6e6cfbe731586323d2a183836` |
| gwz-py | `685ecdc80e165e28f96e84cf68a9be7caed33c1d` |
| gwz-transport | `67ed9b16fa0d560a0912ee846c1aa2a9066cb318` |

No implementation, build, activation, push or tag is authorized by this verdict, and it does not cover platform proof or release.

The same two reviewers re-verdicted with their context intact, and their reports are filed verbatim.
- Both verified the exact tuple at the start and end.
- Neither saw the other's current-round report.
- The only programs either ran were the read-only process-global checker, in all three repositories, and its unit tests. All passed.

| Axis | Verdict | Prior findings | New blocking | New nonblocking |
| --- | --- | --- | --- | --- |
| Consistency | GO | P3-14 to P3-22 all closed | none | P3-23 to P3-27 |
| Safety | GO | P2-8 (B10) and P3-6 to P3-13 all closed | none | P3-23 to P3-27 |

Every finding from earlier rounds on both axes is now closed, so the ten new P3 findings are the only open ones. The two axes numbered their new findings independently, so each ID from P3-23 to P3-27 names two different findings. This verdict always cites them with the axis name.

**Classification.** Both reviewers classified every new finding as a bounded correction; none is an architectural root cause.

**The six applied choices.** Both reviewers ruled that every choice made while applying the second plan stays within its dispositions. The plan's fresh-round clause is therefore not triggered. The six choices were:
- the reply messages §13 names beside the requests;
- five spawn sites in the allowlist, four `debt` and `gh` `permanent`;
- `operation_id` assigned at receipt;
- a per-process default host context held by the Python bridge;
- gwz-transport's CI checkout of gwz-core;
- the O9 wording and the §5.6 heading.

## Nonblocking findings carried forward

Nothing blocks. Both reviewers direct the ten P3 findings into the implementation plan, each with the regression test its report names. No P3 becomes a package.

### Blind convergences

- **Result replies for cancelled waits** (Consistency P3-25, Safety P3-26). Both attack §10's rule that the bridge keeps such a reply until release.
  - Consistency finds that the rule contradicts the closed-loop drop rule beside it.
  - Safety finds that the kept view has no bound.
  - One rule settles both: keep result and response replies whatever the loop's state, bound them by the operation-table size, and drop an entry on release or on `operation_expired`.
- **gwz-transport's unpinned gwz-core checkout** (Consistency P3-27; Safety below the finding bar, in its ruling on choice 5 and its residual risks). The contracts job pins its taut-generator checkout by ref but follows gwz-core's default branch. A checker change in gwz-core can therefore turn gwz-transport's gate red with no gwz-transport change.
- **The reading thread's never-wait rule** (Consistency P3-24; Safety residual risks). §5.1 states the outcome but not the mechanism.
  - Consistency: a held read must be parked as a waiter, not serviced on the reading thread.
  - Safety: workspace resolution must run outside the table lock the reading thread uses to settle cancels, and direct calls still wait behind a hung admission thread.
  - Consistency's residual risks add that an admission thread stuck past the close bound gets no line in the close report.
- **The Taut `CleanupReport` name** (both axes' residual risks). It coincides with `gwz_core::transport_host::CleanupReport`, so one should be renamed if the crate re-exports both.

### Single-axis findings

- **Consistency P3-23.** §5.6 says the bridge captures `os.environ`, beside the lossless `os.environb` rule, and §9 leaves the snapshot's element type unstated. Correction: one capture rule, with byte-string pairs on every platform.
- **Consistency P3-26.** §4.2 still sends a target "the session never admitted" to `operation_not_found`, which contradicts revision 2's receipt-time records. Correction: "never received".
- **Safety P3-23.** W exclusivity is per session. W operations from two sessions sharing a host context therefore collide on the workspace try-lock, and §5.1's list of "already held" sources omits the case. Correction: serialize W per workspace across the sessions sharing a host context, as detached workers already are.
- **Safety P3-24.** In ordinary builds, libgit2 runs the configured git credential helper with the live process environment. It does so from inside the `git2` crate, where neither the snapshot nor the checker reaches. Correction:
  - state the exception in §5.6, §5.8 and §16;
  - flag `Cred::credential_helper` in the checker as a `permanent` `process` entry.
- **Safety P3-25.** A byte-format `diff` log that no reader ever opens stays open until the session ends. After 64 unread diffs, the next one is refused with `transport_session_full`. Correction: release the oldest sealed log with no reader before refusing.
- **Safety P3-27.** The bridge creates its default host context "on first use" with no creation discipline. Concurrent first opens can therefore create two and silently lose the cross-session guarantees. Correction: create it once, at bridge import or under a lock, before any session opens.

Two of these touch operator principle O9:
- Safety P3-24 would add a `permanent` O9 exception for the default wheel's HTTPS authentication.
- Safety P3-27 concerns the bridge's per-process default host context, which the contract places at the driver edge.

## Residual risks below the finding bar

- RemPlan-1 counted seven remaining direct methods, but Consistency counts eight: `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `diff`, `log`, `transport_capabilities` and `configure_transport_runtime`. §15.4's core test must cover all eight.
- `operation.cancel` on a direct call answers with a cleanup report whose values, `(0, true)`, are unstated.
- The signal by which the detached-worker registry learns that a worker has ended is unstated, though it is implementable.
- `OperationCancelRequest` requires exactly one target. The text should say that zero or two targets is `invalid_request`.
- A detached worker that never returns keeps its `flock` and its registration until process exit, as §16 discloses.

## Stop-rule accounting and next action

The object used both remediation rounds the cap allows and closed on the second.
- The first verdict found nine bounded blocking roots (B1–B9).
- The second verdict found one more (B10).
- All ten are closed at the contract level.
- No reviewer classified any finding in any round as an architectural root cause, so the [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop was never triggered.

Next:
1. Make a status-only edit under [AgentProcessRules §7.2](AgentProcessRules.md). Flip the following to this acceptance:
   - the contract's status line;
   - the DRAFT headings of the paired sections in gwz-core's GWZDesign and GWZRequirements;
   - the gwz-py pointers.

   Keep "no implementation or activation authority" in the new status. Safety asks that the contract stay a draft for implementation until the implementation phases reach their own gates. This edit is not part of this verdict.
2. Carry the ten P3 findings, each with its regression test, into the phased plan that item 2 of [the proposals §9](GwzClientCoreTransportProposals.md) calls for.
3. Record the acceptance in the program checkpoint.
