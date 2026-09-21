# SSH N3 aggregate backend attachment — State-AXIS REVIEW

**Review object:** N3 remediation 1, core range `2f12bbd63f748d836cd3ff858c87603e745788c9..c79c7f13aebfcf582d0df75cff469d452e3477f1`, controlled by `gwz-core/dev-docs/GwzRemoteTransportSshN3.md`; candidate implementation, production activation still deferred.  
**Baseline:** root `7f0a844b1bb851eedd3792eb13c0194b2231a190`; core `c79c7f13aebfcf582d0df75cff469d452e3477f1`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; evidence `36d29397faae5205e1e16812f9f573a122665b7f`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** Durable-state semantics under failure, cancellation, concurrency and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — all three prior P2 findings are closed; no new P0–P3 finding was found in the consolidated changed range.

---

## Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| State P2-1 — transient endpoint construction permanently poisoned the backend family | **Closed** | `transport_binding.rs:19-64` now stores only `Option<Endpoint>` behind the family-shared mutex. A failed factory call returns without publishing state; successful construction is serialized and published once. The regression fails first with `WouldBlock`, races three family/scope callers, observes exactly one successful construction, then proves real pooled reuse and separate observation scopes. |
| Code P2-1 — SSH service refusal was lost before private-member classification | **Closed** | `ssh_pump.rs:294-318,334-357`, `ssh_worker.rs:610-628`, and `stream_io.rs:17-66` carry one stream-local boolean only after complete, untruncated canonical stderr and empty stdout. Noncanonical, truncated and output-bearing cases remain generic. Candidate tests prove `RemoteRejected`, generic nonzero failure, quiet private-member omission and observation removal. |
| Code P2-2 — per-key rejection facts overrode timeout/cancellation | **Closed** | `ssh_endpoint.rs:71-98` now requires both an explicit rejected observation and a returned pool `ConnectFailed(Authentication)` cause. The deterministic two-key deadline test retains Net/timeout after the first refusal; exhaustion remains Auth/Ssh and the retained later-key-success path still passes. |

## Changed-range analysis

The runtime correction removes the durable poisoned state. Construction is serialized only while creating the endpoint owner; it performs no network, trust or credential I/O. Factory errors leave the slot empty, while successful construction is shared through the existing `Arc<RuntimeState>` across backend and `with_transport` clones. Poisoned mutex recovery retains the protected state rather than fabricating success. No additional worker, socket or cleanup owner was introduced.

Authentication classification now treats facts as observations rather than terminal authority. Timeout, cancellation, agent failure and transport failure retain their pool cause even if an earlier key was rejected. Explicit exhaustion still requires the matching authentication failure plus rejected facts, preventing unknown or local failures from becoming authentication denial.

The refusal receipt has bounded state and ownership. It contains no remote text and is scoped to one Git stream. It is set only after stdout and stderr EOF, with no stdout and no truncation; the fixed local marker is the only value recognized at the candidate clone boundary. Pump failure still discards the physical resource, so the receipt cannot manufacture cleanup, reuse or successful exit. Each reused physical connection receives a fresh receipt for its new stream.

Transport, fork and wire objects are unchanged. Candidate cfg containment and deferred source/platform activation boundaries remain intact. The aggregate source counts remain within the accepted production and test budgets.

## 0. Evidence base

I read the merged remediation plan, both original reports, the revised N3 contract, the entire core remediation diff, all changed production and regression files, and private campaign `2026-09-22-backend-n3-rem1`. The evidence manifest pins the reviewed core, transport, fork and C sources; its earlier root SHA precedes only the evidence-archive lock/marker commit.

I reran the permitted focused gates:

- candidate backend and driver module: **7 passed, 0 failed**;
- `local_endpoint`: **10 passed, 0 failed**;
- `pump`: **10 passed, 0 failed**;
- `cleanup_capacity`: **1 passed, 0 failed**.

Archived final evidence records backend 7, ordinary backend 8 and isolated SSH 126 passing with one intentionally ignored extended test. The unchanged prior transport result is 94 passing with two ignored campaigns. No whole-core, platform, selected-source or production-activation claim is made.

The exact six-repository tuple was verified unchanged at both review boundaries. Reviewed member trees were clean; root contained only the prompt artifacts explicitly excluded by the review instructions.

## 2. Invariant analysis

The original cleanup-capacity sequence now recovers: transient failure publishes no endpoint state, released capacity permits retry, and concurrent family callers converge on one endpoint. Successful sharing does not merge operation observations.

Failure precedence also held. Per-key refusal cannot replace a later timeout, while genuine exhaustion remains authentication rejection. Complete canonical repository refusal reaches the Git boundary without carrying unbounded diagnostics, and generic or partial terminal states remain generic failures. Physical disposal remains controlled solely by the pump, worker and pool lifecycle.

No changed-range race exposed a reusable resource before completed service cleanup, leaked a receipt across streams, duplicated an endpoint, or allowed late facts to mutate an already copied response.

## 3. Risks and next action

Platform, selected-source, publication and production-activation qualification remain explicitly deferred. The fixed refusal mapping is local to the candidate adapter; future CLI-hosted placement still needs its separately admitted terminal-disposition mapping.

This remediation requires no further N3 correction. Proceed to the next explicitly deferred qualification or activation gate using the settled tuple.
