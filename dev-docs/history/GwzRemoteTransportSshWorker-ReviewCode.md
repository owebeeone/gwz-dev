# Shared SSH Worker Checkpoint — Code-Axis Review

**Review object:** shared SSH worker and destination routing at `gwz-core` `59ac059c8bfe4052d8a5a16d821f314a3985bcc8`; controlling DRAFT `dev-docs/GwzRemoteTransportSshWorker.md`; 2026-09-21  
**Baseline:** root `dc6480116755f95a31ffae0b3dada1ce3d26c2a9`; evidence `53ab4831a04fc7ab67975456737ae5ed20dc8174`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Code architecture, interfaces, call graphs, ownership, error paths, and compatibility. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 compatibility finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I inspected the complete changed range, including `ssh_worker.rs`, `ssh_destination.rs`, `ssh_endpoint.rs`, the pump clock change, all new tests, isolated manifest/lock wiring, the controlling DRAFT, accepted transport stream/pool contracts, the root checkpoint, and the private worker-a campaign. I compared destination behavior with the pinned libgit2 SCP parser and its accepted vectors.

All 23 campaign SHA-256 fingerprints match the committed core blobs. Production additions match the recorded 664 lines across the three new files; the test allowance is accurately disclosed.

I reran the permitted Rust 1.95 offline locked suite. It passed 35 tests, with the one ignored fake-agent child entry executed by its passing parent. Native push, clone, second push, and fetch succeeded; worker admission, shutdown, fault isolation, routing, destination, pump, pool, and channel cases passed.

The tuple was unchanged at review end. Final trees were root `bfcd899b8cac0574535a12adee969d8931e8aa78`, core `219f7e2fe845180a0ab3241815934129f065efae`, evidence `febca07b73fea8274f5dfbc33f4aa9e1d3625a7a`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Only the two permitted root prompts were untracked.

## 1. Findings

### [P2-1] The destination parser narrows the existing SCP grammar

**Location:** `gwz-core/src/git/endpoint/ssh_destination.rs:76-81,105-141`; pinned libgit2 `tests/util/url/scp.c:31-119`.

The accepted design requires preservation of current Git outcomes and routes SCP syntax through the SSH endpoint. The checkpoint also claims that destination admission handles SCP and retains effective ports. The new parser instead treats every bracketed authority as an IPv6 literal and forbids SCP-side port syntax. It also rejects `/` as a repository operand.

Concrete counterexamples accepted by the pinned libgit2 parser but rejected by `Destination::parse` are:

- `[example.com]:/resource`
- `[example.com:42]:/resource`
- `[git@example.com:42]:/resource`
- `[192.168.99.88]:/resource`
- `example.com:/`

The first four fail because `host_port_parts` requires bracket contents to parse as `Ipv6Addr` and rejects an SCP port. The last fails at the explicit `path == "/"` rejection. These are side-effect-free refusals, but after activation they turn previously accepted SSH remotes into `InvalidInput`, violating the compatibility and complete-route contract.

**Required correction:** make SCP admission cover the pinned currently accepted authority/path forms, including bracketed hostname/IPv4/IPv6, bracketed user and effective port forms, and root operands, while retaining explicit Windows-drive/local-path disambiguation and fixed redacted errors.

**Closure test:** add table-driven destination and Route regressions for the pinned libgit2 accepted vectors above, plus `C:\repo`, `C:/repo`, malformed brackets, password-bearing URLs, and invalid ports. Assert the expected canonical key/path and that malformed inputs cause no identity or connector call.

## 2. Invariant analysis

The worker itself held under attack. Admission count and queue length are bounded; permits cover queued and pending work and are released before replies. A single monotonic origin drives pool and stream clocks. Pump time advances before ingress, so a late Close cannot hide exact expiry. Disabled connect timeout remains pending but shutdown wakes the caller.

Pending and active owners retain leases until pool release; cancellation, failed attachment, client drop, pump failure, and shutdown all discard rather than falsely reuse. Physical capacity remains occupied through actual disposal. Active exchange failures are handled per entry and do not terminate unrelated streams. Endpoint clones share the worker, while final endpoint loss stops it and wakes callers.

Route parsing precedes identity resolution, and identity authority is re-resolved before every checkout. Password-bearing URL syntax and malformed recognized SSH spellings fail with fixed redacted errors. The native-agent test accurately characterizes an unresolved blocking API limitation and makes no authentication-readiness claim.

## 3. Risks and next action

Production credential setup, callback activation, platform parity, and selected-source qualification remain explicitly deferred and were not treated as defects. The next action is the bounded SCP parser compatibility correction and focused regression above, followed by a Code re-verdict on that changed range.
