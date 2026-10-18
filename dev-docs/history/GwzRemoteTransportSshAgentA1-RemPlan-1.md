# SSH agent A1 — merged remediation 1

Status: accepted after retained Code/State GO on the corrected tuple after independent Code and State NO-GO on core 3096966.
No architecture or scope expansion. Retained reviewers re-check their own findings.

| Finding | Disposition | Closure |
|---|---|---|
| State P2-1 | Publish only a successfully constructed singleton supervisor; serialize initialization and allow retry after spawn failure. | Inject first supervisor failure, race eight retries, assert one supervisor, then prove all 64 permits available and recover after abandonment. |
| Code P2-1 | Remove owned-channel extraction; keep the agent's state and channel together until drop. | Shared fake write record; repeated enumeration emits no second request; failed/unsupported/partial operations cannot replay; source interface has no extraction method. |
| Code P3-1 | Add deterministic publication-before-exit and joined-before-claim cancellation tests plus transferred-owner lifetime check. | Hold helper wrapper after publication, observe join via completion waker, cancel at each boundary, assert exactly-once destruction and retained capacity until join/disposal. |

Focused Rust 1.95 offline locked fixture gate; no platform/selected-source batch,
native signing or production activation. Preserve red/green evidence privately. Final gate: 54 executions pass, 16 A1.
Test ceiling refined from 700 to 750 lines for review-requested regressions;
742 actual. Production 557 lines/three files remains within the 20% allowance.
One merged patch, same-reviewer closure; this is remediation round 1 of at most 2.
