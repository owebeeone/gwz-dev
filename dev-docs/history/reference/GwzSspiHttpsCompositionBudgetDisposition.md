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

## Resume call-path adjustment

After restart, the draft reached exactly 32 handwritten files. Two existing
live call sites are indispensable to the already accepted protocol: transport
`src/mux/mod.rs::begin` must include native policies in its profile-2 offer while
retaining the bootstrap offer's profile-1 default; core
`src/transport_host/session/driver/opening.rs::open_https` must emit Ambient
identity for native policies as §7 requires. Neither was in the earlier estimate.

The owner permits **34 handwritten production/test files** for this exact
completion, with 3,500 added lines and eight member/API docs unchanged. No new
API, policy, owner, dependency or activation scope is added. Preserve the existing
bootstrap default and test the actual host mux path. This is a budget-only
correction to the estimate, before either extra call site is edited; it does not
waive implementation review or authorize omitted graph/test obligations.

## Required source-ratchet ledger

The Python candidate-switch guard reports two new sites in the already counted
`native/src/client_host.rs::network`: original-caller capture and attaching that
capture to the selected route. The owner authorizes the corresponding two rows
in `gwz-py/scripts/candidate_switch_inventory.txt` before editing it. This is one
administrative ratchet ledger in addition to the 34 handwritten production/test
files (35 files when that ledger is included); the source-line and member/API-doc
ceilings remain unchanged. It records the two accepted existing-entry seams,
not a new switch, consumer or activation rule. The actual guard must pass after
reconciliation; adding rows is not evidence that the seams work.

The final call-path implementation replaces the initially drafted mapping hunk
in `request.rs` with the actual coarse-policy selector in
`git/gitbackend/transport_binding.rs`. This substitution stays within the same
34 source/test files and preserves explicit Anonymous policy rather than
reinterpreting it as WindowsDefault.
