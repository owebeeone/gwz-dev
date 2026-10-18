# SSH N3a — Code-AXIS REVIEW

**Review object:** N3a local endpoint assembly at root `514f3cfeb233acd4e3f6c9c7e1bc07f17275373a`, core `dfe76d0fc4a440f04262d2e1a22e40542e051925`, transport `16a383e7d1c0e7e3234006688986afc2c6e54ca5`, and evidence `e6c9226bf204f9d96b4556878c272e6409636420`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshN3a.md`, status “implemented locally; review pending.”
**Baseline:** Core range `4b66a9b2..dfe76d0fc4a440f04262d2e1a22e40542e051925`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits using `git show`, `git diff`, and `git grep`.
**Date:** 2026-09-22
**Axis:** Code architecture, interfaces, call graphs, compatibility, and claim reality for the bounded N3a composition checkpoint. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

I inspected:

- `GwzRemoteTransportSshN3a.md:1-65`
- `GwzRemoteTransportSshProductionSetup.md:9-120`
- the Phase 3 and current-action portions of `GwzRemoteTransportPlan.md`
- `src/git/endpoint/ssh_local.rs:1-79`
- `src/git/endpoint/ssh_endpoint.rs:1-89`
- `tests/transport_ssh/tests/local_endpoint.rs:1-288`
- the proof-crate manifest and README
- evidence archive `campaigns/ssh-integration/runs/2026-09-22-local-endpoint-n3a`, including its source manifest, development failures, and final full-suite log.

The core delta contains 127 changed production lines across `ssh_local.rs` and `ssh_endpoint.rs`, within the 220-line bound. The 288-line focused test is within the 500-line test/support bound.

I ran the permitted exact-tree focused gate with Rust 1.95.0, locked/offline and an external target directory. All six executions passed:

- selected routes across clone, push, fetch, and advertisement;
- ambient reuse and selected refusal;
- selected reuse with current-file revalidation and no agent fallback;
- ambient refusal to reuse selected authority;
- trust refusal before agent I/O;
- retained queued-expiry regression.

The archived complete isolated SSH suite also passed. The evidence manifest identifies the reviewed core, transport, git2-rs, and libgit2 commits and hashes the relevant final source and test blobs.

Start and end checks matched all six required commits and trees. Only the prompt-authorized generated review files were untracked at root.

## 2. Invariant analysis

`ssh_local::connect` only captures owned configuration, known-host path, optional agent-socket path, and timeout values. Construction performs no trust, key, environment, agent, or network I/O. It creates no new resource owner or thread class; it composes the accepted `Endpoint`, registry, `SetupConnector`, supervised network/trust path, and existing authentication implementations.

The setup ordering holds. Each new connection enters `ssh_network::establish`, which performs trust validation, before either selected-key authentication or ambient agent-socket access. Explicit identities must resolve to a pinned admitted registry entry; setup never reopens their pathname. Ambient identity alone selects agent authentication. HTTPS identity refuses. Neither branch can fall through to the other.

`Route::local` stores immutable per-operation authority. A selected route always calls `open_selected`, so every open—including reuse—re-enters worker-side admission with the current pathname. Ambient routes call `open_observed` with `Identity::Ambient`. Repository path and observer are passed as operation data and do not enter pool compatibility keys. The retained resolver-based constructors preserve their prior internal behavior.

The focused counterexamples confirm authority isolation:

- an unauthorized selected key cannot consume an existing ambient session or retry the agent;
- a selected session cannot satisfy an ambient request when no agent exists;
- deleting a selected file prevents cached reuse and produces no agent traffic;
- empty trust refuses before agent requests.

The native Git test uses one selected physical connection for clone, push, fetch, and advertisement across two repositories while preserving two separate observation sinks. It creates a fresh native `Remote` when changing route context, matching the accepted git2-rs lifetime rule that disconnect retains the original transport.

Every focused case explicitly shuts down the endpoint and waits for truthful cleanup completion without sticky failure. Deadline, cancellation, pool ownership, admission, and physical cleanup remain inherited from accepted N1/N2b components.

Containment is accurate. `ssh_local.rs` is compiled by the isolated proof crate through an explicit test path and is not declared in the production core module graph. No Cargo dependency, public API, wire shape, backend driver, capability advertisement, or production route changed. This matches N3a’s fixture-attached scope rather than falsely activating SSH support.

## 3. Risks and next action

N3a observations cover successful opens only. Failure observations, backend-clone and nested-scope retention belong to N3b. Complete network-driver attachment belongs to N3c. The production git2 source switch, platform and selected-source qualification, HTTPS, and CLI-hosted placement remain explicitly deferred and are not implied by this GO.

Proceed to the bounded N3b observation and backend-lifetime checkpoint, retaining N3a’s immutable route authority and fresh-`Remote` rule.
