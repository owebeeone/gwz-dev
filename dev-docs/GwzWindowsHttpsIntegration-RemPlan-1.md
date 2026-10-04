# Windows HTTPS integration — remediation 1

2026-10-04. Design-only correction to reviewed root
`85efff17673d919f12d93846447c6092efc11879`; product member/evidence reviewed
revisions are unchanged. One bounded remediation round. Safety was GO;
Consistency identified one P2. Reports are filed verbatim alongside this plan.

| Finding | Disposition | Closure |
| --- | --- | --- |
| Consistency P2-1: nonexistent CLI/Python helper-disable option | Accept. Replace the false option claim with a fixed qualification-only backend construction at core `TransportRuntime::request_kind`: existing `without_credential_helpers()` plus unchanged host context. Outside the exact qualification cfg retain `new()`. No flag, API or schema added. Explicit configured/Gh Opens remain refused. | Original Consistency reviewer retraces actual caller reachability on corrected draft §§3/6/9. WH1 regression tests later observe actual policy and context through shared construction and normal CLI/Python call/submit; WH3 runs installed real HTTPS/Git cases. Ordinary routes preserve original policy. |

No other blocking finding exists. The correction implements the original
reviewer's bounded remedy, preserves the reviewed architecture and supersession,
and adds the actual installed caller recipe. The same Consistency reviewer
re-verdicts its counterexample. Safety is asked to confirm the narrow policy
construction disposition on this revision. No implementation is accepted by
closing a design finding. Full Windows release remains NO-GO.
