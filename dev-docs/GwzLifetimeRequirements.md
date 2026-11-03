# gwz lifetime requirements: processes, connections, sessions, operations, resources and state

## 0. Status, scope and how to read

**Status: DRAFT, revision 2, 2026-11-03 (completeness fix pass on revision 1). Written for dual review. It authorizes no
design and no implementation.**
It states what must hold over the whole lifetime of everything gwz creates or depends on. It does not state how.
Section 9 is the only forward-looking part and is explicitly non-normative.

**Why this document.** The operator's requirement for Ctrl-C (D-V, Plan:1036-1041) reads: "Ctrl-C leaves no orphaned
process, stale lock or partial state behind". The amendment renders it as "Ctrl-C must not leave anything behind"
(AM:26-29). Four cancellation designs (A, B accepted; C, D r1 NO-GO) have since met it one mechanism at a time. Between them
they name about 70 residuals, and the designs and their reviews record well over 100 failure modes. Most of those are instances of a few missing
whole-system guarantees. This document replaces "no turds" with numbered guarantees that can be traced and tested, so the
design that follows can remove whole classes of issue instead of patching instances.

**Scope (deliverable boundary).** This is a single requirements document in root `dev-docs/`. It covers gwz-cli, gwz-py
(both the embedded module and its CLI) and the server core in every form: in-process, local server, `gwz server stdio`,
and remote over `ssh`. It also covers the carrier client and connection host, credential helpers, Git CLI children with
their hooks and signers, `ssh` and its children, the gwz-sspi worker, the pager and `forall` commands, the user's
concurrent `git`, and the OS. Reference-only (not changed by this document): every design, contract and plan cited
below, and all code. Release placement (1.1.0, 1.2.0, Phase 7) is out of scope; §11 OQ-19 asks for it.

**Provenance rule.** Every requirement carries one or more sources. A requirement whose only source is review or code
evidence, or only another requirement of this document, is marked **PROP** (proposed). Each PROP needs operator
confirmation and is listed in §11. It is not authority merely because it is written here (standing scope rule).
Requirements state what must hold. Where a source names a mechanism, the mechanism stays in §9 or in the source.

**Source keys** (paths relative to the root at `e63509c2`; line numbers as read in this tree):

| Key | Document |
|---|---|
| Plan | `GwzTransportReleaseConvergencePlan.md`. Goals :37-61; acceptance rows R1-R7 :164-181 and pass rules :278-287; §10 decisions D-A to D-U :938-1029 |
| D-V | Plan:1030-1110. Sub-keys: **OQ2** :1036-1041; **OQ4** :1047-1060; **OD-C1..C6** :1061-1062; resolutions **(1) (2) (3)** :1066-1076; split :1077-1085; decisions **(i) (ii) (iii)** :1086-1093; **O4** :1096-1097; **O5** :1097-1100; **EXIT** (exit status confirmed) :1107-1110 |
| AM | `history/GwzCancellationAmendment-DRAFT.md` (retired, revision 2). Requirement text C1-C6 :36-65; OD-C1..C8 :689-708; residuals :729-756 |
| A, B | `GwzCancelInterruptDesign-DRAFT.md`, `GwzChildLifetimeDesign-DRAFT.md`, accepted 2026-11-03 (A1-A9 A:27-42; BR1-BR8 B:31-40; residuals A:353-372, B:353-372; OD-B1..B4 B:340-343) |
| C, D | `GwzCancelServedDesign-DRAFT.md` and `GwzCancelLeftoversDesign-DRAFT.md`, r1, NO-GO, in lane `gwz-dev-docs-cd` at `a977d241`. They are **evidence, not authority**. Their reviews are CS/CC (C Safety/Consistency) and DS/DC (D) |
| CT | `GwzConnectionApiContract.md` (accepted interface contract; scope table :285-293) |
| SD | `GwzCoreSessionDesign.md` (accepted; ownership O1-O9 :60-89) |
| SCC | `GwzSplitCommandChannel.md` (accepted) |
| SRV | `history/parked/GwzCoreServerDesign.md` (adopted as amended by SCC) |
| P23 | `GwzServerCore-Step2.3-Plan.md` (a plan; its status line says nothing in it is accepted or started, P23:3; operator answers recorded in it are cited by line) |
| IL | `gwz-core/dev-docs/history/reference/GwzTransportIdleLossDesign.md:292-304` (operator decisions 2026-10-07) |
| TR18 | `gwz-core/dev-docs/GwzTransportTR18-OperatorDecisions.md` |
| f9 | the f9 report cited by A:37-38 (`build-targets/winflk-logs/f9-report.md`; W helper drain outlasts the 500 ms grace) |
| H | `GwzHttpsAuthModel-DRAFT.md` r5, §3 (accepted design; not built) |
| TR1.6 | `gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md` (OQ1-OQ7, "all as recommended", as amended by D-V) |
| SR | Operator standing rules: no globals or thread-locals (`gwz-core/AGENTS.md:14-16`); `cfg` only in `cfg_if!` or platform modules (root `AGENTS.md`); Windows-parity inventory (`gwz-core/AGENTS.md:40-45`); no production code under `tests/`; small crates in a DAG (`gwz-core/AGENTS.md:9-13`) |
| MEM | Operator standing rules recorded in the operator's memory files, cited by name: `no-native-route-fallbacks` (:11, :15), `release-scoped-decisions` (:8, :12), `remove-dead-code-now` (:13), `surface-structural-debt` (:15), `small-crates-narrow-apis` (:11, :15) |
| RP | `GwzPhase1-RemPlan.md` (observations; evidence, not authority) |
| SBC | `gwz-core/dev-docs/history/reference/GwzTransportSshBackgroundCloseDesign.md` (operator 2026-10-07, §11 :195-201; revision 3, operator 2026-10-08, :6) |
| ACD | `gwz-core/dev-docs/history/reference/GwzTransportAdaptiveConcurrencyDesign.md:628-646` (operator 2026-10-07) |
| CP | `history/CurrentProgramCheckpoint.md` (archived; operator directions at :231, :268-270, :328) |
| OD12, SD-F8 | Operator decisions recorded in SD: OD12 (the SSH remote server form is in scope, SD:7); session `cancellation` field 8 (operator 2026-09-27, SD:5, :12) |
| P18 | Parity with released gwz 1.0.18 (D-V: "The parity reference is now gwz 1.0.18"). Derived from source; **never measured** (§7 LR-TEST-9) |
| PF | Platform fact. [V] means verified against a local man page or source; (ext) means needing an OS-fact row (LR-TEST-10) |

**Levels.** **MUST** is a release gate once the operator places the requirement in a release. **SHOULD** is a target. A
miss is reported, never silently absorbed (Plan:178-181).

**Verification codes** (used in §5 and §8):

| Code | Method |
|---|---|
| U | Deterministic in-process test through seams: stepped clock, stepped ports, step points. No sleeps |
| P | Deterministic process test. The test binary re-executes itself in each role, using real processes and real signals or console events. Every interleaving is pinned by a handshake over a pipe |
| K | Crash injection: SIGKILL or `TerminateProcess` at each named step point, then the next command's recognition or recovery is asserted |
| F | Fault-injecting port: filesystem (ENOSPC, EIO, Windows sharing violation, power-loss model that drops unsynced writes), network (cut, stall, RST), clock (jump, suspend) |
| X | Census: after the test, no process carrying the test's marker or job, no file outside the expected set, no socket, no lock |
| S | Syntax-aware source check, blind to `cfg` (it inspects disabled platform branches) |
| E | Measured evidence run, archived in the evidence member |
| O | OS-fact row that pins a platform behaviour a requirement depends on |
| R | Review only. Allowed for SHOULD, non-goals and §9 |
| Z | Stochastic stress run (randomized interleavings, PID-reuse pressure, signal storms), archived as evidence. It supplements a U or P row that reaches the same window through an injected seam. It never verifies a MUST alone, and any failure it finds is a defect (LR-TEST-3) |

**Platforms:** M macOS, L Linux, W Windows. A requirement holds on all three unless its row says otherwise.

**Applicability by build and actor.** A MUST binds the candidate build. Until step 3.3, the routes NG-15 lists keep
1.0.18 behaviour, so their runs are parity rows (LR-TEST-9), not gates (LR-TEST-11). Rows that name an actor (§1) bind only
that actor. LR-EXIT binds CLI and the gwz-cli server forms; PYM is bound by LR-EMB instead, and PYC by LR-EMB-9 and NG-6.

**Terms.**

| Term | Meaning here |
|---|---|
| Owner | The one object or process accountable for a resource from its creation to its terminal outcome |
| Scope | An owner together with everything it owns. Scopes nest: process, composition, host or session, operation or call, binding, exchange, child, temporary |
| Terminal | The single final state of a resource. Its **receipt** states the outcome *and* the cleanup status |
| Retained | Transferred at an owner's end to a named longer-lived owner (a retainer, a detached-worker record), with an identity a report names |
| Residue | Anything that remains after a scope ended and that the scope's owner did not hand on: a process, file, lock, socket, record or remote effect |
| Recognizer | A named rule by which a later gwz command identifies a residue, from a durable record or from the residue's own shape |
| Safe point | A point in a local write sequence where stopping leaves no torn file and holds no Git lock file and no in-process lock (A3) |
| Cover | The mechanism that ends gwz's children when gwz itself dies without running code (B's reaper on Unix, job objects on W) |
| Link | A pair of parties where each depends on the other being alive (§5.4 table) |
| Effect | A change outside gwz's memory: a local file, ref, index or working tree; a remote ref or pack; a process started; a socket or record |
| Tier O / C | The guarantee classes of §4 |
| OQ-n | This document's open questions (§11). OQ2 and OQ4, without a hyphen, are TR1.6's questions as re-decided in D-V |
| E-nn, NG-n, B-x, U1-U3 | Failure events (§3), non-goals (§6), named bounds (§5.9), and the enumerated unbounded waits (LR-BND-1) |

## 1. Actors and components

| ID | Actor / component | Created by | Role in lifetime | Notes |
|---|---|---|---|---|
| USR | User, terminal, shell, calling script, CI or service manager | - | Sends Ctrl-C, Ctrl-Z, hang-up, SIGTERM; reads exit status; may kill the whole tree or close a job | bash stops a script only on a SIGINT death (PF ext; A:236-238) |
| CLI | gwz-cli process | USR | Composition root of one command. Owns one `HostOwner` per command (Plan:737-764), the interrupt source (OD-C1), output, and its children | No signal handler exists in code today (P18) |
| PYM | gwz-py native module inside an embedding Python application | the application | One module-level composition object owns one `HostOwner` (Plan:737-764). Clients share it | Installs no handler (OD-C1). The application owns the process |
| PYC | gwz-py CLI (`cli.py`) | USR | Python's own SIGINT delivery only; no handler (OD-C1) | Dies by default on SIGTERM, SIGHUP, W CTRL_BREAK and CTRL_CLOSE (OD-C7 (d); NG-6). LR-EXIT does not bind it (§0) |
| APP | The embedding application's interpreter, asyncio loop and executor threads | the application | Owns the process, its threads and its exit; joins executor threads at exit before atexit runs (F16; platform §2.4) | Its joins can block gwz-py's exit cancel (NG-20) |
| CORE | Serving core: in-process session host; local server (`gwz server start`, background or `--foreground`); `gwz server stdio` as a local child; remote over `ssh` (in scope by OD12, SD:7) | the driver; `server start`; the client (stdio); `sshd` | Owns sessions, calls, operation records, workers, member locks, the detached-worker record, spools and its own children. Never owns a credential (Plan:37-40) | No `src/server/` in this tree (2.3e-2.3l not landed) |
| CAR | Carrier and session channel, client end and core end | the client composition / the core's accept | Frames in transit; liveness of the client-core link (D-D: local socket or pipe, or the `ssh` stdio child) | Carrier loss is terminal; nothing is replayed (D-L; Plan §11) |
| HOST | Connection host: `CleanupDomain`, `HostOwner`/`HostHandle`, endpoint instances, pools, bindings, exchanges, retainer | the client composition | Owns connections and their reuse across operations and repositories (Plan:45-47) | Contract CT, accepted as interface only |
| CHI | Child-process registry and its cover (reaper on Unix, job objects on W) | the composition, before any thread or credential (B) | The real parent of every child gwz starts | Accepted design B; not built |
| HLP | Credential helpers: `git credential fill` and the `git credential-<x>` it runs; native-route libgit2 helpers (`Cred::credential_helper`) | HOST adapters; git2-rs inside one call | Short-lived killable children, bounded by the interaction allowance (TR1.6:79-83) | Native-route helpers sit outside any registry (R-B5) |
| GIT | Git CLI children: `git commit`/`git tag` (+ hooks, signers: gpg-agent/pinentry, `ssh-keygen -Y`), `git rev-list`, `git fetch` fallback, `gwz hook` utilities | gwz-git, gwz-core | Writers of Git state with Git's own lock protocol (PF ext: git `lockfile.c`) | Commit/tag are never killed while gwz lives (decision (ii)) |
| SSH | `ssh` child (served `ssh://` form) and *its* children (ProxyCommand, ProxyJump, askpass) | the client | Carrier transport to a remote `gwz server stdio` | Evidence form this release (SCC:247). Unix: stays in gwz's foreground group with SIGINT and SIGQUIT ignored, because prompts read `/dev/tty`; W: own group (SRV:1218-1219) |
| SSHD | `sshd` and the remote host | the remote's administrator | Runs the remote core; tells it of client loss by closing its stdin | gwz does not configure it (ClientAliveInterval; NG-25) |
| AGT | `ssh-agent` (Unix socket) and Pageant (W) | the user | Holds keys; a connection to it is a credential-bearing resource (LR-OWN-6, R-AGENT) | Pageant loads `user32` in gwz (R9); its confirmation wait counts in the Network phase (TR18 D5 (b)) |
| SSPI | gwz-sspi worker process (W only) | gwz-sspi supervisor | Native authentication; job with kill-on-close | Exists |
| FG | User foreground programs: pager, `forall` member commands | CLI | Own the terminal while they run. gwz never kills them (OD-C6) | |
| UGIT | The user's concurrent `git`, IDEs, other gwz processes on the same workspace | USR | Contend for Git locks and gwz's cross-process locks | |
| RMT | Git servers (SSH, HTTPS) | - | Hold remote effects: pushed refs and packs | Remote effects cannot be rolled back (NG-10) |
| OS | Kernel, tty driver, console host, init/launchd/subreaper, job objects, OOM killer, filesystems | - | Delivers signals and events, reaps orphans, releases advisory locks and handles at death, may end gwz on its own deadline | Windows ends a process ~5 s after CTRL_CLOSE and ~20 s after LOGOFF/SHUTDOWN (PF ext) |

**Lifetime phases** (requirements in §5 cite these):

| Phase | Span | What must already exist |
|---|---|---|
| L0 Inherit | process start until composition | nothing gwz-made. Inherited dispositions, masks, descriptors, jobs and sandbox are observed and kept (LR-EXIT-1, LR-PROC-5) |
| L1 Compose | composition root built | the cover, then the interrupt source (CLI and server forms only; PYM has none, OD-C1), then owners, all before any gwz-made thread, child, gwz-obtained credential or effect (LR-OWN-3). Threads and secrets the application or the inherited environment already holds are outside gwz's ordering (NG-9) |
| L2 Admit | an operation or call is admitted | its record, cancellation token and gate (SD O7) |
| L3 Run | effects happen | a durable intent for every multi-file or Git-locking sequence (LR-REC-3) |
| L4 Stop | Cancel, Force, carrier loss, error | the per-scope cancel and the safe-point map |
| L5 Close | owners close within bounds; remainder retained | the retainer and its successor owner |
| L6 Exit | process exit and exit status | the output claim; the exit-status rule (LR-EXIT-4) |
| L7 After | next command in the workspace | the recognizers and the single recovery path (LR-REC-2) |

## 2. Resources

Columns: **Creator** and **Owner** (current or accepted design). **States**, and **terminal outcomes** (the receipt).
**May outlive its owner** (and who takes over). **Tier C residue** (what a crash can leave, and how it is recognized).

| # | Resource class | Creator / owner | States | Terminal outcomes | May outlive its owner | Tier C residue and recognizer |
|---|---|---|---|---|---|---|
| R-PROC | Processes and process trees: helpers, Git CLI, `ssh`, stdio core, SSPI worker, pager, `forall`, background server, the cover process | spawn site / child registry of the composition (B); FG under the command; background server: nobody (by design) | starting, running, stopped (not exited), exited-unreaped, reaped | exited(status) or killed(by whom); `StatusLost` when a foreign reaper took it (B) | Background server (R-B11, by design). W child until gwz's death (OD-B4 open). FG program (user's). What a successful helper or a hook starts outside its group (NG-1) | Orphan with PPID 1 or a subreaper; recognized by command line and missing parent, not by PPID alone (CS:205) |
| R-CONN | Connections, bindings, exchanges, streams; reuse proofs; idle entries | HOST (`HostOwner` → endpoint instance → pool; operation binding → exchange) (CT:285-293) | opening, idle, leased, closing; binding pending, registered, sealed, retired | idle return (proven clean) or discarded; `ExchangeEnd{disposition, cleanup, failure, effect}`; `OperationEnd{registration, cleanup, ...}` | Idle connections outlive the operation that opened them, owned by the host (CT:45, D1). Pending cleanup goes to the retainer (CT:174-195). A believed concurrency limit outlives the command that learned it: 10 s after the key's last use, in the 1.2.0 session host (ACD:634) | None durable. Sockets close with the process (OS). The remote sees FIN or RST; a remote effect may be unknown (R-REM) |
| R-SESS | Sessions and calls: session context, call records, tokens, gates, workers, event logs, held reads | CORE session host (SD O1-O9); client bridge holds its outstanding calls | Session not opened, open, closing, ended. Call Received, Queued, DirectWaiting, Running, Terminal. Gate Live, Cancelled, Revoked | exactly one reply per call (SD O2); `Completed`, `Cancelled` or `Failed`; close report | A detached worker thread outlives its session, gate revoked (SD:439, :683). A submitted record lasts until release, eviction or session end | None durable. A detached worker's own write can be torn by process exit (SD:687; R-C2) |
| R-OP | Operations (member steps; multi-member commands) | CLI/PYM via HOST admission; CORE via session admission | admitted, registered, running, cancelled, sealed, terminal | per member `Completed`/`Cancelled`/`Failed`/`Partial`, plus effect and cleanup | Completed-result records in gwz-py under a finite policy (CT:279-283) | See R-TREE, R-REM |
| R-CRED | Credentials in memory: helper replies, `Needs` records, auth headers, environment snapshots, NTLM/Negotiate contexts | client only: the operation's `AuthContext` provider (H:182-239); session environment snapshot (SD:306-315) | captured, held, rejected/wiped | wiped at the operation's end or on rejection | Idle authenticated connection, at most 60 s, leasable only by an equal scope (H:234-236) | Memory only; nothing on disk (LR-CRED-6). Swap, dumps and kernel buffers are NG-9 |
| R-LOCK | gwz cross-process locks (workspace mutator, family lock, server lock record), in-process member locks, Git `*.lock` files (gwz-, libgit2- or user-git-made) | the command or handler that takes them | free, held, (Git) present | released | Advisory locks never outlive the process (OS). Member guards held by a detached worker outlive the session (SD) | gwz advisory lock: none (OS releases it; a remaining lock file is not stale). Git `*.lock`: may remain after SIGKILL/crash/power loss; recognized by path and suffix; **reported, never removed** (OD-C3 (a)) |
| R-TEMP | Temporary files: durable-write stagings, refcopy temps, libgit2 `pack_git2_*` / `tmp_object_git2_*`, probe clones in system temp, spools | the writer | staged, published or removed | published (renamed) or removed | Anonymous spools are reclaimed by the OS at death | Named temps remain after a Tier C event: in a workspace or repository, in the system temp directory (probe clones, F57), or in a local-path remote's repository (F43; NG-24). Recognized by shape (D r1 M1/M3/M6 evidence); removed only under proof of ownership and death (LR-CLEAN-5, -13) |
| R-TREE | Refs, index, HEAD, working tree, `.git/shallow`, packs, gwz manifests/locks (`gwz.conf/`, `gwz.lock.yml`), merge records, local-clone rows | operation write sequences | consistent, mid-sequence | complete, or stopped at a safe point (Force), or torn (Tier C) | - | Partial checkout (R1); clone without index (R6); materialize/init without manifest (R7); pack and shallow with old refs (R18). Recognized by intent record or shape (LR-REC-1) |
| R-REM | Remote effects: pushed refs and packs, remote tag deletes | push / tag push exchanges | not sent, possibly sent, received | per member and per ref: `none`, `unknown` or `received` (R5; D r1 M4 evidence) | Always outlives gwz; cannot be rolled back | Unknown after a crash. The next fetch or status shows the truth; gwz never replays (CT:660-666) |
| R-SRV | Server socket/pipe, lock file and record (`ws-<key>`), log, background copy | `gwz server start` / `--foreground` | binding, serving, stopping, exited | address released last after the detached wait (SRV:586-597) | Lock file stays (not stale) (SRV:318). Background server by design | Socket file and record after a crash. Recognized by the stale-socket rule: owned socket, connect refused, record names it (SRV:307-317), and the object removed is the one proven dead (LR-LIVE-8) |
| R-CLONE | Partial clones and lanes: new member directories, `creating` lane rows, family imports, probe clones (`read_remote_file`) | clone, materialize, init, `local clone` | creating, ready, disposing | ready, removed, or stopped (reported) | - | `creating`/`InterruptedDisposal` rows are already recognized (family model). An interrupted plain clone is not; message rules in LR-REC-8 |
| R-LOG | Logs and spools: diff/log spools, event logs, server log | session; server | open, sealed, released | released at session end; server log rotated at 10 MiB (SRV:608-616) | Server log persists by design | Spool files when not anonymous (LR-CLEAN-9) |
| R-THR | Threads, tasks, timers, descriptors, handles, kernel objects (spawn-gate semaphore), runtimes | owners | live | joined, or detached and accounted | Supervisor thread finishing retained jobs; reaper thread of a dropped SSH adapter (not joined) | Released by the OS at death |
| R-AGENT | Agent connections (`ssh-agent` socket; Pageant) | the client's operation (LR-OWN-6) | opening, open, closed | closed at its operation's end | no | None durable; the OS closes it at death |
| R-GATE | W spawn-gate hold (`Local\gwz-spawn-gate-<pid>`, D-E) | the process's hosts; held across each spawn, across the SSPI launch window (TR18 D31), and for a native-route helper's whole call (D25, code §2) | free, held | released | no | Released by the OS at death. The native-route hold has no bound and blocks every other spawn (NG-15) |
| R-TTY | Borrowed user state: terminal modes and foreground group, console attachment, process-wide dispositions and masks | the user / OS; gwz borrows them | borrowed, modified, restored | restored | - | Echo left off after `ssh` is killed at a prompt (CS S-P3-1); a terminal left to an orphaned pager (A R14) |

## 3. The failure model

Each event lists the components it hits and the **required** outcome. "K", "W", "F" and "S" are the child classes of
LR-PROC-1. The tier column follows §4.

| Event | Hits | Required outcome | Tier | Requirements |
|---|---|---|---|---|
| E01 Ctrl-C x1 / SIGINT once (terminal group or the process) | CLI, PYC | Cancel: network work fails `Cancelled`; K children ended; nothing new starts; local writes run on to their end or next safe point; exit within B-CLEAN or go to Force at the bound; exit 130 and, on Unix, re-raise SIGINT | O-clean | LR-CAN-1..7, LR-EXIT-2, -4 |
| | K / W / F children | K: ended by gwz (in their own group or job, so the terminal's interrupt does not race gwz's ending; Unix `ssh` instead shares gwz's group and ignores the interrupt, LR-PROC-9). W: not interrupted, waited for. F: receive the terminal's own interrupt (P18); gwz waits | | LR-PROC-1, -2, -9, LR-CAN-10, -11 |
| | CORE (served) | `operation.cancel` for every outstanding call, ahead of data, before any close the client makes; core cancels; orderly session close | | LR-CAN-2, -8 |
| | R-TREE, R-REM | no torn file, no Git lock, no gwz temp. Each member is complete or untouched. Remote effect reported per member | | LR-OBS-1 |
| E02 Ctrl-C x2, or the Cancel bound expiring | CLI | Force: K children and network end at once; unbounded wait for local writes to a safe point and for W children, each wait named on stderr; exit 130 | O-forced | LR-EXIT-3 |
| | CORE | Client closes the carrier (code Cancelled). The core treats it as carrier loss: cancel every call including local phases; detach at `close_wait`; never tear a write | | LR-CAN-9, LR-EXIT-9 |
| | R-TREE | a multi-file sequence may stop at a file-granular safe point. That state is recorded and handled by the recovery path | | LR-REC-5 |
| E03 Ctrl-C x3 (after Force began) | CLI | OS default disposition: can tear a write (O5, the user's escape) | C | NG-5 |
| E04 SIGTERM to gwz alone | CLI, CORE | as E01 (interrupt path); exit 130 without re-raise. The sender does not signal the children | O | LR-EXIT-1, -4 |
| E05 SIGTERM to gwz's group (`timeout`, `kill -TERM -pgid`) | CLI, children in its group | As E04. A child sharing the group dies by its own disposition: `ssh` dies at once (R-C4), so the session ends without cancel-and-wait; the core cancels as carrier loss; members report `server_session_closed` and `Cancelled`. Whether this meets OQ2 is the operator's (C §9 routes R-C4 there; OQ-24) | O (pending OQ-24) | LR-CAN-9, LR-PROC-9 |
| E06 SIGHUP (hang-up, terminal or window closed) | CLI, FG | As E04. Writes to the dead terminal fail `EIO` and must not panic or stall the exit (E26) | O | LR-EXIT-6 |
| E07 SIGQUIT (Ctrl-\) | CLI | OS default (core dump) | C | NG-5 (OD-C7 (b)) |
| E08 Ctrl-Z / SIGTSTP / SIGSTOP | CLI, foreground-group children; stdio core in its own session | Stop is not death: no party treats a stopped peer as dead before the stall bound. A stdio core with frames queued past B-STALL closes the session as carrier loss and cancels; it does not tear (CS S-P3-2) | O | LR-LIVE-5, LR-LIVE-3; OQ-6 |
| E09 SIGKILL of gwz alone (kill -9, OOM killer, Task Manager) | CLI/PYM | no gwz code runs | C | §4 |
| | K, S children | ended by the cover within B-COVER (K killed; S ends at input EOF after reaching its own safe point) | | LR-PROC-2 |
| | W children | per OD-B4 (open): killed, signalled, or left to finish; on W also the job's close action | | OQ-2, OQ-27 |
| | F children | outlive gwz and keep the terminal until they end | | NG-22 |
| | CORE (peer) | detects client death by EOF and cancels everything that session owns | | LR-LIVE-1, LR-CAN-9 |
| | R-TREE, R-LOCK, R-TEMP | a torn file, a Git `*.lock` or temps may remain; the next command recognizes each and recovers or reports it | | LR-REC-1, -2 |
| E10 SIGKILL of the whole tree / job close (CI kill-on-close job, `killall -9`) | all local processes | Everything dies at once, W children mid-write included. Residue as E09; Git `*.lock` reported, never removed | C | LR-REC-1, NG-2 |
| E11 Panic (unwinding) | worker, reader or main thread | Caught at the operation, thread or FFI boundary: `Failed(internal_error)`, cleanup runs, session continues; no unwind crosses Python or C (CT:255-259). A panic outside those boundaries runs the exit sequence | O | LR-EXIT-7 |
| E12 Crash: abort, double panic, SIGSEGV, stack overflow, W unhandled exception | process | as E09 | C | §4 |
| E13 Power loss, kernel panic | everything | unsynced data lost; every durable record is consistent with the effects it covers; next command recognizes residue | C | LR-REC-3; NG-3 |
| E14 W CTRL_BREAK | CLI, console children | interrupt path, counted like E01/E02. A stdio child in its own group receives CTRL_BREAK itself (R-C4) and acts by LR-EXIT-9; OQ-24 | O | LR-EXIT-1, -9 |
| E15 W CTRL_CLOSE | CLI | Force at once (no Cancel stage). If writes reach a safe point before the OS deadline (~5 s): O-forced; otherwise C | O-forced → C | LR-EXIT-5; NG-2 |
| E16 W CTRL_LOGOFF / CTRL_SHUTDOWN | CLI | As E15 with ~20 s. A process that loaded `user32` (the candidate build does, for Pageant) gets no event and is ended by the OS: C (R9; OQ-26) | O-forced → C | LR-EXIT-5; NG-2 |
| E17 Network loss to a Git server | HOST exchange | the exchange fails with a typed network or stall cause within its bounds; effect reported (`None` or `Possible`); connection discarded; no request byte resent; other operations unaffected. An idle connection lost is discarded before reuse | O | LR-CAN-6, LR-LIVE-7, LR-BND-4 |
| E17b Network loss on the `ssh` carrier | CLI, remote CORE | `ssh` keepalive (15 s x 3) ends `ssh` and the client sees carrier loss. Members report `server_session_closed` and possible remote effect. The remote core sees EOF and cancels when `sshd` sees the loss, which gwz does not control (NG-25) | O (client) | LR-CAN-9, LR-LIVE-1 |
| E18 Peer death | client ← core dies | `server_unavailable` before `SessionOpened`, `server_session_closed` after; never retried; possible effect named; the next `server start` recovers the address by proof | O (client) / C (core) | LR-LIVE-1, LR-CLEAN-8 |
| | core ← client dies | cancel every call of that session including local phases; detach at `close_wait`; no write torn | O (core) | LR-CAN-9 |
| | gwz ← K child dies unexpectedly | the operation fails with a typed cause; no signal reaches a reused ID | O | LR-PROC-3, -4 |
| E19 Peer hang | core alive, not reading, frames queued | `Stalled` at B-STALL becomes carrier loss on both sides | O | LR-LIVE-3 |
| | core alive and idle, not answering | not detected (no keepalive, OD-C5 (a)). The user's interrupt is bounded by B-CLEAN, then Force | O | NG-4 |
| | server silent in handshake | `server_unavailable` at B-HS | O | LR-BND-3 |
| | Git server stalls | setup or I/O stall bound, typed | O | LR-BND-4 |
| E20 Disk full, EIO, W sharing violation during a write | writer | The write fails; its temp is removed; no partial file is published; Git locks are released by the error path; the operation `Failed` with its cause; a stopped multi-file sequence is recorded | O | LR-REC-1, -6 |
| E21 Hung helper / hook / signer / W child | HLP | killed at B-INTER, or on cancel; code `credential_helper_timeout` (75) | O | LR-PROC-8 |
| | GIT W (commit/tag, hooks, signers) | waited for without bound (decision (ii)); the wait is named; Cancel and Force do not kill it; a terminal-reading hook in its own group stops on SIGTTIN (R-B10) and is reported. User escalation (E03, E09) moves to C | O (liveness by user) | LR-CAN-11, LR-OBS-2, NG-14 |
| | SSPI | absolute deadline, then job terminate | O | LR-PROC-11 |
| E22 Embedding application forks without exec | PYM in the child copy | every inherited gwz handle is inert in the copy: refuses, does nothing on drop. The parent's cover is delayed while the copy lives (documented constraint) | O | LR-OWN-7, NG-7 |
| E23 Embedding application exits normally (interpreter exit, `sys.exit` with daemon threads) | PYM | at exit: cancel own operations through their cancellers, close owners within B-CLEAN, wait W children, let local writes reach a safe point. Except while APP's own exit joins block (NG-20) | O (except NG-20) | LR-EMB-3 |
| E24 Embedding application `exec`, `os._exit`, foreign `exit()` | PYM | no gwz code runs | C (pending OQ-20) | LR-EMB-5 |
| E25 OOM | gwz allocation | allocation failure aborts: C. OOM killer: as E09. Cover fork `ENOMEM`: spawns refused before effect, typed (R-B16) | C / O | LR-PROC-10 |
| E26 Output consumer gone or stalled (EPIPE, EIO, `\| head`, stalled pipe) | CLI output | no panic; no unbounded block once an interrupt arrived; command ends `ConsumerGone`; exit sequence runs | O | LR-EXIT-6 |
| E27 Cover dies alone (reaper killed) | CHI | detected and reported (`child-process reaper unavailable`); kills nothing; new K spawns refused; gwz still ends its children while it lives. A later SIGKILL of gwz orphans them (R-B2) | O | LR-LIVE-6 |
| E27b Cover stopped (SIGSTOP of the reaper) | CHI | not a death, so not detected. Once its lifeline fills, spawns block; a blocked spawn holds the creation lock, so `close()`, `kill_killable()` and Force's child step wait without bound until the reaper continues (F82, R-B2) | O → liveness lost | NG-17; OQ-23 |
| E28 System suspend/resume, wall-clock jump | HOST, timers | deadlines run on a monotonic clock; idle connections past their bound are expired before any lease after resume | O | LR-BND-7 |
| E29 User's git or another gwz holds a lock gwz needs | gwz writer | typed busy refusal or a bounded, announced wait; never removal | O | LR-CONC-1 |
| E30 Abnormal inherited state: ignored signals (`nohup`), SIGCHLD `SIG_IGN`, fds 0-2 closed, inherited W job, sandbox | process start | ignores kept; status loss reported as `StatusLost` (never misread as success); liveness descriptors never land on 0-2; nested jobs; sandboxed callers refused by the server | O | LR-EXIT-1, LR-PROC-4, -5, LR-LIVE-4 |
| E31 Host-internal death: the progress driver dies or is dropped unstopped; the retainer thread stops; an endpoint worker or driver dies | HOST | Driver: the host seals with `HostFault` and never fabricates `Complete` (CT:625, :747; code `gc/pump.rs:139-152`). Retainer stopped: retain is refused (code `connection_host.rs:336-338`) and the refused work is reported pending by identity. Endpoint worker or driver: that instance faults (CT:624); other instances are unaffected | O | LR-OWN-14, LR-OWN-4 |
| E32 A worker or close thread cannot be created | CORE, HOST | that call or close ends `Failed(internal_error)` (SD:238); its resources take the LR-OWN-4 path | O | LR-OWN-14 |
| E33 Protocol fault, registry overrun, `SendError::Full` on the carrier | CAR, CORE | the session or carrier ends as carrier loss of that class (SD:105-111; code `inbound.rs:48-79`, `service.rs:784-786`) | O | LR-OWN-14, LR-CAN-8, -9 |
| E34 Server address integrity check finds its socket or lock file unlinked or replaced | CORE (server) | stops accepting, logs, drains within the close bound, shuts down (SCC:241) | O | LR-EXIT-11 |
| E35 Server log cannot be rotated | CORE (server) | shuts down at its next idle point (SRV:608-616) | O | LR-EXIT-11 |

## 4. Guarantee tiers

**Tier O (orderly).** gwz's own code runs the stop. It has two exits:

- **O-clean.** The work completes, or Cancel completes within its bound. **Nothing is left behind** and **every effect is reported**.
- **O-forced.** Force, by a second interrupt, the bound, or a W console event. No process, lock, torn file or gwz temporary is left. Partial state across several files within a member is allowed only at a safe point, and only as **recorded** partial state that the Tier C recovery path handles (resolution (1); decision (i); A R1-R3, R18).

**Tier C (crash class).** No gwz code runs at the moment of death. **Everything left behind is either durably recorded before the
effect began or recognizable by its shape. The next gwz command in the workspace recovers what it can prove is its own and
reports the rest with an executable remedy** (resolution (1): "each such residue gets a named recognizer in the next command").

**Liveness exception (applies to both O exits).** Tier O guarantees safety, not progress. gwz waits without a bound only in
U1-U3 (LR-BND-1). Each such wait is announced. The user's escalation (E03, E09) ends it and moves the event to Tier C (O5).

| Property | O-clean | O-forced | C |
|---|---|---|---|
| gwz process exit | within B-CLEAN. A U-wait that outlasts B-CLEAN moves the ending to Force (LR-EXIT-2) | after the announced U1-U3 waits | no code runs |
| K and S children | ended and reaped by gwz | ended at once, reaped | K: ended by the cover within B-COVER; S: ends at its input EOF after its safe point (LR-EXIT-9) |
| W children (commit/tag, hooks, signers) | finished | finished | OD-B4 (open, OQ-2); W job close action (OQ-27) |
| FG programs | never killed; waited | never killed; waited | outlive gwz (NG-22) |
| gwz advisory locks | released | released | released by the OS |
| Git `*.lock` made by gwz or libgit2 | none | none (no safe point inside one) | may remain: reported, never removed |
| gwz temporaries | none | none | may remain: removed by the next command under proof |
| Torn single file | never | never | possible for the one write in progress; recognized |
| Multi-file partial state within a member | none | only at a safe point, recorded | possible: recorded or recognized by shape |
| Partial across members | reported `Partial` outcome (not residue) | reported | recognized via the intent record |
| Remote effect | reported per member/ref | reported | unknown until next fetch/status |
| Report | complete | complete, marked forced | none from this process; residue report from the next command |
| Exit status (CLI and server forms) | 0/1/2 when the work completed; 130 after an interrupt, re-raising SIGINT on Unix after SIGINT | 130, re-raising SIGINT on Unix after SIGINT (LR-EXIT-4) | the OS's (signal or crash) |

**Event assignment.** Tier O-clean: E01, E04, E05 (pending OQ-24), E06, E14 (first), E11, E17, E17b (client side), E18
(surviving side), E19, E20, E21 (helper, SSPI; W child that finishes within B-CLEAN), E22, E23 (except NG-20), E25 (cover
fork `ENOMEM`: spawn refused before effect), E26, E27, E28, E29, E30, E31-E35. Tier O-forced: E02, E14 (second), E15 and
E16 while within the OS deadline, E21 W child still running at B-CLEAN. Tier C: E03, E07, E09, E10, E12, E13, E24 (pending
OQ-20), E25 (abort, OOM killer), E15 and E16 past the OS deadline or under `user32`, E18 (the dead side), E21 W after user
escalation. E08 is not a termination (OQ-6). E27b loses liveness (OQ-23).

## 5. Requirements by area

Every row is one requirement. "Verify" names the codes of §0 and the essence of the test. §8 gives the platform matrix.

### 5.1 Ownership and scoping (LR-OWN)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-OWN-1 | MUST | Every resource of §2 has exactly one owner at every instant from creation to its terminal. Ownership moves only by explicit transfer, never by dropping or forgetting a handle. | A resource without an owner is the root of every orphan, stale lock and silent leak | CT:285-293; SD O1-O9 (:70-89); OQ2 "cleans up" | U: for each resource class, drop the owner in every state and assert a terminal receipt or a transfer; X |
| LR-OWN-2 | MUST | Owners form one tree per process, rooted in the composition root (the command for CLI, the module composition object for PYM, the server process for CORE). No resource is reachable through a global, a thread-local or an ambient lookup. The exceptions are a closed allow-list, each entry naming its decision: (a) the interrupt module's OS-imposed handler state (OD-C1); (b) the W spawn gate's per-process named kernel object, found by name (D-E); (c) PYM's one module-level composition object, which is PYM's root (Plan:737-764). Recorded debt (gwz-transport `static NEXT_POOL`, allow-listed, RP:50) is surfaced to the operator, not accepted here (OQ-22). | Globals hide ownership and outlive scopes | SR (no globals, thread-locals included); OD-C1; D-E; Plan:737-764; CT:153-158; A9; BR8; MEM `surface-structural-debt` | S (process-globals check across all members; it fails on any entry outside the allow-list) |
| LR-OWN-3 | MUST | **Guarantee before resource.** The mechanism that ends a resource exists before the resource does: the cover before the first child; in CLI and the server forms, the interrupt source before the first gwz-made thread that can block (PYM has none, OD-C1); the cover process created before gwz obtains any credential; the record, token and gate before admission; a durable intent before a multi-file effect. Threads and secrets that exist before gwz composes (an embedding application's, the inherited environment's) are outside this rule (NG-9). | Prevents the "created before it could be cleaned" window class (fork-before-setpgid, interrupt before host, reaper holding secrets) | SD O7 (:84-87); A:95-102 (install order); B:103-104 (registry before threads and credentials); resolution (2) | U, P: step point at each creation edge; cancel or kill there; assert no escape; S for order |
| LR-OWN-4 | MUST | At an owner's terminal each resource it owns is (a) released and confirmed, (b) transferred to a named owner that outlives it (retainer, detached-worker record), or (c) reported pending, by identity. Nothing is transferred to nobody. A retained record is never dropped because an executor or retainer refused it. | Bounded close must not mean silent loss | CT:174-195, :744-747; R5 (Plan:170) "cleanup reports name what the client still owns" | U: refuse executor and retainer together and assert a pending receipt (today dropped: RP:51) |
| LR-OWN-5 | MUST | The cancellation scope equals the ownership scope. Cancelling a scope reaches everything it owns and nothing outside it. Shared physical resources (pooled connections) belong to the host, not to an operation. A cancelled operation's clean idle connection stays reusable. | Cancel must be complete, and also isolated | R3 (Plan:168); CT:619-628; SCC:160 | U, P: two overlapping operations; cancel A; B completes; C reuses the idle connection |
| LR-OWN-6 | MUST | Credential-bearing resources (agent connections, helper processes, authenticated connections, `AuthContext`, environment snapshots with secrets) are created and owned only by the client composition. No core form owns one at any time. | The core is less trusted and credential-free | Plan:37-40, :55-61; R7 (Plan:172); H:229-230 | U (core `connect()` count 0); S (no credential type reachable from core crates) |
| LR-OWN-7 | MUST | In a process created by fork without exec, every inherited gwz handle is inert. Its methods refuse (`ForkedProcess`/`ForkedHost`); its drop does nothing; it never signals, registers, unregisters, closes or joins on the parent's behalf. | An embedding application may fork; a copy acting for the parent kills or corrupts the parent's state | CT:749-753; B:130-132 (fork check); resolution (2) | P (M, L): fork in-process, use and drop every handle type in the child; X on the parent's children |
| LR-OWN-8 | MUST | Only the closed list below may outlive the scope that created it. Each entry has a named owner, a bound or terminal condition, and a status surface: idle connection (host; B-IDLE/B-AGE); believed concurrency limit (1.2.0 session host; B-LIMITMEM after last use); retained cleanup (retainer; until confirmed or process exit); detached worker (host context record; until its thread ends; blocks W/push on its workspace); SSH adapter reaper thread (its adapter's connection; ends when that socket closes or at process exit; not joined); background server (user; `gwz server stop`) and its server log (server; rotated at 10 MiB, persists by design); completed-result record (PYM, finite policy); W child at gwz's death (OD-B4); F program at gwz's death (NG-22); unlocked gwz lock files (workspace; never stale, block nothing, LR-CONC-5); `creating` lane rows and open merge records (their contracts' recovery, LR-REC-12); Tier C records (next command). | Makes "may outlive" an explicit, reviewable list instead of an accident | SD:439, :683; CT:182-183, :279-283; R-B11; SCC:101; SRV:318, :608-616; ACD:634; OD-C6 | U per entry; S: every detach/leak-style API is in an allow-list naming its entry |
| LR-OWN-9 | MUST | One workspace identity function keys every per-workspace lock, record, detached-worker entry and server address. Two paths get one key exactly when the filesystem resolves them to the same directory: case-variant paths share a key only where that filesystem or directory is case-insensitive. | Two identities for one workspace let a detached worker and a new writer meet (display string vs `canonicalize`); folding case on a case-sensitive volume would merge two workspaces | P23:152 (2.3f); gwz-core `session_host/served.rs:269-296` vs `operation/memberlockmanager.rs:106-114`; **PROP** | U: symlinked and relative paths map to one key; case-variant paths map to one key on a case-insensitive volume and to two on a case-sensitive one (L default, case-sensitive APFS, W per-directory case sensitivity); S: no second keying site |
| LR-OWN-10 | MUST | At most one server serves a workspace. Its address (socket/pipe and record) is released last, after its detached workers end or Force ends the wait. A second server can therefore never run beside the first one's detached writers. | Prevents two writers of one workspace across processes | SRV:586-597; SCC:25, :229 | P: stop with a detached writer and start a second server; assert refusal until release |
| LR-OWN-11 | MUST | **Connections outlive operations, not owners.** Pooled connections belong to the host for its whole life, across repositories and phases. After a command's first exchange to a host, its later exchanges to that host create no connection, except replacements of idle-closed ones counted as R6 counts them. | Goal 3: "SSH setup is paid once per connection, not once per repository" | Plan:45-47 (goal 3); R1 (Plan:166, :280); D-A | E (R1 row), U (creation counter, LR-OBS-9) |
| LR-OWN-12 | SHOULD | No lifetime rule assumes that client and core share a process, or that a client is short-lived. Each rule is stated per scope, so the persistent client and the networked core (Phase 7) are not designed out. | Goal 2 | Plan:41-44 (goal 2); D-P (Phase 7 forms recorded); MEM `release-scoped-decisions` | R |
| LR-OWN-13 | MUST | **One architecture.** At step 3.3, the per-operation runtimes and the hostless routes of NG-15 are removed, not kept beside the owner. Code that a later release form needs (persistent client, served test support) is kept. | Goal 5: no further work on the old architecture | Plan:50-51, :765-767 (goal 5); MEM `remove-dead-code-now`, `release-scoped-decisions` | S (after 3.3: no per-operation runtime entry, no hostless route) |
| LR-OWN-14 | MUST | **Internal death is terminal and truthful.** When a component an owner depends on dies or cannot be created (driver, retainer, endpoint worker, close or worker thread), or its channel faults (protocol fault, overrun, full queue), the smallest scope that depended on it fails with a typed cause (`HostFault`, `Failed(internal_error)`, the protocol class). It never reports `Complete` or success for work it cannot confirm, and its resources take the LR-OWN-4 path. | Internal deaths are the unplanned half of the failure model (E31-E33) | CT:624-625, :747; SD:105-111, :238 | U: inject each death and creation failure; assert the typed terminal and the LR-OWN-4 receipt |

### 5.2 Cancellation, local and remote, one semantics (LR-CAN)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-CAN-1 | MUST | **One semantics.** Every cancel source classifies each operation's outcome the same way, in-process and in every served form. Sources: interrupt, gwz-py cancel, `Client.close()`, owner close, `operation.cancel`, `session.close`, carrier loss. Classification: `Completed` if the work had already succeeded, otherwise `Cancelled`, otherwise `Failed` with its cause. `Cancelled` stands even while cleanup is still pending. | One mental model; no form-specific surprises | OQ2 "end to end"; SD:251-254; SD-F8; A6; BR7; D-L | U: the same table-driven case set against in-process, local socket, stdio and a carrier fake with loss |
| LR-CAN-2 | MUST | **Reach.** A cancel reaches every party of the operation: the client; every child process started for it; the serving core's in-flight call; the core's own children and bindings for that call. | The operator's requirement, verbatim scope | OQ2 ("the client, every helper process it started, and a serving core's in-flight call") | P: cancel during each phase (lookup, helper prompt, transfer, served local phase); X |
| LR-CAN-3 | MUST | **Latch.** Cancel is idempotent, monotonic and never undone. A cancel that arrives before its target exists is recorded and refuses the target at creation: before the host is built, before registration, before admission, or before a call is enumerated. | Racing cancels are the most common lost-cancel class | CT:264; A:131-137 (slot `Interrupted`); A8; CS S-P3-3 (C r1 latch gap) | U: cancel at each creation step point; assert refusal `Cancelled` with no effect |
| LR-CAN-4 | MUST | An operation cancelled before its first effect has no effect and reports `Effect::None` (`NeverRegistered(Cancelled)` where it never registered). | Cancel before effect must be free | CT:224-250; D-L | U: step points before each first effect |
| LR-CAN-5 | MUST | Cancel ends network work and K children promptly. A local write sequence stops only at a safe point. No safe point lies inside a held Git lock file or an in-process lock. A parked thread never writes again. | "Never mid-write" without waiting on the network | decision (i); A3; resolution (1); A:157-176 (G1-G4) | U (gate step points); K: kill at each safe point and assert no `*.lock`/torn file |
| LR-CAN-6 | MUST | **Isolation.** Cancelling one operation changes no other operation's outcome, no other session, and no idle connection that was not leased by it. | R3 | R3 (Plan:168, :282); CT:619-628 | U, P: see LR-OWN-5 |
| LR-CAN-7 | MUST | **Cause precedence.** A cancel is recorded before any failure it causes, so the reported cause is `Cancelled`, not the network failure it triggered. A timeout's cause stays the timeout. Cleanup that outlasts its grace never changes the outcome. | Truthful causes | A6; BR7; C:96-98 (cancel before binding failures); f9 report (500 ms grace exceeded on loaded W) | U: cleanup slower than grace on a stepped clock; assert `Cancelled` or `Interaction`, never `Io` |
| LR-CAN-8 | MUST | Across the carrier, cancel and close frames never wait behind data in either direction. A reader never blocks on a consumer. An overrun is a protocol error of its class. | A cancel stuck behind a full data lane is not a cancel | D-N (Plan:979-990) | U: saturate data lanes, send cancel, assert delivery order |
| LR-CAN-9 | MUST | **Carrier loss is cancellation of everything the session owns, on both sides.** Client: each call ends `server_session_closed`, `NeverRegistered(CarrierLost)` before `bound`, `possible remote effect` once receive-pack began; bindings revoked; connections discarded, not pooled; the client's `HostOwner` and idle connections survive. Core: every call is cancelled including local phases, detached at `close_wait`, its outcome logged. Nothing is replayed. | Death of either side must end the other's work truthfully | D-L; SCC:143, :160, :238; Plan §11; R5 | P: kill client, kill core, cut carrier at each phase; X on both sides |
| LR-CAN-10 | MUST | gwz never cancels, signals or kills a user foreground program (pager, `forall` command). On interrupt it stops feeding it, starts no further one, and waits for the current one. | The user's program on the user's terminal | OD-C6; A7 | P (pty): interrupt during pager and during a `forall` member; assert no signal from gwz |
| LR-CAN-11 | MUST | A Git CLI write child (`git commit`, `git tag`, its hooks and signers) is never killed while gwz lives. Cancel, Force and close wait for it. | Killing mid-commit leaves `index.lock` and partial hooks | decision (ii); A4; BR5; OD-B1 (a) | P: Cancel and Force during a slow hook; assert hook completion and no `*.lock` |
| LR-CAN-12 | SHOULD | The time from interrupt (or `cancel()`) to all network work ended and all K children signalled stays within a bound fixed from a measured baseline, per form. | R3 asks for a cancel-to-effect bound | R3 (Plan:282) "fixed from baseline" | E, then a U/P regression row at the fixed bound |

### 5.3 Termination and exit status (LR-EXIT)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-EXIT-1 | MUST | **Scope: CLI (every command, subject to OQ-13) and the local server forms; a stdio core by LR-EXIT-9; PYM and PYC are excluded (LR-EMB-1, NG-6).** The interrupt path is taken on SIGINT, SIGTERM and SIGHUP (Unix) and on CTRL_C, CTRL_BREAK and CTRL_CLOSE (W). CTRL_LOGOFF and CTRL_SHUTDOWN take it in a process that receives them; a process that loaded `user32` receives neither (NG-2; OQ-26). SIGQUIT keeps its OS default. A signal or event ignored at start stays ignored, so `nohup`, `trap '' INT` and inherited CTRL_C-disabled states keep working. | One entry for every orderly stop request | O4 (a); OD-C7 (b); AM C1 :38-42; PF execve(2) [V]; platform §10 (`user32`) | P (pty and child console): each signal or event, inherited ignore preserved. W LOGOFF/SHUTDOWN: U calls the registered handler routine with each event code; O records whether the build loads `user32` |
| LR-EXIT-2 | MUST | **Staging.** The first interrupt is Cancel, which waits at most B-CLEAN. A second interrupt, or the bound passing, is Force. A further interrupt after Force began takes the OS default. | The operator's staged design | OQ2; decision (i); O5; A2, A3 | P: one, two and three interrupts at each phase |
| LR-EXIT-3 | MUST | **Force** ends K children and network work at once. It then waits without a time bound for each local write in progress to reach a safe point or finish, and for W children. gwz's own exit is never mid-write. Each wait is named on stderr while it lasts. | "Force never exits mid-write" | decision (i), (ii); A3; A:205-213 (Force sequence) | P, K: Force at every gate state; assert ordering and no torn file |
| LR-EXIT-4 | MUST | **Exit status.** After an interrupt-path ending, gwz exits 130 in every mode, including on W (not `STATUS_CONTROL_C_EXIT`). On Unix after SIGINT, gwz cleans up, restores the default disposition and re-raises SIGINT, so a shell shows 130 and a calling script stops. SIGTERM, SIGHUP and W console events end with a normal exit 130. Scope as LR-EXIT-1; PYC's target is LR-EMB-9. | Shell cooperation; operator-confirmed reading of OD-C4 | EXIT (Plan:1107-1110); OD-C4; O4 (a); A:233-238, :283-290; P18 (bash script stops) | P: `WIFSIGNALED`/`WTERMSIG` after SIGINT; status 130 after SIGTERM/SIGHUP; W exit code 130; bash script continuation row |
| LR-EXIT-5 | MUST | CTRL_CLOSE, and CTRL_LOGOFF and CTRL_SHUTDOWN where received (LR-EXIT-1), start Force at once (no Cancel stage), so the OS deadline is spent on the write wait. | The OS ends the process ~5 s / ~20 s later regardless | A:107-110 (W close events take Force at once); OD-C7 (c); PF ext (HandlerRoutine timeouts) | P (W): close the console during a write; U: handler routine called with LOGOFF and SHUTDOWN codes; O row for the deadline |
| LR-EXIT-6 | MUST | **Output never blocks or breaks the exit.** A failed write to stdout or stderr (EPIPE, EIO) never panics and never skips the exit sequence. Once an interrupt arrived, no sender blocks on a stalled consumer. Exactly one party writes the final document (the output claim). Progress lines may be dropped, and the drop is counted. Notices and the final document are not dropped while the consumer accepts bytes; if it accepts none until B-OUT, the rest is dropped, the drop is stated on stderr where stderr accepts it, the exit status is unchanged, and a partial document is never presented as complete (a `--jsonl` stream ends without its end record, which LR-OBS-3 defines as truncated). | Dead pipes and hung-up terminals are routine under interrupts (`2>&1 \| tee`); a stalled `\| less` must not hold the exit | A:111-120, :239-244; CS S-P2-3 (server `eprintln!` panic); B-OUT behaviour **PROP** | P: close stdout, close stderr, stall stdout, hang up the pty, at each stage; S (no print macros outside the writer) |
| LR-EXIT-7 | MUST | A panic is caught at the operation, thread and FFI boundaries. It becomes `Failed(internal_error)`, cleanup runs, and other operations continue. No unwind crosses Python or C. A panic on the interrupt reader restores the saved dispositions. | A panic must not become an orphaning crash | CT:255-259; SD:255; A:121-123 (reader under `catch_unwind`) | U: inject panics at each boundary; X |
| LR-EXIT-8 | MUST | **Server shutdown** (stop, idle exit, SIGTERM/SIGINT, W events) runs in order: stop admitting while answering status; close every session through the closure path; wait for detached workers; shut the host context down; release the address last. A Force at **any** step ends every pending wait except local writes to a safe point and W children (no session waits out `close_wait` with network work alive). | Ordered stop; decision (i) applied to the server | SRV:586-598; SCC:239; decision (i); CS S-P2-1 / CC P2-3 | P: Force at each shutdown step; assert immediate kill of network and K children |
| LR-EXIT-9 | MUST | A `gwz server stdio` core ends when its session ends: after `session.close`, EOF on input, or a failed write. It then shuts its host context down within B-CLEAN. It finishes its own local writes to a safe point, and waits for nothing else. The client never signals it. It exits 0 after `session.close` and 1 otherwise. A signal or console event that reaches it directly (SIGINT, SIGTERM, SIGHUP; W CTRL_BREAK in its own group, E14) is carrier loss: every call is cancelled, local writes reach a safe point, and it exits 130 (LR-EXIT-4). The remote stdio core behaves the same. | Killing the core would tear its writes; a core that waits on a vanished client is an orphan | SRV:1080-1084; C:123, :127-130 (stdio never signalled); AM E3; own-signal clause **PROP** (OQ-24) | P: client SIGKILL during core write; core reaches safe point and exits; SIGTERM and W CTRL_BREAK to the core; X |
| LR-EXIT-11 | MUST | **Server self-stop.** A server whose address integrity check (every B-LINK) finds its socket or lock file unlinked or replaced stops accepting, logs, drains within the close bound and shuts down by LR-EXIT-8. A server whose log cannot be rotated shuts down at its next idle point. | An address another process took over, or an unbounded log, must end the server, not run on | SCC:241; SRV:608-616 | P: unlink and replace the socket and the lock file; F: rotation fails |
| LR-EXIT-10 | MUST | After an orderly session end, a client waits at most B-STDIO for its local core child or `ssh` child to exit. Then it kills `ssh` and never kills a stdio core. | Bounded client exit | SRV:1116, :1218-1224 | P |

### 5.4 Death detection (LR-LIVE)

**Links** that LR-LIVE-1 covers. Each direction is required.

| Link | A detects B's death | B detects A's death |
|---|---|---|
| L1 gwz ↔ its registered child | gwz: child-exit notification that keeps the ID pinned (no reap) | K: ended by the cover; S: EOF on its input; W: per OD-B4 |
| L2 gwz ↔ its cover | gwz: the cover's exit or EOF, reported as loss of cover | cover: gwz's death (EOF on its lifeline, or the W job handle closing) |
| L3 client ↔ local server core (socket or pipe) | client: EOF/error; stall rule | core: EOF/error; stall rule |
| L4 client ↔ stdio core child | client: pipe EOF, child exit | core: input EOF |
| L5 client ↔ `ssh` ↔ remote core | client: `ssh` exit or EOF; keepalive ends `ssh` | remote core: stdin EOF via `sshd`, when `sshd` sees the loss (NG-25) |
| L6 session ↔ operation worker | session: the worker's terminal report or thread end | worker: gate revoked (`Revoked` when the session is gone) |
| L7 pool ↔ idle remote connection | FIN/RST readiness while idle; unread bytes are not an end. W SSH: at reuse, before any request byte (NG-21) | (remote's concern) |
| L8 helper / SSPI worker ↔ gwz | gwz: exit and EOF | helper: stdin EOF; job closed with gwz |
| L9 PYM ↔ embedding interpreter | module finalization and atexit, on an orderly interpreter exit only (a SIGKILL runs nothing: Tier C) | (the application's) |
| L10 USR ↔ gwz | (user sees exit status) | gwz: SIGHUP or console close, when a terminal or console is attached; otherwise nothing (NG-25) |

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-LIVE-1 | MUST | For every link above, except the cells that name NG-4, NG-21 or NG-25, each side detects the other's death through an OS event that the death itself produces: pipe or socket EOF or error, a process-exit notification, a job or handle closing. Neither side polls the peer's state to learn of its death. Timers that enforce deadlines are not polling. Retries with a bounded backoff exist only to close an OS registration race. | The operator's "pipe closing or another mechanism"; polling is late, costly and racy | OQ2; AM C3 :46-47; CT:335-340 | P: kill each party in each link at each phase; assert detection without a timer advancing (stepped clock frozen) |
| LR-LIVE-2 | MUST | Detection does not depend on traffic. Death is detected while the link is idle (same exceptions as LR-LIVE-1). | Idle links are where death goes unseen | OQ2; SCC:237 | P: idle link, kill peer, frozen clock |
| LR-LIVE-3 | MUST | A peer that is alive but has stopped reading, while frames are queued for it, is detected at B-STALL and treated as carrier loss on that side. | Alive-but-stuck is the one case EOF cannot show | SCC:237; carrier `StallBounds` | U: stepped clock with queued frames and zero progress |
| LR-LIVE-4 | MUST | No process other than the intended holder keeps open a descriptor or handle whose closing signals death. That includes fork-without-exec copies of gwz's own process, threads spawning in a CLOEXEC window, grandchildren, and W handle inheritance. Liveness descriptors never occupy fds 0-2. | An inherited copy delays EOF indefinitely | B R-B3, R-B15; TR1.6:82; AM:127-130 (W `bInheritHandles`); CS S-P2-2 | U: an injected spawn port records every descriptor a spawn inherits during link creation; P: a spawn barrier in gwz's own threads pins the window; W handle-list assertion; start with fds 0-2 closed; Z: concurrent spawns |
| LR-LIVE-5 | MUST | A stopped process (SIGSTOP, SIGTSTP, SIGTTIN; on W a suspended process) is never treated as exited, and a stopped child never makes a thread spin. | `waitid(WEXITED)` reports stops on macOS | B:170-177 (exit observation); PF [V] Go `wait_waitid.go` | P: stop and continue a child; assert no exit and flat CPU |
| LR-LIVE-6 | MUST | The death of a detector (the cover, or its writer thread) is reported as loss of cover. It is never taken as a peer's death and never kills. Spawns that need the lost cover are refused before effect. | Wrong inference from detector death kills live work | B:183-187 (writer death, `ReaperGone`); R-B2 | P: kill and stop the cover; assert report, no kill, refusal |
| LR-LIVE-7 | MUST | An idle pooled connection closed by its peer is discarded before reuse. A request is retried on a fresh connection only if not one request byte was written (once). Unread bytes on an idle SSH socket do not count as an end. On W the SSH idle watch does not exist, so a dead pooled SSH connection is found at reuse, before any request byte (NG-21). | Idle loss must not become a failed or duplicated request | IL (operator 2026-10-07: "before any request byte"; SSH idle watch Unix-only accepted); CP:231; lane `gwz-dev-winflk2-r` F8 fix (uncommitted) | U, P: peer closes idle; host-key notice bytes pending; W: found at reuse and retried once |
| LR-LIVE-8 | MUST | A death signal is attributed to the right peer. A socket, pipe or PID whose identity is not pinned is never acted on. A stale server address is replaced only on proof that it is dead (owned socket, connect refused, the record names it) and that the object removed is the one proven dead, not a newer server's. | A new server's socket deleted by an old server's late unlink | SRV:307-317; CS S-P3-5 | P: two servers on one address, the stale check's step points (probe, decide, remove) pinned by handshake while the second binds |

### 5.5 Child processes (LR-PROC)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-PROC-1 | MUST | Every child gwz starts has a lifetime class, declared at the spawn site from a closed set. **K** (killable work: helpers, `rev-list`, `ssh`, `git fetch` fallback, SSPI worker) ends with its operation. **W** (Git write: commit/tag with hooks and signers) is never killed while gwz lives. **F** (user foreground: pager, `forall`) is never signalled by gwz. **S** (served core child: stdio core) ends at its input EOF and is never signalled. **D** (background server only) intentionally outlives. An undeclared spawn is refused. | Each class traces to an operator decision; one closed set removes per-site improvisation | decision (ii); OD-C6; R-B11; C:123, :127-130 (stdio never killed); BR1 | S: every spawn site names a class; U: undeclared refused |
| LR-PROC-2 | MUST | **No orphan.** Every K child, with every descendant in its process group or job, ends no later than its operation. When gwz's process ends, however it ends (orderly exit, Cancel, Force, SIGKILL, crash, console close), each K child ends by gwz before it exits, or by the cover within B-COVER after. Unix `ssh` shares gwz's group (LR-PROC-9), so it is ended by its own pinned identity, never by a group kill; its children are NG-1's. An S child ends at its input EOF once its own writes reach a safe point (LR-EXIT-9; bound per OQ-5). A W child ends per OD-B4 (OQ-2) and, on W, per its job's close action (OQ-27). The only exceptions are §6's NG-1, NG-7, NG-15, NG-17 and NG-18. | The operator's "no orphaned process" | OQ2; AM C2 :43-45; BR1; platform §4 (`ssh` and `/dev/tty`); SRV:1218 | P, K: kill gwz at each phase of each class; X: no K marker process B-COVER after gwz's death; S core gone after its safe point |
| LR-PROC-3 | MUST | **Never signal a process gwz did not create and has not reaped.** Every kill targets a process or group whose identity is pinned at the moment of the kill: it cannot have been reaped and its ID reused before the kill lands. | PID reuse makes an unpinned kill hit a stranger | BR4; PF POSIX XBD 4.14 (ext) | U: an injected kill port asserts every kill names a pinned identity, including after a foreign reap; P: signal-recording sentinel; Z: PID reuse pressure |
| LR-PROC-4 | MUST | gwz never waits for, reaps or signals processes it did not start. Its correctness never depends on nobody else reaping its children. A child status lost to a foreign reaper (SIGCHLD `SIG_IGN`, `SA_NOCLDWAIT`, `waitpid(-1)`) is reported as `StatusLost`, never read as success. No library gwz links installs a SIGCHLD handler or reaps on gwz's behalf. | tokio's SIGCHLD driver and embedding apps' reapers break pinning | B:170-177, :252-257 (`StatusLost`; tokio `process` removed); R-B1 | P: start under SIGCHLD `SIG_IGN`; S (no tokio `process` feature in production) |
| LR-PROC-5 | MUST | **Spawn signal hygiene.** No gwz signal handler can run in a child between fork and exec. A child starts with gwz-caught signals at their default, gwz's inherited ignores preserved, and a clean signal mask. A child placed in its own group or session cannot be killed or stopped by a terminal signal that lands between fork and placement. | Handler-in-child and the fork-to-setpgid window | B:138-148 (mask around spawn; child order); A:103-106 (handler PID check) | S: child-side code between fork and exec is async-signal-safe and runs under the blocked mask; P: an injected spawn port pauses the parent before and after placement while the test sends each signal; Z: signal storms during spawns |
| LR-PROC-6 | MUST | **Terminal.** Every Tier O ending leaves the terminal in the modes gwz found it in, including after a child it ended at a prompt. A child that must read the terminal (an `ssh` prompt) can read it. A W hook or signer that reads the terminal from its own group is reported when stopped (SIGTTIN). That accepted cost is R-B10. | Echo-off terminals and silent stops are user-visible residue | CS S-P3-1; OD-B1 (a) cost; SCC:247; **PROP** | P (pty): kill `ssh` at a passphrase prompt; hook reading `/dev/tty` |
| LR-PROC-7 | MUST | On W, every K, W and S child is inside a job from its first instruction. It inherits only its declared standard handles, never the starter's other inheritable handles. Its console membership follows its class. The job's action at gwz's death follows the class: K children are ended (LR-PROC-2); an S child is never ended by it (LR-EXIT-9); a W child per OQ-2. B's registry jobs end every member at close (M-B9), which conflicts for S and W; OQ-27. | Create-then-assign gap; inherited caller pipes hang a capturing caller | B:154-161 (W job from the first instruction); R-B14; CS S-P2-2; R-C8; **PROP** | P (W): capture `gwz --server stdio ... fetch` and `server start` output; assert no hang; job membership query |
| LR-PROC-8 | MUST | A credential helper is a K child bounded by B-INTER and by its operation's cancellation. Its slot wait is charged to the allocation deadline. Its completion is its exit plus EOF on its output. **Every** helper gwz causes to start is covered by LR-PROC-2. Native-route helpers that git2-rs starts are covered only once OQ-3 is decided; until then NG-15 holds for them. What a *successful* helper leaves by design (`credential-cache` daemon) is not gwz's. | OQ2 says "every helper process it started" | OQ2; TR1.6 OQ3 (b), OQ5 (a), OQ6 (a) :404-422; R-B5, R-B6 | P: helper hangs, prompts, forks a background child; X |
| LR-PROC-9 | MUST | An `ssh` carrier child ends with its session. It can read the terminal for prompts, so on Unix it shares gwz's foreground group, and the terminal's interrupt reaches only gwz. Before `SessionOpened` an interrupt ends it; with no terminal it is ended at B-SSHOPEN. After `SessionOpened` cancellation is a protocol cancel, and `ssh` is first given the chance to end by itself (and its children with it), then killed at B-STDIO. Its own children are its concern (NG-1). | Prompts need the terminal; Ctrl-C must not kill a live session | SRV:1213-1224; SCC:247; platform §4 | P (pty and no-terminal) |
| LR-PROC-10 | MUST | **Fail closed.** When the cover a child needs cannot be established (cover not started or gone, job not assigned, capacity, out of memory), the child is not started and the operation fails before effect with a typed cause. | A child without cover is a guaranteed orphan on gwz's death | B:111-123 (`RegistryFault`); CT:574-590 (fail-closed safety) | U, P: inject each failure |
| LR-PROC-11 | MUST | The SSPI worker (W) lives inside a kill-on-close job. It ends at its conversation's end, its absolute deadline, or gwz's death. | Native auth must not outlive gwz | `GwzSspiDesign.md:116-133` (exists) | P (W) |
| LR-PROC-12 | MUST | The number of live and unreaped children per process is bounded: configured helper slots (8 by default) and the registry capacity. Exhaustion is refused before the child starts. | R6 sustainability | D-V (helper slots); B:133-134 (capacity 256); R6 | U |
| LR-PROC-13 | MUST | A child's completion requires its exit and EOF on every output gwz reads. A grandchild that holds the pipe never extends the operation beyond the child's bound. On that bound the tree is ended (K) or the hold is reported. A W child has no bound of its own (U2), so once the W child itself has exited gwz waits at most B-EOF for EOF; then it stops reading, reports the holder (NG-1), and completes the child with its exit status. | Background children holding stderr hang the parent; a hook's daemon must not make U2 endless after git exits | TR1.6:82; W post-exit clause **PROP** | P: child exits; grandchild keeps stderr open (K and W) |
| LR-PROC-14 | MUST | A child's environment is the one captured for its operation, scrubbed by class. No credential environment reaches a child that does not need it. The served core and its children never receive the client's environment. | Credential exposure through children | CT:470-473; SCC:81, :210; R4 ("a helper never runs with another operation's environment") | U: environment captured per operation; P: inspect child environment |

### 5.6 Resource cleanup (LR-CLEAN)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-CLEAN-1 | MUST | Every terminal receipt states the outcome **and** the cleanup status (`Complete` or `Pending(ticket)`) of the resource it closes. | Truthful completion | CT:272 | U |
| LR-CLEAN-2 | MUST | Each owner's close is bounded by **one** overall bound (B-CLEAN), not one bound per child owner in series. At the bound, the remainder is retained (LR-OWN-4) and reported pending. | Serial bounds compound into unbounded exits | CT:189-190 (today: host bound, then domain, in series) | U: N hosts, stepped clock |
| LR-CLEAN-3 | MUST | Cleanup never changes an operation's outcome or cause. | Same as LR-CAN-7, for every resource | BR7 | U |
| LR-CLEAN-4 | MUST | A connection returns to idle only after its protocol confirms clean completion and reusability. Unknown state, a dropped body, pending native work or an unknown write effect discards it. A settling exchange ends at B-SETTLE. A connection or SSH session that failed is never reused; a retry the operator allows (W KEX retry, twice) uses a new TCP connection and a new session, within the same deadline. | A half-clean connection poisons the next operation | CT:612-618 (no settle deadline today); TR18 D14/D32/D44 | U: adapter never resolves `poll_finish`; assert discard at bound; failed-session reuse refused |
| LR-CLEAN-5 | MUST | Every temporary gwz creates in a workspace or repository has a recognizable name shape and one owner. A Tier O ending removes it. After a Tier C event, the next writing command (LR-REC-2) removes it only with proof that it is gwz's own and that its writer is dead; the proof must exclude every other writer, a non-gwz libgit2 user of the same `.git` (an IDE) included. A temporary whose name libgit2 chooses inside a call cannot be named in advance (LR-OWN-3), so it is removed only where the proof still holds, and otherwise reported. | "No turds" made concrete for files, without deleting another process's live work | resolution (1); D:94-120 (D3, M1) evidence; DS S-P2-4, S-P2-5; LR-CONC-3 | K: kill at each temp-creating step; next command; X (file census); U: a foreign libgit2 temp of the same shape is reported, not removed |
| LR-CLEAN-6 | MUST | gwz never removes what it cannot prove is its own: Git `*.lock` files, libgit2 temporaries of another process, user files, a user-chosen clone target. It reports them with absolute path, age and a shell-safe remedy. | A wrong delete destroys the user's or another process's live work | OD-C3 (a); DS S-P1-1 (EACCES → `GIT_ELOCKED`); DS S-P2-3 | U, K: present each foreign artifact; assert untouched and reported |
| LR-CLEAN-7 | MUST | Process exit is relied on only for what the OS reclaims at death: memory, descriptors, handles, advisory locks, anonymous temporary files, job members with kill-on-close. Everything that survives death (named files, sockets, records, remote effects) is cleaned explicitly on Tier O or covered by LR-REC on Tier C. | A precise line between "the OS does it" and "gwz must do it" | OQ2; resolution (1) | R; K |
| LR-CLEAN-8 | MUST | The server socket or pipe and its record are removed on every Tier O exit. After a crash they are recognized and replaced only by proof (LR-LIVE-8). The lock file may stay and is not stale. | Address residue blocks the next server | SRV:301-318; SCC:240 | P, K |
| LR-CLEAN-9 | MUST | Spools and logs end with their session. A spool leaves nothing on disk after any death of its owner, or else it is a named temporary under LR-CLEAN-5. A server log is rotated at its bound. | Bounded on-disk growth | SD:281-289; SRV:608-616 | U |
| LR-CLEAN-10 | MUST | Close and shutdown are idempotent and safe to call concurrently. The first report stands. A dropped close or finish future leaves its cleanup owned (by the domain or retainer); it does not abandon it. | Close races are routine under interrupts | CT:188-189, :746; SD:437 | U |
| LR-CLEAN-11 | MUST | A long-lived owner (gwz-py, server) stays flat: 10,000 operations on one owner; threads, tasks and per-operation records do not grow; the 256-binding bound is crossed with zero stale events; RSS growth after warm-up stays within B-RSS, fixed from the measured baseline before this row gates (as R3 fixes its bound). | R6 | R6 (Plan:171, :285); R2; R3 (Plan:282, "fixed from baseline") | E (baseline fixes B-RSS), then U/E regression at B-RSS |
| LR-CLEAN-12 | MUST | **Physical close is decoupled from member completion.** A member completes at its protocol end. Its connection returns to idle only after the close completes cleanly, and is discarded if the close fails or exceeds B-CLEAN. A push's close waits for the client's EOF to reach the server. A new exchange waits at most B-MIDCLOSE for a connection that is mid-close. A discarded connection is disposed within one pool pass. | Operator direction on SSH background close | SBC:5-7, :110, :195-201 (operator 2026-10-07; revision 3, 2026-10-08) | U: stepped clock, close stuck, fails, mid-close lease |
| LR-CLEAN-13 | MUST | A temporary gwz creates outside any workspace (a `read_remote_file` probe clone in the system temporary directory) has a gwz-specific name shape and names its creator. A Tier O ending (Force included) removes it. After a Tier C event, any later gwz command that finds it reports it; it is removed only under LR-CLEAN-5's proof. | R16: Force or a crash leaves a partial clone in system temp | A R16 (A:370); F57; D:164-166 (M-D6) evidence; **PROP** | K: kill during a probe; X (system temp census, LR-TEST-7) |

### 5.7 Crash consistency and recovery (LR-REC)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-REC-1 | MUST | For every write sequence a Tier C event can cut, the set of states it can leave is **enumerated**: exhaustively from step points for gwz-owned writes; for writes inside libgit2 or a Git child, from the protocol that writer follows (lock, temp-then-rename), checked by LR-TEST-6's file-event cuts. The next gwz command in the workspace recognizes each state, from a durable record or from the state's own shape. It then recovers the state, or reports it with a remedy that gwz itself will accept: gwz's own dirty-tree checks must not refuse the advised re-run. | resolution (1) verbatim; executable remedies | resolution (1); DS S-P2-1 (refused remedy) | K: kill at each step point of each sequence; run the advised remedy; assert it succeeds |
| LR-REC-2 | MUST | **One recovery path, two roles.** Recognition runs at one defined point, in every command that resolves a workspace (and, for creation sequences, every command that names their target, LR-REC-13), in human, `--json` and `--jsonl` modes. In a command that takes no workspace lock (a read-only one, LR-CONC-4) it only reports and changes nothing. Removal and recovery run only in commands that hold the workspace's mutator lock, and in an explicit recovery command. Recognition never runs only in selected commands. | Per-feature recognizers miss commands and modes; a read must not repair | resolution (1); D r1 R-D3 / DC P2-2; read/write split **PROP** | U: every command, every mode, seeded residue; `status` reports and leaves the residue; a writer recovers it |
| LR-REC-3 | MUST | **Durability order.** An intent record covering an effect reaches stable storage, file and directory, before the effect begins; its removal reaches stable storage after the effect completes. A record never claims an effect that did not begin. A power loss between an effect's completion and its record's durable removal leaves a live record for a completed effect; recovery recognizes that state and retires the record without repeating the effect (idempotent). On macOS plain fsync does not reach stable storage (PF ext); on W no directory-sync primitive exists (NG-3, OQ-7), and LR-REC-11's degradation applies. | Unsynced markers resurrect, or vanish before their effect | DS:62 (unsynced unlink); code (`durable_write`, W directory sync is a no-op; code §9); **PROP** | F (power-loss model drops unsynced writes at every cut point; checks gwz's ordering only); O (macOS flush semantics; W directory and rename durability); E (real power-cut runs, where a host allows) |
| LR-REC-4 | MUST | Recovery never destroys user work. It never removes a file it cannot prove it created. It never follows a symlink or reparse point out of the workspace. It never deletes index-tracked content without saying so. | Sweeps through shared or linked directories delete other workspaces' files | DS S-P2-5, P3 (tracked temps); **PROP** | U, F: symlinked swept directory; tracked temp |
| LR-REC-5 | MUST | Partial state left by Force at a safe point (O-forced) is recorded and handled by the same recovery path as Tier C residue. | Force's partial state is a crash-like state with a live witness | resolution (1); A R1-R3, R18 | P: Force mid-checkout; next command reports |
| LR-REC-6 | MUST | **Error-path atomicity.** A write that fails (ENOSPC, EIO, sharing violation) publishes nothing, removes its own temporary and releases its Git lock. A single file is replaced atomically or not at all. On W, a replace refused because another process holds the target open is such a failure (E20), with a typed cause. | Error paths are the orderly cousin of crashes | code (`durable_write` pair-write leaks, D r1 F1 evidence); **PROP** | F: fail each write step; O (W replace atomicity and its sharing-violation refusal) |
| LR-REC-7 | MUST | A working tree torn by Tier C, or stopped at a safe point by Force, cannot be staged or committed by gwz unseen. A later `stage`/`commit` of that member refuses, or warns with the state, until the tree is resolved. | A torn checkout looks like the user's edits | D r1 OD-D16 (open); DS S-P2-1, S-P2-2; **PROP** | K, P |
| LR-REC-8 | MUST | An interrupted clone, materialize or init leaves a state gwz recognizes as an interrupted gwz clone. gwz neither adopts it silently nor advises deleting a directory it cannot prove it created. | `~/src` must never be advised for deletion | A R6, R7; DS S-P2-3; **PROP** | K |
| LR-REC-9 | MUST | For every remote-writing exchange, each ref's effect is one of `none`, `unknown` or `received`, and is reported (human and JSON). gwz never resends a request whose bytes may have been sent. Recovery of `unknown` is the next fetch or status, never a replay. | Remote effects cannot be undone | CT:660-666; R5; A R5; D r1 M4 | P, K: cut at each push phase |
| LR-REC-10 | MUST | HEAD, index and refs of a member are only ever left in enumerated relations. Every Git lock gwz or libgit2 holds is held only around writes that contain no safe point. | No stale lock from an orderly stop; bounded crash states | OQ2 ("no stale lock"); A G2 | S/U (no gate safe point inside a ref transaction); K |
| LR-REC-11 | MUST | On a filesystem where a durability primitive is unavailable (tmpfs, overlayfs without `nfs_export`, some FUSE, W directory sync), gwz warns once per command. The guarantees of LR-REC are then stated as degraded. | Honest guarantees | code (`checked_artifact/entry.rs:231-269`); **PROP** | U, O |
| LR-REC-12 | MUST | Recovery is explicit and command-driven, taken under the workspace's locks. It is itself crash-safe and idempotent: a recovery cut by a crash converges when run again. The existing durable recovery mechanisms form a closed list, and each either is reached through LR-REC-2's point or keeps its own named trigger: merge records and the action journal (authority for merges); the checked-artifact quarantine; the catalog's `recover_or_create`; admission's `resume_or_admit`; family `creating`/`disposing` rows (disposed, never replayed); and the merge's no-recovery degradation on volatile filesystems (LR-REC-11). A mechanism outside the list is refused. | No background repair; recovery must not create new residue; one path, with every existing authority accounted for | `GwzM5-8I2RecordContract.md` §6, `GwzM5-8I2ActionJournalContract.md` §2; code (no startup hook; code §9: `residue.rs`, `catalog/bootstrap.rs:233-240`, `admission/driver.rs:35`, `checked_artifact/entry.rs:231-269`); family store; closed list **PROP** | K: kill during recovery; rerun; S: every recovery entry is on the list |
| LR-REC-13 | MUST | **Creation sequences.** A command that creates a workspace or member directory (workspace-root `clone`, `init`, materialize before the manifest exists) makes its intent durable before its first effect, at a place that exists before the effect. Every later gwz command that names the same target finds it, although the target does not yet resolve as a workspace, and handles it by LR-REC-8. | An interrupted creation leaves a directory that no workspace recognition reaches (R6, R7) | A R6, R7; DS S-P2-3; **PROP** | K: kill at each step of each creation sequence; rerun and `status` naming the target |

### 5.8 Credentials (LR-CRED)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-CRED-1 | MUST | No core form ever holds a credential. No environment and no credential-bearing value crosses the carrier. | Less-trusted core | Plan:37-40; R7; SCC:81; H:229-230 | U, S (wire validation refuses key 3; no credential type in core-reachable crates) |
| LR-CRED-2 | MUST | A credential lives in memory only as long as the operation that obtained it, in zeroizing storage. It is never logged, formatted, put in a pool key, profile, instance, connection record or file, and never sent to another origin. | Operation-scoped secrets | H:182-239 (operator b2); OQ4 (d) | U; S (no `Debug`/`Display` of secret types) |
| LR-CRED-3 | MUST | No process gwz creates for a lifetime purpose (the cover process, the background server copy, the stdio core) holds a credential gwz obtained, or credential-bearing environment. The cover is created before gwz obtains any credential, and runs with an environment that carries no credential variable and no agent socket, whatever gwz inherited. The server scrubs credential variables and `SSH_AUTH_SOCK` before any thread. Memory the embedding application held before composition, copied into a forked cover, is NG-9. | A forked copy taken late holds secrets for its whole life | B:103-104 (registry before credentials); SCC:210-211, :234 | U, P (inspect environment and memory markers) |
| LR-CRED-4 | MUST | A redirect after a challenge resends the accepted credential only to the same origin. Another origin starts its own lookup on its own 401. A redirect never causes a helper `erase`. | Decided from evidence | OQ4 (d) (Plan:1047-1060) | U, E (three platforms) |
| LR-CRED-5 | MUST | An authenticated connection is leased only by an equal scope. It lives idle at most B-IDLE, and is disposed when its scope ends. | Cross-scope reuse leaks identity | CT:553-559; H:234-236 | U |
| LR-CRED-6 | MUST | The credentials of a cancelled, failed or forced operation are wiped at its terminal on every Tier O path. A Tier C event leaves no credential on disk written by gwz. | Wipes must not depend on the happy path | H:231-233; SD:306-315 | U; K (scan the workspace and temp dirs for a canary secret) |
| LR-CRED-7 | MUST | A helper runs with its own operation's captured environment and never with another's. A credential-retaining context is admitted once; clones cannot extend its life into a second operation. | R4; H single admission | R4 (Plan:169); H:211-219 | U |
| LR-CRED-8 | SHOULD | Rejected credentials are wiped eagerly, and only a keyed fingerprint is kept for the operation's life. | Minimizes the time a known-bad secret is in memory | H:240-246 | U |
| LR-CRED-9 | MUST | **A core never extends a connection's or credential's life or reach.** The client admits a core's request only for an allowed (scheme, host, port, path) tuple, with the service class of the client's own command, a separate push allowance, and a client-chosen identity and username. It re-applies the check at every redirect hop. Under `--server` the default allowance is empty and fails closed. No core request lengthens an idle, authenticated or bound connection's life past the client's own bounds. | Goal 7: the core is less trusted | Plan:55-61 (goal 7); D-H (b) (Plan:954-958) | U: disallowed tuple, redirect to a disallowed hop, empty default, core request after B-IDLE |

### 5.9 Bounds and deadlines (LR-BND)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-BND-1 | MUST | Every wait in gwz is (a) bounded by a named bound, (b) ended by its scope's cancellation within B-CLEAN, or (c) one of **U1** a local write to its next safe point (decision (i)), **U2** a W child with its hooks and signers until it exits (decision (ii); afterwards LR-PROC-13's B-EOF), or **U3** a user foreground program (OD-C6). Each U-wait is announced on stderr while it lasts. Waits that §6 accepts (NG-15's native route and spawn-gate hold, NG-17 and E27b, NG-20) are listed there, not here. Waits inside linked C libraries and third-party crates (libgit2, libssh2, the HTTP engine, OS calls) are classified per native call class by the bound or cancellation that ends each. | "bounded and truthful" made checkable | R5 (Plan:170, :284); decision (i), (ii); OD-C6 | S (every blocking wait site in gwz's own sources is classified; the S check does not see inside FFI); U or P per native call class (stall the call; assert its bound or cancel ends it) |
| LR-BND-2 | MUST | A deadline is minted once. No poll, retry, 401, redirect, reuse, new physical generation or native round extends it. Clocks keep running through a credential prompt or agent confirmation (Pageant). The pool's connect clock, which only classifies a server stall, pauses while a resource reports a local wait, so a local wait never reads as a server stall; that pause never extends the operation's deadline (OQ-21). | Silent extensions defeat every bound | CT:704-713; TR18 D5 (b); CP:268-270 (operator 2026-10-07) | U |
| LR-BND-3 | MUST | The bounds below are normative defaults, except rows marked **PROP** or OQ-28, which are not normative until confirmed. Each is enforced only by its owner's timer. A client timeout only stops waiting or sends a cancel; it never decides the core's outcome. | One table, owned timers | SD O5 (:82); sources per row | U per bound |
| LR-BND-4 | MUST | A bound's expiry yields a typed outcome naming the bound (`AdmissionTimeout`, `AllocationTimeout`, `credential_helper_timeout`, `Stalled`, `server_unavailable` ...), never a generic I/O error. An OS connect `TimedOut` or `AddrNotAvailable` stays final (no retry). | Diagnosable stops | CT:717-722; TR1.6 OQ5 (a); TR18 D18 | U |
| LR-BND-5 | MUST | Outcomes stay correct when cleanup is slower than its grace, for example on a loaded W host where a job drain outlasts 500 ms. | Graces are not outcome bounds | f9 report; BR7 | U (stepped), E (loaded W) |
| LR-BND-6 | MUST | Every count is bounded: queues, registries, records, spools, tombstones, observations, children, connections. Exhaustion is refused before effect and names the exhausted budget. | R6; no unbounded growth on long-lived owners | CT:628-629, :675-687; SD:41-58 | U |
| LR-BND-7 | MUST | Deadlines use a monotonic clock. A suspend/resume or wall-clock jump expires idle connections past their bound before any lease. Backward ticks are ignored. | Sleep must not resurrect dead connections | CT (suspend epoch, R1b/R3b); pool `advance` | F (clock) |
| LR-BND-8 | SHOULD | With no U-wait in progress, an orderly stop fits inside common supervisor windows: W CTRL_CLOSE (~5 s), `docker stop` (10 s default), logind (90 s). Waits that cannot fit (for example 2 x `close_wait`) are documented. | Supervisors escalate to SIGKILL | CS:193, :230 (supervisor windows); OD-C7 (c); **PROP** | E, R |
| LR-BND-9 | MUST | **Adaptive concurrency stays bounded.** In the Down state, an arrival waits at most B-RETEST for a retest and fails after two failed retests ("Park, give up after 2"). A throttled upload is reported once and no member is re-run. A believed limit is remembered at most B-LIMITMEM after its key's last use. | Throttle handling must not become an unbounded wait or a replay | ACD:628-646 (operator 2026-10-07); CP:328 | U (stepped clock) |

**Bounds table (LR-BND-3).**

| Bound | Value | Owner | Source |
|---|---|---|---|
| B-CLEAN cleanup after Cancel; host/owner close (one overall bound) | 5 s | the closing owner | CT:189-190; `session_host/context.rs:80`; AM C5 |
| B-ADMIT client admission | 4 s, strictly below B-BOOT | host | D-P (Plan:1000-1004); CT:680 |
| B-BOOT carrier bootstrap (pending to bound) | 5 s | core mux | CT:886 |
| B-ALLOC allocation (incl. helper slot wait, at most 30 s) | 30 s | pool | CT:680; TR1.6 OQ6 (a) |
| B-SETUP setup aggregate / network stall | 30 s / 9 s | pool setup clock | CT:682 |
| B-INTER interaction allowance (cumulative, fixed) | 120 s | lookup | TR1.6:79; CT:683 |
| B-GRACE helper kill grace (not an outcome bound) | 500 ms | helper job | TR1.6:79-83; BR7 |
| B-IDLE / B-AGE idle connection / maximum age | 60 s / 10 min | pool | CT:532, :681 |
| B-CLOSEW session `close_wait` (a client value above the host's default is refused) | 60 s (at most 1 h) | session host | SD:56; P23:175-178 (SessionLimits Q1 (a)) |
| B-STALL / B-CLOSEA carrier no progress / close flush | 60 s / 10 s | carrier writer | SCC:136, :237 |
| B-HS handshake host wait / open wait / state wait | 30 s / 30 s / 5 s | each side | SRV:172-185; SCC:80 |
| B-READ held event read | at most 30 s | session host | SD:41-56 |
| B-STDIO client wait for a local core or `ssh` child after close | 10 s | client | SRV:1116, :1220-1224 |
| B-STOP `server stop` wait | 3 x close bound + 5 s = 185 s | `server stop` | SRV:533, :598 |
| B-LINK server address integrity check | 30 s | server | SCC:241 |
| B-SSHKA `ssh` keepalive | 15 s x 3 | `ssh` | SCC:237, :247 |
| B-OUT Force's final writer flush | 1 s | interrupt module | A:205-213 (Force step 8) |
| B-MIDCLOSE wait for a mid-close connection | 250 ms (constant) | pool | SBC:110, :201 |
| B-RETEST / B-LIMITMEM Down-state retest wait / believed-limit memory | 30 s / 10 s | endpoint supervisor | ACD:634-635; CP:328 |
| B-GUARD guard wait on a contended gwz lock | 300 s | the waiting command | `GwzOpenDecisions.md` D8 |
| B-SSHOPEN `ssh` spawn to `SessionOpened` with no terminal | 30 s | client | SRV:1213-1216 |
| B-HELLO / B-PROBE `server start` wait for hello / stale-socket connect probe | 5 s / 5 s | `server start` | SRV:307-317, :564-575 |
| B-SSHTO libgit2 server timeout (`--ssh-timeout`) | 3 s | native route (NG-15) | P18 (gwz-cli 1.0.18 `src/lib.rs:176-178`) |
| B-TURN / B-LINGER listener turn-away / carrier close linger | 1 s / 2 s | listener / carrier writer | code (session notes: listener, `StreamCarrier`); **PROP** |
| B-SETTLE, B-FACTORY, B-COVER, B-EOF, B-RSS settling exchange; endpoint factory construction (CT:300); cover delivery after gwz's death; W child's post-exit EOF; RSS growth | no source fixes a value | owners per row | OQ-28 (fixed from an E baseline, as R3) |

### 5.10 Observability and reporting (LR-OBS)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-OBS-1 | MUST | **Every effect is reported.** For each member: its outcome (`Completed`, `Cancelled`, `Failed`, `Partial`); the local writes completed, or the safe point at which they stopped (with its label); the remote effect (`none`, `unknown`, `received`); the cleanup still pending, naming what the client still owns. | Tier O's second half | R5; A5, A6; D-V OQ2 | U (renderers, both modes); P (end-to-end) |
| LR-OBS-2 | MUST | Interrupt notices go to stderr in every mode: interrupted and cancelling; forced; each U-wait (`stopped: <member> (<phase>)`, waiting for a hook, signer or pager); transfer ended; progress lines dropped (with the count). | The user must know what gwz waits for | A:245-256; LR-BND-1 | P |
| LR-OBS-3 | MUST | Machine output has a defined shape for every ending: a `Cancelled` entry first among the errors; a per-member `remote_effect`; a forced envelope; a defined end of a `--jsonl` stream. The defined shapes cover server lifecycle subcommands too. | Scripts parse endings | OD-C4; A:245-256; R13 (OD-D8 open); CC P3-7 | U (schema rows) |
| LR-OBS-4 | MUST | Residue found by the recovery path appears in human and machine output of every command that resolves the workspace. | One recovery path, one surface | resolution (1) ("a named recognizer in the next command"); D r1 R-D3 / DC P2-2 (evidence for every mode) | U |
| LR-OBS-5 | MUST | Every LR-OWN-8 entry a user can act on is visible on a status surface: retained cleanup, detached workers, the background server (`server status`, `list`, `stopping`) and its log path, `creating` lanes, open merges. The other entries (idle connections, the believed limit, the SSH reaper thread) are visible to tests through LR-OBS-9's counters; unlocked lock files need no surface (LR-CONC-5). | Outliving things must be findable | LR-OWN-8; SRV | U |
| LR-OBS-6 | MUST | The server log records each operation's method, ID, outcome and duration, on carrier loss too. It never records environment, bodies, credentials, URLs or repository names. | Core-side truth when the client is gone | SCC:238; SRV:608-616 | U |
| LR-OBS-7 | MUST | Remedies are safe to paste: absolute paths, quoting correct for the platform's default shells (on W, `cmd` and PowerShell, with no wildcard or path interpretation), never the word "stale", never a wildcard. A Git lock error is reported with that remedy even where the library's own text would otherwise reach the user. | A remedy must not destroy another repository's live lock | DS P3 (PowerShell `del`, relative lock paths); **PROP** | U |
| LR-OBS-8 | MUST | Reports are truthful. No output claims more than happened (for example a wait that does not exist) or denies a write that happened. | A5 "no output denies a write" | A5; CC P3-7 | U, R |
| LR-OBS-9 | SHOULD | The host exposes fixed-size counters (connections created, reused and discarded; exchanges; retained cleanup) that tests and evidence read. | Makes R1-R6 measurable | CT:736-738, :882 | U |

### 5.11 Concurrency with the user's git (LR-CONC)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-CONC-1 | MUST | gwz never removes, breaks or steals a lock it does not hold: a Git `*.lock`, or another gwz process's lock. Contention gives a typed busy refusal, or a bounded wait that is announced. | Concurrent tools are the user's | OD-C3 (a); workspace mutator try-lock; `GwzOpenDecisions.md` D8 (300 s guard wait) | P: user's `git` holds `index.lock`; another gwz holds the mutator lock |
| LR-CONC-2 | MUST | gwz writes Git state as Git does, so the user's concurrent `git` and gwz exclude or tolerate each other: index, refs, `shallow` and config only through Git's lock protocol; packs and objects through Git's write-temporary-then-rename protocol, with no `.lock`. | Mutual exclusion with the user's tools | PF ext (git `lockfile.c`; pack writing) | P: concurrent `git commit` and gwz writer; O: Git's pack temp shapes |
| LR-CONC-3 | MUST | Age alone never proves a lock or temporary dead. A recognizer reports, and never removes, anything that might belong to a live concurrent process. | A one-hour-old temp can be another process's live transfer | D:137-143 (M3); DS:197; **PROP** | U, P |
| LR-CONC-4 | MUST | gwz's read-only operations take no Git lock, optional locks included (also in Git children they start), and leave no state. Recognition in them only reports (LR-REC-2). | Reads must not block the user's writes | code (`rev-list`); **PROP** | P |
| LR-CONC-5 | MUST | gwz's cross-process locks are released by the OS at process death. A lock file that remains unlocked is not stale and blocks nothing. | Tier C never leaves a blocking gwz lock | code (`workspace_mutator_lock.rs:47-56`; family lock); SRV:318 (lock file stays, not stale); **PROP** (the code-only part) | K |
| LR-CONC-6 | MUST | Across processes on one workspace (two CLIs, a CLI and a server, gwz-py and a CLI), a W or push excludes another W or push. A detached worker excludes later W and push on its workspace. A fetch on a member being written waits or refuses; it is never interleaved. | Cross-process write exclusion | SCC:24, :99-101; SD:224-226; gwz-core `crates/session-host/src/service/table/admit.rs:100-106` (N not gated) | P |
| LR-CONC-7 | MUST | gwz never signals the terminal's foreground group or its own group (`kill(0, ...)`, `killpg` of a group it did not create). | The user's other processes share those groups; Unix `ssh` shares gwz's own group (LR-PROC-9) | BR4 (B:31-40, never signal what gwz did not create); OQ2 | S, P |
| LR-CONC-8 | MUST | A merge record left open (by a crash or an interrupt) is the one gate that blocks guarded mutations until resolved. Its status and remedies stay reachable. | Existing recovery authority | M5-8 contracts; open-merge gate | U |

### 5.12 Embedding: gwz-py (LR-EMB)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-EMB-1 | MUST | gwz-py installs no signal handler and changes no process-wide disposition, signal mask, wakeup fd or console state. It never reaps processes it did not start. | The application owns the process | OD-C1; A:269-282 | S (AST check over `gwz-py/src/gwz/`); P |
| LR-EMB-2 | MUST | An application's cancel (asyncio cancellation, `cancel_call`, `Client.close()`) goes through the operation's canceller before any wait. A cancel before registration is latched. | resolution (2) | resolution (2); A8 | U |
| LR-EMB-3 | MUST | At interpreter exit, gwz-py cancels its own operations through their cancellers. It closes its owners within B-CLEAN, lets local writes reach a safe point, and waits for W children. No gwz-py thread blocks interpreter exit beyond these. Exits the application's own joins hold up are NG-20. | Orderly embedding exit | Plan:737-764; B:275-277 (atexit `close_children`) | P (interpreter exit at each phase, no `to_thread` call running) |
| LR-EMB-4 | MUST | `Client.close()` cancels only that client's operations. It never closes the module's `HostOwner`. | Shared owner across clients | CT:786-788; SCC:284-285 | U |
| LR-EMB-5 | MUST | `exec`, `os._exit` or a foreign `exit()` in the application leaves gwz state recoverable as Tier C residue under LR-REC. Whether these causes are accepted as Tier C at all is open: D classifies them beyond resolution (1) within OD-D1 (OQ-20). | Nothing runs; recovery must hold | B R-B4 causes; **PROP** | K |
| LR-EMB-6 | MUST | No gwz wait holds the GIL. Python-side cancellation can be delivered during any native wait. | Responsiveness to the application's cancel | gwz-py `py.detach` sites; **PROP** | P |
| LR-EMB-7 | MUST | A long-lived application keeps connections across calls: none created after call 2 of 52; creations at most CAP; last-10 median at most first-10 p90. The completed-result store is bounded by a finite policy. | R2, R6 | R2 (Plan:167, :281); CT:279-283 | E, U |
| LR-EMB-8 | MUST | Fork without exec in the application: LR-OWN-7 holds in the child copy. The parent's cover delay while the copy lives is documented in the gwz-py README as the operator's stated constraint. | resolution (2) "documented constraint" | resolution (2) | P (M, L) |
| LR-EMB-9 | SHOULD | The gwz-py CLI behaves like gwz-cli on SIGINT, as far as Python's own delivery allows without a handler (OD-C1): 130, and a SIGINT re-raise on Unix. Other signals and W events end it by default (NG-6). | One user model | A:283-290; OD-C7 (d) | P |

## 6. Non-goals and accepted limits

Each limit is stated once. "Operator" names the decision that accepts it. "none" means it is proposed here and needs a
decision (§11). Every row is also a §10 target.

| ID | Limit | Rationale | Operator | Residuals it covers |
|---|---|---|---|---|
| NG-1 | Processes another program starts outside gwz's process tree, group, session or job: a hook's background job (`ctags ... &`); a successful helper's daemon (`credential-cache`); gpg-agent and pinentry; brokers (WMI, COM, Task Scheduler); `setsid` descendants; `ssh`'s own children (ProxyCommand, ProxyJump, askpass). gwz lets `ssh` end by itself before it kills it (LR-PROC-9), so `ssh` can end its own children. A daemon that holds a W child's output pipe after the child exits is reported at B-EOF (LR-PROC-13). | gwz did not start them; signalling them would break LR-PROC-3 | TR1.6:82-83 for R-B6 ("nothing kills that child on success"); none for the rest (OQ-9) | R-B6, R-B7, R-B12, R-C12; AM E1, E5 |
| NG-2 | Ends forced by the OS or a supervisor: W ends the process ~5 s after CTRL_CLOSE and ~20 s after LOGOFF/SHUTDOWN; a process that loaded `user32` gets no LOGOFF/SHUTDOWN event; a caller's kill-on-close job; a service manager escalating from SIGTERM to SIGKILL. The result is a Tier C event. | The OS ends the process whatever gwz does | OD-C7 (c) (decision (iii)); `user32` narrowing: none (OD-D9; OQ-26). The candidate W build loads `user32` for Pageant (platform §10), so today it never receives LOGOFF/SHUTDOWN | R9, R-C10; R-B4 (W part) |
| NG-3 | Power-loss limits: data not yet synced; storage that acknowledges a flush it did not do; volatile filesystems; network filesystems with broken advisory locks; directory durability on W where no primitive exists. LR-REC-11 makes the degradation visible. | Beyond the process's control | resolution (1) names power loss as a residual class; the exact scope: none (OQ-7) | AM R1 |
| NG-4 | A peer that is alive and idle but not answering is not detected (no keepalive). The user's interrupt (B-CLEAN, then Force) and gwz-py `close()` are the remedies. | Keepalive was declined for this release | OD-C5 (a) | R-C1, R-C6; AM R8 |
| NG-5 | A further interrupt after Force began, and SIGQUIT at any time, take the OS default and can tear a write. | The user's escape without SIGKILL | O5; OD-C7 (a), (b). In-pager case: none (OD-D3) | R14 |
| NG-6 | gwz-py CLI: SIGTERM and SIGHUP end it by default. A local or native call runs to its end. A third (3.10) or fourth (3.11) SIGINT ends the interpreter mid-write. W CTRL_BREAK and CTRL_CLOSE also end it by default. | OD-C1 forbids a handler in gwz-py | OD-C7 (d); the rest: none (OD-D6, OD-D9) | R11, R15 |
| NG-7 | A fork without exec in an embedding application delays the cover's view of gwz's death until the copy exits. | Inherent to descriptor-EOF cover | resolution (2) ("a documented constraint") | R-B3 (fork part) |
| NG-8 | An embedding application that reaps process-wide (`waitpid(-1)`, SIGCHLD `SIG_IGN`) is unsupported. A status lost that way is `StatusLost`. The window between a child's exit and its unregistration while gwz dies can let a later kill hit a reused ID. | A process-wide reaper defeats pinning | none (OD-D10) | R-B1; AM R4 |
| NG-9 | Copies of secrets outside gwz's control: the HTTP engine's header copy, TLS and kernel buffers, swap, core dumps; secrets the embedding application or the inherited environment held before gwz composed, including their copy in a forked cover's memory. No memory locking. | Not controllable from user space portably | H A1 (H:234-236), accepted with H; the pre-composition part: none (OQ-16) | H residuals |
| NG-10 | Remote effects are never rolled back or replayed. gwz reports them (LR-REC-9). | A push cannot be undone safely | CT:660-666 (no replay); Plan §11 | R5, R-C9 |
| NG-11 | What a calling W script (`cmd`, PowerShell) does after gwz exits 130. | gwz cannot stop its caller | none (OD-D9; row 6.11 records it) | R-A1 |
| NG-12 | The background server outlives its starter by design, until `gwz server stop`, idle exit or a signal. | Explicit start and stop | D-I (no auto-start) | R-B11, R-C7; AM E4 |
| NG-13 | A Git `*.lock` is never removed by gwz, even when its writer is provably dead. | Git never judges staleness; neither does gwz | OD-C3 (a) | R-B4 (lock part), R-C5 (lock part) |
| NG-14 | A hung hook, signer, W child or local write keeps gwz waiting. Only the user's escalation ends it, as a Tier C event. A W hook that reads the terminal from its own group stops on SIGTTIN. | Writes finish, by decision | decisions (i), (ii); O5; OD-B1 (a) | R8, R-B10, R-C11; AM R10, R11 |
| NG-15 | Gates without a registry and hostless routes keep their 1.0.18 behaviour until step 3.3 retires them (LR-OWN-13): the ordinary build, `DriverOnly` before C, the native route and transport off. On the native route that includes git2-rs helpers outside any registry (OQ-3), un-abortable native push uploads and connect, negotiation and credential phases bounded only by libgit2's clocks (AM R2, R3), reaperless registries before steps 3.1 and 3.2 (AM R9), and on W the spawn gate held for a native-route helper's whole call, which blocks every other spawn without bound (D25). The operator prefers parity over a safety bound, with the hazard stated (MEM `no-native-route-fallbacks`). | Interim by plan | OD-B2 (a); MEM `no-native-route-fallbacks` (:15); R10's no-host part and the gate hold: none (OD-D5) | R10, R17, R-B5 (interim), R-B13; AM R2, R3, R9 (reaperless part) |
| NG-16 | A third-party spawn in the same process that does not take the spawn gate. | A library gwz does not control | D-E | D-E residual |
| NG-17 | Failures of the cover itself. On M/L: the reaper killed alone or chosen by the OOM killer; `pthread_atfork` handlers in the reaper; fork refused under strict overcommit; the cover's copy-on-write cost (AM R7). A stopped reaper (E27b) blocks spawns, and a blocked spawn holds the creation lock, so `close()`, `kill_killable()` and Force's child step wait without bound, against decision (i)'s "at once"; that wait is not a U-wait (OQ-23). On W: a suspended orphan between creation and job assignment. | Cost of a forked cover | OD-B3 (a) for fork cost (R-B16, R-B19, AM R7); the rest: none (OD-D11, OD-D13, OD-D15; OQ-23) | R-B2, R-B14, R-B16, R-B19, R-B20; AM R7, R9 (cover-dead part) |
| NG-18 | The macOS window between `pipe` and `FD_CLOEXEC`, in which another thread's spawn can inherit a liveness descriptor, so an S (`OwnInput`) child can miss its EOF and run on after gwz. **This conflicts with LR-LIVE-4 and LR-PROC-2**: either the spawn primitive must own every spawn, or the operator accepts it. | std's macOS pipe creation | none (OD-D17; OQ-12) | R-B3 (macOS part), R-B15 |
| NG-19 | HTTPS saturation starvation: when one authentication scope keeps every `per_user_host` connection busy, another scope's request waits and ends `AllocationTimeout` at its original deadline. No fairness. Reopen if the Phase 3 sweep shows a real two-scope workload. | Bounded and typed (LR-BND-4); fairness not built | D-R (Plan:1013-1018) | D-R |
| NG-20 | gwz-py at interpreter exit: CPython joins default-executor threads (`asyncio.to_thread`) before atexit runs, without bound on 3.10 and 3.11, so gwz-py's exit cancel cannot run while such a native call runs. The async `call()` exit waits for running operations. | APP owns the exit order (F16; platform §2.4) | TR18 D26 (:69): stated in 1.1.0's notes, fixed in 1.2.0; the executor join beyond that: none (OQ-19) | R11 (exit part); F16 |
| NG-21 | W has no SSH idle watch: a pooled SSH connection's death is not seen while idle, only at reuse before any request byte, when the request is retried once on a fresh connection (LR-LIVE-7). Needs its Windows-parity row (SR). | Unix-only mechanism, accepted | IL:292-304 (operator 2026-10-07) | IL |
| NG-22 | A user foreground program gwz started (pager, `forall` command) outlives a gwz that dies without running code (SIGKILL, crash) and keeps the terminal until it ends. The cover does not end F children. | gwz never kills them (OD-C6) | OD-C6; B accepted R-B8 under it | R-B8 |
| NG-23 | Before step 3.0g, a served detached worker's write can be torn by the core's process exit (R-C2 interim). | Served write frames land at 3.0g | none (OQ-24) | R-C2 (interim) |
| NG-24 | A Tier C event during a push to a local-path remote leaves Git's `<ref>.lock` and pack temporaries in that remote repository, outside the workspace (F43). The next push there fails on the lock; gwz reports it with its absolute path (LR-CLEAN-6). | The residue is the remote repository's | none (OQ-25) | F43 |
| NG-25 | Death detection that depends on parties gwz does not configure: (a) L5 remote side: the remote core learns of client loss only when `sshd` closes its stdin; with ClientAliveInterval unset and a silent network loss that takes TCP's own timeout (hours); (b) L10: with no terminal or console, the caller's death produces no event for gwz (no parent-death signal on macOS; declined on Linux by B); (c) L9: an interpreter killed without running code runs no atexit (Tier C). | No OS event exists for gwz to wait on | none (OQ-29) | E17b (remote side); L5, L9, L10 |

## 7. Testability requirements (LR-TEST)

| ID | Lvl | Requirement | Rationale | Source | Verify |
|---|---|---|---|---|---|
| LR-TEST-1 | MUST | Every MUST has at least one test that fails when the requirement is violated, on each platform §8 names. Review alone (R) never verifies a MUST. | A guarantee without a failing-capable test is a hope | this document (§0 provenance); OQ2 is unverifiable otherwise; **PROP** | S: matrix audit maps each MUST to named tests per platform |
| LR-TEST-2 | MUST | Tests are deterministic through seams: an injected clock, stepped ports, and named step points where a test can pause, cancel, kill or fault. A sleep is never a pass condition. A timeout in a test is a hang guard only; it records the process tree first, then reports the step it hung at. Where a window cannot be paused (fork to exec, PID reuse), the test reaches it through an injected port; a Z run supplements it, never replaces it. | Sleeps make lifetime tests flaky by construction | Plan §6 "Measured exits"; D-S (the receipt watchdog records the process tree first; tooling, applied here by analogy); **PROP** | S: no `sleep` in lifetime suites outside hang guards; R |
| LR-TEST-3 | MUST | **Flakiness is a defect.** An intermittent failure is filed against the product or the test and fixed. It is never retried into a pass, quarantined silently, or excluded from a gate. | Lifetime races hide exactly in "flaky" tests | Plan:178-181 ("A new blocker is reported with evidence ... never absorbed"); **PROP** | CI policy: no retry wrappers on these suites (S on CI config) |
| LR-TEST-4 | MUST | Process-level tests use a re-exec harness: the test binary re-executes itself in each role (gwz, child, core, peer). Nothing that any build compiles as production lives under `tests/`. | Real processes and signals without production code in tests (SR) | SR (no production code under `tests/`); A:95, :325-327 | S |
| LR-TEST-5 | MUST | Signal and event injection exists on all three platforms. Unix: `kill`/`killpg`, plus a pty for terminal-generated INTR, QUIT, SUSP, hang-up and SIGTTIN. W: `GenerateConsoleCtrlEvent` in a child console, console close, `TerminateProcess` as SIGKILL, process suspend. LOGOFF/SHUTDOWN, which a test cannot generate, are covered deterministically by a U row that calls gwz's registered handler routine with each event code; an O row records whether the build receives them (`user32`); E runs supplement. | Every event of §3 must be reachable by a test | O4 (a); Plan:48-49 (Windows a full peer) | S: every §3 event has a U, P or K row; O and E rows supplement |
| LR-TEST-6 | MUST | Crash injection (K) kills at every named step point of every gwz-owned write sequence. Writes inside libgit2 or a Git child do not pass through gwz's filesystem port, so K also kills on file events the harness observes in the repository (each create, rename and unlink), which places cuts inside those calls. A filesystem port with a power-loss model (drop every unsynced write at a chosen cut point) and ENOSPC/EIO faults tests LR-REC-3 and LR-REC-6 deterministically for gwz-owned writes; for libgit2 and Git writes those faults run as O or E rows on a fault-injecting filesystem where the CI host provides one. | Tier C made testable, within what the seams reach | resolution (1); **PROP** (power-loss model; file-event cuts) | S: every named step point has a K row; every libgit2/Git write sequence has a file-event K row |
| LR-TEST-7 | MUST | **Residue census (X)** runs after every P and K test. It checks: no process carrying the test's marker (environment tag, process group, session or job); no file outside the expected set (directory snapshot diff of the workspace, its `.git`, system temp and the per-user runtime directory); no socket; no lock held. Each P and K test runs with its own system temp (`TMPDIR`/`%TEMP%`), per-user runtime directory, server-address and `ws-<key>` record roots, and a test-scoped prefix on W named objects, so the census sees only the test's own roots, also when tests run in parallel. "No turds" is this census. | The operator's requirement as an executable check | OQ2 ("leaves no orphaned process, stale lock or partial state behind"); **PROP** (census mechanism) | S: census runs after every P and K test |
| LR-TEST-8 | MUST | Source checks are syntax-aware and blind to `cfg`, so they inspect disabled platform branches. They cover: globals and thread-locals, signal and console handlers, print macros, spawn sites and their class, `fork`/`pre_exec`, tokio `process`, wait sites (LR-BND-1), and detach APIs (LR-OWN-8). | Standing cfg rule; whole-class enforcement | SR (cfg rule: checks "must inspect disabled platform branches too"); OD-C1 | S (self-test of each check on seeded violations) |
| LR-TEST-9 | MUST | Parity rows run the released 1.0.18 binary for every behaviour this document calls P18 (exit status, script continuation, pager and `forall` under Ctrl-C, Git lock behaviour). The parity reference has never been measured. | Parity claims must be evidence | D-V (parity reference 1.0.18); P18 | E (archived parity runs, three platforms) |
| LR-TEST-10 | MUST | Every (ext) platform fact a requirement depends on has an O row on the CI hosts. Examples: W event timeouts and the `user32` behaviour; Git's lock cleanup on SIGINT/SIGTERM/SIGHUP; CTRL_C not reaching a new process group; SIGCHLD `SIG_IGN` across exec on Linux; `poll` and `EINTR`; the orphaned-group SIGHUP/SIGCONT; macOS fsync versus full flush; W replace-by-rename atomicity and sharing violations; Git's pack temp shapes; `sshd` client-alive defaults. | Requirements must not rest on memory of man pages | PF entries marked (ext); **PROP** | O |
| LR-TEST-11 | MUST | Every suite runs on macOS, Linux (kernel version recorded; pidfd 5.3+, `SO_PEERPIDFD` 6.5+) and Windows (10 / Server 2016 or later). On the candidate build each MUST is a gate. On the ordinary build and the NG-15 routes, until step 3.3, the same suites run as parity rows against 1.0.18 (LR-TEST-9), not as gates. The cross-process rows (LR-LIVE-1, LR-CAN-9, LR-EXIT-9, -10) run first in the two-processes-on-one-machine form on all three platforms. | Windows is a full peer; first proof is two processes on one machine | Plan:48-49 (goal 4), :52-53 (goal 6); SR (Windows-parity inventory) | CI matrix inspection; S |
| LR-TEST-12 | SHOULD | Fault-injecting ports exist for disk full, EIO, W sharing violations, network cut, stall and RST, and clock jump and suspend. | F-coded rows of §8 | **PROP** | R |

## 8. Acceptance matrix

Cells give the verification codes per platform. "n/a" means the requirement has no instance on that platform; the reason is
given. A row covers the listed IDs when every cell is identical. LR-TEST rows are meta-requirements on the matrix itself.

| Requirements | M | L | W | Notes |
|---|---|---|---|---|
| LR-OWN-1, -4 | U X | U X | U X | resource-class table tests |
| LR-OWN-2, -8 | S U | S U | S U | process-globals and detach allow-list checks |
| LR-OWN-3 | U P S | U P S | U P S | step points at each creation edge |
| LR-OWN-5, LR-CAN-6 | U P | U P | U P | R3 row |
| LR-OWN-6, LR-CRED-1 | U S | U S | U S | R7 row |
| LR-OWN-7, LR-EMB-8 | P | P | n/a (no fork) | |
| LR-OWN-9 | U | U | U | case folding per filesystem (case-sensitive APFS, L default, W per-directory); W short names |
| LR-OWN-11 | E U | E U | E U | R1 row; creation counter |
| LR-OWN-12 | R | R | R | SHOULD |
| LR-OWN-13 | S | S | S | after step 3.3 |
| LR-OWN-14 | U | U | U | each internal death (E31-E33) |
| LR-OWN-10, LR-CLEAN-8, LR-LIVE-8 | P K | P K | P K | two servers, one address |
| LR-CAN-1 | U P | U P | U P | same case set over every form |
| LR-CAN-2, -9 | P X | P X | P X | every phase, both sides |
| LR-CAN-3, -4, -7, -8 | U | U | U | |
| LR-CAN-5 | U K | U K | U K | gate step points |
| LR-CAN-10 | P (pty) | P (pty) | P (child console) | |
| LR-CAN-11, LR-EXIT-3 | P K | P K | P K | slow hook; Force at every gate state |
| LR-CAN-12, LR-CLEAN-11, LR-EMB-7 | E | E | E | baselines, then regression rows |
| LR-EXIT-1 | P O | P O | P U O | W: console events by P; LOGOFF/SHUTDOWN by U (handler routine) and O (`user32`, timeouts) |
| LR-EXIT-2 | P | P | P | 1, 2, 3 interrupts per phase |
| LR-EXIT-4 | P O | P O | P O | M/L: `WIFSIGNALED` and bash script row; W: exit code 130 and the `cmd`/PowerShell row (NG-11) |
| LR-EXIT-5 | n/a | n/a | P U O | W events only |
| LR-EXIT-6 | P S | P S | P S | dead and stalled stdout/stderr, pty hang-up |
| LR-EXIT-7 | U X | U X | U X | |
| LR-EXIT-8, -9, -10 | P X | P X | P X | -9 includes the core's own SIGTERM / CTRL_BREAK |
| LR-EXIT-11 | P F | P F | P F | address taken over; log rotation fails |
| LR-LIVE-1, -2 | P | P | P | frozen clock |
| LR-LIVE-3 | U | U | U | |
| LR-LIVE-4 | U P Z | U P Z | U P Z | M: CLOEXEC window row (NG-18); W: handle list; fds 0-2 closed (M/L) |
| LR-LIVE-5 | P | P | P | W: a suspended child is never read as exited |
| LR-LIVE-6 | P | P | n/a (the job is gwz's own handle) | |
| LR-LIVE-7 | U P | U P | U P | W SSH: found at reuse (NG-21) |
| LR-PROC-1, -10, -12, -14 | U S | U S | U S | |
| LR-PROC-2, -8 | P K X | P K X | P K X | per class |
| LR-PROC-3 | U P Z | U P Z | U (handle pinning) | injected kill port; signal-recording sentinel |
| LR-PROC-4 | P S | P S | n/a (no foreign reaping of handles) | SIGCHLD `SIG_IGN` start |
| LR-PROC-5 | S P Z | S P Z | n/a | injected spawn port pauses the parent |
| LR-PROC-6 | P (pty) | P (pty) | P (console) | |
| LR-PROC-7, -11 | n/a | n/a | P | |
| LR-PROC-9, -13 | P | P | P | |
| LR-CLEAN-1 to -4, -9, -10 | U | U | U | |
| LR-CLEAN-5, -6 | U K X | U K X | U K X | file census; foreign libgit2 temp |
| LR-CLEAN-12 | U | U | U | stepped clock |
| LR-CLEAN-13 | K X | K X | K X | system temp census |
| LR-CLEAN-7 | K R | K R | K R | |
| LR-REC-1, -5, -7, -8, -12, -13 | K P | K P | K P | file-event cuts for libgit2/Git writes |
| LR-REC-2, -4 | U F | U F | U F | -2: read-only command reports, writer recovers |
| LR-REC-3 | F O E | F O E | F O E | M: flush semantics; W: directory and rename durability |
| LR-REC-6 | F | F | F O | ENOSPC, EIO; W sharing violation and replace semantics |
| LR-REC-9 | P K | P K | P K | cut at each push phase |
| LR-REC-10 | S U K | S U K | S U K | |
| LR-REC-11 | U O | U O | U O | tmpfs (L), W directory sync |
| LR-CRED-2, -4, -5, -7, -8, -9 | U | U | U | LR-CRED-4 also E on three platforms |
| LR-CRED-3, -6 | U P K | U P K | U P K | canary secret scan |
| LR-BND-1 | S U P | S U P | S U P | every own wait site classified; one row per native call class |
| LR-BND-2, -3, -4, -6, -9 | U | U | U | |
| LR-BND-5 | U | U | U E | loaded W host |
| LR-BND-7 | F | F | F | |
| LR-BND-8 | E R | E R | E R | SHOULD |
| LR-OBS-1, -2 | U P | U P | U P | |
| LR-OBS-3 to -7 | U | U | U | |
| LR-OBS-8 | U R | U R | U R | |
| LR-OBS-9 | U | U | U | SHOULD |
| LR-CONC-1, -2, -3, -4, -6 | P | P | P | real `git` concurrently; -2 also O |
| LR-CONC-5 | K | K | K | |
| LR-CONC-7 | S P | S P | S U | |
| LR-CONC-8 | U | U | U | |
| LR-EMB-1 | S P | S P | S P | |
| LR-EMB-2, -4 | U | U | U | |
| LR-EMB-3, -6, -9 | P | P | P | |
| LR-EMB-5 | K | K | K | |

## 9. Design implications (non-normative)

**This section is not a requirement and not a design.** It records the forward view the requirements force, so the next
design can be judged against whole classes of issue. Nothing here authorizes work. Mechanism names are illustrative.

### 9.1 Primitives each requirement group forces

| Group | Primitive it forces | Issue classes it removes | Instances it absorbs (evidence) |
|---|---|---|---|
| LR-OWN, LR-CLEAN-1..4, -10 | **P1. One scope tree per process.** Every node has `cancel()`, a bounded `close()` returning `Receipt{outcome, cleanup}`, and `retain(successor)`. Composition → host or session → operation or call → binding → exchange, child, temporary. | Unowned resources; silent drops at the bound; inconsistent close semantics per subsystem; serial bounds | Executor-and-retainer double refusal drops work (RP:51); dropped finish future "changes nothing" vs CT:266; `OperationCancel::cancel` does not seal (CT:264); domain close has no overall bound (CT:189-190); `HostContext::shutdown` disposes only the supervisor; `Service` has no `Drop`; `DetachedWorkers` as a list of `Weak` |
| LR-PROC, LR-LIVE L1/L2 | **P2. One child supervisor that is the real parent of every child.** It has closed lifetime classes (K/W/F/S/D), pinned identities (zombie, handle, pidfd), and one cover: B's reaper on M/L, jobs created with the child on W. Every spawn goes through it, including Git CLI, helpers, `ssh`, the SSPI worker, the pager and `forall`. | Orphans after SIGKILL; kills of reused IDs; foreign-reaper races; signals landing in the fork window; handle and descriptor inheritance; per-site kill logic | Unix helper group orphaned on gwz's death (`process_tree.rs`); `kill_on_drop` not run on SIGKILL; tokio SIGCHLD driver; SSPI 10 ms monitor loop; `owner.rs` 2 ms drain poll; `CLEANUP_GRACE` changing outcomes (f9); W `bInheritHandles` hangs (CS S-P2-2); suspended orphan (R-B14) |
| LR-REC, LR-CLEAN-5, LR-OBS-4 | **P3. A per-workspace intent journal with one recovery path.** Before any multi-file, Git-locking or temp-creating sequence: a durable intent (operation, sequence kind, member, targets, temporary names, from and to commits). It is retired durably after. One recovery point at workspace resolution reads open intents first and shape recognizers second, and recovers or reports. | Per-feature markers and recognizers; pattern sweeps through shared directories; remedies gwz itself refuses; fetch-only reports; unsynced markers; torn checkouts committed unseen | D r1 M1 (sweep table), M3 (fetch-only), M6, M7 marker and its review findings (DS S-P2-1, S-P2-2, S-P2-5; DS:62); `durable_write` pair leaks; refcopy temps in the lane tree; `read_remote_file` probe clones. Merge records and `creating`/`disposing` rows remain as specialized intents |
| LR-CAN | **P4. One cancellation semantics across the carrier.** One token per scope. `operation.cancel` is that token's wire form. Carrier loss is `cancel(scope)` plus revoke. Cancels latch before their target exists. | Lost cancels; cause inversion; served/in-process divergence; per-form first-interrupt bounds | gwz-py cancel before registration (A:269-273); `ServedCancel` enumeration gap (CS S-P3-3); binding failure reported instead of `Cancelled` (C r1); AM's 60 s served bound vs A's 5 s |
| LR-LIVE | **P5. One liveness rule per link** (the §5.4 table as a design artifact). Each link is realized by an OS event on descriptors that P2 or the carrier owns exclusively. | Polling; undetected idle death; EOF delayed by inheritance; stop mistaken for death | Session supervisor 20 ms poll; carrier link 5 ms park; macOS CLOEXEC window (R-B15); stdio Ctrl-Z stall (CS S-P3-2) |
| LR-EXIT-6, LR-OBS | **P6. One output owner and one final-document claim**, per process (client and server). | Print panics; exit blocked by a consumer; double or missing final documents | `print!` sites (AM:150-151); server `eprintln!` panic under `2>&1 \| tee` (CS S-P2-3) |
| LR-EXIT-1..5, LR-CAN-5 | **P7. One interrupt source with staged Cancel/Force, and one safe-point gate** (A's module and `WriteGate`), used by every command and every serving end. | Mid-write exits; per-command interrupt behaviour; a server Force that waits out `close_wait` | A's design; C r1 server shutdown Force gap (CS S-P2-1) |
| LR-BND | **P8. One bounds table with owner timers, and a classification of every wait site** (bounded, cancellable, U1-U3). | Silent unbounded waits; extended deadlines | Member-lock wait with no timeout; settling exchange without deadline (CT:612-618; code `instance/owner.rs:195-207`); factory create without construction bound (CT:300) |
| LR-TEST-7 | **P9. Residue census as a test primitive.** | "Turds" found by users instead of tests | - |

### 9.2 Existing mechanisms: survive, collapse, or remain

| Mechanism | Fate under P1-P9 |
|---|---|
| `CleanupDomain` and retainer; `HostOwner` seal and revocation; admission and registration linearization (CT) | **Survive** as P1's host-level nodes |
| Session gate (Live/Cancelled/Revoked), call records, close steps 1-9 (SD) | **Survive** as P1's session-level nodes, with P4's token |
| A's interrupt module, output writer, `WriteGate` | **Survive** as P6 and P7 |
| B's registry, reaper and W jobs; the spawn-gate semaphore | **Survive** as P2. Once every spawn carries an explicit handle list, the W spawn gate may narrow to third-party spawns (NG-16) |
| Carrier `PeerGone`, `Stalled`; byte-stream EOF; idle watch | **Survive** as P5 links L3-L5 and L7 |
| Git lock protocol; advisory workspace and family locks; `durable_write`; merge record and action-journal contracts; family `creating`/`disposing` rows | **Survive**. The last two become specialized P3 intents |
| Checked-artifact quarantine; catalog `recover_or_create`; admission `resume_or_admit` | **Survive** as listed authorities (LR-REC-12), reached through P3's recovery point or under their own named trigger |
| Helper containment (`process_tree.rs` group or job, `owner.rs` reap and drain polling, `CLEANUP_GRACE` path) | **Collapse** into P2 |
| gwz-sspi 10 ms reaper loop (the job stays) | **Collapse** into P2 and P5 |
| Session-host supervisor 20 ms poll; `DetachedWorkers` `Weak` list | **Collapse** into P1 retained nodes |
| Per-operation `with_local_transport_native*` entries | **Collapse** into P1/P4 (already planned: step 3.3) |
| D r1 M1, M3, M6, M7; AM's interrupted-operation record | **Collapse** into P3 |
| `ServedCancel` enumeration | **Collapse** into P4 |
| AM's stdio watchdog `_exit` | **Not revived**. P5 plus LR-EXIT-9 replace it; OQ-5 decides the bound |

### 9.3 Residuals: which disappear and which map to Tier C

| Outcome if P1-P9 hold | Residuals |
|---|---|
| **Disappear** (the primitive removes the cause) | R-D1, R-D2, R-D3 (temporaries named in intents; one recovery path). R-B14 (job at creation, as the helper and SSPI already do). R-B15 and macOS R-B3 (one spawn primitive). R-C8 and the handle hangs (explicit handle lists). R-C2 (served write frames). R13 (P6 claim plus a defined stream end, subject to OD-D8). The C r1 review gaps S-P2-1, S-P2-3, S-P3-3 and S-P3-5 |
| **Disappear only if the design proves ownership** | R4: libgit2 names its temporaries inside a call, so no intent names them in advance, and a non-gwz libgit2 writer may share the `.git`. They disappear only if the design gives gwz a proof that excludes every other writer (for example a gwz-chosen temp directory); otherwise they remain reported (LR-CLEAN-5) |
| **Disappear if the operator chooses** | R6's cut removal and R7: P3 lets the next command finish or roll back (OQ-1). R-B5 / E6: native-route helpers under P2, or the route removed (OQ-3). R10, R17, R-B13: once step 3.3 leaves no hostless route (NG-15) |
| **Remain as Tier C**, recognized by P3 or by shape | R-B4 and R-B18 (W child at gwz's death, OQ-2); R14 and R11 (further interrupt); R9, R15, R-C10 (OS-forced); R-C5 (server crash); AM R1 (torn file on SIGKILL); R16 (probe clone, LR-CLEAN-13); R-B1, R-B2, R-B20 (cover failures) |
| **Remain as non-goals** | NG-1 to NG-25 |

## 10. Traceability

### 10.1 Operator decisions → requirements

| Decision | Covered by |
|---|---|
| OQ2 reversed (end-to-end cancel; each side detects death; no orphan, stale lock or partial state) | LR-CAN-2, LR-PROC-2, LR-LIVE-1, LR-REC-1, LR-REC-10, LR-CLEAN-5, LR-CLEAN-6, LR-TEST-7; the tiers of §4 |
| Resolution (1) (Force kills at once; file-granular safe points; named recognizer for every Tier C residue) | LR-CAN-5, LR-EXIT-3, LR-REC-1, LR-REC-5; Tier C definition |
| Resolution (2) (forked reaper shared by gwz-cli and gwz-py; canceller before wait; fork-without-exec constraint) | LR-OWN-3, LR-OWN-7, LR-EMB-2, LR-EMB-8, NG-7 |
| Resolution (3); OD-C9 (step placement) | out of scope (placement); OQ-19 |
| Decision (i) Force never exits mid-write | LR-EXIT-3, LR-EXIT-8, LR-BND-1 (U1); O-forced |
| Decision (ii) commit/tag, hooks and signers finish | LR-CAN-11, LR-PROC-1 (W), LR-BND-1 (U2), NG-14 |
| Decision (iii): A O1; OD-C7 (a)-(d); OD-B2 (a); OD-B3 | LR-OWN-2; NG-5, NG-2, NG-6; NG-15; NG-17 |
| O4 (a); O5; EXIT confirmation | LR-EXIT-1, LR-EXIT-4; LR-EXIT-2, NG-5; LR-EXIT-4 |
| OD-C1 one handler site | LR-OWN-2, LR-EMB-1 |
| OD-C2 (a) forked reaper, jobs on W | LR-PROC-2, LR-PROC-7; OQ-27; §9 P2 |
| OD-C3 (a) Git `*.lock` reported, never removed | LR-CLEAN-6, LR-CONC-1, NG-13 |
| OD-C4 exit 130, JSON marking | LR-EXIT-4, LR-OBS-3 |
| OD-C5 (a) no keepalive | NG-4; OQ-4 |
| OD-C6 pager and `forall` never killed | LR-CAN-10, LR-PROC-1 (F), LR-BND-1 (U3) |
| OD-C7 (e) stdio watchdog (never decided) | LR-EXIT-9; OQ-5 |
| OD-C8 (a) native-route helpers; (b) stdio outlive (never decided) | LR-PROC-8, OQ-3; LR-EXIT-9, OQ-5 |
| OD-B1 (a) commit/tag in own group | LR-CAN-11, LR-PROC-6, NG-14 |
| OD-B4 W group signal at gwz's death (open) | LR-PROC-2 (W clause); OQ-2; OQ-27 (W job close) |
| OQ4 (d) redirect after challenge | LR-CRED-4 |
| TR1.6 OQ3 (b), OQ5 (a), OQ6 (a), OQ7 (1) | LR-PROC-8, LR-BND-3, LR-BND-4 |
| Goal 1 credential-free core; R7 | LR-OWN-6, LR-CRED-1, LR-CRED-3 |
| Goal 2 designed for later forms (persistent client, networked core) | LR-OWN-12; LR-CAN-1 (every form); OQ-19 |
| Goal 3 one connection per host across repositories and phases; R1 | LR-OWN-11, LR-OWN-5 |
| Goal 4 Windows a full peer | LR-TEST-11; the W cells of §8; NG-21 (approved parity row) |
| Goal 5 no further work on the old architecture (per-operation runtimes retired at step 3.3) | LR-OWN-13, NG-15 |
| Goal 6 first proof: two processes on one machine, three platforms | LR-TEST-11 (cross-process rows first in that form) |
| Goal 7 the core is less trusted; it never extends a connection's or credential's life or reach | LR-CRED-9, LR-CRED-1, LR-CRED-5 |
| R2, R6 | LR-EMB-7, LR-CLEAN-11 |
| R3 | LR-OWN-5, LR-CAN-6, LR-CAN-12 |
| R4 | LR-CRED-7, LR-PROC-14 |
| R5 | LR-CAN-9, LR-OBS-1, LR-OWN-4, LR-BND-1 |
| D-A the contract governs owner, pool and operation lifetimes | LR-OWN-1, LR-OWN-4, LR-OWN-5, LR-OWN-11, LR-CLEAN-1..4; §2 R-CONN |
| D-B possession proof at cross-binding lease | LR-CRED-5 |
| D-D local socket/pipe or `ssh` stdio carrier | §5.4 links L3-L5 |
| D-E process-wide spawn gate, third-party residual | LR-OWN-2 (b), §2 R-GATE, NG-16 |
| D-H (b) allowed tuples, re-applied at every redirect hop, fails closed | LR-CRED-9 |
| D-I explicit start/stop, no auto-start | LR-OWN-10, NG-12 |
| D-L carrier loss terminal `NeverRegistered` | LR-CAN-4, LR-CAN-9 |
| D-M per-host default 32 | LR-BND-6 |
| D-N control frames overtake; readers never block | LR-CAN-8 |
| D-O no Python-hosted server | §1 PYM row |
| D-P one operation per call; admission 4 s < 5 s | B-ADMIT, B-BOOT; Phase 7 forms kept (LR-OWN-12) |
| D-R saturation starvation recorded | NG-19; LR-BND-4 (typed `AllocationTimeout`) |
| D-S receipt watchdog records the process tree first | Tooling, not product; applied by analogy in LR-TEST-2's hang guard |
| D-C, D-F, D-G, D-J, D-K, D-Q | Not lifetime-relevant (crate placement, a 1.0.17 config-parse fix, phase authorizations, publication order, capability map, a document-cap exception) |
| D-T, D-U | Not lifetime-relevant; instances of "surface structural debt" (MEM), as is OQ-22 |
| TR18 D26 gwz-py async `call()` at exit (1.1.0 only) | NG-20 |
| TR18 D5 (b) Pageant confirmation wait in the Network phase, clocks run | LR-BND-2; §1 AGT |
| TR18 D14/D32/D44 W KEX retry on a new TCP connection and session; a failed session never reused | LR-CLEAN-4 |
| TR18 D18 OS connect `TimedOut`/`AddrNotAvailable` final | LR-BND-4 |
| TR18 D31 gwz-sspi launch guard holds the spawn gate | §2 R-GATE; LR-PROC-11 |
| SessionLimits Q1 (a) | B-CLOSEW |
| IL (idle loss, 2026-10-07): "before any request byte"; SSH idle watch Unix-only | LR-LIVE-7; NG-21 |
| SBC SSH background close (2026-10-07, revision 3 2026-10-08): push keeps its EOF wait; close bound; "wait briefly" 250 ms | LR-CLEAN-12, LR-CLEAN-4; B-MIDCLOSE |
| ACD adaptive concurrency: "Park, give up after 2"; 30 s retest; OQ5 memory 10 s; OQ3 no member re-run | LR-BND-9; B-RETEST, B-LIMITMEM; LR-OWN-8; §2 R-CONN |
| CP:268-270 connect clock pauses during a reported local wait | LR-BND-2; OQ-21 |
| OD12 SSH remote server form in scope | §1 CORE; LR-PROC-9; link L5 |
| SD-F8 session `cancellation` field | LR-CAN-1, LR-CAN-8 |
| Standing rules: no globals; `cfg`; no production code in `tests/`; Windows parity | LR-OWN-2, LR-TEST-4, LR-TEST-8, LR-TEST-11, NG-21 |
| Standing rule: small crates in a DAG | Not a lifetime guarantee. It constrains §9's primitives (each a crate that builds and tests alone; gwz-transport never depends on gwz-core); no requirement |
| MEM no native-route fallbacks (parity over a safety bound, hazard stated) | NG-15; OQ-3 (b) |
| MEM release-scoped decisions; remove dead code now | LR-OWN-13, LR-OWN-12; OQ-19 |
| MEM surface structural debt | LR-OWN-2 (recorded debt); OQ-22 |

### 10.2 Design A residuals

| Residual | Covered by | Tier |
|---|---|---|
| R1 partial checkout at Force | LR-REC-5, LR-REC-7 | O-forced |
| R2 `creating` lane, partial import | LR-REC-5 (existing rows) | O-forced |
| R3 unreachable loose objects | LR-REC-5 (harmless; `gc`) | O-forced |
| R4 libgit2 temporary pack | LR-CLEAN-5, LR-CLEAN-6, LR-REC-2 | C |
| R5 unknown remote effect | LR-REC-9, LR-OBS-1, NG-10 | O / C |
| R6 partial clone; cut removal | LR-REC-8, LR-REC-13; OQ-1 | O-forced / C |
| R7 earlier members done; materialize/init join | LR-REC-1, LR-REC-13, LR-OBS-1 (`Partial`); OQ-1 | O |
| R8 unbounded waits; panic on stalled stderr | NG-14, LR-BND-1; LR-EXIT-6 (panic part, OD-D4) | O (liveness) |
| R9 W OS ends the process; `user32` | NG-2 | C |
| R10 hostless routes; `EINTR` | NG-15 | O |
| R11 gwz-py | NG-6, NG-20 | C |
| R12 `hook`, `claude-code-setup` default death | LR-EXIT-1; OQ-13 | - |
| R13 no complete final document | LR-OBS-3 (OD-D8) | O-forced |
| R14 further interrupt; SIGQUIT; in-pager | NG-5; OQ-10 | C |
| R15 gwz-py CLI signals and W events | NG-6 | C |
| R16 probe clone in system temp | LR-CLEAN-13 | C |
| R17 registry-less commit/tag | NG-15 | O |
| R18 complete pack and `shallow`, refs old | LR-REC-5, LR-REC-10 (enumerated harmless state) | O-forced |
| R-A1 W caller script continues | NG-11 | - |

### 10.3 Design B residuals

| Residual | Covered by | Residual | Covered by |
|---|---|---|---|
| R-B1 reused-ID kill | NG-8, LR-PROC-3 | R-B11 background server | NG-12 |
| R-B2 cover killed or stopped | NG-17, LR-LIVE-6, E27b, OQ-23 | R-B12 hook's background job | NG-1 |
| R-B3 fork / macOS window | NG-7, NG-18 | R-B13 registry-less children | NG-15 |
| R-B4 W child at gwz's death | Tier C, LR-REC-1, OQ-2 | R-B14 W suspended orphan | NG-17; §9.3 disappears |
| R-B5 native-route helper | LR-PROC-8, NG-15 (interim), OQ-3 | R-B15 `OwnInput` EOF missed (M) | NG-18, LR-LIVE-4, LR-PROC-2 |
| R-B6 successful helper's leftovers | NG-1 | R-B16 cover fork refused | NG-17, LR-PROC-10 |
| R-B7 other programs' processes | NG-1 | R-B17 gwz-py close mid-commit loop | LR-EMB-3, LR-OBS-1 (OD-D14) |
| R-B8 pager/`forall` after SIGKILL | NG-22 | R-B18 under OD-B4 (b)/(c) | OQ-2 |
| R-B9 `gwz hook` utilities | LR-EXIT-1, OQ-13 | R-B19 per-spawn fork charge | NG-17 |
| R-B10 stopped, hidden or hung reader | NG-14, LR-PROC-6 | R-B20 `pthread_atfork` in cover | NG-17 |

### 10.4 Design C (r1) and D (r1) residuals; amendment exceptions

| Residual | Covered by | Residual | Covered by |
|---|---|---|---|
| R-C1 hung idle core | NG-4 | R-C8 W inherited handles | LR-PROC-7 |
| R-C2 detached worker torn (interim) | NG-23, OQ-24; LR-CAN-5 after 3.0g | R-C9 served push unknown effect | LR-REC-9, NG-10 |
| R-C3 stdio core outlives client | LR-EXIT-9, OQ-5 | R-C10 W OS-forced served end | NG-2 |
| R-C4 group SIGTERM kills `ssh`; W CTRL_BREAK to stdio | LR-CAN-9, LR-EXIT-9 (E05, E14); OQ-24 (C §9 routes it to the operator) | R-C11 server waits unbounded | NG-14 |
| R-C5 server crash residue | LR-CLEAN-8, LR-REC-1 (Tier C) | R-C12 `ssh`'s children | NG-1 |
| R-C6 remote core, idle network loss | NG-4 | R-C13 served W op cut between steps | LR-OBS-1, LR-REC-1; OQ-1 (served form of R7; C §9 routes it to the operator) |
| R-C7 interrupted `server start` | NG-12 | CS: Ctrl-Z stall; supervisors; echo | OQ-6; LR-BND-8; LR-PROC-6 |
| R-D1 temporaries outside the sweep set | LR-CLEAN-5, LR-REC-2 | AM E1, E5 | NG-1 |
| R-D2 later writer outside the inventory | LR-CLEAN-5 | AM E2 | LR-CAN-10 |
| R-D3 M3 report not in `--json`, `pull` | LR-REC-2, LR-OBS-4 | AM E3; E4; E6 | LR-EXIT-9 / OQ-5; NG-12; LR-PROC-8 / OQ-3 |
| D:133 lock error mapped elsewhere keeps libgit2's text (DC P3-8) | LR-OBS-7 (remedy still required); OQ-11 | F43 local-path remote residue | NG-24, OQ-25 |
| AM R1 torn file, Git lock after SIGKILL/power loss | Tier C; LR-REC-1, NG-3, NG-13 | AM R1a libgit2 temps after Force ends a download | LR-CLEAN-5, LR-CLEAN-6 (as A R4) |
| AM R1b OD-C7 (a)-(e) triggers | NG-5, NG-2, NG-6; OQ-5 | AM R2 native push upload not abortable | NG-15; LR-REC-9 |
| AM R3 native connect/negotiation/credential phases; libgit2 checkout | NG-15; LR-CAN-5 (checkout parks between files) | AM R4 ID reuse within the cover's reaction | NG-8, LR-PROC-3 |
| AM R6 fork without exec / macOS thread spawn delays the cover's EOF | NG-7, NG-18 | AM R7 cover copy-on-write cost | NG-17 (OD-B3 (a)) |
| AM R8 hung idle core | NG-4 | AM R9 cover dead alone; reaperless registries before 3.1/3.2/3.3 | E27, LR-LIVE-6, NG-17; NG-15 |
| AM R10 Force waits for a long local phase (`stash push` reset, Git CLI child) | NG-14, LR-BND-1 (U1, U2) | AM R11 Git CLI child in its own group stops on SIGTTIN | NG-14, LR-PROC-6 |

### 10.5 D's proposed operator decisions (OD-D1 to OD-D18, all open)

| OD | Here | OD | Here | OD | Here |
|---|---|---|---|---|---|
| OD-D1 | OQ-2, OQ-20 | OD-D7 | OQ-13 | OD-D13 | NG-17 |
| OD-D2 | OQ-1 | OD-D8 | LR-OBS-3, OQ-11 | OD-D14 | LR-EMB-3, OQ-11 |
| OD-D3 | OQ-10 | OD-D9 | NG-2, NG-6, NG-11, OQ-26 | OD-D15 | NG-17 |
| OD-D4 | LR-EXIT-6, OQ-11 | OD-D10 | NG-8 | OD-D16 | LR-REC-7, OQ-8 |
| OD-D5 | NG-15, OQ-11 | OD-D11 | NG-17, OQ-23 | OD-D17 | NG-18, OQ-12 |
| OD-D6 | NG-6 | OD-D12 | OQ-3 | OD-D18 | NG-1, OQ-9 |

## 11. Open questions for the operator

| ID | Question | Options (no recommendation is implied by order) | Bears on |
|---|---|---|---|
| OQ-1 | Does recorded partial state at a Force safe point (O-forced) meet D-V's "no partial state", or must each recorded sequence be completed or rolled back (by Force or by the next command)? | (a) recorded and reported suffices; (b) the next command completes or rolls back from the intent; (c) Force rolls back before exit | §4, LR-REC-5, R1, R2, R6, R7, R-C13 (served form of R7), OD-D2 |
| OQ-2 | What ends a W child (commit/tag, hooks, signers) when gwz dies? | (a) SIGKILL (B as written); (b) SIGTERM then SIGCONT (B's and D's recommendation on Unix); (c) let it finish | LR-PROC-2, R-B4, R-B18, OD-B4/OD-D1 |
| OQ-3 | Native-route helpers outside the registry. OQ2 says "every helper process it started", and LR-PROC-8 is written to that. | (a) cover them before 1.1.0, by removing the route (step 3.0f) or by owning the spawn: git2-rs starts the helper with std `Command` inside one blocking call, so covering it in place needs a git2-rs change; (b) accept for 1.1.0 as an explicit, separate relaxation of OQ2 (DS S-P2-6 asks it not be bundled), with the hazard stated as the operator's standing rule asks (MEM `no-native-route-fallbacks`: parity over a safety bound, hazard in the notes) | LR-PROC-8, NG-15, R-B5, OD-C8 (a), OD-D12 |
| OQ-4 | No keepalive: for this release only, or as a standing limit? It is the one link where LR-LIVE-2 cannot hold. | (a) 1.1.0 only; (b) standing | NG-4, R-C1, R-C6 |
| OQ-5 | May a local stdio core whose client died run on until its local writes reach a safe point (LR-EXIT-9 as written: unbounded only while writing)? Or must it have a hard bound that may tear? | (a) as written; (b) hard bound `close_wait` + N, accepting a tear | LR-EXIT-9, R-C3, OD-C7 (e), OD-C8 (b) |
| OQ-6 | Ctrl-Z of a client using a local stdio core: should the core stop too, or is the 60 s stall cancel accepted? | (a) the core stops with the client; (b) stall cancel accepted | E08, CS S-P3-2 |
| OQ-7 | Which filesystems carry the full LR-REC guarantee? Is W directory durability (no directory sync) accepted as degraded? | name the set; accept or reject the W degradation | NG-3, LR-REC-3, LR-REC-11 |
| OQ-8 | Confirm LR-REC-7 (a torn tree cannot be committed unseen) as MUST. | (a) MUST; (b) report only (D's OD-D16 (a)) | LR-REC-7, OD-D16 |
| OQ-9 | Confirm that `ssh`'s own children and a hook's daemons are not gwz's (NG-1). | confirm or narrow | NG-1, R-B12, R-C12, OD-D18 |
| OQ-10 | While only the pager runs, should Force's restore of the default disposition wait, so the third Ctrl-C inside `less` cannot tear? | (a) wait (D's recommendation); (b) as accepted (O5) | NG-5, R14, OD-D3 |
| OQ-11 | D asks for its recommended accepts "as one set". Decide them one by one against §6 instead? | per row | OD-D4, -D5, -D8, -D14 and the rest of §10.5 |
| OQ-12 | macOS `pipe`-then-CLOEXEC window: accept it (NG-18), or require one spawn primitive that owns every spawn in the process (LR-LIVE-4 as written)? | (a) accept; (b) require | NG-18, LR-LIVE-4, LR-PROC-2, R-B3, R-B15, OD-D17 |
| OQ-13 | Should `gwz hook` and `claude-code-setup` take the interrupt path like every other command? A excludes them; DS S-P2-4 and DC P2-1 dispute D's premise. | (a) every command (LR-EXIT-1 as written); (b) exclude, with their residue named | LR-EXIT-1, R12, R-B9, OD-D7 |
| OQ-14 | Should a W background server break away from a caller's kill-on-close job where the job allows it? | (a) break away; (b) die with the caller's job (Tier C) | NG-2, CS S-P3-4 |
| OQ-15 | Should supervisor windows (logind 90 s, `docker stop` 10 s) be a MUST for orderly stops with no U-wait? Should the `close_wait` default change so a server stop fits? | (a) SHOULD as written; (b) MUST with adjusted defaults | LR-BND-8 |
| OQ-16 | Confirm each **PROP** requirement (sourced only by review or code evidence). | per row | rows marked PROP |
| OQ-17 | Confirm the definition of "no polling" in LR-LIVE-1: deadline timers, a bounded backoff that closes an OS registration race (macOS kqueue `ESRCH`), and the server's 30 s address-integrity check are not polling of peer state. | confirm or tighten | LR-LIVE-1, B-LINK |
| OQ-18 | Confirm the closed list of things that may outlive their scope (LR-OWN-8), including the entries added in revision 2 (believed limit, SSH reaper thread, server log, unlocked lock files, `creating` rows and open merges, F programs at gwz's death). | confirm or amend | LR-OWN-8, LR-OBS-5 |
| OQ-19 | Release placement: which requirements gate 1.1.0, and which belong to 1.2.0 or Phase 7 forms (persistent client, networked core)? The standing rule says 1.1.0 decisions never remove later-release code. | per area | all; resolution (3); OD-C9 |
| OQ-20 | Are `exec`, `os._exit` and a foreign `exit()` in an embedding application accepted as Tier C events? D classifies them beyond resolution (1) and puts them to the operator inside OD-D1. | (a) accept as Tier C (LR-EMB-5 as written); (b) require gwz-py to detect them and report | LR-EMB-5, E24, OD-D1 |
| OQ-21 | The connect clock pauses during a reported local wait (CP:268-270). Confirm that the pause applies only to stall classification and never to the operation's allocation deadline (LR-BND-2). | (a) classifier only; (b) the deadline pauses too (LR-BND-2 gains a named exception) | LR-BND-2 |
| OQ-22 | LR-OWN-2's allow-list: confirm (b) the W spawn gate's named object (D-E) and (c) PYM's module-level composition object as exceptions. Decide gwz-transport's `static NEXT_POOL` (allow-listed debt, RP:50) under "surface structural debt". | per entry: accept, or remove the state | LR-OWN-2 |
| OQ-23 | A stopped reaper blocks spawns, and through the creation lock it blocks `close()`, `kill_killable()` and Force's child step without bound (F82), against decision (i)'s "at once". | (a) accept as a fourth unbounded wait, announced; (b) require that Force and close never wait on the creation lock; (c) treat a stalled lifeline as loss of cover after a bound | NG-17, E27b, LR-BND-1, OD-D11, R-B2 |
| OQ-24 | C §9 routes R-C4 (a session closed without cancel-and-wait after a group SIGTERM or W CTRL_BREAK) and R-C2's interim (a served detached write torn before 3.0g) to the operator. Accept them as compliant with OQ2, or require more? | R-C4: (a) compliant (E05, LR-EXIT-9 as written); (b) the stdio core and `ssh` must survive a group signal until cancel completes. R-C2: (a) accept NG-23 until 3.0g; (b) block served detached writes until 3.0g | E05, E14, LR-EXIT-9, NG-23, R-C2, R-C4 |
| OQ-25 | Residue outside the workspace: confirm NG-24 (a local-path remote's Git lock and temps after a Tier C push) and LR-CLEAN-13 (probe clones in system temp found by any later command). | per row: accept, or require a recognizer that names the outside path | NG-24, LR-CLEAN-13, F43, F57 |
| OQ-26 | The candidate W build loads `user32` (Pageant), so it never receives CTRL_LOGOFF or CTRL_SHUTDOWN (R9; OD-D9). | (a) accept (NG-2); (b) move the `user32` use out of the gwz process so the events arrive | LR-EXIT-1, LR-EXIT-5, NG-2, E16 |
| OQ-27 | On W, B's registry jobs end every member at close (M-B9), so gwz's death kills a stdio core (against LR-EXIT-9, E09 and C:123) and a W child (deciding OD-B4 as (a) on W while OQ-2 is open). | (a) S and W children in jobs whose close does not end them; (b) accept the kill for W only; (c) accept for both, with the tear recorded as Tier C | LR-PROC-7, LR-PROC-2, LR-EXIT-9, OQ-2 |
| OQ-28 | Values for bounds this document requires but no source fixes: B-SETTLE, B-FACTORY, B-COVER, B-EOF, B-RSS. | fix each from an E baseline before its row gates (as R3); or name a value now | LR-CLEAN-4, -11, LR-PROC-2, -13, LR-BND-3 |
| OQ-29 | Confirm NG-25: the remote core's view of client loss depends on `sshd`'s settings; a caller's death with no terminal produces no event; an interpreter killed without running code runs no atexit. | (a) accept; (b) narrow (for example require a carrier-level keepalive on L5, which reopens OD-C5) | NG-25, LR-LIVE-1, E17b, OQ-4 |
