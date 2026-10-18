# Windows HTTPS WH1 — merged round-2 verdict

2026-10-04.

**The reviewed tuple** (lane `wh1-rem2`):

| Repository | Revision |
|---|---|
| root | `1513c214fbc9bc492875a8244c7ed62ed4169a3a` |
| core | `f76cf4cc861f5697817ef43be784c6c88558b26e` |
| transport | `966763e429c6c894da52e498b6f95ffd3d5ab5b5` |
| evidence | `56d93909781cc06626054d1210307f483efb72f8` |

Other members are unchanged. Fresh reviewers reviewed it, because round 2 adds the shared interface `Owner::send_if`.

**The reports:**
- [Code-2](GwzWindowsHttpsIntegrationImplementation-ReviewCode-2.md): **GO**, with one non-blocking P3-1. No test proves that `send_if`'s admit answer and the queueing happen in one critical section.
- [State-2](GwzWindowsHttpsIntegrationImplementation-ReviewState-2.md): **NO-GO** on **P2-4**. On an HTTPS-only endpoint Session, a stale admitted non-Open action for a retired HTTPS stream is routed to the absent SSH engine. The resulting `InvalidRequest` closes the session. That loses the pending terminal and fails every concurrent stream. State pre-commits to GO on the specified correction.

**Both axes independently verify State-1 P2-3 closed.** They re-traced its original interleaving, checked RED-v3 against the base, ran the mutation checks, and reran the three Session tests and the transport tests. Round 1's Code P2-1 and P2-2, and State P2-1 and P2-2, remain closed.

**Merged gate: NO-GO for limited WH1 acceptance.**
- P2-4 entered with the original WH1 implementation, core `398158b`, outside round 2's changed range.
- State classifies it as **not architectural**: an HTTPS-only omission of the existing contract that stale admitted input is no-work.
- Neither reviewer found a new architectural root cause.

**The cap.**
- Rounds 1 and 2 are used.
- GwzProcessOptimization §4.1 and the review-loop cap permit a third round confined to non-architectural corrections. Any architectural root cause found in it stops the lane for redesign.
- [Remediation plan 3](GwzWindowsHttpsIntegrationImplementation-RemPlan-3.md) applies it.

**Native evidence.**
- Code-2 risk 1: the native evidence predates round 2's sources. An exact-source native refresh is required before acceptance.
- It runs on the operator's Windows host at the round-3 tuple.

**No finding is closed by its implementer.**
