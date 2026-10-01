# GWZ core server — design

Date: 2026-09-26; revision 1, 2026-09-28; revision 2, 2026-09-28. Status: **accepted as a design at SHA-256 `9fc802617c0478a7568baea6e1a9623bb46bed9a0a9375eb83ed9f9647384e85` after [Consistency-1](GwzCoreServerDesign-ReviewConsistency-1.md), [Safety-1](GwzCoreServerDesign-ReviewSafety-1.md) and [Surface-1](GwzCoreServerDesign-ReviewSurface-1.md) reported GO; this accepts the design text only**. This is TR1.3 of the [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md), as its [amendment](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md) extends it.
- This status sentence was added after that GO. So were the corrections the reviewers cleared without a further round, and a narrowing of stale-socket removal that the lane owner decided and all three reviewers confirmed. [Verdict-1](GwzCoreServerDesign-Verdict-1.md) records them.
- Revision 2 applied the [first remediation plan](GwzCoreServerDesign-RemPlan.md) after the [first verdict](GwzCoreServerDesign-Verdict.md), which found revision 1 NO-GO. Revision 1 applied TR1.3; §18 maps each of its clauses, and each finding of the remediation plan, to the section that answers it. The design builds on the [core session contract](GwzCoreSessionDesign.md) at revision 5, as the accepted [connection reuse design](GwzConnectionReuseDesign.md) amends it.
- On 2026-09-28 the operator decided OD12: the SSH remote form (§16) ships in this release. The operator also signed off the corrections this design carries to accepted plan text (§8), and its "On GO" list was applied.
- OD2 and OD9 were decided on 2026-09-27: the server is opt-in (§7), and proposals G1 reads as §1 quotes it.
- Amended 2026-10-01 by [`GwzTransportReleasePlanAmendment-2.md`](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2.md). This document remains authoritative only as amended for the release from which its rule "One agent source per session" governs the in-process transport, and, from that amendment's revision 4, every sentence on Pageant and on the Windows logon session that its §3.18 lists, and, from its revision 5, the routed native operations its §3.19 removes.
- Acceptance authorizes no implementation, activation or release. TR1.4b places this design's steps.
- Erratum, 2026-09-28: §12's Linux walk row said that without `/proc` the client connects by the path. That contradicts §3 and §14, under which the client fails closed; the row now says the client refuses. The session plan's TR1.4b drafter found it.

This design lets gwz commands run in a long-lived core process that listens on a local socket, or in a child process over its standard streams. There is no new repository or binary. The server is a command of the two CLIs that already embed core:
- **`gwz server start | stop | status | list | stdio`** in gwz-cli;
- **`gwz-py server start | stop | status | list | stdio`** in gwz-py's CLI (`gwz-py`, the console script for `gwz.cli`), which hosts core through its native extension.

Clients reach a server with **`--server`** or **`GWZ_SERVER`** in either CLI (§7, §17). Python code reaches a socket server with a **`SocketCoreBridge`**, which it passes to `Client`.

A server hosts the sessions the contract defines, over its byte-stream adapter. The CLIs reach it with the same driver code they use in-process. **What this design assumes first** (plan Phase 7):
- `gwz server`, with its stdio mode, follows the session plan's gwz-cli phase (its Phase 6);
- `gwz-py server`, with its stdio mode, and `SocketCoreBridge` follow the session plan's gwz-py phase (its Phase 4);
- both use the byte-stream adapter (session plan CS1.3);
- the Windows primitives follow the release plan's Phase 4 (1.1.0 S4.1–S4.5);
- reuse through a server follows the release plan's Phase 6.

Nothing here ships before those.

## 1. Purpose and scope

The contract shows that the client–core boundary is a wire (proposals G3), using a test-only host binary (§12). This design turns that wire into a product path:
- a real second process, reached over a socket, not a test harness;
- both CLI suites run through it, so any behaviour that depends on sharing a process with core shows up as a failure;
- a long-lived host context, where connections are reused across commands and clients (reuse design §11).

**In scope:**
- the `server` command family in both CLIs, with its socket host (§6) and its stdio mode (§15);
- the socket host and the stdio host in gwz-core, which both commands run;
- the byte-stream handshake;
- `--server`, `--no-server` and `GWZ_SERVER` in both CLIs, and gwz-py's `SocketCoreBridge`;
- the address grammar, parsed before any open (§3);
- auto-start;
- Linux, macOS and Windows, with the same functionality, which requires core's transport build on Windows (§11);
- the trust rules: refusing sandboxed peers, and verifying the listener from the client side (§4);
- the process attributes a server does not share with its clients, native routes through a server included (§5);
- connection reuse through a server (reuse design; §4, §6, §10);
- schema additions, contract amendments, verification and cells;
- the SSH remote form, as a separable design question (§16, OD12).

**Out of scope:**
- **A server for another user,** and any TCP listener.
- **A server on another machine,** other than the SSH remote form if OD12 brings it in. A remote core that holds the client's credentials needs client placement (contract §1; proposals P2 and P3). The SSH remote form uses the remote account's own credentials (§16).
- **A gwz-py library client** for the stdio mode or the SSH remote form (amendment §3.13). gwz-py's CLI uses a private client for its stdio local form (§15).
- **Client placement:** the transport lane, frame tags 16–31.

**Fixed requirements** (proposals §2):
- **G1,** as the operator adopted it on 2026-09-27 (OD9): "Core starts no server or daemon on its own. A driver may host one on request, and no deployment may require one."
  - Core provides a socket host and a stdio host. They run only when a CLI's `server` command asks for them.
  - Without `--server` or `GWZ_SERVER`, both CLIs run core in-process exactly as before (OD2, §7).
  - The proposals' G1 row takes this wording (§8).
- **G2 and G3:** the CLIs use the same message path; only the channel changes.
- **G8:** schema additions are append-only (§9).
- **G10:** gwz-py hosts core through its own extension, and never runs or bundles the `gwz` executable. Its copies of itself run its own interpreter on `gwz.cli` (§6). A gwz-py client of a gwz-cli-hosted server uses the remote adapter G10 allows: the same typed message boundary, over a socket.
- **G11:** `Client` is unchanged; `SocketCoreBridge` is one more bridge.

## 2. Where the code lives

| Piece | Repository | Depends on |
| --- | --- | --- |
| Session host, channels, the handshake, `serve_session` (one session over one byte stream), and the client ends of the socket and byte-stream channels. The address parser and its walk (§3). Listener verification and the sandbox rule (§4). The must-match set and the `auto` key (§4, §5). **The socket host:** listening, peer checks, the file rule, the lock and its record, the default address, accepting, the control frames and shutdown. **The stdio host** (§15). **The launcher:** starting a copy of a CLI, a stdio child or `ssh`, with the descriptor, signal and session rules of §6, §15 and §16. The SSH remote form's parser, `ssh` resolution and argument vector (§16) | gwz-core | gwz-transport, in transport builds |
| `gwz server` and its subcommands; `--server`, `--no-server` and `GWZ_SERVER`; starting a copy of itself (§6) and spawning `ssh` (§16), both through core's launcher | gwz-cli | gwz-core |
| `gwz-py server` and its subcommands; the same options; `SocketCoreBridge`; the CLI's private stdio client (§15); extension bindings for the socket host, the stdio host, both client channels and the launcher. Starting a copy of itself (§6) and spawning `ssh` (§16): the extension spawns both, through core's launcher, and Python only composes the copy's argument vector (§6) | gwz-py | gwz-core |

The security-critical code exists once, in core, and both CLIs call it. That includes every rule for starting a process, so gwz-py's copies and its `ssh` are spawned by the extension, not by Python's `subprocess`. No repository depends on another driver, and each repository's CI tests only against its own dependencies (§12).

## 3. The wire

**Transport.**
- A Unix-domain stream socket on Linux and macOS, and a local named pipe on Windows (§11). The stdio mode uses a child process's standard streams (§15).
- There is no TCP listener, so no other host and no browser page can reach a server.

**Addresses, parsed before any open** (amendment §3.4). `--server`, `GWZ_SERVER` and `SocketCoreBridge(address)` accept these forms and no others:

| Form | Accepted from | Grammar |
| --- | --- | --- |
| `auto` | all three | exactly `auto`: the default server for this environment (§4) |
| Socket path (Linux, macOS) | all three | starts with `/`; holds no NUL byte; has no empty, `.` or `..` component, and so no trailing `/`; at most 107 bytes on Linux and 103 on macOS, the `sun_path` limits Rust's standard library enforces. On macOS, it does not start with `/.vol/`, `/.nofollow/` or `/.resolve/` |
| Pipe name (Windows) | all three | the exact-case prefix `\\.\pipe\`, then one component, with no `\`, of ASCII letters, digits, `.`, `-` and `_`, not ending in `.`; at most 256 characters in all |
| The stdio local form | `--server` and `GWZ_SERVER`, in the CLIs only | exactly `stdio` (§15) |
| The SSH remote form | `--server` only, and only if OD12 brings it in | `ssh://[user@]host[:port]/absolute/remote/path` (§16) |

- **Refusals.** Everything else is refused with **`server_address_refused`** before any file-system, pipe or network call takes the address. The message names the source (`--server`, `GWZ_SERVER` or `SocketCoreBridge`) and the reason, and shows the address with control characters escaped. Refused forms include:
  - a relative path, which would depend on the working directory, and any word other than `auto` and `stdio`;
  - on Windows, every form that reaches the network redirector (MUP) or another machine:
    - `\\host\…`, `\\host\pipe\…` and `\\localhost\…`;
    - `//host/…`, `\/host…` and `/\host…`, since Windows turns every `/` into `\` first;
    - `\\?\UNC\…`, `\\.\UNC\…` and `\??\UNC\…`;
    - `\\?\GLOBALROOT\…` and `\\.\GLOBALROOT\…`, and `\\;LanmanRedirector\…` and `\\;WebDavRedirector\…`;
    - WebDAV's `\\host@SSL\…`, `\\host@SSL@443\…`, `\\host@8080\…` and `\\host\DavWWWRoot\…`;
    - a `..` inside a local name, as in `\\.\pipe\..\UNC\h\s`, which Windows resolves up to `\\.\`;
    - a drive, as in `\\.\Z:\…`, which may be mapped to a network share;
  - every other spelling of a local pipe: `\\?\pipe\…`, which skips normalization; `\??\pipe\…`, whose root it shares with `UNC`; and `\\.\PIPE\x`, `\\.\pipe\x.`, `\\.\pipe\x ` and `\\.\pipe\a\b`. One spelling per pipe also keeps the `auto` key and address comparisons exact;
  - on Linux, an abstract name (`@name`, or a leading NUL);
  - on macOS, `/.vol/…`, `/.nofollow/…` and `/.resolve/…`, which the kernel rewrites (vfs_lookup.c:459-520, 897-937). Those directories exist, empty, so a walk by components and the kernel's own lookup would reach different objects;
  - a form outside the sources the table gives it: `stdio` or `ssh://` in `SocketCoreBridge`, and `ssh://` in `GWZ_SERVER`;
  - `ssh://` in every source unless OD12 brings the form in.
- **Empty values.** An empty `GWZ_SERVER` is the same as an unset one. An empty `--server` is refused.
- **Windows' checks after the grammar.** The single-component allowlist narrows amendment §3.4's grammar, which admitted several components. Microsoft's own rule for a pipe name is any character but `\`, and the allowlist also keeps out spaces, control characters, look-alike separators such as U+FF3C, and the trailing dots and spaces Windows strips. Then:
  - `GetFullPathNameW` must return the name unchanged. For a `\\.\` name that is string processing alone (inferred);
  - `QueryDosDeviceW(L"PIPE")` must return `\Device\NamedPipe`, so that a `pipe` device redefined in the caller's logon session is not followed. Only unsandboxed code of the same user can redefine it, since Windows does not follow a sandboxed token's links for an unsandboxed caller (CVE-2015-2428), so this is defence in depth;
  - the grammar is the only guard against a remote name: Windows ignores `SECURITY_IDENTIFICATION` when a pipe connection is remote (Microsoft, "Impersonation Levels").
- **The walk, on Linux and macOS.** After the grammar, the client walks the absolute path from `/`, one component at a time. It refuses the path when a component is an automount trigger, an automounter's mount or a network file system, before it looks up the next component. That order matters: on both systems, looking up any name inside an automounter's directory asks the automounter, whatever the flags (Linux fs/autofs/root.c:530-544; macOS autofs `auto_lookup`). No probe follows a symbolic link or triggers a mount (amendment §3.4), and a refused component's descriptor is never used for the next lookup.
  - **Linux** (sources at v7.2):
    1. The walk starts from `open("/", O_PATH | O_DIRECTORY | O_CLOEXEC)`. It opens each single component with `openat(parent, name, O_PATH | O_NOFOLLOW | O_CLOEXEC)`: never a name of several components, never with `O_DIRECTORY`, never with a trailing `/`, each of which would trigger a mount. Such an open stops at a trigger without mounting it (namei.c:1538-1566; open.c:1270).
    2. `statx` on that descriptor, with `AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW | AT_NO_AUTOMOUNT`, does no second lookup. A component that carries `STATX_ATTR_AUTOMOUNT` is refused. That flag marks the kernel's own automount points: NFS mount points and referrals, AFS mount points, SMB DFS junctions, FUSE submounts and debugfs's. It never marks an autofs trigger (stat.c:200-201; autofs_i.h:193-195).
    3. `fstatfs` on the descriptor gives the file-system type, and a listed type is refused. Only this check catches autofs, so both checks stay.
    4. A symbolic link on procfs (`0x9FA0`) is refused, because a magic link's text need not name what the kernel resolves (inferred). Any other link is read with `readlinkat(fd, "")`.

    `openat2` with `RESOLVE_NO_XDEV` is not a safe probe before Linux 6.18, where it may still trigger a mount (commit `042a60680de4`).
  - **macOS** (sources xnu-12377.121.6 and autofs-322; probed on macOS 26.6.2):
    1. The walk starts from `open("/", O_SEARCH | O_DIRECTORY | O_CLOEXEC)`. It probes each component first with `getattrlistat(parent, name, …, FSOPT_NOFOLLOW)`, for its type, file-system ID, file ID and mount status. That is a get-attributes lookup, which never mounts (vfs_attrlist.c:3589). A component whose mount status carries `DIR_MNTSTATUS_TRIGGER` is refused.
    2. Only a component that the probe showed to be a link is read, with `readlinkat`: `readlink` on a trigger would mount it.
    3. Only a directory that the probe showed not to be a trigger is opened, with `openat(parent, name, O_SEARCH | O_NOFOLLOW | O_CLOEXEC)`. `fstatfs` and `fgetattrlist` on its descriptor must show the file-system ID and file ID the probe saw. The component is refused when its mount is not `MNT_LOCAL`, is `MNT_AUTOMOUNTED`, or has a listed type name. The open uses `O_SEARCH` because `O_RDONLY` gets `EPERM` on auto_home's root.
    4. File-system IDs are never matched to the mount table by `st_dev`, which differs from the mount's ID across the system and data volumes.

    Any `open`, whatever its flags, looks the name up for opening, and Apple's autofs resolver does not spare an open at the last component (vfs_vnops.c:590-592; autofs triggers.c:335-510). So `open` never comes first. The amendment's named probe, `open` and then `fstatfs`, would mount a direct-map trigger; §8 corrects the amendment's text.
  - **Links.** A symbolic-link component is read, not followed. Its target is made absolute against the link's directory, whose walked path holds no link, so the target's `.` and `..` resolve there as the kernel's lookup would; the result is walked from `/` under the same rule. A walk reads at most **8 links** in all, links inside targets included, and a ninth refuses the address.
    - Links are walked rather than refused, because `/var`, `/tmp`, `/etc` and `/home` are links on macOS, and `TMPDIR` lives under `/var/folders`.
    - Eight covers those with room, and stays well below the kernels' own limits: 40 on Linux, 32 on macOS.
  - A missing component ends the walk without a refusal: nothing answers there yet. For `auto`, the host then creates the per-user directory (§4).
  - **The types it refuses, and how a trigger presents:**

    | | Linux (`f_type`, magic.h v7.2) | macOS |
    | --- | --- | --- |
    | Refused | autofs `0x0187`; NFS `0x6969`; smbfs `0x517B`, CIFS `0xFF534D42` and SMB2 and 3 `0xFE534D42`; OpenAFS `0x5346414F` and kAFS `0x6B414653`; Ceph `0x00C36400`; 9p `0x01021997`; Coda `0x73757245`; NCP `0x564C`; OCFS2 `0x7461636F`; GFS2 `0x01161970`; OrangeFS `0x20030528`; vboxsf `0x786F4256`; every FUSE file system, `0x65735546`, fuseblk and virtiofs included; and, out of tree (coreutils `stat.c`), Lustre `0x0BD00BD0`, GPFS `0x47504653`, BeeGFS `0x19830326`, PanFS `0xAAD7AAEA`, IBRIX `0x013111A8`, StorNext `0xBEEFDEAD`, Oracle ACFS `0x61636673`, Parallels `0x7C7C6673` and VMware HGFS `0xBACBACBC` | a mount that is not `MNT_LOCAL`, or is `MNT_AUTOMOUNTED`; one whose `f_fstypename` is `autofs`, `nfs`, `smbfs`, `afpfs`, `webdav`, `ftp`, `acfs`, `virtiofs`, `fusefs`, `osxfusefs`, `osxfuse` or `macfuse`, or starts with `macfuse_` or `osxfuse_`; and, for the socket's own directory, `MNT_IGNORE_OWNERSHIP`, where owner checks mean nothing (inferred) |
    | A direct-map trigger | autofs `0x0187`, with `STATX_ATTR_MOUNT_ROOT` and without `STATX_ATTR_AUTOMOUNT`. The open stops at it with no mount | an autofs mount at its own path, `MNT_DONTBROWSE` and `MNT_AUTOMOUNTED`, not `MNT_LOCAL`, whose mount status is `DIR_MNTSTATUS_MNTPOINT` and `DIR_MNTSTATUS_TRIGGER` (automount.c:256-257; vfs_attrlist.c:2165-2190). From source: building one needs root |
    | An indirect map's root, such as `/net` | autofs `0x0187`, never a trigger (autofs inode.c:349-354). Any new name inside it reaches the daemon | an autofs mount point, never a trigger. Any name inside it asks automountd, which for `-hosts` resolves the host. `/net` is off by default: `/etc/auto_master` has shipped it commented out since autofs-281.0.3. The default automounter mount is auto_home, at `/System/Volumes/Data/home`, which `/home` links to |

    - **Every FUSE file system** is refused on Linux. Its subtype, such as `fuse.sshfs`, is chosen by whoever mounts it and shows only in `/proc/self/mountinfo`, so nothing trustworthy tells sshfs or rclone from a local FUSE file system.
    - **The macOS names are checked as well as the flags,** because macFUSE's `-o local` sets `MNT_LOCAL`, and its `fstypename=NAME` gives `macfuse_NAME`.
  - **What it covers.** An explicit address; the path `auto` derives from `XDG_RUNTIME_DIR` or `TMPDIR` (§4); and `--log PATH` (§6). An unset or empty variable moves to the next choice. A set value that the grammar refuses is refused, not skipped, so an unread environment cannot move the address silently. The host runs the same parse and walk before it creates the per-user directory or binds.
  - A walk refusal uses `server_address_refused`, naming the component and its type.
- **The walk and the connect.** `connect` resolves the path again, so a writer on the path could swap a component between the walk and the connect. Each platform narrows that gap:
  - **Linux:** the walk ends holding an `O_PATH` descriptor of the socket file itself, and the client connects to `/proc/self/fd/<n>` for it, which reaches that inode without resolving the path again (inferred; §12 tests it). The host binds through `/proc/self/fd/<n>/<name>` for the walked directory, likewise (inferred). Where `/proc` is not mounted, the sandbox check cannot be made either, so no client connects and no host binds (§14).
  - **macOS:** there is no `connectat`. A CLI, whose process is its own, holds `/dev/autofs_notrigger` open from the start of the walk until `connect` returns. That makes its process, but not its children, one that resolves no trigger. The serving CLI does the same around its walk and `bind`. Where the device cannot be opened, `setiopolicy_np` at process scope does the same. `SocketCoreBridge`, inside an application's process, takes neither, since both suspend automounts for the whole process; the gap stays open there. macOS 26's private `/.resolve/` prefix would close the gap in the kernel, at the cost of a private interface and 11 bytes of `sun_path`; this design does not use it.
  - **Windows:** there is no walk. `CreateFileW` resolves the name once.
  - **The residual.** Where the gap stays open, a swapped component can make the connect trigger a mount, or ask an automounter, which may contact a network host. It cannot make the client send anything: the client sends no byte before listener verification passes and the host has identified itself (§4). The residual is an unwanted mount or network contact, not disclosure.
- **Windows needs no walk.** The grammar admits one local name in `\\.\pipe\` and nothing else. §11 says how the host checks the directories it opens there.
- **One type.** The parser returns a `ServerAddress` value: auto, a socket path, a pipe name, stdio, or an SSH destination. Every connector, the host's binder, the stdio launcher and the SSH launcher take only that type, so no code path opens an unparsed string.

**Frames.** The contract's byte-stream adapter, unchanged (contract §3). Each frame is a little-endian `u32` length, then a tag byte and a deterministic-CBOR body, at most 64 MiB. Tags 1–3 carry calls, replies and errors, as in-process.

**The handshake.** In-process, `open(options)` is a function call. Over any byte stream the host speaks first, with a frame that identifies it and carries no secret. The client's options travel in the one frame it sends after that:

```text
SessionHello  { protocol_min: u32 (1), protocol_max: u32 (2), server: str (3), core: str (4),
                server_pid: u32 (5) }
SessionOpen   { protocol: u32 (1), client: str (2), environment: EnvironmentEntry[] (3, optional),
                limits: SessionLimits (4, optional), attributes: ProcessAttributes (5),
                host_environment: bool (6, optional; absent means false) }
SessionOpened { protocol: u32 (1), server: str (2), core: str (3), server_pid: u32 (4),
                open_sessions: u32 (5) }
ServerControl { action: ServerAction (1), force: bool (2, optional; absent means false) }
ServerState   { server_pid: u32 (1), open_sessions: u32 (2), max_sessions: u32 (3),
                idle_exit: u32 (4, optional; seconds, absent means off),
                log: str (5, optional; absent means standard error), stopping: bool (6) }
ServerAction  = status (0) | stop (1)
EnvironmentEntry  { name: bytes (1), value: bytes (2) }
ProcessAttributes { umask: u32 (1, optional; POSIX only),
                    transport_off: bool (2, optional; absent means false),
                    openssl: str (3, optional; Linux and macOS only) }
```

- **Tags and order.** `SessionOpen` is tag 4, `SessionOpened` 5, `SessionHello` 6, `ServerControl` 7 and `ServerState` 8.
  1. The host sends `SessionHello` first: a socket host once its peer check passes (§4), a stdio host at once.
  2. The client sends nothing until it holds a well-formed `SessionHello` whose versions it accepts. It then sends exactly one frame: `SessionOpen`, or, to a socket host, `ServerControl`.
  3. The host answers `SessionOpen` with `SessionOpened`, or `ServerControl` with `ServerState` and then closes the connection. Either may be refused instead (below).

  Each frame appears at most once per connection, and any other order is a protocol error (contract §3). The host waits at most 30 seconds for the client's frame, then closes the connection.
- **A well-formed `SessionHello`.** The client reads the first frame's length and refuses one over 4 KiB before it reads on, so bytes that are not a frame, such as a shell's banner, fail at once. The frame must carry tag 6 and decode. A first frame that is not a well-formed `SessionHello` fails with `server_unavailable`, which shows its first bytes with control characters escaped (§17).
- **The handshake's bound.** The client waits at most 30 seconds for `SessionOpened`, and at most 5 seconds for `ServerState`, which only the `server` subcommands ask for (§6). Each bound counts from the start of the client's connect, or of the stdio child, and so covers the connect itself.
  - **Every client-side connect is non-blocking:** a command's, those of the `server` subcommands and `list`, and the host's own stale-socket check, which has 5 seconds (§4). On Linux a connect to a full accept queue fails at once (`EAGAIN`), and is retried until the bound; on macOS it is refused at once (`ECONNREFUSED`, inferred); on Windows a busy pipe (`ERROR_PIPE_BUSY`) is waited for with `WaitNamedPipeW`, for the time left.
  - **At the bound** the client disconnects, or kills the child, and fails with `server_unavailable`, naming the process. For a socket that is the listener's process ID from the kernel (§4), or, when the connection was never accepted and the kernel gives none, the one the lock file records, marked as from its lock file; then the address's lock file and the log path it records. For a stdio child it is the child's process ID. A connect that never completes is reported as a silent listener is.
  - **On macOS** a full queue is refused just as a stale socket is, so there the client reports that nothing answers, and adds the process ID and log path the lock file records, if it has a record.
  - **The lock file** is read under §4's file rule. The client only shows what its record says, escaped, and opens nothing the record names.

  The bound covers `SessionOpened` as well as the `SessionHello`, since a host whose sessions hang in `open` still sends its `SessionHello`. The SSH remote form's bound is §16's.
- **Versions and identity.**
  - `protocol_min` and `protocol_max` give the range of session protocol versions the host serves, starting at 1. A client that speaks none of them refuses with `server_version_mismatch`, giving both, and sends nothing. The host also refuses a `SessionOpen.protocol` outside its range.
  - `server` names the hosting product and its version, for example `gwz 1.1.0` or `gwz-py 1.1.0`, and `core` names the core version and build (ordinary or transport).
  - On the session path, the client refuses a `core` whose version or build differs from its own, with `server_version_mismatch`, before the snapshot crosses: the socket channel before its `SessionOpen`, and the stdio local form's client likewise. A host of another core may compare fewer process-wide values than the client's core lists (§5), and could not refuse what the client's own core would. Under `auto` the key includes both, so there the refusal arises only from a digest collision.
  - The `server` subcommands' `ServerControl` exchange checks only the protocol range, since it compares nothing, so `list`, `status` and `stop` reach a server of another core version or build (§6). A server whose protocol range does not include the client's version cannot be asked anything: `list` shows it as refused, with its range and process ID, and `status` and `stop` fail with `server_version_mismatch`, naming the process (§17).
  - The SSH remote form checks only the protocol range: its session takes the remote process's own environment, so no must-match value crosses (§16).
  - `SessionOpened` repeats the host's identity; a difference from its `SessionHello` is a protocol error. `open_sessions` counts the host's other open sessions. A stdio host reports 0.
- **The environment's source.** `environment` is the client's snapshot (§5). `host_environment` is the SSH remote form's marker (§16): the client sends no environment, and the session uses the host process's own. An empty `environment` is a snapshot with no entries, never the marker. The host checks the combination before anything else (amendment §3.4 question 3, §3.6):

  | Host | `environment` | `host_environment` | `umask` and `openssl`, on POSIX | Result |
  | --- | --- | --- | --- | --- |
  | socket | present | false or absent | present | the must-match check (§5) |
  | stdio | present | false or absent | present | the must-match check, against the stdio process's own values |
  | stdio | absent | true | absent | the session uses the stdio process's environment, mask and OpenSSL, as they were at its start |
  | any | any other combination | | | refused with `invalid_request` before `SessionOpened` |

  A socket host therefore refuses a `SessionOpen` that carries no snapshot, or that carries the marker.
- **`transport_off`** is the client's resolved off switch (§5).
- **`openssl`** names the OpenSSL library the client process loaded, as `OpenSSL_version` reports its version, OPENSSLDIR, ENGINESDIR and MODULESDIR. It is on the must-match set (§5): a gwz-py wheel may bundle an OpenSSL other than the CLI's (inferred), and the library decides the default configuration file and CA locations.
- **Opening.** The host validates the frame and applies the session policy (§5). It then calls the same `open(options)` the in-process path uses, passing its host context, and answers `SessionOpened`. Only then does it read calls.
- **Control.** `ServerControl` needs no session, so no must-match check applies to it: the peer check (§4) is its whole policy, as it is for a process that could signal the server. Only a socket host serves it, so `server stop` and `server status` reach a server of any environment, such as one `server list` shows (§6).
  - `status` asks for `ServerState`.
  - `stop` asks the server to shut down (§6). Its `ServerState` answers before shutdown begins, with `stopping` true. `force` ends shutdown's wait for detached workers, whether it comes with the first stop or a later one.
  - A stdio host refuses `ServerControl` with `invalid_request`.
- **Refusal.** A refusal is a `SessionError` with `call_id` 0, after which the host closes the connection.
  - Client call IDs start at 1, so 0 names no call.
  - `call_id` 0 is legal only before `SessionOpened` or `ServerState`; after `SessionOpened`, it is a protocol error.
  - A socket host's maxima for `SessionLimits` are the contract's §1 defaults: a limit above its default is refused with `invalid_request`, and one at or below it goes on to `open`'s own validation. A stdio host applies that validation alone, as in-process `open` does.
- **Every byte stream uses the handshake:** the socket host, the stdio mode (§15), the SSH remote form (§16) and the contract's stdio test host (§12). The contract's test bridge, `StreamCoreBridge`, sends `umask` and `openssl` as `SocketCoreBridge` does, through the extension, and the test host checks them. The marker stays the SSH remote form's alone.

**The driver's code does not change.** Every channel kind implements one client interface: `open(options)`, then `send` and `recv`.
- The in-process channel calls the session host directly.
- The socket channel parses, walks, connects, verifies the listener (§4), reads the `SessionHello`, sends `SessionOpen` and waits for `SessionOpened`, all within the handshake's bound.
- The byte-stream channel does the same over a child's standard streams, with no listener to verify (§15, §16).

A driver calls `open` and never learns which kind of channel it has.

## 4. Trust boundary

A server acts with its user's full authority over every workspace that user can write. The rule is therefore: **only the same user, on the same machine, from outside any sandbox, may open a session; and a client sends nothing until it has verified the same of the listener, and the listener has identified itself as a gwz server.**

- **Where servers listen.** On Linux and macOS, in a per-user directory:
  - `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`, parsed and walked (§3);
  - the host creates the directory with mode 0700, relative to the walked parent and without following a link, and binds the socket with mode 0600;
  - the host refuses a directory with another owner, any group or other permission bits, or a symlink, with `server_address_refused`;
  - an explicit socket address must be in an existing directory that passes the same checks. The host never creates it.

  Windows uses a named pipe, and a per-user directory for the lock and the log (§11).
- **The default address.** On Linux and macOS, `auto` is `server-<key>` in that directory; Windows follows below. The key is the first 16 hexadecimal digits of the SHA-256 of a deterministic-CBOR list of:
  - the core version and build;
  - every name and value in the must-match set (§5) on this platform, in its table's order and in the comparison's form, for every build and every route, whatever the off switch. That includes the OpenSSL library, the names the client's own OpenSSL configuration scan finds with their values, the identities (§5) of the files that scan read and, on Linux, of the CA bundle its `SSL_CERT_FILE` resolves to, and on Windows the logon session. The libgit2 network timeout is the client's `--ssh-timeout`, 9 seconds by default in every role (§6); a library client uses the default.

  A server started for one environment is found by every client with the same environment and core. A different core version or environment gets a server of its own, instead of a version or environment refusal. The one exception is a library session that sets its own timeout after it opens: the key cannot carry that timeout, so the session's native-path operations are refused (§5).
  - The key only chooses a server. The host still checks every must-match value, so a digest collision is refused, never served.
  - **On Windows** the pipe's name is not derived from the key, since every user can list the machine's pipes and create a pipe under any name it has seen. The host draws the name at each start: `\\.\pipe\gwz-` followed by 32 hexadecimal digits from the operating system's cryptographic generator (`BCryptGenRandom` or `ProcessPrng`). It creates the pipe with `FILE_FLAG_FIRST_PIPE_INSTANCE`, and only then records the name in `server-<key>.lock` in the per-user directory, which only the user can read (§11), so no client reads a name that has not been created. A client reads the name there, and parses it with the address grammar before any open.
    - If a pipe of the drawn name exists already, the creation fails, and the host draws again, at most three times in all, before it fails with `server_unavailable`.
    - When no name is recorded, or the recorded pipe does not answer or fails listener verification, an `auto` client starts a server once (§6), and `server start` for `auto` treats it as nothing answering. The copy either takes the lock and draws a new name, or finds a live server holding the lock, whose record the client then reads again. At an explicit pipe address the refusal stands.
    - A name that another user saw in the listing therefore dies with its server: no later server uses it.
    - The name shows other users neither the user's SID nor the key.
- **The key and the registry partition different things** (reuse design §3, §11; TR1.3's `auto` key).
  - The key partitions servers by must-match values, in the comparison's form. Under `auto`, a server's sessions therefore share the home and the agent source. CLI sessions also share the timeouts, which the key carries as `--ssh-timeout`.
  - Inside one server, the endpoint registry partitions endpoint instances by endpoint configuration, compared as bytes. Under `auto`, only the CA certificates' content, the proxy, the no-proxy list and a library session's own timeouts can separate a server's instances.
  - At an explicit address, sessions with the off switch off may also differ in their agent source, and then in their instances.
  - The two compose. The key only chooses the server; the registry's byte equality alone decides which instance serves a request. A `GIT_SSL_CAINFO` file edited between two commands keeps its server and gets a new instance.
  - §5 says why the key covers the native path's values even with the switch off.
- **Peer check.** On every accept, before reading a byte, the host reads the peer's credentials, and refuses any connection from another user or from a peer the sandbox rule refuses:
  - Linux: `SO_PEERCRED`, and `SO_PEERPIDFD` for the process where the kernel has it;
  - macOS: `getpeereid`, and `LOCAL_PEERTOKEN`'s audit token for the process, where revision 0 took `LOCAL_PEERPID`;
  - Windows: see §11.

  A refused peer gets one `SessionError` (`call_id` 0, `server_peer_refused`) naming the cause, and then the connection closes. The frame carries nothing else. A peer that passes gets the host's `SessionHello` (§3).
- **Sandboxed peers are refused.** A process inside a sandbox must not gain its user's full authority through the server.
  - Limiting the server to certain workspace roots would not confine such a process. Git runs commands named in a workspace's own configuration, such as hooks, `core.fsmonitor` and `core.sshCommand`. A sandboxed process that can write a workspace could therefore still run code outside its sandbox through the server.
  - Refusal is the only sound rule.
- **The sandbox rule.** A process is sandboxed when:
  - on Linux, its `Seccomp` status in `/proc/<pid>/status` is not 0, so that it runs under a seccomp filter or in strict mode; or its user namespace is not the initial one. The initial user namespace is recognised by its inode: `stat` on `/proc/<pid>/ns/user` gives `0xEFFFFFFD`, the kernel's fixed number for it (`PROC_USER_INIT_INO`; inferred, and §12 tests it);
  - on macOS, the sandbox check reports a sandbox: `sandbox_check_by_audit_token` on the process's audit token, where it exists, else `sandbox_check` on its process ID. Apple documents neither (inferred);
  - on Windows, its token is an AppContainer token, or its integrity is below medium.

  A process that checks a peer refuses it when the peer is sandboxed, and also when:
  - on Linux, the peer is in a different mount or user namespace from the checking process;
  - on Windows, a host's client has an integrity below the host's own, or a client's listener has an integrity other than the client's own;
  - the check cannot be made: the process has gone, began after the connection, its status cannot be read, or the interface fails.

  **Process IDs are pinned.** A check reads a process only through a hold on it, taken after the connection, and a process that began after the connection is refused, since a reused process ID would otherwise name another process (all inferred):
  - Linux: `SO_PEERPIDFD`'s process descriptor, Linux 6.5 and later; before that, the process's start time must precede the connection;
  - macOS: `LOCAL_PEERTOKEN`'s audit token, which carries the process's version as well as its ID;
  - Windows: a handle to the process, whose creation time must precede the connection.

  A host's refusal is `server_peer_refused`, and a client's is `server_listener_refused` (below).
  - **The rule is absolute on every platform.** A sandboxed process is refused whatever sandbox the other process is in, so no two sandboxes of one user reach each other through a server, siblings with equal seccomp filters included.
  - **Its cost on Linux.** A container that applies a seccomp profile, Docker's default included, or that runs in a user namespace, hosts no server in this release, and its user runs in-process (§14). A container with neither can host one, which then serves only its own mount namespace.
  - **The Windows comparisons** stop a medium-integrity client from driving an elevated server, and an elevated client from running its commands in a medium-integrity one, where writes and mapped drives differ.
- **Listener verification** (TR1.3; amendment §3.4). A client verifies the listener before it sends any byte. A failed check sends no frame, and refuses with **`server_listener_refused`**, naming the cause.
  - **Linux and macOS, before connecting.** The socket's directory is owned by the caller's user, has no group or other permission bits, and is not a symbolic link. A socket file that exists is a socket, owned by the user, with no group or other bits, and not a link.
    - For `auto`, a missing per-user directory is not a refusal. The auto-started host creates it with mode 0700, and the client then verifies it and connects.
    - The `/tmp/gwz-<uid>/` fallback is refused when it exists with another owner or mode.
  - **Linux and macOS, after connecting.** The connected socket's peer credentials (`SO_PEERCRED`, `getpeereid`) name the caller's user. The listener's process passes the sandbox rule: on Linux the process `SO_PEERCRED` names, held through `SO_PEERPIDFD`; on macOS the process `LOCAL_PEERPID` names, held through `LOCAL_PEERTOKEN`'s audit token.
  - **Windows.** The client opens the pipe with `SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION`, so a local server can identify the client but never impersonate it. `GetNamedPipeServerProcessId` names the pipe server's process. Its token must name the caller's user SID, must not be an AppContainer token, and must have the caller's own integrity level. Microsoft documents that call for a handle `CreateNamedPipe` created, so its use on the client's handle is tested on dabeest (§12), and §14 names the fallback.
  - A check that cannot be made refuses, as on the host.
  - **Then the listener must identify itself.** The client reads the host's `SessionHello` within the handshake's bound (§3), and sends nothing if none arrives, if it is not well formed, or if the versions it checks do not match (§3). A same-user program that is not a gwz server, at a mistyped explicit address, therefore receives no byte of the snapshot.
  - The stdio mode and the SSH remote form have no listener: the peer is a child the client started (§15, §16). They too read the `SessionHello` first.
- **Sandboxed callers** (TR1.3, "Defaults").
  - Before a client connects to or starts a server, it applies the sandbox rule to its own process, on all three platforms. A sandboxed caller, or one whose check cannot be made, is refused with `server_peer_refused` before any connection or start: at the `auto` path, at an explicit address, and in the stdio local form alike. So is `server start`, since such a server would refuse every client. No server is ever started inside a sandbox the rule detects.
  - The stdio child would inherit the caller's sandbox and grant it nothing (§15), but it is refused too, so that one rule covers every value `GWZ_SERVER` can hold. The SSH remote form, which `GWZ_SERVER` cannot hold, runs `ssh` inside the caller's sandbox and grants nothing either, and is not refused (§16).
  - A caller whose sandbox the rule does not detect, but which denies the connection itself, such as a Landlock rule, gets `server_unavailable`, naming the operating system's error.
  - Every such message names the cause and `--no-server` (§17).
  - The `hook` family never uses a server, since tools run it inside their sandboxes (§7).
- **Files the host opens.** Any process of the user may be able to write the per-user directory and an explicit socket's directory, a sandboxed one included (§14). So every file the host creates or opens there, or at `--log PATH`, follows one rule: the lock, the log and the log's previous file (§6).
  - **On Linux and macOS** the host opens the file relative to the walked directory's descriptor, with `O_NOFOLLOW` and `O_CLOEXEC`, and with `O_NONBLOCK` during the open, cleared after it. It uses an existing entry only if `fstat` on the new descriptor shows a regular file that the user owns, with no group or other permission bits. It creates a new file with mode 0600, and sets that mode on it. It opens the log with `O_APPEND` and only ever appends to it, never truncating it.
  - **On Windows** it opens the file with `FILE_FLAG_OPEN_REPARSE_POINT`, refuses a reparse point, and requires the file's owner to be the user. It opens the log for appending only (`FILE_APPEND_DATA`).
  - **An entry that fails** is refused with `server_address_refused`, naming the file and the rule, and the start refuses. It is never followed, truncated or written. So a link planted at the log or the lock never makes the server write another file, and a FIFO planted there never blocks the start.
  - The stale-socket check and removal below use `fstatat` with `AT_SYMLINK_NOFOLLOW`, `connect` and `unlinkat`, all on the same directory descriptor.

  The OpenSSL configuration scan reads system files, not these, and has its own rule (§5).
- **One server per address.** Before listening, the host takes an exclusive lock: on `server-<key>.lock` in the per-user directory for `auto`, and on `<socket path>.lock` beside an explicit socket. §11 gives Windows.
  - **Nothing before the checks.** The host creates the lock file only after the walk and the directory checks pass. `server start` also probes the address as a client before it creates anything (§6), so a start refused because something answers at the address, or because a socket there is not one gwz made, leaves nothing behind. Only a listener or socket that appears between that probe and the lock can leave an empty lock file beside it, and the refusal names that file.
  - **The record.** Once it holds the lock, the host reads the record a previous holder left, which the stale-socket check below uses, and then, once that check has passed, writes its own record into the lock file through its descriptor, before it binds: its process ID, its address and its log path. A start the check refuses writes no record, so the lock file it leaves names nothing a later start could take as gwz's. A clean exit removes the socket, then clears the record, then releases the lock, so a socket gwz made never outlives its record.
    - Every reader of the record, whether a starter, `list`, a client at the handshake's bound or a Windows `auto` client, opens the lock file under the file rule above, so a link or FIFO planted there neither misleads nor blocks it.
    - The record is only a claim. Any process of the user may write the file, a sandboxed one included, or hold its lock. So every message that quotes the record says that it comes from the lock file (§17), and an address the record gives is parsed and walked (§3), like any other, before any connect.
  - **A held lock.** If the lock is held, the host probes the address again, within 5 seconds, and exits; on Windows, for `auto`, it probes the pipe the record names, once the address grammar has passed it. When a gwz server answers, it is running there, and the message names it (§17). When no gwz server answers, the message says that the lock is held but no gwz server answers, and names the lock file, with the process ID and log path its record gives, as from the lock file. A record that is still incomplete means that the holder is starting, and the message says so.
  - **A stale socket.** On Linux and macOS, holding the lock, the host checks the entry at the address. It removes the entry, and binds, only when all three hold:
    1. the entry is a socket the user owns, with no group or other permission bits, and not a link;
    2. a connection to it, made without blocking and within 5 seconds (§3), with nothing sent, is refused (`ECONNREFUSED`);
    3. the record the previous holder left in the lock file beside it, read under the file rule, names exactly this socket's address. That shows that a gwz server made the socket: the record is evidence of provenance, and nothing more.

    A refused connection alone never shows a socket stale, since macOS refuses a connection to a full accept queue just as it refuses one to a dead socket (§3, §14).
    - A socket that no such record names refuses the start with `server_unavailable`, naming the path and saying to remove it if nothing uses it, and nothing is deleted.
    - A connection that succeeds, one that does not complete within the 5 seconds, or any other error refuses the start with `server_unavailable` too, naming the address and, where the connection was made, the listener's process ID from the peer credentials. Any other entry refuses the start.
    - Under `auto`, the per-user directory's own records apply, so a crashed server's socket is removed as before. At an explicit address, gwz's own stale socket still has its record beside it, so crash recovery works there too; a foreign program's socket never does.

    So a mistyped address never deletes a file, never a socket something still answers on, and never a socket that gwz did not make.
  - The lock file stays when the server exits; only the lock is released. Removing the file would let a starter that had opened it before the removal hold a lock that no later starter sees.
  - A Windows pipe disappears with its server, so nothing is left over there.
- **What a server grants.** An unsandboxed process of the same user gains nothing it couldn't already do by running gwz itself. That holds with reuse (reuse design §4, §12):
  - a session uses a pooled connection only after its own agent or key file, and its own known_hosts, re-prove it;
  - what it saves is the handshake.

  The sandbox refusal covers the one case where a peer would gain something. Listener verification covers the reverse: a sandboxed listener gaining a client's environment (amendment §2 item 4).
- **Secrets.** The environment snapshot (§5) is secret-bearing, as the contract says. It crosses the socket once, in `SessionOpen`: only after listener verification has passed, and only after the listener has sent a well-formed `SessionHello` whose versions the client accepts (§3). The host reads it only after its peer check has passed.
  - The host never logs it or writes it anywhere.
  - It keeps the snapshot only for its session, then zeroizes it, since a long-lived server would otherwise accumulate every client's secrets.
  - An endpoint instance's configuration may outlive the session, until the instance is disposed. It is derived from a session's snapshot, and holds no secret: paths, an agent address, public certificates, a proxy host and timeouts (reuse design §3).

## 5. Whose process is it?

A command run through a server must behave as the same command run in the client's own process, or be refused. The server's value must never silently replace the client's.

**Two roles.** The contract's "driver" both captures the environment and creates the host context. Over a byte stream those roles split:
- the connecting client supplies the snapshot;
- the process that serves the stream supplies the host context.

The SSH remote form is the exception: its remote stdio process supplies both (§16).

**Environment.** The client captures its environment at its edge (contract §5.6) and sends it in `SessionOpen`. The host passes it to `open` as the session's snapshot. Everything the session host derives from the snapshot then follows the client, per session: the endpoint configuration (the SSH home, the agent source, known_hosts, TLS roots and proxies), the `gh` environment, and every child process's environment. The server's own environment is used for nothing on the session path, except the process-wide reads that the must-match set covers.

**Process attributes a server cannot hand over.**

| Attribute | Rule |
| --- | --- |
| Working directory | **Forwarded.** Every request that carries `RequestMeta` carries `InvocationContext.caller_cwd`, and core resolves the workspace root and relative operands against it. The host has no directory of its own to fall back to: a request without the context is refused with `invalid_request` before any effect (core's `caller_directory`, gwz-core `5ca45a66`). Every child process core spawns gets an explicit working directory, the repository it acts on, or `/` for the `gh` helper, so none inherits the host's. |
| Paths in requests | **Sent exactly, or not at all.** Request path fields are Unicode text. A client therefore refuses a working directory, root or operand that is not valid Unicode, instead of altering it: a non-UTF-8 name on Linux or macOS, or an unpaired surrogate on Windows. The refusal uses `invalid_request` and shows the path with its invalid bytes escaped. |
| Environment variables the session host reads | **Forwarded** in the snapshot. |
| Values read process-wide | **Must match:** the must-match set, below. |
| The off switch (TR1.5) | **Carried:** the client's resolved value, in `ProcessAttributes.transport_off`, below. |
| Terminal | The server has none and needs none. Core's git children already run with captured output and no stdin (`Command::output()`), and the client renders progress from events. |
| Inputs a CLI reads: its stdin, and files relative to its directory | Read by the CLI into the request, as in-process. The server never reads a client's stdin. |
| User and groups | The same user, by the peer check. Groups are the server's as of its start (§14). |
| Resource limits, the process's locale, and name resolution | The server's own. Git children get the client's locale variables through the snapshot. The C library's resolver reads its configuration from the server's process, including, with glibc, variables such as `HOSTALIASES` and `RES_OPTIONS`. Host keys and TLS verification still decide trust (§14). |

**The must-match set.** Some inputs are read process-wide: by libgit2, its TLS stream and libssh2, with their crypto backends; by the transport's own SSH and TLS, which use the same OpenSSL on Linux and macOS; by the dynamic loader; and by the kernel, for new files. A server's process has one of each. A session's value must therefore equal the server's, or the session or the operation is refused with `server_environment_mismatch`. The refusal names the value, never its content (TR1.3; amendment §3.4).

| Row | Values | Checked | Why |
| --- | --- | --- | --- |
| Every session: libgit2's start | Linux and macOS: `HOME`, `XDG_CONFIG_HOME`, `APP_SANDBOX_CONTAINER_ID`, and `SUDO_UID` when the effective user is root. Windows: `HOME`, `HOMEDRIVE`, `HOMEPATH`, `USERPROFILE`, `XDG_CONFIG_HOME`, `APPDATA`, `LOCALAPPDATA`, `PROGRAMDATA` and `PATH` | at `SessionOpen` | libgit2 fixes its configuration search paths, and native SSH's known_hosts path, from the process's environment at its first initialization, and never re-reads them (sysdir.c:453-465; ssh_libssh2.c:448). That known_hosts file is the native path's only host-key check, since gwz registers no `certificate_check`. `SUDO_UID` decides which repositories a root process trusts, at each open (fs_path.c:1984) |
| Every session, POSIX: the mask | the file-creation mask (`umask`) | at `SessionOpen` | core creates files under the process's mask, which is process-wide. Windows has no process-wide mask: new files inherit their directory's ACL |
| Every session, Linux and macOS: OpenSSL | the OpenSSL library the process loaded, sent as `ProcessAttributes.openssl` (§3); `OPENSSL_CONF`, `OPENSSL_CONF_INCLUDE`, `OPENSSL_MODULES` and `OPENSSL_ENGINES`; each `$ENV::` name the host's configuration scan finds (below); the CPU mask, `OPENSSL_ia32cap` on Linux x86-64 and `OPENSSL_armcap` on macOS ARM64; on Linux also `SSL_CERT_FILE`, `SSL_CERT_DIR`, `SSLKEYLOGFILE` and the `OPENSSL_MALLOC_` variables | at `SessionOpen` | OpenSSL loads its configuration once per process, and serves every session: libssh2's crypto, for the transport's SSH and the native path's, on both platforms; and on Linux also the transport's TLS (native-tls), libgit2's TLS, and its SHA-256. A server whose OpenSSL honours `SSLKEYLOGFILE` would log every client's TLS keys |
| Every session, Linux and macOS: the loader | `LD_LIBRARY_PATH` and `LD_PRELOAD`; `DYLD_LIBRARY_PATH`, `DYLD_FALLBACK_LIBRARY_PATH`, `DYLD_FRAMEWORK_PATH` and `DYLD_INSERT_LIBRARIES` | at `SessionOpen` | at process start they choose the libraries a process runs, its OpenSSL among them |
| The libgit2 network timeout | the session's timeout: the value `--ssh-timeout` or `configure_transport_runtime` sets, 9 seconds by default in every role (§6); 0, no timeout, matches only 0 | when an operation takes libgit2's native path, before any connection opens: every network operation in an ordinary build or with the switch on, a routed operation, and a `git://` or `http://` remote | it is process-wide in ordinary builds, and for libgit2's native remotes and routes in transport builds (contract §5.8). A socket host never changes it for one client; it refuses that operation instead. The transport's own remotes use the session's timeouts, so they are never refused for it. A stdio host serves one session, so it applies the value as in-process does (§15) |
| The native path | `SSH_AUTH_SOCK`; on Windows also the logon session, which the host reads from the client's token at the peer check | as "Native routes through a server" says, below | libssh2 reads the agent socket at each native agent authentication (agent.c:180; agent_win.c:136). On Windows it first tries a visible Pageant window (agent.c:436-439), and WinHTTP answers a server's challenge with the logon session's default credentials (winhttp.c:184-212) |

**The per-platform tables** give each variable's reader, when it is read, what it governs, and its row: "every session" for the rows checked at `SessionOpen`, and "native" for the native path's. They come from an enumeration of the vendored sources on 2026-09-28: libgit2 1.9.7, libssh2 1.11.1 (libssh2-sys 0.3.3), openssl-probe 0.1.6 and git2 0.21.0. No gwz build vendors OpenSSL, so OpenSSL's rows cite OpenSSL 3.6.3 as a reference. "Inferred" marks what the enumeration inferred rather than read. Ordinary and transport builds make the same reads; they differ only in which remotes reach libgit2's own transports (transport_binding.rs:175-256, 282-285).

*Linux x86-64.* The OpenSSL rows are the distribution's OpenSSL 3.x, which is not vendored: 3.0.x and distribution patches are not verified.

| Variable | Reader | Read | Governs | Row |
| --- | --- | --- | --- | --- |
| `HOME` | libgit2 sysdir.c:357, :405 | libgit2's start | `~/.gitconfig` and `~/.config/git/config`; native SSH's `~/.ssh/known_hosts`, read per connection (ssh_libssh2.c:448) | every session |
| `XDG_CONFIG_HOME` | sysdir.c:402 | libgit2's start | the XDG configuration directory. Set but empty, it gives the relative path `git` (inferred, :402-403) | every session |
| `APP_SANDBOX_CONTAINER_ID` | sysdir.c:347 | libgit2's start | when it is present, or when the real and effective user IDs differ, the passwd entry's home replaces `HOME` (:356-359, :401-410). It is not limited to macOS | every session |
| `SUDO_UID` | fs_path.c:1984, used at :2030-2033 | each repository open that checks ownership (repository.c:645-651) | with an effective user of root, trust in repositories the `sudo` caller owns: local operations and `file://` remotes | every session, when root |
| `SSL_CERT_FILE` | openssl-probe lib.rs:155-161, which also writes it (:119-139); OpenSSL by_file.c:56, through libgit2 openssl.c:148; native-tls openssl.rs:19 | git2's start: the probe, then libgit2's global `SSL_CTX`, once; the transport's first TLS connector | the CA bundle: loaded once for native HTTPS, and the transport's default roots | every session |
| `SSL_CERT_DIR` | as `SSL_CERT_FILE`; OpenSSL by_dir.c:91 | as `SSL_CERT_FILE` | the CA directory. Its name is fixed at the start; its files are read at each verification | every session |
| `OPENSSL_CONF` | OpenSSL conf_mod.c:690 | OpenSSL's first initialization, at git2's start | the configuration file (unset: `openssl.cnf` in the library's OPENSSLDIR): providers, and the TLS `system_default` settings applied at each `SSL_CTX_new` | every session |
| `OPENSSL_CONF_INCLUDE` | conf_def.c:440 | the configuration's load, when it uses `.include` | the include directory | every session |
| `$ENV::NAME`, any name | conf_api.c:84-85 | the configuration's load | any configuration value: open-ended (below) | every session |
| `OPENSSL_MODULES` | provider_core.c:999 | the configuration's load, when it activates a provider that is not built in | the provider module directory | every session |
| `OPENSSL_ENGINES` | eng_list.c:461, through eng_cnf.c:97 | the configuration's load, when it names an engine not yet loaded | the engine directory | every session |
| `OPENSSL_ia32cap` | cpuid.c:106 | libcrypto's load | the CPU feature mask, for all OpenSSL use | every session |
| `SSLKEYLOGFILE` | ssl_lib.c:3993-3995 | each `SSL_CTX_new` | TLS key logging, only where the distribution built OpenSSL with it, which cannot be established | every session |
| `OPENSSL_MALLOC_FAILURES`, `_FD`, `_SEED` | mem.c:170, 182, 184 | libcrypto's first allocation | memory debugging, only where the distribution built OpenSSL with it (off by default, Configure:601), which cannot be established | every session |
| `LD_LIBRARY_PATH`, `LD_PRELOAD` | the dynamic loader, ld.so(8) | process start | the libraries the process runs, OpenSSL among them | every session |
| `SSH_AUTH_SOCK` | libssh2 agent.c:180 | each native agent authentication (ssh_libssh2.c:241-246, 314) | native SSH's agent | native |

*macOS ARM64.* OpenSSL is Homebrew's openssl@3, a dynamic library found at build time (inferred for release builds); libgit2-sys and libssh2-sys link it on every Unix target.

| Variable | Reader | Read | Governs | Row |
| --- | --- | --- | --- | --- |
| `HOME`, `XDG_CONFIG_HOME`, `APP_SANDBOX_CONTAINER_ID`, `SUDO_UID` | as on Linux | as on Linux | as on Linux | as on Linux |
| `OPENSSL_CONF`, `OPENSSL_CONF_INCLUDE`, `$ENV::NAME`, `OPENSSL_MODULES`, `OPENSSL_ENGINES` | as on Linux | git2's start (libgit2-sys lib.rs:4740-4744; libssh2-sys lib.rs:776) | libssh2's crypto, for the transport's SSH and the native path's. HTTPS uses SecureTransport and Security.framework. The stock file, `/opt/homebrew/etc/openssl@3/openssl.cnf`, has no active `$ENV::` or `activate` | every session |
| `OPENSSL_armcap` | armcap.c:265, from a constructor at :66 | libcrypto's load | the CPU feature mask | every session |
| `DYLD_LIBRARY_PATH`, `DYLD_FALLBACK_LIBRARY_PATH`, `DYLD_FRAMEWORK_PATH`, `DYLD_INSERT_LIBRARIES` | the dynamic loader, dyld(1) | process start | the libraries the process runs, OpenSSL among them | every session |
| `SSH_AUTH_SOCK` | libssh2 agent.c:180 | each native agent authentication | native SSH's agent | native |

Not read on macOS:
- `SSL_CERT_FILE` and `SSL_CERT_DIR`. Nothing sets default verify paths: libgit2's OpenSSL stream is compiled out (openssl.c:751-767), libssh2 uses no X509 store, and the transport's TLS is Security.framework's;
- `SSLKEYLOGFILE` and the `OPENSSL_MALLOC_` variables: Homebrew builds OpenSSL without them;
- openssl-probe does not run.

libgit2 passes SecureTransport no environment (stransport.c:101-107, 324-336). Whether Security.framework reads any cannot be established.

*Windows x86-64.* libgit2 reads each variable at its start, through `ExpandEnvironmentStringsW` (sysdir.c:29) or `GetEnvironmentVariableW`. Windows links no OpenSSL.

| Variable | Reader | Read | Governs | Row |
| --- | --- | --- | --- | --- |
| `HOME`, `HOMEDRIVE` with `HOMEPATH`, `USERPROFILE` | sysdir.c:327-334 | libgit2's start | the global and home search lists: `.gitconfig`, and native SSH's `.ssh\known_hosts` in the first home directory that exists (ssh_libssh2.c:448). These are lists, not a fallback chain: every template that expands and exists joins (:173-196), and each lookup takes the first match (:562-581) | every session |
| `XDG_CONFIG_HOME`, `APPDATA`, `LOCALAPPDATA`, and the home variables with `\.config\git` | sysdir.c:378-388 | libgit2's start | the XDG configuration list | every session |
| `PROGRAMDATA` | sysdir.c:265-270 | libgit2's start | `%PROGRAMDATA%\Git\config`, ownership-checked (config.c:1251-1268) | every session |
| `PATH` | sysdir.c:140-141; path_w32.c:193, 207 | libgit2's start | where `git.exe` or `git.cmd` is, which gives the system gitconfig (`<git>\etc`, `\mingw64\etc`, `\mingw32\etc`) and the templates (sysdir.c:198-222, 280, 425) | every session |
| `SSH_AUTH_SOCK` | libssh2 agent_win.c:136, the C runtime's `getenv` | each native agent authentication, only when no Pageant window is visible (agent.c:436-439) | the OpenSSH agent's pipe; unset, `\\.\pipe\openssh-ssh-agent` (agent_win.c:124, 138). There is no Unix-socket agent (agent.c:47-54) | native |

libgit2 reads the Win32 environment block and libssh2 the C runtime's copy, so a change made with `SetEnvironmentVariableW` would reach only libgit2 (inferred).

**Compiled in, but unreachable from gwz.** These names appear in the vendored sources. The source-level test takes this table as its exclusion list, each name with its reason.

| Variables | Reader | Why unreachable |
| --- | --- | --- |
| `GIT_DIR`, `GIT_CEILING_DIRECTORIES`, `GIT_DISCOVERY_ACROSS_FILESYSTEM`, `GIT_COMMON_DIR`, `GIT_WORK_TREE`, `GIT_NAMESPACE`, `GIT_CONFIG_NOSYSTEM`, `GIT_CONFIG_SYSTEM`, `GIT_CONFIG_GLOBAL`, `GIT_OBJECT_DIRECTORY`, `GIT_ALTERNATE_OBJECT_DIRECTORIES`, `GIT_INDEX_FILE` | libgit2 repository.c:210, 393, 933, 947-948, 957-958, 1074, 1369, 1380, 1393, 1490, 1510, 1641 | read only with `GIT_REPOSITORY_OPEN_FROM_ENV` (:926, :1130). gwz opens repositories with `NO_SEARCH` or `NO_DOTGIT` (for example inventory.rs:220-222), never from the environment |
| `GIT_AUTHOR_*`, `GIT_COMMITTER_*`, `EMAIL` | signature.c:230-261, 297-305 | read only by `git_signature_default_from_env` (git2 repo.rs:1811, 1841). gwz uses `Signature::now` and `repo.signature()` |
| `https_proxy`, `HTTPS_PROXY`, `http_proxy`, `HTTP_PROXY`, `no_proxy`, `NO_PROXY` | remote.c:1147-1160 | read only with `GIT_PROXY_AUTO`. gwz passes no proxy options, and git2's default is `GIT_PROXY_NONE` (transport_support.rs:89-91, 138-140; proxy_options.rs:9-14) |
| `GIT_SSH_COMMAND`, `GIT_SSH` | ssh_exec.c:156-164 | not compiled: `GIT_SSH_EXEC` is not defined (libgit2-sys build.rs:248-255) |
| `PATH` on Linux and macOS, and a child process's whole environment | fs_path.c:2081; unix process.c:113, 128; win32 process.c:136 | reached only through `git_process`, which only ssh_exec.c uses |
| `POSIXLY_CORRECT` | libgit2's `src/cli` | libgit2's CLI is not compiled (build.rs:149-156) |
| `TMP`, `TEMP`, with `USERPROFILE` (Windows) | `GetTempPathW`, at winhttp.c:1398 | the temporary file of a buffered POST; WinHTTP uses chunked transfer on Windows 6.0 and later (winhttp.c:1605). The names come from Microsoft's documentation, not from source |
| OpenSSL's `RANDFILE` with `HOME`, `CTLOG_FILE`, its proxy variables, `QLOGDIR`, `OSSL_QFILTER`, `LEGACY_GOST_PKCS12`, the CPU variables of info.c, and `OPENSSL_TRACE` | randfile.c:306-308; ct_log.c:163; http_lib.c:282-306; qlog.c:110-111; p12_mutl.c:246; info.c:63-171; apps/openssl.c:272 | read only through interfaces neither libgit2 nor libssh2 calls (libssh2 uses only `RAND_bytes`, openssl.c:261), or only by the `openssl` command |

- **Not needed:** `GIT_SSL_CAINFO` and `http.sslCAInfo`. libgit2 reads neither. The transport's endpoint configuration reads `GIT_SSL_CAINFO` from the snapshot (local_command.rs:85).
- **The list's home.** Core owns the set, as one list beside the contract's §5.8 disclosure, so the two cannot drift (§8). A source-level test scans the vendored sources, libgit2, libssh2, openssl-probe and git2's `lib.rs`, for every environment read, and asserts that each name is in the list or in the exclusion table (amendment §3.6). OpenSSL on Linux and macOS is not vendored, so its rows come from this list, not from a scan. Proxy detection, if the native path ever gains it, joins the list: the proxy variables then leave the exclusion table.
- **Comparison.** Values compare as byte strings, WTF-8 on Windows. Names match exactly on Linux and macOS, and without regard to case on Windows, where environment names are case-insensitive. Unset and empty stay distinct on every platform:
  - on Linux and macOS every reader treats an empty value as set (util.c:786-795);
  - on Windows libgit2 treats an empty value as unset (util.c:767-777), but libssh2 reads the C runtime's copy, which keeps it (inferred). Keeping the two distinct can then only refuse a pair that libgit2 alone would treat alike, which fails closed.

  One exception: on Linux, `SSL_CERT_FILE` and `SSL_CERT_DIR` compare as openssl-probe 0.1.6 resolves them. That is the variable's path when the path exists, else the first of the probe's standard locations that exists, else none (openssl-probe lib.rs:31-50, 155-195).
  - git2's start writes exactly these resolved values into the process environment, and writes nothing when it finds nothing (openssl-probe lib.rs:119-139). Resolving a resolved value gives it back.
  - The comparison therefore gives the same answer whether a side captured its environment before that write, as gwz-py's `os.environb` is, or after it, as a serving CLI's is: gwz-cli runs git2's start at startup (gwz-cli src/lib.rs:178).
  - A host resolves its own two values once, after its git2 start, which is what its libgit2 loaded, and keeps them. A client's are resolved when they are compared or keyed.
- **Relative paths are refused.** A server, and a stdio child, run in another working directory than their client (§6), so a relative path resolves differently there, whatever the comparison finds. A socket or stdio host therefore refuses at `SessionOpen`, with `server_environment_mismatch` naming the variable, a session whose must-match set holds a relative path:
  - in a variable that names a file or directory: `HOME`, `XDG_CONFIG_HOME`, `OPENSSL_CONF`, `OPENSSL_CONF_INCLUDE`, `OPENSSL_MODULES`, `OPENSSL_ENGINES`, `SSLKEYLOGFILE` and `SSH_AUTH_SOCK` on Linux and macOS, and on Linux `SSL_CERT_FILE` and `SSL_CERT_DIR` as resolved; on Windows `HOME`, `USERPROFILE`, `XDG_CONFIG_HOME`, `APPDATA`, `LOCALAPPDATA`, `PROGRAMDATA` and `SSH_AUTH_SOCK`, and `HOMEDRIVE` with `HOMEPATH`, taken together;
  - in an element of a list: `LD_LIBRARY_PATH`, `LD_PRELOAD`, the `DYLD_` variables, and `PATH` on Windows.

  On Linux and macOS an empty value counts as relative, since its reader then forms a relative path from it: a set but empty `XDG_CONFIG_HOME` gives libgit2 the relative path `git`. Two variables are exempt, because their empty value resolves against no directory: an empty `SSH_AUTH_SOCK` selects no agent (below), and OpenSSL ignores an empty `SSLKEYLOGFILE` (ssl_lib.c:4286). Each compares as a value, distinct from unset, and a non-empty relative value of either is still refused. On Windows libgit2 treats an empty value as unset, so it is not refused, and a path is absolute only with a drive and `\`, or as `\\`. A `$ENV::` name that the scan finds is not checked, since nothing says its value is a path. A socket host whose own values hold such a path refuses to start, naming the variable, since it could serve no session. The SSH remote form compares nothing, so the rule does not apply to it (§16).
- **OpenSSL's configuration fails closed.** On Linux and macOS, OpenSSL is the distribution's or Homebrew's, not vendored, and its configuration can expand any variable (`$ENV::NAME`, conf_api.c:84-85). The list therefore cannot be closed from source, and a socket host closes it at run time.
  - **The scan.** At start, the host reads the configuration its OpenSSL loaded: `OPENSSL_CONF`, else `openssl.cnf` in the library's OPENSSLDIR. It follows every `.include` and `.pragma includedir`, relative to `OPENSSL_CONF_INCLUDE` where that is set; for an included directory, it reads that directory's `.cnf` and `.conf` files, as OpenSSL does. It collects every `$ENV::NAME` and `${ENV::NAME}` reference, and each name joins the every-session OpenSSL row for that host.
  - **The files it reads.** The scan opens each file with close-on-exec, and without blocking during the open, and reads it only if `fstat` shows a regular file of at most 1 MiB. It follows symbolic links, as OpenSSL itself does: distributions link their configuration (Debian's `/usr/lib/ssl/openssl.cnf` names `/etc/ssl/openssl.cnf`), and these are system files, owned by root, which the scan only reads. §4's file rule, which never follows a link and requires the user's own file, is for the files a host writes, and would refuse these. A FIFO, a device or an oversized file therefore fails the scan, and never blocks it. A failure names the file and the reason, never the file's content.
  - **Failing closed.** If the scan cannot finish, the start is refused with `server_unavailable`, naming the file and the reason. That covers a file it cannot read, one that is not a regular file or is over the bound, an include it cannot resolve, more than 32 files or includes nested more than 8 deep, and a line it cannot parse.
  - **Files read once.** A file's **identity**, here and wherever this design uses the word, is its device and inode, its size and its modification time. The host records the identity of each file the scan read, and, on Linux, of the CA bundle libgit2 loaded into its global context. OpenSSL and libgit2 read these once per process, so a server would otherwise keep what an in-process command no longer sees. The host refuses, with `server_environment_mismatch`, naming the file and `gwz server stop --server <address>`:
    - a session at `SessionOpen`, if a configuration file's identity has changed;
    - a native HTTPS operation at routing, if the CA bundle's has.

    A host that has refused either also shuts down at its next idle point, when no session is open, whatever its `--idle-exit`, so that a service manager restarts it with the new files (§6).
  - **The key.** A client that computes the `auto` key runs the same scan, under the same file rule, over its own effective configuration. The key covers the names it finds, their values, the identities of the files, and on Linux the identity of the CA bundle. A configuration or bundle changed in any way, edited in place or replaced, therefore gets a new server under `auto`, and the old one exits when idle. A client whose scan cannot finish refuses `auto` with `server_unavailable`, naming the file and the reason, before any connect, and starts nothing. Explicit addresses do not use the key, and are unaffected.
  - **The stdio mode** skips the scan: it serves one session, whose environment it inherited (§15).
  - **The residual.** The scan reads what OpenSSL is documented to read. A distribution patch that makes OpenSSL read another variable or file is not seen (§14). Vendoring OpenSSL is an alternative for the operator, and does not remove the scan (§14).
- **Inputs that are not variables** (the enumeration's §4). The peer check makes a client and its server the same user on the same machine, and, on Linux, in the same mount namespace. These inputs are then equal for both:
  - the system gitconfig and templates: `/etc/gitconfig` and `/usr/share/git-core/templates` (sysdir.c:282, :427); on Windows, `%PROGRAMDATA%\Git\config` once `PROGRAMDATA` matches, and Git for Windows' `InstallLocation`, read from HKCU, which is the user's, and from HKLM (sysdir.c:24-25, 117-134);
  - the WinHTTP machine proxy (`netsh winhttp`), which applies although gwz asks for no proxy (winhttp.c:828-833);
  - macOS keychains and trust settings (stransport.c:107), and the Windows certificate stores, the user's and the machine's;
  - the effective user and its passwd entry;
  - files read when they are used: `.gitconfig`, known_hosts, and the CA directory's certificates.

  These are not equal, and the design handles each:
  - the environment: the must-match set;
  - the OpenSSL library a process loaded, its configuration, and its CA bundle as it was at the server's start: the OpenSSL row and "Files read once" above;
  - the real user ID, where it differs from the effective one. libgit2 then takes the home from the passwd entry instead of `HOME` (sysdir.c:356-359), which a server whose IDs are equal would not do. Such a client uses no socket server: it refuses with `server_environment_mismatch`, before it connects;
  - the Windows logon session: WinHTTP's default credentials, with their Internet-zone fallback (winhttp.c:184-212, 230-282), and the desktop on which a Pageant window is visible, or not (agent.c:343). The native row compares the logon session. Pageant's visibility itself changes over time, and belongs to a desktop, so the rule cannot see it. Within one logon session a client and its server normally share a desktop, and so see the same windows; a client on another desktop of that session can see a different one (§14);
  - the libgit2 timeout: its own row.
- **Ordinary builds.** An ordinary build has no transport, so every network operation takes the native path, and the native row is checked at `SessionOpen`.

**Native routes through a server** (TR1.3; amendment §3.4, replacing the plan's "The off switch through a server"). In a transport build, the native path runs in the server's process:
- **With the off switch on,** every network operation of the session takes it. The native row joins the session's must-match set and is checked at `SessionOpen`. A mismatch refuses the session with `server_environment_mismatch`.
- **With the switch off,** an operation can still take a native route: TR1.6's, for private HTTPS whose credential helper is not `gh`, or OD11's, for an SSH remote whose agent holds a key the transport cannot sign with (TR2.8). It is checked against the native row, and the timeout row, when it routes, before any connection opens.
  - The route decision comes first. TR2.8's agent key listing and TR1.6's helper check use only the session's own snapshot.
  - A mismatch refuses that operation with `server_environment_mismatch`. The message names the route's cause, the variable or the logon session but never a value, and `--no-server`.
- Remotes libgit2 always handles natively (`git://`, `http://` and `file://`) read only what the every-session rows and the timeout row cover.
- The stdio mode applies the same rules (§15). The SSH remote form's session uses its host process's own environment, so there is nothing to compare (§16).

**The `auto` key and native routes** (TR1.3; the item the reuse design's Verdict-1 carried to TR1.3). The key covers the native row whatever the switch (§4). Under `auto`, no must-match refusal of either kind can then arise for a CLI client, or for a library session that keeps the default timeout. At an explicit address, both can.
- **A library session's own timeout.** A library session under `auto` that configures another timeout after it opens has its native-path operations refused on the timeout row, as at an explicit address: the key carried the default, and `SocketCoreBridge` takes no timeout. Its message names the server's libgit2 timeout and no CLI flag (§17).
- **The alternative** covers the row only with the switch on. It keeps one server for clients whose agents differ, and refuses their routed operations under `auto`: a user whose agent holds a security key, and whose `SSH_AUTH_SOCK` differs from the server's, would have every SSH operation refused.
- **The cost of the recommendation** is more servers: one per distinct native-row value, in practice one per agent socket path, such as each login that forwards an agent, and on Windows one per logon session.
  - It loses no connection reuse. The endpoint configuration already partitions instances by agent source (reuse design §3), so clients with different agents never share a connection.
  - It loses queueing across such clients on one workspace. They are in different servers, and meet the workspace lock as separate processes do today.
  - Each auto-started server exits after 10 idle minutes (§6).

**The off switch's value** (TR1.5, "Servers"). The client resolves TR1.5's three forms (flag, then environment, then user configuration) into one value before `open`. It carries that value as `ProcessAttributes.transport_off`, in every form: socket, stdio and SSH. In-process, `open(options)` takes the same attribute. A host never derives a session's value from the snapshot, from its own environment or from its own configuration.
- **Why an attribute, not a snapshot entry.** The value is resolved, not captured: its flag and its user configuration are not in the environment. A synthetic entry would alter a snapshot the contract keeps as captured, and would pass into every child process's environment. The SSH remote form sends no snapshot at all (§16), so the attribute is the one carrier that serves every form.
- **What TR1.5 matches.** TR1.5 offers both carriers, and "whichever of the two closes second matches the first". This design fixes:
  - the carrier: `ProcessAttributes.transport_off`;
  - the `auto` key does not vary with the switch, because it covers the native row either way;
  - with the value on, the native row is checked at `SessionOpen`.

**One agent source per session** (TR1.3, reconciling §11's agent row with 1.1.0 S4.3). This rule governs the transport, whose agent client is gwz's own. Each session has exactly one agent source. It is derived from the session's snapshot, never from the server's environment, and it is the endpoint configuration's agent-source field (reuse design §3):
- the snapshot's `SSH_AUTH_SOCK`, when set and non-empty. On Windows it names a pipe or, where 1.1.0 S4.3 proves it, the form dabeest's mingw OpenSSH exposes;
- when it is unset or empty: on Windows, the Windows OpenSSH agent's documented pipe, `\\.\pipe\openssh-ssh-agent`; on Linux and macOS, no agent, and agent authentication fails as it does today.

Further rules:
- **Pageant** is used only when a session selects it explicitly: its `SSH_AUTH_SOCK` names Pageant's named pipe, as `pageant --openssh-config` writes it. It is never probed, and never tried after another source. It is advertised only after a fixture proves it; until then its S5.6 cell is unsupported (§12).
- **No fallback.** A missing or unreachable agent is a refusal, never a fallback to another source or key store (1.1.0 S4.3).
- **Local pipes only.** On Windows, a pipe that `SSH_AUTH_SOCK` names passes a local-only rule of its own before any open. A remote pipe name is refused, not opened over SMB.
  - The rule: the exact-case prefix `\\.\pipe\`, then one component that is not `.` or `..`, holds no `\`, `/`, NUL or control character, and does not end in a dot or a space; at most 256 characters in all; and `GetFullPathNameW` returns it unchanged. §3's `QueryDosDeviceW` check applies too.
  - It admits any other character, unlike §3's address allowlist. Pageant's pipe embeds the Windows user name (`pageant.<user>.<hash>`), which may hold spaces or letters outside ASCII, so the address allowlist would refuse a legitimate Pageant pipe. The OpenSSH agent's default pipe passes both rules.
  - A form other than a pipe, which 1.1.0 S4.3 may admit, passes §11's `--log` path rule before any open instead: an absolute path on a local fixed drive, never a UNC or device path.
  - A refusal under either rule fails the operation with `permission_denied`, the code an unusable agent gets today, in-process as through a server. Its message, the server's log line and any error Python sees name `SSH_AUTH_SOCK` and the rule, never the value (§17), since agent socket paths are on the plan's redaction list.
- **The native path is not bound by this rule,** in-process or through a server. libssh2 takes its agent from the process.
  - On Linux and macOS it uses `SSH_AUTH_SOCK`.
  - On Windows it first tries Pageant, whenever a Pageant window is visible, and only then the pipe that `SSH_AUTH_SOCK` names, or the default pipe (agent.c:436-439; agent_win.c:124-138). It has no Unix-socket agent (agent.c:47-54), so it cannot use a mingw socket form.
  - Through a server, a routed operation therefore uses the server's view: its `SSH_AUTH_SOCK`, which must match, and the Pageant window visible to its logon session, which the native row compares.
  - TR2.8's route decision lists the keys of the session's own agent source. On Windows the native path may then sign with a Pageant key that decision did not see, in-process as through a server.

**How the check runs.** The serving CLI captures its own environment and `umask` once at start, as a driver does, and sets the libgit2 timeout from its own `--ssh-timeout`. It records its OpenSSL library, scans its OpenSSL configuration, resolves its two certificate variables on Linux, and on Windows reads its logon session. It passes all of these to the socket host. Core compares each session's values with them, because core owns the list of process-wide reads. A stdio host does the same with its own process's values, without the scan (§15).

**Process-global state is the server's hazard.** One process hosting many clients' sessions turns every process-global input into a leak between clients. The contract's ratchet inventories that state (§5.7).
- A `debt` entry of kind `env` or `process` makes a server session use the server's environment where the in-process command would use the client's. **Neither CLI releases `server start` while gwz-core's allowlist holds such an entry,** and gwz-py does not release `gwz-py server` while its own allowlist holds one. The session plan's CS4.8 clears gwz-py's. The checker gains a mode that fails on them (§8).
- At gwz-core `bd538656` that is 10 entries:
  - 5 environment reads: repo-inspect's `Environment::Process`, the transport binding, `~/` identity resolution, `with_local_transport`'s snapshot and `SshEndpointConfig`;
  - 5 spawn sites: `git tag`, `git commit`, the local-import fetch, `git rev-list`, and the credential-helper spawn. That last one is git2's `Cred::credential_helper`, not libgit2's (git2 cred.rs:387-400): it runs `sh -c "<helper> get"` with the whole process environment, and `PATH` chooses `sh` and `git`. The session plan's CS3.4 replaces it.
- The static debt is not a hazard in a server: the SSH helper cap and supervisor, and the HTTPS slots. A server has one host context, so per-process scope and per-host-context scope are the same.
- **A dependency writes the environment, where the checker cannot see it.** On Linux, git2's start runs openssl-probe, which writes `SSL_CERT_FILE` and `SSL_CERT_DIR` into the process environment (git2 lib.rs:766-774, 895; openssl-probe lib.rs:119-139). No scanned code performs it, so no allowlist can hold it; §8 adds it to the contract's §5.7 inventory as a disclosed row. The comparison rule above makes it harmless to a server.
- Other dependency reads the checker cannot see are the per-platform tables. The must-match set, not the checker, covers them.

## 6. The `server` command

The same command family exists in both CLIs, as `gwz server` and `gwz-py server`, with the same subcommands and options. It always runs in the CLI's own process. Bare `gwz server` prints the subcommands, as `gwz local` does. §17 gives every synopsis, option, default, help text and message.

| Subcommand | Behaviour |
| --- | --- |
| `server start` | Starts a socket server at the address and returns once it answers, printing the address and how to use it. If a gwz server already answers there, it reports that one and succeeds, unless an option given differs from the running server's, which refuses. `--foreground` keeps the server attached, for launchd, systemd or debugging. |
| `server stop` | Asks the server at the address to stop, with `ServerControl` (§3), then waits for its process to exit, through a process handle taken while the connection is still open, so the process ID cannot have been reused. It waits up to three close bounds plus 5 seconds, 185 seconds: shutdown's own worst case, since no session's `close_wait` may exceed the contract's 60-second default (§3). If nothing answers, it reports "not running" and succeeds. `--force` ends the server's wait for detached workers (below). |
| `server status` | Reports whether a server answers at the address: its process ID, product, core version and build, protocol and open sessions, and the `--max-sessions`, `--idle-exit` and log it runs with; or that it is stopping. It exits 1 when nothing answers. |
| `server list` | Lists this user's servers found in the per-user directory, every key included (below). |
| `server stdio` | Serves exactly one session over its standard input and output (§15). |

- **Options.** Each subcommand takes only its own options, and refuses another's as a usage error: `start` takes `--foreground`, `--max-sessions`, `--idle-exit` and `--log`; `stop` takes `--force`; `status`, `list` and `stdio` take none. `--dry-run` is refused with every subcommand. `--json` and `--jsonl` apply to `start`, `stop`, `status` and `list`, and give the same records.
- **The address** of `start`, `stop` and `status` is `--server`, else `GWZ_SERVER`, else `auto`. `stdio` and the SSH remote form have no listener, so these three refuse them with `server_address_refused`. `list` and `stdio` take no address: `list` reads the per-user directory, and `stdio` neither reads nor parses `GWZ_SERVER`, which the local client form's child inherits as `stdio` (§15). `--no-server` is refused with every subcommand.
- **Pairs.** This mirrors Git's own `fsmonitor--daemon` (`start`, `run`, `stop`, `status`), and pairs `start` with `stop` as `local clone` pairs with `local dispose`. Both directions are idempotent: starting a running server with the same options, or none, and stopping a stopped one, succeed. `start --foreground` is the exception: it exits 1 when the address is taken, since a foreground server that cannot serve must fail its service manager.

**Defaults:**

| Option | Default |
| --- | --- |
| `--server` | `GWZ_SERVER`, else `auto` |
| `--max-sessions` | 64 |
| `--idle-exit` | `off` for `server start`; `10m` for an auto-started server |
| `--log` | with `--foreground`, standard error. Otherwise, and for an auto-started server: in the per-user directory, `server-<key>.log` for `auto` and `pipe-<digest of the name>.log` for an explicit pipe; beside an explicit socket, `<socket path>.log` |
| `--ssh-timeout`, the global option | 9 seconds, in every role: the client, `server start` and `server stdio`, in both CLIs. 0 means no timeout, and is legal for a server as for a client. A server's value is its libgit2 network timeout, which a session's must match like any other must-match value (§5) |

- `--log PATH` is parsed and walked as an address is (§3), without the length limit, and on Windows is checked as §11 says. The file is then opened under §4's file rule. With `--foreground`, `--log` names the file, and standard error stays quiet.
- `--ssh-timeout` has one default because the [retry plan](../gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md) changes it from 1.0.17's 3 seconds to 9, in its §1 default table and in its §8 help text for `--ssh-timeout`; 1.1.0 S7.2's migration notes record the change (§17). A client and a server that both keep the default therefore never meet the timeout refusal.

**Probing first.** `server start` first probes its address as a client would (§3, §4), asking for `ServerState` within the 5-second bound:
- a gwz server answers: `start` compares each option given with the `ServerState`, and reports the running server, or refuses when an option differs (§17). With `--foreground` it exits 1, as a held lock does;
- a gwz server answers that is stopping: `start` fails with `server_unavailable`, naming it;
- a gwz server answers whose protocol range does not include this client's version: `start` fails with `server_version_mismatch`, naming its process (§3);
- on Windows, for `auto`, the recorded pipe does not answer or fails listener verification: that counts as nothing answering, as it does for an `auto` client (§4), so `start` takes the lock and draws a new name. At an explicit pipe or socket address the refusal stands;
- something else answers, or a check refuses: `start` fails with that refusal, having created nothing;
- nothing answers at a socket address where a socket file exists, and the lock file beside it, read under §4's file rule, holds no record naming that address: `start` fails as §4's stale-socket rule would, naming the path, having created nothing;
- nothing answers: `start` starts a server.

**Starting a copy of itself.** The background start, and the stdio local form (§15), start the CLI again, through core's launcher (§2):
- gwz-cli runs its own executable (`current_exe`);
- gwz-py runs its own interpreter (`sys.executable`) on `gwz.cli`, never the `gwz` executable (G10). It passes the running interpreter's isolation flags as `sys.flags` reports them (`-I`, `-E`, `-s`), and `-P` from CPython 3.11. Python composes that argument vector, and the extension spawns it;
- the copy's working directory is not the caller's: the file-system root for gwz-cli (`/`, or the system drive's root on Windows), and for gwz-py the directory that holds the running `gwz` package. `python -m` puts its working directory first on the module path, so a `gwz/` directory in the caller's workspace would otherwise run as the server. Core uses only request paths (§5), so the copy needs no directory of its own;
- **only the standard streams cross.** The launcher closes every other descriptor in the child before the program runs: with `close_range(3, ~0U, CLOSE_RANGE_CLOEXEC)` on Linux, falling back to the descriptors `/proc/self/fd` lists, and with `POSIX_SPAWN_CLOEXEC_DEFAULT` on macOS. On Windows it passes only the handles the child needs, through `PROC_THREAD_ATTRIBUTE_HANDLE_LIST`: the log handle and a null-device handle for the background copy, and the stream's pipe ends and standard error for the stdio child. A lock or a pipe that the caller holds therefore ends with the caller, never with a server;
- when the program cannot be started, the start fails with `server_unavailable`, naming the program and the operating system's error. When `sys.executable` is empty, as in some embedded interpreters, gwz-py cannot name its interpreter, and says so (§17).

**Starting in the background.** The CLI starts a copy with `server start --foreground`, the same options, `--log` with the log path resolved, and `--ssh-timeout`, so that the copy's must-match values define its key.
- The copy runs in a new session, detached from the terminal. Its standard output and error are the log file, which the starting CLI opens under §4's file rule, so output from before the copy opens its own log, such as a crash, is kept. Its standard input is the null device.
- Its environment is the starting CLI's.
- The starting CLI waits up to 5 seconds for the address to answer with a well-formed `SessionHello` and to pass listener verification. If it does not, the start fails with `server_unavailable`. When the copy found the lock held, the message says so, and gives the process ID and log path of the lock file's record, as from the lock file (§4, §17).
- When two CLIs start the same address at once, the lock makes exactly one of them the server, and the other copy exits.

**Serving.** The serving process is a driver in the contract's sense.
- It creates one host context (contract §5.6) and passes it to every session.
  - Concurrent commands on one workspace therefore queue through the host context's workspace registry, instead of failing on the workspace lock (contract §5.1).
  - The host context also owns the endpoint registry, so commands and clients share its connections (reuse design §2, §11).
- One thread accepts connections. Each accepted connection gets the peer check (§4) and the host's `SessionHello`, then a thread of its own, which reads the client's one frame. A `SessionOpen` runs `serve_session`: the contract's reading thread, plus a writer thread that drains its replies to the socket. A `ServerControl` is answered with `ServerState`, and its connection closed.
- A `SessionOpen` beyond `--max-sessions` is refused with `transport_session_full`. `ServerControl` is answered however many sessions are open, so `server status` and `server stop` work at the cap.
- A stuck client stalls no other client: its replies wait in its own session's bounded queue (contract §3).
- In gwz-py, the accept loop and every session run inside the extension with the GIL released. The extension handles `SIGTERM` and `SIGINT` itself.

**Shutdown.** On `ServerControl`'s `stop`, idle exit, or `SIGTERM` or `SIGINT` on POSIX, the server:
1. stops opening sessions. It keeps its listener, so that a client learns why: each new connection still gets the `SessionHello`, a `SessionOpen` is refused with `server_unavailable`, naming the stop, and `ServerControl` is answered with `stopping` true;
2. closes every session through the contract's channel-closure path (§8): it cancels live work, waits up to `close_wait`, and detaches what remains;
3. waits up to one more close bound for detached workers, and logs how many are still running. A stop with `force`, the first or a later one, ends this wait;
4. calls the host context's `shutdown` within the close bound, which disposes every endpoint instance and returns a cleanup report, and logs its pending counts, never configuration values (reuse design §7);
5. removes its socket, clears its lock record, releases its lock, and exits.

`ServerState` answers a stop before step 1.
- **Why the address is released last.** Releasing it before the drain would let a second server run beside the draining one's detached workers, and the file-system workspace lock would not make that safe.
  - Writes and pushes take that lock (contract §5.1), so a second server's write on the same workspace would be refused, not interleaved.
  - But a fetch's member step takes only the member lock, which lives in the host context, and so does the workspace registry that makes sessions wait for a detached worker (contract §5.1, §8). A second server's fetch could then update the refs of a member that a detached worker is still fetching or pushing.
  - Two in-process commands can meet that way today. A server promises its clients that they queue instead (§10), so it holds its address until its workers are gone.
- **The cost.** While a server drains, commands at its address fail with `server_unavailable`, naming the stop, for up to 185 seconds. Under `auto` the client starts a server once, and that copy finds the lock held and exits. `server stop --force` shortens the drain, at the cost its warning names (§17).

**Idle** (TR1.3). `--idle-exit` counts only time with no open session. A `ServerControl` opens no session, so `server status` in a loop does not keep a server alive. Pooled connections do not count as activity, and do not keep a server alive. The idle exit needs no pool rule (reuse design §11):
- the pool closes an idle connection 60 seconds after its release, and disposes an instance that has no binding and no connection (reuse design §7);
- step 4 disposes whatever remains when the server exits;
- an idle exit shorter than 60 seconds only ends reuse sooner. A longer one keeps the process, not its connections;
- the maximum connection age, 10 minutes, is independent of both.

A server that has refused a session or an operation because a file read once has changed (§5) shuts down at its next idle point, whatever its `--idle-exit`, so that a service manager restarts it.

**Logging.** One line per event:
- a connection accepted or refused: the peer's process ID and the reason;
- a session opened or closed: the client version and duration;
- a status or stop request;
- an operation: its method, operation ID, outcome and duration;
- shutdown's pending counts.

The log never contains environment values, request bodies, credentials, remote URLs or repository names. It follows the plan's redaction list (plan §2, as amended): agent socket paths, known_hosts bodies, `gh` tokens and headers, private members' repository names, and agent key comments and fingerprints. A refused agent pipe is logged by the rule it failed, never by its name (§5).
- **Its size.** When the log passes 10 MiB, the server renames it to `<log>.1` in the same directory, through the directory's descriptor, replacing any previous one, and goes on in a new file created under §4's rule. A rename replaces an entry, and never writes through a link planted at `<log>.1`. If the server cannot rotate, it says so in the old file and shuts down at its next idle point. A foreground server that logs to standard error leaves rotation to its service manager.

**Listing.** `server list` reads each lock file in the per-user directory, `server-<key>.lock` and, on Windows, `pipe-<digest>.lock`, under §4's file rule. It parses and walks each recorded address (§3), like any other, before any connect. For each record it probes the address as `server status` does, within 5 seconds, and lists what answers: its address, process ID, version, key and `--idle-exit`.
- A lock file with no record is not listed. On Linux and Windows, a record whose address nothing listens on is not listed either: that server has exited. On macOS a refused connection may also be a full accept queue (§3), so such a record is listed as not answering, with the process ID its record gives, as from its lock file.
- A record whose address the address rules refuse is listed as refused, naming the lock file and the rule, and nothing is connected to.
- A server that does not answer within the bound, whether it accepted the connection or not, is listed as not answering, with the process ID its record gives, as from its lock file, so that it can be killed.
- A server of another core version or build is listed like any other, since `ServerControl` checks only the protocol range (§3). One whose protocol range does not include this client's version is listed as refused, with its range and process ID.
- The server that `--server auto` would use from this environment is marked `auto`. The others belong to another core version or another environment: `auto` never chooses them, and each ends at its idle exit, or when stopped with `gwz server stop --server <address>`.
- A server at an explicit socket address keeps its lock beside its socket, outside the per-user directory, so `list` does not show it; `server status --server <address>` does.
- When this process's OpenSSL scan cannot finish (§5), `list` still lists, without the `auto` mark.

**Crash.** Every client sees its channel close. Before `SessionOpened` that is `server_unavailable`, since no request reached core, and under `auto` the client may start a server once. After it, the call in flight fails with `server_session_closed`. An operation in flight is left as it would be today if the CLI were killed at that moment. A client never retries a call.

## 7. Clients

**Selection, in both CLIs.** The server is opt-in (OD2).
- With none of `--server`, `GWZ_SERVER` or `--no-server`, the CLI runs core in its own process, exactly as today. No default ever starts or selects a server.
- `--server auto` uses the default server for this environment, and starts one (§6) if none answers.
- `--server ADDRESS` uses the socket server at that address, and never starts one.
- `--server stdio` runs the command through the stdio local form (§15).
- `--server ssh://…` runs it through the SSH remote form, if OD12 brings it in (§16).
- `GWZ_SERVER` sets the same default, from `auto`, an address or `stdio`. `--server` overrides it. An empty `GWZ_SERVER` counts as unset.
- `--no-server` runs in-process even when `GWZ_SERVER` is set, and `GWZ_SERVER` is then not parsed. It cannot be combined with `--server`.
- The `hook` family always runs in-process. It never reads `GWZ_SERVER`, and refuses `--server` as a usage error, because tools run it inside their sandboxes, which a server refuses (§4).
- The CLI parses the address at startup, before anything else runs (§3).

If a server was asked for and cannot be used, the command fails; it never falls back to in-process. Each such refusal names its cause, and the CLI's message names `--no-server`.

**Sandboxed callers.** A sandboxed caller that asks for a server, with `GWZ_SERVER` or `--server`, gets a refusal that names the cause and `--no-server`. That holds on all three platforms, at the `auto` path, at an explicit address and in the stdio local form alike: the caller's own check refuses before any connect or start (§4). `--server auto` from inside a detectable sandbox therefore never starts a server. Only the SSH remote form is exempt, for §4's reason.

**What runs where.** Every command that goes through core's shared dispatch (contract §5.2) goes to the server, except the `hook` family's. A CLI keeps what a driver keeps today:
- argument parsing, help and version;
- rendering, including a pager for `diff` and `log`;
- the `server` command itself;
- commands that never open a session, such as `hook claude-code setup`;
- the `hook` family: its file-system probes, and its family verbs, which use an in-process session (session plan CS6.2);
- `forall`'s child commands, which need the user's terminal. The CLI gets their targets from the server with `resolve_forall_targets`, then runs the commands itself, holding the workspace guard as it does today.

§16 lists what the SSH remote form refuses among these.

**One session per command.** A CLI:
1. opens a session;
2. sends its request as contract §11 specifies: a submit followed by event reads when its renderer consumes events, and a unary call otherwise;
3. renders the replies exactly as in-process;
4. sends `session.close` and disconnects.

Output, JSON records, the progress line and exit codes are identical to the in-process run, unless `--verbose` asks to name the server (below). `--ssh-timeout` travels as `configure_transport_runtime`, as in-process.

**The caller's directory.** A CLI captures its working directory once, makes it absolute, and sends it in every request as `InvocationContext.caller_cwd`.
- It resolves `--root` against that directory and sends an absolute `WorkspaceRef.root`. Without `--root`, core finds the workspace from the caller's directory.
- Relative operands are resolved against the same directory, so the server never uses its own.
- Every path is converted exactly or refused (§5). A Python `str` path that holds a lone surrogate, which is how Python represents undecodable bytes from `os.getcwd()`, is refused.
- This is already in the tree: gwz-cli `49d784d`, gwz-py `3778394` and gwz-core `5ca45a66` send paths exactly or refuse them. That replaced the five lossy conversions this design's revision 0 found in gwz-cli.

**Ctrl-C.**
- The first interrupt sends `operation.cancel` for the running call, waits up to the close bound for its reply, and exits as an interrupted command does today.
- A second interrupt disconnects at once. Channel closure then cancels the session's live work (contract §8).
- The stdio mode and the SSH remote form keep the terminal's interrupt away from their child, so this protocol governs there too (§15, §16).

**Python library.** `Client(bridge=SocketCoreBridge(address))` uses a socket server. `address` is required: `auto`, a socket path or a pipe name.
- `SocketCoreBridge` is `NativeCoreBridge`'s call table and pump over the extension's client socket channel, which implements the same `open`, `send` and `recv` (§3). The parse, the walk, listener verification and the handshake run in core, before any byte of the session.
- It captures the environment when it opens a session, as `NativeCoreBridge` does (contract §10). It captures the file-creation mask from `/proc/self/status` on Linux. Elsewhere it sets a mask of `0o077` and restores the old one, so a file another thread creates meanwhile is only more private. It names the OpenSSL library its extension loaded (§3).
- The library never reads `GWZ_SERVER`. Only code that passes a `SocketCoreBridge` uses a server.
- `SocketCoreBridge("auto")` may start a server exactly as gwz-py's CLI does (§6), with the default timeout in its key (§4).
- It takes no timeout. A session that configures its own timeout after it opens has its native-path operations refused under `auto`, as at an explicit address, with a message that names the server's libgit2 timeout and no CLI flag (§5, §17).
- It refuses `stdio` and the SSH remote form with `server_address_refused`: the library gains no client for either in this release.
- A channel that closes before `SessionOpened` fails the open with `server_unavailable`, since no request reached core. One that closes before a reply fails that call with the contract's closed-session error, whose code is `server_session_closed` on every bridge (§8).

**Errors.** This design appends seven codes to `GwzErrorCode` (§9). §17 gives each one's condition and message.

**Diagnostics.** `--verbose` names the server: in human mode, one line on standard error; with `--json` or `--jsonl`, a `server` object in each record's `meta`, as `meta.transport` carries today's `--verbose` diagnostics (§17). Without `--verbose`, output is identical to the in-process run.

## 8. Amendments

To the [core session contract](GwzCoreSessionDesign.md), at revision 5 as the reuse design amends it:
- **§1.** Only if OD12 brings in the SSH remote form: the out-of-scope item "remote deployment" reads "remote deployment, other than the SSH remote form of the server's stdio mode" (amendment §3.11). Otherwise §1 is unchanged.
- **§3.**
  - Tags 4 (`SessionOpen`), 5 (`SessionOpened`), 6 (`SessionHello`), 7 (`ServerControl`) and 8 (`ServerState`) are added. Every byte-stream channel opens with the host's `SessionHello`, then the client's `SessionOpen`, or, to a socket host, its `ServerControl` (this design's §3). The in-process adapter uses none of them.
  - `call_id` 0 is legal only in a refusal before `SessionOpened` or `ServerState`.
  - The client interface is `open(options)`, `send` and `recv` for both adapters.
- **§4.1.** Path fields in requests are Unicode text. A client refuses to send a path that is not valid Unicode, instead of altering it.
- **§4.2.** The error-code paragraph and its bullets name the codes this design adds, and the new uses of two existing ones:
  - "**Error codes.** Two codes are appended to `GwzErrorCode` after `transport_record_limit` (74): `transport_session_full` (75) and `operation_expired` (76). None is redefined. The frames also use the existing `invalid_request`, `operation_not_found`, `open_operation`, `internal_error` and `cancelled` (73)." becomes: "**Error codes.** Two codes are appended to `GwzErrorCode` after `transport_record_limit` (74): `transport_session_full` (75) and `operation_expired` (76). The server design appends seven after them, `server_unavailable` (77) to `server_session_closed` (83) (its §9). None is redefined. The frames also use the existing `invalid_request`, `operation_not_found`, `open_operation`, `internal_error` and `cancelled` (73). Over a byte stream, `transport_session_full` also refuses a `SessionOpen` beyond a socket host's `--max-sessions`, and `invalid_request` also refuses a disallowed handshake, requested limits above the §1 defaults at a socket host, and `ServerControl` at a stdio host (the server design's §3)."
  - "- `protocol/convert.rs` maps the model's `Cancelled`, `TransportSessionFull` and `OperationExpired` to `cancelled`, `transport_session_full` and `operation_expired`, instead of `io_error`." becomes: "- `protocol/convert.rs` maps the model's `Cancelled`, `TransportSessionFull` and `OperationExpired` to `cancelled`, `transport_session_full` and `operation_expired`, instead of `io_error`, and each of the server design's seven errors to its own code."
  - "- At implementation, the `cancelled` (73) schema comment and the ErrorCatalog are updated to say that `cancelled` covers cancels before any effect as well as after." becomes: "- At implementation, the `cancelled` (73) schema comment and the ErrorCatalog are updated to say that `cancelled` covers cancels before any effect as well as after, and each of the server design's seven codes gains a schema comment and an ErrorCatalog entry."
- **§5.1.** Resolution uses only the request's `InvocationContext` and workspace reference. A request that carries `RequestMeta` without `InvocationContext` is refused with `invalid_request` before any effect. Core's `invocation_start` fallback to a supplied start directory serves only legacy direct callers.
- **§5.6.**
  - The snapshot may cross a byte-stream channel once, in `SessionOpen`. It crosses only after the host's `SessionHello`, only to a listener the client has verified, and only where the host has verified its peer: the same user, on the same machine, outside any sandbox. Or it crosses to a stdio process the client itself started. It is never serialized into any other frame, and it is zeroized when the session ends.
  - Over a byte stream, the client supplies the snapshot and the serving process supplies the host context. A `SessionOpen` with the host-environment marker, which only a stdio host accepts, takes the host process's own environment as the snapshot.
  - `open(options)` also takes the off switch's resolved value, `transport_off`, which the snapshot never carries.
  - Every session-path child process gets an explicit working directory, and never inherits the host process's.
  - The sentence "A later change to the process environment does not change a session's credentials or trust context, except …" gains: and, on Linux and macOS, OpenSSL's process-wide reads, which serve the transport's own SSH crypto and, on Linux, its TLS and default roots (`SSL_CERT_FILE`, `SSL_CERT_DIR`).
  - The must-match set lives beside the §5.8 disclosure.
  - The sentence "That covers `git tag`, `git commit`, the local-import `git fetch` fallback, the commit-log path walk's `git rev-list`, and the credential helpers that libgit2 spawns today (§5.8)." becomes: "That covers `git tag`, `git commit`, the local-import `git fetch` fallback, the commit-log path walk's `git rev-list`, and the credential helpers that the git2 crate spawns today (§5.8)."
- **§5.7.**
  - The checker gains a mode that fails on `debt` entries of named kinds. Releasing `server start` requires it to pass with `env` and `process` (§5).
  - The inventory gains a row that no allowlist can hold, since no scanned code performs it: on Linux, git2's start runs openssl-probe, which writes `SSL_CERT_FILE` and `SSL_CERT_DIR` into the process environment (git2 lib.rs:766-774, 895). Disposition: kept, imposed by a dependency; this design's §5 compares the two variables as the probe resolves them.
  - The credential-helper spawn is named for its spawner, git2's `Cred::credential_helper` (cred.rs:387-400), not libgit2, which spawns nothing:
    - "The check fails on any unlisted static, thread-local, environment read, libgit2 global option, process-wide hook or child-process spawn, including the credential-helper spawn libgit2 performs for `Cred::credential_helper`." becomes: "The check fails on any unlisted static, thread-local, environment read, libgit2 global option, process-wide hook or child-process spawn, including the credential-helper spawn the git2 crate performs for `Cred::credential_helper`."
    - The table row that begins "libgit2's credential-helper spawn, `Cred::credential_helper` (`transport_support.rs`)" begins instead "The git2 crate's credential-helper spawn, `Cred::credential_helper` (`transport_support.rs`)".
    - gwz-core's allowlist entry for it is reworded to match.
- **§5.8.** Its disclosure of libgit2's own reads is replaced by the enumerated list, per platform, in this design's §5: every environment read that libgit2, its TLS stream and libssh2, with their crypto backends, make, OpenSSL's start-up reads included, each with its reader, when it is read and what it governs; and the names compiled in but unreachable, each with its reason. The list is kept beside the must-match set, so the two cannot drift, and a source-level test pins it (§12). The disclosure also states:
  - that the home directory fixes native SSH's known_hosts path as well as global configuration, and that this file is the native path's only host-key check, since gwz registers no `certificate_check`;
  - that the credential helpers are spawned by the git2 crate, not by libgit2. The sentence "Today the default backend's credential callback calls `Cred::credential_helper`, and libgit2 spawns git's configured helpers with the live environment." becomes: "Today the default backend's credential callback calls `Cred::credential_helper`, and the git2 crate spawns git's configured helpers with the live environment (git2 cred.rs:387-400); libgit2 spawns nothing.";
  - that on Linux and macOS the process's OpenSSL also serves the transport's own SSH crypto, and on Linux its TLS, so its reads concern every session, not only native remotes;
  - that OpenSSL's configuration can expand any variable (`$ENV::`), which this design's §5 closes at run time for a server;
  - that on Windows libssh2 tries a visible Pageant window before `SSH_AUTH_SOCK`'s pipe;
  - that these reads are process-wide in a server, and that a server refuses a session or an operation whose values differ from its own (this design's §5).
- **§9.** The extension also exposes the socket host and the stdio host, for `gwz-py server`; the client socket channel, for `SocketCoreBridge`; the byte-stream client channel, for gwz-py's CLI (§15); and core's launcher, which spawns gwz-py's copies of itself and its `ssh` (§2).
- **§10.**
  - gwz-py gains `SocketCoreBridge`, which implements the same bridge methods over a socket channel, with `NativeCoreBridge`'s call table and pump.
  - "- When `recv()` returns `None`, every outstanding call fails with a typed closed-session error." becomes: "- When `recv()` returns `None`, every outstanding call fails with a typed closed-session error. Its code is `server_session_closed` (83) on every bridge, `NativeCoreBridge`, `StreamCoreBridge` and `SocketCoreBridge`, and the pump's step 3 below fails calls with the same error. Over a byte stream, a channel that closes before `SessionOpened` fails the open instead, with `server_unavailable` (77)."
- **§11.** gwz-cli may send its session to a socket server, to a stdio child or, if OD12 brings it in, over `ssh`. It may also host a server (§6, §7, §15 and §16 of this design).
- **§12.** Both CLI suites also run over a socket (§12 of this design). The stdio test host opens its session with the handshake, as the stdio mode does: it sends `SessionHello` first, and `StreamCoreBridge` sends `umask` and `openssl` through the extension, as `SocketCoreBridge` does, which the test host checks (§3 of this design). `StreamCoreBridge` keeps the Cargo example host; §15 of this design says why.
- **§13.**
  - A bullet follows the first: "The server design's handshake and control frames, `SessionHello`, `SessionOpen`, `SessionOpened`, `ServerControl` and `ServerState`, at tags 4 to 8, with `EnvironmentEntry`, `ProcessAttributes`, `ServerAction` and `SessionLimits` (its §3 and §9)."
  - "- Two error codes are appended to `GwzErrorCode`: `transport_session_full` (75) and `operation_expired` (76). None is redefined." becomes: "- Two error codes are appended to `GwzErrorCode`: `transport_session_full` (75) and `operation_expired` (76). The server design appends seven more, 77 to 83 (its §9). None is redefined."
  - "- `protocol/convert.rs` maps three model codes to their own wire members (§4.2). The `cancelled` comment and the ErrorCatalog are updated at implementation." becomes: "- `protocol/convert.rs` maps three model codes, and the server design's seven, to their own wire members (§4.2). The `cancelled` comment and the ErrorCatalog are updated, and the seven codes added to both, at implementation."
- **§15.**
  - In §15.1, "   - A stream-bridge test sends a duplicate outstanding `call_id`, and another sends a lower one. Each ends the session, and the original call fails with the closed-session error, never `invalid_request`." becomes: "   - A stream-bridge test sends a duplicate outstanding `call_id`, and another sends a lower one. Each ends the session, and the original call fails with the closed-session error, `server_session_closed`, never `invalid_request`."
  - §15.2's round trip of "each code this contract names" then covers the seven codes, which §4.2 names.
- **§16.** The transport build is supported on every platform gwz ships on, Windows included (§11 of this design), so its per-session behaviour holds everywhere.

To the [proposals](GwzClientCoreTransportProposals.md): **G1** reads "Core starts no server or daemon on its own. A driver may host one on request, and no deployment may require one." The operator adopted this wording on 2026-09-27 (OD9).

The reuse design's server edits (its §13) are applied in this revision: §1 and §10 bring reuse into scope, §4 carries both of its clauses, and §6's shutdown gains the disposal step. The reuse design's own §11 composition note needs one correction, since the `auto` key now covers the agent source; it is in the "On GO" list below.

To the [plan amendment](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md), accepted. This design meets its §3.4 rule that "Each probe neither follows a symbolic link nor triggers a mount", but not the macOS probe it names, which would mount a direct-map trigger (§3). And its §3.6 macOS rows rest on `/net`, which macOS no longer enables by default. These replacements correct the text:
- **§3.4, the Linux probe** (line 140). "On Linux: `statx` with `AT_SYMLINK_NOFOLLOW | AT_NO_AUTOMOUNT`, refusing a component that carries `STATX_ATTR_AUTOMOUNT`, and `fstatfs` on an `O_PATH | O_NOFOLLOW` descriptor for the file-system type." becomes: "On Linux: `openat` of the single component with `O_PATH | O_NOFOLLOW`, then `statx` on that descriptor with `AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW | AT_NO_AUTOMOUNT`, refusing a component that carries `STATX_ATTR_AUTOMOUNT`, and `fstatfs` on the descriptor for the file-system type. `STATX_ATTR_AUTOMOUNT` marks the kernel's own automount points and never an autofs trigger, which only the file-system type catches, so both checks stay."
- **§3.4, the macOS probe** (line 141). "On macOS: `open` with `O_SYMLINK` or `O_NOFOLLOW`, and `fstatfs` on the descriptor. TR1.3 establishes how a direct-map trigger presents." becomes: "On macOS: `getattrlistat` with `FSOPT_NOFOLLOW` first, refusing a component whose mount status carries `DIR_MNTSTATUS_TRIGGER`; `readlinkat` only for a component that probe showed to be a link; and `openat` with `O_SEARCH | O_NOFOLLOW` only for a directory it showed not to be a trigger, then `fstatfs` and `fgetattrlist` on the descriptor, whose file-system and file IDs must match the probe's. Any `open`, whatever its flags, resolves a trigger at the last component, so `open` never comes first. A direct-map trigger presents as an autofs mount, `MNT_AUTOMOUNTED` and not `MNT_LOCAL`, with `DIR_MNTSTATUS_TRIGGER` (TR1.3)."
- **§3.4, `/net`** (line 143). "macOS's default `/net` map is one such mount. TR1.3 lists the file-system types." becomes: "macOS's `/net` map is one such mount when it is enabled, and so is its default auto_home map, reached through `/home`; `/etc/auto_master` has shipped `/net` commented out since autofs-281.0.3. TR1.3 lists the file-system types."
- **§3.6, the macOS rows** (line 242). "on macOS, an address under `/net`, with no lookup below `/net`, and an address whose component is a symbolic link, in a local directory, that points under `/net`, with no lookup below `/net`." becomes: "on macOS, an address under the default auto_home map through the `/home` symbolic link, such as `/home/gwz.invalid/s`, with no lookup below the map, as a recording double shows; and, on a disposable runner with `/net` and a direct map enabled, an address under `/net`, an address whose component is a symbolic link, in a local directory, that points under `/net`, and an address under a direct-map trigger, each with no lookup below the map and no mount, beside a control `open` of the trigger that shows the old probe mounts it."

**On GO,** these edits follow under AgentProcessRules §7.2, each with a changelog entry:
- the contract's status gains "Amended <date> by `GwzCoreServerDesign.md`. This document remains authoritative only as amended for §3, §4.1, §4.2, §5.1, §5.6, §5.7, §5.8, §9, §10, §11, §12, §13, §15 and §16", with §1 as well if OD12 is yes;
- the proposals' G1 row takes OD9's wording;
- gwz-core's GWZDesign and GWZRequirements gain paired paragraphs for the socket and stdio hosts, as gwz-core's `AGENTS.md` requires before core behaviour expands. GWZRequirements' REQ-011, "daemon use MUST NOT be required", stands;
- GWZDesign's paired "Core session host" paragraph names the git2 crate as the credential helpers' spawner, as the contract's §5.6 to §5.8 do once amended (above):
  - "`scripts/checks/check_process_globals.py` fails gwz-core's test run, which covers gwz-transport too, and gwz-py's test run on any new static, thread-local, environment read, libgit2 global option, process-wide hook or child-process spawn, including the credential-helper spawn libgit2 performs." becomes: "`scripts/checks/check_process_globals.py` fails gwz-core's test run, which covers gwz-transport too, and gwz-py's test run on any new static, thread-local, environment read, libgit2 global option, process-wide hook or child-process spawn, including the credential-helper spawn the git2 crate performs for `Cred::credential_helper`.";
  - "There libgit2 also still reads the SSH agent socket and the home directory itself, while core runs git's credential helpers itself with the session's snapshot instead of letting libgit2 spawn them." becomes: "There libgit2 also still reads the SSH agent socket and the home directory itself, while core runs git's credential helpers itself with the session's snapshot instead of letting the git2 crate spawn them.";
- the release plan's Phase 10 post-release check takes the subcommand spelling (§6), with a status sentence naming this design for that line: "`server --start`, `--status` and `--stop` from each installed CLI. Every server the check starts is stopped;" becomes: "`server start`, `server status` and `server stop` from each installed CLI. Every server the check starts is stopped;";
- the reuse design's §11 "Two partitions compose" note follows this design's `auto` key, which covers the agent source whatever the off switch (§4, §5): "Under `auto`, HOME is equal across a server's sessions, so its instances differ by agent, CA, proxy and timeouts. TR1.3 records this." becomes: "Under `auto`, the home, the agent source and a CLI session's timeouts are equal across a server's sessions, so its instances differ by CA content, proxy, no-proxy list and a library session's timeouts. The server design records this (SRV §4)." The reuse design's status gains "Amended <date> by `GwzCoreServerDesign.md`. This document remains authoritative only as amended for §11's "Two partitions compose" note.", and its changelog an entry;
- OD12's answer, and the edits it brings, are recorded in the plan amendment's changelog (amendment §3.11);
- the plan amendment takes the four replacements above, its §3.4 probe and `/net` sentences and its §3.6 macOS rows, with a changelog entry and a status sentence naming this design for those clauses. This design's review covers them;
- TR1.4b places this design's steps in the release plan's Phase 7.

## 9. Schema additions (append-only)

- `SessionHello`, `SessionOpen`, `SessionOpened`, `ServerControl`, `ServerState`, `ServerAction`, `EnvironmentEntry` and `ProcessAttributes`, with frame tags 4 to 8 (§3).
  - None has shipped. Their field numbers follow revision 0's.
  - Revision 1 appended `SessionOpen.host_environment` (field 6), `ProcessAttributes.transport_off` (field 2) and `ProcessAttributes.openssl` (field 3), and made `SessionOpen.environment` (field 3) optional.
  - Revision 2 adds `SessionHello` (tag 6), and `ServerControl` and `ServerState` (tags 7 and 8) with `ServerAction`. They replace revision 1's `server.stop` method, `ServerStopRequest` and `ServerStopResponse`: a stop that opened a session met the must-match check, so it could not stop a server of another environment, such as one `server list` shows.
- `SessionLimits`: the contract's §1 limits as optional fields, validated by `open` as today. A socket host's maxima are the §1 defaults (§3).
- Error codes appended after `operation_expired` (76):
  - `server_unavailable` (77);
  - `server_version_mismatch` (78);
  - `server_environment_mismatch` (79);
  - `server_peer_refused` (80);
  - `server_address_refused` (81);
  - `server_listener_refused` (82);
  - `server_session_closed` (83).
- No change to existing messages, or to gwz-transport.

## 10. What a server changes for a user

- **Concurrency.** Commands from several terminals on one workspace queue instead of failing with "workspace mutator lock is already held", because they share the server's host context.
- **Reuse.** A later command uses an idle connection that an earlier command, or another client, left in the server's host context. That needs equal endpoint configurations, and the revalidation the reuse design requires. The host context's SSH setup supervisor also lives across commands. Under the 60-second idle default, reuse helps commands that run within a minute of each other (reuse design §7; its §16.1 decision 1).
- **Failure scope.** In-process, a crash or a detached worker affects only its own command. In a server, it affects every client, and so does a server that stops answering: each client then fails at the handshake's bound, naming the server's process (§3). An endpoint-instance fault that ends a physical owner's thread fails the active exchanges of every client bound to that instance (reuse design §8).
- **The stdio mode** changes none of these: one command per process, and reuse within that command, as in-process (§15).

## 11. Platforms

Windows has the same functionality as Linux and macOS:
- the `server` command, auto-start, the stdio mode, `--server` and `SocketCoreBridge`;
- the handshake, the trust and sandbox rules, and listener verification;
- the must-match rule, shutdown and logging;
- the transport build, with its per-session SSH agent, timeouts and cancellation.

Only the operating-system primitives differ:

| Function | Linux and macOS | Windows |
| --- | --- | --- |
| Listener | A Unix-domain socket in the per-user directory. | A named pipe. For `auto`, `\\.\pipe\gwz-` and 32 hexadecimal digits from the system's cryptographic generator, drawn at each start, and recorded in the lock file once the pipe exists (§4). |
| Per-user directory, holding the lock, the log and, on POSIX, the socket | `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`, parsed and walked (§3). Mode 0700, owned by the user. | `gwz\` under the user's local application data folder, from `SHGetKnownFolderPath(FOLDERID_LocalAppData)`, not from the `LOCALAPPDATA` variable. It must be on a local fixed drive (`GetDriveTypeW`) and must not be a UNC or device path, or the host refuses it with `server_address_refused` before any open. Owned by the user, with an ACL that grants access only to the user, SYSTEM and Administrators. The files in it follow §4's file rule. |
| `--log PATH` | Parsed and walked (§3), then opened under §4's file rule. | An absolute path on a local fixed drive; a UNC or device path is refused before any open. Then §4's file rule: `FILE_FLAG_OPEN_REPARSE_POINT`, no reparse point, the user as owner, and appending only. |
| No other process claims the address first | The private directory and the lock. | `FILE_FLAG_FIRST_PIPE_INSTANCE`, the lock, and for `auto` a name drawn at random at each start, which only the lock record names (§4). |
| No remote clients | Unix sockets are local. | `PIPE_REJECT_REMOTE_CLIENTS`, and a security descriptor that grants only the user's SID. The client's grammar admits only one local pipe name, and is the only guard against a remote one, since Windows ignores `SECURITY_IDENTIFICATION` on a remote pipe (§3). |
| Peer's user, host side | `SO_PEERCRED`, and `SO_PEERPIDFD` where the kernel has it, on Linux; `getpeereid` and `LOCAL_PEERTOKEN` on macOS. | `GetNamedPipeClientProcessId`, then a handle to that process, whose creation time must precede the connection, and the user SID in its token. |
| Listener's user and process, client side (§4) | The walk, the directory and socket-file checks, then `SO_PEERCRED` and `SO_PEERPIDFD` on Linux, or `getpeereid` and `LOCAL_PEERTOKEN` on macOS. | The grammar and its checks, `SECURITY_SQOS_PRESENT \| SECURITY_IDENTIFICATION` at open, then `GetNamedPipeServerProcessId`, a handle to that process, whose creation time must precede the connection, and its token, whose integrity level must equal the client's. |
| Sandbox rule (§4) | On Linux, a `Seccomp` status other than 0, or a user namespace other than the initial one; for a peer, also another mount or user namespace. On macOS, the sandbox check on the audit token. | An AppContainer token, or integrity below medium; for a client, also below the server's own; for a listener, any integrity other than the client's. |
| One server per address | `flock` on the lock file, which stays after exit and, while held, records its holder's process ID, address and log path (§4). | `LockFileEx` on the lock file, with the same record: `server-<key>.lock` for `auto`, `pipe-<digest of the name>.lock` for an explicit pipe. The lock covers a byte range past the record, since Windows byte-range locks are mandatory and readers must still read the record. |
| Stale listener | A leftover socket file the user owns, not a link, removed under the lock only when a connection to it is refused and the record its lock file held names exactly that socket (§4). Any other file, a socket that answers, and a refused socket that no record names refuse the start, and nothing is deleted. | None: a pipe disappears with its server. A lock record whose pipe does not answer, or fails listener verification, is stale: an `auto` client starts a server once, and `server start` for `auto` takes the lock and draws a new name (§4, §6). |
| Background start | A new session (`setsid`), with standard output and error on the log, and no other descriptor (§6). | `DETACHED_PROCESS` and `CREATE_NEW_PROCESS_GROUP`, with only the log handle and a null-device handle passed (`PROC_THREAD_ATTRIBUTE_HANDLE_LIST`). The copy leaves the caller's job object where the job allows it. |
| The stdio child (§15) | A new session (`setsid`), so the terminal's interrupt never reaches it; only its standard streams cross (§6). | `CREATE_NEW_PROCESS_GROUP`, which disables Ctrl-C for it; it also ignores `CTRL_BREAK_EVENT`. Only the pipe ends and standard error are passed. |
| Stopping a foreground server | `server stop`, `SIGTERM` or `SIGINT`. | `server stop`, Ctrl-C or Ctrl-Break. |
| File-creation permissions | The process `umask`, which must match. | Nothing process-wide: new files inherit their directory's ACL, in a server as in-process. |
| Values read process-wide (must match) | The must-match set's rows (§5): libgit2's start, the mask, OpenSSL and the loader, the timeout, and `SSH_AUTH_SOCK` for the native path. | libgit2's start, with `HOME`, `HOMEDRIVE`, `HOMEPATH`, `USERPROFILE`, `XDG_CONFIG_HOME`, `APPDATA`, `LOCALAPPDATA`, `PROGRAMDATA` and `PATH`; the timeout; and, for the native path, `SSH_AUTH_SOCK` and the logon session. No OpenSSL. |
| SSH agent, transport build | The session's one agent source (§5): `SSH_AUTH_SOCK` from the snapshot. | `SSH_AUTH_SOCK` from the snapshot, else the Windows OpenSSH agent's pipe, `\\.\pipe\openssh-ssh-agent`. Pageant only when `SSH_AUTH_SOCK` names its pipe. |
| SSH agent, native path (libssh2) | `SSH_AUTH_SOCK` from the process. | A visible Pageant window first, then the pipe `SSH_AUTH_SOCK` names, else the default pipe; no Unix-socket agent (§5). |

**The transport build on Windows.** Core gates its transport build to Unix today (`cfg(all(unix, gwz_transport_candidate))`). A Windows server would then have only ordinary builds: an SSH agent that must match the server's, process-wide timeouts and no network cancellation. This design requires the transport build on Windows, which the release plan's Phase 4 builds (1.1.0 S4.1–S4.5).

The Unix-only code is in core's SSH endpoint, not in gwz-transport, which does no I/O. Each piece has a Windows counterpart:

| Unix code | Where | On Windows |
| --- | --- | --- |
| Non-blocking TCP connect, polled with `libc::poll` (`EINPROGRESS`) | `ssh_network.rs` | `WSAPoll`. |
| The ssh-agent client, a Unix socket polled with `libc::poll` | `agent_socket.rs` | The session's one agent pipe (§5). |
| Files opened with `O_NONBLOCK`, so a FIFO can't block the read: `known_hosts` and key files | `ssh_network.rs`, `ssh_key_snapshot.rs`, `identity.rs`, `ssh_worker.rs` | Check that the path is a regular file before opening it. |
| Key-file permission checks by mode (`PermissionsExt`) | key admission | Checks by ACL, with the rule OpenSSH applies there: no access for other users. |
| `known_hosts` and default paths from `HOME` | the transport host's configuration | `USERPROFILE` when `HOME` is unset, as Windows OpenSSH does. |
| The signature buffer handed to libssh2, from `libc::malloc` | `agent_auth.rs` | Allocated with the allocator libssh2 frees with. |
| Byte conversions of paths and environment values (`OsStrExt`, `OsStringExt`) | several | WTF-8, as contract §5.6 already specifies for the snapshot. |

gwz-py's candidate gate, `cfg(all(unix, gwz_transport_candidate))`, lifts with core's.

## 12. Verification

Each repository tests against what it depends on. Rows the plan amendment's §3.6 and the release plan's Phase 7 exit name are marked "(Phase 7 exit)".

On Linux, every row that runs a server, the stdio local form included, runs where the test's own processes have a `Seccomp` status of 0 and the initial user namespace, such as a virtual-machine runner, and never in a container job, whose default seccomp profile would refuse it (§4, §14). That hosted runners' job processes meet this is inferred, so each such job's first step asserts it. The sandbox rows build their sandboxes inside that job.

**gwz-core.**
- `serve_session` over a socket pair covers:
  - handshake order: the host's `SessionHello` first, then the client's one frame, answered by `SessionOpened` or by `ServerState`; a second frame from the client before `SessionOpened` is a protocol error, and so is a `SessionOpened` whose identity differs from the `SessionHello`'s;
  - `call_id` 0 before and after `SessionOpened`;
  - the version range, refused on the `SessionHello` before the client sends anything;
  - `SessionLimits`: at a socket host a limit above its §1 default is refused with `invalid_request` at open, and one at or below it is accepted; a stdio host applies `open`'s validation alone;
  - `ServerControl`: `status` and `stop` are answered by a socket host, at `--max-sessions` too, and refused with `invalid_request` by a stdio host;
  - relative paths: an empty `XDG_CONFIG_HOME` and a relative `OPENSSL_CONF` are refused at open, by a socket host and by a stdio host, and absolute values pass; a socket host whose own `XDG_CONFIG_HOME` is empty refuses to start;
  - an empty `SSH_AUTH_SOCK`: a client and a socket host with it set but empty open a session, the transport reports no agent, and under `auto` a server starts. An empty `SSLKEYLOGFILE` passes too, and a non-empty relative value of either is refused;
  - §3's table: a socket host refuses a `SessionOpen` with no snapshot, and one with the marker, with `invalid_request` before `SessionOpened` (Phase 7 exit); a stdio host takes the marker only without a snapshot;
  - must-match refusals at `SessionOpen` and at routing, each naming the value and never its content;
  - a session whose timeout differs from the server's: its transport remotes run, and a `git://` or routed operation is refused before any connection opens, whether the session called `configure_transport_runtime` or kept the default;
  - `SSL_CERT_FILE` and `SSL_CERT_DIR` compared as openssl-probe resolves them: unset against the resolved default passes, a different existing path is refused, and snapshots taken before and after git2's start are both accepted by a host whose probe filled the variables. A host resolves its own values once: a bundle created after its start does not make a client that resolves to it pass;
  - the comparison's forms: unset and empty stay distinct, and on Windows names match without regard to case;
  - a client whose `openssl` attribute differs from the host's is refused at `SessionOpen`;
  - a client whose `LD_PRELOAD` on Linux, or `DYLD_INSERT_LIBRARIES` on macOS, differs from the host's is refused at `SessionOpen`;
  - a client whose real and effective user IDs differ refuses before it connects, shown with injected IDs;
  - a session's off switch taken from `transport_off`, never from the snapshot or the host's environment;
  - a snapshot that never appears in any frame after `SessionOpen`;
  - zeroization at session end;
  - a request with `RequestMeta` but no `InvocationContext`, refused with `invalid_request` both in-process and over a socket.
- **The handshake's bound,** on all three platforms:
  - a host double that accepts, passes the peer check and never answers: a client command fails within 30 seconds, and `server status` and `server stop` within 5, each with `server_unavailable` naming the double's process ID, the lock file and the log path; no frame follows the connect;
  - a double that sends a well-formed `SessionHello` and never answers `SessionOpen` fails the client within 30 seconds, naming its process;
  - an echo double and a silent double at an explicit address receive no environment byte; a double whose `SessionHello` gives another core version or build is refused with `server_version_mismatch`, and receives none either;
  - a first frame longer than 4 KiB, or with another tag, fails at once with `server_unavailable`, showing its first bytes escaped;
  - a listener double that never accepts, with its accept queue filled first: on Linux a client command fails within 30 seconds, and `server status` within 5, each with `server_unavailable` naming the process ID from its lock file; on macOS, where the full queue refuses at once, the row asserts the message for nothing answering, with the process ID and log its lock file gives.
- **The address parser,** in CI on each platform, refuses each form §3 refuses with `server_address_refused`, before any open, and a test double for the connector records no call (Phase 7 exit):
  - Windows: a corpus of every network form in §3's list; `\\?\pipe\x`, `\??\pipe\x`, `\\.\PIPE\x`, `\\.\pipe\x.`, `\\.\pipe\x ` and `\\.\pipe\a\b`; an empty rest, a NUL, U+FF3C and an over-length name;
  - Windows agent pipes: a Pageant-style name holding a space and a letter outside ASCII passes the agent rule and fails the address rule; a remote agent pipe is refused before any open;
  - Linux: an abstract name; an empty, `.` or `..` component; a trailing `/`; a path of 108 bytes;
  - macOS: `/.vol/1/2/s`, `/.nofollow/…` and `/.resolve/4/…`; a path of 104 bytes;
  - every platform: `stdio` and `ssh://` from `SocketCoreBridge`, `ssh://` from `GWZ_SERVER`, and both given to `server start`, `stop` and `status`.
- **The walk on Linux,** on a CI runner with `sudo` (Phase 7 exit):
  - an autofs program map at `/gwz-ind` (`auto.gwz-prog`) that appends each key it is asked for to a file: an address below it is refused, and the file stays empty;
  - a systemd transient `--automount` unit, a direct trigger: an address below it is refused, the journal holds no "Got automount request for" its path, and the unit stays "waiting";
  - `/sys/kernel/debug/tracing`, as root where `TRACEFS_AUTOMOUNT_DEPRECATED` is set, refused for `STATX_ATTR_AUTOMOUNT`;
  - loopback NFS, SMB and sshfs mounts, each refused by type, and a fake `statfs` layer for the rarer types;
  - a procfs link component is refused, and a ninth link refuses;
  - the connect through `/proc/self/fd` reaches the walked socket after its directory is swapped for a link, and without `/proc` the client refuses before any connect, as §3 and §14 state;
  - the host refuses to create its per-user directory under a refused path.
- **The walk on macOS:**
  - on every macOS runner: `/home/gwz.invalid/s` walks the `/home` link and is refused at auto_home's root, with no network, and a recording double shows no lookup below the map (Phase 7 exit, as §8 corrects it);
  - on a disposable runner with `/net` enabled (`sudo automount -cv`): `/net/gwz-fixture.invalid/s`, and a local link to it, are refused with no lookup below `/net`; `.invalid` never resolves (Phase 7 exit, as §8 corrects it);
  - on the same runner, a direct map (`/- auto_gwz`, one NFS entry at `gwz-fixture.invalid`): its trigger shows `DIR_MNTSTATUS_TRIGGER`, an address below it is refused with no mount, and a control `open` of the trigger shows that the amendment's former probe mounts it;
  - a loopback `nfsd` mount, refused by type;
  - a CLI holds `/dev/autofs_notrigger` across its walk and connect, and `SocketCoreBridge` does not;
  - the host refuses to create its per-user directory under a refused path.
- **Windows, on dabeest:**
  - `GetFullPathNameW` returns every name the grammar accepts unchanged;
  - `GetNamedPipeServerProcessId` works on a client's handle; if it does not, listener verification fails closed, and §14's fallback is tested instead;
  - a medium-integrity pipe server passes, and low-integrity and AppContainer servers are refused;
  - an elevated client against a medium server is refused before any frame, and a medium client against an elevated server is refused by the server;
  - an `SSH_AUTH_SOCK` in a form other than a pipe, which 1.1.0 S4.3 may admit, naming a UNC path, is refused before any open.
- **Process IDs:** a peer or listener whose process began after the connection, shown with an injected start or creation time, is refused on each platform; on Linux 6.5 and later the check holds `SO_PEERPIDFD`'s descriptor, and on macOS it reads the audit token.
- **Listener verification,** on all three platforms, each refusal `server_listener_refused`:
  - a listener owned by another user, at the `auto` path and at an explicit address, receives zero bytes (Phase 7 exit);
  - a listener of the same user inside a sandbox, at the `auto` path and at an explicit address, receives zero bytes: under `unshare` or bubblewrap, and under a seccomp filter, on Linux; under `sandbox-exec` on macOS; an AppContainer and a low-integrity pipe server on Windows (Phase 7 exit);
  - on Linux, two seccomp-only sandboxes of equal filter count in one namespace: a client in either refuses itself before any connect, a listener double in either is refused by an unsandboxed client, and `server start` in either refuses to start;
  - the detection of the initial user namespace: a process in it is not flagged, and one under `unshare -U` is (the inode rule is inferred);
  - a per-user directory with another owner, a group bit, or a symlink is refused before any connect.
- **The socket host** covers:
  - the directory, owner and permission refusals;
  - the file rule, on Linux and macOS: a symbolic link, and separately a FIFO, planted at the log and at the lock before `server start`, and a link named by `--log`. The start refuses within 5 seconds, and the link's target is unchanged in size and hash. A lock or log with a group bit, or with another owner, is refused too;
  - the lock and its record: a second `server start --foreground` while a server runs exits 1, naming its process ID and log path; with the holder gone and the lock free, the file and its old record still present, the start proceeds; a clean exit clears the record, and the lock file survives exit;
  - the record's trust: with a lock file forged to name a living unrelated process and a foreign address, and its lock held by a test process, `server start` reports the lock held with no gwz server answering, naming that process as from its lock file; `list` lists it as not answering; no connect leaves the walk's rule; and a FIFO at the lock file's path blocks neither `list` nor a client at the handshake's bound;
  - the stale socket: at an explicit address, a test daemon listening on a user-owned socket makes `server start` refuse, create no lock file, and leave the daemon's socket answering. A socket file with no listener and no record naming it makes `server start` refuse, naming the path, and survives; on macOS so does a foreign program's socket whose accept queue is full, which refuses like a dead one. gwz's own stale socket at an explicit address, whose lock file's record names it, is removed, and the start proceeds; under `auto` a crashed server's socket is removed as before. A non-socket file at the address survives;
  - the peer check;
  - the sandbox refusal: a client double that skips its own check, under `unshare` or bubblewrap, or under a seccomp filter, on Linux, and under `sandbox-exec` on macOS, refused by the host's peer check. The host's own namespaces and filters are varied with a server double, since a real `server start` refuses inside a sandbox; and `server start` inside a sandbox is refused on all three platforms;
  - descriptors: a starter that holds an `flock`ed descriptor and a pipe's write end starts a server. After `server start` returns, another process can take the lock, and the pipe's reader sees end of file. The same holds for the stdio child;
  - the log's bound: a log past 10 MiB is renamed to `<log>.1`, and a link planted at `<log>.1` is replaced, never followed;
  - on Windows: a remote client, an AppContainer client, a low-integrity client, a medium-integrity client of an elevated server, a per-user directory whose ACL grants another user, and a `LOCALAPPDATA` that names a UNC path, which is not used; a junction and a symbolic link at the log path, each refused; a pipe that another user pre-creates under any name does not stop the user's `auto` server, and a pipe it creates under a name it saw in the listing, after that server exited, is never used, since the next server draws a new name. With such a stale record, `gwz server start` starts a server under a new name and exits 0, and `server status` finds it. A unit row asserts that the name comes from the system's cryptographic generator, and that a pipe created first under the first drawn name makes the host draw another and succeed;
  - the `auto` key: equal environments give equal keys; each must-match row changes it; an in-place edit of a scanned file changes it; the off switch does not.
- **The stdio host** (Phase 7 exit, with the CLI rows below):
  - end of input cancels live work, and the process exits with no helper left;
  - a child that writes to its standard output or reads its standard input cannot touch the stream;
  - it sends `SessionHello` first, and refuses `ServerControl` with `invalid_request`.
- **Native routes through a server,** at an explicit address (Phase 7 exit):
  - A server and a client have different `SSH_AUTH_SOCK` values, and the client's agent holds a key TR2.8 routes native. The client's SSH operation is refused before any connection opens, and the server's agent records zero signature requests.
  - On Linux, a server and a client have different `SSL_CERT_FILE` values, and the client's private HTTPS member takes TR1.6's native route. It is refused before any connection opens, with `server_environment_mismatch` naming `SSL_CERT_FILE`, and the disposable TLS fixture records no handshake. On Linux that variable is on every session's row (§5), so this refusal comes at `SessionOpen`. The `SSH_AUTH_SOCK` row exercises the refusal at routing.
  - A source-level test asserts that the must-match list holds every name in §5's per-platform tables. It also scans the vendored libgit2, libssh2, openssl-probe and git2's `lib.rs` for environment reads, and fails on a name that is in neither the list nor §5's exclusion table.
- **OpenSSL's configuration,** on Linux and macOS:
  - with `OPENSSL_CONF` naming a test configuration whose include holds `$ENV::` of a test variable, the name joins the set: a client with another value is refused at `SessionOpen`, and under `auto` it gets another server;
  - an include that cannot be read, includes nested more than 8 deep, a FIFO, or a file over 1 MiB refuse the start within 5 seconds, with `server_unavailable` naming the file;
  - at an explicit address, a configuration file changed after the start refuses the next session, naming the file and `gwz server stop --server <address>`, and the server exits when its last session closes;
  - under `auto`, an in-place edit of `openssl.cnf` starts a new server, and no `server_environment_mismatch` arises;
  - a client whose scan cannot read an include refuses `--server auto` before any connect, and no server starts;
  - on Linux, a CA bundle changed after the start refuses a native HTTPS operation, and not a transport one.
- **Windows' logon session:** a client in another logon session of the same user has its transport operations run, and a routed operation refused before any connection opens, naming the logon session.
- Shutdown disposes every endpoint instance within the close bound, and logs pending counts without configuration values (reuse design §15 item 11).
- The checker's new mode fails on an `env` or `process` debt entry.

**gwz-cli.**
- CI runs its suite twice: once in-process, and once with `GWZ_SERVER` set to a server that the test harness starts per test, from the binary under test, with `server start --foreground` and that test's environment. Any difference between the two runs is a defect.
- The suite runs once more through the stdio local form, with `GWZ_SERVER=stdio`, on Linux (Phase 7 exit).
- In every mode, a working directory whose name is not valid Unicode is refused before anything is sent: non-UTF-8 bytes on Linux and macOS, an unpaired surrogate on Windows. Relative `--root` values and operands arrive at the server as the same absolute paths an in-process run resolves.
- The lifecycle is tested:
  - `start`, `stop`, `status` and `list`, including a start of a running server and a stop of a stopped one, which succeed, and `status` exiting 1 when nothing answers;
  - `start` with options A, then with options B: refused, and `status` reports A;
  - two servers at two keys: `list` shows both, and each stops by its address;
  - a server double whose `SessionHello` names another core version and build is listed by `server list` and stopped by `server stop --server <address>`, while a command through it is refused with `server_version_mismatch` and no environment byte is sent. A double whose protocol range does not include the client's version is listed as refused, with its range and process ID, and `server status` and `server stop` fail with `server_version_mismatch`;
  - `stop` with an open session: the bound, the message at the bound, and `--force`'s behaviour and warning;
  - a client arriving during a drain gets `server_unavailable`, never `server_session_closed`, and the old process exits within its bound;
  - two auto-starts racing, which leave one server;
  - idle exit, which a `status` loop does not delay;
  - a different `HOME` under `auto`, which starts a second server rather than being refused;
  - an explicit address that refuses a different `umask`;
  - `--max-sessions`, and its message at the cap: the CLI's text ends with the hint, and the library's has none;
  - shutdown with a latched worker;
  - `--foreground --log PATH`, which logs to the file with standard error quiet;
  - a server started with no options, and a client with no options: a native-path operation is not refused;
  - log redaction, against the plan's list.
- **The command surface:**
  - `gwz server` with no subcommand prints the subcommands; `gwz --help` lists the family; each subcommand refuses another's options, and `--dry-run`; `--jsonl` gives `--json`'s records;
  - help snapshots of `gwz --help`, `gwz help server` and each subcommand's help, taken as `gwz server start --help` and so on, since `gwz help COMMAND SUBCOMMAND` does not resolve in released gwz; `--ssh-timeout`'s default of 9 in `gwz server start --help` and in another command's help, and the per-user directory and the grammar in `gwz help server`; a test checks the help's grammar against the parser;
  - the `already running` line names the running server's `--idle-exit`;
  - if OD12 is yes, an error-text row for each command the SSH remote form refuses;
  - `server stdio` on a terminal refuses with §17's message;
  - error text: each class of `server_address_refused`; the four kinds of `server_environment_mismatch` at open; a `server start` failure, with no `--no-server` hint and a pointer to the log; the file-read-once message, naming the address;
  - with `--verbose --server auto --json`, `meta.server` is present; without `--verbose`, it is absent;
  - the `hook` family runs in-process with `GWZ_SERVER` set, and refuses `--server`.
- **The client side,** end to end:
  - the squatted-listener refusal: a listener owned by another user, at the `auto` path and at an explicit address, receives zero bytes (Phase 7 exit);
  - a sandboxed caller with `GWZ_SERVER` set gets a refusal that names the cause and `--no-server`, on all three platforms, at the `auto` path, at an explicit address and with `stdio`, and no server starts (Phase 7 exit);
  - `GWZ_SERVER` holding each refused form is refused before any open, and `--server` with `--no-server` is a usage error.
- **The off switch** (Phase 7 exit): a server started with one `SSH_AUTH_SOCK`, and a client with another and the switch on, run once with the switch as a flag and once as an environment variable. Both runs are refused at `SessionOpen`, and the server's agent records zero signature requests.
- **Reuse through a server** (Phase 7 exit): a second command through a server reuses the first command's connection. Its row has `reused = true` and the first row's `connection_id`, and the fixture's accept count does not change.
- **The stdio rows,** in both CLIs' local forms, on all three platforms (Phase 7 exit):
  - end of input cancels live work, and the process exits with no helper left;
  - a child that writes to its standard output or reads its standard input cannot touch the stream;
  - the terminal's interrupt reaches only the client: a pseudo-terminal test on Linux and macOS, and a console test on Windows, show the first interrupt cancelling by protocol while the child keeps its stream;
  - `server stdio` refuses every option, `--server` included, and a terminal on its standard input or output, with §17's message;
  - with `GWZ_SERVER` set to `stdio`, to an `ssh://` form and to a refused path in the child's environment, the session opens each time.
- A soak runs many client processes against one server. It is the only test that can catch coupling between sessions that the checker cannot see. TR8.3 measures its memory.

**gwz-py.**
- The same two runs of its CLI suite, against `gwz-py server start --foreground`, and its stdio local form's rows above; the lifecycle and command-surface rows above, with gwz-py's strings.
- Help-snapshot and error-text rows assert `gwz-py`, never `gwz`, in the `server` summary, the `server stdio` terminal refusal, the different-options refusal, the stop-bound message and the error prefix.
- Its Client-level tests run through `SocketCoreBridge`, beside the existing `NativeCoreBridge` and `StreamCoreBridge` runs.
- Every bridge refuses a working directory that holds a lone surrogate.
- `SocketCoreBridge` refuses `stdio` and `ssh://` with `server_address_refused`, and never reads `GWZ_SERVER`.
- A copy of itself, started in the background or as a stdio child, never imports a `gwz` package planted in the caller's directory.
- The mask capture never exposes a more permissive mask to another thread.
- Under `auto`, a session that configures 30 seconds gets the library's timeout message on a `git://` remote.
- The closed-session error's code is `server_session_closed` on each of the three bridges, and a channel closed before `SessionOpened` fails the open with `server_unavailable`.
- `StreamCoreBridge` sends `umask` and `openssl`, the test host refuses a `SessionOpen` without them, and contract §15.13 passes with both bridges.
- With the interpreter that `sys.executable` names unable to run, `SocketCoreBridge("auto")` and `gwz-py --server auto` give the documented message.

**Windows.** Every test above also runs on Windows CI in each repository, and every transport row on dabeest. So do:
- the transport build's SSH tests against the Windows OpenSSH agent, and against a Pageant pipe only once a fixture proves it;
- an `SSH_AUTH_SOCK` that names a remote pipe, refused before any open with `permission_denied`; its message, the server's log and the error Python sees hold no substring of the refused name;
- an error-text row for each value of the local-pipe rule's `<rule>`, and of the local-path rule's, in-process and through a server, each message ending with its next step;
- a background start inside a job object.

**The workspace.** A local run in which each CLI is the client of the other's server, with both repositories checked out side by side. Cross-driver integration happens here, not in either repository's CI.

**Release gate.** Neither CLI releases `server start` until gwz-core's checker, in its new mode, reports no `env` or `process` debt, nor gwz-py `gwz-py server` until its own does (§5). That is Phase 7's last exit row.

**Design §11 cells (1.1.0 S5.6 rows)** the server adds. Each is marked per platform with an evidence ID or as unsupported. The plan's §2 puts the server and its stdio mode in this release on all three platforms, so an unsupported mark on any of these cells except Pageant's fails S5.6, unless an amendment first narrows the release (1.1.0 S5.6). Pageant is not in the release's scope until a fixture proves it (plan TR1.3). The SSH remote form's cell exists only if OD12 brings the form in, and is then in scope like the rest.

| Cell | Evidence kind |
| --- | --- |
| Server lifecycle: start, stop, status, list, auto-start, the lock and its record, the file rule, idle exit, shutdown with instance disposal | CI fixture: the lifecycle rows of both CLIs |
| Both CLI suites through a socket server | CI: each suite run twice |
| Peer and sandbox refusal of clients | CI fixture: namespaces and seccomp on Linux, `sandbox-exec` on macOS, AppContainer and integrity on Windows |
| Listener verification: another user's listener, a sandboxed one, and one that never answers | CI fixture, zero bytes received; the handshake's bound |
| The address grammar and the walk | unit, with a connector double; CI fixture for the autofs, direct-map and `/net` rows |
| Must-match and native routes through a server | CI fixture: the `SSH_AUTH_SOCK` rows with the switch on and off, the Linux `SSL_CERT_FILE` row, the OpenSSL configuration rows, Windows' logon-session row; a source-level test for the per-platform tables and the exclusion table |
| One agent source per session; on Windows, the OpenSSH agent pipe | CI fixture on macOS and Linux; dabeest fixture on Windows |
| Pageant, selected by `SSH_AUTH_SOCK` | dabeest fixture; unsupported until one exists (plan TR1.3) |
| Reuse through a server, across commands and clients | CI fixture (Phase 7 exit; reuse design §15 items 1 and 3); measurement (TR8.2) |
| `SocketCoreBridge` | CI: gwz-py's Client-level tests through it |
| The stdio mode and its local client form | CI: gwz-cli's suite through it on Linux; the stdio rows in both CLIs; S7.3's fifth route check |
| Windows primitives: the pipe, its ACL, the per-user directory, job objects | Windows CI and dabeest |
| A long-lived server under a client soak | CI soak; measurement (TR8.3) |
| Log redaction | CI: a scan of the log against the plan's redaction list |
| The SSH remote form, if OD12 brings it in | CI fixture with a disposable `sshd` on Linux and macOS; the Windows OpenSSH client on dabeest; no live account (§16) |

## 13. Relationship to existing documents

- **[Proposals](GwzClientCoreTransportProposals.md) P2.** P2's local form was excluded for gwz-py because it meant running the `gwz` executable (G10). Here gwz-py hosts its own core through its extension, and its copies run its own interpreter. The local form, over a socket or to a stdio child, is therefore available to both CLIs. The SSH remote form is P2's remote adapter, if OD12 brings it in. G1 has OD9's wording (§1, §8).
- **[Session contract](GwzCoreSessionDesign.md).** Unchanged except for the §8 amendments. A serving CLI is a driver. The session, admission, gates, host context, closure and the ratchet all belong to the contract.
- **[Reuse design](GwzConnectionReuseDesign.md).** Its model governs reuse through a server: the host context's endpoint registry, per-operation bindings, and revalidation at each cross-operation lease. This revision applies its §13 server edits (§8), and records its §11 partition and idle-exit notes (§4, §6). Its §11 partition note is corrected on GO, since the `auto` key covers the agent source (§8).
- **Release plan and its amendment.** This revision is TR1.3 as the amendment extends it; §18 maps each clause. TR1.4b places the steps in Phase 7. TR1.5 matches §5's carrier.
- **Client placement** (the transport lane, tags 16–31) stays reserved. A server on another machine that keeps the client's credentials would need it.

## 14. Risks and open points

- **Sandbox detection covers the common mechanisms, not all of them.** The Linux rule's limits are all here.
  - **No server in a sandboxed container.** On Linux the rule is absolute (§4): a seccomp filter or a user namespace refuses. So in this release there is no server inside a container that applies a seccomp profile, Docker's default included, or that runs in a user namespace, such as rootless Podman or an unprivileged LXC container. Its user runs in-process, as by default; with `GWZ_SERVER` set there, each command is refused, naming the cause and `--no-server`. A container with neither can host a server, which then serves only its own mount namespace. The same holds for CI: §12's server rows need a runner outside such a container.
  - **Sandboxes the rule cannot see** are not refused: on Linux, one built on Landlock alone, or on an AppArmor or SELinux profile alone. Such a sandbox must not be given access to the per-user directory. A caller whose sandbox denies the connection itself gets `server_unavailable` (§4).
  - **Kernels.** Kernels before 5.9 are no longer a limit: the rule reads `Seccomp`, which Linux reports from 3.8, not the filter count that 5.9 added. The initial user namespace is recognised by its fixed inode number, which is inferred, and §12 tests it.
  - **Without `/proc`** the Linux check cannot be made, so every caller is refused as one whose check could not be made: it fails closed, and `--no-server` still works.
  - The macOS check relies on an interface Apple does not document. If it disappears, the check fails closed and refuses every peer.
  - Checks by process ID are pinned where the platform allows (§4). `SO_PEERPIDFD` needs Linux 6.5; an earlier kernel compares start times, which a listening socket handed to another process can defeat (inferred).
  - **Windows' listener process.** Microsoft documents `GetNamedPipeServerProcessId` only for a server's handle, so its use on a client's is a dabeest test. If it fails there, listener verification on Windows fails closed, and no Windows client uses a server, until a fallback is designed and tested. The fallback is the pipe object's own security descriptor, read on the client's handle with `GetSecurityInfo`: its owner must be the caller's SID, and its mandatory label must give the caller's integrity level (inferred).
- **A sandboxed process can hold an `auto` address** on Linux and macOS. One that can write the per-user directory can bind first, or after a server's idle exit. Clients refuse it and send nothing (§4). Until it exits, `auto` fails there with `server_listener_refused`; `--no-server` or an explicit address still works. On Windows another user can create a pipe under any name, but `auto` never uses a name its own server did not draw (§4).
- **A hung server** fails each client at the handshake's bound, naming its process ID and its log (§3). `server stop` then fails at its own bound, and the user kills the process the message names.
- **A draining server** holds its address for up to 185 seconds, and commands through it fail meanwhile with `server_unavailable` (§6).
- **A socket is stale only with gwz's record.** macOS refuses a connection to a full accept queue just as it refuses one to a dead socket (§3), so a refused connection alone never shows a socket stale. The host removes a socket only when the lock file beside it also holds a record naming exactly that socket, which a gwz server left (§4). A foreign program's socket, whether its program has gone or its queue is full, is refused and survives, and the message tells the user to remove it if nothing uses it. The record is evidence of provenance only, and any process of the user may write one. A forged record therefore only matters at a path the user typed, since under `auto` the per-user directory holds only gwz's sockets; and a process that can write the record beside a socket can remove the socket itself anyway.
- **The walk and the connect.** On macOS inside `SocketCoreBridge`, a writer on the path can swap a component between the walk and the connect (§3). The snapshot is never sent before listener verification passes, so the exposure is an unwanted mount or network contact, not disclosure.
- **The walk's limits.**
  - Linux: a lookup may block while another process's mount or expiry of an autofs directory is under way (`d_manage`). A stacked file system over a network one, such as eCryptfs, or overlayfs with an NFS lower layer, is not detected by its type. Opening the root of an existing NFS mount may send its server a GETATTR before the type check refuses it; no new mount results (inferred). No Linux experiment was run: the rule rests on the v7.2 sources.
  - macOS: no direct-map trigger could be built without root, so its presentation comes from source. Whether tmpfs sets `MNT_LOCAL`, and the type names of FSKit-backed macFUSE 5, are unverified.
  - Windows: no Windows machine was used. `GetFullPathNameW`'s identity is inferred until dabeest runs it, and whether a user who is not an administrator can redefine `PIPE` in their own session is unclear.
- **Servers multiply.** Each distinct core version, build and must-match environment gets its own auto-started server, until its idle exit. Covering the native row makes that one server per agent socket path, and on Windows per logon session; a changed OpenSSL configuration or, on Linux, CA bundle starts another (§5). `server list` shows them all, with the one `auto` would use marked (§6).
- **Native routes through a server** (amendment §3.12). At an explicit address, a client whose agent, or on Windows whose logon session, differs from the server's is refused for routed operations; one whose OpenSSL values or trust roots differ is refused at open, on Linux and macOS. Under `auto` neither can arise for a CLI client, or for a library session that keeps the default timeout; a library session that sets its own timeout meets the timeout row under `auto` too (§5). The rule is only as complete as §5's per-platform tables, which Phase 7's source-level test pins, and the OpenSSL configuration scan.
- **Dependency state the checker cannot see.** OpenSSL's reads serve every session on Linux and macOS, and on Linux git2's start writes `SSL_CERT_FILE` and `SSL_CERT_DIR` (§5). The must-match set covers them. A new dependency read of this kind is found only by review, and by the source-level test where the source is vendored.
- **OpenSSL is not vendored on Linux and macOS.** Its rows cite OpenSSL 3.6.3. Distributions ship 3.0.x to 3.6.x with their own patches and OPENSSLDIR, and 3.0.x was not verified. The release artifacts' linkage is inferred, not observed: Homebrew's openssl@3 on macOS, the distribution's libssl on Linux. The configuration scan closes `$ENV::`; a patch that adds a read is not seen.
- **Readers whose source is closed.** Whether Security.framework, WinHTTP, SChannel and the glibc resolver read environment variables cannot be established.
- **Pageant's visibility,** on Windows' native path, belongs to a desktop and changes over time. The rule compares the logon session, not the window, so a client on another desktop of the same logon session may meet a different agent through a server. On the same native path, OD11's route may sign with a Pageant key that TR2.8's listing never saw, in-process as through a server.
- **Different OpenSSL builds.** A gwz-py wheel may bundle an OpenSSL other than the CLI's (inferred). Such a client and server never share an `auto` server, and are refused at an explicit address on Linux and macOS.
- **Open point for the operator: vendoring OpenSSL.** git2's `vendored-openssl` feature (git2-rs Cargo.toml:59; openssl-src 300.6.1+3.6.3 is already in the registry) would build OpenSSL 3.6.3 into gwz on Linux and macOS. This design does not decide it.
  - For: the OpenSSL rows close from source; the macOS binary loses its run-time dependency on Homebrew's openssl@3; every gwz and gwz-py build runs one known OpenSSL.
  - Against: OPENSSLDIR becomes the build's default, so the system's `openssl.cnf` and default CA locations stop applying unless `OPENSSL_CONF` or the certificate variables name them; on Linux the default roots then come from openssl-probe's locations alone. gwz ships OpenSSL's security fixes itself, in its own releases, instead of the distribution or Homebrew. Builds take longer.
  - Unchanged either way: `OPENSSL_CONF` can still name a configuration with `$ENV::` references, so the scan stays.
- **Mixed versions at an explicit address.** A command refuses a server whose core version or build differs from its own, before the snapshot crosses (§3), since an older server's must-match list may be shorter than the client's, and a method the older server lacks would be refused besides. `auto` avoids both by keying on the core version. The `server` subcommands check only the protocol range, so `list` and `stop` still reach an older server. The SSH remote form checks only the protocol range, since no must-match value crosses (§16).
- **Group changes** take effect only after a server restart.
- **Name resolution** is the server's (§5). A client's `HOSTALIASES` does not apply through a server. Host keys and TLS verification still decide trust.
- **A detached worker** holds its workspace until it ends. With a server, that blocks every client, not one CLI process.
- **A crash** affects every connected client.
- **Job objects.** Where the caller's job object forbids breakaway, as some CI runners and terminals do, an auto-started server ends with its caller's job. A server started with `--foreground` under a service manager is unaffected.
- **The stdio stream** (amendment §3.12). Anything written to the stdio mode's standard output corrupts its session, and a child that reads its standard input takes frames meant for core. §15's descriptor rule and Phase 7's rows cover both.
- **The SSH remote form** (amendment §3.12), if OD12 brings it in.
  - A remote host the user's SSH trusts can answer every request, and chooses the text the client renders, terminal control sequences included, as a repository's content can in-process.
  - A `--server` value in a script or an alias can point the client at such a host. `GWZ_SERVER` cannot.
  - §16's answers 1, 2, 6 and 7 bound both.
  - Agent forwarding lends the remote host the user's keys (§16, question 4).
- **Python's mask.** Where the platform has no getter, reading the mask sets `0o077` for an instant. A file another thread creates then is more private than intended.

## 15. The stdio mode

`server stdio` serves exactly one session over the process's standard input and output (amendment §3.4). It ships in this release. §16 runs it on another machine.

**Spelling.** `gwz server stdio` and `gwz-py server stdio`. The local client form is `--server stdio`, or `GWZ_SERVER=stdio`. Amendment §5 delegates the mode's spelling to TR1.3: its working name, `server --stdio`, becomes the subcommand `server stdio`, like the family's other verbs (§6). Both ends say `stdio`, one word, distinct from every address, which is `auto` or absolute.

**The stream.**
- The contract's byte-stream adapter and the handshake (§3), binary on every platform, with no text-mode translation on Windows. The mode sends its `SessionHello` at once.
- Nothing but frames is written to standard output.
- Log lines go to standard error. The mode writes only failures there: a refused handshake, a protocol error, a failed write or a panic. The local client form shares the user's terminal, so there are no per-event lines.
- The mode refuses to start when its standard input or output is a terminal, since frames on a terminal would be neither readable nor safe. It refuses as a usage error, exit 2, with a message that says it serves a program, not a person, and names `--server stdio` (§17).

**Lifetime.**
- The process exits when its session ends: after `session.close`, at end of input, or when a write fails. End of input and a failed write are channel closure (contract §8), which cancels live work within `close_wait`.
- After the session ends, it calls the host context's `shutdown`, which returns within one cleanup bound, and exits. It does not wait for detached workers: they end with the process, as they end with an in-process command (contract §16).
- Its exit status is 0 when the session ended with `session.close`, and 1 otherwise.
- It opens no socket and takes no lock. `auto` never selects or starts it. It is started by the local client form, by the SSH remote form, or by a caller that runs the command.

**Options.**
- `server stdio` takes no option of its own. `start`'s options, `stop`'s `--force`, `--server` and `--no-server` are each refused as usage errors: the mode has one session, no listener, no idle period and no log file.
- It neither reads nor parses `GWZ_SERVER`. The local client form's child inherits `GWZ_SERVER=stdio` from its starter, and a remote account's environment may hold any value, so the variable says nothing about this process.
- Of the global options, `--ssh-timeout` sets the process's libgit2 timeout at start, 9 seconds by default, as in every role (§6). The session's `configure_transport_runtime` may still change it before the first backend exists, as in-process.
- The workspace-selection and output options have no effect.

**Trust.** The mode's peer is the process that started it, over a stream only that process holds. The mode runs with that process's user, authority, environment and sandbox, which it inherits, so §4's peer and sandbox checks have nothing to check. It grants nothing its starter lacks:
- it opens no listener, so no third process can reach it;
- everything it does, its starter could do by running core in-process with the same environment;
- a sandboxed starter's child is sandboxed alike. A gwz client inside a detectable sandbox still refuses the local client form, under §4's one rule for sandboxed callers.

In the SSH remote form, the remote process runs as the remote account, which the user's SSH login already reaches (§16).

**Descriptors and signals.** No child process inherits the stream, at either end.
- **The stdio process** moves the channel off the standard descriptors first, before anything else runs:
  - on POSIX, it duplicates standard input and output onto new descriptors with close-on-exec (`F_DUPFD_CLOEXEC`), then points descriptors 0 and 1 at the null device with `dup2`;
  - on Windows, it duplicates the standard input and output handles into non-inheritable handles, points both standard handles at `NUL` (`SetStdHandle`), and clears the inherit flag on the originals before closing them;
  - in gwz-py, Python's standard streams and the C runtime's descriptors 0 and 1 refer to the null device afterwards, so a stray `print` cannot reach the stream.

  A child that core spawns, whatever its standard-stream settings, then can neither read frames nor write into the stream. Standard error stays, for log lines.
- **The client** creates the stream's pipes close-on-exec on POSIX and non-inheritable on Windows, and its launcher passes only the child's ends, with standard error, to the child: no other descriptor or handle crosses (§6). Every other process it starts, such as a pager, `forall`'s commands or a background server, gets standard streams of its own.
- **The terminal's interrupt.** The client starts the stdio process in a new session on POSIX (`setsid`). On Windows it starts it in a new process group (`CREATE_NEW_PROCESS_GROUP`, which disables Ctrl-C for it), where the process also ignores `CTRL_BREAK_EVENT`. The terminal's interrupt therefore reaches only the client, whose protocol governs (§7): the first interrupt cancels the running call, and the second closes the stream, whose end of input cancels live work. The stdio process has no controlling terminal, and needs none (§5).

**Environment.** A local client sends its snapshot in `SessionOpen`, as over a socket, and the must-match check runs against the stdio process's own values, captured at its start. The child inherits its starter's environment, so a mismatch means the starter changed the environment for the child. The child loads OpenSSL and its configuration afresh, as its starter would in-process, so it skips the configuration scan (§5). A relative path among the must-match values is refused, as over a socket, since the child runs in another directory (§5, §6). The local client form never sends the SSH remote form's marker. The off switch travels as an attribute, as over a socket (§5).

**Reuse.** The mode's host context serves one session. That session reuses connections across its own operations (reuse design §11, §12), never across processes. The local client form runs one command per process, so it reuses within the command, as in-process does.

**The local client form.** Both CLIs run one command through `--server stdio`:
- gwz-cli starts its own executable with `server stdio`, and gwz-py starts its own interpreter on `gwz.cli`, never the `gwz` executable (G10), through core's launcher, as §6's "Starting a copy of itself" says;
- the child gets the client's environment and standard error, and pipes for its standard input and output;
- the CLI reads the child's `SessionHello`, opens the session over the pipes within the handshake's bound of 30 seconds (§3), and runs the command. It sends `session.close`, closes the stream, and waits up to 10 seconds for the child to exit, then kills it;
- the CLI's `--ssh-timeout` travels as `configure_transport_runtime`, as in-process.

gwz-py's CLI uses a private client for this: `NativeCoreBridge`'s call table and pump over the extension's byte-stream client channel. It is neither exported nor documented. gwz-py's library gains no stdio client in this release (amendment §3.13).

**The contract's test host.** Contract §12's `StreamCoreBridge` keeps the Cargo example host. This design does not point it at `gwz-py server stdio`:
- the stdio mode stays behind the candidate switch until activation (plan §6(a)), and the wire proof runs before then, in the session plan's Phase 5;
- the release plan's Phase 7 follows that phase, so the proof would wait on the server steps.

Both hosts call core's `serve_session` over the byte-stream adapter with the same handshake: the host's `SessionHello` first, then a `SessionOpen` that carries `umask` and `openssl`, which `StreamCoreBridge` sends through the extension as `SocketCoreBridge` does (§3). So the proof exercises the stdio mode's core path. Pointing the test bridge at the stdio mode after activation is a later simplification.

## 16. The SSH remote form (OD12)

A `--server` form that runs the stdio mode on another machine, through the system `ssh`.
- **Separable.** This section never holds up this design's GO. If a blocking finding is confined to it, the operator may accept the rest without it.
- **OD12.** At this design's GO the operator decides, on this section and its test cost, whether the form ships in this release. Until OD12 is yes, the parser refuses the form in every source (§3).

The section answers amendment §3.4's nine questions, in order.

**1. Address and sources.**
- The form is `ssh://[user@]host[:port]/absolute/remote/path`, parsed by the address parser, with question 2's character allowlist.
  - The remote path is required. It is the remote working directory: the remote session's `InvocationContext.caller_cwd`, against which `--root` and relative operands resolve (question 5). It starts with `/`, and is never relative to the remote home. It is valid UTF-8, and holds no NUL, `?` or `#`. It is taken as written, with no percent-decoding.
  - The scp-like `host:path` is not the form, and is refused.
- Only `--server` carries it. `GWZ_SERVER`, gwz-py's library and every configuration file never do.
  - The parser refuses the form from `GWZ_SERVER` and `SocketCoreBridge` with `server_address_refused`, before `ssh` runs.
  - No gwz configuration file names a server address, and this design adds none.
  - `GWZ_SERVER` is excluded because an environment the user does not read can set it (amendment §2 item 3). A `--server` value in a script or an alias can still name a host; §14 records that risk.

**2. Launch.**
- **The program.** Core resolves `ssh` to an absolute path through `PATH` alone: each absolute element in order, skipping empty and relative elements, taking the first regular executable file named `ssh`.
  - On Windows that is an explicit search of `PATH` for `ssh.exe` alone, with no `PATHEXT`, no current directory and no App Paths.
  - Never the current directory, the workspace or a repository.
  - The CLI runs that program directly, never through a shell.
- **Characters.** gwz itself admits only an allowlist, whatever `ssh` is installed.
  - The host holds ASCII letters, digits, `.`, `-` and `_`, or is a bracketed literal that parses as an IPv6 address, with no zone identifier.
  - The user holds ASCII letters, digits, `.`, `-` and `_`.
  - Neither is empty, and neither starts with `-`. The port is a decimal from 1 to 65535.
  - OpenSSH expands the host and user into `ProxyCommand`, `LocalCommand`, `Match exec` and `KnownHostsCommand`, which run through the user's shell. Clients before 9.6 check neither for shell characters (CVE-2023-51385).
- **The argument vector:**
  1. the resolved `ssh`;
  2. `-T`;
  3. `-o BatchMode=yes`, when the client has no terminal (question 7);
  4. `-p <port>`, when the address gives one;
  5. `--`;
  6. the destination: `user@host` or `host`, an IPv6 literal without its brackets;
  7. the remote command.

  `--` closes the option-injection class Git fixed as CVE-2017-1000117. The address sets no option beyond its port. `-T` keeps a pseudo-terminal off the stream whatever the user's `RequestTTY` says. Otherwise the user's SSH configuration applies.
- **From a sandbox.** The form stays open to a sandboxed caller: its `ssh` runs inside the caller's sandbox, and does nothing the caller could not run itself (§4).
- **The remote command** is fixed text, with nothing from the address in it: `gwz server stdio` from gwz-cli, and `gwz-py server stdio` from gwz-py. The remote host therefore needs gwz, or gwz-py, installed where the remote account's non-interactive `PATH` finds it; when it does not, the remote shell exits with status 127, and the client says so (question 7). The remote directory travels in-band, as `InvocationContext.caller_cwd`.

**3. Environment.**
- The client reads the remote's `SessionHello` first (§3). It checks only the protocol range, not the core version: the session takes the remote process's own environment, so no must-match value crosses, and a remote gwz of another release serves it if it speaks the protocol.
- The client sends no environment. Its `SessionOpen` carries no `environment`, carries `host_environment` set to true, and carries no `umask` or `openssl`. The remote session uses the remote process's own environment, mask and OpenSSL, as they were at its start, as a driver's are.
- Only a stdio host accepts the marker. A socket host refuses a `SessionOpen` that carries it, or that carries no snapshot, with `invalid_request` before `SessionOpened` (§3).
- The frame change is the append-only addition of `SessionOpen.host_environment` (field 6), with `environment` now optional (§9).
- `ssh` itself runs with the client's environment, which it needs for its own authentication and configuration. What it forwards follows the user's `SendEnv`, and the remote `sshd`'s `AcceptEnv`.

**4. Credentials.**
- The remote core uses the remote account's agent, keys and `gh` login, or an agent the user's SSH configuration forwards (`ForwardAgent`). gwz neither turns forwarding on nor turns it off.
- **The risk of forwarding.** While the session runs, the remote core, and any process on the remote host with the remote account's authority or with root, can ask the forwarded agent to sign. It can therefore authenticate as the user to any host that accepts the user's keys.
  - A key added with confirmation (`ssh-add -c`) asks for each use.
  - A variable the user's `SendEnv` forwards, such as `GH_TOKEN`, reaches the remote session the same way.
  - Forward only to hosts trusted with those credentials.
- Keeping credentials on the client while core runs remotely is client placement, which stays out (plan §9).

**5. Paths and inputs.**
- `--root` and relative operands resolve against the remote directory.
  - The CLI makes no local file-system call on a path it sends. `diff` and `log` compute their workspace-relative directory, which in-process canonicalizes locally, from the remote directory and `--root`, as text.
  - Paths core reads name remote files: `--identity`, `--remote-identity`, `auth identity --set`, the destinations of `clone` and `materialize`, and `local clone --from`.
- Every input the CLI itself reads from a path or a stream, and what the form does with it:

  | Input | CLI | In the SSH remote form |
  | --- | --- | --- |
  | The hook payload on standard input, `CLAUDE_PROJECT_DIR`, the hook's log, ignore and lock files, and its worktrees (`hook claude-code worktree-create` and `worktree-remove`) | gwz-cli | refused: the hook's file-system work is local (session plan CS6.2) |
  | Claude Code's settings files (`hook claude-code setup`) | gwz-cli | refused: it acts only on local files |
  | The bootstrap files of `init --update`, while it has no protocol method (session plan D3) | gwz-cli | refused. With a protocol method, it runs remotely |
  | `forall`'s commands, run in member directories | both | refused |
  | The pager: `GIT_PAGER`, `PAGER` and `LESS` (`diff`, `log`) | gwz-cli | the client's, as local rendering |
  | `GWZ_URL_SCHEME` | both | the client's, sent as the request field it fills |
  | The off switch's flag, variable and user configuration | both | the client's (question 9) |

- A refused command is refused before `ssh` runs, with a usage error that names the command and the form.

**6. What the remote side can do to the client.**
- The remote host is trusted as far as the user's SSH trust goes.
- The client's local effects in this form are rendering and the exit code: text and JSON on its standard output and error, the progress line, and the pager's input.
- No reply makes the client read or write a local file, or run a command:
  - the CLI builds its request before any reply arrives;
  - it decodes each reply into a typed message and renders it;
  - the pager is chosen before the session, from the client's environment and terminal;
  - the commands that run local programs or touch local files are refused (question 5).
- Every reply is bounded as every frame is: 64 MiB, one reply per call, and a protocol error closes the session.
- The remote side does choose the text the client renders, as a repository's content does in-process (§14).

**7. Prompts, termination and errors.**
- **Prompts.** `ssh` keeps the client's controlling terminal and standard error, while its standard input and output carry the stream. OpenSSH reads its prompts, such as an unknown host key, a password or a passphrase, from the terminal, not from standard input, so they reach the user. On Windows it reads them from the console.
- **No terminal.** A run with no controlling terminal, such as CI, gwz-py's CLI driven from a script, or an agent, is one where `/dev/tty` cannot be opened on POSIX, or no console is attached on Windows.
  - `ssh` gets no terminal either, and the client sets `BatchMode=yes`, so `ssh` never prompts.
  - The client waits at most 30 seconds, the transport's setup budget, from the spawn to `SessionOpened`, the remote's `SessionHello` included. At the bound it kills `ssh`, waits for it, and fails with `server_unavailable`, naming the bound and the missing terminal.
- **With a terminal** the client sets no bound before `SessionOpened`, since a prompt waits for the user; the first interrupt ends the wait (below).
- **The first frame.** A first frame that is not a well-formed `SessionHello` (§3) fails with `server_unavailable`. A remote account's shell start-up files, such as a `~/.bashrc` that prints text for a non-interactive command, are the likely cause: their text reaches the client before the stdio host's frame. The message says so, and shows the first bytes with control characters escaped. The client kills `ssh` and waits for it, as at the bound.
- **The interrupt.** The client starts `ssh` with `SIGINT` and `SIGQUIT` ignored on POSIX, and in a new process group on Windows. OpenSSH's client loop keeps an inherited ignore. The terminal's interrupt then reaches only the client:
  - before `SessionOpened` there is no call to cancel, so the first interrupt kills `ssh` and the command ends as interrupted. An interrupt during an `ssh` prompt also ends that prompt;
  - after `SessionOpened`, the client's protocol governs (§7).
- **Termination.** Closing the client ends the remote session through channel closure.
  - After `session.close`, or on a second interrupt, the client closes `ssh`'s standard input. The remote stdio process reads end of input, which cancels its live work (contract §8), and exits. `ssh` then exits.
  - The client waits for `ssh` up to 10 seconds after `session.close`, then kills it.
  - If the client dies, the kernel closes the pipe, with the same result.
- **Errors:**

  | `ssh` outcome | Code |
  | --- | --- |
  | `ssh` not found on `PATH` | `external_tool_missing`, naming `ssh` and saying that only `PATH` is searched |
  | `ssh` exits, or the stream ends, before `SessionOpened`: a host-key or authentication failure, a refused connection, a remote command that is too old | `server_unavailable`, with `ssh`'s exit status. `ssh`'s own messages reach the client's standard error unchanged |
  | `ssh` exits with status 127 before `SessionOpened`: the remote shell found no `gwz server stdio` | `server_unavailable`, saying that the remote host has no gwz (or gwz-py) on its non-interactive `PATH` |
  | The first frame is not a well-formed `SessionHello` | `server_unavailable`, naming the remote shell's start-up output as the likely cause, with the first bytes escaped |
  | No `SessionOpened` within 30 seconds, with no terminal | `server_unavailable` |
  | The remote stdio host refuses the handshake | the refusal's own code, such as `server_version_mismatch` |
  | The stream ends after `SessionOpened`, before a reply | `server_session_closed` |
  | An operation fails remotely | its own code, rendered as in-process |

**8. Tests and cells.** These run with no live account: against a disposable `sshd` on Linux and macOS, on loopback, with the binary under test first on that `sshd`'s `PATH`; and with the Windows OpenSSH client on dabeest, against a disposable `sshd`. If OD12 is yes, Phase 7's exit gains these rows (amendment §3.11), in gwz-cli's and gwz-py's suites:
- one network operation that uses the remote account's credentials;
- no environment entry leaves the client, and its `SessionOpen` carries the marker;
- a socket host refuses the marker;
- `forall`, and each input question 5 refuses, are refused;
- closing the client ends the remote session and leaves no helper;
- a user or host that starts with `-`, or holds a character outside question 2's allowlist, including a backtick and `$(`, is refused before `ssh` runs, and a spawn double records zero spawns;
- with a planted `ssh.exe` in the caller's directory on dabeest, and a planted `ssh` with `.` absent from `PATH` on Linux and macOS, the form runs the recording `PATH` program, never the planted one;
- `GWZ_SERVER` holding the form is refused before `ssh` runs;
- a run with no terminal against an unknown host key fails within the bound and leaves no `ssh` process;
- beyond the amendment's list: after `SessionOpened`, the first interrupt cancels by protocol, and `ssh` survives it;
- a login shell that prints a banner: the command fails within the bound with the start-up output message, and no `ssh` process is left;
- a remote host without gwz on its non-interactive `PATH`: the status-127 message;
- help and `--verbose` line rows for the form (§17).

The form's cell is in §12. S7.3 runs one CLI network operation through the form against a disposable loopback `sshd` on Linux, and Phase 10's post-release check repeats it on one host with a disposable `sshd` (amendment §3.11).

**9. The off switch.** The form sends no snapshot, so TR1.5's snapshot carrier is unavailable. The client's resolved value governs. It travels as `ProcessAttributes.transport_off`, as in every form (§5). The remote's environment and user configuration are not consulted for it. `--verbose` names the value that applied, and says it is the client's (§17).

**If OD12 is yes,** this design carries the contract §1 edit of §8 and the Phase 7 rows of question 8 (amendment §3.11). The amendment's other "If yes" edits are the plan's, recorded in its changelog. **If no,** the parser keeps refusing the form, the contract's §1 stands, and this section waits for a later release.

## 17. User-facing surface

This section is what Surface review freezes (plan TR1.3, as amended; amendment §5). Every name, default, help text and message here is proposed, and the section is meant to be read on its own. "The CLI" means both: gwz-py's forms are the same with `gwz-py` in place of `gwz`.

**Commands.**

```text
gwz [--server auto|stdio|ADDRESS | --no-server] <command> …
gwz server start [--foreground] [--max-sessions N] [--idle-exit DURATION] [--log PATH] [--server auto|ADDRESS]
gwz server stop [--force] [--server auto|ADDRESS]
gwz server status [--server auto|ADDRESS]
gwz server list
gwz server stdio
```

- Bare `gwz server` prints the subcommands, as `gwz local` does. Each subcommand takes only its own options, and refuses another's, and `--dry-run`, as a usage error (exit 2).
- The global options, `--server` among them, may come before the command or after it, in both CLIs, as for every other command: `gwz --server X server status` and `gwz server status --server X` are the same.
- If OD12 is yes, `--server` also takes the SSH remote form for every command except `server` and `hook` (below).

**Help.** These strings are frozen, and gwz-py uses them in argparse's layout. In every frozen string of this section, whether help, output, a message or an instruction, and in the prefixes `gwz:` and `gwz server:`, `gwz` is the invoking CLI's program name: `gwz-py` for gwz-py. Nothing else differs.
- `gwz --help` gains one group line, after `Lanes:`:

  ```text
  Server:     server start|stop|status|list|stdio
  ```

  gwz-py's command list gains `server`, summarised `Run gwz-py commands in a long-lived server of your own`.
- `gwz help server` opens:

  ```text
  Run gwz commands in a long-lived gwz process of your own, or serve one
  command over standard streams.

  A server is off unless you ask for it. Run a command with --server auto, or
  set GWZ_SERVER=auto, and gwz uses this user's server for your environment,
  starting one if none answers. A server serves only your user, on this
  machine, from outside any sandbox. --no-server runs one command in-process.

  Servers live in a per-user directory: $XDG_RUNTIME_DIR/gwz/, else
  $TMPDIR/gwz-<uid>/, else /tmp/gwz-<uid>/ on Linux and macOS, and gwz\ in the
  local application data folder on Windows. An address is auto, an absolute
  socket path (Linux, macOS) or \\.\pipe\<name> (Windows).
  ```

- One line per subcommand:

  ```text
  start    Start a server at an address and return once it answers
  stop     Stop the server at an address, waiting for its work to end
  status   Report the server at an address; exit 1 if none answers
  list     List this user's servers in the per-user directory
  stdio    Serve one session over standard input and output, for a program
  ```

- `start`'s and `stop`'s options:

  ```text
  --foreground          Serve in this process until stopped; log to standard error unless --log is given
  --max-sessions N      Open sessions to allow before refusing the next (default 64, at most 1024)
  --idle-exit DURATION  Exit after this long with no open session: 30s, 10m, 2h or off (default off)
  --log PATH            Log file (default: standard error with --foreground; otherwise server-<key>.log
                        in the per-user directory, pipe-<digest>.log for a Windows pipe, or
                        <socket path>.log beside an explicit socket)
  --force               (stop) Stop waiting for the server's detached workers
  ```

- `server stdio` with a terminal on its standard input or output refuses, as a usage error:

  ```text
  gwz server stdio: standard input is a terminal; this mode serves a program over its standard streams, not a person. To run a command through it, use gwz --server stdio <command>
  ```

  It names standard output instead when that is the terminal.

**Options.**

| Option | Applies to | Default | Values and meaning |
| --- | --- | --- | --- |
| `--server` | every command except `hook`, `server list` and `server stdio` | `GWZ_SERVER`; unset means in-process. For `server start`, `stop` and `status`, unset means `auto` | an address (below). For `server start`, `stop` and `status`: `auto`, a socket path or a pipe name |
| `--no-server` | every command except `server` | off | run in-process; `GWZ_SERVER` is not read. Refused with `--server` |
| `--foreground` | `server start` | off | serve in this process until stopped; log to standard error, unless `--log` is given |
| `--max-sessions N` | `server start` | 64 | 1 to 1024 open sessions; the next is refused with `transport_session_full` |
| `--idle-exit DURATION` | `server start` | `off`; `10m` for an auto-started server | a whole number followed by `s`, `m` or `h`, at least `1s`; or `off`. Counts only time with no open session |
| `--log PATH` | `server start` | with `--foreground`, standard error. Otherwise, and for an auto-started server, in the per-user directory: `server-<key>.log` for `auto`, and `pipe-<digest>.log` for an explicit Windows pipe; beside an explicit socket, `<socket path>.log` | a file, under the log path rules below. With `--foreground`, the log goes to the file, and standard error stays quiet |
| `--force`, global | `server stop` | off | stop waiting for the server's detached workers: it exits after closing its sessions (below) |
| `--ssh-timeout SECS`, global | every command, `server start` and `server stdio` included | 9, in every role and in both CLIs | seconds; 0 means no timeout. A server's value is its libgit2 network timeout, which a client's native-path operations must match; with both at the default, they do. This release changes the default from 1.0.17's 3, as the [retry plan](../gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md) sets out in its §1 default table and its §8 help text; 1.1.0 S7.2's migration notes record it |
| `--json`, `--jsonl`, global | `server start`, `stop`, `status` and `list` | off | one JSON record per outcome (below); `--jsonl` gives the same records |
| `--verbose`, global | every command | off | name the server that ran the command (below) |

**The environment.** `GWZ_SERVER` holds `auto`, a socket path, a pipe name or `stdio`. Empty or unset means in-process. The SSH remote form is refused there. `SocketCoreBridge`, the `hook` family and `server stdio` never read it. Explicit selection of an agent is `SSH_AUTH_SOCK` (§5): unset means the OpenSSH agent's pipe on Windows and no agent on Linux and macOS, and Pageant is used only when `SSH_AUTH_SOCK` names its pipe.

**Python.** `SocketCoreBridge(address)`: `address` is required, and is `auto`, a socket path or a pipe name, as `str` or `os.PathLike[str]`. It is passed as `Client(bridge=…)`. `"auto"` may start a server. `stdio` and `ssh://` are refused with `server_address_refused`. It takes no timeout: a session that configures its own timeout after it opens has its native-path operations refused, under `auto` as at an explicit address, with the library's timeout message below. A session's limits may not exceed a socket host's maxima, which are the contract's defaults: 8 running operations, 64 queued, an operation table of 128 entries, 8 direct-method workers, 4096 events per operation, 64 open logs, 1024 outstanding calls, a control reserve of 64 frames, 1 MiB returned by one read, and a `close_wait` of 60 seconds. A larger request is refused with `invalid_request`.

**Addresses.** `--server`, `GWZ_SERVER` and `SocketCoreBridge` take exactly these forms, and refuse everything else before anything is opened:
- `auto`: this user's server for this environment. On Linux and macOS it is the socket `server-<key>` in the per-user directory. On Windows it is the pipe that the lock file `server-<key>.lock` records: `\\.\pipe\gwz-` and 32 random hexadecimal digits, drawn at each start. `<key>` is 16 hexadecimal digits of a digest of the core version and build and of every value a server reads process-wide (§5), so each gwz version and environment gets a server of its own.
- A socket path, on Linux and macOS only. It is absolute, starting with `/`; it has no empty, `.` or `..` component, and so no trailing `/`; it holds no NUL; it is at most 107 bytes on Linux and 103 on macOS; and on macOS it is not under `/.vol/`, `/.nofollow/` or `/.resolve/`. Its directory must exist, be owned by the user, have no group or other permission bits, and not be a symbolic link, so `/tmp` itself does not qualify. No component may be an automount point, or on a network or FUSE file system.
- A pipe name, on Windows only: exactly `\\.\pipe\`, then one name of ASCII letters, digits, `.`, `-` and `_`, not ending in `.`, at most 256 characters in all, such as `\\.\pipe\my-gwz`.
- `stdio`, from `--server` and `GWZ_SERVER`, for commands other than `server`: run the command in a child process (§15).
- The SSH remote form, from `--server` only, if OD12 is yes (below).

A relative path, any other word, a socket path on Windows or a pipe name elsewhere, another spelling of a pipe name, and any name that reaches another machine are refused.

**The per-user directory** is `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/` on Linux and macOS, with mode 0700. On Windows it is `gwz\` in the local application data folder that the system reports, whatever `LOCALAPPDATA` says, with an ACL for the user alone, SYSTEM and Administrators. A variable that is set, but that the address rules refuse, is refused, not skipped.

**Files.**
- In the per-user directory: `server-<key>`, the socket, on Linux and macOS; `server-<key>.lock`, which stays after exit and records the process ID, address and log path of the server that holds it. A clean exit clears the record; after a crash it stays, and is how gwz knows a socket left behind as its own; `server-<key>.log`, and `server-<key>.log.1`, the previous log, once the log passes 10 MiB. On Windows also `pipe-<digest>.lock` and `pipe-<digest>.log` for an explicit pipe.
- Beside an explicit socket: `<socket path>.lock` and `<socket path>.log`.
- gwz removes a socket it finds at an address only when nothing answers there and the lock file beside it records that very socket. Any other socket stays, and the start refuses, telling the user to remove it if nothing uses it.
- gwz never follows, truncates or writes a symbolic link, a FIFO, another user's file, or a file with group or other permission bits, at any of these names. It refuses, and names the file.

**Log path rules.** `--log PATH` is absolute. On Linux and macOS it follows a socket path's rules without the length limit: no empty, `.` or `..` component, and no component that is an automount point or on a network or FUSE file system. On Windows it is on a local fixed drive, and is neither a UNC nor a device path. The file itself follows the rule above, and a new one is created for the user alone.

**What each form starts.** `server start`, an auto-start and `--server stdio` start a copy of the CLI, never another program:
- gwz runs its own executable;
- gwz-py runs its own Python interpreter, `sys.executable`, on `gwz.cli`, and never the `gwz` executable, so a machine with gwz-py alone needs nothing else;
- `SocketCoreBridge("auto")` starts what gwz-py's CLI starts.

When the program cannot be started, the start fails with `server_unavailable`, naming the program and the cause (below).

**The `hook` family** always runs in-process: it never reads `GWZ_SERVER`, and refuses `--server`, because tools run it inside their sandboxes, which a server refuses. The refusal is a usage error (exit 2): `gwz hook always runs in this process, since tools run it inside sandboxes that a server refuses; run it without --server`.

**The SSH remote form**, if OD12 is yes: `gwz --server ssh://[user@]host[:port]/path <command>` runs the command on the remote host, through the system `ssh`.
- `/path` is the remote working directory: an absolute path on the remote host, never relative to the remote home. `--root` then names a directory on the remote host, relative to `/path` unless it is absolute, and relative operands resolve against `/path` too.
- The remote host needs gwz, or gwz-py for gwz-py, on the remote account's non-interactive `PATH`: the client runs `gwz server stdio` there. Without it, `ssh` exits with status 127, and the message says so (below).
- The command uses the remote account's own credentials and environment, never this machine's (§16).
- Three commands act on this machine, and are refused under the form before `ssh` runs, as a usage error (exit 2), as §16 says: `forall`, `init --update` while it has no protocol method, and the `hook` family. The message for the first two is `gwz <command> is not available through the SSH remote form: <reason>; run it on the remote host`, where `<reason>` is `it runs its commands on this machine` for `forall`, and `it reads bootstrap files on this machine` for `init --update`. The `hook` family gives its own refusal of `--server` (above).
- Every other command runs on the remote host, and a path it names, such as `--identity`, the key file `auth identity --set` names, or a `clone` or `materialize` destination, is a path on the remote host.

**Output of the `server` command.** `<address>` is the resolved socket path or pipe name, for `auto` too. `<form>` is `auto` when the address came from `auto`, and the address otherwise.

| Subcommand and outcome | Output | Exit |
| --- | --- | --- |
| `start`, started | standard output: `gwz server: running at <address> (pid <pid>); use it with --server <form> or GWZ_SERVER=<form>` | 0 |
| `start`, already running, with the same options or none given | standard output: `gwz server: already running at <address> (pid <pid>, --idle-exit <duration or off>)` | 0 |
| `start`, already running with different options | standard error: `gwz server: already running at <address> (pid <pid>) with different options (<option>, …); run gwz server stop first` | 1 |
| `start --foreground` | nothing; its log begins `gwz server: listening at <address> (pid <pid>, <product> <version>, core <version> (<build>))`, on standard error or in the `--log` file | 0 after a clean stop |
| `start --foreground`, a gwz server answers there | standard error: `gwz server: already running at <address> (pid <pid>); its log is <log path>`, since a foreground server that cannot serve must fail its service manager. The process ID is the kernel's and the log path the server's own | 1 |
| `start --foreground`, the lock held and no gwz server answering | standard error: `gwz server: <lock path> is held, but no gwz server answers at <address> (pid <pid> and log <log path>, from its lock file); end that process if it is not a gwz server, or wait for it to exit` | 1 |
| `stop`, stopped | standard output: `gwz server: stopped at <address> (pid <pid>)` | 0 |
| `stop --force`, stopped | as `stop`, after a warning on standard error: `gwz server: warning: --force stops waiting for detached workers; a write one of them had in progress may be left as a killed process leaves it` | 0 |
| `stop`, nothing answers | standard output: `gwz server: not running at <address>`, followed, when the address's lock file holds a record, by ` (pid <pid> and log <log path>, from its lock file)` | 0 |
| `stop`, still stopping after 185 seconds | standard error: `gwz server: still stopping at <address> (pid <pid>) after 185 seconds; run gwz server stop again to keep waiting, or gwz server stop --force to stop waiting for its detached workers` | 1 |
| `status`, running | standard output: `gwz server: running at <address> (pid <pid>, <product> <version>, core <version> (<build>), protocol <n>, <k> open sessions)`, then `  --max-sessions <m>, --idle-exit <duration or off>, log <path or standard error>` | 0 |
| `status`, stopping | standard output: `gwz server: stopping at <address> (pid <pid>)` | 0 |
| `status`, nothing answers | standard output: `gwz server: not running at <address>`, followed, when the address's lock file holds a record, by ` (pid <pid> and log <log path>, from its lock file)` | 1 |
| `list` | standard output, a line per server: `gwz server: <address>: pid <pid>, <product> <version>, core <version> (<build>), key <key>, idle exit <duration or off>`, ending `, auto` for the one `--server auto` uses from this environment; for a server that did not answer within 5 seconds, `gwz server: <address>: not answering (pid <pid>, from its lock file)`; for one whose protocol range this client does not speak, `gwz server: <address>: refused: it serves protocol <min>–<max>, which this gwz does not speak (pid <pid>)`; for a record whose address the rules refuse, `gwz server: <lock path>: refused: its recorded address fails the address rules: <rule>`. When an unmarked server is listed, a last line: `gwz server: --server auto from here uses only the server marked auto; each other one ends at its idle exit, or with gwz server stop --server <address>`. With none: `gwz server: no servers in <per-user directory>` | 0 |
| `stdio` | frames only (§15) | 0 after `session.close`; 1 otherwise |

With `--json` or `--jsonl`, each outcome is one record: `address`, and `state` (`running`, `stopped`, `not_running` or `stopping`); `started`, for `start` only; and, when a server answered, `pid`, `server`, `core`, `protocol`, `open_sessions`, `max_sessions`, `idle_exit` (seconds, or null for off) and `log` (a path, or null for standard error). `list` gives one record, `servers`: a list of those objects, each with `key`, `auto` and `answering` as well. A refusal prints the CLI's usual error record and exits 1.

A stopping server refuses new sessions, naming the stop (`server_unavailable`, below), and still answers `server status` and `server stop`, so running `server stop` again keeps waiting, and `server stop --force` ends its wait for detached workers.

**Errors.** In human mode each message follows `gwz: ` on standard error, and each exits 1. `<address>` is the resolved address or form, and every shown value has its control characters escaped. No message ever shows an environment value. The hint depends on the invocation:
- a command run through a server gets "; run with --no-server to run in-process" where a template says "+ hint";
- a `server` subcommand gets "; see its log at <log path>" there instead, when the server concerned has a log, and nothing otherwise, since `--no-server` does not apply to it;
- the library's messages carry no hint.

- **`server_unavailable` (77):** the server could not be reached, or did not act as a gwz server, before any request reached core. By cause:
  - nothing answers: `no gwz server answers at <address>` + hint. When the address's lock file holds a record, the message adds it, as `no gwz server answers at <address> (pid <pid> and log <log path>, from its lock file)` + hint, which is also how macOS reports a full accept queue (§3);
  - the handshake's bound passed, 30 seconds for a command and 5 for a `server` subcommand, before `SessionOpened` or `ServerState`, whether or not the connection was accepted: `the listener at <address> (pid <pid>) did not answer as a gwz server within <n> seconds; its log is <log path>, from its lock file <lock path>` + hint. The process ID is the kernel's; when the connection was never accepted, it is the lock file's, and reads `pid <pid>, from its lock file`. Without a record, the part after the semicolon is left out, and so is a process ID the kernel does not give. A stdio child's message ends after `<n> seconds`;
  - a first frame that is not a `SessionHello`: `<address> did not answer as a gwz server; its first bytes were <bytes>` + hint;
  - a stopping server: `the server at <address> (pid <pid>) is stopping; try again when it has stopped` + hint;
  - the connection closed before `SessionOpened`: `the server at <address> closed the connection before the session opened; no request was sent` + hint;
  - a start that failed: `could not start a gwz server at <address>: <cause>` + hint, where `<cause>` is `could not run <program>: <operating system error>`, `Python cannot name its interpreter: sys.executable is empty`, `it did not answer within 5 seconds`, `<lock path> is held, but no gwz server answers there (pid <pid> and log <log path>, from its lock file)`, `a socket file is there, but no gwz server's lock file names it; remove it if nothing uses it`, `no unused pipe name after 3 draws`, or `OpenSSL's configuration could not be read: <file>: <reason>`;
  - a denied connection: `connecting to <address> was denied: <operating system error>` + hint;
  - under `auto`, this process's OpenSSL scan could not finish: `--server auto needs this process's OpenSSL configuration, which could not be read: <file>: <reason>` + hint;
  - `server start` at a socket that something else answers on: `something already answers at <address> (pid <pid>), and gwz server start never removes a socket that answers`, where `(pid <pid>)` is left out when the connection was not accepted within the 5 seconds;
  - `server start` at a socket that refuses connections, but that no gwz server's lock file names: `a socket file is at <address>, but no gwz server's lock file names it, so gwz server start leaves it; remove it if nothing uses it`;
  - the SSH remote form: `ssh exited with status <n> before the session opened` + hint; for status 127, `ssh exited with status 127 before the session opened: the remote host has no gwz on its non-interactive PATH` + hint; `ssh gave no handshake within 30 seconds, and there is no terminal for its prompts` + hint; `the remote command's first output was not a gwz handshake, but <bytes>; the remote account's shell start-up files may print text for non-interactive commands` + hint.
- **`server_version_mismatch` (78),** always before the client sends a frame, so before the snapshot crosses:
  - a command, when the protocol range excludes its version: `the server at <address> serves protocol <min>–<max>; this client speaks <n>; --server auto finds a matching server` + hint;
  - a command, when the core differs: `the server at <address> runs core <version> (<build>), but this client has core <version> (<build>); --server auto finds a matching server` + hint;
  - a `server` subcommand, which checks only the protocol range: `the server at <address> (pid <pid>) serves protocol <min>–<max>, which this gwz does not speak; stop it with the gwz that started it, or end process <pid>` + hint.
- **`server_environment_mismatch` (79):** a must-match value differs, or cannot be used, at open or when an operation takes the native path (§5).
  - At open, composed per kind: `the server at <address> runs with a different value of <VAR>`, `the server at <address> runs with a different file-creation mask`, `the server at <address> was started against a different OpenSSL library`, or `the server at <address> runs in a different Windows logon session`; each followed by `, which it reads process-wide; --server auto finds a matching server` + hint.
  - A relative path: `<VAR> holds a relative path, which a server would resolve against its own directory, not yours; set it to an absolute path` + hint. A set but empty value, on Linux and macOS: `<VAR> is set but empty, which is read as a relative path; unset it, or set it to an absolute path` + hint, and for `XDG_CONFIG_HOME`, `XDG_CONFIG_HOME is set but empty, which libgit2 reads as the relative path git; unset it, or set it to an absolute path` + hint. An empty `SSH_AUTH_SOCK` or `SSLKEYLOGFILE` is never refused: the first selects no agent, and OpenSSL ignores the second (§5).
  - At routing: `<cause>, so this operation takes libgit2's native route, which uses the server's <kind>, and it differs from yours` + hint, where `<kind>` is `value of <VAR>` or `Windows logon session`, and `<cause>` is `this remote's agent holds a key the transport cannot sign with`, `this remote's credential helper is not gh`, or `the transport off switch is on`.
  - The timeout, from a CLI: `this operation takes libgit2's native path, which uses the server's --ssh-timeout of <n> seconds, not yours` + hint. From the library: `this operation takes libgit2's native path, which uses the server's libgit2 timeout of <n> seconds, not this session's`.
  - A file read once: `<file> changed after the server at <address> started; run gwz server stop --server <address>` + hint.
  - Before connecting: `this process's real and effective user IDs differ, so libgit2 would take its home from the passwd entry, which a server does not` + hint.
- **`server_peer_refused` (80):**
  - `the server at <address> refused this process: <cause>` + hint, where `<cause>` is `another user`, `a sandbox (<mechanism>)`, `a lower integrity level than the server's` or `a check that could not be made`;
  - `this process runs inside a sandbox (<mechanism>), and a gwz server refuses sandboxed callers` + hint, where `<mechanism>` is `a seccomp filter`, `a user namespace`, `the macOS sandbox`, `an AppContainer` or `low integrity`;
  - `gwz server start` inside a sandbox: `gwz server start refuses inside a sandbox (<mechanism>), since a server started there would refuse every client`.
- **`server_address_refused` (81),** before anything is opened. One template per class, each with its next step; `<source>` is `--server`, `GWZ_SERVER` or `SocketCoreBridge`:
  - the grammar: `server address from <source> refused: <rule>: <address>; use auto, an absolute socket path (Linux, macOS) or \\.\pipe\<name> (Windows)`;
  - a walk component: `server address from <source> refused: <component> is <what>: <address>; use a path on a local file system`, where `<what>` is `an automount point`, `an automounter's mount`, `on a network file system (<type>)`, `on a FUSE file system`, `a procfs link`, or `one link too many (more than 8)`;
  - a pipe name that Windows would change: `server address from <source> refused: Windows would change this pipe name: <address>; use \\.\pipe\ and one name of letters, digits, ., - and _`;
  - a redefined `PIPE` device: `server address from <source> refused: the PIPE device is redefined in this logon session, so \\.\pipe\ does not reach the pipe file system; run with --no-server`;
  - `stdio` given to `server start`, `stop` or `status`: `<source> is stdio, which has no listener for gwz server <subcommand>; pass --server auto or an address`;
  - the SSH remote form from `GWZ_SERVER`: `the SSH remote form is accepted only from --server, never from GWZ_SERVER`; given to a `server` subcommand: `gwz server <subcommand> takes no SSH remote form; run it on the remote host`; while OD12 is no: `the SSH remote form is not in this release`;
  - `stdio` or the SSH remote form given to `SocketCoreBridge`: `SocketCoreBridge takes auto, a socket path or a pipe name; stdio and the SSH remote form have no library client`;
  - a directory: `server directory refused: <directory> <rule>; fix its owner or permissions, or choose another address, or set XDG_RUNTIME_DIR or TMPDIR`, where `<rule>` is `is owned by another user`, `has group or other permission bits`, `is a symbolic link` or `is not on a local fixed drive`, and the last clause appears on Linux and macOS only;
  - a file at a server's name: `server file refused: <path> is <what>; gwz never follows, truncates or writes it, so remove it if you did not create it`, where `<what>` is `a symbolic link`, `a reparse point`, `not a regular file`, `owned by another user` or `open to group or others`;
  - a `--log` path: `log path refused: <rule>: <path>`, with the rules above, saying "log path" and never "address".
- **`server_listener_refused` (82):** `the listener at <address> is not this user's unsandboxed gwz server: <cause>; nothing was sent` + hint, where `<cause>` is `another user`, `a sandbox (<mechanism>)`, `a different integrity level`, `its directory or socket file <rule>`, or `a check that could not be made`.
- **`server_session_closed` (83):** `the server at <address> closed the session before replying; the command's effect is unknown, and it was not retried`. Through an in-process bridge, `the session closed before replying; …`.

Existing codes on these paths:
- `invalid_request`: a malformed or disallowed handshake, a requested limit above a socket host's maximum, and `ServerControl` sent to a stdio host (§3).
- `transport_session_full`, at `--max-sessions`: `the server at <address> has <N> open sessions, its --max-sessions; wait` + hint.
- `external_tool_missing`, when `ssh` is not on `PATH` (§16): `ssh was not found on PATH; only PATH is searched`.
- `permission_denied`, on Windows, when `SSH_AUTH_SOCK` fails the local-pipe rule, or, for a form other than a pipe, the local-path rule (§5). The rule is the transport's own, so it applies in-process and through a server alike. The message never shows the value, and ends with the next step:
  - `SSH_AUTH_SOCK does not name a local pipe (<rule>), so the agent was not opened; point it at your agent's local pipe, unset it to use \\.\pipe\openssh-ssh-agent, or name a key file with --identity`, where `<rule>` is `it does not start with \\.\pipe\`, `its name is empty, . or ..`, `it names more than one component`, `it holds a NUL or control character`, `it ends in a dot or a space`, `it is longer than 256 characters`, `Windows would change it`, or `the PIPE device is redefined in this logon session`;
  - for a form other than a pipe: `SSH_AUTH_SOCK does not name a local agent path (<rule>), so the agent was not opened; …`, with the same next step, where `<rule>` is `it is not an absolute path`, `it is a UNC or device path`, or `it is not on a local fixed drive`.

**`--verbose`** names the server that ran the command. Without `--verbose`, all output is identical to the in-process run.
- In human mode, one line on standard error, before the command's output:
  - a socket server: `gwz: server <address>: pid <pid>, <product> <version>, core <version> (<build>), protocol <n>`. When `auto` chose the address, it begins `gwz: server auto (<address>):`;
  - the stdio local form: `gwz: server stdio: pid <child pid>, <product> <version>, core <version> (<build>), protocol <n>`;
  - the SSH remote form: `gwz: server ssh://<destination><path>: remote pid <pid>, <product> <version>, core <version> (<build>), protocol <n>, environment: remote, transport off switch: <on|off> (from this client)`.
- With `--json` or `--jsonl`, the `meta` object of each record gains `server`, as `meta.transport` carries today's `--verbose` diagnostics: `{"address": …, "pid": …, "protocol": …, "mode": "socket" | "stdio" | "ssh"}`. No line is printed.

**Lifecycle pairs.**

| Begins | Ends |
| --- | --- |
| `server start`, idempotent | `server stop`, idempotent; `server stop --force`; idle exit; `SIGTERM` or `SIGINT`, or Ctrl-C or Ctrl-Break on Windows, for `--foreground` |
| An auto-start, by `--server auto` or `SocketCoreBridge("auto")` | its idle exit, 10 minutes by default, or `server stop` |
| A server of another key, which `server list` shows | its idle exit, or `server stop --server <address>` |
| `--server …`, or `GWZ_SERVER` | `--no-server`, for one command; unsetting or emptying `GWZ_SERVER` |
| A session's `open` | `session.close`, or channel closure |
| The stdio child of `--server stdio`, per command | its session's end |
| The `ssh` process of the SSH remote form, per command | its session's end |
| `SocketCoreBridge(address)`, through `Client` | `Client.close()`, or leaving `async with` |
| A server's log, as it grows | its rotation at 10 MiB, which keeps one previous file |

## 18. Closure map

Each TR1.3 clause, with the section that answers it.

| Source | Clause | Answered in |
| --- | --- | --- |
| Plan TR1.3 | Reuse into scope (§1, §10); §4's "gains nothing" as question 5 allows | §1, §10; §4 "What a server grants" |
| Plan TR1.3 | Listener verification: POSIX peer credentials and file checks; Windows token and SQOS; the `/tmp/gwz-<uid>/` rule; no frame and a named code | §4 "Listener verification"; §9 (82); §17 |
| Amendment §3.4 | The listener's process passes the sandbox refusal on all three platforms (`SO_PEERCRED`, `LOCAL_PEERPID`, `GetNamedPipeServerProcessId`); a check that cannot be made refuses | §4 "The sandbox rule", "Listener verification"; §11 |
| Amendment §3.4, replacing plan TR1.3's off-switch clause | Native routes: switch on, the must-match set; switch off, the route-time check and its message | §5 "Native routes through a server" |
| Amendment §3.4 | The enumerated list, per platform and from the vendored sources, OpenSSL's start-up reads included, recorded as a contract §5.8 amendment beside the must-match list | §5: the per-platform tables, the exclusion table, the comparison and the OpenSSL configuration scan; §8 §5.7 and §5.8 |
| Amendment §3.4 | Whether the `auto` key covers the native values with the switch off | §5 "The `auto` key and native routes"; §4 |
| Plan TR1.5 "Servers" | The resolved value's carrier in `SessionOpen`; the rule TR1.3 and TR1.5 match | §5 "The off switch's value"; §3; §9 |
| Plan TR1.3 | One agent source per session; Pageant only when selected explicitly, after a fixture | §5 "One agent source per session"; §11; §12 cells; §17 |
| Plan TR1.3 | Defaults: OD2; a sandboxed caller with `GWZ_SERVER` set, including `--server auto` from inside a sandbox | §7 "Selection", "Sandboxed callers"; §4 "Sandboxed callers". Answered as written, on all three platforms, with no departure |
| Plan TR1.3 | Idle: `--idle-exit` and the pool's idle expiry | §6 "Idle" |
| Plan TR1.3 | The `auto` key and TR1.2's eligibility rule | §4 "The key and the registry partition different things" |
| Plan TR1.3; OD9 | G1's wording | §1; §8 |
| Plan TR1.3, as amended | Cells, the stdio mode's included, with evidence kinds | §12 cells |
| Amendment §3.4 | Addresses: the forms; every refusal and its one code; the walk and its probes; links and their bound; the `auto` path, host included; the parser's type; how a direct-map trigger presents, and the file-system types | §3 "Addresses", the walk and its type table; §4; §9 (81) |
| Amendment §3.4, §3.6 | Corrections this design carries: the named macOS probe, which would mount a trigger; the Linux automount flag, which never marks autofs; `/net`, off by default on macOS | §3; §8, the replacement text and "On GO" |
| Amendment §3.4 | The stdio mode: spelling, stream, lifetime, options, trust, descriptors and signals at both ends, environment, reuse, the local client form, no library client | §15 |
| Amendment §3.4 | Whether `StreamCoreBridge` points at `gwz-py server stdio` | §15 "The contract's test host"; §8 §12 |
| Amendment §3.4 | The SSH remote form: a separable section, questions 1–9 | §16; status |
| Amendment §3.6 | Phase 7 exit rows: refused addresses, sandboxed listeners, native routes, the handshake, the stdio mode | §12, rows marked "(Phase 7 exit)" |
| Amendment §3.11 | OD12's "If yes" edits: the contract §1 text; the Phase 7 rows | §8 §1; §16 questions 8 and "If OD12 is yes" |
| Amendment §3.12 | Risks: the stdio stream, native routes, the SSH form | §14 |
| Amendment §5 | Surface freezes the stdio spelling and options, the client forms, the grammar's refusals and code, and the SSH form | §15; §17; §3; §16 |
| Plan TR1.3, as amended | Surface covers the `server` family, `--server`, `GWZ_SERVER`, `--no-server` and `SocketCoreBridge` | §17 |
| Plan Phase 7 exit | §12 on all three platforms; the client-side squatted listener; the off switch as flag and variable; the sandboxed caller; reuse through a server; the checker's release gate | §12 |
| Plan Phase 10 | The post-release check's `server` spelling | §8 "On GO" |
| Reuse design §13 | §1 (SRV:33): reuse leaves the out-of-scope list | §1 |
| Reuse design §13 | §4 (SRV:125): "gains nothing" stands, and says why | §4 "What a server grants" |
| Reuse design §13 | §4 (SRV:128): zeroization, and an instance's configuration outliving the session | §4 "Secrets" |
| Reuse design §13, §7 | §6 (SRV:200-204): the disposal step after step 3 | §6 "Shutdown", step 4 |
| Reuse design §13 | §10 (SRV:308): reuse happens here | §10 |
| Reuse design §11 | The two partitions compose; `--idle-exit` needs no pool rule | §4; §6 "Idle"; §8 "On GO", which corrects the note for the agent source |
| Reuse design Verdict-1 | Carried: the `auto` key for routed operations with the off switch off | §5 "The `auto` key and native routes" |
| Plan §1, as amended; plan Phase 7 | The design the release carries, on contract revision 5 as the reuse design amends it; the phases the server steps follow | Status; introduction |

**The first remediation plan.** Each finding of the [first verdict](GwzCoreServerDesign-Verdict.md), as the [remediation plan](GwzCoreServerDesign-RemPlan.md) disposes of it, with the sections that resolve it. §12 holds each closure test.

| ID | Resolved in |
| --- | --- |
| S-P1-1 | §4 "Files the host opens"; §5 "The files it reads"; §6 "Starting in the background", "Logging"; §11 `--log PATH`, per-user directory; §12 socket host and OpenSSL rows |
| S-P2-1, C-P3-1 | §4 "The sandbox rule", "Sandboxed callers"; §7 "Sandboxed callers"; §11 sandbox rule; §12 listener verification, socket host and client rows; §14 "Sandbox detection"; this section's "Defaults" row |
| S-P2-2 | §3 "The handshake", its bound and a well-formed `SessionHello`; §4 "Listener verification"; §8 §3; §9; §12 "The handshake's bound"; §15 "The local client form"; §16 question 7; §17 77 |
| S-P2-3 | §4 "One server per address"; §6 "Probing first"; §11 "Stale listener"; §12 socket host |
| Su-P2-1 | §5 the timeout row; §6 "Defaults"; §15 "Options"; §17 options |
| Su-P2-2 | §6; §8 "On GO", the release plan's Phase 10 line; §15 "Spelling"; §16 question 2; §17 "Commands", "Help"; the spelling throughout |
| C-P3-2 | §5 "Files read once", which defines a file's identity, and "The key"; §4 "The default address" |
| C-P3-3, S-P3-11 | §4 "The default address"; §5 "The `auto` key and native routes"; §7 "Python library"; §14 "Native routes through a server"; §17 79 and "Python" |
| C-P3-4 | §8 §4.2, §10, §13 and §15, and the contract's status sentence in "On GO" |
| C-P3-5, S-P3-12 | §6 "The address"; §15 "Options"; §12 stdio rows |
| C-P3-6 | §3 "Every byte stream uses the handshake"; §8 §12; §15 "The contract's test host"; §12 gwz-py |
| C-P3-7 | §8 "On GO", the reuse design's §11 note and status |
| C-P3-8 | §8 §5.6 to §5.8, and "On GO", GWZDesign's two sentences |
| C-P3-9 | §2 |
| C-P3-10 | §3 "Refusal"; §17 "Python"; §9; §12 `serve_session`; §6 `server stop`'s bound |
| C-P3-11 | §5 "Local pipes only"; §12 Windows, on dabeest |
| S-P3-1 | §3 "Tags and order"; §4 "Listener verification", "Secrets"; §12 "The handshake's bound" |
| S-P3-2 | §4 "One server per address"; §6 "Listing"; §11 "One server per address"; §12 socket host |
| S-P3-3 | §6 "Shutdown", with the reason the order stays, and "Crash"; §7 "Python library"; §14; §17 77 |
| S-P3-4 | §6 "Starting a copy of itself"; §11 background start and stdio child; §15 "Descriptors and signals"; §12 socket host |
| S-P3-5 | §3 "Versions and identity"; §14 "Mixed versions"; §17 78 |
| S-P3-6 | §4 "The sandbox rule", "Listener verification"; §11; §12 Windows, on dabeest |
| S-P3-7 | §4 "The default address"; §11's listener row, and its row on claiming the address first; §14 "A sandboxed process can hold an `auto` address"; §12 socket host |
| S-P3-8 | §5 "Local pipes only"; §6 "Logging"; §12 Windows; §17 existing codes |
| S-P3-9 | §5 "The key"; §6 "Listing"; §12 OpenSSL rows; §17 77 |
| S-P3-10 | §5 "Relative paths are refused"; §15 "Environment"; §12 `serve_session`; §17 79 |
| S-P3-13 | §3 "A well-formed `SessionHello`"; §16 question 7; §17 77 |
| Su-P3-1 | §6 "Defaults"; §17 options and "Help" |
| Su-P3-2 | §17 "Addresses", "The per-user directory", "Files", "Log path rules" and "Help" |
| Su-P3-3 | §17 "Help"; §15 "The stream" |
| Su-P3-4 | §6 "Probing first"; §17 output and JSON |
| Su-P3-5 | §6 "Listing"; §17 output and lifecycle pairs |
| Su-P3-6 | §6 "Shutdown" and `server stop`; §17 options, output and lifecycle pairs |
| Su-P3-7 | §17 "Errors", the hint rules, and 79's file-read-once message |
| Su-P3-8 | §17 81 |
| Su-P3-9 | §17 79 |
| Su-P3-10 | §17 existing codes |
| Su-P3-11 | §7 "Diagnostics"; §17 `--verbose` |
| Su-P3-12 | §6 "Starting a copy of itself"; §17 "What each form starts" and 77 |
| Su-P3-13 | §16 questions 1, 2 and 7; §17 "The SSH remote form", 77 and `--verbose` |
| Residual notes, Safety | §5 "Files read once" and §6 "Idle": a server exits at its next idle point after a changed file; §14: the fallback for `GetNamedPipeServerProcessId` |
| Residual notes, Surface | §7 and §17: the `hook` family; §17: gwz-py's option placement, `--jsonl`, `--dry-run` and the start message; §6 "Logging": the log's bound |
| Residual notes, Consistency | §6 "Pairs": `start --foreground` on a taken address; §17 77: the handshake's scope of each bound |

**The corrections after GO.** The re-verdicts on revision 2, [Consistency-1](GwzCoreServerDesign-ReviewConsistency-1.md), [Safety-1](GwzCoreServerDesign-ReviewSafety-1.md) and [Surface-1](GwzCoreServerDesign-ReviewSurface-1.md), cleared these to land without a further round.

| ID | Resolved in |
| --- | --- |
| Consistency P3-1 | §3 "Versions and identity"; §6 "Probing first" and "Listing"; §14 "Mixed versions"; §17 78 and the `list` row; §12 gwz-cli lifecycle |
| Consistency P3-2 | §5 "Relative paths are refused"; §17 79; §12 `serve_session` |
| Consistency P3-3 | §4 "On Windows"; §6 "Probing first"; §11 "Stale listener"; §12 socket host, on Windows |
| Safety P3-14 | §4 "The record" and "A held lock"; §3 "The handshake's bound"; §6 "Starting in the background" and "Listing"; §17 output and 77; §12 socket host |
| Safety P3-15 | §3 "The handshake's bound"; §4 "A stale socket"; §14 "A socket is stale only with gwz's record"; §17 77; §12 "The handshake's bound" |
| Safety P3-16 | §4 "On Windows"; §11 listener row; §17 77; §12 socket host, on Windows |
| Surface P3-14 | §17 "Help"; §12 gwz-py |
| Surface P3-15 [SSH form] | §17 "The SSH remote form" and "The `hook` family"; §12 command surface |
| Surface P3-16 | §17 existing codes; §12 Windows |
| Surface P3-17 | §17 existing codes; §12 gwz-cli lifecycle |
| The stale-socket rule, decided after these corrections: a socket is removed only when its connection is refused and the lock file beside it names it | §4 "The record", "Nothing before the checks" and "A stale socket"; §6 "Probing first" and "Shutdown" step 5; §11 "Stale listener"; §14 "A socket is stale only with gwz's record"; §17 "Files" and 77; §12 socket host |
| Residual notes | Consistency R1: §8 "On GO"; R2: §12 socket host; R3: §17 existing codes. Safety §4: §14 "Without `/proc`". Surface §3: §17 output, options and 81; §12 command surface. Consistency R4, Linux container CI, stays a note for TR1.4b |

## Changelog

- 2026-09-27: status notes that the transport release ships the server, pending TR1.3 of [`GwzTransportReleasePlan.md`](../gwz-core/dev-docs/GwzTransportReleasePlan.md).
- 2026-09-27: status adds the TR1.3 changes that [`GwzTransportReleasePlanAmendment.md`](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md) requires.
- 2026-09-28: revision 1 applies TR1.3, as the plan amendment extends it. It builds on contract revision 5 as the [reuse design](GwzConnectionReuseDesign.md) amends it, and remains a DRAFT awaiting review.
  - Reuse is in scope. The shutdown gains the instance-disposal step, and §4 and §10 carry the reuse design's §13 edits.
  - Addresses are parsed before any open, with a file-system walk and one error code, `server_address_refused`.
  - Clients verify the listener before sending a byte, with `server_listener_refused`. The sandbox rule is relative on Linux, and compares integrity on Windows.
  - §5 restructures the must-match set. It adds the Linux TLS roots the transport reads from the process, native routes through a server, and the off switch's carrier, `ProcessAttributes.transport_off`. The `auto` key covers the native values whatever the switch.
  - One agent source per session. Pageant only when `SSH_AUTH_SOCK` names its pipe.
  - OD2's default and OD9's G1 are recorded. Sandboxed callers are refused before any start.
  - New: the stdio mode (§15), the SSH remote form as a separable section (§16), the user-facing surface (§17) and a closure map (§18).
  - The handshake gains the host-environment marker. The schema gains three error codes and `server.stop`'s messages.
  - Corrections: the lock file stays after exit; a stale socket is removed only if it is one; logs omit remote URLs and repository names; copies of the CLI never start in the caller's directory; `SocketCoreBridge` reuses `NativeCoreBridge`'s pump; §7 records that exact path handling is already in the tree.
  - The libgit2 timeout is checked when an operation takes the native path, not when `configure_transport_runtime` sets it. Revision 0's rule refused a client with another `--ssh-timeout` even for transport remotes, and missed a session that kept the default while the server ran another value.
  - §5 gains the per-platform tables of environment reads, from an enumeration of the vendored sources, and the exclusion table for the source-level test. With them:
    - OpenSSL's reads join every session on Linux and macOS, since the transport's own SSH crypto and, on Linux, its TLS use the process's OpenSSL. `ProcessAttributes.openssl` carries the client's library;
    - Windows adds `XDG_CONFIG_HOME`, `APPDATA`, `LOCALAPPDATA`, `PROGRAMDATA` and `PATH`; Linux and macOS add `APP_SANDBOX_CONTAINER_ID`, `SUDO_UID` for root, and the dynamic loader's variables;
    - `HOME` also fixes native SSH's known_hosts, its only host-key check; the credential-helper spawn is git2's;
    - unset and empty stay distinct; the certificate variables compare as openssl-probe resolves them, whatever the capture timing;
    - OpenSSL's configuration is scanned for `$ENV::` references at a server's start, and files read once are checked for change;
    - the native path's agent on Windows is Pageant first; the logon session joins the native row; a client whose real and effective user IDs differ uses no server;
    - vendoring OpenSSL is recorded as an open point for the operator.
  - §3 gains the address-safety facts:
    - the macOS walk probes with `getattrlistat` before any open, since any `open` mounts a direct-map trigger; §8 gives replacement text for the amendment's named probe, its `/net` sentence and its §3.6 macOS rows, which rest on a map macOS no longer enables;
    - the Linux walk opens single components with `O_PATH | O_NOFOLLOW`, refuses every FUSE file system, and keeps the type check, the only one that catches autofs;
    - socket paths lose empty, `.` and `..` components, and macOS's kernel-rewritten prefixes;
    - a Windows pipe name is one component from an allowlist, checked with `GetFullPathNameW` and `QueryDosDeviceW`; agent pipes get a local-only rule of their own, since Pageant's embeds the user name;
    - the gap between the walk and the connect is narrowed per platform, and its residual is an unwanted mount, not disclosure;
    - process IDs are pinned against reuse; macOS reads the audit token;
    - §12 takes the fixtures, by repository and platform.
- 2026-09-28: revision 2 applies the [first remediation plan](GwzCoreServerDesign-RemPlan.md) after the [first verdict](GwzCoreServerDesign-Verdict.md), in one patch, and remains a DRAFT awaiting re-verdict. §18 gives each finding the section that resolves it.
  - The plan's three marked choices:
    - the Linux sandbox rule is absolute: a seccomp filter or a user namespace makes a process sandboxed, and a peer in another mount or user namespace is refused. The relative rule and its three claims are withdrawn. Every sandboxed caller is refused before any connect or start, on all three platforms, and no server runs in a container with a seccomp profile or a user namespace;
    - the host speaks first, with a secret-free `SessionHello` (tag 6). The client sends nothing until it holds one, and waits at most 30 seconds for `SessionOpened`, or 5 for the `server` subcommands, which use `ServerControl` and `ServerState` (tags 7 and 8) in place of revision 1's `server.stop`. It refuses another core version or build before the snapshot crosses;
    - the `server` command's actions are subcommands: `start`, `stop`, `status`, `list` and `stdio`.
  - Files in the per-user directory and at `--log` are opened without following links, as the user's own regular 0600 files; the log is only appended to, and rotates at 10 MiB. The lock file records its holder. A stale socket is removed only when a connection to it is refused, and `server start` probes first, so a refused start leaves nothing behind.
  - `--ssh-timeout` defaults to 9 seconds in every role, as the retry plan sets it.
  - The Windows `auto` pipe name is random, drawn at each start and recorded in the lock file. A Windows listener must have the client's own integrity level.
  - Relative must-match paths are refused. The client's OpenSSL scan fails closed, and the scan reads only regular files within a bound. A file's identity is defined once. A server that meets a changed file exits at its next idle point.
  - Only the standard streams reach a started copy or a stdio child. Core's launcher spawns both, and gwz-py's `ssh`, through the extension.
  - Shutdown keeps its order, with the reason; a draining server refuses new sessions, and `server stop --force` ends its wait for detached workers.
  - §17 is self-contained, with frozen help, a message template per class and per kind, `server list`, and `--verbose` in JSON output. The `hook` family always runs in-process.
  - §8 gains the contract's enumerations (§4.2, §10, §13 and §15), exact text for the contract's credential-helper sentences, and "On GO" replacement text for the release plan's Phase 10 check, the reuse design's §11 note and GWZDesign's two credential-helper sentences.
- 2026-09-28: accepted as a design at SHA-256 `9fc80261…` ([Verdict-1](GwzCoreServerDesign-Verdict-1.md)). All three axes reported GO on revision 2. The corrections they cleared without a further round were applied after the GO, together with a narrowing of stale-socket removal that the lane owner decided and all three reviewers confirmed: a socket is removed only when the lock record beside it names it.
- 2026-09-28: the operator decided OD12: the SSH remote form ships in this release. The operator also signed off the corrections §8 carries to the release plan's Phase 10 check and to the amendment's §3.4 and §3.6. The "On GO" list was applied to the contract, the release plan and its amendment, the reuse design, gwz-core's GWZDesign and GWZRequirements, and the proposals' G1 row.
- 2026-09-28: erratum in §12's Linux walk row: without `/proc` the client refuses before any connect, as §3 and §14 state. The row had kept revision 1's "connects by the path".
- 2026-10-01: amended by [`GwzTransportReleasePlanAmendment-2.md`](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2.md), accepted at SHA-256 `c5850e52…` ([its verdict](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-Verdict.md)). Its rule "One agent source per session" governs the in-process transport from 1.1.0. No other text changes.
- 2026-10-02: amendment 2's revision 4 applies the operator's OD15: on Windows the transport does what libgit2's native path does. Under its §3.18, the Pageant bullet of "One agent source per session" (a visible Pageant window first, through the transport's own Pageant protocol), lines 497, 512 and 513, the Windows logon session's move from the native row (line 365) into a new every-session row with lines 470, 478, 812 and 1435 following, §11's agent cell (line 813), §12's rows and cells (lines 926, 987, 996 and 1007), §14's two risks (lines 1044 and 1048), §17's environment sentence (line 1348) and the closure row (line 1499) read as that section states. Revision 4 was skim-reviewed only.
- 2026-10-02: amendment 2's revision 5 records the operator's reversal of OD10 and OD11: the transport runs configured credential helpers and signs with every agent key type 1.0.17 uses, so with the switch off only `git://`, `http://` and `file://` remotes take the native path. Under its §3.19, lines 476–478 (routed native operations), 484 (the `auto` key's alternative, for this design's next revision), 916–917 (the Phase 7 exit rows), 1005 (the verification row), 1435 (the routing message) and 1495 (§18's row) read as that section states. Revision 5 was skim-reviewed only.
