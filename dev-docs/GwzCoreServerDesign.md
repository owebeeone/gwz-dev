# GWZ core server — design

Date: 2026-09-26. Status: **DRAFT design; review required; no implementation or activation authority.** As of 2026-09-27, the [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md) ships the server in the transport release. Before this design's review, its TR1.3 revises it: reuse in scope, client-side listener verification, the off switch's must-match rule, one agent source per session, and the default. The plan's [amendment](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md) adds to TR1.3: strict address parsing, a sandbox check on the listener, native routes through a server, a stdio mode, and the SSH remote form as a separable section.

This design lets gwz commands run in a long-lived core process that listens on a local socket. There is no new repository or binary. The server is a command of the two CLIs that already embed core:
- **`gwz server --start | --stop | --status`** in gwz-cli;
- **`gwz-py server --start | --stop | --status`** in gwz-py's CLI, which hosts core through its native extension.

Clients reach a server with **`--server auto|ADDRESS`** in either CLI. Python code reaches one with a **`SocketCoreBridge`**, which it passes to `Client`.

It builds on the [core session contract](GwzCoreSessionDesign.md), revision 3. A server hosts the sessions that contract defines, over its byte-stream adapter. The CLIs reach it with the same driver code they use in-process. The design assumes the contract is implemented as far as gwz-cli's move onto the session host (proposals §9, phased-plan items 1, 2 and 5). Nothing here can ship before that.

## 1. Purpose and scope

The contract shows that the client–core boundary is a wire (proposals G3), using a test-only host binary (§12). This design turns that wire into a product path:
- a real second process, reached over a socket, not a test harness;
- both CLI suites run through it, so any behaviour that depends on sharing a process with core shows up as a failure;
- a place where later work can keep state warm across commands.

**In scope:**
- the `server` command in both CLIs;
- the socket host in gwz-core that both commands run;
- the byte-stream handshake;
- `--server` in both CLIs, and gwz-py's `SocketCoreBridge`;
- auto-start;
- Linux, macOS and Windows, with the same functionality, which requires core's transport build on Windows (§11);
- the trust rules, including refusing sandboxed peers;
- the process attributes a server does not share with its clients;
- schema additions, contract amendments and verification.

**Out of scope:**
- **A server on another machine, or under another user's account.** It would hold the client's credentials on another machine; that needs client placement instead (contract §1; proposals P2 and P3).
- **Connection reuse across operations** (contract §1).

**Fixed requirements** (proposals §2):
- **G1** says core "starts no server or daemon itself, and no deployment may require a daemon". Here, core provides a socket host that runs only when a driver's `server --start` asks for it. It starts nothing on its own, and without `--server` both CLIs run core in-process exactly as before. G1's wording should be amended to say so; that is an operator decision (§8).
- **G2 and G3:** the CLIs use the same message path; only the channel changes.
- **G8:** schema additions are append-only.
- **G10:** gwz-py hosts core through its own extension and never runs or bundles the `gwz` executable. A gwz-py client of a gwz-cli-hosted server uses the remote adapter G10 allows.
- **G11:** `Client` is unchanged; `SocketCoreBridge` is one more bridge.

## 2. Where the code lives

| Piece | Repository | Depends on |
| --- | --- | --- |
| Session host, channels, the handshake, `serve_session` (one session over one byte stream), the client socket channel, and **the socket host**: listening, peer and sandbox checks, the lock, the default address, accepting and shutdown; plus the must-match list (§5) | gwz-core | gwz-transport, in transport builds |
| `gwz server` and `--server` | gwz-cli | gwz-core |
| `gwz-py server`, `--server`, `SocketCoreBridge`, and extension bindings for the socket host and the socket channel | gwz-py | gwz-core |

The security-critical code exists once, in core, and both CLIs call it. No repository depends on another driver, and each repository's CI tests only against its own dependencies (§12).

## 3. The wire

**Transport.**
- A Unix-domain stream socket on Linux and macOS, and a named pipe on Windows (§11).
- There is no TCP listener, so no other host and no browser page can reach a server.

**Addresses.** `--server` takes one of:
- `auto`: the default server for this environment (§4);
- an absolute address: a socket path on Linux and macOS, or a pipe name (`\\.\pipe\…`) on Windows.

A relative path is refused, because it would depend on the working directory.

**Frames.** The contract's byte-stream adapter, unchanged (contract §3). Each frame is a little-endian `u32` length, then a tag byte and a deterministic-CBOR body, at most 64 MiB. Tags 1–3 carry calls, replies and errors, as in-process.

**Opening a session.** In-process, `open(options)` is a function call. Over any byte stream, its options travel in the first frame the client sends:

```text
SessionOpen   { protocol: u32 (1), client: str (2), environment: EnvironmentEntry[] (3),
                limits: SessionLimits (4, optional), attributes: ProcessAttributes (5) }
SessionOpened { protocol: u32 (1), server: str (2), core: str (3), server_pid: u32 (4),
                open_sessions: u32 (5) }
EnvironmentEntry  { name: bytes (1), value: bytes (2) }
ProcessAttributes { umask: u32 (1, optional; POSIX only) }
```

- **Tags.** `SessionOpen` is tag 4 and `SessionOpened` is tag 5. Each appears once per connection: `SessionOpen` is the client's first frame and `SessionOpened` the server's. Any other order is a protocol error (contract §3).
- **Every byte stream uses the handshake,** including the contract's stdio test host (§12). A test host and a server therefore open sessions the same way.
- **Opening.** The host validates the frame and applies the session policy (§5). It then calls the same `open(options)` the in-process path uses, passing its host context, and answers `SessionOpened`. Only then does it read calls.
- **Refusal.** A refusal is a `SessionError` with `call_id` 0, after which the host closes the connection.
  - Client call IDs start at 1, so 0 names no call.
  - `call_id` 0 is legal only before `SessionOpened`; after that, it is a protocol error.
  - Requested limits above the host's maxima are refused with `invalid_request`.
- **Versions.**
  - `protocol` is the session protocol version, starting at 1. A host serves a range. A client outside it is refused, and the error gives both versions.
  - `server` names the hosting product and its version, for example `gwz 1.2.0` or `gwz-py 1.2.0`.
  - `core` names the core version and build (ordinary or transport).

**The driver's code does not change.** Both channel kinds implement one client interface: `open(options)`, then `send` and `recv`.
- The in-process channel calls the session host directly.
- The socket channel connects, sends `SessionOpen` and waits for `SessionOpened`.

A driver calls `open` and never learns which kind of channel it has.

## 4. Trust boundary

A server acts with its user's full authority over every workspace that user can write. The rule is therefore: **only the same user, on the same machine, from outside any sandbox, may open a session.**

- **Where servers listen.** On Linux and macOS, in a per-user directory:
  - `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`;
  - the directory is created with mode 0700, and the socket gets mode 0600;
  - the host refuses a directory with another owner, any group or other permission bits, or a symlink;
  - it refuses a path longer than the platform's `sun_path` limit, and says how long the path is.

  Windows uses a named pipe (§11).
- **The default address.** `auto` is `server-<key>` in that directory, where the key covers:
  - the core version and build;
  - a short hash of every value that is on the must-match list (§5) in any build.

  A server started for one environment is found by every client with the same environment and core. A different core version or environment gets a server of its own, instead of a version or environment refusal.
- **Peer check.** On every accept, before reading a byte, the host reads the peer's credentials and closes any connection from another user:
  - Linux: `SO_PEERCRED`;
  - macOS: `getpeereid`, and `LOCAL_PEERPID` for the process ID;
  - Windows: see §11.
- **Sandboxed peers are refused.** A process inside a sandbox must not gain its user's full authority through the server.
  - Limiting the server to certain workspace roots would not confine such a process. Git runs commands named in a workspace's own configuration, such as hooks, `core.fsmonitor` and `core.sshCommand`. A sandboxed process that can write a workspace could therefore still run code outside its sandbox through the server.
  - Refusal is the only sound rule. The host refuses a peer that is:
    - on Linux, in a different mount or user namespace from the host, or under a seccomp filter;
    - on macOS, sandboxed according to `sandbox_check` on its process ID, an interface Apple does not document;
    - on Windows, running in an AppContainer or below medium integrity.
  - If the check cannot be made, the peer is refused. The code is `server_peer_refused`.
- **One server per address.** Before listening, the host takes an exclusive lock on `server-<key>.lock` in the per-user directory (§11 gives the Windows directory).
  - If the lock is held, it exits and says which process holds it.
  - On Linux and macOS, if it gets the lock and a socket file already exists, that file is stale, and the host removes it before binding. A Windows pipe disappears with its server, so nothing is left over there.
- **What a server grants.** An unsandboxed process of the same user gains nothing it couldn't already do by running gwz itself. The sandbox refusal covers the one case where it would gain something.
- **Secrets.** The environment snapshot (§5) is secret-bearing, as the contract says. It crosses the socket once, in `SessionOpen`, over a connection that has already passed both checks.
  - The host never logs it or writes it anywhere.
  - It keeps the snapshot only for its session, then zeroizes it, since a long-lived server would otherwise accumulate every client's secrets.

## 5. Whose process is it?

A command run through a server must behave as the same command run in the client's own process, or be refused. The server's value must never silently replace the client's.

**Two roles.** The contract's "driver" both captures the environment and creates the host context. Over a byte stream those roles split:
- the connecting client supplies the snapshot;
- the process that serves the socket supplies the host context.

**Environment.** The client captures its environment at its edge (contract §5.6) and sends it in `SessionOpen`, and the host passes it to `open` as the session's snapshot. Everything the session host derives from the snapshot then follows the client, per session: the SSH agent socket, TLS roots, proxies, the `gh` environment and every child process's environment. The server's own environment is used for nothing on the session path.

**Process attributes a server cannot hand over.**

| Attribute | Rule |
| --- | --- |
| Working directory | **Forwarded.** Every request that carries `RequestMeta` carries `InvocationContext.caller_cwd`, and core resolves the workspace root and relative operands against it. The host has no directory of its own to fall back to: a request without the context is refused with `invalid_request` before any effect. Every child process core spawns gets an explicit working directory, the repository it acts on, or `/` for the `gh` helper, so none inherits the host's. |
| Paths in requests | **Sent exactly, or not at all.** Request path fields are Unicode text. A client therefore refuses a working directory, root or operand that is not valid Unicode, instead of altering it. That means a non-UTF-8 name on Linux or macOS, or an unpaired surrogate on Windows. The refusal uses `invalid_request` and shows the path with its invalid bytes escaped. |
| Environment variables the session host reads | Forwarded in the snapshot. |
| Variables libgit2 and libssh2 read themselves (contract §5.8): `HOME` and `XDG_CONFIG_HOME` in every build, plus `SSH_AUTH_SOCK` in ordinary builds. On Windows, add `USERPROFILE`, `HOMEDRIVE` and `HOMEPATH` | **Must match.** libgit2 fixes its global-configuration directories from the host's environment when it initializes, and libssh2 reads the agent socket from the host's environment. A differing value is refused; the refusal names the variable, never its value. |
| libgit2's network timeout: process-wide in ordinary builds, and for libgit2's native remotes in transport builds (contract §5.8) | **Must match.** In a server, `configure_transport_runtime` refuses a value that differs from the host's, instead of changing it for every client. Transport builds keep per-session timeouts for their own remotes. |
| File-creation mask (`umask`), POSIX only | **Must match.** Core creates files under the process's mask, which is process-wide. The client sends its own, and a different value is refused. Windows has no process-wide mask; new files inherit their directory's ACL. |
| Terminal | The server has none and needs none. Core's git children already run with captured output and no stdin (`Command::output()`), and the client renders progress from events. |
| Inputs a CLI reads: its stdin, and files relative to its directory | Read by the CLI into the request, as in-process. The server never reads a client's stdin. |
| User and groups | The same user, by the peer check. Groups are the server's as of its start (§14). |
| Resource limits, and the server process's locale | The server's own. Git children get the client's locale variables through the snapshot. |

**How the check runs.** The serving CLI captures its own environment and `umask` once at start, as a driver does, and passes them to the socket host. Core compares each session's values with them, because core owns the list of process-wide reads.

That list is kept beside contract §5.8's disclosure, so the two cannot drift. In `auto` mode the address key already covers it, so a client only meets a mismatch at an explicit address.

**Process-global state is the server's hazard.** One process hosting many clients' sessions turns every process-global input into a leak between clients. The contract's ratchet inventories that state (§5.7).
- A `debt` entry of kind `env` or `process` makes a server session use the server's environment where the in-process command would use the client's. **Neither CLI releases `server --start` while gwz-core's allowlist holds such an entry.** The checker gains a mode that fails on them (§8).
- Today that is 10 entries:
  - 5 environment reads: repo-inspect's `Environment::Process`, the transport binding, `~/` identity resolution, `with_local_transport`'s snapshot and `SshEndpointConfig`;
  - 5 spawn sites: `git tag`, `git commit`, the local-import fetch, `git rev-list` and libgit2's credential-helper spawn.
- The static debt is not a hazard in a server: the SSH helper cap and supervisor, and the HTTPS slots. A server has one host context, so per-process scope and per-host-context scope are the same.

## 6. The `server` command

The same command exists in both CLIs, as `gwz server` and `gwz-py server`. It always runs in the CLI's own process; `--server` names the server it acts on, and defaults to `auto`.

| Command | Behaviour |
| --- | --- |
| `server --start [--foreground] [--max-sessions N] [--idle-exit DURATION] [--log PATH]` | Starts a server at the address and returns once it answers, printing the address. `--foreground` keeps it attached, for launchd, systemd or debugging. |
| `server --stop` | Opens a session and calls `server.stop`, a method only a socket host serves. It then waits up to the close bound plus a margin for the server to exit. If nothing answers, it reports "not running" and succeeds. |
| `server --status` | Reports whether a server answers at the address, with its process ID, product, core version and build, protocol range and open sessions. |

This mirrors Git's own `fsmonitor--daemon` (`start`, `run`, `stop`, `status`), and pairs `--start` with `--stop` as `hook claude-code` pairs `--write` with `--remove`.

**Defaults:**

| Option | Default |
| --- | --- |
| `--server` | `auto` |
| `--max-sessions` | 64 |
| `--idle-exit` | off for `--start`; 10 minutes for an auto-started server |
| `--log` | `server-<key>.log` in the per-user directory; standard error with `--foreground` |

**Starting in the background.** The CLI starts a copy of itself with `server --start --foreground` and the same options:
- gwz-cli runs its own executable;
- gwz-py runs its own interpreter on its CLI module (`gwz.cli`), never the `gwz` executable (G10).

The copy runs in a new session, detached from the terminal, with standard streams redirected to the log. Its environment is the starting CLI's, so its must-match values define its key. The starting CLI waits up to 5 seconds for the address to answer. When two CLIs start the same address at once, the lock makes exactly one of them the server, and the other copy exits.

**Serving.** The serving process is a driver in the contract's sense.
- It creates one host context (contract §5.6) and passes it to every session. Concurrent commands on one workspace therefore queue through the host context's workspace registry, instead of failing on the workspace lock (contract §5.1).
- One thread accepts connections. Each accepted connection gets both checks (§4), then its own thread running `serve_session`. That session runs the contract's reading thread, plus a writer thread that drains its replies to the socket.
- A connection beyond `--max-sessions` is refused at the handshake with `transport_session_full`.
- A stuck client stalls no other client: its replies wait in its own session's bounded queue (contract §3).
- In gwz-py, the accept loop and every session run inside the extension with the GIL released. The extension handles `SIGTERM` and `SIGINT` itself.

**Shutdown.** On `server.stop`, idle exit, or `SIGTERM` or `SIGINT` on POSIX, the server:
1. stops accepting connections;
2. closes every session through the contract's channel-closure path (§8): it cancels live work, waits up to `close_wait`, and detaches what remains;
3. waits up to one more close bound for detached workers, and logs how many are still running;
4. removes its socket and lock, and exits.

`--idle-exit` counts only time with no open session.

**Logging.** One line per event:
- a connection accepted or refused: the peer's process ID and the reason;
- a session opened or closed: the client version and duration;
- an operation: its method, operation ID, outcome and duration.

It never logs environment values, request bodies or credentials. Remote URLs are logged with any user information removed.

**Crash.** Every client sees its channel close and reports that the server closed the session. An operation in flight is left as it would be today if the CLI were killed at that moment. A client never retries a call.

## 7. Clients

**Selection, in both CLIs.**
- `--server auto` uses the default server for this environment, and starts one (§6) if none answers.
- `--server ADDRESS` uses the server at that address, and never starts one.
- `GWZ_SERVER` sets the same default; `--server` overrides it.
- `--no-server` runs in-process even when `GWZ_SERVER` is set.
- With none of these, the CLI runs core in its own process, exactly as today.

If a server was asked for and cannot be used, the command fails; it never falls back to in-process.

**What runs where.** Every command that goes through core's shared dispatch (contract §5.2) goes to the server. A CLI keeps what a driver keeps today:
- argument parsing, help and version;
- rendering;
- the `server` command itself;
- `forall`'s child commands, which need the user's terminal. The CLI gets their targets from the server with `resolve_forall_targets`, then runs the commands itself, holding the workspace guard as it does today.

**One session per command.** A CLI:
1. opens a session;
2. sends its request as contract §11 specifies: a submit followed by event reads when its renderer consumes events, and a unary call otherwise;
3. renders the replies exactly as in-process;
4. sends `session.close` and disconnects.

Output, JSON records, the progress line and exit codes are identical to the in-process run.

**The caller's directory.** A CLI captures its working directory once, makes it absolute, and sends it in every request as `InvocationContext.caller_cwd`. It resolves `--root` against that directory and sends an absolute `WorkspaceRef.root`; without `--root`, core finds the workspace from the caller's directory. Relative operands are resolved against the same directory, so the server never uses its own. Every path is converted exactly or refused (§5). gwz-cli's request building converts lossily today (`to_string_lossy()`) in five places:
- `caller_cwd` and the workspace root in `clirequest/invocation.rs`;
- the workspace root in `globalargs/invocation.rs`;
- a repo `cwd` in `clirequest/repo.rs`;
- a relative path in `clirequest/common.rs`.

Each becomes a refusal. gwz-py's bridge likewise refuses a `str` path holding a lone surrogate, which is how Python represents undecodable bytes from `os.getcwd()`.

**Ctrl-C.**
- The first interrupt sends `operation.cancel` for the running call, waits up to the close bound for its reply, and exits as an interrupted command does today.
- A second interrupt disconnects at once. Channel closure then cancels the session's live work (contract §8).

**Python library.** `Client(bridge=SocketCoreBridge(address))` uses a server, where `address` is `auto` or an absolute address.
- The bridge speaks the same frames as the CLIs, from an asyncio reader task.
- The library never reads `GWZ_SERVER`. Only code that passes a `SocketCoreBridge` uses a server.
- `SocketCoreBridge("auto")` may start a server exactly as the CLI does.

**Errors.** Four new codes are appended to `GwzErrorCode` (§9):

| Code | When |
| --- | --- |
| `server_unavailable` | Nothing answers at the address, or it does not answer as a gwz server, or an auto-start failed. |
| `server_version_mismatch` | The server does not serve the client's protocol version. |
| `server_environment_mismatch` | A must-match attribute differs (§5). The error names the attribute, never its value. |
| `server_peer_refused` | The server refused this process: another user, or a sandbox (§4). |

**Diagnostics.** `--verbose` prints the address and the server's product, core version, build and protocol.

## 8. Amendments

To [GwzCoreSessionDesign](GwzCoreSessionDesign.md):
- **§3.**
  - Tags 4 (`SessionOpen`) and 5 (`SessionOpened`) are added, and every byte-stream channel opens with them. The in-process adapter does not use them.
  - `call_id` 0 is legal only in a refusal before `SessionOpened`.
  - The client interface is `open(options)`, `send` and `recv` for both adapters.
- **§4.1.** Path fields in requests are Unicode text. A client refuses to send a path that is not valid Unicode, instead of altering it.
- **§5.1.** Resolution uses only the request's `InvocationContext` and workspace reference. A request that carries `RequestMeta` without `InvocationContext` is refused with `invalid_request` before any effect. Core's `invocation_start` fallback to a supplied start directory serves only legacy direct callers.
- **§5.6.**
  - The snapshot may cross a byte-stream channel once, in `SessionOpen`, and only over a connection whose peer the host has verified as the same user, on the same machine, outside any sandbox. It is never serialized into any other frame, and it is zeroized when the session ends.
  - Over a byte stream, the client supplies the snapshot and the serving process supplies the host context.
  - Every session-path child process gets an explicit working directory, and never inherits the host process's.
  - The must-match list lives beside the §5.8 disclosure.
- **§5.7.** The checker gains a mode that fails on `debt` entries of named kinds. Releasing `server --start` requires it to pass with `env` and `process` (§5).
- **§9.** The extension also exposes the socket host, for `gwz-py server`, and the client socket channel, for `SocketCoreBridge`.
- **§10.** gwz-py gains `SocketCoreBridge`, which implements the same bridge methods over a socket channel.
- **§11.** gwz-cli may send its session to a server, and may host one (§6 and §7 of this design).
- **§12.** Both CLI suites also run over a socket (§12 of this design), and the stdio test host opens its sessions with the handshake.
- **§16.** The transport build is supported on every platform gwz ships on, Windows included (§11 of this design), so its per-session behaviour holds everywhere.

To the [proposals](GwzClientCoreTransportProposals.md), an operator decision: **G1** reads "Core starts no server or daemon on its own. A driver may host one on request, and no deployment may require one."

## 9. Schema additions (append-only)

- `SessionOpen`, `SessionOpened`, `EnvironmentEntry` and `ProcessAttributes`, with frame tags 4 and 5.
- `SessionLimits`: the contract's §1 limits as optional fields, validated by `open` as today.
- A service method, `server.stop`, served only by a socket host. An in-process session refuses it with `invalid_request`.
- Error codes appended after `operation_expired` (76):
  - `server_unavailable` (77);
  - `server_version_mismatch` (78);
  - `server_environment_mismatch` (79);
  - `server_peer_refused` (80).
- No change to existing messages, or to gwz-transport.

## 10. What a server changes for a user

- **Concurrency.** Commands from several terminals on one workspace queue instead of failing with "workspace mutator lock is already held", because they share the server's host context.
- **Warm state.** The host context's SSH setup supervisor lives across commands. Reusing connections across operations remains out of scope (contract §1), but the server is where it would live.
- **Failure scope.** In-process, a crash or a detached worker affects only its own command. In a server, it affects every client.

## 11. Platforms

Windows has the same functionality as Linux and macOS:
- the `server` command, auto-start, `--server` and `SocketCoreBridge`;
- the handshake, and the trust and sandbox rules;
- the must-match rule, shutdown and logging;
- the transport build, with its per-session SSH agent, timeouts and cancellation.

Only the operating-system primitives differ:

| Function | Linux and macOS | Windows |
| --- | --- | --- |
| Listener | A Unix-domain socket in the per-user directory. | A named pipe, `\\.\pipe\gwz-<user SID>-<key>`. |
| Per-user directory, holding the lock, the log and, on POSIX, the socket | `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`. Mode 0700, owned by the user. | `%LOCALAPPDATA%\gwz\`, owned by the user, with an ACL that grants access only to the user, SYSTEM and Administrators. |
| No other process claims the address first | The private directory and the lock. | `FILE_FLAG_FIRST_PIPE_INSTANCE`, and the lock. |
| No remote clients | Unix sockets are local. | `PIPE_REJECT_REMOTE_CLIENTS`, and a security descriptor that grants only the user's SID. |
| Peer's user | `SO_PEERCRED` on Linux; `getpeereid` on macOS. | `GetNamedPipeClientProcessId`, then the user SID in that process's token. |
| Sandbox refusal | Another mount or user namespace, or a seccomp filter, on Linux; `sandbox_check` on macOS. | An AppContainer token, or integrity below medium. |
| One server per address | `flock` on the lock file. | `LockFileEx` on the lock file. |
| Stale listener | A leftover socket file, removed under the lock. | None: a pipe disappears with its server. |
| Background start | A new session (`setsid`), with streams redirected to the log. | `DETACHED_PROCESS` and `CREATE_NEW_PROCESS_GROUP`, with streams redirected to the log. The copy leaves the caller's job object where the job allows it. |
| Stopping a foreground server | `server.stop`, `SIGTERM` or `SIGINT`. | `server.stop`, Ctrl-C or Ctrl-Break. |
| File-creation permissions | The process `umask`, which must match. | Nothing process-wide: new files inherit their directory's ACL, in a server as in-process. |
| Where libgit2 finds global configuration (must match) | `HOME`, `XDG_CONFIG_HOME`. | `HOME`, `USERPROFILE`, `HOMEDRIVE`, `HOMEPATH`. |
| SSH agent, transport build | `SSH_AUTH_SOCK` from the session's snapshot. | `SSH_AUTH_SOCK` from the snapshot, else the Windows OpenSSH agent's pipe, `\\.\pipe\openssh-ssh-agent`; and Pageant. |

**The transport build on Windows.** Core gates its transport build to Unix today (`cfg(all(unix, gwz_transport_candidate))`). A Windows server would then have only ordinary builds: an SSH agent that must match the server's, process-wide timeouts and no network cancellation. This design requires the transport build on Windows.

The Unix-only code is in core's SSH endpoint, not in gwz-transport, which does no I/O. Each piece has a Windows counterpart:

| Unix code | Where | On Windows |
| --- | --- | --- |
| Non-blocking TCP connect, polled with `libc::poll` (`EINPROGRESS`) | `ssh_network.rs` | `WSAPoll`. |
| The ssh-agent client, a Unix socket polled with `libc::poll` | `agent_socket.rs` | The agent's named pipe, and Pageant. |
| Files opened with `O_NONBLOCK`, so a FIFO can't block the read: `known_hosts` and key files | `ssh_network.rs`, `ssh_key_snapshot.rs`, `identity.rs`, `ssh_worker.rs` | Check that the path is a regular file before opening it. |
| Key-file permission checks by mode (`PermissionsExt`) | key admission | Checks by ACL, with the rule OpenSSH applies there: no access for other users. |
| `known_hosts` and default paths from `HOME` | the transport host's configuration | `USERPROFILE` when `HOME` is unset, as Windows OpenSSH does. |
| The signature buffer handed to libssh2, from `libc::malloc` | `agent_auth.rs` | Allocated with the allocator libssh2 frees with. |
| Byte conversions of paths and environment values (`OsStrExt`, `OsStringExt`) | several | WTF-8, as contract §5.6 already specifies for the snapshot. |

gwz-py's candidate gate, `cfg(all(unix, gwz_transport_candidate))`, lifts with core's.

## 12. Verification

Each repository tests against what it depends on.

**gwz-core.**
- `serve_session` over a socket pair covers:
  - handshake order;
  - `call_id` 0 before and after `SessionOpened`;
  - the version range;
  - must-match refusals;
  - a snapshot that never appears in any frame after `SessionOpen`;
  - zeroization at session end;
  - a request with `RequestMeta` but no `InvocationContext`, refused with `invalid_request` both in-process and over a socket.
- The socket host covers:
  - the directory, owner and permission refusals;
  - the lock and stale-socket handling;
  - the peer check;
  - the sandbox refusal, with a client under `unshare` or bubblewrap on Linux and under `sandbox-exec` on macOS;
  - on Windows: a squatted pipe name, a remote client, an AppContainer client, a low-integrity client, and a per-user directory whose ACL grants another user;
  - the `auto` key.
- The checker's new mode fails on an `env` or `process` debt entry.

**gwz-cli.**
- CI runs its suite twice: once in-process, and once with `GWZ_SERVER` set to a server that the test harness starts per test, from the binary under test, with `server --start --foreground` and that test's environment.
- Any difference between the two runs is a defect.
- In both modes, a working directory whose name is not valid Unicode is refused before anything is sent: non-UTF-8 bytes on Linux and macOS, an unpaired surrogate on Windows. Relative `--root` values and operands arrive at the server as the same absolute paths an in-process run resolves.
- The lifecycle is tested:
  - `--start`, `--stop` and `--status`;
  - two auto-starts racing, which leave one server;
  - idle exit;
  - a different `HOME` under `auto` starting a second server rather than being refused;
  - an explicit address that refuses a different `umask`;
  - `--max-sessions`;
  - shutdown with a latched worker;
  - log redaction.
- A soak runs many client processes against one server, which is the only test that can catch coupling between sessions that the checker cannot see.

**gwz-py.**
- The same two runs of its CLI suite, against `gwz-py server --start --foreground`.
- Its Client-level tests run through `SocketCoreBridge`, beside the existing `NativeCoreBridge` and `StreamCoreBridge` runs.
- Every bridge refuses a working directory that holds a lone surrogate.

**Windows.** Every test above also runs on Windows CI in each repository. So do:
- the transport build's SSH tests, against the Windows OpenSSH agent and against Pageant;
- a background start inside a job object.

**The workspace.** A local run in which each CLI is the client of the other's server, with both repositories checked out side by side. Cross-driver integration happens here, not in either repository's CI.

**Release gate.** Neither CLI releases `server --start` until gwz-core's checker, in its new mode, reports no `env` or `process` debt (§5).

## 13. Relationship to existing documents

- **[Proposals](GwzClientCoreTransportProposals.md) P2.** P2's local form was excluded for gwz-py because it meant running the `gwz` executable (G10). Here gwz-py hosts its own core through its extension, so the local form becomes available to both CLIs. G1 needs the wording in §8.
- **[Session contract](GwzCoreSessionDesign.md).** Unchanged except for the §8 amendments. A serving CLI is a driver. The session, admission, gates, host context, closure and the ratchet all belong to the contract.
- **Client placement** (the transport lane, tags 16–31) stays reserved. A server on another machine would need it.

## 14. Risks and open points

- **Sandbox detection covers the common mechanisms, not all of them.** A sandbox the host cannot see, such as a Linux sandbox built on Landlock alone, is not refused. Such a sandbox must not be given access to the per-user directory. The macOS check relies on an interface Apple does not document; if it disappears, the check fails closed and refuses every peer.
- **Servers multiply.** Each distinct core version, build and must-match environment gets its own auto-started server, until its idle exit.
- **Mixed versions at an explicit address.** A newer client calling a method an older server lacks gets that method refused. `auto` avoids this by keying on the core version.
- **Group changes** take effect only after a server restart.
- **A detached worker** holds its workspace until it ends. With a server, that blocks every client, not one CLI process.
- **A crash** affects every connected client.
- **Job objects.** Where the caller's job object forbids breakaway, as some CI runners and terminals do, an auto-started server ends with its caller's job. A server started with `--foreground` under a service manager is unaffected.

## Changelog

- 2026-09-27: status notes that the transport release ships the server, pending TR1.3 of [`GwzTransportReleasePlan.md`](../gwz-core/dev-docs/GwzTransportReleasePlan.md).
- 2026-09-27: status adds the TR1.3 changes that [`GwzTransportReleasePlanAmendment.md`](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md) requires.
