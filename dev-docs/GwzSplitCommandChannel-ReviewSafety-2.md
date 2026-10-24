# GwzSplitCommandChannel.md revision 2: Safety-axis re-verdict

**Review object:** `dev-docs/GwzSplitCommandChannel.md` revision 2 (368 lines, SHA-256 prefix `bbe74ddd…`) at lane root `2f3b7cb7`, together with its in-place amendments of `dev-docs/GwzCoreSessionDesign.md` (§3, plus the clerical renumbering in §4.2 and §13). The diff reviewed is `f923969d67..2f3b7cb7`.
**Baseline:** lane workspace `/Volumes/projects/limbo/gwz-dev-p0-design`: root `2f3b7cb7`, gwz-core `0ed481688d`, gwz-transport `a27817b9c8`, gwz-cli `cea7bd4cab`, gwz-py `71d2d07249`. Controlling plan: the root workspace's `dev-docs/GwzTransportReleaseConvergencePlan.md` at root `7ce0b749`.
**Date:** 2026-10-25 **Axis:** Safety (what the text permits to go wrong)
**Verdict: GO.** All 11 prior findings are closed (4 P2, 7 P3). Four new findings are open, all P3 and none architectural. P0 0, P1 0, P2 0.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P2-1 (a dry-run push admitted receive-pack) | Plan class: a push-class request type with `meta.dry_run = true` admits `bind` and `check_identity` only, never an `open`; plan-only fields only ever narrow the class (§4.5 L225–237, §5.3 L306). | **Original interleaving re-run.** A `--dry-run push` with a push allowance in configuration falls in the L226 "plan" row. A core-sent `receive_pack_exchange` gets `open_failed(invalid_request)` with `Effect::None` (L306). The dry-run tag push and remote tag delete are in the same row. **Code citations hold at `0ed48168`:** `push_member.rs:111–131` returns its plan before any preflight; `:475` validates the identity before the `dry_run` branch; `handle_tag.rs:118` gates `prepare_push` on `!dry_run`, and `:152–161` validates the identity then gates remote work. `check_identity` crosses under `cli` (`identity.rs:230–233`). Admitting `check_identity` gives the core one bit, for paths the client named only (L162, L310). | Closed |
| P2-2 (the reader's bound and the overrun consequence) | Per-stream state bounded in quantities the sender controls; an overrun faults its binding only; tag 17 carries exactly the kinds gwz-transport charges to its control reserve (§3.3 L93, L96, L102–105; session design §3). | **Interleaving A** (small data frames within credit while the consumer is paused): `data` goes to a byte buffer bounded by `receive_window`, so frame count no longer matters. `window` coalesces, which is safe because `incoming.rs` ignores a lower limit. Every other kind has one slot per stream; gwz-transport's own state machine permits at most one outstanding `flush` (`incoming.rs`: a second `peer_flush` is `Protocol`). **Interleaving B:** `flush`, `flushed`, `end_write`, `close`, `closed`, `opened` and `bind*` are now on tag 17, matching `mux/mod.rs:638–641`. **Fault scope** now agrees with contract §6: L104, and the new session design paragraph after the protocol-error list. A consumer that is not ready holds back only its own binding (L103). | Closed |
| P2-3 (the close report overtook the step-6 replies) | The close report is keyed ordinary and so exempt from priority; the writer drains before step 9; the client reads until the report (L74, L100, L253; session design §3). | **Interleaving re-run.** Under the writer rule, every control frame queued before the report goes first, and ordinary frames keep FIFO order, so every frame queued before the report precedes it. Step 9 waits for the drain. `close()` delivers late replies, including the `operation_result` of a push, before failing what remains (L253). A client that stops reading during the drain is bounded by the 60-second stall rule (L205). | Closed |
| P2-4 (`server start` refused when `SSH_AUTH_SOCK` was set) | Scrub, do not refuse: the background server is launched with the list and `SSH_AUTH_SOCK` removed, and a `--foreground` server removes them before any thread starts; only `server stdio` refuses (§3.8 L178–179, §4.3 L202, §4.9 L264–266). | A default macOS login (launchd's `SSH_AUTH_SOCK`, verified in round 1) now starts the server; §4.9 shows exactly that case. `server stdio` keeps the plan's refusal (plan 2.3), and L178 names its remaining purpose, variables set on the core's side. Matches plan provenance. | Closed |
| P3-1 (the body-blind adapter could not see a frame's target) | `FrameSink::send(frame, lane, key)` with `FrameKey { class, target }`; a core-supplied `FrameRouter` (L99, L102; session design §3; §8 L361). | The adapter compares an opaque target for equality only and still decodes nothing; core computes the key at encoding time and decodes in `route`. The original consequence (a `cancel` overtaking the same stream's `open`, then `routing.rs:87–91` faulting the binding) cannot happen when the key carries `(request_id, session_id, stream_id)`. | Closed |
| P3-2 (`Bound` fields missing from §3.6) | `Bound.schemes`, `policies` and `receive_limits` added as client-configuration metadata (L164). | Present, with direction, class and reason; no log. | Closed |
| P3-3 (deny-list scrub; `SendEnv`/`SetEnv` from the user's configuration) | `ssh` runs with an allow-listed environment and `-o SetEnv=GWZ_NONE=1` (L177, L215); the §7 probe asserts the canary does not reach the remote side (L336). | Round-1 `ssh -G` evidence: a command-line `SetEnv` replaces the configuration's `SetEnv`. With an allow-listed environment, a configured `SendEnv *` can send only allow-listed names. A canary `https_proxy` with user info is not on the allow-list. Residuals are in §3: the `LC_*` wildcard, and the Windows allow-list, which is unprobed (N-P3-3). | Closed |
| P3-4 (no client-side stall bound) | Symmetric 60-second rule; `ssh` keepalive 15 × 3 = 45 seconds (L205, L215, L252). | **Original closure test re-run:** a core stopped mid-fetch while the client has data queued toward it, so the socket buffer fills and the client closes the carrier within 60 seconds and revokes its bindings. Passes. A silent network path is detected by the keepalive, except under a user's ControlMaster (N-P3-2). Residual: an idle client facing a hung core with an empty socket buffer is not detected, which is inherent without an application heartbeat (§3). | Closed |
| P3-5 (Python carrier loss during a push) | `server_session_closed` carries `possible_remote_effect` when a `receive_pack_exchange` had begun (L252). | Matches the CLI's report (L206). | Closed |
| P3-6 (the per-user directory removed under a running server) | macOS uses `confstr(_CS_DARWIN_USER_DIR)`; a 30-second link check shuts down an unreachable server; the Linux logind case is stated (L23, L196, L209). | dirhelper sweeps the temporary directory, which is no longer used. The check compares inodes and shuts down instead of leaving a server holding a lock on an unlinked inode. The rationale's mode claim is wrong (N-P3-1). The shutdown cancels live sessions, which is noted in §3. | Closed |
| P3-7 (admission is not confinement on the same-user forms) | One sentence in §5.1; 6.4 and the R7 README carry it (L288; residual L180). | The boundary is stated per form, citing server design §4's "full authority". | Closed |

## Changed-range analysis

`git diff f923969d67 2f3b7cb7` changes only the two documents and a gwz commit marker (`gwz.conf/markers/01a16ec8-….yaml`). By location:

- **Header and §2 map (L3, L10–16, L39–63).** Bookkeeping; every RemPlan ID is mapped.
- **§1.**
  - The macOS row changes (P3-6), with the new N-P3-1 premise error.
  - The second-client row names the two members of the host context (C-P3-11).
  - Error codes are renumbered 77–86 (the lane owner's change). Its necessity is demonstrated: `gwz.taut.py:840` (75) and `:847` (76) already end the schema, so appending at 75 would redefine codes, against "None is redefined". No wire allocation exists yet: `transport_session_full`, `operation_expired` and the `server_*` names appear in no `.taut.py` or Python source, only as model enums (`model/error_code.rs:138`, `convert.rs:88`) and as an opaque test vector in `byte_stream/tests.rs:109`. So the renumbering breaks no deployed peer.
- **§3.1 and §3.3.** The P2-2, P2-3 and P3-1 changes, the D-N verbatim bullets kept (L98, L101) and the "Reply order" note (L107). The reply-order note is safe: the bridge waits per `call_id`.
- **New interface text: the `FrameSink` key and the `FrameRouter` clauses.** They trace to demonstrated necessity, because the D-N rule needs per-frame metadata that a body-blind adapter cannot read. They are recorded against the crate map (:40) and `session-contract` in §8 L361 and are left to step 2.1's code review. They introduce no wire field, since `FrameKey` is local, and they do not change trust direction: classification at the receiving end is the receiving core's own, so a peer cannot claim control priority at its peer.
- **§3.4 L113, §5.6 L325.** Refusal mapping (C-P3-7): an admission refusal becomes `server_destination_refused`, and any other invalid `Open` is a binding fault. No widening.
- **§3.6, §3.8, §4.3, §4.4, §4.7, §5.1, §5.3, §7.** The Safety dispositions, verified above.
- **§4.1.** C-P3-9 changes. `stop` and `status` now default to the workspace's own server, which on Windows means the pipe recorded in `ws-<key>.lock`. A pipe name planted in that record still has to pass listener verification (same SID and integrity, not AppContainer), and `ServerControl` carries no secret. No new exposure.
- **§4.9.** The prompt walkthrough (C-P3-10) grants one exact fetch tuple on the default port, consistent with §1.
- **§8.** New rows record what the step changes or replaces (GWZDesign, gwz-py, the renumbering, the crate map).
- **Session design.** Header line 9, §3 (lines 93, 112–129), §4.2 (:191) and §13 (:546), plus the changelog.

**Scope.** Every change traces to a RemPlan disposition or to demonstrated necessity (the renumbering). No parked or historical file is edited. One inaccuracy in the scope record is filed as N-P3-4.

**New architectural root cause: none.** The per-stream bound and binding-only fault fit inside D-N's decided outcome and contract §6. One implementation condition applies, recorded in §2: "window coalesces" must mean applying windows in order, not dropping frames ahead of an order kernel.

## 0. Evidence base

- **Tuple.** Checked at the start and the end; every value matched the coordinator's message. The only working-tree entry is the lane's stale untracked plan copy, which is out of scope.
- **Read in full.** `git show 2f3b7cb7:dev-docs/GwzSplitCommandChannel.md`; `git diff f923969d67 2f3b7cb7` for both documents, plus a hunk list; `dev-docs/GwzSplitCommandChannel-RemPlan.md`; `git -C /Volumes/projects/limbo/gwz-dev show 7ce0b749` (plan line 524 now reads "the port must match the allowance's tuple"; the §1 D-L wording is corrected).
- **Reference code (read only).**
  - gwz-core: `protocol/gwz.taut.py:830–847`; `src/workspace_ops/push_member.rs:108–132, :472–478`; `handle_tag.rs:114–163`; `git/gitbackend/transport_support/identity.rs:226–236`.
  - gwz-transport: `src/mux/mod.rs:89–97, :627–650`; `src/stream/incoming.rs`; `src/sequenced.rs:130–222`.
  - The contract, §6 and §6.1.
- **Local probes (read only).**
  - `getconf DARWIN_USER_DIR` gives a per-user `…/0/` directory with mode `drwxr-xr-x` (0755), owned by the user. Round 1 found the temporary directory (`…/T/`) at `drwx------`.
  - Round-1 `ssh -G` results (OpenSSH_10.3p1) stand: a command-line `SetEnv` replaces the configuration's; `SendEnv` from configuration is additive; `ControlMaster` from configuration survives the argument vector.

## Problem, requirement provenance, and scope

- **Requested outcome:** unchanged (plan 2.0 (a), (b) and (c), the 2.4 amendment, the §8 record; D-N's in-place amendment of session design §3).
- **New constraints in revision 2 and their provenance:**
  - The plan class traces to D-H (b), "the service class of its own command".
  - The scrub at start restores plan 2.3's stdio-only refusal.
  - The allow-listed environment and the keepalive trace to the threat model and R5.
  - The 30-second link check traces to the server design §4 invariant "the lock file stays".
  - `FrameSink`/`FrameRouter` are demonstrated necessity for D-N.
  - The renumbering is demonstrated necessity (schema collision).

  None is a preference written as a requirement. None widens what the client admits: the plan class and the allow-list only narrow, the stall rule and keepalive only add bounds, and the binding-only fault narrows the blast radius.
- **Component authorization:**
  - Root `dev-docs/` only.
  - Session design §3 under D-N. Its §4.2 and §13 numbers change as clerical edits driven by the collision (see N-P3-4).
  - Crate map, `session-contract` and `session-channel` changes are recorded for step 2.1, not made.
- **Out-of-scope observation:** none new.

## 1. Findings

### N-P3-1: §1 says the macOS `_CS_DARWIN_USER_DIR` directory is created with mode 0700; it is 0755

- **Location.** `GwzSplitCommandChannel.md:23` ("The `confstr` directory is per user, created by the system with mode 0700").
- **Evidence.** `getconf DARWIN_USER_DIR` gives `/var/folders/<xx>/<hash>/0/` with mode `drwxr-xr-x`, owned by the user. The 0700 claim was true of revision 1's `…/T/` and was carried over unchanged.
- **Violated invariant.** The rationale for the directory choice must be true. Server design §4's checks apply to the directory gwz creates (`gwz-<uid>/`, mode 0700), not to its parent.
- **Consequence.**
  - Safety still holds: the parent is owned by the user and not writable by group or other, so no other user can create or rename `gwz-<uid>`.
  - An implementer who applies the "0700" check to the `confstr` directory itself makes `server start` refuse on every Mac.
  - Other users can list `0/` and so learn that `gwz-<uid>` exists, which is a trivial disclosure.
- **Correction.** State that the system creates the directory owned by the user and not writable by group or other (0755 observed), and that the 0700 check applies to the `gwz-<uid>/` directory the host creates. Optionally require the walk to refuse a parent that is group- or other-writable.
- **Closure test.** A 2.1 macOS row: `server start` succeeds with a `0/` parent at 0755, and refuses when `gwz-<uid>` is anything but 0700.

### N-P3-2: The 45-second `ssh` keepalive does not apply when the user's configuration multiplexes

- **Location.** `GwzSplitCommandChannel.md:205` ("the `ssh` form adds a keepalive below that bound") and `:215` (the argument vector).
- **Evidence.** `ControlMaster`/`ControlPath` from the user's configuration survive the vector (round-1 `ssh -G`). `ServerAliveInterval`/`ServerAliveCountMax` are properties of the connection, run by the master. A multiplexed client session cannot set them. A gwz `ssh` that becomes the master with `ControlPersist` also leaves an agent-less (`-a`) master behind for the user's later sessions.
- **Consequence.** With `ControlMaster auto` in the configuration (common among power users), a silent network path to an `ssh://` core is detected only by the frames-queued stall rule. An idle client waiting on a reply hangs until a second Ctrl-C. This contradicts L205's claim.
- **Correction.** Add `-o ControlMaster=no -o ControlPath=none` to the vector, so gwz's session uses its own connection with its own keepalive and never becomes or rides a shared master.
- **Closure test.** `ssh -G` with a configuration holding `ControlMaster auto` shows `controlmaster false` and `controlpath none` under gwz's vector. A 2.3 row: a dropped path to a multiplexed host is detected within 45 seconds.

### N-P3-3: The Windows-client allow-list is unprobed, while R8 depends on a Windows client

- **Location.** `GwzSplitCommandChannel.md:177` (the Windows additions are only `SystemRoot` and `USERPROFILE`) and `:336` (probe 1 runs "from the Mac to dabeest and to WSL" only).
- **Evidence and inference.** R8 requires "Windows client → Linux core" over `ssh … gwz server stdio` (plan §3). Win32-OpenSSH locates its system configuration and `ssh_known_hosts` under `%ProgramData%\ssh` and runs `ProxyCommand` through the shell. That it reads `ProgramData` (and `ComSpec`/`PATHEXT`) from the environment is inferred and has not been probed. None of these is on the allow-list.
- **Consequence.** The R8 Windows-client leg may fail late, or may ignore an organization's system-wide host-key pins. With `BatchMode` that fails closed, which is a parity and feasibility risk, not an exposure.
- **Correction.** Extend probe 1 to a Windows client (dabeest → Linux or WSL) under the exact allow-list, and fix the Windows entries from that evidence.
- **Closure test.** Probe 1 passes from a Windows client, and the system `ssh_config`/`ssh_known_hosts` are honoured.

### N-P3-4: The scope record says "§3 only", but §4.2 and §13 of the session design are also amended in place

- **Location.** `GwzSplitCommandChannel.md:340` ("amended in place for §3 only"). The session design header (line 9) names the §4.2 renumbering but not §13. The actual edits are at session design :191 (§4.2) and :546 (§13); §8 L356 records only §4.2.
- **Consequence.** The renumbering is authorized by demonstrated necessity, but an auditor who checks component authorization from §8's preamble or the header misses the §13 edit. This is a scope-record defect on an accepted contract.
- **Correction.** Say "§3, plus clerical renumbering in §4.2 and §13" in the §8 preamble, row 356 and the session design header.
- **Closure test.** Every section the diff touches is named in both scope statements.

## 2. Invariant analysis (attacks that failed, with evidence)

1. **Plan class.** `bind` gives the core only a `Bound` (classified in L164) and no network effect. `check_identity` gives one bit about a client-named path (L162). A core-named path is refused (L310). No `open` of any service is admitted (L306).
2. **Close report starved or reordered.** Any control frame queued after the report overtakes it harmlessly. The drain is bounded by the stall rule, and the client's wait by the close bound (L253).
3. **Coalescing under the contract's ordered kernel** (contract §6.1, `sequenced.rs`). If bindings carry sequenced events, "window coalesces" must be implemented as applying each event in order, which is in-memory and never blocks for data within credit, not as dropping events ahead of the kernel. The text ("Core hands this state to the binding's consumer", L103) permits the correct reading. Step 2.1/2.2c's red row (minimum-size frames plus a window per frame) catches a gap-producing implementation. No finding.
4. **Starvation and stuck states after the tag move.** `bind`, `opened`, `flush`, `close` and the rest are now control, but the own-target exception keeps each stream's own data ahead of them. Control output is still driven by the peer's traffic, so a flood starves only the flooder's own operations.
5. **Renumbering against the wire.** No peer or schema has allocated these codes yet. Step 2.2b assigns them at the schema's tail from a single generated schema.
6. **`stop`/`status` default address.** A planted lock record leads only to a listener-verified connection that carries `ServerControl` and no secret.
7. **Start-time scrub.** The served core holds no agent variable. Hooks that needed one fail visibly, and the help says so (L179).
8. **Allow-listed `ssh` environment.** Only allow-listed names can be sent. If `SSH_AUTH_SOCK` is sent by a user's `SendEnv` and accepted, the remote `server stdio` refuses with a diagnosable error (L178).

## 3. Risks and next action

- **Residuals (non-blocking, for 6.4 or later steps):**
  - The `LC_*` wildcard in the allow-list admits user-defined `LC_` variables, which default `SendEnv`/`AcceptEnv` settings forward. Narrowing it to the POSIX locale names would close that.
  - A hung core while the client is idle with an empty socket buffer is not detected without an application heartbeat. A gwz-py shielded cancel waits there; `close()` is bounded.
  - The 30-second link check "shuts down as on stop", which cancels live sessions when the directory is removed (logout, a user's `rm`). Draining open sessions before exit would be gentler.
- **Next action.** File the verdict. N-P3-1 to N-P3-4 can be fixed in place during the lane's acceptance edit or at step 2.1/2.3 without re-review on this axis.
