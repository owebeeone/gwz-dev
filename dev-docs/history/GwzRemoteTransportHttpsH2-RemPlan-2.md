# HTTPS H2 — consolidated correction 2

Status: closed by retained Code/State GO in ReviewCode-2 and ReviewState-2 at
core `c92abc4110fc7c1ef89600118284724c942f8985`; exact tuple in H2 acceptance record.

Correction 1 closed all three original blocking roots (four findings). State GO
and Code NO-GO are filed verbatim in ReviewState-1 and ReviewCode-1. Code found
one new architectural root in the new serialization stage: admission wait receives
an independent allowance before a fresh endpoint allocation allowance.

| Finding | Disposition | Closure test |
|---|---|---|
| Code P2-3 | Charge canonical-route gate contention to the existing allocation deadline. Start before the gate, reject exhaustion before Open, carry only its positive remainder to the first Open. Preserve the endpoint's cumulative retry budget. No new field or public API. | Short-budget, synchronized gate contention followed by endpoint allocation waiting; assert original total bound and reduced carried allowance. Exhaustion must issue no Open. |

This is the second consolidated correction, with one newly classified architectural
root during remediation. Original findings stay reviewer-closed; no self-closure.
Retained Code re-verdict targets P2-3; retained State confirms the changed range
preserves its prior GO at the same final tuple. No fresh reviewers per operator
instruction. No platform/selected-source, activation, release or physical-wire work.

Drafting note: owner inspection rejected a global-counter fixture (parallel-test
interference) and corrected missing argument plumbing before the first compile.
The retained test uses per-call Open-deadline observation and the real one-slot
physical pool; no global counters remain. This is an implementation-stage fixture
correction, not another reviewer finding or an accepted-code escape.
