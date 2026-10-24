# GWZ Connection API contract — remediation round 2

Date: 2026-10-19. Reviewed revision 1:
`011d3eb0a487dbf62e10432034a0105be2518a22`.

## Finding and single-patch disposition

Consistency round 1 GO closed its original P2 and two P3s. Safety round 1 closed
its original permit-revocation P2 but identified **Safety P2-2**, a new architectural
root: sequence advancement/Applied receipt was separated from actual byte-state
mutation by an owned token handoff. Total architectural roots recorded: two.

Accept Safety P2-2. Revision 2 removes the owned AppliedEvent/take_applied surface.
Use the existing kernel's synchronous atomic apply model: StreamOrder retains
the event, passes a borrowed CandidateEvent to the byte/control callback, and
advances sequence/publishes Applied only after that mutation succeeds. A no-effect
WouldBlock retains the same event and charge. Successors and orderly terminals
cannot commit ahead; cancel fences are serialized with application. No token
escapes to a worker or later host-scheduled application. Callback panic/rejection
faults the lane without Applied or replay of uncertain mutation.

This is one contract patch, updating §6.1, its invariant matrix and amendment row.
No other owner, platform, native mechanism, dependency, source, feature or wire
schema changes. The neutral start, Git wrapper and version-free byte entry remain;
the unsafe handoff is replaced by the established apply-before-Applied boundary.

## Closure specification

- Hold sequence 1 at eligibility using a no-effect WouldBlock. Queue data 2 and
  orderly terminal 3. Neither commits/reports Applied until 1 actually mutates
  byte state. Another lane completes without a host ordering barrier.
- Resume retries the retained predecessor once, with its charge still present;
  no copied or separately owned event can apply out of order.
- Cancel before application and immediately after commit. Outcomes are fenced
  uncommitted work or a committed prefix then cancellation, never delayed apply.
- Keep both non-Git generic traces and existing profile-3 vectors on one factored
  algorithm. Recheck prior permit/domain/input-budget closures are unchanged.

Reviewers must independently verify these text/design traces on the revised
committed object. No executed proof or implementer self-closure is claimed.

## Cap and reviewer continuity

This is the second remediation round. Continue the same two axis reviewers under
the operator's recorded “use the old reviewers” direction; no extra reviewers.
Consistency rechecks Surface/compatibility and earlier closures plus the changed
commit boundary. Safety re-runs its original P2-2 counterexample and prior closure.
Both classify any new architectural root. A third such root stops this lane for
an operator redesign-or-accept decision; do not append another architecture patch.
