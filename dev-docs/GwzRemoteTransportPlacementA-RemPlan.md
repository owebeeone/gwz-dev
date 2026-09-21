# Placement A — merged correction 1

Status: DRAFT correction to root `9cc9747c9466e877579bf95779dece092864b97e`,
core `c0649c3d9a4804cee6eb180911da4863266c3045`, transport
`f6e9ee21fe5e39e3fc61d8484a9410062728131c`; other pins in the reports.

Code and State independently found the terminal-retirement defect. There are
two distinct blocking root causes, not three. One combined patch; unchanged
schema, physical ownership, no-carrier and activation deferrals. Retained
reviewers recheck their findings and direct changed-range interactions.

| Finding | Disposition | Closure evidence required |
|---|---|---|
| Code P2-1 / State P2-1 | Accept. Preserve admitted terminal frames during seal/cancel; keep terminal handoff request-owned until port transfer (or local worker/action transfer for received terminal cleanup). finish returns WouldBlock while routes or terminal frames remain. A stalled handoff expires through bounded generation closure, never silently succeeds. | Every terminal family through direct and async ports; exact facts; worker and timeout terminals; queue saturation, repeated cancel/finish, deadline and drop/closure propagation. |
| Code P2-2 | Accept. Valid incompatible bootstrap queues typed effect-free BindRejected; malformed Bind remains protocol failure. Rejection is retained through retirement and available to the initiator readiness caller. | Incompatible limit and v1-only offer yield exact reason, zero bindings/routes/work; async ready distinguishes rejection from closure; malformed Bind closes without typed negotiation acceptance; stalled rejection handoff expires. |
| Surface P3-1 | Accept, nonblocking. Add a small construction/register/bootstrap example to the lower-level README. | Trace signatures against implementation and compile the documentation fixture; no claim that the deferred core facade exists. |

The public lower-level mux adds a Rejecting lifecycle state and a Rejected
readiness error plus a typed bootstrap_failure accessor. This exposes the
already-frozen BindRejected meaning; it does not change fields or wire tags.
Rejecting admits no work; the bounded pending reply is transferred before local
retirement, or host closure/deadline aborts delivery. No terminal receipt proves
physical cleanup. Preserved replies use the existing queue bounds/reserves.

Acceptance requires focused retained Code and State GO on the corrected tuple;
Surface checks its documentation correction. This is correction round 1 of 2.
