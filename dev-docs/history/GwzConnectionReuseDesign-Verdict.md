# GWZ connection reuse design — first review verdict

Date: 2026-09-27. Status: **NO-GO at SHA-256 `573d4e958289710407535b2232a53cc6efa84afc61b215c757642c15a05ccf95`: [Consistency](GwzConnectionReuseDesign-ReviewConsistency.md) and [Safety](GwzConnectionReuseDesign-ReviewSafety.md) both reported NO-GO. Both reviewers committed in advance to GO on a revision that resolves their blocking finding as specified.** This verdict accepts nothing.

The object was the reuse design, step TR1.2 of the transport release plan: the draft committed at root `9bce0e4`, plus the pre-review edits for the plan amendment's key-types rule, identified by its SHA-256. It was read against root `4bf52e00`, gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5` and gwz-transport `a7a36aec`.
- Two fresh reviewers ran in parallel. Neither read the other's report.
- Both verified the object and the five HEADs at the start and the end.
- Both read the session plan only at its committed version, because TR1.4a was revising it in the working tree.
- Both ran only inspection commands. Neither wrote a file.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | NO-GO | 0 | 0 | 1 | 7 |
| Safety | NO-GO | 0 | 0 | 1 | 4 |

## Blocking findings

| ID | Axis | Finding |
| --- | --- | --- |
| C-P2-1 | Consistency | §14's T1 says idle pool entries have no owner; T3 says an idle entry keeps its last owner. Implemented as T3 reads, `cancel_session` for an abandoned binding would close the idle connections it last used, which breaks §8's isolation and TR1.2 question 8. |
| S-P2-1 | Safety | §8 promises that other clients' active exchanges run to their end when an instance's worker thread ends or a panic is caught. The worker's loop holds those exchanges and drops them on unwind, and the accepted A3 slice says a worker panic recovers by shutdown. Through a server, one panic in instance code fails every client's active exchange on that instance, which the text denies. |

Both reviewers classified every finding as a bounded text correction. Neither is architectural: the sharing model, the trust model and the ownership split held under both attacks. This was the first round; the two-round cap has one round left.

## Blind convergence

Both axes, reviewing blind, landed on the same clauses:
1. **T3's ownership record.**
   - Consistency P2-1: T3 contradicts T1 and §8.
   - Safety, as a residual: T3's comparison must be by the binding (`Owner.session`), not the full owner, whose serial differs per request.
2. **The registry's refusal code.**
   - Consistency P3-3: `transport_session_full` gains a new cause without a contract amendment, and the spent-cleanup-budget refusal has no code.
   - Safety P3-3: the code points the operator at the wrong limit, and the refusal does not wait within the admission deadline.
3. **The proof's lease and job.**
   - Safety P3-4: a proof job shared by concurrent leases has no stated clock or owner.
   - Consistency, as a residual: the connection is checked out during the proof, but T4 retires only idle entries.
4. **The instance's threads against A3.**
   - Safety P2-1: the fault scope contradicts A3's panic rule.
   - Consistency P3-7: "one supervisor per instance" is undefined, and collides with A3's no-reaper-per-endpoint rule.

## Next action

[GwzConnectionReuseDesign-RemPlan.md](GwzConnectionReuseDesign-RemPlan.md) maps each finding to one disposition and one closure test. The whole mapping is applied as one revision. The same two reviewers then give a focused re-verdict on the new SHA-256, with the plan and the diff.
