# Transport execution plan (revision 2, sha256 d3f669c8…) — SAFETY-AXIS REVIEW, round 2

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 2, 659 lines, sha256 `d3f669c85b879d87845aa5edefc81111157d079d87dc7e8ac8018a22cb75ed83`. It is an uncommitted draft (`??`).
**Baseline:** gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b`. Lane evidence: p0-design `830f3463` (contract §15 rows 20–22).
**Date:** 2026-10-24.
**Axis:** Safety. The review is independent, adversarial and read-only. The Consistency axis was not consulted. This report is filed verbatim as `GwzTransportReleaseConvergencePlan-ReviewSafety-2.md`.

**Verdict: NO-GO.** P0: 0, P1: 3, P2: 4, P3: 10.

The round-1 P0 is closed. All three new P1s come from text added in revision 2:
- the admission rule is too coarse, and gates the wrong Windows policy;
- adopting the parked server design (D-I) brings in its rule that the remote core uses forwarded credentials.

Pre-commit: GO once P1-1 to P1-3 and P2-1 to P2-4 are closed in text. No code is needed for this gate.

## 0. Evidence base

- **Object hash.** Verified at start; it matches the pin (§3).
- **Revision 2:** read in full (lines 1–659).
- **Round-1 report:** used as the closure baseline.
- **Lane p0-design** (`830f3463`, contract §15 rows 20–22):
  - Bind/Bound, request-registration and receiver-affinity rules "apply per binding".
  - The carrier is a composition adapter.
- **Parked server design** (`dev-docs/history/parked/GwzCoreServerDesign.md`), which D-I would adopt:
  - **:202 and :1170.** "A socket host … refuses a `SessionOpen` that carries no snapshot".
  - **:1175.** "The remote core uses the remote account's agent, keys and `gh` login, or an agent the user's SSH configuration forwards … gwz neither turns forwarding on nor turns it off".
  - **:1176.** The forwarded agent lets the remote host "authenticate as the user to any host".
  - **:1178.** `SendEnv` can forward `GH_TOKEN`.
- **Live core documents:**
  - `gwz-core/dev-docs/GWZRequirements.md:108`: "The snapshot MUST cross a byte stream once, in `SessionOpen`".
  - `gwz-core/dev-docs/GWZDesign.md:104,108`.
- **Crates:**
  - `crates/session-contract/src/frame.rs:25`: `SessionOpen` bodies "carry the environment snapshot, which is secret-bearing".
  - `crates/session-channel` has snapshot-wiping tests.
- **Source:**
  - `git/endpoint/https_worker/prepare.rs:118-121`: `native_policy` covers both `WindowsConfigured` and `WindowsDefault`.
  - `git/gitbackend/transport_binding.rs:190-200`: every Windows HTTPS remote gets one of the two.
  - Spawn-gate constructors with their own gate: `https_auth.rs:140` (`HelperSlots::new`), `https_worker/native/sspi.rs:32` (`NativeHost::new`), `transport_host/request.rs:66` (`SpawnGate::default()`).
  - `https_auth/runner/environment.rs:9`: the helper's configuration view "can carry a credential".

## Problem, requirement provenance, and scope

- **Threat model.** Revision 2 states it (§1 :42): the core is the less-trusted side. That is the right premise, and it now traces to an open operator decision (D-H).
- **Undecided items.** Two scope items are still open, D-H and D-I. The plan presents their recommended options as if the closure table could already rely on them.
- **What D-I brings with it.** The parked server design is a whole accepted design whose credential model is the opposite of R7's: the core holds and uses credentials, and the client sends its environment snapshot. Adopting its "surface and §4 rules" without naming the clauses that must change would bring back two exposure routes that round 1 closed (P1-3, P2-1).
- **§1.1 table.** It records many amended clauses, but none of these four: `GWZRequirements.md:108`, `GWZDesign.md:104,108`, server design §3's snapshot requirement (:202) and server design §16's credential rule (:1175–1178).

## 1. Closure of round-1 findings

| Round 1 | Revision 2 text | Status |
|---|---|---|
| P0-1 carrier peer authentication | 2.1 states the guards inline: random pipe name, first-instance, reject-remote, SID-only DACL, peer checks both ways, 0700 directory, squat/remote/other-user/low-integrity rows. Independent of D-I | **Closed.** The Python client's rows are a residual (P3-6) |
| P1-1 trust model | §1 threat model; 2.0(c); D-H open | **Partly closed.** Replaced by new P1-1 (granularity) and P1-2 (Windows native, redirects) |
| P1-2 R7 proves only non-use | 2.5: separate OS user on macOS/Linux, PID-recording doubles, canaries, carrier capture; 1.7 refuses the native route | **Closed for M and L,** except that the separate-user leg cannot connect under 2.1 (P2-3). Windows is P2-4 |
| P2-1 HTTPS at `cli` | 2.4: its own step, design amendment, dual review | **Closed.** Ordering residual in P3-9 |
| P2-2 carrier message set | 2.0(a): no snapshot; client-side proxy, CA and helpers; D43/D51 reader at the client | **Closed in 2.0, reopened by D-I** (P2-1) |
| P2-3 server stuck states | 2.3: handshake and idle bounds, carrier loss cancels local phases, mid-push row | **Closed.** Second-client rule deferred to 2.0 (P3-7) |
| P2-4 helper environment | 1.5 moves it to the per-operation auth context; R4 text in §3 | **Closed.** §3.1's R4 pass rule (:210) omits `GIT_CONFIG_GLOBAL`/`GH_TOKEN` (P3-10) |
| P2-5 rollover | 1.2: per-operation binding, local 256 limit, stale-event row | **Closed** |
| P2-6 spawn gate | 1.1: one gate per process; D-E recorded against §7.3 | **Mostly closed.** Residuals in P3-3 and P3-4 |
| P2-7 per-step dual review | Six steps marked (dual) | **Mostly closed.** Omissions in P3-5 |
| P3-1 control progress, frame cap | 2.1 rows | Closed |
| P3-2 lane disjointness | §4 file ownership; Phase 2 code steps gated on named Phase 1 steps | Closed |
| P3-3 §3.18 logon-session row | 1.6 and Phase 7 | Closed |
| P3-4 sensitive metadata | 2.0(a) | Closed; see P1-1 on core-chosen identities |
| P3-5 two-machine path | 6.3 via `ssh … gwz server stdio`; feasibility checked in 0.3 | Closed as a mechanism; new P1-3 |
| P3-6 diagnostics across processes | 2.0(a) | Closed |
| P3-7 adoption before authorization | 2.5's last line | Closed |

## 2. Findings on revision 2

### P1-1: The admission rule in 2.0(c) is per host only. On an allowed host, including github.com, the core keeps the client's full authority, and §1's promise of service-level admission is not delivered.

- **What §1 promises.** The client "admits only destinations and services it was told to allow for that invocation" (:42–45).
- **What 2.0(c) delivers** (:381–386):
  - an allowed set of *hosts*;
  - a *type* restriction ("only as a `GitService`-typed open").
- **The consequence.** Once github.com is allowed, which every real `--server` use needs, a less-trusted core can:
  - push, delete or force-push to any repository the user can write there;
  - choose any client key path (repository-configured identities);
  - probe the client's filesystem through `CheckIdentity`.
- **The client already knows enough to do better.** It runs the command, so it knows the command, the members and the service class. Nothing makes `gwz --server X fetch` refuse a `receive_pack_*` open.
- **The default is ambiguous.** "The hosts of the client's own remotes for a `local` client" can be read as remotes from the core's repository configuration, which is the untrusted side.
- **Fix.** Admit (scheme, host, port, repository path) tuples together with a service class taken from the client's own command (fetch-class commands get upload-pack only). Allow identity selection only from the client's invocation or the client's configuration. For `--server`, the default set is empty, failing closed, unless the user supplies one. If the operator accepts host granularity, record the push residual against D-H.

### P1-2: The Windows credential gate covers only `windows_default`, but `WindowsConfigured` answers NTLM and Negotiate too, and redirect targets are never admitted.

- **The code.** On Windows, every HTTPS remote is `WindowsConfigured` or `WindowsDefault` (`transport_binding.rs:190-200`). Both are native policies (`prepare.rs:118-121`).
- **The decision record.** Amendment 2 revision 8 says the logon session answers NTLM and Negotiate whenever they are offered, ahead of any helper.
- **The gap.** 2.0(c) and 2.3 gate only `windows_default`. Under the ordinary `WindowsConfigured` policy, the NetNTLMv2 response still goes to any core-named host. That is the OD16 hazard the §1.1 table says D-H re-decides.
- **Redirects.** Discovery redirects are followed at the endpoint. The amendment row "authenticates at the second" means credentials are answered at the redirect target, which 2.0(c) never checks.
- **Fix.** Gate the logon-session answer under either Windows policy. Apply admission again at every redirect hop, before any credential, helper or native exchange. Add red rows for both: a `WindowsConfigured` NTLM challenge from a host not allowed, and a redirect from an allowed host to one not allowed.

### P1-3: D-I adopts the server design's SSH stdio form, which leaves agent forwarding on and assumes the remote core uses credentials. The recommended path puts the client's agent on the less-trusted machine.

- **The adopted text.** Server design §16 (:1175–1178), which D-I adopts and §1.1 does not amend:
  - "The remote core uses the remote account's agent, keys and `gh` login, or an agent the user's SSH configuration forwards … gwz neither turns forwarding on nor turns it off".
  - It records that a forwarded agent lets the remote host "authenticate as the user to any host".
  - It records that `SendEnv` can forward `GH_TOKEN`.
- **The consequence.** Step 6.3's evidence, `ssh dabeest gwz server stdio`, and the supported `ssh://` form both inherit the user's `ForwardAgent`/`SendEnv` settings. R7 can then be false on the very run that is meant to prove the split across machines.
- **Fix:**
  - Amend §16 in §1.1. In `cli` placement the client launches `ssh` with `-a -x -T -o ForwardAgent=no -o ClearAllForwardings=yes`, and with no `SendEnv` for credential variables (or `-o SendEnv=-*` where supported).
  - `gwz server stdio` refuses, or reports, an inherited `SSH_AUTH_SOCK` or credential variable in `cli` placement.
  - 6.3 asserts both on the core side.

### P2-1: D-I's surface and the consumed `session-contract` both require the environment snapshot that 2.0(a) forbids, and the live requirements still say MUST.

- **The conflict.**
  - Server design :202 and :1170: a socket host refuses a `SessionOpen` "that carries no snapshot".
  - `GWZRequirements.md:108`: "The snapshot MUST cross a byte stream once, in `SessionOpen`".
  - `session-contract/src/frame.rs:25` defines `SessionOpen` as the snapshot's carrier.
  - 2.0(a) says no snapshot crosses, but §1.1 lists none of these clauses.
- **The risk.** Building "per the server design" puts the snapshot, and with it every secret in the client's environment, on the carrier.
- **Fix.**
  - Add the clauses to §1.1.
  - Define the `SessionOpen` body for `cli` placement as carrying no environment.
  - Add a red row in 2.1 or 2.3: a `SessionOpen` carrying any environment field is refused, and the canary capture covers the handshake frames.

### P2-2: Where admission is enforced, and how it behaves without a terminal, is unspecified. The Python and CI paths cannot run under it, or would bypass it.

- **Placement.** 2.3, a core-side step, says admission is "implemented in the client's open path". It must instead sit in the client endpoint, the `cli` `HostHandle`, which gwz-cli and `SocketCoreBridge` share. It must run before `CheckIdentity`, agent, helper and connect.
- **No terminal.** "Accepts a prompt" has no defined behaviour for:
  - gwz-py;
  - `--json`;
  - CI;
  - `ssh` in `BatchMode`.
- **Effect.** Either the R7 CI rows and Phase 3's rows cannot run, or an implementation defaults to accepting.
- **Fix.** Name the flag, the configuration and the Python parameter that supply the allowed set. Without a terminal, fail closed with a named refusal.

### P2-3: The separate-user leg of R7 cannot connect under 2.1's same-user peer check.

- **The contradiction.** 2.5 (:426) runs the core as a separate OS user on macOS and Linux. 2.1 refuses "an other-user client … at accept", and listener verification refuses an other-user listener.
- **The danger.** The proof can only pass through an override of the peer check, and an override that reaches production code undoes P0-1's closure.
- **Fix.** Run that leg over the stdio form (`sudo -n -u <core-user> gwz server stdio` on a pipe), which needs no peer check. Forbid any peer-check override in production code, by source check.

### P2-4: R7 on Windows still claims "cannot use the client's", but in the same-user form the core's own token is the logon session.

- **Why the doubles fall short.** On dabeest, client and core run as one user, and SSPI default credentials belong to the core's token. The doubles cannot observe in-process `AcquireCredentialsHandleW`.
- **What else can use the logon session.** Code the core runs that gwz does not write: repository hooks, filters, and Git for Windows' own networking (Negotiate) in spawned `git`.
- **Why the account rule doesn't decide it.** The "no account change" rule is dabeest's host rule. GitHub-hosted Windows runners allow creating a local user, as the macOS and Linux runners do.
- **Fix.** Either run the Windows CI leg as a separate local user over the stdio form, or narrow the §3 R7 row (:130) for Windows to "gwz code never uses". State that core-side hooks and filters run with the core user's logon session.

### P3-1: Bindings are not scoped to their carrier connection

Contract §6 says the carrier adapter attaches the binding capability. The plan should state that a session id is valid only on the carrier connection that bound it, because 3.2 lets one host serve several `SocketCoreBridge`s. Add a red row: a core on bridge A sends frames under bridge B's session id, and they are discarded with no state created.

### P3-2: The admission prompt shows core-controlled strings

The prompt must show a canonical, control-stripped host:port, with an IDN or punycode form. The "refused port" set must be defined.

### P3-3: The D-E named object needs ownership and residual rules

- Its name must be per-process.
- If the object cannot be created, or exists with a foreign owner, native authentication must fail closed.
- The residual must state its scope: gwz-py processes, and native `CreateProcess` calls with handle inheritance.

### P3-4: Spawn-gate constructors that make their own gate remain until 4.1

`HelperSlots::new` (`https_auth.rs:140`), `NativeHost::new` (`sspi.rs:32`) and `SpawnGate::default()` (`request.rs:66`) each make a fresh gate. 1.1's row "two hosts share one gate" does not remove them. Remove them, or route them through the composition gate, in 1.1.

### P3-5: Three secret-handling or wire steps are not marked (dual)

- 1.5: the helper environment is a secret-bearing configuration view (`runner/environment.rs:9`).
- 1.7: the credential-free builder.
- 2.2: the change to the ordinary schema on the wire.

### P3-6: 2.1's verification rows do not name the Python client

2.1's listener-verification rows should run for both the CLI client and `SocketCoreBridge`.

### P3-7: Concurrent clients on one server have no locking statement

The server design serves concurrent sessions. 2.0 should state that two clients on one workspace get the same locking as two CLI processes.

### P3-8: The Pageant double is unproved

A Pageant double that records the caller's PID over the window protocol is a hypothesis. Check its feasibility in 0.3, as the stdio feasibility is checked.

### P3-9: 2.3 tests behaviour that only 2.4 builds

2.3's `windows_default` and generic-HTTP refusal rows need HTTPS at `cli`, which 2.4 builds afterwards. Reorder the steps, or move those rows to 2.4.

### P3-10: §3.1's R4 pass rule omits the helper variables

The R4 pass rule (:210) lists only `HOME`, agent, CA and proxy. The §3 table and 1.5 also require `GIT_CONFIG_GLOBAL` and `GH_TOKEN`.

## 3. Is the plan executable under the recommended options?

### D-H (b) and D-I yes

The plan is executable once these are closed in text:
- P1-1: tuple and service-class admission;
- P1-2: both Windows policies and redirect hops;
- P1-3: §16 amended, forwarding off;
- P2-1: no-snapshot `SessionOpen` and the requirement clauses;
- P2-2: where admission lives and how it fails without a terminal;
- P2-3: the stdio form for the separate-user leg;
- P2-4: the Windows R7 leg.

With those closed, no step is stuck, and Phases 0–2 run in the stated order, with P3-9's reorder.

### What breaks under the other options

- **D-H (a), trust the core.**
  - 2.0(c), 2.3's admission rows and R7's "cannot use the client's" must be rewritten.
  - OD16 must be re-decided explicitly, accepting NetNTLMv2 responses to core-named hosts.
  - The §1 threat model becomes false and must change.
  - P1-1 and P1-2 become recorded residuals rather than defects.
- **D-I no.**
  - P0-1's closure still holds, because 2.1 states the guards inline.
  - P1-3 and P2-1 disappear.
  - 6.3's `gwz server stdio` evidence path disappears with it. R8's two-machine row then has no mechanism, since pipe forwarding was dropped, and is stuck until 2.0(b) freezes a replacement stdio or remote form.

## 4. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `d3f669c85b879d87845aa5edefc81111157d079d87dc7e8ac8018a22cb75ed83`, 659 lines, status `??`, unchanged from the start. Nothing was modified during the review.
