# GwzRemoteTransportSshN2b remediation plan 1

Status: **implementation in progress** against the N2b review tuple.

The first dual review found two blocking P2 findings. This round keeps the
N2b interface and ownership design unchanged and closes both findings in one
bounded patch.

## P2-1 — absolute deadline across interaction and ready states

Disposition: **fix**.

The transport pool now carries the caller's absolute deadline into the helper
interaction clock, so the interaction budget cannot extend it. `advance` and
`next_deadline` also expire a request that became ready but was not claimed
before that deadline. Expiry uses the existing bounded cancellation/disposal
path; no lease is handed out after the deadline.

Closure tests:

- keep helper interaction open across the absolute deadline and require
  `InteractionTimeout` plus `CancelConnect`;
- complete a connection before the deadline, advance to the boundary, and
  require `AllocationTimeout` from `take` plus physical close scheduling;
- run the complete transport timeout suite and the SSH integration suite.

## P2-2 — admission ownership and retained-cleanup evidence

Disposition: **fix**.

The selected-pool integration suite now uses the existing injected reader seam
with deterministic release barriers. It proves that a stalled admission:

- expires without invoking native setup, while the snapshot charge remains
  held until the helper joins;
- remains visible as `pending_admissions` during combined endpoint shutdown,
  then reaches `cleanup_complete` only after release and disposal;
- does not stop an already active stream while the admission is stalled; and
- produces no late setup or stream effect after the caller deadline.

Closure tests:

- `stalled_admission_expires_without_setup_and_retains_charge_until_join`;
- `stalled_admission_does_not_stop_an_existing_stream`;
- the existing selected-pool, worker, and supervised cleanup tests;
- refreshed evidence from the exact post-remediation source tuple.

The production route remains inactive as required by N2b. Platform and
selected-source qualification, HTTPS, and N3 backend attachment remain
deferred.
