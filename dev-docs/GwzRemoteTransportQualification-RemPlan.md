# Q6 pre-gate qualification corrections

Status: implemented and tests pass; retained reviewer verification pending.
Q6 is the newly authorized Phase6 qualification object after H2 acceptance.
These defects were found before its aggregate gate; no Q6 review remediation
round has been used. Prior H2 round counts are unchanged.

| Finding | Disposition | Closure evidence |
|---|---|---|
| DiscoveryState P2, repeated logical retirement | Record monotonic mux_retired per registration; preserve independent physical cleanup and deadline behavior | Deterministic completed-request regression runtime red then host53 green; long repeated HTTPS clone workloads now pass on Mac/Linux; retained State must verify |
| Q5 State P3-1 diagnostic retention | Write raw completed/timeout outputs and status before checking post-command Cargo configuration; separately preserve rejection in persisted state | Completion success/failure and timeout with post-check rejection tests; ten runner tests pass Mac/Windows; retained State must verify |

No protocol/public surface, timeout default, physical pool limit or production
dependency changes. Initial compile/setup failures remain raw evidence, distinct
from the meaningful runtime-red proof and successful native runs. No independent
publication or full-platform claim. The public Q6 report controls the bounded
batch scope, raw run links, measurements and unresolved Phase6 obligations.
