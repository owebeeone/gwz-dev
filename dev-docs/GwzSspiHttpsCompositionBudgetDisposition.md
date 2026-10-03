# HTTPS SSPI implementation budget disposition

2026-10-04. Owner disposition before broad source edits, under accepted
[composition §9](GwzSspiHttpsCompositionDesign-DRAFT.md).

The drafter's actual call-path inventory identifies approximately 30 handwritten
production/test files: five SSPI capture/export/test files; five transport
schema/policy/binding/validation/test files; approximately 17 core candidate,
bridge/test, worker, deadline, TLS/lease, endpoint/request/host and facts files;
one CLI dispatch and two Python entry/route files. Generated projections are
separate. Requiring 26 would encourage omitted live integration or arbitrary
consolidation of unrelated responsibilities.

The owner therefore permits **32 production/test files**, retaining the
**3,500 handwritten added-line** ceiling including tests and eight member/API
documents. This is a budget-only disposition, before exceeding 120% of the
original file estimate. It adds no consumer, policy, API, wire field, dependency,
process owner or activation scope to the reviewed design. Do not manufacture
extra modules to spend the allowance. Stop before exceeding this revised budget
or changing the accepted architecture; report exact modified-file/line counts.

The implementation remains one cohesive draft and one settled dual Code/State
secret-adapter review plus installed-caller Surface gate. Windows activation and
release remain separately gated.
