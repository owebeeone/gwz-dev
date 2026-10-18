# SSPI supervisor remediation 1

2026-10-03. **Complete: original reviewers verified all findings closed and returned GO.**
GwzSspiSupervisorAcceptance.md records the exact corrected tuple.
The initial NO-GO and dispositions below are retained as the audit trail. Review tuple:
root d2821a2db90aa641b1af6f80cadaf1aba9b35a0c, member
fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a, unchanged reference core
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31. One merged patch; native provider,
wire format, public lifecycle choices and platform assumptions remain unchanged.
No finding is self-closed by the drafter or owner. Original reviewers verify their
counterexamples on the corrected committed tuple. This is remediation round 1.

| Finding | Disposition | Required closure |
|---|---|---|
| Code P2-1 | Accept: make start/shutdown opaque futures explicitly independent of receiver lifetime with precise capture. Keep step's mutable borrow. | Compiled public examples accept Future + Send + 'static and retain/use each future after dropping Supervisor, without native execution. |
| State P2-1 | Accept: publication's lost-record fallback recovers authoritative terminal/completion cause before synthesizing Protocol. | Deterministic cancel/expiry between readiness and publication, both taken-token and retired-token orders; Confirmed cleanup, winning error, no token. |
| State P2-2 | Accept: wipe owned challenge on every completed error path outside state lock. | Existing live-before-deallocation probes with retained completed future: oversized, wrong phase, cancelled and expired input; no frame sent. |
| Code P3-1 | Accept documentation alternative: pre-registration snapshot-handle disposal is synchronous on refusal/Drop. Remove blanket no-OS-call promise; explicitly exclude metadata/provider/worker/thread creation, IPC and process waits/joins from polling, distinguish captured-handle destruction, and disclaim a hard OS time bound. No offload queue or new worker mechanism. | Fake successful Origin capture with observable Drop for cancellation, expiry and closed admission; dispose during refusal as documented. Existing native resource destruction remains outside state locks; Windows runtime remains unqualified. |
| Code P3-2 / State P3-1 | Accept, blind convergence: add fake-port coverage of production orchestration decisions, not merely manually supplied kernel proofs. Private bounded iteration extraction is permitted; drivers retain existing effects and ownership. | Fake child/launch/read/write paths cover late successful launch without resume or secrets, containment/resume failures, termination/observation failures, owner failures/panics, unfinished owner tasks and eventual disposal/completion before capacity release. Reproducible traces; no sleeps or native processes in fast tests. |
| Surface P3-1 | Accept: identify host-owned trusted packaging metadata as expected fingerprint source, exact matching worker Hello bytes; no invented runtime executable-hash rule. Packaging producer itself remains step 4. | Cold caller can construct WorkerExecutable/Supervisor from explicit host metadata; compiled synthetic construction example clearly labels absent production packaging. |

The P3 items are included in this existing blocking patch; they do not create
additional work packages or blocking gates. The three P2s are concrete API/failure
projection/secret-lifetime corrections, not evidence for protocol redesign.
The two axes independently identified the same ownership-bridge coverage gap;
their distinct blocking findings did not converge.

After corrections, run focused regressions, full standalone Rust/schema/lint/fmt,
disabled-branch scope, Windows cross-target and archive checks. Settle via GWZ;
send the revised tuple, this merged plan and changed-range diff to the original
reviewers. Any material architecture/interface-policy change instead requires a
fresh numbered review. Native SSPI does not start until this checkpoint is GO.
