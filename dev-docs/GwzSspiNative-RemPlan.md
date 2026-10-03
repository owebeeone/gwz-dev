# GWZ SSPI native worker remediation 1

2026-10-03. State NO-GO at the initial settled tuple; Code and Surface GO with
P3s. Fix all findings in one bounded merged patch. Production state/wire/API,
identity and authentication policy are unchanged. Same reviewers re-verdict
their own counterexamples; the implementer does not self-close findings.

| Finding | Disposition | Closure evidence |
|---|---|---|
| State P2-1 | Accept. Guard the helper immediately after spawn; retain terminate/wait and coordination-file ownership through error/unwind. Report failed cleanup without calling a kill request proof. | Native forced failures after spawn, after PID publication and observer-handle acquisition; held helper/worker exit observations and removed owned scratch file; successful parent loss. |
| State P3-1 | Accept. Strengthen Job-drop and parent-death rows using still-suspended production children, so EOF cannot execute as an alternative exit cause. Correct earlier evidence strength. | Native held worker exit before Resume; a test-only owned-Job no-kill negative control may prove discrimination without source mutation or a product knob. No blocked-provider/descendant claim. |
| Code P3-1 | Accept. Mark old testing snapshot historical/superseded or update stale unconditional refusal. | Read complete Testing guide for consistent current status. |
| Code P3-2 | Accept. Add opt-in initial Negotiate fixture calling real observation/query/free path with synthetic input; allow legitimate unresolved/provisional intermediate result. | Pinned Windows result, checked query-allocation release if returned; actual production path, no fake enum; completed remote Kerberos/NTLM remains open. |
| Surface P3-1 | Accept. Correct stale CallerValues opening and link current WorkerEntry/native gate scope. | Cold cross-page status consistency. |
| Surface P3-2 | Accept. Fixture recipe preserves/restores prior process environment values (including absence) in finally; documents removal only of the unique new runtime after evidence retention. | Windows restoration cases absent/pre-existing values and forced failure, with actual owned-directory disposal; no full auth rerun required merely for documentation. |

Blind convergence: Code and Surface independently found stale unconditional
refusal statements in different guides. One P2 and five P3 finding records;
status-document maintenance is one shared cause. Reviewers found no production
credential exposure or publication defect and explicitly classified fixes as
bounded fixture/docs work. No new architecture is authorized by this plan.

Do not rewrite initial reports or raw receipts. Initial Job/parent-loss runs prove
held-handle exit with EOF as a competing cause; later suspended-child receipts
must carry the stronger containment claim separately. Raw new evidence remains
in the private member; public fixtures remain independently replayable.

## Corrected object and gates (pending reviewer closure)

Member 425e13dc011c42e94fdea31779a8e5967aedc82b; evidence
9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c; unchanged reference core
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31. Portable/cross/fmt/schema/package and
owner disabled-branch scope checks pass. Actual Windows Supervisor/native/default
suites, still-suspended Job/no-kill/parent-death cases, forced helper errors,
real Negotiate query/allocation release and recipe restoration/disposal pass.
Final owned-path process inventory is empty. New campaign preserves the failed
encoded recipe invocation and corrected file invocation separately. Same reviewers
must verify closure; no implementer closure or new policy/wire is claimed.
