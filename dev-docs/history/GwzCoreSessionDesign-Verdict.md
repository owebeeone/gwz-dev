# GWZ core session contract — first design verdict

Date: 2026-09-24. Status: **NO-GO; bounded remediation under the review loop, not a lane stop.**

The reviewed committed tuple is:

| Repo | Commit |
| --- | --- |
| root | `e4b8d4332ccc87712a946a34c2d8355e22a8b633` |
| gwz-core | `31036fdde1cc7d87fcaa7a977527acfe0f526d1d` |
| gwz-py | `5f938a05fd505a83ab93c74c09533afe6fe6f3b2` |

No implementation, build, activation, push or tag is authorized by this verdict.

The peer-blind [Consistency](GwzCoreSessionDesign-ReviewConsistency.md) and [Safety](GwzCoreSessionDesign-ReviewSafety.md) reviews both returned **NO-GO**. The reports are filed verbatim.
- Both reviewers verified the exact tuple at the start and end.
- Both were fresh reviewers for this new object, running on Fable. The object was drafted on Opus, so this follows [GwzProcessOptimization §4.3](GwzProcessOptimization.md).
- No Surface review ran, because the public Python API and the CLI surface are unchanged.

| Axis | Verdict | Blocking | Nonblocking |
| --- | --- | --- | --- |
| Consistency | NO-GO | P2-1 to P2-4 | P3-1 to P3-13 |
| Safety | NO-GO | P2-1 to P2-7 | P3-1 to P3-5 |

**Classification.** Both reviewers classified every finding as a bounded correction; none is a new architectural root cause. Both pre-committed to GO on a revision that resolves their P2 findings as specified.

The core of the design held under both attacks. Safety found no deadlock in the admission classes and confirmed:
- several transport runtimes can coexist in one process, because libgit2 transports are installed per remote and never registered globally;
- cooperative cancellation is real where the transport exists;
- each operation's existing deadlines bound it.

Consistency confirmed that every existing service method is classified exactly once, and that the frame layout, the `with_local_transport` description and the ownership rules O1–O6 match their sources, apart from the findings below.

## Merged blocking root causes

| ID | Root cause | Findings | Convergence |
| --- | --- | --- | --- |
| B1 | `TransportSessionFull` and `OperationExpired` have no wire code, and the model-to-wire conversion turns them, and `Cancelled`, into `io_error`. A refusal with no effect cannot be told apart from an I/O failure. | Consistency P2-1, Safety P2-1 | Blind, both P2 |
| B2 | `with_local_transport` never exposes the transport request's cancellation handle, so the session host has nothing to signal. | Consistency P2-2 | Single axis |
| B3 | The error frame carries only a code and a string. It drops the structured error context the public Python errors expose today. | Consistency P2-3 | Single axis |
| B4 | Close waits without bound, contradicting the proposals' "within a bound". A hung handler leaves outstanding calls unanswered and the byte-stream host open. | Consistency P2-4 | Single axis |
| B5 | Transport settings and endpoint configuration are not session state. `configure_transport_runtime` is process-global and freezes after the first backend, and every operation re-reads the process environment. | Safety P2-2 and P2-6; Consistency P3-6 and P3-1(b) | Blind, P2 and P3 |
| B6 | Push takes the cross-process workspace mutator lock with a try-lock, so two pushes on one workspace cannot run concurrently as the class table promises. | Safety P2-3 | Single axis |
| B7 | Retention (64 terminal records) is smaller than admission (8 running plus 64 queued), so unread outcomes of accepted operations can be evicted silently. | Safety P2-4 | Single axis |
| B8 | A reused outstanding `call_id` is refused with an error carrying that `call_id`, so the legitimate call receives a second reply. | Safety P2-5 | Single axis |
| B9 | The client pump has no failure rule. A pump that raises leaves every outstanding call pending forever. | Safety P2-7 | Single axis |

Two of the nine roots were found independently by both axes: B1 (both P2) and B5.

Nonblocking convergences:
- in-process queue capacities and control-call accounting (Consistency P3-9, Safety P3-1);
- session ownership of `diff.output` and `log.output` logs (Consistency P3-11, Safety P3-4);
- direct reads waiting behind eight long operations (both axes' residual risks).

## Stop-rule accounting and next action

This is the object's first review round, and no reviewer classified any finding as a new architectural root cause. The [GwzProcessOptimization §4.1](GwzProcessOptimization.md) stop is therefore not triggered, and the next round is remediation round 1 of at most 2.

The next action is the single consolidated patch in the [remediation plan](GwzCoreSessionDesign-RemPlan.md), covering:
- the contract;
- the paired core paragraphs;
- the GWZ Taut schema additions it specifies.

Commit it as a new exact tuple, then obtain focused re-verdicts from the same two reviewers. The contract remains a draft.
