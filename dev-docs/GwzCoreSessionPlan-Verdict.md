# GWZ core session plan, revision 1 (TR1.4a) — first review verdict

Date: 2026-09-27. Status: **NO-GO at SHA-256 `51cea8b6166d4cc6a54beb15c1cbebceeec0172e1a3ab25b8daf98c447f43fc3`: [Consistency](GwzCoreSessionPlan-ReviewConsistency.md) reported GO and [Safety](GwzCoreSessionPlan-ReviewSafety.md) reported NO-GO. Safety committed in advance to GO on a revision that resolves its two blocking findings as specified.** This verdict accepts nothing.

The object was revision 1 of the session plan, which applies step TR1.4a of the transport release plan, identified by its SHA-256. Revision 0 is the plan committed at root `63c58f6`. It was read against root `4bf52e00`, gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5` and gwz-transport `a7a36aec`.
- Two fresh reviewers ran in parallel. Neither read the other's report.
- Both read every controlling document at its committed version. They read the accepted reuse design, and the drafter's eleven disclosed open points, from frozen copies.
- Both verified the object and the five HEADs at the start and the end, and ran only inspection commands.

| Axis | Verdict | P0 | P1 | P2 | P3 |
| --- | --- | --- | --- | --- | --- |
| Consistency | GO | 0 | 0 | 0 | 5 |
| Safety | NO-GO | 0 | 0 | 2 | 5 |

## Blocking findings

| ID | Axis | Finding |
| --- | --- | --- |
| S-P2-1 | Safety | The legacy transition rule covers only the two candidate-only transport entries. In the ordinary build, gwz-cli and gwz-py call the handlers directly, so when CS3.3, CS3.4 and CS3.9 land those callers have no snapshot, host context or lock manager. CS3.3 changes ordinary-build behaviour and is unmarked, and its spawn entries could flip to `permanent` while the legacy path still reads the live environment. |
| S-P2-2 | Safety | CS1.1 changes the production protocol file, but the candidate build compiles a separate, hand-maintained protocol file that no step names and no CI builds. After CS1.1 the candidate build breaks silently, and every transport row waits on an unowned hand edit. |

Neither reviewer classified any finding as architectural. This was the first round; the two-round cap has one round left.

## Blind convergence

Both axes, reviewing blind, landed on the same clauses:
1. **CS2.10's bridge wait.** Consistency P3-4 and Safety P3-3 both find that `diff_log_read`'s bounded wait changes a public bridge behaviour, and that neither R8 nor the Phase 4 Surface list carries it.
2. **The `cancellation` capability under native routes.** Consistency P3-5 and Safety P3-4 both find that C1 and D7 ignore the adopted native routes: the off switch, TR1.6's non-gh HTTPS route and OD11's key-type route.
3. **Ordinary-path marking.**
   - Consistency P3-1: CS4.7 and CS4.8 are unmarked, although §6(b) gives TR1.4a the duty to mark every such step.
   - Safety P2-1: CS3.3 is unmarked.
4. **TR3.1's edges.**
   - Safety P3-1: the other candidate-only steps lack TR3.1 as a dependency.
   - Consistency, as a residual: the TR3.1 edge revision 1 adds is not in the transport plan's sketch.

## Next action

[GwzCoreSessionPlan-RemPlan.md](GwzCoreSessionPlan-RemPlan.md) maps each finding to one disposition and one closure test. The drafter applies the whole mapping as revision 2. The same two reviewers then give a focused re-verdict on the new SHA-256, with the plan and the diff.
