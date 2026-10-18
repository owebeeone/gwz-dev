# GWZ internal Taut compatibility amendment — remediation plan 1

Date: 2026-09-24. Reviewed object: gwz-core `e063bb020bc0d9023eff9fc0fa3f6bacbc2f8d8e`, workspace `a70fd7557762449174218dafd465dd2632dd964d`.

The peer-blind [Consistency review](GwzTransportInternalTautCompatibilityAmendment-ReviewConsistency.md) returned NO-GO on one P2 scope defect; [Safety](GwzTransportInternalTautCompatibilityAmendment-ReviewSafety.md) returned GO. The operator's internal-only transport compatibility decision remains the chosen outcome.

| Finding | Disposition | Single correction | Closure test |
| --- | --- | --- | --- |
| Consistency P2-1: waiver could cross into the outer GWZ request/response protocol | Accept | Restrict every waived reader/byte/old-version gate to the internal `gwz-transport` Envelope and its generated Rust/Python projections. Explicitly preserve GWZ ordinary-local request/response compatibility and old/new driver/core fixtures, receiver affinity and unsupported explicit-placement refusal. | Trace the revised boundary against remote requirements §5.1 G6 and placement §§3/8; retain ordinary-local old/new fixtures and explicit-cli/old-core refusal before operation submission. |

The correction changes the amendment's compatibility boundary. Submit the revised committed document to a fresh peer-blind dual review, file both reports verbatim, and merge a new verdict before acceptance.
