# GWZ core session contract — remediation round 1 verdict

Date: 2026-09-24. Status: **NO-GO; one bounded blocking finding; remediation round 2 of 2 is next, not a lane stop.**

The reviewed committed tuple is contract revision 1:

| Repo | Commit |
| --- | --- |
| root | `cb5d6f219ae81df77125bd3e82b7103c766036ae` |
| gwz-core | `a8b27f276ea1a4020226ca596301ac253f7ce99d` |
| gwz-py | `0905f737562e942ae223a2cd8e6d82462a7b0d0d` |

No implementation, build, activation, push or tag is authorized by this verdict.

The same two reviewers re-verdicted with their round-1 context intact. Their reports are filed verbatim: [Consistency](GwzCoreSessionDesign-ReviewConsistency-1.md) and [Safety](GwzCoreSessionDesign-ReviewSafety-1.md). Both verified the exact tuple at the start and end, and neither saw the other's current-round report.

| Axis | Verdict | Round-1 closure | New blocking | New nonblocking |
| --- | --- | --- | --- | --- |
| Consistency | GO | P2-1 to P2-4 closed; P3-1 to P3-4 and P3-6 to P3-13 closed; P3-5 partial, its residual filed as P3-22 | none | P3-14 to P3-22 |
| Safety | NO-GO | all twelve closed | P2-8 | P3-6 to P3-13 |

**Classification.** Both reviewers classified every new finding as a bounded correction; none is a new architectural root cause. Safety pre-committed to GO on a revision that resolves P2-8 as specified.

**The ten applied choices.** Both reviewers ruled that every choice made while applying the first plan stays within its dispositions: the driver-captured environment, `log_verb`, the capabilities field, detached terminals, direct-call records, control counters, cancel targets, the empty close report, the host context and the gated transport entry. The plan's fresh-round clause is therefore not triggered.

## Merged blocking root cause

| ID | Root cause | Findings | Convergence |
| --- | --- | --- | --- |
| B10 | `remote_identity` is classified R and runs on the direct pool. Its `set` and `unset` ops write repository configuration, and all three ops take the workspace mutator lock, which is a try-lock. A `get` fails while a W operation holds the lock, and a running `set` can make a W operation fail with "already held". | Safety P2-8 | Single axis |

## Nonblocking convergences

- **Receipt versus admission** (Consistency P3-14, Safety P3-7). Records and tokens exist only after workspace resolution. A cancel can land in the resolution window, and control frames wait behind a slow resolution.
- **`call_id` monotonicity** (Consistency P3-18, Safety P3-13). Increasing IDs are a client obligation with no host rule, yet the cancel lookup relies on them.
- **Host context scope** (Consistency P3-21, Safety P3-6). Both push toward one host context per driver process by default, created by the driver and passed to each `open`, so the SSH agent design's process-wide budgets still hold by default, and so a later session can see a worker detached from an earlier one.

## Stop-rule accounting and next action

Round 1 found nine bounded roots (B1–B9), all now closed. This round found one more bounded root (B10) and seventeen nonblocking findings. No reviewer classified any finding as a new architectural root cause, so the [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop is not triggered.

The next round is remediation round 2 of 2, the last. Its plan is the [second remediation plan](GwzCoreSessionDesign-RemPlan-1.md). Apply it as one patch, commit a new exact tuple, and obtain focused re-verdicts from the same two reviewers, filed as `-ReviewConsistency-2.md` and `-ReviewSafety-2.md`. A third new architectural root cause in that round would stop the lane for an operator redesign-or-accept decision. The contract remains a draft.
