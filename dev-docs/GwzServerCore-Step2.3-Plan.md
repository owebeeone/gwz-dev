# Step 2.3 `gwz server` core side: split into sub-steps 2.3a to 2.3l

Date: 2026-10-31. Status: **plan for the lane owner and operator; nothing here is accepted or started.** It splits step 2.3
of [GwzTransportReleaseConvergencePlan.md](GwzTransportReleaseConvergencePlan.md) (revision 5, §4 Phase 2) because the step
as written (about 400 + 450 lines) does not hold the carried items the plan appended to it. Read at root `bcfa8ac7`,
gwz-core `a7b965c5`, gwz-transport `0a25363`, gwz-cli at the root's pin. Read-only inputs: the plan, `GwzSplitCommandChannel.md`
(the 2.0 document, "split doc"), the parked server design (`history/parked/GwzCoreServerDesign.md`, "server design"), the
contract. This file is the only change.

## 1. Scope and inputs

**The step's text** (plan :656-667): the server command runs the 1.7 driver-only builder behind the 2.1 carrier with the 2.2b
operation service; accepts clients per 2.0; bounds the handshake and idles the carrier per the server design; on carrier loss
cancels the whole core operation (local phases included) and records the outcome core-side; `gwz server stdio` refuses, in
`cli` placement, an inherited `SSH_AUTH_SOCK` or any credential variable; the client-side admission of 2.0(c) is implemented
in the client endpoint's open path (SSH rows here, HTTPS in 2.4). Red rows: (R1) a dead client without EOF does not wedge the
server; (R2) carrier drop mid-push: the client reports a possible remote effect, the core records no success, nothing replays;
(R3) a core-chosen destination outside the allowed set is refused with the destination named; (R4) a receive-pack open under a
fetch-class command is refused; (R5) a core-named identity path is refused; (R6) a `SessionOpen` with an environment is
refused; (R7) stdio with an inherited agent socket is refused. After 1.7, 2.2, 2.2b, 2.2c: all landed (1.7 `a681cf26` merged
`7e7d36a9`; `gwz_session_candidate` retired, gwz-py `test_candidate_switches.py:19`; 2.2b `9359a180`; 2.2c `09f4b8a8`).

**Carried and attached items.** Status is as of the tuple above, with evidence.

| # | Item | Source | Status and evidence |
|---|---|---|---|
| C1 | Sandbox rule platform checks: Linux seccomp and user/mount namespaces, macOS `sandbox_check`, Windows AppContainer | plan :668-669; server design :257-277, :288-296 | **Open** on Linux and macOS: no sandbox code (`carrier/local/mod.rs:34-36` says it belongs to 2.3). Windows: AppContainer token refusal and below-Medium refusal landed in 2.1 (`peer.rs:105-158`, `windows.rs:176`), but no real-AppContainer row (`tests/windows_rows.rs:152-190` has only a Low-integrity row) |
| C2 | Address walk | plan :668-674 (the 2.1 paragraph) | **Open**; no `ServerAddress` type exists |
| C3 | `SessionHello` and a bounded `connect` | plan :668-674; split doc :130 | **Open**: `frame.rs:94-95` has the tags only, no body is encoded anywhere in gwz-core; `local/unix.rs:585` `connect` blocks |
| C4 | The 30 s link check | plan :668-674; split doc :234 | **Open** |
| C5 | Drain-before-exit; shutdown order | plan :668-674; split doc :232; server design :586-600 | **Open** |
| C6 | Lock file, stale-socket rule, `ws-<key>` naming replacing 2.1's random `s-` names | plan :668-674; split doc §1 table, :233 | **Open**: `unix.rs:530` draws `s-<16 hex>` |
| C7 | The CLI client rows the plan's 2.1 text named | plan :668-674; plan 2.1 text (round trip, squatted listener, remote pipe, other-user, lower integrity, environment `SessionOpen` + canary, saturated cancel, oversize length) | **Open** for the rows that need a client binary; see 2.3k, 2.3l and Q2. The carrier-level versions landed in 2.1 (`tests/local_rows.rs`, `ending_rows.rs`) |
| C8 | Carrier stall bounds set from policy (mechanism has 60 s defaults) | plan :668-674; split doc :129 | **Open**: `carrier/mod.rs:183-198` `StallBounds::DEFAULT` |
| C9 | `SocketCoreBridge` rows | plan :668-674 | Moved to 3.2; not planned here |
| C10 | §5.1 member lock manager in the host context; "blocks rather than refusing" within a session | plan :675-680; session design :224 | **Open**: `HostContext` (`session_host/context.rs:53-69`) has no manager; `operation/membermutationguard.rs:6-27` is a try-lock used only by `operation/tests.rs:365` |
| C11 | Call `Service::open()` before writing `SessionOpened` | plan :675-680; `service.rs:646-651` | **Open**: `served.rs:539` builds the only session with `opened = true` (in-process) |
| C12 | Transport-lane routing is the carrier's; 2.2b's duplicate removed | plan :675-680 | Landed (`served.rs:24-26`); residue is C15 |
| C13 | Three 2.2c P3s: State P3-5 (more than 16 waiters lose a wakeup), Code P3-7 (`BindBounds` and `RemoteConfig` built apart), Code P3-8 (`Fault(Capacity)` right after an abort) | `Step2.2c-RemPlan.md:58-65`; `-ReviewState-2.md:71`; `-ReviewCode-2.md:82, :98` | **All open**: `remote/core.rs:187-208` (`wait_for` evicts `same[0]`), `remote/core.rs:673`, `remote/lane.rs:23-37` beside `remote_lane.rs:62` |
| C14 | 2.2c items "recorded for step 2.3": lock order remote-state then registry when `Calls` is implemented; idle polling; no terminal for a peer's cancel; `CarrierLane` serves both roles; ignored `spawn` results | `RemPlan.md:39-41`; `ReviewState-2.md:122-125` | Lock order is **planned** (2.3h). The other four are limitations, not prerequisites: they stay findings (Q5) |
| C15 | Dead-code allowances that name 2.3 or 2.2c | `served.rs:497-503, :517-520`; `served/dispatch.rs:49-52`; `served/lane.rs:33, :55-58`; `RemPlan.md:36-38` (one encoder) | **Open**: removed by 2.3g and 2.3h as they gain callers |
| C16 | Windows allow-list entries of the client's `ssh` environment, "fixed from probe (1) before 2.3 lands" | split doc :202, §7 :361 | **Open** (planned in 2.3j) |
| C17 | D-P (one operation per call; client admission default 4 s) and D-T (session-host splits) | plan :969, :992 | **Landed**: gwz-transport `0a25363` / gwz-core pin `25039b78`; `d72905ba` (`service/table.rs` 15 lines, `service/tests.rs` 25 lines) |
| C18 | Two rows the split doc names "2.3 red row": a refused push tuple fails the push before any member is pushed; a `--dry-run push` with a push allowance admits no `open` | split doc :330-331 | **Open**: 2.3l |
| C19 | Core maps an admission `open_failed(invalid_request)` to `server_destination_refused` (86); bind and identity refusals keep their codes | split doc §5.6 :350 | **Open**: 86 exists in the schema (`generated.rs:1654`) with no producer |
| C20 | `--max-sessions` refuses with `transport_session_full`; one workspace per server; concurrent clients get CLI-equivalent locking | split doc :229, §1 table | **Open** |

Not in scope: HTTPS rows (2.4), the R5/R7 proof (2.5), `--server` routing of ordinary commands and `--allow-*` options (3.1,
Q2), `SocketCoreBridge` (3.2), any gwz-py server (D-O).

## 2. Current state of the server path

- **Pieces that exist.** `DriverOnlyBuilder` and `DriverOnly` (`transport_host/driver_only.rs:30-176`, exported at
  `mod.rs:17`); the stream carrier and its local listener, connect and peer check (`carrier/stream.rs`,
  `carrier/local/unix.rs:511-609`, `windows.rs`, `peer.rs`); `validate_session_open` (`carrier/wire.rs:228`) and the
  refusal path (`stream/inbound.rs:224-230`); the operation service (`session_host/served.rs`, `crates/session-host`,
  `Service::is_live` at `service.rs:664`); `CarrierLane` and `serve` (`remote_lane.rs:49-211`) with the `Calls` trait
  (`:39-47`) that nothing implements; the remote handle and the client's `Receiver` and admission
  (`gwz-connection/src/remote/`, `admission.rs:239`, `receiver.rs:108`, receiver rows at `tests/receiver_rows.rs:388, :462,
  :538, :704` against fake allowed sets); a scoped join of a driver `TransportPort` to a carrier binding
  (`carrier/link.rs:242`, exercised by `carrier/tests/seam_rows.rs:226-236`).
- **Pieces that do not.** No `SessionHello`, `SessionOpened`, `ServerControl` or `ServerState` body or state machine; no
  byte-stream session (`serve_in_process`, `served.rs:521`, is the only way to start a `Service`); no accept loop, lock,
  record, address type, log, shutdown or signal handling; no `server` module and no `pub` path to the carrier (`mod carrier`
  is private, `transport_host/mod.rs:10`, and `transport_host` itself is behind `gwz_transport_candidate`, `lib.rs:47-51`),
  so gwz-cli cannot reach it today; no `gwz server` in gwz-cli (the pattern for a command that owns its lifecycle is `hook`:
  `parser.rs:304-307`, `lib.rs` `CliRequest::Hook` branch, `hook_exec.rs`); no producer of code 86; the `Calls` trait has no
  `endpoint_path_base` query on `Service`.
- **Consequence for size.** The plan's 2.3 estimate counted the composition. The carried list adds the whole listener and
  client-connect surface of the server design (address grammar and walk, handshake, lock, stale socket, shutdown), the
  sandbox probes and the CLI family. Summed below: about 4,300 implementation lines, a little more again in rows.

## 3. Sub-steps

Each is one goal, aimed at 500 implementation lines or fewer (rows counted apart, as the plan counts "~400 + 450"). A lane
that cannot fit reports first. Conventions for all of them: new modules sit beside the code they extend (`carrier/local/`
for OS-level pieces, a new `gwz-core/src/server/` module behind the same `gwz_transport_candidate` switch for composition),
each step adds its own `scripts/checks/windows_parity/2.3<x>.json` (one file per step, so inventories never conflict), and
`carrier/local/mod.rs` / `carrier/local/{unix,windows}.rs` are shared only by named function (below). Tier: dual = Code and
State reviewers on the step; single = one reviewer of the axis named; the Phase 2 review (2.6) covers the slice either way.

**2.3a Peer sandbox rule, platform probes (dual: peer admission).** Goal: a same-user peer that is sandboxed is refused on
Linux and macOS, a sandboxed caller cannot connect or start, and a real AppContainer process is shown refused on Windows (C1).
Owns: create `carrier/local/sandbox.rs`, `sandbox/{linux,macos}.rs` (`windows.rs` only if its row finds a gap),
`carrier/tests/sandbox_rows.rs`; edit `local/peer.rs`, `local/unix.rs` (`check_peer` only), `local/mod.rs` (mod and
re-export lines), `scripts/checks/check_carrier_peer_rule.py` and its test. Reads: all other carrier files. Red rows: a child
under an allow-all seccomp filter (Linux), a child in a new user namespace, one in a new mount namespace, an unreadable
`/proc/<pid>` status, a `sandbox-exec` child (macOS), a real AppContainer child (Windows): each refused with zero bytes read
and one `server_peer_refused` frame; the unsandboxed control accepted; the caller's own sandbox refuses `PeerPolicy::current_user`;
decision-table rows over injected findings; each probe mutation-red. Size ~350 + 350. After: none. Evidence: Linux on dabeest
WSL as root (namespaces) and on the hosted ubuntu runner; macOS on the Mac and the macOS CI leg; Windows receipt (new
process rows). The brief is §5.

**2.3b Carried gwz-connection fixes (single, State).** Goal: close C13. Owns: gwz-transport
`crates/gwz-connection/src/remote/{core.rs,backend.rs,lane.rs}`, a new `remote/tests/carried_rows.rs`, the crate README note;
gwz-core `transport_host/remote_lane.rs` and `remote_lane/tests.rs`; the pin file `.github/gwz-transport.commit` and
`scripts/checks/process_globals_allowlist_gwz_transport.json` `reconciled_commit` (gwz-transport pushed first). Reads: the rest
of `remote/`. Red rows: 17 waiters on one registration each woken once (per-future slots, not eviction); `CarrierLane::core` and
`handle_with` take one config, and a core at `streams_per_binding = 128` with a lagging client faults nothing, with a row that
pins the client-side ceiling; an open right after an abort at the stream bound waits (like `WouldBlock`) and never answers
`Fault(Capacity)`; the contract §15 sentence on `OpenFailure::Fault` agrees (lane owner records). Size ~150 + 250. After: none.
Evidence: gwz-transport CI on three platforms; no Windows receipt (pure logic, say `ci-only`).

**2.3c Member lock manager in the host context (single, State).** Goal: C10. Owns: create `session_host/members.rs` (a blocking,
poison-recovering, cancellation-aware manager); edit `session_host/context.rs` (the field and accessor), the plumbing from
`served/dispatch.rs` to the fetch and push member steps (the lane first reports the call sites in `workspace_ops`, which no Phase
5 row owns, and edits only the narrow port), `operation/membermutationguard.rs` (becomes this manager's guard type; its one test
caller moves). Red rows: fetch beside fetch on one member in one session waits then both succeed (today one fails on Git's ref
lock); two sessions of one host never update a member's refs at once; a wait wakes on cancel; a panic while holding does not
fail later operations; different members do not wait. Size ~300 + 300. After: none. Evidence: Mac and WSL; Windows `ci-only`
(pure Rust, no process or socket rows).

**2.3d Handshake: `SessionHello`, `SessionOpen`, `SessionOpened`, `ServerControl`, `ServerState`, and a bounded `connect`
(dual: wire format).** Goal: C3, R6 and the server design §3 order, on the carrier that exists. Owns: create
`carrier/handshake/{mod,body,host,client}.rs` (bodies as deterministic CBOR beside `wire.rs`; the host and client machines;
30 s and 5 s bounds from the start of the connect; 4 KiB first-frame cap; protocol range and `server_version_mismatch`;
`server_unavailable` with escaped first bytes; the single `SessionError(call_id 0)` refusals), `carrier/tests/handshake_rows.rs`;
edit `local/unix.rs` (`connect` only: non-blocking connect retried on `EAGAIN` until the bound) and `local/windows.rs`
(`connect` only: `WaitNamedPipeW` for the time left), `local/mod.rs` lines, `carrier/mod.rs` (export). Reads: `wire.rs`
(`validate_session_open` is called, not changed), `stream.rs`. Exposes to 2.3g one call that yields the validated open
and a hook that runs before `SessionOpened` is written (C11). Red rows: host speaks first after the peer check; a client that
sends before a well-formed hello is a protocol error; a first frame over 4 KiB or tag other than 6 fails `server_unavailable`
without reading on; a silent host fails at 30 s (injected clock) naming what it can; `ServerControl` gets `ServerState` and
close, and does not count as a session; a `SessionOpen` with an environment, `umask`, `openssl` or `transport_off = true` is
refused with exactly one `invalid_request` and the canary never leaves the client; a version outside the range is
`server_version_mismatch`; a full accept queue is retried to the bound (Linux), refused (macOS), `ERROR_PIPE_BUSY` waited
(Windows). Size ~450 + 450. After: none (the 2.1 carrier). Evidence: Linux WSL and hosted; macOS; Windows receipt (pipe rows).

**2.3e Server address: grammar and walk (single, Code).** Goal: C2. Owns: create `carrier/local/address/{mod,grammar,walk_unix,
walk_windows}.rs` (`ServerAddress`: socket path, pipe name, `stdio`, `ssh://`; `auto` and every refused spelling give
`server_address_refused`; Linux `openat`/`statx`/`fstatfs` walk with the type table and 8-link cap; macOS `getattrlistat` walk;
Windows `GetFullPathNameW` and `QueryDosDeviceW` checks), `carrier/tests/address_rows.rs`. Edit: `local/mod.rs` lines only.
Red rows: the server design §3 refusal list row by row (relative path, `.`/`..`, abstract name, macOS `/.vol`, every Windows
UNC/GLOBALROOT/WebDAV/drive spelling, `\\.\pipe\a\b`, trailing dot); `auto` refused with its reason; a walk through a symlink
chain of 8 passes and 9 refuses; type-table rows with injected `f_type`; real rows: macOS `/System/Volumes/Data/home` (auto_home)
refused; Linux autofs/NFS/FUSE rows where the host can make one (else the row reports "no fixture" in the evidence README, it
does not pass silently). Size ~480 + 450. After: none. Evidence: Mac, WSL, Windows receipt (grammar and `QueryDosDeviceW`).

**2.3f Listener files: lock, record, stale socket, `ws-<key>` names, link check (single, State).** Goal: C4, C6. Owns: create
`carrier/local/server_files/{mod,lock,record,stale,key}.rs`, `carrier/tests/server_files_rows.rs`; edit `local/unix.rs`
(`LocalListener::bind` and a new `bind_at`, `Drop`) and `local/windows.rs` (`bind`, the pipe record), `local/mod.rs` lines.
Content: key = first 16 hex of SHA-256 of the canonical workspace root; `ws-<key>` socket, `ws-<key>.lock` with pid, address,
root, log; the file rule (`O_NOFOLLOW`, regular file, owner, no group/other bits, never truncated; Windows reparse-point refusal);
exclusive lock; record written after the stale check and before bind; the three-part stale-socket test (never delete on a
refused connect alone); clean exit order; the 30 s "still linked and still my inode" check as a callable. Red rows: second
start finds the first; planted symlink/FIFO at lock or log never followed or blocked; foreign socket never removed; macOS
full-queue refusal is not stale; record naming another address blocks removal; crash leaves a stale socket the next start
recovers; unlinking the socket or lock is reported by the check; Windows pipe name recorded only after creation. Size
~450 + 450. After: none (uses the existing `RuntimeDir`; 2.3e's walk plugs in at the call site in 2.3i). Evidence: Mac, WSL,
Windows receipt (pipe and lock rows).

Carried in (2026-10-31): from the 2.3c round-2 reviews (blind convergence State R2S-1 = Code P3-N1), the member lock keys a workspace by its canonical path, which does not merge macOS firmlinks, bind mounts or Linux case-folding directories; this step defines one `workspace_identity` function (filesystem identity: `(st_dev, st_ino)` on Unix, volume serial and file index on Windows, inside `cfg_if!`) that both the `ws-<key>` name and `MemberLockManager`'s key use, and `test_support::case_variant` decides sameness by that identity, not by `canonicalize`; red row: a firmlink spelling waits (macOS). With it, Code P3-N4 (State §4's hypothesis): the per-call manager keys by the call's own spelling so a volume where `canonicalize` fails does not refuse direct CLI and gwz-py calls; the host manager keeps refusing.

**2.3g Serve a session over a stream carrier (single, State).** Goal: C8, C11, C20 (session half), R1. Owns: create
`gwz-core/src/server/{mod,session}.rs` (and `pub mod server` in `lib.rs` behind the candidate switch), `served/stream.rs`
(a byte-stream counterpart of `serve_in_process`: the carrier's registry frames go to `Service::route`, replies go back through
the carrier with their `FrameKey`, `Service::open()` runs before `SessionOpened`, carrier end maps to `channel_closed(Channel)`
and a router error to `Protocol`), `server/tests/session_rows.rs`; edit `served.rs` (remove the two dead-code allowances it
makes live), `crates/session-host/src/service.rs` (the live-call query 2.3h needs, if not placed in 2.3h). Reads: handshake API
of 2.3d. Red rows: a call round-trips over a real local carrier with scripted dispatch; a call before `SessionOpened` is a
protocol error; a session beyond `--max-sessions` is `transport_session_full`; a client that stops reading with no EOF ends the
session at the policy stall bound (set small in the row, 60 s in production) and the next client is served; carrier loss cancels
every live operation including local phases and logs method, id, outcome and duration core-side; cancel under a saturated spool
progresses over the real carrier. Size ~400 + 400. After: 2.3d. Evidence: Mac, WSL, Windows receipt.

**2.3h Per-call transport composition (single, State).** Goal: R2 core half and C14's lock order. Owns: create
`server/{calls,transport}.rs`, `server/tests/transport_rows.rs`; edit `transport_host/remote_lane.rs` (the `Calls` implementation
and the single `TransportLaneFrame` encoder: the carrier calls `encode_lane`/`decode_lane`, both dead-code allowances go),
`served/lane.rs`, `served/dispatch.rs` (`CoreDispatch::new` becomes live with the 1.7 backend), `crates/session-host/src/service.rs`
(`endpoint_path_base` query). Content: one `DriverOnly` per session; per call a driver `TransportPort` joined to the call's
carrier binding (`CarrierLink::start_joined`, `Join { binding }`); bounds from one `RemoteConfig`; `Calls` takes no lock the cancel
path holds (order: remote state before registry, documented and row-checked). Red rows: a fetch-class call's Bind/Open reaches a
fake client end and its data returns; two concurrent calls keep separate bindings; carrier drop mid-push ends the call with a
possible remote effect, the core records no success and replays nothing; cancel reaches a running exchange; `Calls` under a
concurrent cancel does not deadlock (stress row). Size ~400 + 450. After: 2.3g, and 2.3b for the bounds. Risk: Q3. Evidence: Mac,
WSL, Windows receipt (process-kill row).

Carried in (2026-10-31): from the 2.3c round-2 reviews (blind convergence State R2S-2 = Code P3-N2), nothing pins a member step's guard over its network call: `let _ = steps.enter(..)?`, an `if let Err` temporary, or a renamed receiver passes `check_member_step_boundary.py`, clippy and every row. This step, which edits the per-call composition beside those sites, makes the check require a named, live binding (and reject `drop` of it before the call and an `.enter(` in a condition), widens its receiver match, and adds a test-only `MemberLockManager::is_held` that the tracking doubles assert inside `fetch`, `push_prepared` and `tag_fetch`; closure: the reviewers' mutations U-L, U-M, U-O, U-R, V and W each red. Code P3-N3: one row per tag op on a `mem_` member (a member holder makes it wait; an `@root` holder does not); mutations K2, K3 red. Code P3-N5: the waiter count is production state; declare it or move it behind `cfg_if!`.

**2.3i Server lifecycle, core side (single, State).** Goal: C5, C20, R1 at server level. Owns: create `server/{run,shutdown,log}.rs`,
`server/tests/lifecycle_rows.rs`. Content: the accept loop (peer check, hello, thread per connection), `ServerControl` stop/status,
the five-step shutdown of the server design (stop opening sessions but keep the listener to say why; channel closure; one more
bound for detached workers, ended by `force`; host-context shutdown; remove socket, clear record, release lock), idle exit that
counts only time with no session, the 30 s link check from 2.3f, the log (one line per event, never environment, bodies,
credentials, URLs or repository names; 10 MiB rotation by directory-descriptor rename), SIGINT/SIGTERM and Windows Ctrl-Break
through a handler that sets a flag only. Red rows: `status` in a loop never delays idle exit; stop with a live session closes
it and releases the address last; a second signal or `force` ends the detached wait; a dead client without EOF does not stop
the accept loop; unlink of the socket drains and shuts down; log never contains a canary placed in the request; rotation never
writes through a planted `<log>.1`. Size ~480 + 400. After: 2.3f, 2.3g (not 2.3h; scripted dispatch). Evidence: Mac, WSL, Windows
receipt and dabeest (signals, pipe).

**2.3j Credential variables and `server stdio` (dual: secret handling).** Goal: R7, C16. Owns: create `server/credentials.rs`
(the closed scrub list of split doc §3.8, a pure function), `server/stdio.rs` (one session over standard streams, no listener,
no peer check, no idle, no log file), `server/launch.rs` (argument vectors and the allow-listed child environment for `ssh`,
`sudo` and the local stdio child, pure), `server/tests/{credentials,stdio}_rows.rs`. Red rows: `stdio` with `SSH_AUTH_SOCK` or any
listed variable (`GH_TOKEN`, `GIT_CONFIG_COUNT`, `GIT_CONFIG_KEY_3`, `GCM_X`, ...) refuses with a usage error naming the variable
and the likely start-up file; `LC_FOO` is not forwarded, `LC_ALL` is; `start` scrubs and does not refuse; the `ssh` vector
equals the split doc §4.4 list exactly; canary variables never appear in a spawned child's environment (real child on each
platform); the Windows allow-list entries match probe (1) evidence. Probe (1) is run as part of this step on dabeest to WSL and
from the Mac, archived in the step's evidence run. Size ~250 + 300. After: 2.3g (stdio serves through it). Evidence: all three
platforms, with the probe on a Windows client; Windows receipt.

**2.3k `gwz server` in gwz-cli (single, Surface).** Goal: the command family and its client half. Owns (gwz-cli): create
`src/clirequest/server.rs`, `src/server_exec.rs`, `src/server_after.rs`, `src/server_long.rs`, tests under `src/tests/`; edit
`globalargs/parser.rs` (the `Server` subcommand and the global `--server`/`--no-server` parse, `GWZ_SERVER` read; see Q2),
`src/lib.rs` (run-own-lifecycle branch like `hook`), `help.rs`, `cli_reference.rs`; gwz-core: one `pub` re-export surface in
`server/mod.rs` and `transport_host/mod.rs` so the CLI reaches `Server::run`, `connect_control` and the address type. Content:
`start [--foreground] [--max-sessions] [--idle-exit] [--log]` (probe first; idempotent; background copy via the launcher with
closed descriptors and a new session), `stop [--force]`, `status`, `list`, `stdio`, `--json` records `{address, pid, root, log}`,
the help text of split doc §4.9, `server_address_refused` for `auto`, `gwz --server X server start` a usage error. Red rows: the
§4.9 sequence read cold from `--help`; `start` twice; `stop` of a stopped server succeeds; `status` exits 1 when nothing
answers; `list` shows a lock record whose server died; the client rows C7 that `status`/`stop` can drive: round trip, squatted
listener receives zero bytes, remote pipe name refused, other-user listener refused. Size ~500 + 400. After: 2.3d, 2.3e, 2.3i, 2.3j.
Evidence: gwz-cli CI on three platforms; dabeest run of start/stop/status/list on Windows.

**2.3l End-to-end SSH admission, refusal mapping and the closing rows (dual: admission).** Goal: R2 to R6, C7 (call-carrying
rows), C18, C19. Owns: create `server/tests/e2e_rows.rs` (rows only; production code stays in `src/`); edit `transport_host/session/driver/opening.rs` and `session.rs` (map exactly an admission `open_failed(invalid_request)`
to `server_destination_refused`; the trunk's files, now free; ask the lane owner), gwz-connection `remote/receiver.rs` only if a row
finds a gap. Content: a served core against the 2.2c client receiver with an SSH fixture server and a real local carrier. Red rows:
destination outside the allowed set refused with the canonical destination named and code 86 on the member; receive-pack open
under a fetch-class command refused; core-named key path refused (`identity_check_failed`, existing mapping) and the client prints
the identity text; a refused push tuple fails the push before any member is pushed; `--dry-run push` with a push allowance admits no
`open`; a `SessionOpen` with an environment is refused and the capture of the handshake bytes holds no canary; carrier drop
mid-push against the fixture reports `Effect::Possible`, the fixture remote shows no replay; a dead client without EOF with a push
in flight does not wedge the server. Size ~100 + 550. After: 2.3h, 2.3i, 2.3j. Evidence: all three platforms, Windows receipt,
dabeest including WSL for the SSH fixture.

## 4. Order and parallelism

Critical path: 2.3d, then 2.3g, then 2.3h, then 2.3l. Off it: 2.3a, 2.3b, 2.3c, 2.3e, 2.3f. At most three compiling lanes run on
the Mac (lane concurrency cap).

| Wave | Lane 1 | Lane 2 | Lane 3 | Why these can overlap |
|---|---|---|---|---|
| 1 | 2.3d handshake | 2.3a sandbox probes | 2.3b then 2.3c (two small steps, one lane, two repositories) | d owns `carrier/handshake/` and `connect` in `unix.rs`/`windows.rs`; a owns `carrier/local/sandbox*`, `peer.rs` and `check_peer`; b owns `gwz-connection/remote/` and `remote_lane.rs`; c owns `session_host/` |
| 2 | 2.3g session (needs d) | 2.3f listener files | 2.3e address | g owns `server/`; f owns `carrier/local/server_files/` and `bind`; e owns `carrier/local/address/` |
| 3 | 2.3h composition | 2.3i lifecycle | 2.3j credentials and stdio | h owns `server/{calls,transport}.rs`, `remote_lane.rs`, `served/lane.rs`, `service.rs`; i owns `server/{run,shutdown,log}.rs`; j owns `server/{credentials,stdio,launch}.rs` |
| 4 | 2.3k gwz-cli family | 2.3l end-to-end rows | | k edits gwz-cli and re-exports; l edits `server/tests/e2e_rows.rs` and `opening.rs` |

Shared files, resolved by function and by the lane owner at merge: `local/unix.rs` and `local/windows.rs` (a: `check_peer`; d:
`connect`; f: `bind`/`Drop`), `local/mod.rs` (one line per step), `server/mod.rs` (a `mod` line per step; g creates it),
`transport_host/mod.rs` and `lib.rs` (one export each in g and k). If two lanes would edit one function, the later lane waits.
Merge serially. A step that changes a Windows trigger path gets its own receipt, and the merged commit gets one when both sides
touched trigger paths (plan §5). Run `scripts/checks/check_lane_commits.sh origin/main HEAD` before filing (the 2.1 landing
deviation, plan :618-626). Review: dual on a, d, j, l; single elsewhere; the Phase 2 review (2.6) reads the whole. Totals:
about 4,300 implementation lines and 4,750 rows across 12 steps, against the plan's 400 + 450.

## 5. Lane brief for 2.3a

The brief is also written to `/Volumes/projects/limbo/build-targets/tools/brief-p2-3a-sandbox.md` by the lane owner at spawn;
it is the text between the rules.

---
# Lane brief: p2-3a (step 2.3a: the peer sandbox rule's platform probes; dual review)

**Lane:** `/Volumes/projects/limbo/gwz-dev-p2-3a`, cut from root main at spawn (tuple at planning time: root `bcfa8ac7`, gwz-core
`a7b965c5`, gwz-transport `0a25363`). This is an admission step and will get a dual review. A sandbox probe that errs open lets a
sandboxed process drive a server with its user's full authority; every probe that cannot complete must refuse.

## Why
The 2.1 carrier checks the peer's user, pins its process, refuses below-Medium integrity and AppContainer tokens (Windows), but
`carrier/local/mod.rs:34-36` leaves "the sandbox rule's platform checks (seccomp and namespaces, `sandbox_check`, AppContainer
beyond the token flag)" to step 2.3. On Linux and macOS a sandboxed same-user process (seccomp filter, user namespace, App Sandbox)
passes today. Git runs commands named in a workspace's configuration (hooks, `core.fsmonitor`, `core.sshCommand`), so refusal is
the only sound rule and it is absolute: any sandboxed process is refused whatever sandbox the other side is in.

Governing:
- `dev-docs/history/parked/GwzCoreServerDesign.md` §4 :257-296 ("Sandboxed peers", "The sandbox rule", process-ID pinning, "Sandboxed callers" :288);
- `dev-docs/GwzSplitCommandChannel.md` §4.3 "Peers" :228;
- plan `dev-docs/GwzTransportReleaseConvergencePlan.md` :668-674 (the carried paragraph);
- `dev-docs/GwzServerCore-Step2.3-Plan.md` §3 2.3a;
- code: `gwz-core/src/transport_host/carrier/local/{mod,peer,unix,windows}.rs`, `scripts/checks/check_carrier_peer_rule.py`.

## Deliver (one patch, about 500 lines or fewer including rows; report first if it cannot fit)
1. **Probes**, in new files under `carrier/local/sandbox/` selected by `cfg_if!` platform modules, never a bare `#[cfg]`:
   - **Linux.** Read the peer only through the hold 2.1 takes (pidfd or the `/proc` start-time bracket). Sandboxed when `Seccomp:` in `/proc/<pid>/status` is not 0, or `stat /proc/<pid>/ns/user` is not inode `0xEFFFFFFD`. Also refused when the peer's mount or user namespace differs from this process's. Any unreadable file refuses (`Unreadable`).
   - **macOS.** `sandbox_check_by_audit_token(token, NULL, 0)` on the `LOCAL_PEERTOKEN` token 2.1's hold already has, else `sandbox_check(pid, NULL, 0)`, both resolved with `dlsym`; non-zero means sandboxed. A missing symbol or a failed call refuses.
   - **Windows.** The AppContainer token flag and the integrity rule exist. Add a real-process row; change code only if the row finds a gap, and report it.
2. **Rule.** `PeerRefusal::Sandboxed(Cause)` with a closed `Cause` (seccomp, user namespace, other namespace, profile). The message names the cause, never a path or a pid-derived string beyond what 2.1 prints. `PeerPolicy::verify` stays a pure function of identities: the findings are part of `PeerIdentity`.
3. **The caller's own process.** `PeerPolicy::current_user` refuses a sandboxed own process on all three platforms (client connect and `server start` both build it), with the same refusal type.
4. **Source guard.** `check_carrier_peer_rule.py` stays green and still means what it says: no environment read, no bypass identifier, no cfg switch beyond the listed ones. If the new platform modules need the allowed-switch list widened, widen it narrowly and add a test that a bypass spelling still fails. Probes read `/proc`, never the environment.

## Red rows (each seen red first; keep the red logs and the mutation that made them red)
- **Linux** (real fixtures, no mocks): a child that applies `prctl(PR_SET_NO_NEW_PRIVS)` then an allow-all `SECCOMP_MODE_FILTER` and connects; a child in `unshare(CLONE_NEWUSER)`; a child in `unshare(CLONE_NEWNS)` (needs privilege: run as root on WSL); the same children as listeners, which a client refuses having sent nothing; an unprivileged unsandboxed child accepted (control); a peer that exits between connect and probe refused `Unreadable`; own-process seccomp refuses `current_user`.
- **macOS**: a child under `sandbox-exec -p '(version 1)(allow default)'` refused; plain child accepted; missing-symbol and non-zero-result paths through the probe's injected symbol resolver (a function parameter in private code, not a build switch).
- **Windows**: a real AppContainer child (`CreateAppContainerProfile`, `PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES`) refused as client and as listener; own-process AppContainer refuses `current_user`.
- **All**: the decision-table rows over injected findings; each refusal answers exactly one `SessionError(call_id 0, server_peer_refused)` and then EOF, and the listener read nothing first.
- A fixture the host cannot make (for example unprivileged user namespaces off) fails the row with a message naming the missing capability; do not skip. List in the evidence README which host made which fixture.

## File ownership
- **Create:** `gwz-core/src/transport_host/carrier/local/sandbox.rs`, `sandbox/linux.rs`, `sandbox/macos.rs` (`sandbox/windows.rs` only if needed), `carrier/tests/sandbox_rows.rs` (and its line in `carrier/tests/mod.rs`), `scripts/checks/windows_parity/2.3a.json`.
- **Edit:** `carrier/local/peer.rs`; `carrier/local/unix.rs`, the function `check_peer` and the `Hold` accessors it needs, nothing else in the file; `carrier/local/windows.rs` only for a gap the Windows row finds; `carrier/local/mod.rs` (mod and re-export lines); `scripts/checks/check_carrier_peer_rule.py` and its test.
- **Read only:** everything else under `carrier/` (2.3d owns `connect`, 2.3f owns `LocalListener::bind`), `stream*`, `wire.rs`, gwz-transport, gwz-cli, gwz-py.
- Parallel lanes in this wave: p2-3d (`carrier/handshake/`, `connect`), p2-3b (`gwz-connection/remote/`, `remote_lane.rs`). Report any need for their files.

## Gates
Per `lanes-1024-common.md` as `lanes-1031-overrides.md` amends it:
- `fmt +1.95.0`; the argument-free `scripts/checks/*.py`, including `check_windows_parity.py`, `check_cfg_boundaries.py`, `check_process_globals.py`, `check_carrier_peer_rule.py`;
- `python3.13 scripts/checks/run_boundary_job.py` once on the final tree;
- locally only focused `cargo test sandbox` / `peer` rows with `TMPDIR=/Volumes/gwztmp` and one warm target dir `/Volumes/projects/limbo/build-targets/p2-3a-*`, candidate shape `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real`;
- Linux rows on dabeest WSL as root and Windows rows on dabeest are run by the lane owner, not by you. Do not start `remote-gates.zsh` or `win-receipt.zsh`.

## Evidence
Run `2026-10-31-p2-3a` at `gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-31-p2-3a/` in your lane's gwz-core-evidence: README (question, source tuple, hosts, commands, which host made which fixture, limits), `raw/` (red and green logs, the mutations), `manifest.json` (`{"files":{rel:sha256}}`, indent 1, sorted), `python3.13 -B tools/verify_archive.py` passing. Do not commit it.

## Report
Include: the design and the exact `Cause` list; each row with its red evidence and mutation; what each probe cannot see (for example a Landlock-only sandbox, which is not refused and is not claimed); `git diff --stat`; the parity, cfg and process-globals results; the proposed commit message in `/Volumes/projects/limbo/build-targets/p2-3a-logs/commit-msg.txt`, no AI attribution, ending `Windows-receipt: p2-3a-20261031-a`; the commands you want run, receipt label `p2-3a-20261031-a`:
- `zsh /Volumes/projects/limbo/build-targets/tools/remote-gates.zsh /Volumes/projects/limbo/gwz-dev-p2-3a p2-3a-20261031-a --shape all`
- `zsh /Volumes/projects/limbo/build-targets/tools/win-receipt.zsh /Volumes/projects/limbo/gwz-dev-p2-3a p2-3a-20261031-a -- --worktree --tests --hosted-like --label p2-3a-20261031-a --receipt-dir /Volumes/projects/limbo/build-targets/p2-3a-receipts --taut-proto 0.10.0 --cache-from win-flakes-20261031-d`

Findings outside the step go in the report with evidence, not into code.

Read and obey agent-rules.md, lanes-1024-common.md and lanes-1031-overrides.md
---

## 6. Open questions for the operator

Q1. **Where the server code lives.** Options: (A) a `gwz-core/src/server/` module plus new files beside the carrier in
`carrier/local/` (this plan); (B) a new small crate for the OS-level pieces (address, files, handshake, sandbox), the module
only for composition. Evidence: the carrier is already a private module of gwz-core (`transport_host/mod.rs:10`); the pieces in
2.3a, 2.3d, 2.3e, 2.3f need carrier-internal types, so (B) means extracting the carrier first. Recommend (A) for 2.3 and a
recorded structural item: extract the carrier and local carrier into a crate at the Phase 2 review (2.6), as a decision, not
parked in plan text. Needed before wave 2.

Q2. **Who owns `--server` and `GWZ_SERVER` parsing, and the call-carrying CLI client rows.** The plan puts `--server` routing in
3.1 but `server stop|status` need `--server` in 2.3k. Options: (A) 2.3k parses `--server`, `--no-server` and `GWZ_SERVER`
and refuses `auto`; 3.1 consumes them and adds `--allow-*`; the C7 rows that need `gwz --server X fetch` move to 3.1 and 2.3l
uses the 2.2c receiver as the client; (B) all selectors wait for 3.1 and `stop`/`status` take only an address argument until
then. Recommend (A). Needed before 2.3k (wave 4).

Q3. **Which path carries the served core's Git in 2.3.** The 1.7 contract row says the core's requests reach the client through the
session port until Phase 3 and makes no `HostHandle`; D-L and the 2.2c handle are the end state. 2.3h therefore joins the driver
`TransportPort` to the carrier binding (seam rows exist) and the client's receiver serves it. Risk: the old driver bootstrap may not
satisfy the receiver's call naming (`request_id` in the lane frame); 2.3h begins with a one-day probe and reports before building
if it does not. Options: (A) join path as planned; (B) skip it and wait for the Phase 3 handle, which moves the server's end-to-end
rows into Phase 3. Recommend (A); the probe is the gate. Needed at the start of wave 3.

Q4. **The address walk's weight.** 2.3e is about 900 lines with rows, mostly per-OS syscall work, and sits off the critical path.
Options: (A) build it now as the plan carries it; (B) ship grammar, the per-user-directory checks 2.1 has, and the Windows checks, and
record the Linux/macOS automount walk as a Phase 6 hardening item. Recommend (A), scheduled last in wave 2 so it cannot delay the
path; (B) is the fallback if its report shows it cannot fit. Needed by the end of wave 2.

Q5. **Limitations the 2.2c reviews recorded for 2.3** (idle polling at 100 ms/5 ms, no terminal for a peer's cancel, `CarrierLane`
serving both roles, ignored `spawn` results): this plan does not implement them; they stay findings with the evidence in
`RemPlan.md:39-41`. Options: (A) leave them findings; (B) add a step. Recommend (A): none blocks R2, R5 or R7. Revisit if 2.3h's
stress row shows a wake or leak. No decision blocks work.

Q6. **Hosted CI and the sandbox rows.** The hosted ubuntu-24.04 runner may restrict unprivileged user namespaces (AppArmor policy), so the user and mount
namespace fixtures can fail there; a containerised CI leg with a default seccomp profile would also be refused by the new rule. No
workflow in gwz-core uses `container:` today (`.github/workflows/*.yml`). Options: (A) the namespace rows are required on dabeest
WSL as root and, on the hosted runner, fail with the named missing capability unless the workflow runs `sysctl` to allow them;
(B) mark them ignored on hosted and rely on WSL evidence. Recommend (A) with the sysctl in `transport-candidate.yml`. Needed
before 2.3a is reviewed.

Q7. **Probe (1) timing.** The split doc requires the Windows `ssh` allow-list entries to be fixed from a probe "before 2.3 lands"
(`GwzSplitCommandChannel.md:202`, §7). This plan schedules the probe inside 2.3j (wave 3). Option: run it now, off the lane, as a
dabeest task so 2.3j starts with the answer. Recommend now: it needs no code and is the one external dependency of a secret-handling
step.
