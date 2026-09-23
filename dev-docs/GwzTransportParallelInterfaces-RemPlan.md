# Parallel transport interfaces: correction 1

Both Consistency and Safety independently found the same two roots. Surface
found two additional concrete Python lifecycle API gaps. One bounded document
patch addresses all findings; implementation has not started on either new seam.

| Findings | Disposition | Closure test |
|---|---|---|
| Consistency P2-1, Safety P2-1 | Freeze enum values 1–7 and reserved/unknown behavior. | Seven golden CBOR cases, missing cause, old-reader drop, unknown rejection. |
| Consistency P2-2, Safety P2-2 | Rust-owned monotonic session lifecycle orders close, construction and admission; late construction cannot publish after close. | Barriers for both races, close-before-first-use, typed post-close refusal, one shutdown. |
| Surface P2-1 | Specify async Client/bridge close signatures and typed cleanup result, repeated-close behavior and context-manager path. | Await twice, identical retained result; context exit uses same close. |
| Surface P2-2 | Specify operation-ID cancellation, bridge-local request mapping, wrong/expired identity behavior and completion after finish. | Active setup/stream cancellation, wrong ID, queued waiter cancellation, observable cleanup. |

No change to accepted retry defaults or retry set. Closing/cancellation snapshots
must not introduce an unbounded completed-operation registry. The named tests are
implementation obligations, not tests already executed by this document review.
