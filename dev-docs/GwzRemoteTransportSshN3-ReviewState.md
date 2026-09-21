# SSH N3 aggregate backend attachment — State-AXIS REVIEW

**Review object:** Aggregate N3 backend attachment at core `2f12bbd63f748d836cd3ff858c87603e745788c9`, transport `a6562e654b52705b72ef1f793ae2045c320cee47`, controlled by `gwz-core/dev-docs/GwzRemoteTransportSshN3.md`; candidate implementation, production activation still deferred; 2026-09-22.  
**Baseline:** root `2e5c06c38bf161a852afefc7c0307226b6862212`; core baseline `6ea83ddbad4a5ae9ffaf555732704735c05bc1b8`, reviewed through `2f12bbd63f748d836cd3ff858c87603e745788c9`; transport baseline `16a383e7d1c0e7e3234006688986afc2c6e54ca5`, reviewed through `a6562e654b52705b72ef1f793ae2045c320cee47`; evidence `3dad43c66544486786a866d6f5b1bc6f5f788077`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Durable-state semantics under failure, cancellation, concurrency, restart and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 recovery finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the controlling N3 draft in full and traced the complete changed production range, including:

- `src/git/gitbackend/transport_binding.rs`, `transport.rs`, `transport_support.rs`, `transport_observations.rs`, backend construction and `with_transport`;
- `src/git/endpoint/ssh_endpoint.rs`, `ssh_local.rs`, `ssh_pool.rs`, `ssh_setup.rs`, `ssh_worker.rs`, key/agent reporting and remote error classification;
- `gwz-transport/src/pool/asynchronous.rs`;
- candidate backend and command-driver tests, SSH correlation tests, external candidate harness and cfg boundary;
- private campaign README, source manifest and final logs under `2026-09-22-backend-n3`.

I ran the permitted focused candidate gate: 5 passed, 0 failed. I also ran `pool_async`: 7 passed, 0 failed. Archived evidence records the ordinary backend gate, isolated SSH suite and transport suite, with its stated ignored campaigns and full-core limitation. Evidence source pins match the reviewed core and transport commits; its earlier root SHA differs only because the final root commit archives that evidence through workspace lock/marker metadata.

The exact six-repository tuple was verified unchanged at both review boundaries. Reviewed members were clean; only the prompt artifacts listed as excluded by the canonical prompt were untracked at root.

## 1. Findings

### [P2-1] A transient endpoint-construction failure permanently poisons the backend family

**Location:** `gwz-core/src/git/gitbackend/transport_binding.rs:14-25`; trigger path in `src/git/endpoint/ssh_worker.rs:179` and `src/git/endpoint/agent_job.rs:12,314-322`; propagation through `src/git/gitbackend.rs:200-215`.

`Runtime` stores `Result<Endpoint, io::ErrorKind>` inside a shared `OnceLock`. Therefore the first initialization error is published as permanent state, even though endpoint construction has explicitly transient failure modes.

A concrete sequence is:

1. Sixty-four other endpoints hold the process-wide cleanup reservations.
2. The first SSH operation on a fresh backend reaches `Cleanup::reserve()` and receives `WouldBlock`.
3. `Runtime::endpoint()` stores that error in its `OnceLock`.
4. One existing endpoint completes cleanup, releasing a reservation; the existing cleanup-capacity regression establishes this transition.
5. A later SSH operation on the same backend, an operation-scoped `with_transport` backend, or any family clone never retries construction. It reads the cached error and returns the generic “SSH endpoint unavailable” failure forever.
6. Recovery requires replacing the whole backend family or restarting the process, despite the resource condition having cleared.

This violates the lazy endpoint’s recovery invariant: a failed attempt that created no endpoint must not invent durable failed-endpoint state. It can strand all subsequent SSH operations on a long-lived backend after momentary process-wide pressure.

Publish only successful endpoint construction. Serialize concurrent initialization, recheck after acquiring that serialization, and leave the success cell empty when construction returns an error. Preserve the initiating caller’s actual failure while allowing a later operation to retry; successful construction must still be shared across all backend-family and operation-scope clones.

Closure requires an injected-constructor regression that fails the first initialization with `WouldBlock`, then succeeds after the simulated reservation is released. It must prove that a later operation recovers, concurrent initialization does not publish multiple successful endpoints, and `with_transport` clones share the recovered endpoint while retaining isolated observation rows.

## 2. Invariant analysis

The remaining attacks held:

- Backend-family clones share one successful endpoint, while `with_transport` creates fresh identity and observation scopes. Nested driver responses merge inner rows once; the focused candidate tests exercised reuse and non-duplication.
- Clone, fetch, tag fetch, advertisement, remote-file discovery and push all enter the common callback assembly. Existing transfer-progress and push negotiation/rejection callbacks remain installed, and each changed context creates a fresh native `RemoteTransport`.
- Non-SSH and local-family URLs retain native routing. Recognized malformed SSH forms fail closed rather than falling back.
- `Checkout::opening_connection()` exposes only an ID during `Opening`; waiting, ready and consumed states returned `None` in the focused test. It grants no lease or resource access.
- Per-request progress cells are associated before setup starts. Credential rejection is recorded only for explicit native authentication refusal; trust, local signing and queue failures do not fabricate rejection. Joined live success overwrites setup progress, reuse suppresses a new-offer claim, and response publication copies facts so late helper writes cannot mutate an already published row.
- Candidate code remains inside the explicit Unix plus `gwz_transport_candidate` boundary. The harness builds externally and does not change production dependency selection or activate routing.
- Added production and test source remain within the documented bounds.

## 3. Risks and next action

Platform, selected-source, publication and production-activation qualification remain explicitly deferred and are not findings here.

The next action is to replace failure-caching endpoint initialization with success-only publication, add the retry/concurrency regression described in P2-1, then rerun the focused candidate backend gate and the relevant endpoint cleanup-capacity test.
