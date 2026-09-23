# Python session v2 — second review merge and bounded remediation

Date: 2026-09-24. Status: **NO-GO pending focused re-verdict**. Reviewed tuple: root `59c13d6184c82789d9dc558ca436b3a43be8bdcd`, core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`, Python `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`. [Consistency](GwzPyTransportSessionV2-ReviewConsistency-2.md) and [Safety](GwzPyTransportSessionV2-ReviewSafety-2.md) reported NO-GO; [Surface](GwzPyTransportSessionV2-ReviewSurface-2.md) reported GO. Each prior blocking finding closed in the design text. Neither blocking reviewer classified its new finding as an architectural root cause.

| Finding | Disposition in one correction | Closure evidence |
| --- | --- | --- |
| Safety P2-1 | Reject request IDs longer than 256 UTF-8 bytes before issuing a handle. Charge bounded identity and descriptor copies to the 64 MiB ledger. | Over-limit synchronous refusal; 64 maximum IDs plus descriptor/close copies stay within budget through close and expiry. |
| Consistency P2-1 | Scope the native host, ledger and close report to `NativeCoreBridge`; preserve `Client.close() -> TransportCleanup | None`, returning/storing `None` for a custom bridge lacking `close`. Custom bridges keep existing call/stream methods; new handle/ledger hooks refuse if absent. | Custom bridge with no close returns `None` from close and context exit leaves `close_report=None`; native close returns attributed report. |
| Consistency P3-1 | Split retained-stream `aclose()` and true dropped-stream discovery into separate proof cases. | Drop case holds no stream reference when recovering by `recent_operations()`; retained case verifies `aclose()` summary. |
| Surface P3-3 | State omitted push remote selection and `remote_check` default beside the push example. | Docs-only caller can determine destination and check behavior without source inspection. |

Commit the correction at an exact root/core/Python tuple. Because this is bounded text clarification, ask the same Consistency and Safety reviewers to re-trace their P2 counterexamples; Surface P3 is nonblocking and has a clear documented disposition. No implementation starts until both blocking reviewers return GO on the corrected tuple.
