# GWZ transport: execution plan for the client/core split and the connection owner

Date: 2026-10-25. Revision: **5 — accepted as the execution plan after
[Consistency-5](GwzTransportReleaseConvergencePlan-ReviewConsistency-5.md) and
[Safety-5](GwzTransportReleaseConvergencePlan-ReviewSafety-5.md) both reported GO; this accepts the
plan text only.** The three P3 text corrections Consistency-5 lists are applied in this acceptance
revision (1.2's sentence, the diagram's 2.2c/3.0/3.0b, the `remote` module's owner, two stale
headings); nothing else changed after the reviewed hash `b225a24b…`. History: round 1
([Consistency](GwzTransportReleaseConvergencePlan-ReviewConsistency.md),
[Safety](GwzTransportReleaseConvergencePlan-ReviewSafety.md)), round 2 (-2), round 3 (-3: a new
architectural root cause R3-A1, stopped under the cap, decided by the operator as D-L on
2026-10-25), closure check (-4), final (-5); the intermediate rounds are archived in `history/`.
§12 maps every finding to its disposition. Revision 0 (the 2026-10-19 proposal) is preserved
unchanged as [history/GwzTransportReleaseConvergencePlan-r0.md](history/GwzTransportReleaseConvergencePlan-r0.md).
Each phase starts on the operator's instruction (D-G covers Phase 0, Phase 1's trunk, Phase 2 from
1.1, and Git rows 5.1–5.3).

Clerical record after acceptance (2026-10-25, no re-review; the decisions are the operator's, the
split changes no accepted clause): §10 gains **D-N** (control priority on byte streams) and **D-O**
(gwz-py's `server` family replaced), both stops raised by step 2.0's draft; §1.1 gains the three
rows those record and the placement design §4 row 2.0's §8 found implied by 2.0(a); step 1.3 is
split into 1.3a, 1.3b and 1.3c because the step as written would have had an endpoint instance own
its pool, against contract §4.1 and §8. Nothing else changed.

Companions: the [remediation process](GwzTransportReleaseRemediationPlan.md) (M1–M5, which this
plan sequences), the governing [Connection API contract](GwzConnectionApiContract.md), the accepted
[placement design](../gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md)
and its public guide [`docs/TransportPlacement.md`](../gwz-core/docs/TransportPlacement.md), the
accepted and parked [server design](history/parked/GwzCoreServerDesign.md) (its surface and §4
peer rules are adopted here under D-I), the [Connection–Credential boundary](GwzConnectionCredentialBoundary.md),
the [Credential API](GwzCredentialApiDesign.md), the [Git boundary plan](GwzGitBoundaryRemediationPlan.md),
the [residual anti-pattern plan](GwzAntiPatternRemediationPlan.md), and the
[lane closeout](GwzLaneCloseout-2026-10-19.md), which records where every paused lane's work is.

## 1. The goal, in the operator's words of 2026-10-24

- **Machines that run gwz-core without holding any credentials.** The core can run as a server
  on a machine that has software but no keys. A client elsewhere holds the agent, Pageant or
  Windows identity and the key files, makes every outbound Git connection, and the core's
  repositories are written where the core runs. Nothing on the wire between them is a credential.
- **The split is chosen per invocation, not fixed.** The first form is one command split across
  two processes; later forms (two machines as a supported feature, a persistent client, a
  networked core, remote file access) use the same seam. The architecture is designed and tested
  for the split now, even where a form is not shipped yet.
- **Connections are reused.** Per host, the client opens as many connections as the server
  allows (the operator expects 8 for GitHub; §3.1 measures it) and reuses them across repositories
  and phases. SSH setup is paid once per connection, not once per repository.
- **Windows is a full peer on both sides**, as client and as core. Nothing ships that works on
  macOS and Linux only.
- **No further work on 1.0.17 or the old in-process architecture.** The transport is the mechanism
  that opens the next features; releases are judged by what it makes possible, not by parity.
- **First proof:** the client and the core run as two separate processes on one machine, on all
  three platforms.

**Threat model (added in revision 2; Safety P1-1).** The core machine is the less trusted side:
it is the one that must not hold credentials. The client therefore does not take the core's word
for where to connect or as whom: it admits a core's request only for a (scheme, host, port,
repository path) it was told to allow for that invocation, only for the Git service class its own
command implies (a fetch-class command never admits receive-pack), and with an identity chosen by
the client's own invocation or configuration, never by the core (§4 step 2.0(c), decision D-H). The carrier between the two carries stream bytes, so when
it ever leaves the machine it needs transport security even though it carries no credentials.

Terms, matching the code: the **core** side issues stream requests and runs Git (the code calls it
the *driver*); the **client** side owns connections and credentials (the code calls it the
*endpoint*). The **carrier** moves frames between them; today it is the in-process `LocalLink`.
**Placement** names which side the endpoint runs on: `local` (same process as the core) or `cli`
(the client's process). The **owner** is the contract's `HostOwner`: the long-lived holder of
pools and endpoints on the client side. **In a served core the core holds a carrier-backed
`HostHandle` to the client's owner (D-L):** its `register` (as `bind`) and `open` travel over the
carrier while `admit` and `acquire` stay core-local (step 2.2c's wording governs; corrected 2026-10-25
after 2.0's Consistency review, O3), so gwz-git receives the same `Operation` under `local` and `cli`, and pool, reuse and
admission policy stay with the owner on the client. The carrier-backed `admit` carries only an
operation reference: the client's owner builds the `PreparedOperation` (profile, authentication
context, policy) from its own invocation and capture, and refuses any such field from the core.
The carrier-backed `open` carries only the existing typed Open (scheme, destination, Git service,
the identity the client selected); the core-side handle maps gwz-git's `ExchangeSpec` onto it
through a closed table of Git smart-HTTP service paths and refuses anything else. Admission is
enforced only at the client's owner on receipt, never in the core's handle; `snapshot()` on the
core's handle is refused.

### 1.1 Release identity and the accepted clauses this plan changes

The next release's version number is fixed at tag time. Amendment 2's split (`1.1.0` = the
in-process transport with per-operation runtimes; `1.2.0` = reuse, session host, server, gwz-py
through the session host) is replaced by this plan's §3 table: reuse across a client's operations
and the `cli` placement are in the next release; the persistent client, the networked core and
remote file access are Phase 7. Clauses changed (file:line as read 2026-10-24; historical files are
not edited, this list is the record):

| Where | Current text | Change |
|---|---|---|
| Amendment 2 §3.1 :84, :89 | per-operation runtime, `local` only; the reuse design's supersessions "take effect with the reuse steps in 1.2.0" | owner-held reuse and `cli` placement; the supersessions take effect with Phase 1 |
| Amendment 2 §3.2 :95–97, :103 | rows at BP:54 (reuse; "BP" is the release plan `GwzTransportReleasePlan.md`) and BP:57 (gwz-py through the session host) are 1.2.0; "pooled within one CLI command or one gwz-py operation"; "Reuse across a Python process's operations, which 1.2.0 ships" | reuse rows move into this release; gwz-py through the session host stays out |
| Amendment 2 §3.3 :113, :124; §3.17 | Phases 5–7, TR1.2, TR1.3 are 1.2.0; gwz-py one runtime per operation | reuse work in this release; gwz-py owns a host |
| Amendment 2 §3.13 rule (a) :348, :278–282 | `gwz_session_candidate` gates "reuse, and the candidate protocol's placement projection" | both leave the 1.2.0 gate (step 2.2) |
| Amendment 2 OD13 :355–356, :364, §3.5 OD16 :168 | 1.1.0/1.2.0 split; client placement "if that is scheduled"; OD16's NTLM hazard accepted "as on 1.0.17" | split replaced by §3; client placement scheduled; OD16 re-decided for the split by D-H |
| Release plan §2 :52, :60–64; §9 :565; amendment 1 :301, :336–337 | client placement and "a separate-process wire other than the local server socket" out of scope; `transport_capabilities` advertises only `local` | `cli` placement ships; the local server socket is the carrier; capabilities advertise `cli` |
| Placement design :12, :17–39, :63–65, :96–98, :132–135 | "not … a new carrier"; in-process embedding only; "No new command, service method, physical framing, socket between CLI/core"; `cli` admits SSH only; production builds do not advertise candidate placement | carrier and server command authorized (Phase 2); separate-process and process-kill qualification in scope; HTTPS at `cli` (2.4); production advertises `cli` |
| `docs/TransportPlacement.md` :3–9, :13–14, :25, :83–84, :104–126, :139, :150, :188–190, :274–278 | 1.2.0 header; "no new CLI command, server address or hosting service"; "Client HTTPS comes later"; `install_cli`/`CliEndpoint::new(SshEndpointConfig)` | rewritten in 2.2 and 2.4 |
| Server design (parked) :35–38, :77–88, §4 :223–330 | `gwz server start/stop/status/list/stdio`, `--server`, `GWZ_SERVER`, `SocketCoreBridge`, §4 peer and listener rules, `ssh://` stdio form | adopted as this plan's surface and carrier rules (D-I); its session host as a client-facing service stays Phase 7 |
| Server design §4 :223–330 (credential model) | "A server acts with its user's full authority"; the default address `auto` is `server-<key>` hashing the must-match set (home, agent source, OpenSSL configuration, CA files); "a server's sessions share the home and the agent source"; the server's endpoint registry; `auto` starts a server | **replaced** for the `cli` placement: the core holds none of those values; there is no `auto` address and no auto-start in this release (`--server <address>` or `GWZ_SERVER` is explicit); the registry and the must-match set are the client's profile (1.5); "reuse through a server" is reuse in the client |
| Server design §3 :202, :1170; `GWZRequirements.md:108`; `GWZDesign.md:104,108`; `session-contract` `frame.rs:25` | a socket host refuses a `SessionOpen` "that carries no snapshot"; "The snapshot MUST cross a byte stream once, in `SessionOpen`"; `SessionOpen` is the snapshot's carrier | **amended**: in the `cli` placement `SessionOpen` carries no environment; the core's Git uses the core's own environment, the client's connections use the client's (2.0(a), 2.1 red row) |
| Server design §16 :1175–1178 | "The remote core uses the remote account's agent, keys and `gh` login, or an agent the user's SSH configuration forwards … gwz neither turns forwarding on nor turns it off"; `SendEnv` may forward `GH_TOKEN` | **replaced** for the `cli` placement: the client launches `ssh` with `-a -x -T -o ForwardAgent=no -o ClearAllForwardings=yes` and no credential `SendEnv`; `gwz server stdio` refuses an inherited `SSH_AUTH_SOCK` or credential variable (2.3, 6.3) |
| Contract §4 :155 ("no … public constructor for Operation/Exchange"), §4.1 ("`HostPorts` is exactly four"), §5 (`register` is synchronous, does no I/O; revocation under the owner's authority lock with two legal race outcomes), §7 (`PreparedOperation` carries the profile and an `AuthContext` captured on the original caller entry), §12 (the composition root builds the owner and injects its handle) | an `Operation` exists only where a `HostOwner` exists, in one process | **amended (D-L)**: `HostHandle` becomes a capability type with two backings, in-process and carrier-backed; the carrier-backed one is constructed by `gwz-connection::remote::handle(binding)` from a carrier binding (a constructor argument, not a fifth `HostPort`); its `admit` carries only an operation reference, and the client's owner builds the `PreparedOperation` at its own entry (§7 holds at the client; the core supplies none of it); `register` on the core side is a deferred commit whose outcome arrives over the carrier, and carrier loss is a third revocation outcome, terminal `NeverRegistered` with zero effects (§5); `Operation`/`Exchange` keep no public constructor; §12 gains the served-core walkthrough (2.0(a)); §3's edges (gwz-git → gwz-connection) are unchanged |
| `GwzCoreSessionDesign.md` §4.1–§4.2, §5.1–§5.2, §13 (accepted, active); crate map frame-tag rules | call, cancel and `events.subscribe` as session methods, defined against §5's session host (records, admission, direct workers §5.2, the class-W mutator lock §5.1); frame tags 16–31 reserved for a transport lane | adopted as the operation service the carrier carries (2.2b): §4.1–§4.2 and the §5 clauses 2.2b names; §5's session host as a client-facing service stays Phase 7; the transport lane defined in 2.0(a) |
| gwz-core `GwzNoFallbackPlan.md` §6 (operator decision of 2026-10-14, `8cefbe40`) | native commit and tag "stay a module of gwz-core" | superseded by D-G: step 5.2 moves them to gwz-git with their characterization tests (the capability map, step 5.1, records the size: about 2,200 production lines) |
| Contract §7.3 | "A parent mutex that only known GWZ callers voluntarily take is not proof"; fail closed | D-E scopes the Windows claim to gwz callers and records the residual (third-party spawns) in the release notes; recorded in §15 at step 1.1 |
| Reuse design §2–§5, §11 | registry/instances/bindings, configuration, revalidation, capacity; "Client placement is not in this release" | superseded by contract §4–§8, §7.1; `cli` placement ships (step 0.1, landed in lane p0-design) |
| Placement design §4 :181–183 | hooks are "not new RPC methods or an independently serialized wrapper"; `RequestMeta`/`ResponseMeta` are the canonical attachment slots | the served form carries core-to-client envelopes in `TransportLaneFrame` on tags 16–17 and leaves the meta slots absent (2.0 §3.1, §3.3); recorded 2026-10-25 from 2.0's §8, implied by 2.0(a) |
| Session design §3 :115 (a byte stream "writes frames in the order they are sent and has no lanes to bound"); `session-channel` `byte_stream.rs:65, :130` ("writes every frame in call order") and its README ("does not use the lane"); `session-contract` `channel.rs:18–19` (one order "whatever their lanes") | one FIFO order on every byte stream; the lane argument is ignored there | **amended (D-N)**: a per-end priority writer on byte streams; the session design §3 and the byte-stream adapter are amended in place by 2.0 (text) and 2.1 (code) |
| Server design :13–14, §6: `gwz-py server start\|stop\|status\|list\|stdio` | gwz-py hosts a server with the same family as gwz-cli | **replaced (D-O)**: gwz-py is a client only in this release; it owns its in-process host (3.2) and reaches a served core through its bridge (`SocketCoreBridge`); no gwz-py `server` family is built or scheduled; a Python-hosted server is Phase 7 at the earliest |

## 2. What exists, so nothing is designed twice

Read 2026-10-24 at gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b9`.

- **The seam exists in process.** `transport_host/mod.rs:327–338` builds a driver session and an
  endpoint session and joins them with `session/local_link.rs` (`LocalLink`, 110 lines, two
  `TransportPort`s); `session.rs` defines the constructors. The frame codec, mux, sequenced streams
  and pool state machines are gwz-transport (no physical I/O, by charter). Bind/Bound bootstrap,
  registration, cancellation and shutdown already cross that seam as discrete messages.
- **The Cli placement is built and parked.** `transport_host/cli_placement.rs` (265 lines), the
  `pub use` at `mod.rs:119–123`, the cli arms in `request.rs`, `session.rs`, `session/checks.rs`
  and `session/driver/opening.rs`, and the protocol's placement projection sit behind
  `cfg(gwz_session_candidate)` since C3a. `git/endpoint/placement_endpoint.rs` (343 lines) is NOT
  parked: it is the live endpoint bridge under the transport switch and serves `local` placement
  too. The built `cli` endpoint is SSH-only (`CliEndpoint::new(SshEndpointConfig)`).
- **The framing exists as a crate; the operation service does not.** `gwz-core/crates/session-channel`
  gives length-prefixed frames with a size cap and a control reserve. `session-contract` is
  bytes-only (its own words: "this crate holds no GWZ or Taut protocol type and decodes no CBOR"):
  its tags are `SessionOpen`, `SessionCall`, `SessionReply`, `SessionError` and handshake/server-
  control frames; there is no event or cancel frame, and tags 16–31 are reserved for a transport
  lane and are a protocol error today (`frame.rs:14, :96, :154`). The call, cancel and
  `events.subscribe` methods exist only in the accepted `GwzCoreSessionDesign.md` §4.1–§4.2;
  nothing implements them. These crates are compiled into every gwz-core build through
  `pub mod session_host` (`lib.rs:70`), and their `EnvironmentSnapshot` is consumed by gwz-cli
  and gwz-py. `SessionOpen` is defined as the carrier of the environment snapshot, which is
  secret-bearing (`frame.rs:25`). The parked server design specifies the listener, peer-check,
  stale-socket and address rules for a same-user local socket and a named pipe. Phase 2 consumes
  the framing and those rules, builds the operation service (2.2b), and defines a `SessionOpen`
  without an environment for the `cli` placement.
- **Credentials already live on the endpoint side.** Agent, key files, Pageant, the OpenSSH agent
  pipe, SSPI, and the HTTPS credential helper are `git/endpoint/*`; the WinHTTP machine proxy is
  captured in `transport_host/machine_proxy/winhttp.rs` by `endpoint_environment`. The helper's
  executable and environment are fixed when the endpoint is built (`endpoint_environment.rs:115–122`).
- **One runtime per operation** is built and shut down in `transport_host/local_command.rs` and
  `cancellable.rs`, called from gwz-py `route/transport.rs` and gwz-cli `globalargs/dispatch.rs`.
  `TransportRuntime::build_native` (`mod.rs:291–346`) always takes `EndpointSettings`, builds the
  in-process `Handoff` and links with `LocalLink`. A mux session admits 256 registrations for life
  (`session.rs:642`); differing limits are refused (`session/capacity.rs:93`); a fault closes the
  runtime. About 48 test files construct runtimes (38 test-named, 10 under test directories).
- **The native libgit2 route** (`gitbackend/transport_support.rs:155–163`, selected by
  `GWZ_TRANSPORT`/`gwz.transport`) opens the agent and reads key files without any
  `EndpointSettings`. A credential-free core must refuse it.
- **Reuse is observable** per operation: `TransportObservation.reused` (tag 12,
  `protocol/generated.rs:2678`), in the ordinary protocol since C2.
- **gwz-credential** exists (14 commits, three platforms green per its own records) with no
  consumer; **gwz-git** exists (367 lines) with no consumer.

## 3. Acceptance: the finish line

Numbers are fixed in Phase 0 from baseline runs; none below is a target until §3.1 records it.

| ID | Outcome | Measurement | Proven in | Status |
|---|---|---|---|---|
| R1 | A client reuses connections per host across repositories and phases | 16 members at github.com, `fetch` then `push` then post-push reads: creations ≤ the host cap, `reused: true` after the cap is reached, warm time against baseline | Phase 1 (the fakes row of 1.8: two operations, one connection), Phase 3 (real CLI and Python clients) | not started |
| R2 | A long-lived Python client keeps connections across calls | two fetches then 50 sustained on one `Client` | Phase 3 | not started |
| R3 | Concurrent operations are independent | A and B overlap; cancel A; B completes; C reuses the idle connection | Phase 1 | not started |
| R4 | Changed identity, configuration or helper environment never reuses an incompatible connection | `HOME`, agent, CA file, proxy, `GIT_CONFIG_GLOBAL`/`GH_TOKEN` changed between operations: zero reuse across the change; no stale event reaches the new operation | Phase 1 | not started |
| R5 | Timeouts, retries and cleanup stay bounded and truthful across the carrier | the accepted stall and aggregate clocks hold with the endpoint in another process; cleanup reports name what the client still owns; carrier loss mid-fetch and mid-push ends both sides with a truthful outcome | Phase 2 | not started |
| R6 | Long-lived use is sustainable | 10,000 operations on one owner; per-operation bindings never exhaust a host-wide bound; memory, tasks and native work bounded | Phase 1 | not started |
| **R7** | **The core holds no credentials, and gwz code on the core cannot use the client's** | the core process holds no key, agent, Pageant, helper or `gh`; every connection is made by the client; the core's requests are admitted only for the client's allowed (scheme, host, port, path) and service class and with a client-chosen identity; no environment crosses the carrier; proven per platform as §4 step 2.5 states, in CI (on Windows the claim is "gwz code never uses": core-side hooks and filters run with the core user's logon session) | Phase 2 | not started |
| R8 | Shipping is supportable on the promised matrix | installed CLI and Python artifacts on three platforms with exact source receipts; one two-machine evidence run per direction (macOS client → Windows core; Windows client → Linux core) over `ssh … gwz server stdio` | Phase 6 | not started |
| R9 | Core consumes the restored libraries | every migrated gwz-git capability and `gwz-connection` have a production caller in core (adoption tests) | Phases 4, 5 | not started |

A status changes only in a phase report, which names the evidence run id. Phase 0 landed on
2026-10-24: 0.1 (root `830f3463`), 0.2 (§3.1), 0.3 (evidence run `2026-10-24-p0-baseline`, 901
files), 0.4 (root `475a1586`). The baseline values below are from that run's `tables/*.md`. Rules: a GO without the
row's numbers is not a GO. A new blocker is reported with evidence and its effect on this table,
never absorbed. A row that cannot be met with the proposed boundary is reported with the failed
assumption and cost; it is not moved to another release unless the operator writes that down here.

### 3.1 Measurement recipe

One recipe for every row, fixed here; step 0.3 fills each `___` with a measured value and later
phases rerun it unchanged. Hosts: **M** macOS ARM64; **L** Linux x86-64, WSL 2 on dabeest
(`/root/gwz-tests/<lane>`, ext4, D20); **W** Windows x86-64, dabeest (`E:/gwz-tests/<lane>`).
dabeest and WSL are booked with `BUSY.lock`, one run per booking. Runs are filed under
`gwz-core-evidence` `transport-qualification` (§8). Runners are byte-identical copies of the
sources below, with a diff for any change. The build under test and the Phase 0 baseline (current
main, transport shape, `--profile release`; 1.0.17 has no reuse and is not a baseline) are built
the same way on each host (`2026-10-15-s51/runner/build-candidate.sh`; W as `2026-10-16-tr84speed`).

**Workloads** (command `gwz --json ...`, cwd the workspace):

- **A, no-op fetch** (`2026-10-16-tr84speed`, `2026-10-04-tr8-1-macos-parity`, the 2026-10-01 Mac
  run): the 32 smallest non-fork public `owebeeone` repositories (`members32.txt`), at 1
  (`--target m/raidzz`), 16 (`members16-active.txt`, 16 detached) and 32 members, every member
  `Noop` before timing. HTTPS: anonymous github.com on M, L and W. SSH: github.com with the
  operator's agent on M only; L and W use the local fixture sshd (`fixture_ssh.py`, W as
  `tr84ctl.py`), because no credential may reach dabeest or WSL.
- **B, push and reads** (`2026-10-15-s51`, `s5ops.py`): 16 members each with a new 3-file 256 KiB
  commit against `fixture_ssh.py` and `fixture_https.py`, timed steps `push`, `read_moved`,
  `read_noop`, `read_pusher`; large packs `bigclone` (400 MiB) and `bigpush` (128 MiB). Restores
  and verification outside the timers; server repositories `pack.window=0`,
  `core.compression=0`, `transfer.unpackLimit=1`; every result compared with git's own. R1's
  "at github.com" push leg runs only against scratch repositories the operator names; until then
  R1 cannot pass (fetch legs at github.com, push legs on fixtures).
- **C, Python sustained** (new): one process, one `Client`, one member (as the baseline ran it),
  two fetches, then 50 sustained fetches back to back; per call: wall time, `reused`, creations;
  and a 16-member variant for R1's Python column.
- **D, overlap and cancel** (new; `s51` `slow_pipe.py` and `2026-10-17-gapsa`
  `ctl_latency_harness.rs`): on one owner, A (stalled fixture repository) and B overlap, A is
  cancelled, B completes, C then runs; records creations, `reused` per operation, cancel-to-effect.

**Definitions.** *Cold*: fresh process, no connection, owner just built; wall time from spawn to
`wait4`/`perf_counter` end, setup outside the timer. *Warm*: the same owner, an operation after a
completed one, connections idle and inside the pool's idle limit (the runner records the gap and
any `idle_closed`). *Sustained*: 50 consecutive operations on one owner, no sleep; median, p90,
max, and the first-10 against last-10 medians. Current main has no owner across CLI commands, so
its CLI baseline is cold only; warm and sustained baselines come from rows C and D (per-operation
runtime). The CLI's warm column is filled from Phase 3 on.

**Counting connection creations** (untimed passes, 3 per cell, never mixed with timed rounds):

| Host | Physical count | Cross-check |
|---|---|---|
| M | `connlog.dylib` (`s51/runner/connlog-macos.c`, `DYLD_INSERT_LIBRARIES`): `connect()` to port 22, 443 or the fixture port, agent socket ignored; the measured binary must be ad-hoc signed without hardened runtime, so for row C use an interpreter that keeps `DYLD_INSERT_LIBRARIES` (else `lsof -i` polling, recorded) | protocol count |
| L | `connlog.so` (`s51/runner/connlog.c`, `LD_PRELOAD`) | `ss -tn state established` polled at 10 ms for the peak; `nstat` `TcpActiveOpens` delta |
| W | `tcpwatch.py` (`GetExtendedTcpTable` polled about every 2 ms: entries owned by the process, plus new TIME_WAIT entries to the same remote) and, for SSH, the counting relay (`tr84ctl.py` `FixSshd`) | protocol count |

From the protocol on every host: **creations** = the `meta.transport[]` observations with
`reused: false` (distinct `connection_id` where the build emits it; `2026-10-16-tr84speed`'s did
not); **peak** = most connections to one host at once (`ss`, `tcpwatch`). Physical and protocol
counts must agree from step 3.0 on (the CLI's `--json` drops the receipts until then; Phase 1 and
Phase 2 rows rest on physical counts); a disagreement is a finding, not a rounding. A retried setup
is a creation and is reported as such, never excused.

**The per-host cap, measured, not assumed.** `CAP_gh_ssh` (M only) and `CAP_gh_https` (M, L, W):
a sweep of `gwz --max-per-host N --json fetch` over the 32-member workspace, N = 1, 2, 4, 8, 12,
16, 24, 32, 10 rounds each, 3 s apart, order shuffled; N is *clean* when all 10 rounds have no
failed member, no retried setup and peak = min(N, members); the cap is the largest clean N, found
by bisection between the last clean and first unclean N, and rechecked on a second day. The
operator's expectation of 8 was a hypothesis. **Measured 2026-10-24** (run `2026-10-24-p0-baseline`,
row C): GitHub refused nothing and throttled nothing from the Mac at 8, 12, 16, 24 and 32
concurrent SSH connections (3 rounds each; peak established sockets = the cap), nor over anonymous
HTTPS at 8, 12 and 16 from all three hosts. `CAP_gh_ssh` and `CAP_gh_https` are therefore **not
reached at 32**, and the per-host default stays **32, a chosen default, not a server limit**
(decision D-M). Rows below use CAP = 32 at github.com unless a later measurement lowers it. Fixtures: `CAP_fixture` is the `--max-per-host` the row passes;
the fixture sshd (`MaxStartups 200:30:300`) and HTTPS server are configured not to bind, and the
sweep is run against them once to show that. Rows below use `CAP` for the host's measured cap.

**The `--server` variant** (every row is also run with the core served; Safety round 3 P2-2): the
core is started with `gwz server start --root <workspace>` and its printed address passed to the
client as `--server <address>` (or `GWZ_SERVER`); the client is given the allowed set explicitly,
and nothing else: for workload A at github.com, `--allow-destination ssh://github.com/owebeeone/`
and `https://github.com/owebeeone/` (prefix form, trailing `/`), fetch class only; for workloads B
and D against the fixtures, `--allow-destination ssh://127.0.0.1:<sshd port>/` and
`https://127.0.0.1:<https port>/` with `--allow-push` for the same tuples; for the push leg at
github.com, `--allow-push` naming the operator's scratch repositories only. Row C (Python) passes
the same set as `SocketCoreBridge(address, allow=[…], allow_push=[…])`. A host-only grant is a full
grant for that host and is not used in these rows. R7's CI rows use the fixture tuples.

**Baseline values** (run `2026-10-24-p0-baseline`, candidate at gwz-core `461ddf4b`, transport
shape, cold processes): every CLI command creates one connection per member on every host and
protocol (32/16/1 members → 32/16/1 creations); a 16-member push and each of its three post-push
reads create 16 each (64 for the sequence); one Python `Client` over 52 fetches of one member made
52 creations with `reused: false` on all 52; `gwz --json` emits none of `reused`, `connection_id`,
`endpoint_id`, `stream_id` (step 3.0 adds them). Cold wall times, medians: HTTPS github.com
32/16/1 members M 0.466/0.426/0.274 s, L 0.475/0.284/0.251 s, W 0.369/0.322/0.283 s; SSH github.com
(M only) 2.567/2.659/1.984 s; SSH fixture 32/16/1 M 0.243/0.145/0.078 s, L 0.132/0.122/0.084 s,
W 0.660/0.452/0.305 s; push16 steps and large packs in the run's tables. The Windows SSH fixture
cannot serve receive-pack (plain Git for Windows hangs the same way), so Windows push rows are
HTTPS-only until step 3.0b fixes the fixture; recorded as a fixture limit, not an exception.

**Pass rules** (baseline = the values above, same host, protocol and step):

| Row | Workload, hosts | Creations / behaviour | Warm time against the Phase 0 baseline | Baseline |
|---|---|---|---|---|
| R1 | A (16 members), B; M, L, W; SSH, HTTPS | creations ≤ min(CAP, members) and peak ≤ CAP; after the fetch has created its connections, the push and the three reads create none (idle-closed replacements excepted, as R6 counts them); `reused: true` on every operation after the min(CAP, members)-th creation | warm build against the cold baseline (current main has no warm CLI state): warm median ≤ `___` s per step, wins in more than half the rounds; row C supplies the per-call warm figure | creations `___`; cold s `___` |
| R2 | C; M, L, W; HTTPS (SSH on M) | creations over the 52 calls ≤ CAP; none after call 2 | sustained median ≤ `___` s per call; last-10 median ≤ first-10 p90 | creations `___`; per call `___` s |
| R3 | D; M, L, W | creations ≤ 2; A `Cancelled`, B `Ok`; C `reused: true` | cancel-to-effect ≤ `___` ms (gapsa method); B within `___` s of its solo time | creations `___`; effect `___` ms |
| R4 | D variant: change `HOME`, agent, CA file, proxy, `GIT_CONFIG_GLOBAL`, `GH_TOKEN` between operations; fixtures | first operation after each change `reused: false`, then reuse resumes; no event of the old binding reaches the new one; a helper never runs with another operation's environment | none (correctness row) | n/a |
| R5 | `2026-10-16-r19stall` stall method (stall and aggregate clocks), endpoint in a second process; fixtures | failure inside the accepted clock plus `___` s; cleanup report names every client-owned item | none | clocks `___` s |
| R6 | A at 16 members, 10,000 operations on one owner (method `2026-10-15-s53`, `rss_run.py`) | creations ≤ CAP plus replaced idle-closed `___`; threads and tasks flat; the 256 bound crossed with zero stale events | RSS at 10,000 ≤ RSS at 1,000 + `___` MiB; W peak working set is new in 0.3 | RSS `___` MiB |
| R7 | two processes, scrubbed core (§4 step 2.5); CI | `connect()` count in the core process = 0; client count = R1's | none | n/a |
| R8 | A, B through installed CLI and wheel; M, L, W | as R1 | as R1 | as R1 |

**Rounds and pairing** (the `tr84speed` method). Phase N build and Phase 0 baseline alternate by
round on the same host in the same session, round 1 baseline first; 3 s idle after every run
(`--poison-gap 3`); a discarded warm-up first; 24 rounds in the main rows (even, so order stays
balanced), 20 in extra rows, 15 for push16, 5 for large packs. Report median and min..max of
each build, the paired median and p90 of differences, and rounds won. Verdict rule fixed before
any number is read: **met** when the paired median is at most 0 and the build wins more than half
the rounds; **missed by X s** when the median X is above 0 and it loses more than half; **within
noise** otherwise. A bad run (non-zero exit or failed member) is reported and counted, not
dropped; a second table without bad runs may be added. Host load is sampled every 5 s; M below a
1-minute load of 4, or the run is labelled loaded and not primary.

## 4. Phases, steps and parallelism

Each step is one goal, aimed at under 500 implementation lines (moved code counted separately).
Every code step is TDD: the row is written and seen red first. Each phase has one review of its
vertical slice, two reviewers, two-round cap; in addition, the steps marked **(dual)** below are
secret-handling or wire-format steps and keep their own per-step dual review (crate map §5,
line 144). "Receipt" means the Windows lane check with the hosted-like suite, plus a second receipt on
the merged commit whenever both sides of a merge touched a trigger path.

```text
Phase 0  ─ 0.1 ∥ 0.2 ∥ 0.3 ──► 0.4 ──► review of this plan (rounds 1–3, D-L, two closure checks: GO)
Phase 1  ─ trunk: 1.1 → 1.2 → 1.3a → 1.3b → 1.3c ──► { 1.5 ∥ 1.6 } → 1.4 → 1.7 → 1.8 (review)
Phase 2  ─ 2.0 (doc, dual) may start now ──► 2.1 (dual) after 1.1 ──► 2.2 (dual) after 1.2
           ──► 2.2b (dual) after 2.0 ──► 2.2c (dual) after 1.2 and 2.1 ──► 2.3 (dual) after 1.7
           ──► 2.4 (dual) ──► 2.5 → 2.6 (review)
Phase 3  ─ 3.0 ∥ 3.0b ∥ 3.1 ∥ 3.2 → 3.3 → 3.4 → 3.5 (review)   (after Phases 1 and 2)
Phase 4  ─ 4.1 → 4.2 → 4.3 (review)                          (after Phase 3)
Phase 5  ─ 5.1 now; 5.2 ∥ 5.3 after 5.1 (local Git only); 5.4 after 4.1; 5.5 → 5.6 (review)
Phase 6  ─ after 4 and 5
Phase 7  ─ next release: persistent client, networked core, remote files
```

Parallel means separate lanes with disjoint file ownership, merged serially. Nothing runs in
parallel on both sides of an unsettled interface: Phase 2's code steps start only after the Phase 1
step that fixes the interface they consume has merged (named per step). File ownership: the Phase 1
trunk owns `transport_host/mod.rs`, `session.rs`, `session/*`, `host.rs` and `endpoint_environment.rs`;
Phase 2 owns `session/local_link.rs` (until 2.1 replaces it), `cli_placement.rs`, the protocol
projection, `crates/session-channel`, `crates/session-contract`, the new carrier and server
modules, `gwz-connection`'s `remote` module (2.2c; the trunk owns the rest of the crate, and 2.2c
asks the trunk for any change outside `remote`), gwz-cli's server command and gwz-py's bridge; Phase 5 rows A/B own
`gitbackend/native_commit`, `refs.rs`, `repository.rs`. A step that needs a file another lane owns
asks that lane; it does not edit it.

### Phase 0: freeze (documents and measurements, about 6 lane-days)

- **0.1 One design per seam.** Landed in lane p0-design (root `830f3463`): the reuse design's
  status block names what contract §4–§8 and §7.1 supersede and what stays authoritative; contract
  §15 gains the matching rows and the `cli`-placement rows. §1.1 above is the record of the
  amendment-2, release-plan and placement-design clauses this plan changes; historical files are
  not edited. Remaining in 0.1: the contract's §15 rows for D-C (§3) and D-E (§7.3) are added when
  steps 4.1 and 1.1 land, not before (process rule 1).
- **0.2 Numbers.** Landed in lane p0-status: §3.1 above.
- **0.3 Baselines.** Landed in lane p0-baseline (gwz-core-evidence `aa4d8990`): the §3.1 recipe on
  current main (transport shape) on macOS, WSL and dabeest, evidence run `2026-10-24-p0-baseline`;
  the values are in §3.1. The cap sweep found no GitHub limit up to 32 (D-M). Not done there, and
  moved to 2.0's feasibility list: `ssh -a -x -T … <stdio process>` to dabeest and WSL with
  forwarding and `SendEnv` off; a Pageant double recording its caller's PID; the Windows
  separate-user launch (`CreateProcessWithLogonW` with redirected stdio).
- **0.4 Status consolidation.** Landed in lane p0-status (root `475a1586`): the checkpoint retired
  to `history/` unchanged, a 23-line pointer in its place; §3 is the live status. Live references
  to the checkpoint (AgentProcessRules L1761/1866, GwzProcessOptimization, GwzM5-8Refactor,
  WindowsParityPlan's line citations) are left as they are: they cite the archived file by its old
  path, which the pointer names.
- **Phase review.** This plan's own dual review: three rounds, stopped under the cap on R3-A1,
  decided by the operator as D-L on 2026-10-25; revision 4's closure check closed the record of
  D-L; revision 5's final check: GO on both axes.

Exit: one governing design per seam, a recipe, three-platform baselines, a cost, a GO on the plan.

### Phase 1: the connection owner, conforming to the contract (one trunk lane across gwz-transport and gwz-core, 22–27 lane-days)

The owner is built as the contract specifies it, in the crate contract §3 selects:
`gwz-transport/crates/gwz-connection` (D-C reversed, see §10), which gwz-core depends on by path
as it does on gwz-transport. The crate has no dependency on gwz-core or git2 (a Cargo edge, so
contract §13.1's "the first proof does not compile core/git2" is true by construction, not by
discipline). Today's SSH and HTTPS endpoint construction stays in gwz-core behind the contract's
`EndpointFactory` port, so the pools, the agent and key code, Pageant, the Windows agent pipe,
SSPI and WinHTTP stay where their parity evidence was taken. The Phase 1 trunk lane therefore
edits two repositories: the crate, and gwz-core's adapters. Step 1.1's paused attempt is in the
closeout archive's `pause/p1-trunk.md`; its owner/handle skeleton and the `build_native`
construction half carry over.

- **1.1 Ports, cleanup domain, owner and handle.** The crate `gwz-connection` (`lib.rs`, `host.rs`
  and children): the four port traits of contract §4.1 (`Clock`, `OwnerSeed`, `Progress`,
  `EndpointFactory`) with fakes in a test module; `cleanup_domain(DomainConfig, RetentionPort) -> (CleanupDomainOwner, CleanupDomain)`
  with today's reaper as the first `RetentionPort`; `build(HostConfig, HostPorts) -> (HostOwner, HostHandle)`
  (`HostConfig` carries the cleanup domain and the finite limits; no `Default`; no late context);
  `HostOwner` (`Send`, not `Clone`, `close(self) -> HostClose`), `HostHandle` (`Clone + Send + Sync`,
  `snapshot()`). The composition root supplies **one `ProcessSafety` gate per process** to every host
  it builds (contract §7.3; Safety P2-6): the self-minting constructors `HelperSlots::new`
  (`https_auth.rs:140`), `NativeHost::new` (`sspi.rs:32`) and `SpawnGate::default()`
  (`request.rs:66`) are removed or routed through the composition gate in this step. The Windows
  implementation per D-E: a per-process name; if the object cannot be created or exists with a
  foreign owner, native authentication fails closed; the residual (gwz-py processes, native
  `CreateProcess` calls with handle inheritance by code gwz does not write) recorded in the
  contract's §15 row this step adds. Red rows (fakes only): build/close/drop/invalid limits; owner
  close seals and completes within the bound; a handle outliving the owner fails with a named
  error; two hosts share one gate; no production constructor mints a gate (source check). ~350 + 300.
- **1.2 Operation bindings.** `handle.admit(PreparedOperation) -> Admission`, `acquire().await ->
  AdmissionPermit`, `register() -> Operation`, `Operation::open(ExchangeSpec)`, `finish()`,
  `cancel_handle()` (contract §4, §5). **Each operation gets its own binding: a fresh transport
  session with its own mux and its own 256-registration limit local to the binding (contract D1,
  D6, §6: no host-wide rollover, no identifier reuse).** The host owns endpoint instances and pools
  through `EndpointFactory`; a binding leases connections from them. **Both of today's mux sessions, the driver's and the endpoint's, become per
  binding**; only endpoint instances and pools are host-wide (contract §6: an operation binding owns its bootstrap and mux; an endpoint
  instance owns physical owners and pools). `ClientRequest::new`'s registration (`request.rs:463`)
  therefore counts against the binding, never the host. A binding is valid only on the carrier
  connection that bound it. Under `cli` the binding's driver half lives in the core process behind
  the carrier-backed `HostHandle` (D-L); the client's owner creates its endpoint half. Red rows (fakes): finish A then B reuses the idle fake connection;
  cancelling A leaves B's exchange; 300 bindings on one host never hit any 256 bound, on either
  side; a late event of binding *g* is rejected by *g+1*; frames under binding B's id arriving on
  carrier A are discarded with no state created. ~450 + 450.
- **1.3 `EndpointFactory` over today's endpoints (trunk; it edits `mod.rs`).** The SSH and HTTPS
  endpoint construction from `build_native` becomes gwz-core's production `EndpointFactory`; idle liveness (dead idle HTTPS detected
  and discarded; SSH already reports `idle_closed`) and fault scope (a binding fault closes the
  binding; a physical fault retires the connection, not the host; contract §8). Files:
  `git/endpoint/https_pool.rs`, `https_endpoint.rs`, the SSH worker's fault arm, the factory file.
  Red rows against the fixture servers. ~300 + 300.
  Split 2026-10-25 (clerical; no accepted clause changes): the step as written would have had an
  endpoint instance own its pool, which contract §4.1 and §8 forbid (adapters are at the
  Connector/Resource level; the host owns pooling). **1.3a** gate routing, the `ProcessSafety`
  implementations including Windows (D-E), and the source check with its planted
  `SpawnGate::new()` red row; independent of the adapter question; Windows receipt; ~400.
  **1.3b** the executor-run pump in the crate, the SSH adapter at the Connector/Resource level, the
  `ExchangeSpec` Git service and path (a crate addition under contract §9), fixture sshd rows.
  **1.3c** the HTTPS adapter and fixture HTTPS rows. Sizes beyond the original ~300 + 300 are
  recorded in the Phase 1 report.
- **1.4 Capacity per binding.** Limits travel with the binding; `TransportCapacityConflict` for
  differing limits goes; the host keeps physical ceilings (contract §8). Red row: two bindings with
  different `--max-per-host` overlap. Trunk, after 1.3 (shares `session/capacity.rs`). ~200 + 200.
- **1.5 Profile and partition (dual: the helper environment is a secret-bearing view).** `PreparedOperation` carries the `EndpointRoute` and validated
  `EndpointProfile` (contract §7), captured once at the original caller entry; the host's registry
  serves a request only on an equal profile and otherwise chooses another instance; **the helper
  environment and account tokens move into the per-operation authentication context** (Safety
  P2-4), so equal-profile Clients cannot run one another's helper. Red rows: `HOME`, agent, CA,
  proxy, `GIT_CONFIG_GLOBAL`, `GH_TOKEN` changed → `reused: false` then reuse resumes; no stale
  event. Files: `host.rs` registry, `endpoint_environment.rs`, the HTTPS auth context. ~300 + 300.
- **1.6 Cross-binding reuse authorization (dual).** Before leasing an idle connection last used
  by another binding: the agent signs a fresh SSHSIG nonce with the recorded key (ambient), or the
  key file is re-read and compared (explicit); known_hosts is re-checked (contract §7.1; the reuse
  design's §4 details stay authoritative). HTTPS connections authenticated by a helper token or by
  the Windows logon session are per-exchange scoped and are not reused across bindings (contract
  §7.2; amendment 2 §3.18 for the logon session). Files: `agent_auth.rs`, the `ssh_pool.rs` lease
  path. Red rows: key removed from the agent → fresh connection; known_hosts changed → refusal;
  expired helper token → no reuse. ~300 + 300.
- **1.7 Driver-only construction (dual).** A core-side builder with no `EndpointSettings`, no
  `Handoff` and no local endpoint: every request's placement is `cli`, local-only routes
  (`file`, clone-family, `http://`, `git://`) are refused before any effect, and the native
  libgit2 route is refused (`GWZ_TRANSPORT=native`, `gwz.transport=native` → refusal with zero
  agent or key access). This is what `gwz server` runs. Trunk, because it edits `mod.rs`. Red
  rows: the builder cannot be given endpoint settings (type-level); a `native` setting is refused;
  a `file://` remote is refused. ~250 + 250.
- **1.8 Phase review.** Code and State on the slice. Portable proof rows of contract §13.1–2 with
  the fakes, run in the crate alone (two operations, one fake connection, which is R1's owner-level
  row; 10,000 operations; no host-wide tombstone growth; R3, R4, R6); the fixture-server rows of 1.3 and 1.6; the three-platform hosted-like receipt; the
  spawn-gate fairness probe from the closeout (§6.3: a many-member pathspec log against a concurrent
  helper launch on Windows).

Parallelism: 1.1, 1.2 and 1.3 run in the trunk (1.3 rebuilds `build_native` in `mod.rs`). After
1.3 merges, 1.5 (the crate's registry module, created in 1.2 as its own file; `endpoint_environment.rs`;
the HTTPS auth context) and 1.6 (`agent_auth.rs`, `ssh_pool.rs`) run in two sub-lanes. 1.4 and
1.7 stay in the trunk.

Exit: R3, R4, R6 pass with fakes and on the hosted-like suite on three platforms; the driver-only
builder exists.

### Phase 2: the carrier, the server command and the two-process form (one lane plus one doc step, 35–41 lane-days)

Phase 2 adopts the parked server design's surface and §4 listener and peer rules (D-I), with the
clauses §1.1 lists replaced, consumes the `session-channel` framing, and builds the operation
service the accepted session design specifies but nothing implements (2.2b). It does not write a
new framing or a new peer check.

- **2.0 Operation channel and surface freeze (document, dual review; may start now).** One
  document, `GwzSplitCommandChannel.md` (root dev-docs; count checked first), fixing before any
  code: (a) **what crosses the carrier**: the session contract's request/response/event/cancel
  methods (call, reply, events, cancel; 2.2b) carried as `session-contract` frames, the calls
  carrying the existing taut `RequestMeta`/`ResponseMeta` with their `transport_message` envelopes
  (placement design §3–§4), diff and log spools as in-band attachments, progress events; **the
  served-core composition (D-L)**: the core builds a carrier-backed `HostHandle`
  (`gwz-connection::remote::handle(binding)`) whose `admit/acquire/register/open` cross the carrier
  to the client's owner, so `run_git_via_gwz_git` receives an ordinary `Operation` (contract §12's
  walkthrough, served form); the carrier-backed `admit` carries an operation reference only, and
  the client's owner builds the `PreparedOperation` from its own invocation (a core-supplied
  profile, authentication context or policy field is refused); the carrier-backed `open` carries
  the typed Open only, and the core-side handle maps gwz-git's `ExchangeSpec` to it through a closed
  table of Git smart-HTTP service paths, refusing anything else; admission runs only at the client's
  owner on receipt; `snapshot()` on the core's handle is refused; **whichever lane
  2.0(a) chooses for core-to-client envelopes, transport control (Window, Cancel, terminal frames)
  stays in `session-channel`'s control reserve and never queues behind an in-band spool**;
  **how core-to-client transport envelopes (`Open`, `CheckIdentity`) travel while a call is
  running**: the reserved frame tags 16–31 as the transport lane, or a long-poll method, decided
  here against placement design §4's rule that delivery is never held until the final response
  (wire format, so this document is dual-reviewed); **`SessionOpen` for the `cli` placement carries
  no environment** (amending `GWZRequirements.md:108`, `GWZDesign.md:104,108` and the server
  design §3, §1.1); **no environment snapshot crosses**; the core's Git uses the core's own filesystem and
  configuration for repositories, and the client's for connections, proxy, CA, helpers and
  identity (the D43/D51 proxy reader, when it resumes, runs at the client); the sensitive-metadata
  classification of what does cross (key paths, `endpoint_path_base`, `CheckIdentity` results;
  contract §7 "sensitive metadata") and why it is acceptable; diagnostics from the endpoint carried
  as events, or their loss stated. (b) **the surface**, from the server design as amended:
  `gwz server start|stop|status|list|stdio`, `--server <address>` (global, like every selector)
  and `GWZ_SERVER`, `gwz --server X server start` refused; **no `auto` address and no auto-start
  in this release** (the server design's `auto` keys the address on the credential environment,
  which the core does not have; §1.1); the address is a path in a 0700 per-user runtime directory
  (a defined macOS location) or a random-named pipe, printed by `server start`; concurrent clients
  on one server get the same workspace locking as two CLI processes; SIGINT/SIGTERM, stale socket
  and crash rules; the commands `--server` refuses under `cli` placement
  (local-file routes, clone-family routes, `http://`, `git://`, `--max-retries`); whose filesystem
  `--root`/`--target` paths and `endpoint_path_base` describe; Python: `SocketCoreBridge(address)`
  passed as `bridge=`, no `core=` parameter, carrier lifetime and `close()` semantics; the
  `transport_capabilities` advertisement of `cli`; **the client forms `stdio` and `ssh://[user@]host[:port]/path`**
  of the server design (:87–88), amended per §1.1's §16 row, adopted as the client's way to reach a
  served core over a child's standard streams (R7's separate-user leg and R8 depend on them; a
  test-only launch hook, living in the test harness and never in the client binary (the 2.1 source
  check covers it; it handles a generated password), covers `sudo -n -H -u <core-user> env -i …
  gwz server stdio` and the Windows `CreateProcessWithLogonW` form). The SSH username is the
  client's: `user@` in the allowance, else the client's SSH configuration for that host, else `git`. The service-class table is closed and by request type,
  not command name: fetch-class (`fetch`, `pull`, remote reads, `ls-remote`), push-class (`push`
  with its preflight and post-push reads); a request type not in the table is refused under
  `--server`; `forall` runs client-side on paths the core reports; `materialize` that would clone,
  `local clone` and `merge --remote` are refused (clone-family and local routes); repository-
  configured identities (`gwz auth identity`) are refused under `--server`. (c) **the trust model**
  (D-H). Admission lives
  in the client endpoint (the `cli` `HostHandle`, shared by gwz-cli and `SocketCoreBridge`) and runs
  before `CheckIdentity`, agent, helper or connect. The client admits a core's `Open` only when: its
  (scheme, host, port, repository path) is in the allowed set for the invocation, where the path
  is matched exactly, or as a prefix when the allowance ends in `/`, after canonicalization that
  refuses `.`/`..` segments and percent-encoded separators and normalizes SCP-style paths, `~`,
  `.git` and case per scheme; its service class matches the client's own command (fetch-class
  commands admit upload-pack only); **receive-pack is admitted only for tuples in a separate push
  allowance** (`--allow-push`, `gwz.server.allow-push`, `allow_push=`), so a push-class command
  cannot reach every allowed repository; the identity **and the username** are the client's choice
  (its invocation or its configuration), never a core-named key path or `ssh_username`/
  `https_username`; it is a `GitService`-typed open
  (never generic HTTP); the port must match the allowance's tuple (D-H (b); "the port is not refused" was corrected 2026-10-25 after 2.0's Safety review); and, under either Windows native policy
  (`WindowsConfigured` or `WindowsDefault`), the logon session answers NTLM or Negotiate toward a
  core-named host only with an explicit flag. Admission is applied again at every redirect hop
  before any credential, helper or native exchange. The allowed set is supplied non-interactively:
  `--allow-destination <scheme://host[:port]/path[/]>` (repeatable), the `gwz.server.allow` and
  `gwz.server.allow-push` settings (**keyed by server address**, so a grant for one server never
  applies to another; a host-only grant is a full grant for that host, documented in 6.4), and the
  `SocketCoreBridge(address, allow=[…], allow_push=[…])` parameters; for `--server` the default set
  is empty and admission fails closed with a named refusal, including the destination (canonical,
  control-stripped, punycode where applicable); with a terminal a prompt may add to the set for
  the invocation. §3.1's `--server` variant names the tuples each row uses. **Credential variables
  never leave the client**: the client launches `ssh` and `sudo` children with `GH_TOKEN`,
  `GITHUB_TOKEN`, `GIT_ASKPASS`, `SSH_ASKPASS`, `GCM_*`, `GIT_CONFIG_PARAMETERS` and the helper's
  own variables removed from the child's environment (so `SendEnv` has nothing to send), with `-a`;
  `gwz server stdio` refuses them if present. ~350 lines of document.
- **2.1 Carrier (dual).** `LocalLink`'s two-lane pump behind a `Carrier` seam used by the
  composition (outside gwz-transport and outside the future `gwz-connection`); the in-process
  carrier stays the default. The local carrier: Unix domain socket (macOS, Linux) in a 0700
  per-user directory, and a Windows named pipe created with a random name,
  `FILE_FLAG_FIRST_PIPE_INSTANCE`, `PIPE_REJECT_REMOTE_CLIENTS` and a DACL granting only the
  user's SID; peer checks in both directions (`SO_PEERCRED`/`getpeereid`,
  `GetNamedPipeClientProcessId`/`GetNamedPipeServerProcessId`, token SID and integrity level);
  framing from `session-channel` (length prefix, size cap, control reserve) so a saturated data
  queue never starves Window, Cancel or terminal frames. No test-only override of the peer check
  exists in production code (source check). Red rows on three platforms, for the CLI client and
  for `SocketCoreBridge`: round trip; carrier dropped → both sessions end with the carrier's
  reason; a squatted listener owned by another user receives zero bytes; a remote pipe open is
  refused; an other-user client is refused at accept; a same-user listener at lower integrity is
  refused (Windows); a `SessionOpen` carrying any environment field is refused and the canary
  capture covers the handshake; cancel under saturated data progresses; an oversized length is
  refused before allocation. After 1.1. ~350 + 450.
  Landing deviation, accepted by the operator 2026-10-28 ("record the deviation and push"): six
  gwz-core commits of the 2.1 landing, `d8e7478d`, `49e79493`, `91b4c924`, the merge `03cb6c34`,
  `5d2af53f` and `58bd9e07`, are RED in the per-commit lane gate for one cause only: the 25
  Unix-only Windows-parity rows the lane added in `windows_parity/2.1.json` lacked
  `approved_platform` (149 complaints, all that field). `3d0ec6bc` adds it and is green. The
  lane's matrices check only the tree at the tip, so none caught it; the six commits stay RED in
  the gate's history and the push's boundary job reports them. Finding for the Phase 2 report:
  lanes run `check_lane_commits.sh` before filing for review.
- **2.2 Unpark the `cli` placement (dual: the ordinary wire schema changes).** `cli_placement.rs`, the `pub use`, the cli arms and the
  protocol projection leave `gwz_session_candidate`: the ordinary schema gains the placement and
  attachment fields (an additive change: the placement design §3 gate on decoding missing fields
  in Rust and Python runs first; L2-02 regeneration; `check_log_additive` pins); both switch
  inventories (gwz-core 36 session lines, gwz-py 2), `check-cfg` declarations, the workflows'
  session leg, the 29 `reason = "gwz_session_candidate adds fields"` attributes and the "1.2.0"
  comments follow; `docs/TransportPlacement.md` is rewritten (§1.1 lists its lines). The `cli`
  endpoint is a `HostHandle` of the client's owner. After 1.2. ~200 changed + inventories + docs.
- **2.2b Operation service (dual).** The session design's §4.1–§4.2 methods, implemented against
  the §5 clauses the served core adopts: operation records and admission (one workspace per
  server; concurrent clients get the locking two CLI processes get), direct workers (§5.2) for
  direct methods, and the class-W mutator lock (§5.1) for `remote_identity` and the other
  mutators; §5's session host as a client-facing service (the persistent client) stays Phase 7.
  Call encoding of the existing taut requests, dispatch to the core's handlers, events (progress,
  notes), cancel, the §13 schema additions, and the transport lane or long-poll decided in 2.0(a)
  for core-to-client envelopes during a call. Built on `session-contract`'s frames; `SessionOpen`
  without an environment. Red rows: a call with a running `Open` round-trips over the in-process
  carrier; cancel reaches the handler; events arrive before the reply; a transport-lane frame
  before the handshake is a protocol error; cancel during a saturated spool attachment progresses
  (control reserve). After 2.0. ~650 + 500.
- **2.2c Carrier-backed `HostHandle` and its client counterpart (dual).** `gwz-connection::remote`:
  the core-side handle (admit → operation reference; acquire/register as a deferred commit whose
  outcome arrives over the carrier; open → the typed Open; `snapshot()` refused) and the client-side
  receiver that builds the `PreparedOperation` from the client's invocation, runs admission (2.0(c)),
  and creates the binding's endpoint half on the owner. Carrier loss is a terminal `NeverRegistered`
  with zero effects for a pending registration, and a revocation for live bindings. Red rows (fake
  carrier): two operations over the remote handle reuse one fake connection; a core-supplied
  profile or policy field is refused; an unmapped `ExchangeSpec` is refused core-side; a pending
  register whose carrier drops ends `NeverRegistered` with zero effects; `snapshot()` is refused.
  After 1.2 and 2.1. ~400 + 400.
- **2.3 `gwz server` core side (dual).** The server command runs the 1.7 driver-only builder
  behind the 2.1 carrier with the 2.2b operation service: it accepts clients per 2.0, bounds the
  handshake and idles the carrier per the server design, and on carrier loss cancels the whole
  core operation (including its local phases) and records the outcome core-side. `gwz server stdio`
  refuses, in `cli` placement, an inherited `SSH_AUTH_SOCK` or any credential variable. The
  client-side admission of 2.0(c) is implemented in the client endpoint's open path (SSH rows here;
  the HTTPS rows in 2.4). Red rows: a dead client without EOF does not wedge the server; carrier
  drop mid-push → the client reports a possible remote effect, the core records no success,
  nothing replays; a core-chosen destination outside the allowed set is refused with the
  destination named; a receive-pack open under a fetch-class command is refused; a core-named
  identity path is refused; a `SessionOpen` with an environment is refused; stdio with an inherited
  agent socket is refused. After 1.7, 2.2, 2.2b and 2.2c. ~400 + 450.
  Carried from step 2.1's review (2026-10-25, lane owner): 2.3 also owns the sandbox rule's platform
  checks (Linux seccomp and user/mount namespaces, macOS `sandbox_check`, Windows AppContainer; the
  below-Medium integrity refusal and process-identity pinning landed in 2.1's remediation), the address
  walk, `SessionHello` and a bounded `connect`, the 30 s link check, drain-before-exit, the lock file,
  the stale-socket rule and `ws-<key>` naming (replacing 2.1's random `s-` names), the CLI client rows
  the plan's 2.1 text named, and setting the carrier's stall bounds from policy (2.1 provides the
  mechanism with 60 s defaults). `SocketCoreBridge` rows move to 3.2.
  Carried from step 2.2b's review (2026-10-25, lane owner): 2.3 also owns the §5.1 member lock
  manager in the host context (2.0 §3.2 adopts it; 2.2b built only the detached-worker record, so
  fetch beside fetch on one member relies on Git's own lock refusal, and within one session the
  design's "blocks rather than refusing" does not yet hold); and 2.3 calls `Service::open()` before
  it writes `SessionOpened`. The transport lane's routing is 2.1's carrier (one owner; 2.2b's
  duplicate was removed in its remediation).
- **2.4 HTTPS at the `cli` placement (dual).** A design amendment to the placement design's SSH-only
  rule (§2, :96–98) and `docs/TransportPlacement.md:83–84`, carried in 2.0's
  `GwzSplitCommandChannel.md` (historical files are not edited) and reviewed with it, then: the `cli` endpoint is
  the client owner's HTTPS endpoint too, so helper tokens, `gh`, SSPI/CBT and the WinHTTP proxy run
  at the client; policy advertisement, redirects and the 401→helper transition across the carrier;
  admission re-applied at every redirect hop. Red rows: a generic-HTTP open is refused; under
  `WindowsConfigured` an NTLM challenge from a host not allowed gets no answer; a redirect from an
  allowed host to one not allowed is refused before any credential; `WindowsDefault` toward a
  core-named host without the flag is refused; an `ExchangeSpec` outside the smart-HTTP table is
  refused core-side while the client still refuses generic HTTP. ~350 + 350.
- **2.5 R7 and R5 proof, per platform.** CI on ubuntu, macos and windows runners, plus dabeest.
  Common: canary secrets in the client's environment, key file and helper output; the carrier's
  bytes captured and asserted canary-free; agent, Pageant, helper and `gh` **doubles that record
  the caller's PID**, asserting zero calls from the core process; the core's `connect()` count is 0.
  **Separate-user leg** (macOS, Linux, and the hosted Windows runner, which allows a local user):
  the core runs as a separate OS user that cannot read the client's key directory, over the stdio
  form (`sudo -n -u <core-user> gwz server stdio` on a pipe, or the Windows equivalent), which has
  no peer check to bypass; the same-user socket carrier is proven separately by 2.1's rows. On
  dabeest (no account change, by host rule) the Windows leg is the instrumented doubles, a source
  and runtime check that the server binary never launches the gwz-sspi worker or opens the agent
  pipe, and the refusal rows of 1.7; the README states that on Windows the claim is "gwz code
  never uses" and that core-side hooks and filters run with the core user's logon session. R5:
  carrier dropped mid-fetch and mid-push; client cancel reaches the core; core shutdown with a live
  client operation. Adoption of reuse by the `--server` client waits for 1.5, 1.6 and the Phase 1
  GO. ~0 production, ~550 test.
- **2.6 Phase review.** Code and State, with the Surface section reading `gwz server` and
  `--server` cold from `--help`.

Exit: R5 and R7 pass as two processes on three platforms, in CI, with the proof's per-platform
limits stated.

### Phase 3: real consumers (two lanes then one, 11–13 lane-days)

- **3.0 `--json` emits the transport receipts.** `gwz-cli/src/response_meta_json.rs` renders
  `reused`, `connection_id`, `endpoint_id` and `stream_id` from `TransportObservation` (the
  baseline found they are dropped, so the CLI cannot show reuse). ~100 + 80.
- **3.0b The Windows SSH fixture serves receive-pack.** The baseline's `push16` and `bigpush`
  failed on the Windows fixture sshd (plain Git for Windows hangs in `git receive-pack` the same
  way); the fixture is fixed or replaced so the W/SSH push cells have a route to a number. Test
  infrastructure only. ~100.
- **3.1 gwz-cli owns the host.** `globalargs/dispatch.rs` builds one `HostOwner` per command
  through the Phase 1 API, `local` or `--server`; the ~48 runtime-constructing test files move to
  the host API (mechanical). The old entry points stay until 3.3. ~200 + 150 + moved tests.
- **3.2 gwz-py owns the host.** A module-level composition object owns one `HostOwner` (the host's
  registry partitions profiles, 1.5) and hands `HostHandle`s to `ClientHost`; `route::capture` admits through the handle; `Client.close()`
  cancels its own operations only; module shutdown and the atexit join close hosts through the
  cleanup domain. `SocketCoreBridge(address)` as `bridge=` reaches a served core. Red rows: second
  fetch `reused: true`; equal-profile Clients share a host but never a helper environment; a
  Client against a served core authenticates locally. ~300 + 300 Rust, ~100 Python tests. Parallel
  with 3.1 (disjoint repositories; the old entry points still exist).
- **3.3 Retire the per-operation entry points.** `with_local_transport*` and
  `with_cancellable_local_transport*` removed after 3.1 and 3.2 have both landed (gwz-py's CI
  builds against gwz-core main, so the order matters). ~−200.
- **3.4 Measure** R1, R2, R3 on three platforms in both placements against §3.1; evidence run.
- **3.5 Phase review.** Code and State; Surface on gwz-py's public lifecycle.

Exit: R1 and R2 measured through the installed CLI and Python package in both placements.

### Phase 4: settle the library, adopt gwz-credential (one lane, 14–18 lane-days)

- **4.1 Settle the crate.** The crate has lived in `gwz-transport/crates/gwz-connection` since 1.1
  (contract §3; D-C reversed). The step 5.1 capability map (F6) shows the core-hosted alternative
  would need an extra gwz-git-defined port that core implements to keep the repository DAG
  (core → gwz-git → connection → gwz-transport); the contract's placement is the simpler route.
  This step finalizes core's adapters for the four ports, keeps the `ProcessSafety` gate a
  native-adapter capability (contract §7.3), and makes the D52 source check the guard that every
  host in a process shares one gate. The libgit2-facing stream adapters (`ssh_remote.rs`,
  `https_remote.rs`, `stream_io.rs`) are NOT in this crate: they are gwz-git's row G6 (5.4).
  Builds and tests alone (LBT); the kernel crate's no-physical-I/O charter holds, and
  `gwz-connection` depends on it, not the reverse. Phase 3's rows rerun. ~200.
- **4.2 gwz-credential's first consumer.** `ExplicitKeyProvider` behind the Connection–Credential
  boundary's endpoint composition bridge (not a host port) for the explicit `--identity` route:
  re-read-before-lookup maps to `AcquisitionPurpose::BeforeLookup`, `same_material` replaces the
  token comparison (dual). Agent, Pageant and SSPI providers follow as later rows, each only with
  its consumer. ~250 + 200.
- **4.2b D38 exact agent-key selection** keeps its standing in this release (closeout §5): it
  becomes the agent-provider row of the Credential boundary, resumed from the archive's Phase A
  work, with its own dual review. ~400 + 400 (from the archived estimate).
- **4.3 Phase review.** Code and State; standalone crate build on three platforms; the adoption
  gates for `gwz-connection` and gwz-credential recorded (R9).

### Phase 5: the Git boundary (parallel lanes, 30–40 lane-days, ~15 elapsed)

Authority: D-G and the Git boundary plan (a draft the operator adopted by D-G); R9.

- **5.1 Capability map.** Landed in lane p5-gitmap: `gwz-core/dev-docs/GwzGitCapabilityMap.md`
  (rows G1–G10 with classes A/B/C/D, sizes, callers, platform gates, the five adoption-failure
  answers and the proposed 5.5 ratchet). Its findings are carried into the steps below.
- **5.1b G0 guide amendment and gwz-git's own gates.** The gwz-git API gains what the map found
  missing: an environment-honouring open (native commit opens `FROM_ENV`; G0's `open_exact` ignores
  `GIT_DIR`), a revision `resolve`, and a "not a repository" error kind; gwz-git gets its own
  Windows run (none of G1's seven platform gates is a parity-inventory root in core, so they would
  leave core's scan on the move) and its own spawn allowlist (as gwz-transport has). Reviewed with
  5.2. ~150 + 150.
- **5.2 Row A (G9 then G1): error translation and `is_repository`, then native commit and tag →
  gwz-git**, with their characterization tests; core delegates in the same commits. Local Git only.
  About 2,200 production and 4,400 test lines moved, in two or three packages. ~2,200 moved + 150.
- **5.3 Row B (G2): refs and reads → gwz-git.** ~345 moved + 80. G3 (stage, checkout, reset,
  branches), G7 (history cursor), G8 (single-repository diff) and `crates/repo-inspect`'s
  disposition are later rows, recorded in the map, not in this release's Phase 5.
- **5.4 Row C (G5, G6): network operations take the connection capability** (contract §12's
  `run_git_via_gwz_git`, receiving an `Operation` from the composition's `HostHandle`: in-process
  under `local`, carrier-backed under `cli`, D-L, so gwz-git is one code path); the per-remote stream binding and the libgit2-facing stream adapters
  move to gwz-git with it; the libgit2 blocking-stream contract waits under the operation's bounds
  (Git plan G3; `stream_io.rs:246` is the unproved point). After 4.1. About 1,400 moved + 300.
- **5.5 Boundary guard.** An existing source check extended so the migrated area imports no
  `git2` directly; the adoption test that every public gwz-git capability has a core caller. ~150 + 100.
- **5.6 Phase review.** Code and State per two or three rows.

Exit: each chosen row has a production caller and its old path is gone (R9).

Record (2026-10-25): 5.2, 5.3 and 5.5 landed in lane p5-gitmap and passed the Phase 5 review of rows A,
B and the guard (Code and State, two rounds; one remediation patch; accepted at gwz-git `6dcb6b84`,
gwz-core `a6ae99ae`; merged to root). 5.5 also moved `repository_paths`/`repository_index` behind
`reads.rs` (lane owner). Open from the review: the adoption check's same-name receiver credit (Code
P3-2, carried), five unconsumed gwz-git capabilities on the dated ratchet (R9 debt), the CLI route's
environment inheritance (`env_clear()` + `GitEnvironment` snapshot before release). 5.4 waits on 4.1;
5.6 then reviews 5.4 only.

### Phase 6: qualify and release (12–15 lane-days)

- **6.1 Activation.** The archived C3b recipe re-run on main and C4a/C4b; the 62 + 116 clippy
  errors judged (the seven `MutexGuard`-across-`await` sites each); the C3b workflow's sshd
  provisioning (closeout §5) done.
- **6.2 Correctness in shipped behaviour.** The libgit2 config-parse message leak fixed with the
  D50 allowlist at the generic `git_error` mapping and the other paths the D50 run named
  (`preservation_image.rs`, `local_clone/adapters/*`, `operation/commit_log/*`,
  `gitbackend/transport.rs`); D43/D51 resumed as a correctness row with its reader at the client;
  the S5.6 cell table completed for both placements, including the Windows column.
- **6.3 Two-machine evidence.** macOS client → `ssh -a -x -T -o ForwardAgent=no -o
  ClearAllForwardings=yes dabeest gwz server stdio`; Windows client → the same to WSL or weftpi; no
  credential `SendEnv`; the core side asserts no inherited agent socket or credential variable.
  The stdio form carries no listener and needs no pipe forwarding. Evidence, not a supported
  feature in this release.
- **6.4 Help and notes.** `gwz server`, `--server`, `GWZ_SERVER`, the allowed-destination and push
  allowances, the username rule, the per-host default of 32 with its consequences (an sshd with a
  low `MaxStartups` fails early connections at the default and `--max-retries 0` never lowers the
  limit; a confirm-required agent key can raise up to 32 prompts; the HTTPS sweep reached 16, so
  32 for HTTPS is stated as the chosen default, not a measurement, until 3.4's sweep extends it),
  the Python Client lifetime and `SocketCoreBridge`, `reused` in `--json`, what the core never
  holds, the D-E residual.
- **6.5 Release review and publish chain.** Two reviewers (Code and State), with the Surface
  section inside one of them, on the settled tuple; the release plan's Phase 8 order with
  `gwz-connection` published after gwz-transport (D-J); the relscaf scaffolding from the archive;
  the D37 bootstrap.
- **6.6 Post-release check** on three hosts with installed artifacts (R8).

Exit: every §3 row has current evidence; the version is fixed at tag time; R8's two-machine form
is evidence and Phase 7's items are recorded in §10.

### Phase 7: next release (sequenced, not authorized here)

- **A persistent client** (the server design's session host as a client-facing service): a
  per-user process holding `HostOwner`s so successive `gwz` commands reuse connections; the Phase 1
  host is hostable from day one (no process-local state beyond its ports). Carries amendment 2
  §3.18's logon-session must-match row on Windows.
- **A networked core**: `gwz server` on a TCP listener with mutual authentication and encryption of
  the carrier; decision on the mechanism before any listener.
- **Remote file access** through the same seam.

## 5. Lane and merge rules

- Phase 1's trunk is one lane; Phase 2 is one lane with the files named in §4; sub-lanes open
  only after the step that settles their interface has merged.
- Lanes merge serially through the root workspace; when both the lane and main changed a Windows
  trigger path, the merged commit gets its own receipt before anything is pushed.
- Untracked operator files are parked for merges and restored by hash.
- Explicit `CARGO_TARGET_DIR`s under `build-targets`, one or two per lane, deleted at landing.
- dabeest and WSL are booked with the existing lock protocol; only owned processes are stopped.

## 6. Process rules that stop the circles

1. **Code probes before contract changes.** A seam's contract changes only after a probe in code
   shows the need; prose reviews are for scope decisions and for the phase slice. The contract
   amendments this plan already knows about (D-C, D-E) are recorded when their steps land.
2. **One live design per seam, amended in place.**
3. **Adoption gate.** No crate, module or capability is complete without its first production
   consumer in the same package: gwz-connection, gwz-credential, gwz-git alike (R9).
4. **One review per phase**, two reviewers, two remediation rounds, filed verbatim; plus the
   per-step dual reviews marked **(dual)** in §4.
5. **Measured exits**: §3's numbers on three platforms, not a verdict.
6. **Decisions mid-phase only for scope**; preferences are recorded in the phase report.
7. **One live status**: §3 of this document.
8. **Findings outside a step** go to the phase report as findings for a decision.

## 7. What is parked, and where its work is

Preserved in the closeout archive (campaign `workspace-lanes`, run `2026-10-19-lane-closeout`):

- D38 exact agent-key selection: a Credential provider row after 4.2.
- D43/D51 git `http.proxy`: step 6.2, with the reader at the client.
- D52 spawn-gate source check: step 4.1's guard.
- Residual A2 (process-wide launch safety): steps 1.1 and 4.1 under D-E; A3 (Python result
  retention): its own bounded package after Phase 3 (shares `client_host.rs`).
- s63 Windows evidence: step 6.2. C3b's sshd provisioning: step 6.1. Spawn-gate fairness probe: 1.8.
- Step 1.1's first attempt: `pause/p1-trunk.md` in the closeout archive.

## 8. Evidence

Each phase's measurements go to the private evidence member, campaign `transport-qualification`,
one run per phase, with the recipe, raw counts and timings, `manifest.json` and a passing
`verify_archive.py`. The R7 two-process test is public CI; the two-machine run is evidence.

## 9. Cost

| Phase | Lane-days | Elapsed with the parallelism above |
|---|---|---|
| 0 (incl. this plan's review) | 6 | 1 week |
| 1 | 22–27 | 4 weeks |
| 2 | 35–41 | 4–5 weeks, 2.0, 2.1, 2.2b and 2.2c alongside Phase 1 |
| 3 | 11–13 | 2 weeks |
| 4 | 14–18 | 2–3 weeks (4.2b added) |
| 5 | 30–40 | ~3 weeks elapsed, rows A/B alongside 1–4 |
| 6 | 12–15 | 2 weeks |
| **0–6** | **130–160** | **about 15–17 weeks** |

These assume §6 holds. At the rate of 13–19 October, roughly double.

## 10. Decisions

Decided 2026-10-24 (all YES): **D-A** the Connection API contract governs the owner and the
placement design governs the split, `cli` placement in this release; **D-B** the ambient-key
possession proof at cross-binding lease stays; **D-C** `gwz-connection` core-hosted first (**to be reversed, see
below**); **D-D** the first carrier is a same-user local socket or named pipe,
no network listener, two-machine evidence over `ssh … gwz server stdio`; **D-E** the Windows
`ProcessSafety` implementation is a named kernel object shared by every gwz host in the process,
the claim scoped to gwz callers, the third-party-spawn residual recorded (amends contract §7.3;
recorded at 1.1); **D-F** the 1.0.17 config-parse leak fixed in 6.2; **D-G** Phase 0, Phase 1
trunk, Phase 2 when 1.1 merges, Git rows 5.1–5.3 in parallel.

Decided 2026-10-25 (all as recommended):
- **D-C (reversed)** `gwz-connection` lives in `gwz-transport/crates/gwz-connection` from step 1.1,
  as contract §3 selects; no contract amendment. The step 5.1 map (F6) showed the core-hosted
  alternative needs an extra gwz-git-defined port to keep the repository DAG.
- **D-H (b) Trust model for a served core**: the client admits only allowed (scheme, host, port,
  path) tuples, with the service class of its own command, a separate push allowance, a client-
  chosen identity and username, re-applied at every redirect hop, both Windows native policies
  gated; empty default under `--server`, failing closed (2.0(c)). OD16 is re-decided for the split
  accordingly (§1.1).
- **D-I Adopt the parked server design's surface and §4 listener/peer rules**, with the clauses
  §1.1 replaces (credential model, `auto` address and auto-start, server-side registry and
  must-match set, §16 forwarded credentials, §3 snapshot requirement) and its `stdio` and `ssh://`
  client forms.
- **D-J Publish `gwz-connection`** in the release chain after gwz-transport (which itself enters
  the chain per amendment 2 TR3.3).
- **D-K Capability-map decisions**: F2 (the 2026-10-14 "stays a module of gwz-core" line) is
  superseded by D-G; G3, G7, G8 and `crates/repo-inspect` are later rows outside this release's
  Phase 5; G10 stays in core.
- **D-L The served core's connection capability** (Consistency round 3, R3-A1, the architectural
  stop): a carrier-backed `HostHandle` held by the core process, whose admissions and opens cross
  the carrier to the client's `HostOwner`; `admit` carries an operation reference only and the
  client builds the `PreparedOperation`; `open` carries the typed Git-service Open only; `Operation`
  and `Exchange` keep no public constructor; gwz-git receives the same `Operation` under `local` and
  `cli`; pool, reuse and admission policy stay at the client; built in step 2.2c. Amends contract
  §4, §4.1, §5, §7 and §12 as §1.1 records; §3's edges unchanged. The
  alternatives, a gwz-git-defined stream port with two core implementations (map F6) and a proxy
  host in the core, were not taken.
- **D-M Per-host default**: 32, a chosen default; the 2026-10-24 sweep found no GitHub limit up to 32.

Decided 2026-10-25 (the two stops step 2.0's draft raised; operator: "A and replace"):
- **D-N Control priority on byte streams (S1, alternative A).** The session design §3 and the
  byte-stream adapter are amended with a per-end writer that, at each frame boundary, writes the
  oldest control frame (tag 17, and tags 1–3 on the control lane) ahead of ordinary frames, except
  ordinary frames of its own target (same `call_id`, or same `(request_id, session_id, stream_id)`)
  and any `SessionCall` (tag 1), which are never overtaken, so per-target order and the increasing
  call-ID rule survive. Each end's reader never blocks on a consumer: it routes every frame to a
  per-class queue whose bound the peer must respect (the call limit, the control reserve, the
  binding's transport `Limits`), and an overrun is a protocol error of that class. The tag split
  (16 data, 17 control) stands. Alternative B (one FIFO order with a stated bound of N MiB of
  in-band spool ahead of control) was not taken. The text lands in 2.0 (amending
  `GwzCoreSessionDesign.md` §3 in place, dual review) and the code in 2.1 (dual review).
- **D-O gwz-py's `server` family (S2, replace).** The server design's `gwz-py server
  start|stop|status|list|stdio` is replaced: gwz-py is a client only in this release (its
  in-process host, 3.2; its bridge to a served core). No step builds a Python-hosted server; it is
  Phase 7 at the earliest. 2.0 freezes gwz-cli's family and `SocketCoreBridge` only.

Open: none.

## 11. What this plan does not do

It creates no repository, changes no source, merges, pushes, tags or publishes. It adopts no
server programme beyond the server design's surface, `gwz server` on a local carrier and its stdio
form for evidence; it adds no network listener and upgrades no dependency.

## 12. Review closure (revisions 1 → 5)

| Finding | Disposition (revision in which it closed) |
|---|---|
| Safety P0-1 carrier peer authentication | 2.1: server design §4 rules normative; random pipe name, first-instance, reject-remote, SID DACL, peer checks both ways, 0700 dir, macOS dir; squat/remote/other-user/low-integrity rows on three platforms |
| Safety P1-1 trust model | §1 threat model; 2.0(c) client-side admission; D-H (open, recommended (b)); OD16 re-decision in §1.1 |
| Safety P1-2 R7 proves non-use | 2.5 rewritten: separate OS user (M, L), PID-recording doubles, canaries and carrier capture, native route refused (1.7), per-platform limits stated |
| Safety P2-1 / Consistency (HTTPS at `cli`) | 2.4: own step with design amendment and dual review |
| Safety P2-2 carrier message set | 2.0(a): the operation channel defined; no snapshots; which environment each side uses; proxy/CA/helpers client-side; D43/D51 reader at the client |
| Safety P2-3 server stuck states | 2.3: handshake/idle bounds, second-client rule, carrier loss cancels core operation incl. local phases, mid-push row |
| Safety P2-4 helper environment | 1.5: helper environment in the per-operation auth context; R4 row with `GIT_CONFIG_GLOBAL`/`GH_TOKEN` |
| Safety P2-5 / Consistency P2-4 rollover | 1.2: per-operation binding with its own session and local 256 limit; no rollover; stale-event row |
| Safety P2-6 spawn gate per process | 1.1: one gate per process from the composition root; D-E recorded as amending §7.3 with residual; per-process named object |
| Safety P2-7 per-step dual reviews | 1.6, 2.0, 2.1, 2.3, 2.4, 4.2 marked (dual) |
| Consistency P2-1 duplicated surface | D-I; 2.0(b) freezes the surface before 2.3; Python uses `bridge=` |
| Consistency P2-2 operation channel | 2.0(a); `session-contract` and `session-channel` consumed (§2) |
| Consistency P2-3 contract departures | 1.1 four ports + cleanup domain; 1.5 profile in `PreparedOperation` with host registry; 4.1 ports already exist; 4.1 placement back to contract §3 (D-C reversal) |
| Consistency P2-5 phase order vs §13 | ports and fakes in 1.1, so §13.1–2 rows run in 1.8; R1 "owner level" is the fakes row |
| Consistency P2-6 `placement_endpoint.rs`, schema change | §2 corrected; 2.2 re-scoped (projection, additive-decoding gate, gwz-py, inventories) |
| Consistency P2-7 core-only runtime | 1.7 in the trunk |
| Consistency P2-8 R7 on Windows | 2.5 Windows proof stated honestly (doubles and checks, no account change) |
| Consistency P2-9 pipe forwarding | 6.3 uses `ssh … gwz server stdio`; 0.3 checks feasibility |
| Consistency P2-10 pipe as network listener | 2.1 reject-remote and peer rows |
| Consistency P2-11 release identity | §1.1 with exact clauses |
| **Closure check (on revision 4)** | |
| Consistency R4-P2-1 D-L amendment incomplete, no step | §1.1 row extended to §4.1, §5, §7; constructor named (`remote::handle`); step 2.2c; D-L text |
| Consistency R4-P2-2 R1 rule under CAP 32 | §3 R1: creations ≤ min(CAP, members); no creation in push and reads after the fetch |
| Consistency R4-P3-1/2/3 | §9 totals recomputed; workload C size; agreement rule from 3.0; step 3.0b for the Windows SSH fixture; 1.2 leftover sentence removed; §12 header |
| Safety N-1 core-supplied `PreparedOperation` | §1 terms, 2.0(a), 2.2c: admit carries an operation reference; the client builds the profile; refusal row |
| Safety N-2 generic HTTP from core-side gwz-git | §1 terms, 2.0(a), 2.2c, 2.4: typed Open only; closed smart-HTTP table; refusal rows |
| Safety P3-a admission side, `snapshot()` | 2.0(a) and 2.2c |
| Safety P3-b D-M consequences | 6.4 notes; HTTPS sweep extension in 3.4 |
| Safety P3-c username default | 2.0(b) |
| Safety P3-d launch hook placement | 2.0(b): test harness only, under the 2.1 source check |
| **Round 3 (on revision 3)** | |
| Consistency R3-A1 (architectural) served core's capability | Operator decision D-L; §1 terms, §1.1 contract §4/§12 row, 1.2, 2.0(a) walkthrough, 5.4 |
| Consistency R3-P2-1 session design §5 | §1.1 row and 2.2b name the §5 clauses adopted (records, admission, direct workers, class W); re-sized |
| Consistency R3-P2-2 / Safety P3-1 stdio and `ssh://` client forms, Windows launch | 2.0(b) adopts the server design's `stdio` and `ssh://` forms; test-only launch hook for `sudo -n -H -u … env -i` and `CreateProcessWithLogonW`; feasibility in 2.0's list (0.3 landed without it) |
| Consistency R3-P3-1/2/3 | phase headers match §9; stale status text replaced; 2.0(a) wording; 1.2 "endpoint instances"; path exact-or-prefix defined |
| Safety P2-1 admission tuple | 2.0(c): canonicalization, exact-or-prefix, username client-chosen, separate push allowance |
| Safety P2-2 `--server` rows | §3.1 `--server` variant with explicit tuples per workload |
| Safety P3-2 credential variables | 2.0(c): scrub list removed from the child's environment, `-a`; stdio refuses |
| Safety P3-3 control reserve | 2.0(a) constraint; 2.2b row |
| Safety P3-4 service classes | 2.0(b): closed table by request type; unclassified refused; `forall`, `materialize`, `local clone`, `merge --remote`, repository-configured identities |
| Safety P3-5 `gwz.server.allow` scope | keyed by server address; host-only grant documented |
| Baseline findings (0.3) | §3.1 values and D-M; step 3.0 for `--json` receipts; Windows SSH fixture limit recorded |
| **Round 2 (on revision 2)** | |
| Safety P1-1 admission granularity | §1 threat model and 2.0(c): (scheme, host, port, path) tuples, service class from the client's command, client-chosen identity, empty default for `--server`; 2.3 rows |
| Safety P1-2 both Windows policies, redirects | 2.0(c) gates `WindowsConfigured` and `WindowsDefault`; admission re-applied per redirect hop; 2.4 rows |
| Safety P1-3 ssh forwarding | §1.1 server design §16 replaced; 2.3 stdio refuses inherited agent; 6.3 launches with forwarding off |
| Safety P2-1 / Consistency R2-P2-2 snapshot and server-design credential model | §1.1 rows for GWZRequirements:108, GWZDesign, server §3/§4; 2.0(a)/(b): no environment in `SessionOpen`, no `auto`, registry = client profile; 2.1 and 2.3 rows |
| Safety P2-2 where admission lives, no terminal | 2.0(c): the `cli` `HostHandle`, before any credential step; `--allow-destination`, `gwz.server.allow`, `allow=`; fail closed |
| Safety P2-3 / Consistency R2-P2-3 separate-user leg vs peer check | 2.5: stdio form under a separate user; no peer-check override in production (2.1 source check) |
| Safety P2-4 R7 on Windows | 2.5: separate local user on the hosted Windows runner; dabeest leg narrowed to "gwz code never uses"; hooks/filters stated; R7 row reworded |
| Consistency R2-P2-1 operation service | §2 corrected; §1.1 adopts the session design §4.1–§4.2 and frame-tag rules; 2.0(a) decides the transport lane; 2.2b builds the service; costs raised |
| Consistency R2-P2-4 endpoint session host-wide | 1.2: both mux sessions per binding; endpoint-side >256 row |
| Consistency R2-P2-5 §13.1 as built | the crate lives in `gwz-transport/crates/gwz-connection` from 1.1 (no core/git2 edge) |
| Round-2 P3s | binding scoped to its carrier connection (1.2); canonical prompt text (2.0(c)); D-E object rules (1.1); self-minting constructors removed (1.1); 1.5, 1.7, 2.2 marked dual; `SocketCoreBridge` in 2.1 rows; concurrent-client locking (2.0(b)); Pageant-double feasibility (0.3); Windows/HTTP rows moved to 2.4; §3.1 R4 rule; R1 warm basis and Phase-1 row named; Phase 6 exit says R8; "BP" defined; crate map §5; 4.1 reworded with map F6 and D-K; 3.2 one owner per composition; D38 as 4.2b; 2.4's amendment carried in 2.0's document; 1.3 in the trunk |
| P3s (both axes, round 1) | §2 corrected (proxy location, ~48 files, join in `mod.rs`); diagram and dependencies restated; file ownership named; §7 items each given a step; §1 gains the "no old architecture" statement; Phase 5's authority stated and R9 added; Phase 6 exit line; 6.5 two reviewers; "release plan's Phase 8"; contract amendments recorded at their steps (rule 1; D-C withdrawn, so only D-E remains); documentation capacity freed (nine closed review rounds archived to `history/`, 66 active files); 3.1/3.2 ordering via 3.3; control-progress and oversized-length rows in 2.1; §3.18 row in 1.6 and Phase 7; sensitive metadata in 2.0(a); diagnostics loss in 2.0(a); reuse adoption waits for 1.5/1.6/Phase 1 GO (2.5) |
