# Endpoint pool remediation — round 1, 2026-09-19

Status: **merged correction implemented and locally verified; awaiting original reviewers**.

Inputs: transport `554384f5808264009b3af016452290cacefab855`, core
`3522a1cc11991d81f9376c6ff134900233d3b4c1`, root
`49d6f76d005e1898e253fc16243f0f20291c05af`; original
[Code](GwzRemoteTransportPool-ReviewCode.md) and
[State](GwzRemoteTransportPool-ReviewState.md) NO-GO reports. Three independent
P2 roots, discovered before integration. No blind convergence or escaped defect.

One bounded patch corrects pool API/lifecycle/accounting and their tests/docs.
The API is unfrozen. The operator's instruction to reuse original reviewers
continues to override the fresh-reviewer preference for changed draft interfaces.
No protocol, stream, physical adapter or CLI/core communication change is needed.

| Finding | Disposition | Closure proof |
|---|---|---|
| Code P2-1 | Add host `idle_closed(ConnectionId)` acknowledging actual spontaneous disposal. Accept Idle or already Closing, validate scoped ID, remove and reschedule; reject Opening or Leased so a late idle callback cannot steal a lease. If checkout won, host routes the failure to the active exchange and its lease/cleanup path. | Loss before checkout causes a new Connect; checkout before loss preserves the exclusive lease; eviction/closure race frees capacity and wakes driver; foreign/duplicate callbacks cannot affect another entry. |
| Code P2-2 | Replace owner string with `Owner { session, operation }`; distinct `cancel_session` and `cancel_operation`. Host supplies the fresh binding session ID, reused operation IDs are permitted only in different session scopes. Stop admission for a dead session before cancellation; no new unbounded session registry in the pool. | Delayed session/operation cancellation for S1/op-1 leaves S2/op-1 live. Session cancellation affects all its queued/opening/ready/leased work, preserving idle and other sessions. Invalid owner fields refused. |
| State P2-1 | Rename construction bound `per_user_host`; count exact configured host + SSH username across ports, preserving full reuse key. HTTPS uses its no-username host bucket. Creation and eviction use the capacity group; requests cannot resize it. | Limit 1/host 4 test covers opening, leased, idle and closing across ports, another username, and allocation only after disposal. Independent random ledger checks the same user/host aggregate. |

Add regressions and observe failure before implementation. Update random generator
version because ownership/loss events and capacity policy change traces. Run
minimum-version full suite, fmt/clippy, direct replay, fixed 50,000 pool cases,
regeneration and standalone package checks. Commit the one corrected checkpoint,
then ask both original reviewers to verify their counterexamples and changed-range
interactions. One of two remediation rounds used; any further architectural root
must be classified by reviewers under the review-loop cap.

Executed: original cross-port counterexample failed before the correction; new
idle/owner API regression tests failed to compile before implementation. The
corrected 66-test Rust 1.95 suite, v2 fixed 50,000-case campaign, fmt/clippy,
regeneration, direct replay and standalone package checks pass. No finding is
closed until its original reviewer verifies the corrected committed tuple.
