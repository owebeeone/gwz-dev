# Transport execution plan (revision 2, sha256 d3f669c8…) — CONSISTENCY-AXIS RE-VERDICT (round 2)

**Review object:** `dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 2, uncommitted draft, 659 lines, sha256 `d3f669c85b879d87845aa5edefc81111157d079d87dc7e8ac8018a22cb75ed83`.
**Baseline:** root `9fbe5108`, gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b9`.
**Lanes read as evidence for closures (not part of the object):**
- `gwz-dev-p0-design` `830f3463`
- `gwz-dev-p0-status` `475a1586`
- `gwz-dev-p5-gitmap`: root `46768dc0`, gwz-core `0ed48168`

**Date:** 2026-10-24.
**Axis:** Consistency, with a Surface section. Independent, adversarial, read-only. The Safety axis runs in parallel. This report is filed verbatim.

**Verdict: NO-GO.** P0 0, P1 0, **P2 5**, P3 4.

- **Closed:** 15 of the 17 round-1 findings are closed.
- **Partly closed:** P2-4 and P2-5. Each leaves one remnant: P2-5's remnant is R2-P2-5 below, and P2-4's is R2-P2-4.
- **New, introduced by the remediation:**
  - R2-P2-1: Phase 2 now leans on an "operation service" that does not exist.
  - R2-P2-2: under D-I the plan adopts a server design whose rules assume the server holds the credentials.
  - R2-P2-3: the new R7 proof uses a separate OS user, and the new carrier rules refuse one.
  - R2-P2-4: step 1.2 keeps one endpoint session per host, so the client side still has a host-wide 256-registration bound.

Pre-commit to GO: GO once R2-P2-1 to R2-P2-5 are closed as each finding's "Fix" states, with no new P2.

## 0. Evidence base

- **Object hash.** Checked at the start and at the end, identical (§3). Read in full: §1, §1.1, §2, §3, §3.1, Phases 0–7, and §5–§12.
- **§1.1 line citations, checked one by one:**
  - amendment 2: :84, :89, :95–97, :103, :113, :124, :168, :278–282, :348, :355–356, :364;
  - release plan: :52, :60, :64, :565;
  - amendment 1: :301, :336–337;
  - placement design: :12, :63, :96, :132–135;
  - `TransportPlacement.md`: :83–84, :139, :150, :188–190, :274–278.

  All match. The abbreviation "BP" is not defined (R2-P3-3).
- **§2 claims, checked one by one:**
  - correct: `mod.rs:117-123`, `:325-338`; `endpoint_environment.rs:115-122`; `transport_support.rs:149-170`; `capacity.rs:93`; `generated.rs:2678`;
  - correct: the gwz-core inventory has 36 non-comment `gwz_session_candidate` lines and gwz-py's has 2; there are 29 `reason = "gwz_session_candidate adds fields"` attributes;
  - correct: the cli arms in `request.rs`, `session.rs`, `session/checks.rs` and `session/driver/opening.rs`.
- **Session crates.** Read `crates/session-channel/src/lib.rs`, `crates/session-contract/src/{lib.rs, frame.rs}`, `src/session_host/{mod.rs, context.rs}`, the `session_host::` callers in gwz-cli and gwz-py, and `dev-docs/GwzCoreSessionDesign.md` (accepted, active) §4.1–§4.2. Searched for `SessionCall` in the protocol and binding sources: none.
- **Server design.** Read `dev-docs/history/parked/GwzCoreServerDesign.md` §4 (lines 223–330).
- **Registration path.** Read `transport_host/request.rs:455-470` (`ClientRequest::new` → `Session::register`) and `session.rs:637-650`.
- **Lanes.**
  - `830f3463`: contract §15 gains 8 rows; the reuse design gains a status block.
  - `475a1586`: the checkpoint's blob `dbb2bfb4` is byte-identical before and after the move; a 23-line pointer replaces it.
  - The capability map: 370 lines; read F2, F6, the open-decisions line, and rows G1, G2, G5, G6 and G9.
- **Document count.** 66 active root `dev-docs/` files, confirmed. The archive moves are uncommitted.
- **Not done.** No build or test run. Lane `gwz-dev-p1-trunk` was not reviewed.

## Problem, requirement provenance, and scope

- **§1.** Now also carries the operator's "no further work on 1.0.17 or the old in-process architecture" line. It is a faithful restatement.
- **The threat model in §1** is new. It comes from Safety's review, not from the operator. It becomes binding only through D-H, which is open. That is correct as written.
- **§1.1** is an adequate record of the clauses changed in amendment 2, the release plan and the placement design. It omits the accepted `GwzCoreSessionDesign.md`, the parked session plan and the crate map. Amendment 2 (line 90) says these govern the session release. The plan consumes their crates and frame tags (R2-P2-1).
- **Phase 5 provenance.** Its authority is now D-G plus the Git boundary draft, with R9 added. Acceptable.
- **Process rule 1** ("code probes before contract changes"). Lane `830f3463` already amended contract §15 as a pure documentation step. Its rows record decided operator scope (D-A), not new design. Accepted as consistent with the rule's intent.

## Surface (read cold)

- **Under D-I's recommendation,** the user-facing shape is the server design's: `gwz server start|stop|status|list|stdio`, a global `--server`, `GWZ_SERVER`, `gwz --server X server start` refused, and Python `SocketCoreBridge(address)` passed as `bridge=` with no `core=` parameter. This matches the existing `gwz-cli` command families and `Client(root, bridge, *, max_connections_per_host)` (`gwz-py/src/gwz/client.py:193-199`).
- **2.0(b)** lists what must be frozen before any code: refusals, path filesystem, second client, signals and stale sockets, and capability advertisement. That closes round-1 P2-1's "left to the implementer".
- **Remaining surface gaps:**
  - The default address and discovery are "per the server design". That design keys the default address on the credential environment (R2-P2-2).
  - The allowed-destination set (2.0(c)) has no non-interactive spelling ("the user names them or accepts a prompt"). See §1.3.

## 1. Findings

### 1.1 Closure of round-1 findings

| Round 1 | Status | Evidence in revision 2 |
|---|---|---|
| P2-1 duplicated surface | **Closed**, conditional on D-I (new issue R2-P2-2) | §1.1 server-design row; D-I; 2.0(b); `bridge=` |
| P2-2 operation channel | **Superseded by R2-P2-1** | 2.0(a) names a channel, but the service it names does not exist |
| P2-3 contract §4/§4.1/§6/§7 | **Closed**, with remnant R2-P3-2 | 1.1: four ports, `cleanup_domain`, `HostConfig`; 1.5: profile in `PreparedOperation` plus a host registry |
| P2-4 rollover vs D1/D6 | **Partly closed**: driver side fixed; remnant R2-P2-4 | 1.2 gives each operation its own driver session; the endpoint session stays host-wide |
| P2-5 phase order vs §13 | **Partly closed**: remnant R2-P2-5 | Ports and fakes move to 1.1 and the §13.1–2 rows to 1.8, but the code still compiles inside gwz-core |
| P2-6 `placement_endpoint.rs`, schema | **Closed** | §2 corrected; 2.2 re-scoped (additive-decoding gate, L2-02, inventories, gwz-py) |
| P2-7 core-only runtime | **Closed** | 1.7 in the trunk; type-level row; native route refused |
| P2-8 R7 on Windows | **Closed** (honest limit) | 2.5 Windows: recording doubles, source and runtime checks, README states the gap |
| P2-9 pipe forwarding | **Closed**, subject to R2-P2-1 for the stdio framing | 6.3 `ssh … gwz server stdio`; 0.3 feasibility |
| P2-10 pipe as network listener | **Closed** | 2.1: `PIPE_REJECT_REMOTE_CLIENTS`, SID DACL, first instance, peer checks, rows |
| P2-11 release identity | **Closed**, with remnant R2-P3-4 (D38) | §1.1 table; "version fixed at tag time"; §9 citation corrected |
| P3-1 §2 inaccuracies | **Closed**; new §2 inaccuracy in R2-P2-1 | proxy location, ~48 files, `mod.rs` join |
| P3-2 parallelism and ownership | **Closed**, minor remnant in R2-P3-4 | diagram matches text; ownership named; 3.3 sequences the retirement |
| P3-3 §7 vs closeout | **Closed** except D38 (R2-P3-4) | 6.2 names the D50 paths and D43/D51; 4.1 names D52; 1.8 the fairness probe; 6.1 sshd |
| P3-4 unnamed contract amendments | **Closed** | D-E recorded at 1.1; D-C reversal recommended to contract §3 |
| P3-5 §1 omission, Phase 5 provenance | **Closed** | §1 bullet; Phase 5 authority line; R9 |
| P3-6 documentation rules | **Closed**, minor remnants in R2-P3-3 and R2-P3-4 | historical files not edited; 66 files; Phase 6 exit; two reviewers in 6.5 |

### 1.2 New and remaining findings

#### R2-P2-1 — Phase 2 consumes an operation service that does not exist; §2 misdescribes the session crates
The plan's claims:
- §2: `session-contract` holds "request, response, event and cancel messages", and the crates are "consumed by nothing yet".
- Phase 2: "does not write a new framing, a new peer check or a new operation service".
- 2.3: the server runs "with the session-contract operation service".

What the source shows:
- `crates/session-contract/src/lib.rs`: "Bytes only: this crate holds no GWZ or Taut protocol type and decodes no CBOR."
- Its tags (`frame.rs:104-121`) are `SessionCall`, `SessionReply` and `SessionError`, plus handshake and server-control frames. There is no event or cancel frame.
- Tags 16–31 are "reserved for a transport lane" and are a protocol error today (`frame.rs:14`, `:96`, `:154`).
- The call, cancel and `events.subscribe` methods are defined in the accepted `dev-docs/GwzCoreSessionDesign.md` §4.1–§4.2, with schema additions in §13. None is implemented: there is no `SessionCall` in the protocol or binding sources. `src/session_host` says "nothing calls them yet".
- The crates are compiled into every gwz-core build through `pub mod session_host` (`lib.rs:70`). Its `EnvironmentSnapshot` is consumed by gwz-cli (`lib.rs:197`) and gwz-py (`route/transport.rs:18`).

The design gap:
- The session protocol is call and reply, with the client as the caller.
- At `cli` placement the core's driver must push `Open` and `CheckIdentity` envelopes to the client while a call is running.
- The placement design §4 forbids holding delivery until the final response.
- 2.0(a)'s "frames carrying `RequestMeta`/`ResponseMeta` with their `transport_message` envelopes" therefore needs either a defined transport lane (the reserved tags) or a long-poll method. 2.0 does not mention either.

Impact: 2.3's estimate (about 400 + 400 lines) silently includes building the session protocol's dispatch, events and cancel, the schema additions, and the transport lane. The governing design is uncited.

**Fix.**
- Correct §2.
- Add `GwzCoreSessionDesign.md` (and the crate map's frame-tag rules) to §1.1, as adopted or amended.
- Make 2.0(a) decide how core-to-client envelopes travel during a call (tags 16–31 or a long-poll), and mark it dual (wire format).
- Add a step that builds the operation service (call encoding, dispatch to handlers, events, cancel, schema additions) with its own size, or re-size 2.3.

#### R2-P2-2 — D-I adopts a server design whose §4 rules assume the server holds credentials and endpoints
Server design §4 (lines 223–330) states:
- "A server acts with its user's full authority".
- The default address `auto` is `server-<key>`, where the key is a SHA-256 over "every name and value in the must-match set (§5)": home, agent source, OpenSSL configuration, CA files.
- "Under `auto`, a server's sessions therefore share the home and the agent source."
- "Inside one server, the endpoint registry partitions endpoint instances". The `auto` client also starts a server itself (§6).

Under this plan:
- the core must hold none of those values;
- the client's owner holds the registry;
- the core machine is the less-trusted side (§1).

Yet 2.0(b) takes the "default address and discovery per the server design", and §1.1 records the design as adopted "as this plan's surface and carrier rules" without listing which clauses do not carry over.

**Executability under D-I = yes:** possible only if 2.0 states the amendments:
- an address key that does not hash the credential environment, or no keyed `auto` at all;
- whether `auto` start survives;
- that §4's registry bullets and §5's must-match set are replaced by the client's profile;
- that §1's "reuse through a server" becomes reuse in the client.

**Fix.** List those server-design clauses in §1.1's server row and in 2.0(b), as replaced.

#### R2-P2-3 — The R7 proof on macOS and Linux contradicts the carrier's same-user rule
- 2.5: "macOS and Linux: the core runs as a separate OS user that cannot read the client's key directory."
- 2.1: "an other-user client is refused at accept". Peer checks run in both directions under server design §4: "only the same user … may open a session". D-D: "same-user".

As written, the 2.5 proof cannot connect over the 2.1 carrier. The two remediations (Safety P0-1 and Safety P1-2) collide.

**Fix.** State the route. For example, the stdio form under `sudo -u <core-user> gwz server stdio`: the server design has no listener and no peer check for stdio children. Then add a same-user socket row that proves the carrier separately. A test-only bypass of the peer check would weaken the property under test, so it should not be the answer.

#### R2-P2-4 — Step 1.2 keeps one endpoint session per host, so the client side still has a host-wide 256-registration bound
- 1.2: "Today's `TransportRuntime` driver session becomes the binding; its endpoint session and pools become the host's."
- Endpoint-side registrations go through the same lifetime bound:
  - `ClientRequest::new` calls `session.register(request, None)` (`request.rs:463-464`);
  - the check is `state.used.len() >= 256` (`session.rs:642`).
- So a host-wide endpoint session reintroduces the bound on the client. That contradicts 1.2's own red row ("300 bindings on one host never hit a host-wide bound"), R6, contract D6, and contract §6, where an operation binding owns "own bootstrap/mux" and an endpoint instance owns "physical owners/pools".

**Fix.** In 1.2, make both mux sessions (driver and endpoint) per binding, with only the endpoint instances and pools host-wide. Add an endpoint-side row with more than 256 bindings.

#### R2-P2-5 — The claim that the first proof meets contract §13.1 is false as built (remnant of round-1 P2-5)
- Phase 1 preamble (line 286): "The first proof runs against fakes (contract §13.1: no core/git2 compile …)".
- 1.8 runs the §13.1–2 rows. But `host.rs` and its fakes live in `gwz-core/src/transport_host/`, and every gwz-core test build compiles git2 and core.
- Contract §3's warning ("would preserve the current temptation to reach core internals") is not policed until the 4.1 move.

**Fix.** Choose one:
- build `host.rs` from 1.1 in the crate that D-C's reversal recommends (`gwz-transport/crates/gwz-connection`) or in a standalone candidate crate, so 1.8's rows really do not compile core;
- or state that 1.8 meets §13.1 only in substance, and add an import guard (the host module imports nothing from `crate::` except its ports) until 4.1.

#### R2-P3-1 — 4.1 misreports the capability map, and the map's open decisions are not in §10
- 4.1 says the map "showed why the core-hosted alternative … cannot work". The map's F6 (lines 277–280) offers a way that keeps the repository DAG with a core-hosted crate ("a gwz-git-defined port that core implements over `gwz-connection` keeps the DAG"), or a separate repository. The D-C reversal is still a sound recommendation, but "cannot work" overstates the evidence.
- The map ends: "Open decisions for the operator: F2 (supersession line), F6 (DAG), G3/G7/G8 in or out of Phase 5, F10, and the G10 dispositions." The plan settles F2 by reading D-G as superseding `GwzNoFallbackPlan` §6, and puts G3/G7/G8 out of scope. §10 lists none of these as decisions.

#### R2-P3-2 — 3.2 still keeps one owner per profile
3.2: "A module-level composition object owns `HostOwner`s by profile". But 1.5 puts the profile partition inside a host's registry ("otherwise chooses another instance"), per contract §6. With one host per profile, the registry is never exercised in Python. The plan should state one owner per composition, or give the reason for several.

#### R2-P3-3 — Inconsistencies in §3 and §3.1
- **R4 pass rule.** §3.1's R4 row lists `HOME`, agent, CA file and proxy. The §3 R4 row and 1.5 also include `GIT_CONFIG_GLOBAL` and `GH_TOKEN`.
- **R1 warm baseline.** R1's "warm median ≤ `___` s", to be filled by 0.3, cannot be filled for the CLI: §3.1's own Definitions say "Current main has no owner across CLI commands, so its CLI baseline is cold only". The comparison basis needs stating (warm build against cold baseline, or row C's per-call value).
- **R1 "Proven in Phase 1".** Phase 1's exit lists only R3, R4 and R6.
- **Phase 6 exit.** It says "R7's two-machine form"; this should be R8.
- **"BP:54/57"** in §1.1 is undefined.
- **"crate map §1 line 144".** Line 144 is in the crate map's §5.

#### R2-P3-4 — Smaller remnants
- **D38.** It is "a Credential provider row after 4.2" with no step before Phase 6. The closeout gives it 1.1.0 standing, and §3's rule says it stays in the release unless the operator moves it in writing. Say which.
- **2.4's amendment location.** 2.4 amends the placement design's SSH-only rule, "reviewed", but §1.1 says historical files are not edited. Name the document that carries the amendment (2.0's `GwzSplitCommandChannel.md`, or a new one).
- **Sub-lane ownership.** 1.3 runs as a sub-lane but rebuilds `build_native` (`mod.rs`) and, by 1.4's note, shares `session/capacity.rs`; both are trunk-owned. Name the hand-off per §4's own rule.

### 1.3 The open decisions: executability on the recommended option, and what breaks otherwise

| Decision | If the recommended option is taken | If not |
|---|---|---|
| **D-H (b)** client-side admission | Executable once 2.0(c) also defines a non-interactive allowed set. "Names them or accepts a prompt" cannot serve `--json`, CI or `SocketCoreBridge`. §3.1 must say how R1/R2/R3 runs in the `--server` placement name `github.com` and the fixtures. Under `--server` every host is core-named, so Windows default credentials always need the flag. This is a recorded difference from `local`, consistent with §1.1's OD16 row. | (a): §1's threat model, R7's outcome ("cannot use the client's") and its measurement ("admitted only against the client's allowed destinations"), 2.0(c), 2.3's three refusal rows and 6.4's note must all be rewritten. OD16 parity returns, and Safety's P1-1 reopens. |
| **D-I yes** | Executable once R2-P2-1 and R2-P2-2 are fixed. | 2.0(b) must freeze a new surface. 2.1 loses its normative rules ("server design §4"). 6.3 and R8's method (`ssh … gwz server stdio`) and 0.3's feasibility check lose their basis. 3.2's `SocketCoreBridge` and 6.4 must be rewritten. |
| **D-C reversal** | 4.1 and 5.4 are consistent (gwz-git and gwz-core both depend on gwz-transport's crate). | 4.1 has no destination as written. 5.4's gwz-git → connection edge inverts the DAG unless map F6's port option is added as a step. |
| **D-J yes** | 6.5 is consistent. | Installing the CLI from crates.io fails, because gwz-core depends on an unpublished crate. 6.5's chain must vendor it or drop the dependency. |

## 2. Out-of-scope observations

- Lane `830f3463` changed the accepted contract's §15 without its own review record. Its rows record operator decisions; whoever lands it should say which review covers it.
- `gwz-core/docs/TransportPlacement.md:125` still says "isolated Unix candidate configuration", against `any(unix, windows)`. 2.2 rewrites the file.
- The p0-status lane's `history/CurrentProgramCheckpoint.md` is byte-identical to the original (blob `dbb2bfb4`), as the plan claims.
- The nine archive moves (contract and credential review rounds) are uncommitted, so the root count of 66 holds only in this working tree.

## 3. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `d3f669c85b879d87845aa5edefc81111157d079d87dc7e8ac8018a22cb75ed83`, 659 lines. This matches the pin given at the start. Nothing was modified: only inspection commands were run, in this workspace and in the lanes.
