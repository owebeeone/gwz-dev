# SSH N2b selected admission/pool-worker integration remediation 1 — Code-axis re-verdict

**Review object:** N2b remediation 1 at root `9459ae4c2f5ad5061a2eaba92785a1f87bece938`, core `6616a2cd66d64f04f9eb3c370e8a3a677fba1c36`, transport `16a383e7d1c0e7e3234006688986afc2c6e54ca5`, and evidence `593d2c36780d6278eee21e67dc0cc102673ed887`.
**Baseline:** Original N2b review tuple: core `0f476cb9c67bce00e24afbdef6caee57fb6cd3dd`, transport `50bb6b1275b26f7c2a38493f06a1638d220a6c7a`, evidence `7cea42eb004ab439519dd86293852adf74e050cd`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.
**Date:** 2026-09-22
**Axis:** Focused Code re-verdict of the original P2-1 and P2-2 counterexamples and remediation-range interactions. Independent, adversarial, read-only. Nothing here relies on the parallel axis. Filed verbatim by the lane owner.

## Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-1 — absolute deadline omitted from interaction and ready states | **Closed** | The interaction clock now caps `until` at the request’s stored absolute deadline. `advance` expires `Ready` requests at that boundary, `next_deadline` exposes it, and `fail_request` closes the ready connection before returning the timeout. Direct tests keep interaction open through the boundary and expire an unclaimed ready lease. |
| P2-2 — asynchronous admission ownership and cleanup were not causally exercised | **Closed** | Barrier-controlled reader tests now exercise stalled expiry, retained snapshot charge, pending-admission reporting, admission-only and combined cleanup, late-result disposal, and active-stream progress. Refreshed evidence records and passes the exact corrected source blobs. |

**Verdict: GO** — both original P2 findings are closed. No new P0–P2 finding was introduced by the remediation.

---

## 0. Evidence base

I read the remediation plan, prior Code report, revised controlling checkpoint, and exact committed diffs:

- Transport `50bb6b12..16a383e7`: `src/pool/clock.rs` and `tests/network_timeouts.rs`
- Core `0f476cb9..6616a2cd`: `tests/transport_ssh/tests/selected_pool.rs`
- Evidence `7cea42eb..593d2c36`: `campaigns/ssh-integration/runs/2026-09-22-selected-key-n2b-rem1`

No production core worker, admission, setup, shutdown, snapshot, or authentication code changed in this round.

The refreshed archive records:

- five selected-pool tests passing, including both new barrier-controlled tests;
- the full isolated SSH suite passing;
- the complete standalone transport suite passing;
- source hashes matching the corrected committed `selected_pool.rs`, `clock.rs`, and `network_timeouts.rs` blobs.

I did not rebuild the object. The committed raw logs supplied the required execution evidence.

Start and end verification matched all six required SHAs and trees:

- root tree `8482f42800a3728dffd64a03df4723d747a3c2ed`
- core tree `f5a524e2e3b9b9a385e1f1a6a9beab282871ba21`
- evidence tree `490299248885636e73bbad3d43f93400239bd429`
- transport tree `b47dba6f28a464123f402b6fddbb8ba7083b09cd`
- git2-rs tree `28fc6ad905653db47a7705706e1edea0f39d7054`
- libgit2 tree `af52de522aacf3593250c45159a038801153c6b5`

Only the generated review prompts were untracked at root.

## 1. Findings

No open findings.

## 2. Invariant analysis

P2-1’s two state sequences now fail as required. Entering helper interaction retains the request’s absolute boundary by taking the minimum of the interaction allowance and absolute deadline. At that boundary, `advance` returns `InteractionTimeout` and schedules `CancelConnect`. A connection that becomes `Ready` before the boundary is also still governed by the absolute deadline: `advance` marks the request failed, `fail_request` starts physical closing, and `take` returns `AllocationTimeout` without handing out the lease. Requests using the legacy API retain their prior behavior because ready-state expiry applies only when `absolute_deadline` exists.

For P2-2, the injected reader reserves snapshot capacity before blocking. The expiry test observes a timed-out caller, zero native setup calls, one retained registry charge, one pending admission, and false cleanup completion. Only after releasing and joining the helper do pending admissions and registry usage reach zero. Inspection of `Admissions::poll` confirms that expired, cancelled, or reply-less requests stay on the disposal path and cannot intern a late result or return to checkout.

The second test establishes a real selected SSH stream, stalls a subsequent admission, and completes the existing stream while the admission remains blocked. Shutdown then covers an owned pool resource and the stalled admission together and reaches completion only after the admission is released.

The unchanged `OpenRequest.deadline` passes from admission into `pool.checkout_until`; it is never recomputed after file admission. The stalled-expiry test establishes admission-side consumption, while the corrected transport boundary tests establish exact interaction and ready-state expiry.

The remediation is confined to the original defects: 23 production-line changes in the transport clock and focused tests/evidence. It does not activate routing, alter credential policy, reopen accepted N2a, or absorb deferred N3/platform/source/HTTPS scope.

## 3. Risks and next action

This verdict accepts the N2b fixture-attached checkpoint only. Production backend attachment, network entry points, HTTPS, platform qualification, and selected-source qualification remain deferred as documented.

The next action may proceed to the separately reviewed N3 production attachment gate.
