# Transport execution plan (revision 1, sha256 08968279…) — SAFETY-AXIS REVIEW

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 1, 375 lines, sha256 `089682795e934a1506f08b3d17fbea51e612b9ae125bc070bfb600849b88c919`. It is an uncommitted draft (`??`).
**Baseline:** gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b` (the `a27817b9` short form matches).
**Date:** 2026-10-24.
**Axis:** Safety: what the plan permits to go wrong. The review is independent, adversarial and read-only. The Consistency axis runs in parallel and was not consulted. This report is filed verbatim.

**Verdict: NO-GO.** P0: 1, P1: 2, P2: 7, P3: 7.

Pre-commit: GO once P0-1, P1-1, P1-2 and P2-1 to P2-7 are closed by plan text. That means named steps, red rows or recorded operator decisions. No code is needed for this gate.

## 0. Evidence base

- **Object hash.** Verified at start, matching the pin (§3).
- **Member heads.** Each `git rev-parse --short HEAD` matches the baseline above.
- **Controlling documents read:**
  - `dev-docs/GwzConnectionApiContract.md`: §1–§9; quoted lines 50, 307, 314, 481–485, 554, 578–583.
  - `gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md`: all of it.
  - `gwz-core/docs/TransportPlacement.md`: all of it; quoted line 84.
  - `dev-docs/GwzConnectionRemediationPlan.md` §4–§6, including items 9 and 17.
  - `dev-docs/GwzAntiPatternRemediationPlan.md` A2.
  - `dev-docs/GwzLaneCloseout-2026-10-19.md`: all of it.
  - `dev-docs/GwzCoreSessionCrateMap.md` §1 and line 144.
  - Amendment 2 §3.5 OD16 (line 168), §3.18 and §3.19.
  - `dev-docs/history/parked/GwzCoreServerDesign.md`: §4 peer and listener rules, lines 229–284 and 799–804. This is the accepted, now parked, design for exactly this kind of local socket.
- **Source read at the baseline:**
  - **Wire schema** (`gwz-transport/protocol/transport.taut.py`):
    - `Destination{host,port,path,ssh_username,https_username}`
    - `Identity{mode,key_path,path_base}`
    - `AuthPolicy` includes `windows_default`
    - `SetupFailureCause{connection_refused,not_found,address_not_available}`
  - **Helper configuration** (`gwz-core/src/transport_host/endpoint_environment.rs:115-122`): the helper environment is built into the endpoint configuration when the host is made.
  - **Helper isolation** (`git/endpoint/https_auth/runner/environment.rs`): neutral working directory, no repository config. This holds.
  - **Windows HTTPS policy** (`git/gitbackend/transport_binding.rs:190-200`): every HTTPS remote on Windows becomes `WindowsConfigured` or `WindowsDefault`.
  - **Spawn gate** (`git/spawn_gate.rs:13-15`): "Two hosts have two gates … that scope is the host's, not the process's."
  - **Carrier ownership:** `transport_host/mod.rs:243,338` (the `RuntimeState.local_link` field and its construction), `session.rs:45,642` (the 256 bound).
  - **Native HTTPS scope** (`https_worker/native/exchange.rs:200`): a unique scope per exchange. This holds.
  - **Native libgit2 route:** the credential callback in `gitbackend/transport_support.rs:155-163`, selected by `transport_setting.rs` (`GWZ_TRANSPORT`, `gwz.transport`).
  - **known_hosts** (`ssh_host_key.rs`): checked strictly on the endpoint, nothing written. This holds.
- **Checked and found to hold:**
  - The possession-proof nonce is drawn by the endpoint and uses a fixed gwz namespace (reuse design line 99). There is no signing oracle through the carrier.
  - The HTTPS helper never reads repository config.
  - Native-authenticated HTTPS connections are scoped per exchange and are not reused across bindings.
  - known_hosts and CA files are read on the client.

## Problem, requirement provenance, and scope

- **Problem and necessity.** The operator statements of 2026-10-24 justify both the client/core split and the long-lived connection owner. Decisions D-A to D-G are YES. That authorizes the new command `gwz core serve`, the `--core` flag, the local socket and the SSH-forwarded evidence run. It does not authorize a network listener.
- **Scope.** The safety premise of the goal, "machines that run gwz-core without holding any credentials", implies a threat model: the core machine is less trusted than the client. The plan never states one. Its acceptance row R7 measures that the core *possesses* no credentials. It does not measure that the core cannot *use* them.
  - Because the threat model is missing, P0-1 and P1-1 arise.
  - The operator can settle the model by decision. It cannot be left implicit while Phase 2 freezes the carrier and the `--core` surface.
- **Provenance problems.**
  - **D-E against contract §7.3 and A2.** D-E replaces the contract's fail-closed rule and A2's acceptance with a scoped claim, and the plan does not record that it amends them (P2-6).
  - **Turnover against contract D6/§6.** Step 1.3's two options contradict D6/§6 without naming an amendment (P2-5).
  - **HTTPS through the client.** R7 requires it, but the accepted placement design says "Client HTTPS comes later" (P2-1).

## 1. Findings

### P0-1: The carrier has no peer authentication, and the prescribed pipe name is one the accepted server design rejected as squattable. An impostor "core" gets the client's full credential authority.

- **What the plan says.** Step 2.2 (lines 183–188) gives the Windows pipe name as `\\.\pipe\gwz-<user>-<id>` and puts sockets "under the user's runtime dir". Its only access rule is "Same user only". Its red rows cover only a frame round-trip and a dropped carrier.
- **What is missing:**
  - a random pipe name;
  - `FILE_FLAG_FIRST_PIPE_INSTANCE`;
  - `PIPE_REJECT_REMOTE_CLIENTS`;
  - a security descriptor that grants only the user's SID;
  - a host-side peer check (`SO_PEERCRED`, `getpeereid`, `GetNamedPipeClientProcessId`);
  - client-side listener verification: `SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION`, `GetNamedPipeServerProcessId`, a token SID and integrity-level check, and the walk of a 0700 per-user directory;
  - a definition of the macOS directory, which has no `XDG_RUNTIME_DIR`.
- **The accepted design already solved this.** `GwzCoreServerDesign.md:240` says: "the pipe's name is not derived from the key, since every user can list the machine's pipes and create a pipe under any name it has seen". Its lines 799–804 list the full guard set above.
- **Why it matters.** Without these guards, a default-DACL named pipe is reachable over SMB, so "no network listener" is false on Windows. Another local user can also pre-create the name.
- **The client-side direction is the critical one.** The core decides `Destination`, `Identity.key_path` and `AuthPolicy` for every Open (schema lines 76–86). An impostor listener therefore makes the user's client:
  - authenticate to github.com with the user's agent or key, then push whatever Git data the impostor streams;
  - on Windows, answer NTLM to any host (OD16, amendment 2 line 168).
- **This is credential-equivalent exposure, and nothing in Phase 2 prevents it.**
- **Fix.** Make §4 of the parked server design (the peer check, listener verification, per-user directory and pipe rules) a normative input to 2.2 and 2.4. Add red rows:
  - a squatted listener owned by another user receives zero bytes;
  - a remote-pipe open is refused;
  - an other-user client is refused at accept;
  - an impostor listener of the same user at a lower integrity level is refused on Windows.
  - All rows run on three platforms.

### P1-1: No trust model for the core. A credential-free core can still direct every client credential at any host, and the plan does not decide whether that is acceptable.

- **The core chooses everything.** In `cli` placement the core freezes URLs and identity (placement design §5), then sends `Open{destination{host,port,path},identity{key_path,path_base},policy}`. Repository config on the core supplies remote URLs, `url.insteadOf` and `gwz auth identity` key paths.
- **Consequences under the plan:**
  - **Confused deputy.** The core can use the client's agent, keys or gh token against any repository the user can write.
  - **NTLM to any host.** On a Windows client every HTTPS remote takes `WindowsConfigured` or `WindowsDefault` (`transport_binding.rs:190-200`). The core can therefore aim the logon session's NetNTLMv2 response at any host. OD16 accepted that hazard "as on 1.0.17", where the remote list is the user's own on the same machine. The split moves the choice to the less-trusted machine, and OD16 never decided that.
  - **SSRF and port scanning.** The core can open TCP from the client's network position to any host and port. `SetupFailureCause.connection_refused`, `not_found` and `address_not_available` return a port-scan answer to the core.
  - **Client filesystem probe.** `CheckIdentity` success or failure on core-chosen `key_path` strings tells the core whether files exist and are readable on the client.
  - **Future widening.** After Phase 5.4, if `cli` placement carries the contract's generic `ExchangeSpec::Http{method,target,headers}` instead of the `GitService`-typed Open, the core could send arbitrary same-origin API requests (for example GHE `/api/v3`) with the client's credential attached.
- **Fix.** Before 2.2 freezes, add a §1 or §10 decision that states the core's trust level. Then either:
  - **(a)** record that a served core is fully trusted with the client's credentials for the session, with OD16 re-decided for the split; or
  - **(b)** add a client-side admission rule (an allowed destination set per invocation, no `windows_default` toward core-named hosts without an explicit client flag, refused ports), with red rows.
- **In both cases, add the invariant:** the client accepts only `GitService`-typed opens from a carrier, never generic HTTP.

### P1-2: R7 can pass while its property is false. Its measurement cannot tell non-use from non-possession, and "enforced by construction" is not true.

- **The test's limits.** In step 2.5 (lines 200–206) the core runs as the same OS user on the same machine. Scrubbing `HOME`, `SSH_AUTH_SOCK` and `PATH` changes discovery, not access:
  - `~/.ssh/*` is still readable;
  - on Windows, Pageant's window and the logon session's SSPI default credentials belong to the session and token, not the environment.
- **The pass condition is not measured.** "A fetch succeeds only because the client authenticated" has no stated measurement. Both processes reach the fixture sshd from the same loopback address.
- **The "by construction" claim (2.4) is false.** Lines 196–198 say the serve path has no `EndpointSettings`, but:
  - the native libgit2 route (`transport_support.rs:155-163`, selected by `GWZ_TRANSPORT` or `gwz.transport`, read on the core's side) opens the agent and reads key files without any `EndpointSettings`;
  - `with_local_transport*` builds an endpoint from the process environment and stays linked until 3.1, after Phase 2's proof;
  - the source check covers only "the serve module", not what it calls.
- **The wire is never checked.** No assertion covers the bytes on the carrier.
- **Fix:**
  - Run the R7 core as a separate OS user, or in a sandbox that cannot read the client's key directory.
  - Instrument the agent, Pageant, helper and gh doubles to record the PID of each caller, and assert zero calls from the core.
  - Force `cli` placement for every network route on the serve path, and refuse the native route there. Red row: `GWZ_TRANSPORT=native` and `gwz.transport=native` on the core side give a refusal with zero agent or key access.
  - Put canary secrets in the client's environment, key file and helper output, capture the carrier bytes, and assert that no canary appears.

### P2-1: HTTPS through the client is required but has no step, design or review.

- **The conflict.** R7 and 2.5 require `fetch`/`push` against "the fixture HTTPS server" through the client, and Windows parity needs SSPI and the WinHTTP proxy at the client. The accepted guide says the opposite: `TransportPlacement.md:84` reads "Client HTTPS comes later", and placement design §2 allows only SSH routes for `cli`.
- **The gap.** Step 2.3 only unparks the existing SSH-only placement. No step designs `cli`-placement HTTPS: the policy advertisement, helper and gh on the client, SSPI and CBT on the client, redirects, the 401→Gh transition across the carrier.
- **The risk.** It gets built inside 2.5's "~0 production" budget without the per-step dual review that secret handling requires.
- **Fix.** Add a step, before 2.5, with its own design amendment and review.

### P2-2: Only transport frames are specified on the carrier. The application request and response, and anything carried with them, are not, so "nothing on the wire is a credential" is unverifiable.

- **What is unspecified.** Step 2.4's client "runs the command" over the carrier, but the plan never says how the command, `RequestMeta` and `TransportOptions` (including identity strings and `endpoint_path_base`), events and the response cross.
- **The precedents point the wrong way.** The only accepted split design sends the client's environment snapshot over the socket (`GwzCoreServerDesign.md:325`, "secret-bearing"). That would put `GH_TOKEN` and similar values on the wire.
- **The proxy reader is a second path.** Phase 6.2 resumes the D43/D51 git `http.proxy` reader with no rule for where it runs. Proxy URLs carry passwords (closeout §6.1). If core-side repository config supplies the proxy, a credential, or a core-chosen route for the client's TLS, crosses the carrier.
- **Fix.** Define the message set that crosses the carrier. Forbid snapshots, and say explicitly which side's environment the core's Git uses. State that proxy and CA configuration are client-side only in `cli` placement, and assign 6.2's reader to the client.

### P2-3: `gwz core serve` has stuck and lying states that the plan's own rules reach.

- **Wedged server.** "Accepts one client at a time" (line 195) comes with:
  - no bound on the handshake or first frame;
  - no idle or keepalive timeout on the carrier;
  - no rule for a second connection (refuse, or queue in the backlog).
- **How it wedges.** A client that dies without EOF, which is the usual case behind `ssh -L` (6.3), blocks the server for every later client. The parked design had this row ("accepts … never answers: a client command fails within 30 seconds", line 860). The plan has nothing like it.
- **Unobserved local mutation.** When the carrier drops, nothing requires the core to cancel the operation's *local* phases (merge, checkout, commit after a fetch). The core can keep mutating repositories that no client observes.
- **Mid-push loss is untested.** R5 tests only "carrier dropped mid-fetch". The item-17 case (bytes accepted versus Git success, remediation plan §4) is a drop mid-push. There the client must report a possible remote effect, the core must not record success, and nothing may replay.
- **Fix.** Add these rows, and a rule that carrier loss cancels the whole core operation and records its outcome core-side.

### P2-4: Clients with equal configuration share a host whose helper environment belongs to the first Client, so a later Client can authenticate as the wrong account.

- **The code.** The helper's executable and environment are fixed when the endpoint is built (`endpoint_environment.rs:115-122`).
- **The contract.** It puts "helper environment, account tokens" outside the profile key (contract line 483).
- **The plan.** Step 1.5 keys hosts by the contract's `EndpointConfig`, and 3.2 makes equal-configuration Clients share a host.
- **The result.** Client B, whose `GIT_CONFIG_GLOBAL`, `GH_TOKEN` or helper differs, has its HTTPS lookups run with Client A's environment. B pushes as A.
- **R4 misses it.** R4 varies only `HOME`, the agent, the CA file and the proxy.
- **Fix.** Either move the helper environment into the per-operation authentication context before 3.2, or include it in the key. Add an R4 row that changes `GIT_CONFIG_GLOBAL` or `GH_TOKEN`.

### P2-5: Both turnover options in step 1.3 break the contract's stale-event and rollover rules.

- **Option 1, renew the mux session "when idle near the bound".** This is the rollover the contract forbids:
  - D6 (line 50) forbids "a host-wide 256-registration exhaustion/rollover programme";
  - line 307 says "No host rollover waits for other operations";
  - overlapping long operations can keep the session from ever going idle, which leaves `BindingLimit` refusals stuck at 256.
- **Option 2, "a bounded generation ring".** A ring reuses identifiers, which D6 and line 314 ("Retired binding IDs are never reused") forbid. Reuse lets a late event from generation *g* be accepted as *g+N*.
- **Fix.** Adopt the contract's per-operation binding, with a fresh session ID and the 256 limit local to that binding, so no host-wide turnover exists. Or record an amendment, with a stale-event row across wrap.

### P2-6: The Windows spawn-gate hazard multiplies from step 1.5 onward, and D-E narrows a fail-closed rule without recording it.

- **The multiplication.** Gates are per host (`spawn_gate.rs:13-15`). Step 1.5 creates one host per key, and 3.2 holds several hosts in one Python process. That is the A2 hazard, now on main. The `ProcessSafety` port only arrives in 4.1 (§7 line 326), and A2 itself has no position in the sequence (line 327).
- **D-E against its controlling rules.** D-E ("named kernel object … claim scoped to gwz callers") contradicts:
  - contract §7.3: "A parent mutex that only known GWZ callers voluntarily take is not proof" (line 578), plus its fail-closed rule (line 581);
  - A2's acceptance.
- **What the plan leaves out.** It names no amendment and no residual. The residual is real: third-party `subprocess` launches in gwz-py can inherit credential pipe handles.
- **The named object leaks across processes.** It is visible to the whole session. Client and core on one machine would contend for it, causing spurious `GateTimeout`, and any process in the session could hold it as a denial of service.
- **Fix:**
  - Have the composition root supply one gate for every host in the process from 1.1.
  - Name the clauses D-E amends and record the residual in the release notes.
  - Use an unnamed object, or a per-process name.

### P2-7: One review per phase lets secret-handling and wire-format steps merge to main unreviewed, and later phases build on them.

- **What the plan says.** Lines 91–92 and §6.4 give one review per phase.
- **What the controlling text says.** Crate map line 144: "Wire-format and secret-handling steps keep their per-step dual review" (line 30 says the same). Amendment 2's TR4.10 is the precedent for logon-session code.
- **Steps that qualify:**
  - 1.7: possession proof and known_hosts re-check;
  - 2.2: carrier framing;
  - 2.4: the credential-free core boundary;
  - 4.3: explicit-key material.
- **Why it matters now.** Phase 2 runs alongside Phase 1, so these steps merge serially and Phase 2 builds on Phase 1 code before Phase 1's review.
- **Fix.** Name those steps as per-step dual review.

### P3-1: The socket carrier's control-progress guarantee and frame-size bound are not stated or tested.

- **Lanes.** Step 2.1 says the carrier keeps "frames in order per direction". It does not carry over placement design §4's rule that a saturated data queue must not starve Window, Cancel or terminal frames, or the separate control capacity.
- **Allocation.** There is no bound on the whole received frame before allocation for the length prefix.
- **Fix.** Add a real-socket row for cancel under saturated data, and an oversized-length row.

### P3-2: The parallel lanes do not own disjoint files, and Phase 2 starts on an unsettled interface.

- **Overlap.** `LocalLink` is owned and built in `transport_host/mod.rs:243,338` and re-exported at `session.rs:45`. The Phase 1 trunk edits both: 1.1 and 1.2 wrap `RuntimeState`, and 1.3 edits `session.rs:642`. Phase 2's file list omits them.
- **Unsettled interface.** Phase 2 starts after 1.1, but 2.3 and 2.4 consume the operation API that 1.2 is still defining. That contradicts line 108's "Nothing runs in parallel on both sides of an unsettled interface".

### P3-3: Amendment 2 §3.18's logon-session must-match row has no home in the split.

- The plan does not say whether `gwz core serve` on Windows refuses a client from another logon session.
- It does not say that Phase 7's persistent client must carry the row, even though the Phase 1 host is to be "hostable from day one".

### P3-4: Sensitive client metadata crosses the carrier, and the plan does not classify it.

- Key paths, `endpoint_path_base` (the client's absolute directory) and `CheckIdentity` results all reach the core.
- Contract line 481 says "Paths/agent addresses are sensitive metadata", which sits uneasily with "nothing on the wire is a credential" left unqualified.
- **Fix.** State what crosses and why that is acceptable.

### P3-5: The two-machine evidence path is unproven for Windows, and its likely fallback widens exposure (hypothesis, not verified on a host).

- **Feasibility.** Step 6.3 forwards a named pipe through `ssh -L`/`-R`. OpenSSH forwarding of Windows named pipes is not established.
- **The fallback.** Loopback TCP forwarding would:
  - expose the carrier to every local user on the forwarding machine;
  - make the peer check see ssh or sshd rather than the user's process;
  - pull a TCP connect path into gwz, which is Phase 7 work arriving early.
- **Fix.** Check feasibility in Phase 0. If TCP is needed, state that it requires a decision.

### P3-6: Diagnostics are lost when the endpoint is in another process, and R5 does not acknowledge it.

- Host-key refusal wording and capacity notes travel only through the in-process `Handoff`. `TransportPlacement.md` says "an endpoint in another process leaves none".
- R5's "truthful" cleanup and failure reports should state that loss, or plan for carrying the information.

### P3-7: Connection reuse can exist on main before its authorization, unless adoption waits.

- After 1.2, the host reuses idle connections across bindings without 1.5's partitioning or 1.7's authorization. That is safe only while no consumer adopts the host.
- Step 2.4's `--core` client does adopt it, inside Phase 2, possibly before 1.5, 1.7 and Phase 1's review.
- **Fix.** State that adoption waits for 1.5, 1.7 and the Phase 1 GO.

## 2. Out-of-scope observations

- **Crate placement.** D-C ("core-hosted … `gwz-core/candidate-crates/connection`") differs from contract §3's selected `gwz-transport/crates/gwz-connection/`. This belongs to the Consistency axis.
- **Duplicated design.** Phase 2 designs a new carrier module and framing without citing the crate map's `gwz-session-channel` (u32 length prefix, 64 MiB cap, control reserve) or `gwz-server-os` (listeners and peer checks). §2's "nothing is designed twice" list omits the parked server design entirely.
- **Publication.** Step 6.5 adds `gwz-connection` to the publication chain. §10 has no decision about publishing a new crate name.

## 3. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `089682795e934a1506f08b3d17fbea51e612b9ae125bc070bfb600849b88c919`, 375 lines, status `??`, unchanged from the start. Nothing was modified during the review.
