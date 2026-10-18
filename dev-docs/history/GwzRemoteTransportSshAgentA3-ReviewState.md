# SSH agent A3 supervised pool integration — State-axis review

**Review object:** A3 supervised physical-pool/shared-worker integration at `gwz-core` `e92c5d1ec09dd64a397dbacf6c78888955e4ae12`, controlling draft `dev-docs/GwzRemoteTransportSshAgentA3.md`, implemented local checkpoint, 2026-09-21  
**Baseline:** root `fe68d36f939de8cba2bc8a509f85a24cefe85db5`; `gwz-core` `e92c5d1ec09dd64a397dbacf6c78888955e4ae12`; `gwz-core-evidence` `dd5b5f144c4db29968d578655ca279a7f74c1119`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; the clean checked-out tuple was used only for the permitted external-target test.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics and adversity. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I read the canonical State prompt, the complete A3 controlling document, the accepted helper-design ownership and lifecycle requirements, `CurrentProgramCheckpoint.md`, and the private `agent-a3` evidence README, inputs, red factory-panic log, corrected panic log, and final full-gate log.

The changed implementation was inspected at the pinned core commit:

- `ssh_setup.rs:22-311`: setup factory, original-origin deadline conversion, authenticated handoff, connecting/idle/active/disposed states, reclamation, Facts, and disposal.
- `ssh_pool.rs:16-37,47-260`: physical-owner ledger, destruction-before-acknowledgement, connect-unwind settlement, shutdown, and field/drop order.
- `ssh_worker.rs:55-272,293-539`: endpoint admission, worker ownership, shutdown timing, request/lease state, resource attachment, observations, and transfer/release.
- `ssh_shutdown.rs:14-87` and `agent_job.rs:12-359`: sticky status, whole-host retention, helper and cleanup-owner caps, zero-helper progress, and callback panic containment.
- `ssh_pump.rs` Facts propagation and healthy-close completion; `ssh_endpoint.rs:19-53` per-Route observers.
- `supervised.rs`, `cleanup_capacity.rs`, and directly affected route/worker support.

Recorded SHA-256 values for the seven production files and two principal A3 test files matched the blobs at the reviewed commit.

I independently ran the permitted Rust 1.95 locked/offline suite with an external target directory. All 73 executed tests passed; the one documented child-only test remained ignored and was exercised through its parent. Expected injected helper and connector panics were contained. The native signing/reuse, original deadline, no-timeout cancellation, failed-setup recovery, cleanup overrun, retained physical charge, factory-panic recovery, independent receipts, and cleanup-cap tests all passed.

The exact tuple was verified at both start and end. All member repositories remained clean at their stated commits. Root contained only the two explicitly excluded untracked A3 prompt files.

## 2. Invariant analysis

**Setup and physical ownership held.** `SetupConnector` converts the pool’s absolute logical deadline from the worker’s original `Instant`; it does not restart the budget. A `NativeResource` becomes idle only after its joined result matches the requested identity and both native and typed authentication states are true. Cancellation and forced disposal of a connecting job preserve supervisor ownership until join and destruction; neither path acknowledges a still-running helper.

**Ledger and shutdown ordering held.** Failed setup is disposed before `connected(Err)` releases capacity. Ready or active native owners remain in `PoolHost.entries`, whose destruction precedes driver loss. A connector panic settles the dequeued unowned connection before unwind. The outer worker boundary catches that unwind, closes admission, shuts down the pool, and transfers the whole unfinished host to the reserved cleanup owner. Cleanup errors remain sticky after eventual completion.

**Bounded retention held.** Endpoint creation reserves one of 64 cleanup-owner slots before worker creation, separately from the 64 setup-helper permits. Retention consumes that existing reservation. The common supervisor polls retained hosts even with no live helpers. A retained callback panic is caught and causes permanent fail-closed ownership rather than supervisor loss or a false completion. The cap test demonstrated refusal at 64, autonomous zero-helper progress, and permit recovery after actual destruction.

**Observation state held.** Pool allocation state supplies the authoritative fresh/reused classification. Native exchange state independently clears `credential_offered` after the first exchange, and the pump carries the same exchange Facts into healthy Close. Legacy injected resources retain default unknown Facts. Each `Route` owns its observer, so two routes sharing one endpoint receive distinct operation-scoped receipts. Native tests demonstrated one authentication and connection across two exchanges and two Git clones, with fresh then reused receipts and matching open/close Facts.

**Adverse transitions failed closed.** Exact-deadline completion cannot win after expiry; stalled signing remains cancellable with network timeout disabled; setup failure permits retry; cleanup overrun stops admission while retaining its physical charge; endpoint drop stays bounded; and late disposal changes only `cleanup_complete`, never the recorded failure.

## 3. Risks and next action

This verdict covers the local fixture-compiled A3 integration only. Platform and selected-source qualification, discovery/DNS/TCP/handshake and trust selection, explicit-key production setup, backend observation-sink attachment, and production network activation remain the stated later gates.

Accept this A3 checkpoint and require those deferred gates before any production activation claim.
