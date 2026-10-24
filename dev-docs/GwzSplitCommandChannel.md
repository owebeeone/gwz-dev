# GWZ split command channel — operation channel, surface and trust freeze

Date: 2026-10-25. Revision 1 — **ready for dual review** (Consistency with a Surface section; Safety); the operator's decisions D-N and D-O are applied (§2). Revision 0 stopped at two points, S1 and S2. This is step 2.0 of [GwzTransportReleaseConvergencePlan.md](GwzTransportReleaseConvergencePlan.md) (revision 5), items (a), (b) and (c), with the plan's decisions D-H (b), D-I, D-L, D-M, D-N and D-O taken as given. It also carries step 2.4's design amendment (§6). Acceptance freezes this design only; it authorizes no code. Steps 2.1–2.5 implement it.

- **Governing:** the [Connection API contract](GwzConnectionApiContract.md) (as D-L amends §4, §4.1, §5, §7, §12) and the accepted [placement design](../gwz-core/dev-docs/history/reference/GwzRemoteTransportPlacementDesign.md) (as the plan's §1.1 and §8 below amend it).
- **Adopted, as amended here:** the parked [server design](history/parked/GwzCoreServerDesign.md) (surface, §3 handshake, §4 listener and peer rules, §6 lifecycle, §11 platform primitives, §15 stdio, §16 `ssh://` form) under D-I; the accepted [session design](GwzCoreSessionDesign.md) §4.1–§4.2 and the §5 clauses of §3.2 below.
- **Reference only:** gwz-transport's schema (`protocol/transport.taut.py`), `gwz-core/crates/session-contract` and `session-channel`, gwz-cli's command set, gwz-py's `Client` and bridge. No repository other than root dev-docs changes in this step.
- **Terms** are the plan's §1: *core* (driver side, runs Git, holds no credentials), *client* (endpoint side, holds the `HostOwner`, every credential and every Git-host connection), *carrier* (moves frames between them), *served core* (a core reached with `--server`).

## 1. Decisions this document takes where the plan left a choice

| Choice | Decision | Reason |
| --- | --- | --- |
| Core-to-client envelopes during a call (2.0(a)) | **The transport lane: frame tags 16 (transport data) and 17 (transport control)** of the reserved range 16–31; 18–31 stay reserved. No long-poll method. | Placement design §4 forbids holding delivery until the final response. A long-poll method only approximates that: between one poll's reply and the next poll nothing can travel, every client-to-core `Data` would become a call against the 1024-call limit and the increasing call-ID rule, and it adds a service method. The lane carries the existing Envelope unchanged, flow-controlled by gwz-transport's own `Window` and `Limits`, and the tag lets the receiver check the frame's class without decoding. |
| macOS per-user directory (2.0(b)) | `confstr(_CS_DARWIN_USER_TEMP_DIR)` followed by `gwz-<uid>/`; `XDG_RUNTIME_DIR` and `TMPDIR` are not read on macOS; if `confstr` fails the server refuses to start (`server_address_refused`). Linux keeps the server design's order (`$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`). | macOS has no `XDG_RUNTIME_DIR` (Safety round 1 P0-1). The `confstr` directory is per user, created by the system with mode 0700, independent of an inherited environment, and short enough for `sun_path` (103 bytes) with the names of §4.2. A `/tmp` fallback would put the socket in a shared sticky directory. |
| Second client on one server (2.0(b)) | **CLI-equivalent.** Each client gets its own session; sessions share the server's one host context only for the member lock manager (session design §5.1, which waits and never refuses). A workspace write or push from one session that meets another session's write or push gets the cross-process try-lock's existing refusal, exactly as a second CLI process does; there is no cross-session queue. | The plan requires the locking two CLI processes get. The shared member lock only removes a race that two CLIs can hit today; it changes no outcome. Cross-session queuing (the workspace registry's wait rule) belongs to the session host as a client-facing service, Phase 7. |
| Unix socket name | `ws-<key>`, key = the first 16 hex digits of SHA-256 of the core's canonical absolute workspace root, in the per-user directory. Windows keeps the server design's random pipe name, recorded in `ws-<key>.lock`. | One workspace per server (2.2b): a second `server start` for the same workspace finds the running one, and an address-keyed grant (§5.6) survives a restart. The 0700 directory stops other users squatting the predictable name. |
| Windows native authentication flag (2.0(c)) | `--allow-native-auth <https://host[:port]/>` (repeatable), Python `allow_native_auth=[…]`; no configuration key. | The plan requires "an explicit flag" and names none. A configuration key would make the grant implicit. |
| What a terminal prompt may add (2.0(c)) | One exact fetch-class tuple on the scheme's default port, for this invocation only. Never receive-pack, a prefix, a non-default port or native authentication. | The prompt's text is chosen by the core; its reach is kept to the smallest grant. |
| Endpoint diagnostics | Rendered by the client from its own note sink; they never cross the carrier (§3.7). | The endpoint runs in the client, so nothing has to cross and nothing is lost. |
| Python grants | Only the bridge's parameters; the library never reads `gwz.server.*` configuration or `GWZ_SERVER`. | A library's grant is visible in the code that makes it, as the server design already keeps `GWZ_SERVER` out of the library. |
| Admission refusal code | `server_destination_refused` (84), appended to `GwzErrorCode` after `server_session_closed` (83). | The server design's codes 77–83 cover addresses and sessions, not destinations. |

## 2. Decisions applied (the two points revision 0 stopped on)

Both are the operator's decisions of 2026-10-25 ("A and replace"), recorded as D-N and D-O in the plan's §10 and in its §1.1.

- **D-N (revision 0's S1, alternative A): control priority on byte streams.** The session design's "no lanes to bound" and "writes every frame in call order" (`GwzCoreSessionDesign.md` §3 :115; `session-channel` `byte_stream.rs`, README; `session-contract` `channel.rs:18–19`) are amended with a per-end priority writer and a non-blocking reader. The rule, with its bounds, is §3.3 "Byte-stream writer" and "Byte-stream reader"; the cancel-and-close path is §3.1; the session design's §3 is amended in place to match (§8); the tag split of §1 stands. Alternative B (one FIFO order with a stated bound of N MiB of in-band spool ahead of control) was not taken. The code is step 2.1's.
- **D-O (revision 0's S2): gwz-py's `server` family replaced.** gwz-py is a client only in this release: its in-process host (plan step 3.2) and its bridge to a served core, `SocketCoreBridge` (§4.7). No Python-hosted server is built or scheduled; it is Phase 7 at the earliest. This document freezes gwz-cli's family (§4.1) and `SocketCoreBridge` only; the server design's `gwz-py server start|stop|status|list|stdio` is replaced (§8).

## 3. (a) What crosses the carrier

### 3.1 Frames and the handshake

- **Framing:** `session-channel`'s byte-stream framing unchanged (its writer changes, §3.3): little-endian `u32` length, tag byte, deterministic-CBOR body, at most 64 MiB, the length checked before allocation. The tag registry gains tags 16 and 17 on both adapters (`frame.rs:14, :96, :154` change; 18–31 remain a protocol error).
- **Handshake:** the server design's §3 order and bounds (host `SessionHello` first after its peer check; client sends nothing until it holds a well-formed `SessionHello` within 30 s; one `SessionOpen`, or `ServerControl` to a socket host; `SessionOpened`). The client checks only the protocol range, as the server design's SSH form does, then `transport_capabilities` (§4.8): no must-match value exists any more.
- **`SessionOpen` for the `cli` placement carries no environment** (amending `GWZRequirements.md:108`, `GWZDesign.md:104,108`, server design §3 :193–202): `environment` absent, `host_environment` true, `attributes` with `umask` and `openssl` absent and `transport_off` absent or false. Every host in this release, socket and stdio alike, accepts exactly that combination and refuses any other with `invalid_request` before `SessionOpened`. The session then uses the core process's own environment for Git (§3.5). `session-contract`'s `frame.rs:25` comment ("a body can carry the environment snapshot") becomes historical for this placement; the byte-stream reader keeps wiping bodies.
- **Calls:** `SessionCall`/`SessionReply`/`SessionError` (tags 1–3) carry the existing taut requests with their `RequestMeta`/`ResponseMeta` (session design §4.1). In this placement the request's `TransportOptions` carry `placement = cli` and the client's `endpoint_path_base`; `RequestMeta.transport_message` and `ResponseMeta.transport_message` are absent in both directions (a present value is `invalid_request`), because every envelope travels on the transport lane (§3.3).
- **Events and spools:** progress events and notes are read with `events.subscribe`; diff and log output with `diff.output` and `log.output` reads (session design §4.2, §6), each reply at most 1 MiB. Nothing else is pushed by the host on tags 1–3.
- **Cancel and close:** `operation.cancel` and `session.close` on the control lane (session design §3, §4.2). On a byte stream the priority writer of §3.3 writes them ahead of ordinary frames, subject to the rule's exceptions (a `SessionCall` and ordinary frames of their own target are never overtaken), so neither waits behind spool replies written before it.

### 3.2 The operation service: what the served core adopts from the session design

| Session design clause | Served core in this release |
| --- | --- |
| §4.1 frame bodies; §4.2 direct methods, operation methods (`unary`, `submit`), log reads, results, `operation.response/cancel/release`, `session.close`; §4.3 identity; §13 schema additions | adopted unchanged |
| §5.1 records, tokens and gates at receipt (O7); resolution then admission; control frames and reads never wait; classes R/N/W with `remote_identity` in W; the per-session limits (8 running, 64 queued, 128-entry table, 1024 outstanding, 64 control) | adopted; resolution also refuses a request whose workspace root is not the server's (`invalid_request`, naming the served root) |
| §5.1 member lock manager in the host context | adopted, shared by the server's sessions (§1) |
| §5.1 workspace mutator lock (push and W) | adopted as the cross-process try-lock; across sessions it refuses, it does not queue (§1) |
| §5.1 workspace registry: waiting behind another session's W/push | not adopted (Phase 7); kept only to record workers detached at a close bound, so a later W or push on their workspace is refused, not run beside them |
| §5.2 direct workers (8) and one worker per operation; panics | adopted; the worker's transport entry takes an `Operation` from the session's carrier-backed `HostHandle` (§3.4), never building a runtime or reading an endpoint environment |
| §5.3 cancellation, §5.4 retention, §5.5 timeouts, §6 events, §8 closure | adopted; channel closure also revokes the session's carrier bindings (§3.4) |
| §5.6 session context | adopted without the endpoint environment; host context = member lock manager and detached-worker record only (no SSH supervisor, helper slots or endpoint registry: those are the client's) |
| §5 session host as a client-facing service (persistent client holding owners), §11 in-process CLI sessions | Phase 7; the in-process CLI keeps today's dispatch |

### 3.3 The transport lane

- **Body:** a GWZ taut message appended to the schema, `TransportLaneFrame { request_id: str (1), envelope: Envelope (2) }`, where `Envelope` is gwz-transport's, imported through the existing `transport_consumer` route (placement design §3; no second definition, no change to gwz-transport's schema).
- **Tag by kind:** tag 17 carries `window`, `cancel`, `failed`, `bound`, `bind_rejected`, `open_failed`, `identity_checked`, `identity_check_failed`; tag 16 carries `bind`, `open`, `opened`, `data`, `flush`, `flushed`, `end_write`, `close`, `closed`, `check_identity`. A kind under the other tag is a protocol error. Orderly kinds (`closed`, `close`, `end_write`) stay on tag 16 so they never overtake data of their stream.
- **When:** only after `SessionOpened`; a lane frame before it is a protocol error ending the carrier. Both directions.
- **Scope:** `request_id` must name a live call this client sent on this carrier; `Envelope.session_id` is the binding, valid only on the carrier connection and request that bound it (plan 1.2). A frame for a binding of another carrier, or a retired one, is discarded with no state created; an invalid frame for a live binding faults that binding only (contract §6).
- **Bounds:** the lane's queue at each end is bounded by the binding's negotiated `Limits` (`queued_frames`, `queued_bytes`, `receive_window`, `control_reserve_*`), separately from the call lanes; the sender sends `data` only within `Window` credit.
- **Timeliness:** the core's handler never sends an envelope in a reply and never waits for its call's reply to deliver one; the lane is pumped by the carrier's reader and writer independently of operation workers (placement design §4).
- **Byte-stream writer (D-N).** Each end's writer, at each frame boundary, writes the oldest control frame (tag 17, and tags 1–3 on the control lane) ahead of ordinary frames, except ordinary frames of its own target (same `call_id`, or same `(request_id, session_id, stream_id)`) and any `SessionCall` (tag 1), which are never overtaken, so per-target order and the increasing call-ID rule survive.
- **Byte-stream reader (D-N).** Each end's reader never blocks on a consumer: it routes every frame to a per-class queue whose bound the peer must respect (the call limit, the control reserve and the binding's transport `Limits`), and an overrun is a protocol error of that class.
- **Where it applies (D-N).** To every byte-stream carrier: socket, named pipe, stdio and `ssh`, which are every carrier of a served core. The in-process adapter already has a call lane and a control lane (session design §3) and is unchanged by it. The tag split (16 data, 17 control) stands under the rule. This replaces the session design's byte-stream FIFO (§8); step 2.1 implements it and step 2.2b's control-progress rows depend on it.

### 3.4 The served-core composition (D-L)

- **Construction:** for each session the core builds `gwz_connection::remote::handle(binding)`, where `binding` is a constructor argument (the session's lane port and its live-call registry), not a fifth `HostPort`. It is a `HostHandle` with a carrier backing; `Operation` and `Exchange` keep no public constructor.
- **`admit(op_ref)`:** carries only an operation reference, the call's `request_id`. Core-side it reserves a local record with no carrier I/O. **`acquire().await`** waits for core-side bounds only. **`register()`** stays synchronous and infallible and is a **deferred commit**: it enqueues one `bind` (fresh `session_id`, `stream_id` 0, `role = driver`) into a preallocated lane slot and returns an `Operation` whose registration is pending. On receipt the client's receiver resolves `request_id` to the call it sent, builds the `PreparedOperation` itself from that call's captured invocation (route, profile, `AuthContext`, `OperationPolicy`, allowed sets, service class), and runs its own owner's `admit → acquire → register`; it answers `bound` (the client binding is registered) or `bind_rejected` (`capacity`, `invalid_request`, `cancelled`; `Effect::None`). The core's registration becomes `Registered(binding)` on `bound`; on `bind_rejected` it ends terminal `NeverRegistered(ClientRefused(code))`; on carrier loss before `bound` it ends terminal `NeverRegistered(CarrierLost)` with zero effects (the third revocation outcome, amending contract §5). An `open` on a pending operation waits for the outcome first.
- **What the core cannot supply:** a profile, authentication context, policy, retry budget or capacity field. The lane carries none; a `bind` for an unknown, finished or already-bound `request_id`, or whose request's class (§4.5) admits no network, is `bind_rejected(invalid_request)`.
- **`open(spec)`:** the core-side handle maps gwz-git's `ExchangeSpec` to the typed `Open` through this closed table and refuses anything else core-side (`invalid_request`, `Effect::None`, nothing sent):

  | `ExchangeSpec` | `Open.service` |
  | --- | --- |
  | `SshExec`, command `git-upload-pack '<path>'` in gwz-git's own quoting | `upload_pack_exchange` |
  | `SshExec`, command `git-receive-pack '<path>'` | `receive_pack_exchange` |
  | `Http`, `GET <path>/info/refs?service=git-upload-pack` | `upload_pack_advertisement` |
  | `Http`, `GET <path>/info/refs?service=git-receive-pack` | `receive_pack_advertisement` |
  | `Http`, `POST <path>/git-upload-pack`, `application/x-git-upload-pack-request` | `upload_pack_exchange` |
  | `Http`, `POST <path>/git-receive-pack`, `application/x-git-receive-pack-request` | `receive_pack_exchange` |

  Refused: any other method, path suffix, query, command or content type; any header beyond those the endpoint itself sets for the service; a non-empty `auth`; an `http://` origin. The client independently refuses any `Open` that is not one of these `GitService` values (generic HTTP never crosses). The `Open` carries `destination`, `service`, `identity`, `policy`, `deadlines` and `receive_limits`; §5 says what the client accepts of each.
- **Admission** (§5) runs only at the client's receiver, on `bind` and on every `open` and `check_identity`; the core's handle enforces nothing a hostile core could skip.
- **`snapshot()`** on the carrier-backed handle returns a refusal-only value with no counts: the client's pools (which hosts it is connected to) are not the core's to see.
- **Live bindings on carrier loss:** revoked; open exchanges fail `carrier_lost`; a `receive_pack_exchange` that had started reports `Effect::possible`; their connections are discarded, never pooled. Client close or `session.close` cancels the session's bindings; the client's `HostOwner` and its idle connections survive for its next session.
- **Walkthrough, served form** (contract §12 gains this beside the in-process one):

  ```rust,ignore
  // client process: gwz-cli's command, or gwz-py's module composition (plan 3.1, 3.2)
  let (owner, host) = build(config, ports)?;                    // the only HostOwner
  let carrier = connect_verified(&address)?;                    // 2.1: walk, listener check, SessionHello
  let session = open_session(carrier, SessionOpen::no_environment())?;
  let receiver = serve_bindings(&host, &session);               // 2.2c: builds PreparedOperation, admits
  let reply = session.call(fetch_request_with(capture_on_original_entry()?)).await?;
  // core process (gwz server), per session
  let handle = gwz_connection::remote::handle(binding);         // carrier-backed HostHandle
  let op = handle.admit(OperationRef::from(&meta))?.acquire().await?.register(); // deferred commit
  run_git_via_gwz_git(&op).await?;                              // opens become typed Opens on the lane
  let end = op.finish().await;                                  // the client's connections survive
  ```

### 3.5 Whose environment each thing uses

| Core (the serving process's own environment and filesystem) | Client (its captured invocation) |
| --- | --- |
| repositories, worktrees, `.git/config`, global Git configuration for Git behaviour, `url.<base>.insteadOf`, hooks, filters, `core.*`, commit and tag identity, file-creation mask or ACL, locks | known_hosts, the agent (`SSH_AUTH_SOCK`, Pageant, the OpenSSH agent pipe), key files, `--identity`/`--remote-identity` paths and `endpoint_path_base`, the HTTPS helper and its environment, `gh` and its tokens, CA and trust, proxy and no-proxy, the WinHTTP machine proxy, the SSPI logon session, `--max-per-host`, `--ssh-timeout` ceilings, the D43/D51 `http.proxy` reader when it resumes |
| `--root`, `--target`, member paths and `InvocationContext.caller_cwd` describe the core's filesystem | `GWZ_URL_SCHEME`/`--url-scheme`, `--transport`, pager and terminal are the client's, sent as the request fields they fill |

A destination rewritten by the core's `insteadOf` is still only an `Open` the client must admit (§5).

### 3.6 Sensitive metadata that crosses, and why it is acceptable

Nothing listed in contract §7 as secret crosses: no environment, key bytes, agent signature, token, helper input or output, password or OS handle. A core URL carrying a password is refused core-side before any effect. What does cross:

| Item | Direction | Class | Why acceptable |
| --- | --- | --- | --- |
| `Open.destination` (scheme, host, port, path, usernames) | core → client | location | the core holds its workspace's URLs anyway; the client never sends its allowed set back |
| `Identity.key_path`, `path_base` (= `endpoint_path_base`) | both | sensitive metadata (contract §7) | the client sent them in its own request; admission refuses any other path, so the core can only echo them; the server never logs them (server design §6 log rule) |
| `CheckIdentity` result | client → core | one bit, `Effect::None`, no facts | asked only for client-named paths, so it is no file-existence oracle on the client |
| `Opened.facts` (method, `credential_offered`, `authenticated`, `key_fingerprint`, HTTP status, SSH exit status, native facts) | client → core | sensitive metadata (public key fingerprint, mechanism) | needed for `TransportObservation`; public values; never logged by the server |
| `endpoint_id`, `connection_id`, `trust_owner` | client → core | opaque identifiers | no authority (placement design §2) |
| `Failure` code, effect, cause, detail | client → core | diagnostics | no raw stderr or helper output (placement design §6) |
| `SetupFailureCause` (`connection_refused`, `not_found`, `address_not_available`) | client → core | reachability | only for admitted tuples, so it answers no port scan beyond the grant |
| `data` payloads | both | repository content | the content of the core's own repositories |

### 3.7 Diagnostics

Endpoint notes (agent source, proxy, helper, Pageant, retry lines) are produced in the client and rendered by the client from its own sink, to standard error in the CLI and through the sink the `local` placement uses in gwz-py; under `--json`/`--jsonl` they are standard-error lines, not record fields. Driver-side notes and observations reach the client in events and `ResponseMeta` as today. Nothing is lost; the relative order of a client note and a core event is not preserved.

### 3.8 Credential variables never leave the client

- **Scrub list** (closed): `GH_TOKEN`, `GITHUB_TOKEN`, `GH_ENTERPRISE_TOKEN`, `GITHUB_ENTERPRISE_TOKEN`, `GIT_ASKPASS`, `SSH_ASKPASS`, `SSH_ASKPASS_REQUIRE`, `GIT_CONFIG_PARAMETERS`, `GIT_CONFIG_COUNT`, every `GIT_CONFIG_KEY_<n>` and `GIT_CONFIG_VALUE_<n>`, every `GCM_*`. The helper's own variables are covered by the `GIT_CONFIG_*` and `GCM_*` families.
- **Children:** the client removes the list from every `ssh`, `sudo` and stdio child it starts, so `SendEnv` has nothing to send; for the local stdio child and the test-only separate-user launch it also removes `SSH_AUTH_SOCK`. `ssh` keeps `SSH_AUTH_SOCK` for its own login and is started with `-a` (§4.4).
- **`gwz server stdio`** refuses to serve, with a usage error naming the variable, when its environment holds `SSH_AUTH_SOCK` or any listed variable (this also catches a user's `SetEnv` in `ssh_config`, which scrubbing cannot). `gwz server start` refuses the same way, so no served core starts holding one.

## 4. (b) The surface

### 4.1 Commands and selectors

- `gwz server start | stop | status | list | stdio` (server design §6, §17 synopses). Bare `gwz server` prints the subcommands. The root help's "Other" line gains `server`, and its common options gain `--server ADDRESS`.
- `--server <address>` is global, like every selector; `GWZ_SERVER` sets its default; `--no-server` runs in-process even with `GWZ_SERVER` set and cannot be combined with `--server` (server design §7). An empty `GWZ_SERVER` is unset; an empty `--server` is refused. Without any of them gwz runs in-process exactly as today; nothing ever starts or selects a server.
- **There is no `auto` address and no auto-start** in this release: `--server auto` and `GWZ_SERVER=auto` are refused with `server_address_refused`, naming the reason.
- `gwz --server X server start` is a usage error ("server start chooses its address and prints it"); `server start` and `server stdio` neither read nor parse `GWZ_SERVER`. `stop` and `status` take `--server` or `GWZ_SERVER`; `list` and `stdio` take no address. The `hook` family refuses `--server` and never reads `GWZ_SERVER`.
- New client options, accepted only with a server selected: `--allow-destination`, `--allow-push`, `--allow-native-auth` (§5.6).
- **gwz-py has no `server` family (D-O).** The server design's `gwz-py server start|stop|status|list|stdio` is replaced: gwz-py is a client only in this release, through its in-process host (plan step 3.2) and `SocketCoreBridge` (§4.7). No step builds or schedules a Python-hosted server; it is Phase 7 at the earliest.

### 4.2 Addresses and the per-user directory

- **Forms accepted by `--server`:** an absolute socket path (Linux, macOS), a pipe name `\\.\pipe\<name>` (Windows), `stdio`, and `ssh://[user@]host[:port]/absolute/path` (§4.4). `GWZ_SERVER` accepts the first three; `SocketCoreBridge` the first two. The server design's §3 grammar, refusals and walk apply unchanged except for `auto`.
- **Per-user directory:** §1's locations on Linux and macOS (mode 0700, owned by the user, checked under server design §4), `%LOCALAPPDATA%\gwz\` from `SHGetKnownFolderPath` on Windows (server design §11).
- **Address chosen by `server start`:** `<per-user directory>/ws-<key>` on Linux and macOS (socket mode 0600); on Windows `\\.\pipe\gwz-<32 hex>` drawn at each start from the system's cryptographic generator, created with `FILE_FLAG_FIRST_PIPE_INSTANCE`, `PIPE_REJECT_REMOTE_CLIENTS` and a DACL granting only the user's SID, then recorded. The lock file `ws-<key>.lock` (server design §4 file rule and lock) records the process ID, the address, the workspace root and the log path. `start` prints the address, and with `--json` a record `{address, pid, root, log}`.
- `server list` reads every `ws-*.lock` record in the per-user directory and probes each address as `status` does (server design §6 "Listing", without the `auto` mark).

### 4.3 Server lifecycle

- **Start:** `gwz --root <workspace> server start [--foreground] [--max-sessions N] [--idle-exit D] [--log PATH]`, defaults 64 sessions, idle exit off, log `ws-<key>.log` beside the lock (standard error with `--foreground`). It resolves the workspace from `--root` or the current directory and refuses outside one. It runs the plan's 1.7 driver-only builder: no endpoint, no agent, `native` transport refused. It probes first and is idempotent for the same workspace and options (server design §6 "Probing first"); background start, descriptor and session rules as server design §6, §11.
- **Peers:** the server design's §4 peer check, sandbox rule and listener verification apply in both directions, unchanged; no test-only override exists in production code (2.1 source check).
- **Concurrent clients:** each connection is its own session; §1's CLI-equivalent locking; `--max-sessions` refuses the next `SessionOpen` with `transport_session_full`.
- **Stuck peers:** the handshake bounds of server design §3; a session whose carrier accepts no byte for 60 seconds while the host has frames queued for it is closed as carrier loss, so a dead or stopped client without EOF wedges nothing.
- **Carrier loss** runs the session design's §8 channel-closure path (steps 2–5 and 7): every live operation of that session is cancelled, local phases included (its gate refuses at the next crossing), bindings are revoked (§3.4), and each operation's method, ID, outcome and duration are logged core-side. Nothing is replayed; the client reports a push interrupted after its receive-pack exchange began as a possible remote effect.
- **SIGINT/SIGTERM** (Ctrl-C, Ctrl-Break or `server stop` on Windows) run the server design's §6 shutdown: stop opening sessions (keeping the listener to say why), close sessions through channel closure, wait one more bound for detached workers (a second signal or `stop --force` ends that wait), then remove the socket, clear the record, release the lock and exit. Step 4 (endpoint disposal) has nothing to dispose.
- **Stale socket and crash:** the server design's §4 rule unchanged: under the lock, a leftover socket is removed only when it is the user's, a non-blocking connect is refused, and the lock record names exactly that address; otherwise the start refuses and deletes nothing. After a crash clients see `server_unavailable` before `SessionOpened` and `server_session_closed` after it; the next `start` recovers through the stale-socket rule.
- **Client interrupts:** first Ctrl-C sends `operation.cancel` and waits up to the close bound; the second disconnects (server design §7).

### 4.4 The `stdio` and `ssh://` client forms

- **`--server stdio`:** the client starts its own executable as `gwz server stdio` through core's launcher (server design §15: new session, only the stream pipes and standard error cross, interrupt kept from the child), with the §3.8 list and `SSH_AUTH_SOCK` removed; same user, same machine.
- **`--server ssh://[user@]host[:port]/path`** (`--server` only, never `GWZ_SERVER` or configuration; server design §16 grammar, `ssh` resolution on `PATH`, prompts, bounds and error table). **Amended argument vector:** resolved `ssh`, `-a`, `-x`, `-T`, `-o ForwardAgent=no`, `-o ClearAllForwardings=yes`, `-o BatchMode=yes` when no terminal, `-p <port>` when given, `--`, the destination, `gwz server stdio`. The child's environment has the §3.8 list removed. The path is the remote `caller_cwd`. Server design §16 question 4 ("gwz neither turns forwarding on nor turns it off"; `SendEnv` of `GH_TOKEN`) is replaced: the remote core holds no credential and every Git-host connection is made by the client.
- **Test-only launch hook:** the test harness alone may start `sudo -n -H -u <core-user> env -i PATH=… HOME=<core home> gwz server stdio` and, on Windows, `CreateProcessWithLogonW` with redirected standard streams and a generated password. It lives in test code only; the 2.1 source check proves no production binary carries it.

### 4.5 The service-class table (closed, by request type)

The client classifies each request it sends; its class fixes what admission can grant during that call (§5.3). A request type not in this table, including one added later, is refused under `--server` before connecting.

| Request type | Class under `--server` |
| --- | --- |
| `FetchRequest`, `PullHeadRequest`, `PullSnapshotRequest`; `TagRequest` with `op` Fetch, or List with a remote | **fetch**: upload-pack only |
| `PushRequest` (with its preflight and post-push reads); `TagRequest` with `op` Push, or Delete with a remote | **push**: receive-pack for push-allowed tuples, upload-pack for allowed tuples |
| `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `diff`, `diff.output`, `log`, `log.output`, `snapshot`, `capture`, `commit`, `stage`, `stash`, `branch`, `merge` without a family, other `TagRequest`s, `repo_sync`, `add_existing_repo`, `create_repo`, `detach_repo_member`, `attach_repo_member`, `local_family`, `transport_capabilities`, `configure_transport_runtime`, `events.subscribe`, `operation.*`, `session.close` | **local**: no `bind` or `open` is admitted |
| `materialize` | **local**; the core refuses it before any effect when its plan clones |
| `clone_workspace`, `init_from_sources`, `create_workspace`, `clone_repo_member`, `clone_local_workspace`, `remote_identity` (`gwz auth identity`), `merge --remote <family>`, `pull`/`push` whose `--remote` names a local clone family | **refused** (clone-family and local routes; workspaces other than the served one; repository-configured identities) |

`remote_identity` stays class W in the operation service (§3.2); refusing it is the client's table, not the host's.

### 4.6 Other dispositions

- **`forall`:** the client gets targets with `resolve_forall_targets` and runs the commands itself on the paths the core reports, holding the workspace guard as today, over a socket or `stdio`; refused over `ssh://` and the test-only separate-user launch, where those paths are another account's or machine's (server design §16 question 5).
- **Repository-configured identities:** the core does not consult them under `cli`; a planned remote that has one fails before any effect with a message naming `--remote-identity`.
- **`--max-retries`:** refused under `--server` with today's text ("not carried to the Cli placement's endpoint", `cli_placement.rs:56–71`): the endpoint runs the retries and the lane carries no budget. `--transport native`, `GWZ_TRANSPORT=native` and `gwz.transport native` are refused too (plan 1.7).
- **Routes:** `file`, local clone families, `http://` and `git://` are refused before any effect (placement design §2); `init --update` and the `hook` family stay in-process and refuse `--server`.
- **Paths:** `--root`, `--target`, member paths and `caller_cwd` describe the core's filesystem; the CLI resolves `--root` and operands against its captured directory as text (server design §7). `--identity`, `--remote-identity` and `endpoint_path_base` describe the client's: the client sets `endpoint_path_base` to its own captured absolute directory whenever a selected key path is relative.

### 4.7 Python

- gwz-py is a client only (D-O): this bridge is how it reaches a served core, and it has no `server` command family.
- `Client(root, bridge=SocketCoreBridge(address, *, allow=[…], allow_push=[…], allow_native_auth=[…]))`. `Client` gains no `core=` parameter. `root` is a core-filesystem path.
- **Construction** parses the address (socket path or pipe name only; `stdio`, `ssh://` and `auto` raise `server_address_refused`) and every grant (the §5.2 grammar; a malformed one raises `ValueError`); it does no I/O. The bridge takes its `HostHandle` from the module composition object of plan 3.2.
- **Carrier lifetime:** one carrier and one session per bridge, opened on the first call; its pump, call table and errors follow session design §10. Carrier loss closes the bridge for good: outstanding calls fail `server_session_closed`, there is no reconnect, a new bridge is needed.
- **`close()`** sends `session.close`, waits up to the close bound, disconnects and returns the close report; it revokes this bridge's bindings but never closes the module's `HostOwner`, whose idle connections serve the next bridge.
- The library never prompts, never reads `GWZ_SERVER` and never reads `gwz.server.*` (§1).

### 4.8 Capabilities

`transport_capabilities` (placement design §3 fields 3–7): a served core advertises `placements = [cli]` only, schemes `ssh` and `https`, the auth policies its driver may request, its message versions and limits; an in-process core advertises `local` and `cli`. A client refuses a served core that does not advertise `cli` with a common version, before any `bind`. The answer is pinned to the carrier connection; a new carrier asks again.

### 4.9 First day, from `--help` alone

```text
$ gwz --help                 # "Other: auth forall server"; "--server ADDRESS  Run through a gwz server (gwz help server)"
$ gwz help server start      # synopsis, the printed address, the per-user directory, no auto-start
$ cd ~/ws && gwz server start
gwz: server for /Users/alex/ws started (pid 4242)
  address: /var/folders/zz/abc123/T/gwz-501/ws-1f2e3d4c5b6a7980
  use:     gwz --server /var/folders/zz/abc123/T/gwz-501/ws-1f2e3d4c5b6a7980 --allow-destination https://github.com/OWNER/ fetch
  stop:    gwz --server /var/folders/zz/abc123/T/gwz-501/ws-1f2e3d4c5b6a7980 server stop
$ export GWZ_SERVER=/var/folders/zz/abc123/T/gwz-501/ws-1f2e3d4c5b6a7980
$ gwz fetch
gwz: refused: the core asked to fetch from https://github.com/owebeeone/raidzz (upload-pack, port 443), which this command does not allow
gwz: allow it with --allow-destination https://github.com/owebeeone/raidzz, or https://github.com/owebeeone/ for every repository under it
$ gwz --allow-destination https://github.com/owebeeone/ fetch      # output identical to an in-process fetch
$ gwz server stop
gwz: server at /var/folders/zz/abc123/T/gwz-501/ws-1f2e3d4c5b6a7980 stopped
```

The help for `--allow-destination`, `--allow-push`, `--allow-native-auth`, `gwz.server.allow`, the username rule and "a host-only grant is a full grant for that host" is written in 6.4; 2.6's Surface review reads exactly this sequence cold.

## 5. (c) The trust model (D-H (b))

### 5.1 Where and when

Admission lives in the client's receiver of the carrier-backed binding (§3.4), shared by gwz-cli and `SocketCoreBridge`. It runs on every `bind`, `open` and `check_identity`, and again at every HTTPS redirect hop, before `CheckIdentity` work, agent, key file, helper, `gh`, native authentication or connect. A refused envelope gets `open_failed`/`identity_check_failed`/`bind_rejected` with `invalid_request`, `Effect::None` and no facts; the core learns only that it was refused, never the allowed set. The client prints the refusal itself (§5.6).

### 5.2 The tuple and its canonical form

An allowance is `scheme://[user@]host[:port]/path` (exact) or `…/path/` (prefix), or the SCP form `[user@]host:path[/]`; an `Open` is canonicalized by the same function before matching:

- **Scheme:** `ssh` for SCP-like, `ssh://`, `git+ssh://` and `ssh+git://`; `https`. Anything else is refused.
- **Host:** IDNA ToASCII (UTS-46, nontransitional), lowercased, no trailing dot; IPv4 only in canonical dotted decimal; IPv6 bracketed in RFC 5952 form; zone identifiers, empty labels and non-LDH labels refused.
- **Port:** the explicit one, else 22 or 443; compared as a number.
- **Path:** the operand actually sent (libgit2's grammar, never decoded for SSH); a `.` or `..` segment, an empty segment, `\`, NUL, a control character, or a percent-encoded `/`, `\`, `.` or NUL is refused; other escapes compare in uppercase hex, never decoded; one trailing `.git` on the last segment and one trailing `/` on an `Open` path are removed; case is preserved and compared exactly on both schemes.
- **`~` and SCP:** for SSH, `~/` marks a home-relative path and `~user` is refused. An SCP-relative operand (`host:owner/repo`) is home-relative (`~/owner/repo`) and matches only a home-relative allowance, except on gwz's known hosts (`github.com`, `gitlab.com`, `bitbucket.org`, `git/url_scheme.rs:372`), where gwz already treats `git@HOST:o/r` and `ssh://git@HOST/o/r` as one repository and the SCP operand canonicalizes to `/owner/repo`. For HTTPS `~` is an ordinary character.
- **Match:** exact when the allowance ends without `/`; prefix when it ends with `/`, requiring at least one further segment. `ssh://host/` or `https://host/` is a host-only grant: every path on that host and port, a full grant for the host (6.4 says so).

### 5.3 Service class and the push allowance

- The class is the client's own, from its request (§4.5), never the core's. Fetch class admits `upload_pack_advertisement` and `upload_pack_exchange` only.
- `receive_pack_*` is admitted only for a tuple in the separate **push allowance** (`--allow-push`, `gwz.server.allow-push`, `allow_push=`) and only under a push-class request. A push allowance does not imply the fetch allowance; a push needs both for its preflight and post-push reads.
- The core must open every planned member's receive-pack advertisement before its first `receive_pack_exchange`, so a refused push tuple fails the push before any member is pushed (2.3 red row).

### 5.4 Identity, username, policy and limits

- **Identity:** an `Open` or `CheckIdentity` naming `explicit_key` is admitted only when its `key_path` is one the client's invocation named (`--identity`, or a `--remote-identity` value) and its `path_base` equals the `endpoint_path_base` the client sent; `ambient` only when the invocation named none for that request; `credentials_disabled` always. A core-named key path is refused.
- **Username:** SSH: `user@` in the matching allowance, else the client's SSH configuration for that host, else `git`; HTTPS: `user@` in the allowance, else none. A core-supplied `ssh_username` or `https_username` is admitted only when absent or equal to the client's choice.
- **Policy:** `Open.policy` must be one the client's own configuration permits for that destination; the client advertises that set per binding in `bound.policies` (this keeps the anonymous-then-helper HTTPS transition). Windows native policies also need §5.5.
- **Limits:** `deadlines` and `receive_limits` are clamped to the client's own ceilings; `--max-per-host`, its default of 32 (D-M) and pool policy are the client owner's.
- **Ports:** refused unless they match the allowance (§5.2); no prompt grants a non-default port.

### 5.5 Windows native authentication and redirects

- Under either native policy (`windows_configured` or `windows_default`), the logon session answers an NTLM or Negotiate challenge only for an `https` host named by `--allow-native-auth` (or `allow_native_auth=`) that is also allowed; otherwise the client answers no challenge and the exchange fails with `authentication`, `Effect::None`. Every host in a served `Open` is core-named, so this flag is required for native authentication under `--server`.
- Every redirect hop is canonicalized and admitted again, with the same service class and native-auth rule, before any credential, helper or native exchange; a hop from an allowed to a refused tuple ends the exchange. The existing HTTPS-to-HTTP refusal stands.

### 5.6 Supplying the allowed set; prompt; refusal

- **Sources**, unioned per invocation: `--allow-destination` and `--allow-push` (repeatable); `gwz.server.allow` and `gwz.server.allow-push` in the client's global Git configuration, read from the files and with the rules of `gwz.transport` (workspace and member configuration ignored with a note), each value `<server address> <allowance>` (the allowance is the last space-separated token) and applied only when the address equals the one in use, byte for byte after parsing; and the bridge parameters (§4.7). On Windows the pipe name changes at every start, so a configuration grant lasts one server lifetime (6.4 notes it).
- **Default under `--server`:** empty; admission fails closed.
- **Refusal text** (standard error, before the core's own result): `gwz: refused: the core asked to <fetch from|push to> <canonical tuple> (<service>, port <n>), which this command does not allow`, followed by the flag that would allow it. The tuple is canonical, control characters removed, hosts in punycode only. The core maps an `open_failed` or `bind_rejected` with `invalid_request`, `Effect::None` and no facts under `cli` to `server_destination_refused` for the member, so `--json` records carry the code too.
- **Prompt:** only when the client has a controlling terminal (`/dev/tty` opens, or a console is attached), never in gwz-py: the same canonical text, `Allow this repository for this command only? [y/N]`, and §1's limits on what it adds.

## 6. Amendment: HTTPS at the `cli` placement (plan 2.4)

Replacing placement design §2 :96–98 ("Phase 4 cli admits only SCP-like SSH, ssh://, git+ssh:// and ssh+git://"), :132–135 (Phase 4 advertises SSH only; production builds do not advertise candidate placement) and `docs/TransportPlacement.md:83–84` ("Client HTTPS comes later"):

> `cli` admits SCP-like SSH, `ssh://`, `git+ssh://`, `ssh+git://` and `https://`. At `cli` the HTTPS endpoint is the client owner's HTTPS endpoint, so helper tokens, `gh`, SSPI with channel binding and the WinHTTP machine proxy run in the client; the core never runs a helper, reads a token or performs native authentication for a `cli` request. The client advertises its policies in `Bound`; the core's `Open` names a `GitService` and a policy from that set, and the core-side handle maps gwz-git's HTTP exchanges through the closed smart-HTTP table of `GwzSplitCommandChannel.md` §3.4. Redirects and the once-only anonymous 401/404 to helper transition happen at the client endpoint inside the exchange's budgets, with admission applied again at every redirect hop before any credential, helper or native exchange. `http://`, `git://` and file routes stay local-only and fail preflight under `cli`. Production builds advertise `cli` with schemes `ssh` and `https` and the policies the endpoint implements.

## 7. Feasibility probes carried from step 0.3

Unrun, owned by the 2.x step named: (1) `ssh -a -x -T -o ForwardAgent=no -o ClearAllForwardings=yes <host> gwz server stdio` from the Mac to dabeest and to WSL, with `SendEnv` off and the §3.8 list scrubbed, the remote side asserting no agent socket or listed variable (2.3, 6.3); (2) a Pageant double that records its caller's process ID (2.5); (3) the Windows separate-user launch, `CreateProcessWithLogonW` with redirected standard streams on the hosted runner (2.5). A probe that fails is reported with evidence and its effect on R7 or R8.

## 8. Clauses this document amends

Historical and parked files (the parked server design, the placement design) are not edited; this table and the plan's §1.1 are the record (file:line as read 2026-10-25). The session design is active and is amended in place, with a changelog entry, by this step.

| Where | Current text | Change here |
| --- | --- | --- |
| Placement design :12, :17–39, :63–65 | in-process only; "no new command, service method, physical framing, socket between CLI/core" | the carrier, `gwz server` and the transport lane (§3.3) |
| Placement design §4 :181–183 | hooks are "not new RPC methods or an independently serialized wrapper"; `RequestMeta`/`ResponseMeta` are the canonical attachment slots | the served form carries envelopes in `TransportLaneFrame` on tags 16–17 and leaves the meta slots absent (§3.1, §3.3); **not in plan §1.1's line list, implied by its 2.0(a) choice** |
| Placement design :96–98, :132–135; `docs/TransportPlacement.md:83–84` | `cli` is SSH-only; production does not advertise `cli` | §6 |
| `docs/TransportPlacement.md` :3–9, :13–14, :25, :104–126, :139, :150, :188–190, :274–278 | 1.2.0 candidate; "no new CLI command, server address or hosting service"; `install_cli`/`CliEndpoint` | rewritten in 2.2 and 2.4 against this document |
| Server design :35–38, :77–88, §4 :223–330, §6, §15 | `auto` (address and key), auto-start, must-match set, server endpoint registry, socket host requires a snapshot, any-workspace server | no `auto` (§4.1); one workspace per server; addresses per §4.2; no must-match; registry at the client; §1's macOS directory narrows :228 and :799 |
| Server design §3 :193–202, :1170 | socket host refuses a `SessionOpen` without a snapshot; only stdio accepts the marker | §3.1: every host requires no environment and the marker |
| Server design §16 :1172–1178 | forwarding left to the user; `SendEnv` may carry `GH_TOKEN` | §4.4 argument vector and §3.8 |
| `GWZRequirements.md:108`; `GWZDesign.md:104,108` | the snapshot crosses once, in `SessionOpen`; the client supplies the session's snapshot | no environment crosses in the `cli` placement (§3.1, §3.5) |
| `session-contract` `frame.rs:14, :25, :96, :154`; `lib.rs:14`; session design §3 :100 | tags 16–31 reserved and a protocol error; bodies may carry the snapshot | tags 16–17 assigned (§3.3); snapshot comment historical for `cli` |
| Session design §3 :115 (byte-stream adapter); `session-channel` `byte_stream.rs:65,130` and `README.md` ("`send` writes and flushes whole frames, one at a time … It does not use the lane"); `session-contract` `channel.rs:18–19`, `README.md:30`, `lib.rs:19` (frames arrive "in the order sent, whatever their lanes") | a byte stream "writes frames in the order they are sent and has no lanes to bound"; the lane argument is ignored; one order whatever the lanes | **amended (D-N)**: a per-end priority writer and a non-blocking per-class reader on every byte stream (§3.3); the session design is amended in place by this step (text); `session-channel` and the `session-contract` comments and READMEs are step 2.1's (code); recorded in the plan's §1.1 |
| Session design §3 :117, :119 | "The session host only ever sends replies"; nothing streams onto the channel | on tags 1–3 the host still sends only replies; it also sends transport-lane frames on tags 16–17, bounded by each binding's `Limits` and `Window` (§3.3); events are still retained and read by calls; amended in place with the row above (D-N) |
| Session design §3 :100 (tag table) | 16–31 "reserved … unused by this contract" | 16 transport data and 17 transport control assigned; 18–31 stay reserved; amended in place (D-N) |
| Server design :13–14, §6: `gwz-py server start\|stop\|status\|list\|stdio` | gwz-py hosts a server with the same family as gwz-cli | **replaced (D-O)**: gwz-py is a client only in this release (its in-process host, plan step 3.2; its bridge to a served core, `SocketCoreBridge`, §4.7); no gwz-py `server` family is built or scheduled; a Python-hosted server is Phase 7 at the earliest. The parked server design is not edited |
| Session design §5.1 (workspace registry wait), §5.2 transport entry, §5.6 endpoint environment | sessions queue behind each other's writes; each operation builds a runtime from the session's endpoint environment | §3.2 |
| Contract §4 :155, §4.1 :162, §5 :223, §7 :467, §12 :757 (D-L) | `Operation` only where a `HostOwner` exists; `register` does no I/O; `PreparedOperation` captured at the caller entry; one composition walkthrough | §3.4: carrier-backed handle by `remote::handle(binding)`; `admit` carries an operation reference; deferred-commit `register` with `NeverRegistered(CarrierLost)` and `NeverRegistered(ClientRefused)`; `PreparedOperation` built by the client; `snapshot()` refused; served walkthrough |
| Release plan §2 :52, :60–64, §9 :565; amendment 1 :301, :336–337 | `cli` and a separate-process wire out of scope; capabilities advertise `local` only | §4.8 |

The plan's other §1.1 rows (amendment 2's reuse rows, the reuse design, `GwzNoFallbackPlan.md` §6, contract §7.3 under D-E) are recorded by the steps that own them, not here.
