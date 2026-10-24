# Transport execution plan (revision 3, sha256 fb0062e3…) — CONSISTENCY-AXIS RE-VERDICT (round 3)

**Review object:** `dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 3, uncommitted draft, 754 lines, sha256 `fb0062e3eec64dcb814c67cfa7d2a8c7a25c0c55c5aca967c0f075bf2abb4854`.
**Baseline:** root `9fbe5108`, gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b9`.
**Lanes read as evidence (not part of the object):** `gwz-dev-p0-design` `830f3463` (contract §15 rows) and `gwz-dev-p5-gitmap` (`GwzGitCapabilityMap.md`).
**Date:** 2026-10-24.
**Axis:** Consistency, with a Surface section. Independent, adversarial, read-only. The Safety axis runs in parallel. This report is filed verbatim.

**Verdict: NO-GO — NEW ARCHITECTURAL ROOT CAUSE (R3-A1): in the two-process split, the served core has no connection capability that gwz-git can consume. This stops the object and goes to the operator.** Besides R3-A1: P0 0, P1 0, P2 2, P3 3.

- **What R3-A1 is.** In the split the core process holds no `HostOwner`. Under the accepted contract, the only communication capability gwz-git may consume (`Operation`) exists only where a `HostOwner` exists, and nothing else may construct one. No accepted document says what gwz-git's capability is in the served core.
- **Why this round surfaces it.** The problem has been present since revision 1, but nothing depended on it until now. Revision 3's text makes it load-bearing:
  - step 1.2 now says both mux sessions belong to the binding;
  - 4.1 and D-C now reject the gwz-git-defined port as an unnecessary extra;
  - 5.4 binds gwz-git to the contract's `run_git_via_gwz_git(&Operation)`.
- **Why no text fix closes it.** Any fix changes accepted contract clauses (§3 edges, §4, §12), which D-A says govern the owner. It needs an operator decision.
- **Round-2 findings:** all 9 closed, three with consequences recorded below.
- **Executable as written under the recommended options?** Phases 0–3 and the plan's local-placement work are executable. The whole is not, because of R3-A1 (it bites from Phase 5) and R3-P2-1 and R3-P2-2 (they bite in Phase 2).

## 0. Evidence base

- **Object hash.** Checked at the start and at the end, identical (§3). Read in full: §1, §1.1, §2, §3, §3.1, Phases 0–7, §9, §10 and §12.
- **New citations, checked:**
  - session-contract `frame.rs:20-27`: the `SessionOpen` body "can carry the environment snapshot, which is secret-bearing";
  - server design :202 and :1170 (a socket host refuses a `SessionOpen` without a snapshot) and :1175–1178 (forwarding);
  - `gwz-core/dev-docs/GWZRequirements.md:108` ("The snapshot MUST cross a byte stream once, in `SessionOpen`") and `GWZDesign.md:104, :108`;
  - `https_auth.rs:139-141` (`HelperSlots::new` mints `SpawnGate::new()`), `https_worker/native/sspi.rs:32` and `request.rs:66` (`SpawnGate::default()`).

  All match.
- **Repository facts.** `gwz-transport/Cargo.toml` is a workspace root (`[workspace]`), `publish = false`, and has no `crates/` yet. gwz-core depends on gwz-transport by path under `cfg(gwz_transport_candidate)` (`gwz-core/Cargo.toml:124-126`).
- **Contract.** `dev-docs/GwzConnectionApiContract.md`:
  - :70: "gwz-git | gwz-connection and the qualified Git bindings";
  - :155: "or public constructor for Operation/Exchange";
  - :779: "`run_git_via_gwz_git` accepts the operation communication capability";
  - lane `830f3463` §15 row: "The carrier between the two sides is a composition adapter outside this library, delivering the same discrete messages through the binding ports (§6)".
- **Session design.** `dev-docs/GwzCoreSessionDesign.md` §4.2: the methods refer to the §5 session host (direct workers §5.2, admission, records, the class-W mutator lock).
- **Capability map.** F6 (lines 277–280).
- **Not done.** No build or test run.

## Problem, requirement provenance, and scope

- **§1.** Unchanged from round 2 apart from the threat model, which was made more precise (tuple, service class, client-chosen identity). The threat model comes from Safety's review and binds through D-H (open; revision 3 is written on (b)).
- **§1.1.** Now records the session design §4.1–§4.2, the frame-tag rules, the server design's credential-model and §16 clauses, `GWZRequirements.md:108` and `GWZDesign.md:104,108`. No unrecorded change of an accepted clause was found, except the ones R3-A1 requires: contract §3, §4 and §12, as they apply to the served core.
- **Smuggled items.** None: no dependency upgrade, no new repository (the crate joins gwz-transport's existing workspace), no network listener.

## Surface (read cold)

`--server <address>` as frozen in 2.0(b) accepts only "a path in a 0700 per-user runtime directory … or a random-named pipe". The proofs, however, depend on two client launch forms that this grammar leaves out:
- the stdio form, launched under a separate user (2.5);
- `ssh … gwz server stdio` (6.3, R8).

See R3-P2-2. The rest of 2.0(b) is consistent with the server design as amended in §1.1, including concurrent-client locking and the `cli`-placement refusals. Python `SocketCoreBridge(address, allow=[…])` passed as `bridge=` is consistent with `Client`'s signature.

## 1. Findings

### 1.1 Closure of round-2 findings

| Round 2 | Status | Evidence in revision 3 |
|---|---|---|
| R2-P2-1 operation service | **Closed as stated**; the scope consequence is R3-P2-1 | §2 corrected (bytes-only frames, no event or cancel, tags 16–31 reserved, compiled in through `session_host`); §1.1 adopts session design §4.1–§4.2; 2.0(a) decides the lane; 2.2b builds the service |
| R2-P2-2 server design's credential model | **Closed** | §1.1 rows: §4 credential model replaced (no `auto`, no auto-start, registry is the client's profile); §3 snapshot requirement amended; §16 forwarding replaced |
| R2-P2-3 separate-user leg vs peer check | **Closed in principle**; the client launch form is missing (R3-P2-2) | 2.5 uses the stdio form under `sudo -n -u`; 2.1 adds a source check that no override exists |
| R2-P2-4 host-wide endpoint session | **Closed**; consequence R3-A1 | 1.2: both mux sessions per binding; `ClientRequest::new` counted per binding; row "on either side" |
| R2-P2-5 §13.1 as built | **Closed** | 1.1: the crate in `gwz-transport/crates/gwz-connection` with no core or git2 edge; feasible, since gwz-transport is already a workspace root |
| R2-P3-1 map misreport, open decisions | **Closed**; consequence R3-A1 | 4.1 and D-C cite F6 correctly; D-K lists F2, G3, G7, G8, repo-inspect, G10 |
| R2-P3-2 owner per profile | **Closed** | 3.2: one owner per composition |
| R2-P3-3 §3 and §3.1 inconsistencies | **Closed** | R4 rule; R1 warm against the cold baseline; R1 named in 1.8; Phase 6 exit says R8; "BP" defined; crate map §5 |
| R2-P3-4 D38, 2.4 carrier, 1.3 ownership | **Closed** | 4.2b; 2.4 carried in `GwzSplitCommandChannel.md`; 1.3 in the trunk |

### 1.2 New findings

#### R3-A1 — ARCHITECTURAL: in the split, the served core has no connection capability that gwz-git can consume
The accepted contract fixes three things:
- **Edges (§3, line 70):** gwz-git depends on gwz-connection.
- **The capability (§12):** gwz-git's network code takes the contract's `Operation` ("`run_git_via_gwz_git` accepts the operation communication capability; it cannot fall back by constructing a private host").
- **Construction (§4, line 155):** "There is no … public constructor for Operation/Exchange". An `Operation` exists only through `HostHandle::admit → acquire → register`, that is, only where a `HostOwner` exists.

What the plan puts on each side of the carrier:
- **The client process** holds the only `HostOwner` (§1, "Terms"; lane `830f3463`: "At the `cli` placement the endpoint side is the client process's `HostOwner`, and the core process builds no endpoint with credentials for it").
- **The core process** runs gwz-git and the 1.7 driver-only builder, and holds no host.
- **1.2:** "Both of today's mux sessions, the driver's and the endpoint's, become per binding", so the binding's driver half sits in the core process.
- **5.4:** network operations "take the connection capability (contract §12's `run_git_via_gwz_git`)".

That leaves four questions with no answer in any accepted document or in the plan:
- what object gwz-git receives in a served core;
- which crate defines it;
- which process holds the driver half of a binding, and as which type;
- how a core-side driver half exists without the `HostOwner` that the contract requires for every `Operation`.

Revision 3 (4.1, D-C) rejects the one mechanism the evidence offered. The capability map's F6 option was a gwz-git-defined port that core implements over gwz-connection; the plan calls it "an extra … port" that "the contract's placement" makes unnecessary. In the split, a port of that kind (or an equivalent core-side capability type) is not optional: it is the only way for gwz-git to be the same code under `local` and `cli`.

Why this is architectural and not a wording fix:
- Each candidate resolution changes accepted contract clauses (§3 edges, §4 constructors, §12 consumer) or the layering of the binding. Candidates include a gwz-git-defined stream port with two core implementations, a gwz-connection driver-half type with its own constructor, or a proxy host in the core whose adapter forwards to the client host.
- One of those candidates (the proxy host) would also move pool and reuse policy to the less-trusted side, against §1's threat model.
- D-A makes the contract govern the owner, so the choice belongs to the operator.

Effect if left open:
- 5.4 cannot land without breaking R5 and R7 for `--server`, or the `cli` placement cannot use the migrated gwz-git;
- R9's adoption test cannot cover the `cli` path;
- 1.2 has no defined home for the binding's driver half.

**Needed from the operator.** A decision on the core-side capability: a gwz-git port per F6, a driver-half type in gwz-connection, or another option. Then a contract amendment for §3, §4 and §12 (or a clarification that they do not apply), recorded in §1.1. D-C's rationale should then be restated, because the F6 port may be needed regardless of where the crate is placed.

#### R3-P2-1 — 2.2b adopts session design §4.1–§4.2 without §5, which those methods are defined against
- 2.2b implements "the session design's §4.1–§4.2 methods" in about 500 + 400 lines, and §1.1 adopts only §4.1–§4.2 and §13.
- §4.2's semantics are written in terms of the §5 session host:
  - "Direct methods … run on the session's direct workers (§5.2)";
  - `submit` "is sent as soon as the session host admits the operation … its record, token and gate exist";
  - "`remote_identity` is therefore an operation method in class W (§5.1)" (the mutator lock);
  - cancel and release act on "records".
- Meanwhile §1.1 and Phase 7 defer "the session host" (as a client-facing service), and the plan never says whether §5's admission, records, worker classes and mutator lock are adopted, replaced, or reduced for the served core.

Impact: the service cannot be implemented to the cited design without §5, and §5 is neither adopted nor sized.

**Fix.** In §1.1 and 2.2b, state which §5 clauses the served core implements (records, admission, direct workers, class W). Size them, or split 2.2b. Distinguish §5's session host (in the served core) from Phase 7's persistent client.

#### R3-P2-2 — The frozen `--server` address grammar omits the launch forms that R7 and R8 depend on
- 2.0(b): the address is "a path in a 0700 per-user runtime directory … or a random-named pipe, printed by `server start`".
- But:
  - 2.5's separate-user leg runs "`sudo -n -u <core-user> gwz server stdio` on a pipe, or the Windows equivalent";
  - 6.3 and R8 run `ssh -a -x -T … dabeest gwz server stdio`;
  - D-I's alternative text says that removing the server design removes "6.3's stdio evidence path".
- The server design's own client forms are exactly `stdio` and `ssh://[user@]host[:port]/path` (server design :87–88). 2.0(b) neither adopts nor replaces them.
- No step builds a client that speaks over a child's standard streams. That client is needed whether the child is `sudo …` or `ssh …`.
- "The Windows equivalent" of `sudo -n -u` is undefined, and 0.3's feasibility checks do not cover it.

Impact: R7's separate-user leg and R8's method have no client-side mechanism within the frozen surface.

**Fix.** Choose one:
- in 2.0(b), adopt the server design's `stdio` and `ssh://` client forms, amended per §1.1's §16 row, plus a test-only launch hook if `sudo` is not a supported form, and name the step that builds them;
- or state that 2.5 and 6.3 use a harness client, and name where it lives.

Add the Windows separate-user launch method to 0.3's feasibility checks.

#### R3-P3-1 — Costs in the phase headers disagree with §9
- Phase headers say Phase 1 is 20–25, Phase 2 is 20–25, Phase 4 is 8–10 and Phase 5 is 20–30 lane-days. §9 says 22–27, 30–36, 14–18 and 30–40.
- §9's upper total adds to 154, not the stated 156.

#### R3-P3-2 — Stale status text
- §12's heading still reads "Review closure (revision 1 → 2)".
- Phase 0's diagram ("done: rev 1 → rev 2") and its review bullet ("re-verdict pending") predate round 2.

#### R3-P3-3 — Leftovers and loose ends in 2.0
- 2.0(a) still says "the session contract's request/response/event/cancel frames". §2 now says correctly that there is no event or cancel frame.
- The admission grammar `--allow-destination <scheme://host[:port]/path>` takes a path, but §3.1's `--server` rows are said to "name `github.com`". The plan does not say whether a path is exact or a prefix, so a 16-member run would need 16 entries.
- 1.2's sentence "The host owns pools and endpoint sessions through `EndpointFactory`" contradicts the same step's "Both … mux sessions … become per binding". It should read "endpoint instances".

### 1.3 Executable as written under the recommended options?

| Part | Under D-C reversal, D-H (b), D-I yes, D-J yes, D-K as stated |
|---|---|
| Phase 0 | Executable; 0.1, 0.2 and 0.4 have landed in lanes |
| Phase 1 | Executable for the `local` placement. The binding's driver half under `cli` has no defined home (R3-A1) |
| Phase 2 | 2.0, 2.1 and 2.2 executable. 2.2b blocked on R3-P2-1. 2.5's separate-user leg blocked on R3-P2-2 |
| Phase 3 | Executable, given Phase 2 |
| Phase 4 | Executable (the crate already in place; 4.2 and 4.2b sized) |
| Phase 5 | 5.1–5.3 executable (local Git). **5.4 not executable for `cli` (R3-A1)** |
| Phase 6 | 6.3 and R8 depend on R3-P2-2 |

## 2. Out-of-scope observations

- gwz-transport's manifest is `publish = false`. D-J's "`gwz-connection` published after gwz-transport" assumes gwz-transport itself enters the publish chain (amendment 2 TR3.3). The plan does not restate this.
- Lane `830f3463` edited the accepted contract's §15 without a review record of its own (noted in round 2; unchanged).

## 3. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `fb0062e3eec64dcb814c67cfa7d2a8c7a25c0c55c5aca967c0f075bf2abb4854`, 754 lines. This matches the pin given at the start. Nothing was modified: only inspection commands were run.
