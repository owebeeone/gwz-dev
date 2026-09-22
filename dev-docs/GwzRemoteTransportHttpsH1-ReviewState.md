# GWZ Remote Transport HTTPS H1 — State Review

**Date:** 2026-09-22  
**Verdict:** **NO-GO**  
**Findings:** 4 × P2; no P0/P1 findings.

## Review object

The immutable tuple matched at both review boundaries:

- root: `ead57a6353daa145d7372231c22e9d058f4cf085`
- core: `280f970a66047942b006cfecfae51db60fa0550b`
- transport: `aa40936d0805e8cb60f8027615abe20d4f2045e4`
- taut: `bcf98b64d465fc54841121b6d1a2d46940f81a3c`
- CLI: `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`
- Python: `d07d55dacb1725d9306be9c04d157ac29a78e000`
- git2-rs: `ce78628308e11b4e8901d5061602619109bce21a`
- libgit2: `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`
- evidence: `a4088b50f6322fb6c8985f3559fb45486d29a873`

Member trees were clean. Root noise was limited to the permitted generated prompts. I did not inspect the current Code-axis prompt or report.

I reviewed the H1 source ranges, checkpoint documents, accepted HTTPS design and requirements, and evidence manifest. All 49 recorded hashes across 21 manifest entries matched the committed artifacts. Recorded evidence reports 45 focused endpoint tests, the full transport suite, the core check, formatting, and archive checks passing. Per instruction, I did not rebuild or rerun those gates.

## Findings

### P2-1 — Endpoint shutdown does not own or close active credential work

**Locations:** `gwz-core/src/git/endpoint/https_worker.rs:43-80`, `gwz-core/src/git/endpoint/https_auth.rs:117-215`, `gwz-core/src/git/endpoint/https_auth.rs:249-298`

`Endpoint::shutdown` reaps the process-global retained-child registry and shuts down the pool. The endpoint has no closed-admission state or registry of active `prepare` operations. A credential helper belongs only to its lookup future and enters the retained registry only after cancellation or timeout when immediate termination fails.

Concrete sequence:

1. A cloned client begins `prepare` using a helper that blocks.
2. After the helper starts, endpoint shutdown runs.
3. The child is still active rather than retained, and no pool resource exists yet.
4. Shutdown may report zero pending work and return while the helper and request remain live.
5. A retained client clone can also initiate another helper lookup after shutdown; rejection occurs only later at pool checkout.

Because the retained registry is global, shutdown of endpoint A may additionally reap or count endpoint B’s retained child while A’s own active child remains invisible.

This violates the cleanup contract requiring endpoint shutdown to account for HTTP tasks, helper children, and noninterruptible jobs. It permits post-shutdown credential access and false clean-shutdown reporting.

**Required correction:** Add endpoint-scoped lifecycle state that atomically closes admission, registers and cancels active preparations before spawning helper work, and waits for or reports every active and retained job together with pool shutdown. Helper ownership must be endpoint-scoped or explicitly ownership-tagged.

**Regression:** Hold a helper behind a barrier, start shutdown, and prove shutdown either reaps it or reports it pending; reject a post-shutdown preparation before helper execution; then converge to zero. A two-endpoint test must prove cleanup attribution remains isolated.

### P2-2 — Dropping one local remote clears operation-wide redirect state

**Locations:** `gwz-core/src/git/endpoint/https_local.rs:151-155`, `gwz-core/src/git/endpoint/https_worker.rs:101-105`, `gwz-core/src/git/endpoint/https_policy.rs:112-114`

`LocalRpc::drop` cancels that remote and immediately calls `finish_operation`, which removes every route entry for the operation. There is no operation owner, dependent-stream count, generation guard, or drain before removal.

Concrete sequence:

1. Remotes A and B share an operation, original destination, and service family; discovery pins route RA.
2. A’s `LocalRpc` drops while B remains live.
3. A clears the operation-wide route.
4. Remote C in the same operation can now pin RB.
5. B’s later receive-pack exchange resolves RB, sending it to a different redirect target, or fails because its route vanished.

This directly contradicts the accepted rule that stream cancellation or closure must not erase a route while dependent streams remain and that only whole-operation retirement may clear it.

**Required correction:** Give the route table an operation-scoped owner or generation/refcount. Dropping one `LocalRpc` may cancel its own work but must not retire shared route state. Retirement must occur only after the operation is sealed and all dependent work is drained.

**Regression:** Use actual `LocalRpc` instances with barriers for A, B, and C. After A drops, prove B retains RA and C cannot repin the operation to RB.

### P2-3 — The required pre-Opened failure path is synthetic rather than composed through H1

**Locations:** `gwz-core/src/git/endpoint/https_local.rs:106-124`, `gwz-transport/tests/https_reuse.rs:140-175`

The local H1 adapter calls `Client::prepare` directly. On failure it sends an `io::Error` through a local channel before constructing a stream. No production-local mapping creates a mux `OpenFailed` message.

The cited “real mux” test instead manually fabricates `Open` and `OpenFailed`; it does not feed an actual HTTPS worker failure through the mapper. It covers selected HTTP statuses but not the complete required classes: malformed headers, trust failure, network loss, and exhausted budget.

Therefore the candidate does not yet demonstrate the accepted opening-state contract that every discovery failure before `Opened` reaches the initiator as `OpenFailed` with available facts. A synthetic envelope test cannot detect a missing or incorrect worker-to-mux mapping.

**Required correction:** Add the bounded H1 composition seam that maps actual worker failures into `Failure` and sends them through a real mux `Open`/`OpenFailed` exchange. This does not require the deferred all-command H2 integration.

**Regression:** Drive each required failure class through the actual H1 mapper and mux, including first anonymous receipt, and prove exactly one `OpenFailed`, with no `Opened` or stream delivery.

### P2-4 — Redirect processing discards earlier credential-offer facts

**Location:** `gwz-core/src/git/endpoint/https_worker.rs:218-364`

Each redirect iteration creates fresh failure facts. Facts indicating that credentials were offered are discarded when a redirect is followed.

Concrete sequence:

1. Explicit `Gh` discovery sends an Authorization header to origin A.
2. A returns an accepted redirect to B.
3. B’s helper lookup or connection fails before its request.
4. The terminal failure carries default or absent facts, concealing that credentials were already offered during this logical discovery.

This defeats the requirement that credential offering and authentication remain distinct observable facts and makes terminal failure accounting incomplete.

**Required correction:** Maintain cumulative facts for the logical discovery across redirects. `credential_offered` should be a monotonic OR over attempts; final-status and authentication semantics should be documented and attached consistently to `Failure`/`OpenFailed`.

**Regression:** Prove A receives a sentinel Authorization value, force B’s helper or connection failure, and require terminal `OpenFailed` to report `credential_offered=true` without exposing the secret or raw URL.

## State properties that held

The inspected implementation correctly places the POST effect at `Possible` before transmission and does not retry POST, disposes the failed authenticated lease before retrying, bounds helper output and pipes, directly executes the helper with a cleared environment, separates proxy and origin authorization, verifies TLS, enforces exclusive leases, and permits reuse only after body EOF and sender readiness. Response parsing, informational-response limits, truncated-body handling, credit backpressure, and physical-byte accounting were consistent with the accepted H1 boundaries. Connector setup jobs and physical reservations use fail-closed retention rather than optimistic capacity release.

H2 host embedding, higher-level authority injection, Placement C P3, platform/source qualification, real GitHub parity, public construction, physical wire work, and release qualification remain explicitly deferred and are not findings here.

The four P2 defects require one bounded lifecycle/composition correction followed by focused State re-review. This verdict does not certify implementation acceptance or any deferred gate.
