# HTTPS H1 correction 2 — retain ownership during child reaping

Status: owner-found continuation of the helper cleanup invariant, confirmed by
causal tests after both correction-1 reviewers returned GO. No acceptance was
recorded between finding and correction. This is the second consolidated
remediation round; reviewers classify whether it introduces an architectural
root. Scope remains private H1 helper cleanup.

| Finding | Disposition | Closure |
|---|---|---|
| Owner P2-1: reaping transfers owned children into an unguarded local vector across await; abort can free permits before actual reap, and return counts omit concurrent arrivals. | Accept. Retain a guarded reap batch; its Drop returns unreaped children and permits to the same owner. Include in-flight reap ownership in pending counts, and count all queued arrivals on return. Apply the same retention to orphan fallback reaping. Read active preparation count before final retained-helper count at endpoint shutdown, so active-to-retained transfer cannot disappear between snapshots. | `aborting_reap_preserves_child_and_permit_ownership` and `reap_reports_children_arriving_while_it_waits` both fail on correction1 (reported0, expected1), then pass after correction. Existing post-start cancellation, abort, endpoint isolation and 64-test gate must remain passing. |

Both original reviews and correction1 GO reports remain verbatim. The original
nine findings are not erased; this narrower cleanup case escaped that review.
No released defect is claimed: H1 remains a nonactivated candidate. No new
protocol, public interface, dependency or platform work. Retained State reviewer
checks this exact counterexample and changed-range interactions; retained Code
reviewer confirms the prior verdict carries to the corrected tuple.

Closure: retained Code and State correction2 reports both return GO at root
`63ef26308979b6ce2e2925d71a96f42afcde2645`, core
`e29e799ee65fb9794ac2fad7972d94707262b4cb`. Both classify Owner P2-1 as a bounded
continuation of helper ownership/accounting, not a new architectural root.
All H1 findings are reviewer-closed. Endpoint66 and scoped checks pass.
