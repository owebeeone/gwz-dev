# SSH Agent A3 — Code-Axis Review

**Review object:** A3 supervised physical-pool/shared-worker integration at gwz-core `e92c5d1ec09dd64a397dbacf6c78888955e4ae12`; controlling draft `dev-docs/GwzRemoteTransportSshAgentA3.md`; implementation checkpoint dated 2026-09-21  
**Baseline:** root `fe68d36f939de8cba2bc8a509f85a24cefe85db5`; gwz-core-evidence `dd5b5f144c4db29968d578655ca279a7f74c1119`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from the checked-out immutable commits and cross-checked with `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** Code — architecture, ownership, interfaces, call graphs, recovery paths, and compatibility reality. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings. The reviewed code implements the bounded A3 local integration it claims without asserting production activation or deferred platform/source qualification.

---

## 0. Evidence base

I inspected:

- `ssh_setup.rs:22-311`: setup factory and Job transfer, authenticated/identity validation, shared-origin deadline conversion, connecting/idle/active/disposed transitions, reclaim, observation facts, and disposal.
- `ssh_pool.rs:39-264`: entry ownership, reuse marking, deadline-before-completion ordering, physical destruction before ledger acknowledgment, start-unwind settlement, shutdown completeness, and emergency Drop.
- `ssh_worker.rs:52-509`: endpoint construction, pre-reserved cleanup ownership, request admission, shared worker loop, stop/error transitions, pending and active teardown, attachment, receipt construction, transfer, and release.
- `ssh_shutdown.rs:14-88` and `agent_job.rs:125-169,300-358`: sticky status, whole-host retention, supervisor polling, independent cleanup permits, zero-helper progress, and panic containment.
- `ssh_pump.rs:87-126,197-227,342-355` and `ssh_endpoint.rs:12-55`: close facts, reclaim eligibility, and operation-scoped Route observation.
- Focused tests in `supervised.rs`, `cleanup_capacity.rs`, and directly interacting pool, worker, route, pump, channel, and A1/A2 tests.
- The A3 evidence README, `inputs.json`, final full-gate log, panic-regression log, source hashes, current program checkpoint, accepted helper design, and accepted A1/A2 boundaries. Recorded source hashes match the reviewed files. The archived Rust 1.95 locked/offline gate records all focused suites passing; I did not repeat that unchanged campaign.
- Relevant gwz-transport pool driver/machine behavior to verify that dequeued Connect settlement, Lease Drop, shutdown, and physical closure acknowledgments have the assumed semantics.

The tuple was verified unchanged at both boundaries. Final trees were root `e8b70f0d93fa2ad8746514ad3ed78406e2b557e2`, core `432541438f137491f08161715d58e7d4b3af7724`, evidence `eb389d121a993d3f649b03c57b1fc16bc7c3e6bb`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Members were clean; only the two allowed owner-generated root A3 prompts were untracked.

## 2. Invariant analysis

**Setup ownership and time held.** `SetupConnector` converts the pool’s absolute logical deadline through the worker’s original `Instant`; it does not restart the budget. Job success is consumable only after supervisor join. Promotion requires exact requested identity, a native authenticated session, and `authenticated == Some(true)`. Cancellation or failure remains connecting/disposal work until the Job reports joined destruction; capacity is not acknowledged early.

**Physical state and ledger ordering held.** Native ownership moves exclusively through connecting, idle, active, and disposed states. Healthy channel completion alone can recover the session. Failed exchange, cancellation, or cleanup invalidation schedules discard. `PoolHost` removes the resource only after `poll_dispose` returns success, then acknowledges `connected(Err)` or `closed`. A connector-start panic first settles the already-dequeued Connect and then unwinds; the worker-level catch still owns all earlier entries.

**Shutdown and recovery held.** Cleanup errors set sticky failure and stop admission. Normal stop disconnects callers, shuts down the pool, and polls within the configured cleanup budget. Overrun or worker panic transfers the complete host, including driver and charged entries, into the reserved supervisor record. Status keeps the physical count visible; eventual completion sets `cleanup_complete` without erasing the earlier failure. Emergency Drop cancels supervised jobs and destroys ready/active native owners without claiming reuse.

**Bounded supervision held.** Each endpoint reserves one of 64 cleanup records before worker creation; retention consumes that same permit. This counter is independent of the 64 setup-helper permits. Healthy exit, constructor failure, spawn failure, and completed retention release the permit. The common reaper now bases sleep on its actual entry list, so retained hosts continue progressing with zero live setup helpers. A retained callback panic is caught and conservatively keeps ownership and its permit rather than falsely acknowledging disposal.

**Observation semantics held.** First native exchange receipts carry the authenticated setup facts. Pool reuse is tracked on the physical entry, clears `credential_offered`, retains authentication proof, and preserves the connection ID. The same per-exchange facts are installed in the pump’s healthy Close. Legacy injected resources retain default/unknown facts. Each Route owns its observer, so independent Routes sharing an Endpoint receive only their operation’s `Opened` receipt.

## 3. Risks and next action

The reviewed modules remain fixture-compiled and inactive. DNS/TCP discovery, handshake and trust selection, explicit-key production setup, platform/selected-source qualification, backend observation-sink attachment, and all-network-entry activation remain explicit later gates. Retained cleanup polling also continues to rely on the documented bounded, non-panicking resource contract; violation fails conservatively by retaining ownership.

The next action is to complete the independent aggregate gate and, if accepted, proceed to the separately scoped production setup/activation and deferred platform/source work.
