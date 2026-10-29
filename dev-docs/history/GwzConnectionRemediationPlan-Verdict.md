# GWZ Connection remediation plan — merged verdict

Date: 2026-10-19. **GO — plan review only.**

## Reviewed object and authority

The user requested the review loop on `GwzConnectionRemediationPlan.md`.
The exact object is the 204-line draft committed at root
`468975076f94cd761aa07ba4b47c2c4af797a892`, Git blob
`6befb9fee5ca60879fd98016f133148afca22e6d`, SHA-256
`6335e68c5058095b890246079e1ad814788d581d9e70f1d7d7c356345d288403`.

The [review package](GwzConnectionRemediationPlan-ReviewPackage.md) records the
full nine-repository tuple, original operator brief, authorized scope, generated
canonical prompts and review tier. Other repository sources were reference-only.
The other three remediation drafts and unrelated work were excluded.

Two isolated peer-blind reviewers inspected the same object and verified the
tuple at start and end. They inherited the session model; the exposed harness
does not provide a cross-family reviewer, so that diversity is not claimed.
Consistency included the proposed lifecycle surface. This was not a concrete
Rust API freeze or a test of implemented behavior.

## Results and cap accounting

| Axis | Report | Verdict | P0 / P1 / P2 / P3 |
| --- | --- | --- | --- |
| Consistency | [Verbatim report](GwzConnectionRemediationPlan-ReviewConsistency.md) | GO | 0 / 0 / 0 / 0 |
| Safety | [Verbatim report](GwzConnectionRemediationPlan-ReviewSafety.md) | GO | 0 / 0 / 0 / 0 |

Initial review: one round. Remediation rounds: zero. Blocking findings: zero.
New architectural roots in this review object: zero. No remediation patch or
remediation plan was needed; no finding was self-closed by the drafter.

There was no blind convergence on a new defect. Both independently confirmed
the existing runtime-lifetime problem: retaining a Python native auth host does
not retain its operation-created transport runtime or pool. They also confirmed
that retaining today's runtime unchanged is insufficient because lifetime
registration/identifier state is finite. Existing in-process delivery and fake
pool/stream tests support a small proof without requiring a new physical carrier.

These observations support the plan's proposed route. They do not prove that
the future library, adapters or real-client reuse already work.

## Acceptance boundary and remaining decisions

Accept the plan's route to a contract, placement comparison, clause-specific
amendments and small-proof test specification. Concrete API signatures, package
placement, secret/authentication inputs, timeout-domain details, cross-operation
SSH checks, HTTP allocation, native process-safety coverage and finite lifecycle
policies must still be settled and reviewed at that next boundary.

No source implementation, new repository, dependency change, protocol revision,
activation or release is authorized by this GO. The other three remediation
plans remain unreviewed. No native tests, build, network run or qualification
campaign was performed during this document review.

The only post-review change to the plan is its acceptance/status paragraph and
links to the reports/verdict. Its substantive reviewed body is unchanged. Reports
are filed verbatim; the review package and this verdict are the per-object
checkpoint, avoiding further growth of the already oversized aggregate checkpoint.

## Next handoff

Prepare the plan's contract and small-proof design package when directed; do not
start the larger historical session programme or source migration from this GO.
The existing two oversized root-document dispositions remain separate cleanup.
No push, tag, publish, lane merge or member change occurred in this review loop.
