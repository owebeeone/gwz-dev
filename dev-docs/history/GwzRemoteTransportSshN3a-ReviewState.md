# SSH N3a — State-AXIS REVIEW

**Review object:** N3a local endpoint assembly and per-operation routes, core range `4b66a9b2dbb64ec8ae2c926f9289b4545f7145f0..dfe76d0fc4a440f04262d2e1a22e40542e051925`, controlled by `gwz-core/dev-docs/GwzRemoteTransportSshN3a.md`; status implemented locally and under review on 2026-09-22.  
**Baseline:** root `514f3cfeb233acd4e3f6c9c7e1bc07f17275373a`; core `dfe76d0fc4a440f04262d2e1a22e40542e051925`; transport `16a383e7d1c0e7e3234006688986afc2c6e54ca5`; evidence `e6c9226bf204f9d96b4556878c272e6409636420`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects. Start and end tuple checks matched exactly; reviewed members remained clean and only the declared generated root prompts were untracked.  
**Date:** 2026-09-22  
**Axis:** State-machine isolation, authority lifetime, cancellation, cleanup, per-remote context, and fail-closed recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I inspected the complete N3a source delta, principally:

- `ssh_local.rs:18-77`, which assembles the accepted registry, setup connector, supervised network/trust path, selected-key authentication, ambient agent authentication, and shared endpoint.
- `ssh_endpoint.rs:19-88`, including immutable `Ambient`/`Selected`/resolved route authority and success observation.
- `ssh_remote.rs:13-99` and the pinned git2-rs `RemoteCallbacks::smart_transport` lifetime contract.
- `local_endpoint.rs:60-286`, covering native clone, push, fetch and advertisement, authority separation, current-file admission, trust ordering, observer isolation and cleanup.
- The N3a control document, production-setup and selected-identity authority, worker call-site map, source-switch gate, evidence README, source hashes and final logs.

The delta adds 120 production lines net across the two bounded source files and 288 focused test lines, within N3a’s 220/500 ceilings. No production manifest, dependency source, wire type, public surface, or resource-owner class changed. `git diff --check` passed.

I ran the permitted focused gate with the external target directory. All six executions passed in 0.75 seconds. The committed archive independently records the full isolated SSH suite passing and its hashes for the exact core, transport, git2-rs and libgit2 sources. The three reviewed changed-file hashes match the archive.

## 2. Invariant analysis

Construction is side-effect-free with respect to trust, credentials and network access. `ssh_local::connect` stores owned paths and creates the already accepted worker/registry/connector owners; filesystem, DNS, TCP, trust and credential work starts only inside the supervised setup Job.

Route authority is immutable. A selected route always calls `open_selected`, which rereads and validates the current file before pool lookup, including reuse. Its connector can obtain only the admitted opaque registry entry and invokes in-memory selected authentication; no branch reaches the agent. Ambient routes use `Identity::Ambient`, never acquire selected registry authority, and require the supplied agent socket for a fresh connection. Pool identity therefore prevents either authority class from leasing the other’s session.

Trust ordering held. Both branches first call `ssh_network::establish`; ambient authentication opens the agent socket only after host trust succeeds, and selected authentication rechecks the approved host-key bytes before offering the snapshot. Missing or changed selected files fail before checkout and cannot consume a cached selected session. An untrusted host produced no agent traffic.

The composition inherits the accepted endpoint deadline, request permit, setup Job, pool resource and retained-cleanup owners. It adds no independent thread, socket or connection lifetime. Every tested endpoint reached `cleanup_complete` without a sticky failure.

The per-remote context attack also failed. The pinned binding documents that disconnect may retain the original smart transport; changing route or observer therefore requires a fresh native `Remote`. The native test does this explicitly while retaining the shared endpoint. Clone, push, fetch and advertisement used one selected authenticated connection across two repositories, while observations remained in their assigned sinks and reuse did not claim a new credential offer.

The implementation and evidence make no claim that backend drivers are attached or that the production dependency has switched. The local fixture composes the isolated source directly against the pinned fork, matching the stated N3a boundary.

## 3. Risks and next action

Failure observations, backend clone/nested-scope propagation, the full network-driver map, production dependency activation, platform/source qualification, HTTPS and CLI-hosted placement remain explicitly deferred.

Proceed to N3b’s bounded observation and backend-context package. Preserve the fresh-`Remote` rule when route or observer context changes, then retain N3c and activation as separate gates.
