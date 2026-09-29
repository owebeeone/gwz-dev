# GWZ core session crate map

Date: 2026-09-28. Status: **accepted 2026-09-28**, on GO for revision 1 from the architecture review ([round 2](GwzCoreSessionCrateMap-ReviewCode-1.md)) and the operator's three decisions (§8). That round's four P3 corrections were applied after GO and [confirmed](GwzCoreSessionCrateMap-ReviewCode-1a.md). The operator directed it ("ok on crates") after observing that the session plan builds a new subsystem inside gwz-core behind prose interfaces instead of small crates. It re-homes the code of the [session plan](GwzCoreSessionPlan.md)'s remaining steps. The plan's behaviour, limits, tests and phases carry over unchanged. One reviewer checks it on the architecture axis. Revision 1 answers that reviewer's round 1 ([report](GwzCoreSessionCrateMap-ReviewCode.md); §9). On acceptance, the map amends the text listed in §7.

## 1. Rules

- **Small crates, narrow APIs.** Every new gwz-core library follows the [library boundaries](GwzLocalCloneLibraryBoundaries.md) policy (LBT-001 to LBT-012), not only the local-clone libraries.
  - Each crate builds without git2, and its fast tests run alone, on fakes.
  - Suites that drive git2 stay in core's integration tests (LBT-010). They reach crate internals through each endpoint crate's `test-support` feature, which only core's dev dependencies enable, as with the local-clone contract crates' `contract-tests` feature. So the crates' public APIs do not grow for tests.
  - A crate is reviewed against its own API and README.
- **Roles and edges as today.** A crate is a contract, pure, implementation, integration or harness crate, and it may depend only on contract and pure crates.
  - gwz-core is the composition root: it wires implementations into integrations through the ports each integration owns.
  - gwz-transport, which has no dependencies and does no I/O, counts as a pure crate that the candidate crates may use. It never depends on a gwz-core crate, so the repositories form a DAG: gwz-core depends on gwz-transport, never the reverse.
- **No GWZ protocol in crates.** Crates carry frames and message bodies as bytes, and all encoding and decoding of GWZ's Taut-defined messages stays in gwz-core. That keeps gwz-core's "no shadow protocol" rule. The frame layer itself is contract §3's carrier format, implemented once: a tag byte, plus a length prefix on byte streams.
- **No globals.** No crate or adapter keeps global mutable state, and a thread-local counts as global state.
  - Unique numbers come from an `IdSource` that each context owns (§2).
  - The only exceptions are immutable data, caches of immutable data, and state that a named dependency imposes.
- **Secrets stay in core where they can.** The environment snapshot never enters a crate: core spawns helpers from it and hands the crate the child's pipes. Two crates handle credential material by their nature:
  - `gwz-ssh-endpoint` holds the private key text of explicit identities;
  - `gwz-https-endpoint` holds the tokens its helper returns.

  Those two APIs keep per-step dual review under the review granularity ruling.
- **Two classes of crate.**
  - **Ordinary crates** live in `gwz-core/crates/`. They are built in every build and published in lockstep with gwz-core on the internal crate line.
  - **Candidate crates** are built only by the candidate build until activation. They stay out of gwz-core's ordinary dependency graph and lock file, and they are published at activation with gwz-transport, before gwz-core 1.1.0.

## 2. Ordinary crates

| Crate | Role | Job | Public API, in outline | First-party dependencies |
|---|---|---|---|---|
| `gwz-ids` | pure | Unique numbers | `IdSource::new(prefix)`. Core draws each context's 64-bit prefix from the operating system's random source; tests pass fixed ones. `next() -> u64` is unique within the source. `unique()` pairs the prefix with the counter, so it is unique across contexts and processes unless two random prefixes collide. | none |
| `gwz-session-contract` | contract | Frames and channel traits | `Frame { tag, body }`, the tag registry, `MAX_FRAME_BYTES`, `FrameError`; `Lane`; `FrameSink::send(frame, lane)`, `FrameSource::recv()`; `Limits` | none |
| `gwz-session-channel` | implementation | Carrying frames | `pair(capacity)` gives a client end and a host end: two bounded queues with the control reserve, and a `send` that never blocks. `byte_stream(read, write)` adds the u32 length prefix and the 64 MiB cap. | contract |
| `gwz-session-host` | integration | Running sessions | `serve(host_end, session, ports)` and the `HostPorts` it consumes. It holds call controls and gates, the supervisor with `shutdown(bound)`, admission, workers, event rings, the operation table, cancellation, the workspace registry, member locks, the log registry and closure. | contract, ids |
| `gwz-server-policy` | pure | Server rules | `ServerAddress::parse` and the SSH form's argument vector; the must-match list and `compare`; `auto_key`; the OpenSSL configuration parser, over text; lock records; the handshake and lifecycle rules; log redaction | none |
| `gwz-server-os` | integration | Server platform code | The walks, the sandbox and peer checks, the per-user directory and files, locks, listeners (Unix sockets and named pipes), `connect` with listener checks, the launcher, the stdio moves, and reading the OpenSSL configuration files. It returns an owned `Conn: Read + Write` and exposes no raw handles. | policy |

Notes:
- **`HostPorts`,** which core implements, decodes a call's header (method, call ID and class), resolves the workspace, runs an operation, encodes replies and errors, and holds log spools. An unknown method is class W, as contract §5.1 requires.
- **Core's per-session data passes through the host crate opaquely,** as a type parameter. The environment snapshot therefore stays in core, under its CS1.5 and CS1.9 reviews.
- **Nesting without a thread-local:**
  - Crossing and revoking move onto the operation's controls, which are not `Clone`.
  - A crossing's closure and a cancel callback receive only a `GateScope`. It exposes session data and cannot cross or revoke.
  - `OperationGate` and `HandlerContext` stop being cloneable handles that can cross; a cloneable view keeps `state()` and the cancellation token.
  - As a backstop, each gate records the thread running its closure and panics on re-entry from that thread, instead of deadlocking on its `crossing` mutex.
  - What stays undetected: a path that smuggles another operation's controls into a closure. The step's review checks that no such path exists.
- **One supervisor on the session path.** CS3.5 moves `agent_job`'s hub into this crate's supervisor. The legacy path keeps its process-wide budgets as `debt` until CS6.5 removes them, as the plan's CS3.1 says.
- **Fast tests spawn no process** (LBT-010). `gwz-server-os`'s launcher, listener and walk fixtures run as a separate integration tier.
- **Windows type checks** become `cargo check -p <crate> --target x86_64-pc-windows-msvc`, since no crate pulls in git2.

## 3. What stays in gwz-core

gwz-core keeps, as adapters, everything that touches its generated protocol, its operations or git2, and the environment snapshot:
- **Protocol:**
  - the codecs for session calls, replies, errors, and handshake and control messages;
  - the `OperationResult` projection and `CleanupReport`;
  - taut-shape's log messages;
  - classifying each outgoing frame's lane;
  - reporting undecodable bodies and call-ID order.
- **Dispatch:**
  - the method registry and class table, as core's `HostPorts` implementation (CS1.6);
  - the routes (CS2.13 to CS2.16, CS3.10);
  - the lock-path crossings (CS2.8);
  - the legacy adapter (CS3.1);
  - child processes and credential fill (CS3.3, CS3.4);
  - timeouts and member locks (CS3.8, CS3.9).
- **The composite:** `HostContext`, `SessionContext` with the environment snapshot, `open` and `ClientChannel`. They stay re-exported at today's `gwz_core::session_host` paths, so the frozen API in `docs/RustApi.md` does not change.
- **Helper spawning.** Core spawns `gh`, and any other helper, from the requester's snapshot. It implements the HTTPS crate's `HelperSpawn` port, returning the child's pipes and a kill handle.
- **Server glue:** `serve_session`, the handshake's contents, the host's own values after git2 starts, the OpenSSL library's identity, and the native-route check (CS8.3).
- **Transport adapters,** in the candidate build only:
  - request mapping (`transport_host/mod.rs`, `request.rs`, `local_command.rs`, and the new `session_entry.rs`);
  - deriving the endpoint configuration from the snapshot;
  - binding the candidate crates' ports;
  - git2's subtransports and binding (`ssh_remote`, `https_remote`, `transport_binding`, `transport_observations`);
  - the git2-driven SSH and HTTPS suites.
- **libgit2's timeout:** the one global a dependency imposes, set once at startup.

The drivers keep server selection, the `server` subcommands, rendering and the server design's §17 message text.

## 4. Candidate crates

`git/endpoint` has 11,786 production lines. They depend on gwz-core only through `HttpsOpenFailure`, and only `ssh_remote` and `https_remote` use git2. `tests/transport_ssh` already builds 23 of its files outside gwz-core. The target:

| Crate | Role | Job; what it takes over | First-party dependencies |
|---|---|---|---|
| `gwz-endpoint-contract` | contract | `BlockingStream`, `GitService` and `HttpsOpenFailure`; the `Engine` port with its `Outbound` and `EndpointError` types, which both engines share; the `Reservations`, `InstancePort` and `HelperSpawn` ports | gwz-transport |
| `gwz-endpoint-policy` | pure | The canonical, secret-free endpoint configuration, with its encoding and digest (CS3.2's logic; the derivation stays in core). The reservation authority: total and per-host ceilings, and retiring the oldest idle entry when full (`shared_reservation.rs`; CS7.21, and the reservation part of CS7.10). | contract, gwz-transport |
| `gwz-endpoint-registry` | integration | Selects or builds an instance by configuration through `InstancePort`; the 16-instance bound with LRU; fault exclusion; disposal within the deadline (CS3.7's registry, CS7.11, CS7.12, CS7.26) | contract, policy, ids |
| `gwz-endpoint-instance` | integration | An instance and its isolated bindings, over the `Engine` and `Reservations` ports: `transport_host/session.rs` and its driver (CS7.9, CS7.23, and the instance's parts of CS7.10 and CS7.13) | contract, policy, ids |
| `gwz-ssh-endpoint` | implementation | Implements `Engine` for SSH: connections, agent authentication, explicit keys (credential material), the possession proof and revalidation. It takes the SSH files except `ssh_remote`, with `placement_endpoint.rs` as its `Engine` (CS7.7, CS7.17 to CS7.20, and its parts of CS7.10, CS7.13 and CS7.22). | contract, policy |
| `gwz-https-endpoint` | implementation | Implements `Engine` for HTTPS: connections and the `gh` helper through `HelperSpawn`, whose tokens are credential material. It takes the HTTPS files except `https_remote`, with the HTTPS bridge (`transport_host/https_endpoint.rs`) as its `Engine` (CS3.6, CS7.8, CS7.15, CS7.16, and its parts of CS7.13, CS7.20 and CS7.22). | contract, policy |

gwz-core binds the ports:
- `InstancePort` to the instance crate;
- `Engine` to the two endpoint crates;
- `Reservations` to the policy crate's authority;
- `HelperSpawn` to its snapshot-based spawner.

gwz-transport keeps its protocol, codec, mux, stream and pool. The pool also takes T1 to T5 (CS7.2 to CS7.6), and it receives its pool ID from its host at construction, which replaces `NEXT_POOL`. gwz-transport keeps no first-party dependency.

- **Where these crates live:** a second workspace in the gwz-core repository, `gwz-core/candidate-crates/`, excluded from gwz-core's own workspace. The operator decided this on 2026-09-28 (§8). Each crate stays in one repository, and one commit, with its core adapter, and clear of gwz-transport's other lane. At activation the crates become ordinary and can move into `crates/`. The workspace needs three tooling changes:
  - the boundary gate gets that workspace as a second root, with its own inventory;
  - `tests/transport_backend/prepare.py` links that workspace into the candidate root the way it links `crates/`, so every crate reaches `gwz-ids` through one path;
  - the crate-version and release tooling takes them in at activation.
- **The protocol they may use.** These crates may use gwz-transport's own generated protocol (the transport's owner schema), and never gwz-core's.
- **Cuts follow the reuse design's §2 table, not today's files.** One mutex covers every table in `session/driver.rs`, and `https_worker::Client` mixes instance state with binding state. CS7.1 becomes the extraction, in three commits:
  1. a movement-only commit that moves each file with its unit tests;
  2. a commit that points core's integration suites at the crates' `test-support` features;
  3. splitting the four files over 1,000 lines by responsibility.
- **Names are settled at the extraction.** "Binding", "EndpointConfig" and "Owner" each name two different things today.
- **The counters go.** `SERIAL`, `NEXT_WORKER`, `NEXT_ID` and `NEXT_SESSION` take IDs from the host context's `IdSource`. The HTTPS engine receives its source at construction, replacing its call to `session::unique()`. `https_local`'s fixture leaves product code.
- **Timing.** These crates wait for TR3.1, since the candidate build is broken until it lands, and for the decision on where they live.

## 5. Where the remaining steps go

CS1.8 is done.

| Steps | Code goes to |
|---|---|
| CS1.1, CS1.10 (schema) | core, unchanged |
| CS1.2, CS1.3 | `gwz-session-contract` and `gwz-session-channel`; lane classification in core |
| CS2.2 to CS2.7, CS2.9, CS2.11, CS2.12, CS3.5 | `gwz-session-host` |
| CS1.6, CS2.1, CS2.8, CS2.10, CS2.13 to CS2.16, CS3.1, CS3.3, CS3.4, CS3.8 to CS3.10, CS6.5, CS7.14, CS7.24, CS8.3 | core adapters |
| CS3.2 | core (derivation) and `gwz-endpoint-policy` |
| CS3.6, CS3.7 | `gwz-https-endpoint`, with core's spawner; core's `session_entry` and `gwz-endpoint-registry` |
| CS5.1, CS5.4 | core (the example host, `serve_session`); the handshake rules in `gwz-server-policy` |
| CS7.1 | the transport extraction |
| CS7.2 to CS7.6 | gwz-transport's pool, including the host-given pool ID |
| CS7.7 to CS7.13, CS7.15 to CS7.23, CS7.26 | the candidate crates (§4) |
| CS8.1, CS8.5, CS8.11, CS8.16 | `gwz-server-policy` |
| CS8.2 | `gwz-server-policy` (the parser, `auto_key`) and `gwz-server-os` (reading the files) |
| CS8.6 to CS8.10, CS8.12 to CS8.15, CS8.17 to CS8.19 | `gwz-server-os`, with core glue |
| CS4.1 to CS4.9, CS5.2, CS6.1 to CS6.4, CS6.6, CS6.7, CS7.25, CS8.20 to CS8.24, CS8.26 to CS8.29 | the drivers, unchanged |
| CS3.11, CS5.3, CS7.27, CS8.4, CS8.25, CS8.30 to CS8.32 | CI and evidence, unchanged |

Phases, milestones and dependencies stay as the plan has them. Under the [review granularity ruling](GwzProcessOptimization.md) (§8), each phase is reviewed once, and a crate is checked against its own API and README. Wire-format and secret-handling steps keep their per-step dual review.

## 6. First steps

They are built back to back and reviewed once, as a group:
1. **Checkers and tooling.**
   - Widen the boundary inventory from the local-clone crates to every gwz-core library.
   - Make `check_process_globals.py` refuse a counter, flag or thread-local marked `permanent` unless the entry names the dependency that imposes it.
   - Reclassify the eight counters and `CROSSING` as `debt`, each with an owner.
   - Grow `check_crate_versions.py`'s `EXPECTED_CRATES` (14 today) and the release bump's crate list with each new crate.
2. **`gwz-ids`,** replacing the four temp-name counters.
   - `verified_write.rs`, the merge store's `rewrite.rs` and `artifact/encoding.rs` take their operation's source. Until CS3.1's per-call context exists, each legacy entry that reaches them creates one per call.
   - Family-store's `publish.rs` takes a source from its store session, which core seeds. It also opens its temporary exclusively and retries, instead of truncating it with `File::create`.
   - `write_atomic_verified`'s pinned seam (`NEUTRAL_RAW_WRITE_FLOOR` in `check_checked_artifact_boundaries.py`) is re-pinned in the same commit.
   - Callers keep their current temp-name markers; family-store's cleanup looks for `.tmp.`.
3. **`gwz-session-contract` and `gwz-session-channel`,** which are CS1.2 and CS1.3. Frames carry bytes, so these no longer wait on CS1.1 or TR3.1. CS1.3's interop test uses checked-in vectors, since `taut-shape-tool` is a binary-only crate.
4. **`gwz-session-host`.**
   - The gate, with §2's nesting design, moves out of core together with the limits and the supervisor, using crate-local error types; `CROSSING` goes.
   - Core's `gwz_core::session_host` paths stay as re-exports.
   - Phase 2's generic machinery then lands in this crate.
5. **crates.io.** Thirteen names need their one-time bootstrap ([CrateBootstrapHowTo.md](CrateBootstrapHowTo.md)): the six ordinary crates, the six candidate crates and gwz-transport.
   - They are registered together, just before release preparation begins (§8), with the operator's account.
   - Steps 1 to 4 need none of them: only the release's publish job (`scripts/publish_crates.py`) reads crates.io.

## 7. Text this map amends on acceptance

- **The contract:**
  - O9, and §5.7's row that keeps ID counters as permanent allowlist entries;
  - §3 and §9: `send` takes a lane, and core classifies each frame;
  - §5.1 and §5.2: the class table and dispatch are core's implementation of the host crate's ports;
  - §5.6: `HostContext` stays a core composite.
- **gwz-core's `dev-docs/GWZDesign.md`:** its "Core session host" paragraphs that pair with those contract sections. They are authoritative under gwz-core's `AGENTS.md`.
- **The server design:**
  - §2's "exists once, in core" and its placement rows;
  - §4's placement of the walk, the sandbox rule, the hosts and the launcher.

  All of these move to `gwz-server-policy` and `gwz-server-os`, which both CLIs reach through gwz-core, so each still exists once.
- **The session plan:**
  - §3.0's placement sentence and its ratchet rules, including the exception that lets a marker, flag or counter be `permanent`;
  - §5.4's list of entries that stay `permanent`;
  - §4's file-ownership list;
  - the file lists of the steps that §5 moves, including CS3.5's;
  - CS7.1, which becomes the extraction;
  - CS7.2 to CS7.6, which gain the host-given pool ID.

  The plan allows only status edits after acceptance (its §7), so this map carries these changes until the plan's next revision.
- **The reuse design:** §14's gwz-transport pool changes gain a step for the host-given pool ID, beside T1 to T5.
- **The library boundaries:**
  - §1's scope sentence covers every new gwz-core library;
  - §2's rule on generated messages lets the candidate crates use gwz-transport's own protocol, and gwz-transport counts as a pure edge;
  - gwz-core's `AGENTS.md` points at the policy.
- **The process-globals allowlist:** its definition of `permanent`, as §1's globals rule states it.

How these land: the library boundaries and gwz-core's `AGENTS.md` changed on acceptance. The allowlist's definition changes in §6's first step. The contract, `GWZDesign.md`, the server design and the reuse design change in the step that moves their code, or at their next revision if that comes first. Until then, this map controls where they differ.

## 8. Operator decisions

1. **Where the candidate crates live: decided 2026-09-28, gwz-core,** as the second workspace `gwz-core/candidate-crates/`. The reviews call them the transport crates, in `gwz-core/transport/`; both were renamed because that folder reads as the gwz-transport repository. The comparison that decided it:
   - The repositories stay a DAG. gwz-core depends on gwz-transport, and gwz-transport on nothing. In gwz-transport, these crates would need `gwz-ids` from gwz-core, a cycle between the two repositories.
   - A port and its core implementation land in one commit, instead of two repositories plus a re-pin of `reconciled_commit`.
   - The dependencies (`gwz-ids` into the candidate crates, and those crates into gwz-core) stay inside one repository and one release script.
   - gwz-transport keeps its charter: independent of GWZ core, no dependencies and no I/O.
2. **The rules in §1: adopted 2026-09-28,** including that no counter, flag or thread-local is ever `permanent`.
3. **When to bootstrap the thirteen crates.io names: decided 2026-09-28,** together, just before release preparation begins.

## 9. Review round 1

The architecture review of revision 0 (`eac8e018…`) was NO-GO, with P2 ×3 and P3 ×6. Every finding is answered:

| Finding | Disposition |
|---|---|
| P2-1: forbidden transport edges | A transport contract crate, a dependency column, gwz-transport as a pure edge, and core binding the ports (§1, §4) |
| P2-2: secrets leave core | The snapshot stays in core behind `HelperSpawn`; the two endpoint crates' credential material is named and kept under per-step dual review (§1, §3, §4) |
| P2-3: `NEXT_POOL` | The pool takes its ID from its host (§4, CS7.2 to CS7.6) |
| P3-1: `IdSource` uniqueness | The prefix is random for each context; each source's owner is named; family-store creates exclusively; the seam is re-pinned (§2, §6) |
| P3-2: `CROSSING` | A capability design with a per-gate backstop; what stays undetected is stated (§2) |
| P3-3: omitted text | §7 lists it. The supervisor note now matches CS3.1's legacy budgets (§2). |
| P3-4: roles | `gwz-server-os` is an integration crate. The OpenSSL file reading moves into it, so `gwz-server-policy` stays pure (§2). |
| P3-5: tooling | `EXPECTED_CRATES`, the release list, a second gate root and `prepare.py` are named; there are thirteen names (§4, §6) |
| P3-6: HTTPS suite on git2 | The git2-driven suites stay in core's integration tests. The review's alternative, a git2 dev dependency "as repo-inspect does", does not hold: repo-inspect has none, and a `gwz-` dev dependency would itself need classifying (§1, §3). |

Round 2, on revision 1 (`d0c82295…`), was GO: all nine findings closed, and the P3-6 departure was confirmed ([report](GwzCoreSessionCrateMap-ReviewCode-1.md)). Its four new P3s were applied after GO:

| Finding | Disposition |
|---|---|
| N1: the HTTPS bridge on the wrong side of `Engine` | The bridge is `gwz-https-endpoint`'s `Engine`. `Outbound` and `EndpointError` move to the contract crate. Each step's crate follows its files (§4). |
| N2: two paths to `gwz-ids` | `prepare.py` links the candidate-crates workspace the way it links `crates/` (§4) |
| N3: reuse §14 | Listed in §7 |
| N4: tests of crate internals | `test-support` features; CS7.1's movement commit keeps unit tests with their files (§1, §4) |

The same reviewer confirmed all four ([confirmation](GwzCoreSessionCrateMap-ReviewCode-1a.md)); GO stands. Its two notes are applied as it worded them: the HTTPS engine takes its `IdSource` at construction, and the HTTPS row lists its part of CS7.20.
