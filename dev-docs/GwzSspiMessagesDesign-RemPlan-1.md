# SSPI messages — merged remediation 1

2026-10-03. Initial Consistency NO-GO (P2-1), Safety GO, no blind convergence.
One reviewer-classified architectural root cause: HTTP cap lacks caller carrier.
One patch updates caller/design/profile/model; no field/tag change or runtime code.
Operator requested this review; correcting the necessary interface is within scope.

| Finding | Disposition | Closure |
|---|---|---|
| Consistency P2-1 | Add required AuthRequest.token_limit: TokenLimit, checked raw-byte u32 1–65536, no default or knob. Transfer its immutable value exactly into Begin; parent/worker/provider enforce without enlargement. Explicit bounded amendment supersedes previous no-API-change claims. | Synthetic caller-to-Begin model distinguishes otherwise identical limits, reference encode/decode preserves each; invalid 0/65537 and cap−1/cap/cap+1 plus narrower-provider checks; original Consistency reviewer retraces input bridge. Surface reviews revised caller guide cold. |

Safety has no findings; retrace changed limits/deadline/secret boundary before GO
on corrected tuple. Existing reviewers retained at operator's standing preference;
new Surface reviewer gets fresh context and reads only the guide. Explicit API
amendment is not grandfathered under earlier Surface GO. Mandatory production
codec/API Code/State gate stays open; model tests are not production proof.

Re-verdicts include closure table and changed-range analysis. Hard cap two
remediation rounds; this is round 1, architectural cause count 1. No tags/push.
