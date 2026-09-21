# Placement A — correction 2

Status: DRAFT. Baseline root `e4bf1328de6ac5a56645a13827a0b6d739a70462`,
core `018176c6783d900570ba8f918aa744bd642fc3b6`, transport
`d7051eede48f6cd8b07815edc9ced1916f82585c`; remaining pins in round-one reports.

Round-one State and Surface GO; Code closes both original P2s but identifies
one changed-range error-domain defect. This is the second bounded correction;
no schema, API, architecture, carrier or activation change.

| Finding | Disposition | Closure test |
|---|---|---|
| Code P2-3 | Accept. Centralize BindRejected admission to UnsupportedVersion or UnsupportedOperation, Effect::None and absent facts. Apply to incoming codec envelopes and local rejection construction. Invalid endpoint configuration fails locally. | Typed and encoded Authentication/Io (and all other forbidden codes), effects/facts fail admission; mux closes Protocol with no bootstrap_failure/work. Invalid endpoint settings fail construction. Both allowed codes retain exact reason through Rejecting, async port handoff, closure and readiness. |

Retained Code reviewer verifies its counterexample and changed range. Retained
State checks that strengthened admission leaves its lifecycle proof valid on the
same tuple. Surface docs are unchanged; its prior GO carries only over unchanged
surface bytes. No third implementation round is planned.
