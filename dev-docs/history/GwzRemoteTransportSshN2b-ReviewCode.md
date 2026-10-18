# SSH N2b selected admission/pool-worker integration — Code-axis review

**Review object:** N2b implementation checkpoint at root `3d411d8c84a93839cd04ddc4262e37c31c27e53b`, core `0f476cb9c67bce00e24afbdef6caee57fb6cd3dd`, transport `50bb6b1275b26f7c2a38493f06a1638d220a6c7a`, and evidence `7cea42eb004ab439519dd86293852adf74e050cd`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshN2b.md`.
**Baseline:** git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show` and `git diff`.
**Date:** 2026-09-22
**Axis:** Code architecture, interfaces, call graphs, compatibility, and evidence reality. Independent, adversarial, read-only. Nothing here relies on the parallel axis. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

I inspected the committed N2b document, accepted selected-identity design §§3–5, production-setup requirements, core changes from accepted N2a, transport changes from its accepted baseline, and the complete committed evidence archive.

The principal inspected implementation paths were:

- `gwz-core/src/git/endpoint/ssh_admission.rs`
- `ssh_worker.rs`, `ssh_setup.rs`, `ssh_shutdown.rs`
- `ssh_key_snapshot.rs`, `ssh_key_auth.rs`
- `tests/transport_ssh/tests/selected_pool.rs`
- `gwz-transport/src/pool/{allocation,asynchronous,clock,lifecycle,machine}.rs`
- `gwz-transport/tests/network_timeouts.rs`
- evidence run `campaigns/ssh-integration/runs/2026-09-21-selected-key-n2b`

The permitted exact-tree focused runs passed:

- `selected_pool`: 3/3
- `network_timeouts`: 9/9

Start and end commit/tree checks retained the six pinned objects: root tree `dc67aa0…`, core `4c9ce4d…`, evidence `655f718…`, transport `0cc9de3…`, git2-rs `28fc6ad…`, and libgit2 `af52de5…`. Owner-authored remediation edits appeared in the transport working tree after substantive review; the owner explicitly excluded them and confirmed immutable committed blobs remained the review object. No such edits informed this verdict.

## 1. Findings

### [P2-1] The absolute request deadline is not enforced across interaction and ready states

**Location:** `gwz-transport/src/pool/clock.rs:13-45,78-112,116-140`; `src/pool/machine.rs:114-182`; `tests/network_timeouts.rs:28-66`.

The accepted invariant is one unchanged absolute deadline through admission, queueing, checkout, connection, interaction, and handoff. `request_until` stores that deadline, but expiry is state-selective.

First counterexample:

1. Submit `request_until(..., Some(25))`.
2. Start connection setup and call `begin_interaction` before time 25.
3. `begin_interaction` replaces the absolute-capped network clock with `Interaction { until: now + interaction_ms, … }`.
4. Leave the interaction active and advance to 25.

`advance` tests only the interaction clock’s later `until`; `next_deadline` likewise reports that later time. The request therefore survives its absolute deadline.

Second counterexample:

1. Connect successfully before time 25, making the request `Ready`.
2. Advance to 25 or later.
3. Call `take`.

`advance` ignores `RequestState::Ready`, and `take` performs no deadline check, so it returns a lease after the request deadline. This violates bounded checkout/handoff and permits a late resource effect after caller expiry. The existing interaction test ends the interaction at time 5 and therefore exercises only the resume path.

**Required correction:** make the stored absolute deadline participate in every live request state, including active interaction and `Ready`. `next_deadline`, `advance`, interaction construction, and `take` must agree on the same boundary and preserve the appropriate phase-specific error and disposal action.

**Closure tests:** keep an interaction active through the absolute boundary and assert expiry plus connect cancellation at exactly that boundary; separately make a request ready before the boundary, advance to it, and prove `take` returns timeout while the connection is scheduled for disposal and no lease is handed out.

### [P2-2] The evidence does not exercise the new asynchronous admission ownership and cleanup contract

**Location:** `GwzRemoteTransportSshSelectedIdentityDesign.md:184-206,249-289`; `GwzRemoteTransportSshN2b.md:22-37`; `ssh_admission.rs:50-132`; `ssh_worker.rs:397-499`; `tests/transport_ssh/tests/selected_pool.rs:44-71`.

The N2b gate introduces an asynchronous `Admissions` owner and specifically requires proof for cancellation/expiry during a stalled read, late-result suppression, admission-only and combined retained cleanup, shared helper exhaustion, unchanged deadline consumption, and active-stream progress while admission is stalled.

The selected-pool suite contains only:

- concurrent successful admission/authentication and reuse;
- alternate-path same-byte reuse plus changed/deleted-file refusal.

It does not inject a stalled or late admission `Job`. The referenced queue-expiry coverage is an ambient worker path and never enters `Admissions`. Consequently, none of the new admission disposal, retained-owner, deadline-consumption, or fairness interleavings is causally observed. Passing normal filesystem reads cannot establish these properties.

This leaves the checkpoint’s central new ownership boundary unqualified: a regression that drops the permit/snapshot charge early, publishes a late result, reports cleanup complete while a reader remains live, or starves an active stream would pass the submitted gate.

**Required correction:** use the existing injectable reader seam to add deterministic barrier-controlled admissions. At minimum prove:

- cancellation and expiry while a read is stalled produce no checkout/auth/stream effect, while ownership remains charged until the job joins;
- admission-only and combined pool/admission shutdown retain ownership and report `pending_admissions` truthfully until physical completion;
- a stalled admission does not stop an existing stream;
- admission consumes the original deadline, and checkout/setup receive only the remaining time, including exact/past expiry.

Refresh committed evidence from the corrected exact source tuple.

## 2. Invariant analysis

Several attacks failed. Selected requests branch into admission before pool checkout. Joined live admission replaces the pathname with an opaque identity and strong `Entry` pin; setup resolves the registry entry without reopening the path. The resource retains authority through idle, leased, reclaim, and disposal states, and promotion occurs only after joined successful setup. The capacity-one fan-out test demonstrates one physical authentication for six identical requests, one credential offer, alternate-path same-byte reuse, and refusal after file change or deletion.

Worker work is bounded to 32 admission items and 32 incoming requests per tick. `PoolHost` and `Admissions` reside outside the unwind boundary and are transferred together to retained cleanup. The legacy transport API still delegates to the new API with no absolute deadline. The production delta stays within the accepted 500-line limit.

Those properties do not cure the deadline state omissions or substitute for causal admission-cleanup evidence.

## 3. Risks and next action

Production routing, backend activation, platforms, selected-source qualification, HTTPS, and the operator-deferred batch remain outside this review and are not findings.

The next action is one bounded correction: enforce the absolute deadline in interaction and ready states, then add deterministic injected-admission lifecycle tests and refresh exact-tuple evidence. A focused re-verdict can retrace the two counterexamples without reopening accepted N2a or deferred N3 work.
