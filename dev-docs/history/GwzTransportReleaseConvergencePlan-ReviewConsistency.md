# Transport execution plan (revision 1, sha256 08968279…) — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 1, uncommitted draft, 375 lines, sha256 `089682795e934a1506f08b3d17fbea51e612b9ae125bc070bfb600849b88c919`.
**Baseline:** root `9fbe5108` (`7af11354` is an ancestor), gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b9`.
**Date:** 2026-10-24.
**Axis:** Consistency, with a Surface section. The review is independent, adversarial and read-only. The Safety axis runs in parallel. This report is filed verbatim.

**Verdict: NO-GO.** P0 0, P1 0, **P2 11**, P3 6.

The plan's goals trace to the operator. Its measurement table is mostly coherent. The main problems are four:
- Phase 1 departs from the accepted Connection API contract in several places, while citing that contract as its authority.
- Phase 2 adds a CLI and Python interface that duplicates an accepted, Surface-reviewed server design. The plan neither cites nor retires that design.
- Phase 2 has no channel for carrying the operation itself between the two processes.
- Two of its acceptance rows (R7 on Windows and R8) cannot be measured as written.

Pre-commit to GO: GO once P2-1 to P2-11 are closed as each finding's "Fix" states, with no new P2 in that remediation.

## 0. Evidence base

- **Object hash.** Checked at the start and at the end, identical (§3). Line count 375.
- **Contract.** Read `dev-docs/GwzConnectionApiContract.md` (888 lines, accepted revision 2) in full for §1–§8 and §12–§15.
- **Placement documents.** Read in full: `gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md` (448 lines) and `gwz-core/docs/TransportPlacement.md` (408 lines).
- **Amendment 2 and the release plan.** Read `gwz-core/dev-docs/history/reference/GwzTransportReleasePlanAmendment-2.md` §1–§3.3 (accepted), and `…/reference/GwzTransportReleasePlan.md` §2 and §9.
- **Other controlling documents.** Read:
  - `dev-docs/GwzTransportReleaseRemediationPlan.md` §5 (DRAFT);
  - `dev-docs/GwzLaneCloseout-2026-10-19.md` in full;
  - `dev-docs/GwzGitBoundaryRemediationPlan.md` (status line: DRAFT);
  - `dev-docs/AgentProcessRules.md` L1-33.
- **Parked server design.** Read `dev-docs/history/parked/GwzCoreServerDesign.md`, the status block and §1–§3. It was accepted 2026-09-28 by Consistency, Safety and Surface reviewers, and parked at `39b342ed`.
- **Source.** Inspected:
  - gwz-cli `src/globalargs/parser.rs`, `src/help.rs` (`ROOT_HELP`) and `src/globalargs/dispatch.rs`;
  - gwz-py `src/gwz/client.py:185-235`, `src/gwz/bridge.py:150-290`, `native/src/route/transport.rs` and `native/src/client_host.rs`;
  - gwz-core `transport_host/{mod.rs, session.rs, session/local_link.rs, cli_placement.rs, local_command.rs, cancellable.rs, session/capacity.rs, driver_tests.rs}`, `git/endpoint/{mod.rs, placement_endpoint.rs}`, `git/mod.rs`, `protocol/mod.rs`, `Cargo.toml`, and `git log` for `a5702b38` (C3a);
  - the gwz-transport README and `src/codec.rs`.
- **Document counts.** Root `dev-docs/` has 73 active files (66 tracked, 7 untracked) against a cap of 75. `CurrentProgramCheckpoint.md` is 8,388 lines against a cap of 2,500.
- **Not done.** No build, test or tool run. The claim that gwz-credential is green on three platforms was not checked against CI.

## Problem, requirement provenance, and scope

**§1 against the operator's 2026-10-24 statements.** Credential-free core machines, the client making every outbound connection, a per-invocation split designed and tested now, per-host reuse at the server's cap across repositories and phases, Windows as a full peer, and two processes on one machine as the first proof are all restated faithfully. Two gaps:
- §1 **omits** the operator's direction "no further work improving 1.0.17 or the old architecture; the transport is the mechanism for future features". A reader therefore cannot test D-F, Phase 5 or the plan's silence on the parked session and server designs against it (P3-5).
- "The first form is one command split across two machines" is §1's own framing. §6.3 then makes that form evidence-only. This is a consistent reading, but it should be stated as a choice.

**Phase traceability.**
- Phases 0–3 and 6 trace to §1 and to the accepted contract.
- Phase 4 traces to contract §3 and §13. Its placement is reversed by D-C (P3-4).
- Phase 5 traces to the Git boundary plan, which is a DRAFT. It has no row in §3, yet Phase 6 waits on it. D-G (YES) authorizes 5.1–5.3, which is the only accepted provenance (P3-5).

**Preferences presented as requirements.** Phase 1's "in place, nothing moves files" is a preference ("so … parity evidence … stay[s]"). It drives the phase order that contradicts contract §13 (P2-5).

**Smuggled items.**
- Dependency upgrade: none. `windows-sys` already carries `Win32_System_Pipes` (`gwz-core/Cargo.toml:115`), and tokio `net` is present.
- New repository: none, under D-C.
- Network listener: the Windows named pipe in 2.2 is reachable from other machines by default, which contradicts D-D and §11 (P2-10).

## Surface (read cold)

This section was read from `gwz --help` (`gwz-cli/src/help.rs:7-37`, a hand-written `override_help`) and the `CommandArgs` enum (`parser.rs:255-380`) before the rest of the plan.

**Placement among the existing families.** The command families are `auth`, `hook`, `local`, `repo` and `stash`. `hook claude-code` already uses "Serve" in its description ("Serve another tool's hooks"). The root help is hand-grouped into Inspect / Change / Workspace / Members / Lanes / Other. `gwz core serve` therefore needs:
- a group line (none is named);
- `--core` added to the "Selection" or "Common options" block (not mentioned);
- a decision on whether `--core` is `global = true`, as every other selector is;
- a rule refusing `gwz --core X core serve`.

**Lifecycle.** There is only `serve`, with no stop, status or list, and no stated behaviour for:
- SIGINT or SIGTERM;
- a stale socket file after a crash;
- a server that exits or persists after the client disconnects.

**Defaults.**
- 2.2 puts sockets "under the user's runtime dir". macOS has no `XDG_RUNTIME_DIR`, and Unix socket paths are limited in length.
- The `<id>` in `\\.\pipe\gwz-<user>-<id>` is undefined.
- `--core` has no default, and the client has no stated way to discover the address.

**Single-client behaviour.** "Accepts one client at a time": a second client is not stated to be refused (with which error) or queued.

**What `--core` runs.** None of the following is listed as a user-visible refusal:
- Under Cli placement, local-file routes, clone-family routes (`local …`, `merge --remote LANE`), `http://` and `git://` fail preflight. This is placement design §2, lines 95-98.
- `--max-retries` is refused (`cli_placement.rs:55-70`).
- In-process `note:` lines do not cross processes (`TransportPlacement.md:371-378`).

**Paths.**
- `--root` and `--target` paths: whose filesystem? The help text says relative operands stay relative to the invoking directory (`parser.rs:69`).
- `caller_cwd` must keep describing the execution filesystem (placement design §5).
- `endpoint_path_base` is required for relative `--identity` paths.

None of these is settled. On one machine they coincide, which hides the problem until §1's two-machine form.

**Python.** Today the signature is `Client(root=None, bridge=None, *, max_connections_per_host=None)`, and the bridge is the seam for alternative cores. `core=` duplicates the role of `bridge=`, with no stated precedence. Also unstated:
- whether the carrier connection belongs to the Client or to the module's composition object;
- what `close()` does to that connection;
- how two Clients behave against a single-client server.

**Prior art the plan does not cite.** The accepted server design, parked but not cancelled under L1-33 ("Archive location does not cancel authority"), already settled this surface through a Surface review:
- `gwz server start | stop | status | list | stdio`;
- `--server`, `--no-server` and `GWZ_SERVER`;
- `SocketCoreBridge(address)` passed to `Client` as its bridge;
- default addresses, peer checks and stale-socket rules;
- an `ssh://host/path` remote form.

The accepted release plan's §2 row ships "`gwz server` and `gwz-py server` over a Unix-domain socket or a named pipe, for the same user on the same machine".

**Is deferring the shape to Phase 2's review acceptable?** No. The review reads `--help` after implementation, so every item above becomes the 2.4 implementer's decision. §6 rule 6 then records it as a "preference" rather than a decision. Raised as P2-1.

## 1. Findings

### P2-1 — `gwz core serve` / `--core` / `Client(core=…)` duplicate an accepted surface without naming or retiring it, and leave the shape to the implementer
- **Evidence.** The accepted parked server design (`dev-docs/history/parked/GwzCoreServerDesign.md:12-16`, lines 35-38 and 77-88) and the release plan's §2 row (`…/reference/GwzTransportReleasePlan.md`, the "`gwz server` … same user on the same machine" row) define the same split surface under different names. Amendment 2 §1 (line 90): "session contract, server design and session plan govern 1.2.0".
- **The plan's position.** It cites none of these. §2 ("so nothing is designed twice") and §6.2 ("one live design per seam") are contradicted. The open shape items are listed in the Surface section above.
- **Impact.** Two incompatible public surfaces for one seam. The user-facing shape gets decided in code, which is the churn §6 is meant to stop.
- **Fix.**
  - Add a §10 decision: adopt the server design's surface (as amended), or retire it with a reason. The operator's "old architecture" line may be that reason; if so, say it.
  - Then freeze, before 2.4: family and help placement, global flag and conflict rules, default address and discovery, the second-client response, stop and stale-socket handling, the list of commands that `--core` refuses, the path-base rule, and the relation between `core=` and `bridge=` in Python.

### P2-2 — The carrier only carries transport frames; nothing carries the operation between the processes
- **What the plan has.** 2.1 extracts `LocalLink`'s two-lane pump, which moves driver and endpoint transport ports only (`local_link.rs:1-40`). 2.2 puts "the frame codec from gwz-transport" on a socket. 2.4 says the client "connects the carrier, and runs the command".
- **What the command needs.** gwz-cli calls core handlers directly (`dispatch.rs:153-397`, about 25 handlers with in-process event callbacks). A `--core` client therefore needs a serialized operation request/response/event/cancel service: progress rendering, `--json`/`--jsonl`, and diff/log spools.
- **What the accepted designs say.** The placement design says transport envelopes ride in `RequestMeta.transport_message` / `ResponseMeta.transport_message` of the existing Taut messages (§3 table; §4, lines 182-189: "This programme does not invent its missing outer framing or service mechanism"). §1 (lines 63-64) adds: "No new command, service method, physical framing, socket between CLI/core … is authorized". `TransportPlacement.md:13-14` says "There is no new CLI command, server address or hosting service to configure."
- **What 0.1 names.** Only "amendment-2 clauses (§3.2's placement rows, §9's 1.2.0 list)" and the 1.2.0 parking. D-D decides the socket but not the operation channel.
- **Impact.** 2.4 (estimated about 300 + 250 lines) cannot be executed as written. A core design decision is left open.
- **Fix.** Add a Phase 2 step that defines the operation channel: which messages, whether attachments are carried in-band per the placement design §3/§4, ordering against transport frames, cancellation, and backpressure. Name the placement-design clauses (§1 lines 19-39 and 63-67; §4 lines 182-189) and `TransportPlacement.md:13-14, 115-120` in 0.1.

### P2-3 — Phase 1's host shape departs from contract §4, §4.1, §6 and §7 while claiming "Public shape per contract §4"
- **Ports (4.1).** The plan lists "contract §4.1's ports (EndpointFactory, Clock, Progress, ProcessSafety, CredentialPort, Carrier)". Contract §4.1 (line 162) says "`HostPorts` is exactly four narrow capabilities": `Clock`, `OwnerSeed`, `Progress`, `EndpointFactory`. `OwnerSeed` is dropped. ProcessSafety is a NativeAdapter capability (§7.3). Neither CredentialPort nor Carrier is a host port. A `Carrier` port in gwz-connection also conflicts with §3 ("does not implement sockets").
- **Cleanup domain.** Contract §4 requires `cleanup_domain(config, retention) -> (CleanupDomainOwner, CleanupDomain)`, passed in `HostConfig`. No step in Phase 1 builds it, and no CleanupDomain or RetentionPort exists in source (searched gwz-core and gwz-py). Yet 3.2 closes hosts "through the retained cleanup domain".
- **Configuration placement.** 1.1 puts "SSH and HTTPS endpoint settings" into `build`, and 1.5 and 3.2 use "one host per key" ("`HostOwner`s keyed by `EndpointConfig`"). The contract puts the profile in `PreparedOperation` ("an explicit EndpointRoute and validated EndpointProfile", §7) and the registry in the host ("Connection host | Endpoint registry", §6: "Configuration mismatch chooses another instance").
- **Impact.** The contract cited as governing (D-A) is contradicted in three places, none named in §10. 3.2 depends on an object no step creates.
- **Fix.** Either conform: four ports, `cleanup_domain` in 1.1, profile per operation with a host registry. Or name each departure as a contract amendment and record it in 0.1.

### P2-4 — 1.2 and 1.3 keep one long-lived mux and roll it over, which contract D1 and D6 forbid
- **The plan.** 1.2 binds operations "over today's `runtime.request`", that is, registrations on the runtime's single mux session (`session.rs:637-650`). 1.3 offers "renewing the mux session when idle near the bound, or a bounded generation ring", citing "(contract §6)".
- **The contract.**
  - D1 (line 47) prevents "Keeping one long-lived application mux and rolling it over to recover reuse".
  - D6 prevents "A host-wide 256-registration exhaustion/rollover programme".
  - §6 (lines 304-309): "Each operation gets a fresh transport binding/session ID. Existing 256-request mux … limits remain local to that binding … No host rollover".
- **Interaction with Phase 2.** Under Cli placement, any session replacement invalidates the capability generation and requires a fresh query (placement design §3, lines 163-166). Phase 2 runs in parallel and does not account for that.
- **Fix.** Make the operation binding the mux/session owner, per contract §6. Drop the rollover option from 1.3. State how a per-operation binding is bootstrapped over the socket carrier.

### P2-5 — The phase order inverts contract §13, so Phase 1's review gate cannot be met
- **The plan.** Phase 1 is built "in place" inside `gwz-core/src/transport_host`. The ports arrive only in 4.1, and the crate only in 4.2, after the consumers in Phase 3. Phase 1's review nevertheless requires "contract §13.1–2 portable proof rows (fake connector and clock, 10,000 operations…)".
- **The contract.** §13.1 calls for a proof "with fake EndpointFactory, Clock, Progress and CleanupDomain" and says "The first proof does not compile core/git2". §16 says the next implementation step is "a small-proof boundary limited to the new crate". Remediation plan M3's exit: "a public-API proof on the intended library boundary. Keeping a runtime alive … alone does not pass."
- **Consequence.** Without ports there is no fake connector, so the gate cannot be satisfied. Phase 1's "R1 (owner level)" also has no defined measurement: R1's recipe targets github.com.
- **Provenance.** Neither D-C nor D-G names this reordering.
- **Fix.** Move the ports (4.1) into Phase 1 before 1.2, so the §13.1–2 rows can run against fakes. Or name the reordering as a contract §13 amendment and replace Phase 1's gate with one that is achievable. Define what "R1 owner level" measures.

### P2-6 — §2 and step 2.3 misstate what is parked; the unpark is a public schema change
- **The file that is not parked.** §2 says `placement_endpoint.rs` (343 lines) sits "behind `cfg(gwz_session_candidate)`". It does not:
  - it is declared without a condition at `git/endpoint/mod.rs:9`, under the transport switch only (`git/mod.rs:4-8`);
  - it is the live endpoint bridge, used by `session.rs:4`, `session.rs:593` and `https_endpoint.rs:13`;
  - C3a (`a5702b38`) moved `cli_placement.rs`, not this file.
- **Consequences of that.** 2.3's "move … `placement_endpoint.rs` … to the transport switch" is a no-op. Phase 2's claimed ownership of the file collides with Phase 1's endpoint work.
- **The protocol switch.** Unparking switches `protocol/mod.rs:4-20` from `generated.rs` to `candidate_generated.rs`. That changes the shipped wire schema (placement and attachment tags) for both drivers. It brings in the placement design §3 gate (lines 145-151: qualify decoding of missing new fields in Rust and Python before integration) and L2-02 regeneration. gwz-py also carries `gwz_session_candidate` handling (`Cargo.toml`, `run_tests.py`, `native/src/dispatch/mod.rs`, and the switch tests).
- **Scale.** The inventory has 39 `gwz_session_candidate` lines across about 25 source files, against "~150 changed".
- **Fix.** Correct §2. Re-scope 2.3 to include the protocol projection, the additive-decoding gate and gwz-py's side. Add gwz-py to Phase 2's ownership, or split 2.3 out of it.

### P2-7 — 2.4's "enforced by construction" has no step that builds a core runtime without an endpoint
- **Today's constructor.** `TransportRuntime::build_native` (`transport_host/mod.rs:291-346`) always takes `local: EndpointSettings`. It builds the local endpoint session, an in-process `Handoff` ("The driver and the endpoint share this process", TR2.18), the driver's SSH `home`, and the HTTPS `machine` proxy, then links them with `LocalLink` (line 338).
- **The other gap.** Local-only routes under libgit2 hold "libgit2's credential helper" (`gitbackend.rs:466`).
- **The plan.** No Phase 1 or Phase 2 step removes these inputs or defines a core runtime with no endpoint and no handoff. `mod.rs` belongs to the Phase 1 trunk, not the Phase 2 lane.
- **Fix.** Add a step that defines the core-only runtime and its constructor: no `EndpointSettings`, no `Handoff`, Cli placement forced on every request, local-only routes refused. Assign it to the lane that owns `mod.rs`.

### P2-8 — R7's Windows measurement does not exist as written
- **The row.** R7 and 2.5 prove "the core holds no credentials" by running the core "under a scrubbed environment (empty `HOME`, no `SSH_AUTH_SOCK`, no Pageant…)".
- **Why that proves nothing on Windows.** For a same-user process (D-D), the OpenSSH agent pipe `\\.\pipe\openssh-ssh-agent` is per-user and reachable whatever the environment says. Pageant is found by window discovery, not by an environment variable. The logon identity used by SSPI is inherent to the process. A scrubbed environment therefore shows nothing there.
- **Fix.** Specify a Windows row that actually separates the two sides, such as:
  - an agent that holds the real key and is reachable by the core, with a test showing the core never connects to it (by instrumentation or a source check);
  - or different users for core and client, with D-D adjusted;
  - or both.
  Then state what R7 proves on each platform.

### P2-9 — 6.3 and R8: the forwarding plan cannot carry the Windows carrier
- **The conflict.** 2.2 makes the Windows carrier a named pipe. 6.3 forwards "the local-socket carrier … over `ssh -L`/`-R`" for macOS → dabeest (Windows core) and Windows client → WSL/weftpi. OpenSSH forwards TCP ports and Unix-domain socket paths. It neither listens on nor connects to a Windows named pipe.
- **Consequence.** Neither pair named in R8 can be built as written without an unplanned relay or a second carrier type.
- **Fix.** Choose AF_UNIX on Windows, a relay with its own step, or the server design's `ssh://` stdio form. Then restate R8 and 6.3.

### P2-10 — The Windows named pipe is a network listener unless remote clients are rejected
- **The conflict.** D-D and §11 promise "no network listener in this release". A Windows named pipe created without `PIPE_REJECT_REMOTE_CLIENTS` accepts SMB clients from other machines.
- **The plan.** 2.2 says only "Same user only". It neither requires the flag nor tests it. A different-user refusal is also untested on Unix. Its red rows cover round-trip and drop only.
- **Fix.** Require reject-remote and same-user peer checks in 2.2 (pipe DACL and client token check on Windows; peer credentials on Unix). Add red rows for both.

### P2-11 — The release's identity against accepted amendment 2 is unstated, and the cited clause does not exist
- **Amendment 2 (accepted).** It assigns reuse across a Python process's operations, the session host and the server to **1.2.0** (§3.1; §3.2 table, line 103: "Reuse across a Python process's operations, which 1.2.0 ships"; §3.3 phase-to-release table). It keeps gwz-py at one runtime per operation in 1.1.0 (§3.17).
- **The plan.** It ships reuse, the split and the client-side host in one unnamed release.
- **0.1's citation.** 0.1 cites "§9's 1.2.0 list" among amendment-2 clauses. Amendment 2 has §1–§5 only; §9 is the release plan's "Out of scope", which lists "Client placement" and "a physical carrier". §3.1, §3.3 and §3.17 are changed and not named.
- **The closeout.** Paused obligations "keep their 1.1.0 standing … Nothing here is moved to 1.2.0" (§5). With no version stated, that cannot be checked; see P3-3 on D38.
- **Fix.** State the version (1.1.0, 1.2.0 or both merged). List the exact amendment-2 and release-plan clauses changed, with correct section numbers, in 0.1 and §10.

### P3-1 — §2 source-claim inaccuracies
- "WinHTTP proxy … are all `git/endpoint/*`": the proxy is in `transport_host/machine_proxy/winhttp.rs`, captured by `endpoint_environment::capture_machine_proxy`.
- 3.1's "46 test files that construct runtimes" could not be reproduced (42 / 31 / 14 by three plausible patterns). They are in gwz-core, not gwz-cli; only `dispatch.rs` and `src/tests/transport_scope.rs` in gwz-cli use these entries.
- Verified correct: `local_link.rs` 110 lines, `cli_placement.rs` 265, `placement_endpoint.rs` 343, `session.rs:642` (the `>= 256` check), `session/capacity.rs` (`TransportCapacityConflict`, line 93), `reused` tag 12 in the ordinary `generated.rs:2696`, gwz-git `src` 367 lines, gwz-credential 14 commits, `driver_tests.rs:76`, the existence of `with_local_transport*` and `with_cancellable_local_transport*`, and the gwz-py `route/transport.rs` call site.

### P3-2 — The parallelism diagram and the step dependencies disagree with the plan's own rule
§4 says "Nothing runs in parallel on both sides of an unsettled interface". Against that:
- Phase 2 starts "after 1.1", but 2.3 and 2.4 consume 1.2 (`admit`/`register`) and 1.5 (`EndpointConfig`).
- 2.1 must edit `mod.rs:338`, which belongs to the trunk.
- 1.5's `host.rs` is shared with the trunk's 1.3 and 1.4, so the sub-lanes are not disjoint.
- 3.1 retires `with_cancellable_local_transport*` while 3.2, running in parallel, still calls it (`gwz-py/native/src/route/transport.rs:21,194`). gwz-py's CI builds against gwz-core main (closeout §7), so 3.1 breaks gwz-py until 3.2 lands.
- 3.3 is drawn parallel to 3.1 and 3.2, but needs both.
- The diagram puts 1.4 after {1.3, 1.5, 1.6, 1.7}; the text says after 1.3 only.

### P3-3 — §7's parked items do not match the steps or the closeout
- D43/D51 "resumes in Phase 6.2", but 6.2 lists only the D50 mapping and the S5.6 cells.
- D52 "resumes in Phase 4.1", but 4.1's text does not mention it.
- D38 "resumes after Phase 4" with no step, while the closeout keeps its 1.1.0 standing.
- Closeout §6.1 names "at least three other paths" besides the generic `git_error` mapping. 6.2 fixes only "that mapping".
- Closeout §6.3 (spawn-gate fairness, "probe it before relying on it") and C3b's "sshd provisioning in the workflow" are not carried forward.

### P3-4 — §10 decisions amend contract clauses without naming them, and no step amends the contract
- **D-C** reverses contract §3 (selected `gwz-transport/crates/gwz-connection/`; "A core-hosted crate would preserve the current temptation") and §15's first two rows.
- **D-E** conflicts with contract §7.3: "A parent mutex that only known GWZ callers voluntarily take is not proof about arbitrary third-party spawns."
- 0.1 updates only the reuse and placement designs, leaving the governing contract stale. That contradicts §6.2.

### P3-5 — §1 omission, and Phase 5's provenance
- §1 leaves out the operator's "no further work improving 1.0.17 or the old architecture; the transport is the mechanism for future features".
- Phase 5 rests on the DRAFT Git boundary plan, has no §3 row, and still gates Phase 6. Its authority is D-G alone, which should be stated.

### P3-6 — Documentation rules and smaller wording issues
- 0.1 writes into `gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md`. L1-33 says "Historical files retain their original contents". The file must be restored to the active set, or the amendment placed elsewhere.
- Root `dev-docs/` is at 73 of 75 active files. This plan's two review files reach 75, and any verdict or remediation file exceeds the cap before 0.4 runs. Capacity is not checked before Phase 0's own artifacts are added.
- The revision-0 history copy was never committed, so "preserved unchanged" cannot be verified.
- Phase 6 has no Exit line, and Phase 0 has no phase review, against §4's "Each phase has one review".
- 6.5's "Dual review plus Surface" can be read as a third reviewer. Clarify that Surface is a section within one of the two reviews.
- "Phase 8's order" (6.5) should name the release plan's Phase 8.

## 2. Out-of-scope observations

- `gwz-core/docs/TransportPlacement.md:125` still says "isolated Unix candidate configuration", but the code is `any(unix, windows)` (`transport_host/mod.rs:121`, `cli_placement.rs:7`). This is a stale public guide; 2.3 edits that file anyway.
- `dev-docs/CurrentProgramCheckpoint.md` (8,388 lines) is an existing cap violation. The plan's 0.4 resolves it.
- The release plan §9 lists `dev-docs/GwzWorkspaceRouteMappingDesign.md` (untracked, not reviewed here) as "not transport work". The client-versus-core path mapping in the Surface section may belong there. Not assessed.
- Closeout §6.2 (root push blocked by `mem_gwz_credential` having no fetch URL): current status not verified. The root head `9fbe5108` has local commits past the closeout.
- `GwzTransportReleaseRemediationPlan.md`, which the plan "sequences", is itself a DRAFT.

## 3. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `089682795e934a1506f08b3d17fbea51e612b9ae125bc070bfb600849b88c919`, 375 lines. This matches the pin given at the start. Nothing was modified: the working tree shows the same untracked set as at the start.
