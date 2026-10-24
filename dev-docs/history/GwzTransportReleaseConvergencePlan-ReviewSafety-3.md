# Transport execution plan (revision 3, sha256 fb0062e3…) — SAFETY-AXIS REVIEW, round 3

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 3, 754 lines, sha256 `fb0062e3eec64dcb814c67cfa7d2a8c7a25c0c55c5aca967c0f075bf2abb4854`. It is an uncommitted draft (`??`).
**Baseline:** gwz-core `461ddf4b`, gwz-py `71d2d07`, gwz-cli `cea7bd4`, gwz-transport `a27817b`.
**Date:** 2026-10-24.
**Axis:** Safety. The review is independent, adversarial and read-only. The Consistency axis was not consulted. This report is filed verbatim as `GwzTransportReleaseConvergencePlan-ReviewSafety-3.md`.

**Verdict: NO-GO.** P0: 0, P1: 0, P2: 2, P3: 5.

**No new architectural root cause.** Both P2s are definitional corrections inside 2.0(c) and §3.1. I pre-commit to GO when they are closed in text, without needing another full round.

## 0. Evidence base

- **Object hash.** Verified at start; it matches the pin (§3).
- **Revision 3:** read in full (lines 1–754), including §12's "Round 2" rows.
- **Round-2 report:** used as the closure baseline.
- **gwz-cli command set:** read to classify commands by service class. Files: `src/*_long.rs`, `forall.rs`, `pushargs.rs`.
  - `forall` runs its command directly in the client process (`std::process::Command`).
  - `repo sync` and `repo create` are local-only.
  - `materialize` can clone, which is a clone-family route and is refused under `cli`.
- **§3.1 (lines 160–255):** checked for `--server` rows or allowed sets. There are none.
- **Earlier evidence still in force:** the parked server design, the session crates, `prepare.rs:118-121` and `transport_binding.rs:190-200`.

## Problem, requirement provenance, and scope

- **Revision 3 is written on the recommended options:** D-H (b), D-I yes, and the D-C reversal. It says so for each (§10).
- **Threat model.** §1 :47–52 states it, and it traces to D-H.
- **§1.1 replacements.** The table now records replacements for:
  - server design §3, §4 and §16;
  - `GWZRequirements.md:108`;
  - `GWZDesign.md:104,108`;
  - `session-contract` `frame.rs:25`.
- **Scope.** No step opens a listener, adds a dependency or publishes anything without a §10 decision.

## 1. Closure of round-2 findings

| Round 2 | Revision 3 text | Status |
|---|---|---|
| P1-1 admission granularity | §1 :47–52; 2.0(c) :420–435: (scheme, host, port, path), service class from the client's command, client-chosen identity, empty default for `--server`; rows in 2.3 | **Closed in principle.** The tuple and push-scope definitions are residual (new P2-1) |
| P1-2 Windows native policies, redirects | 2.0(c) gates both policies; admission re-applied at each hop; rows in 2.4 | **Closed** |
| P1-3 ssh forwarding | §1.1 replaces §16; 6.3 launches `ssh -a -x -T -o ForwardAgent=no -o ClearAllForwardings=yes`; `server stdio` refuses an inherited agent or credential variable | **Closed.** Robustness of the `SendEnv` stripping is P3-2 |
| P2-1 snapshot, server credential model | §1.1 rows; 2.0(a)/(b); 2.1 refusal row with canaries covering the handshake | **Closed** |
| P2-2 where admission lives, no terminal | the `cli` `HostHandle` before any credential step; `--allow-destination`, `gwz.server.allow`, `allow=`; fails closed | **Closed.** The §3.1 consequence is new P2-2 |
| P2-3 separate-user leg vs peer check | 2.5 uses the stdio form; 2.1 has a no-override source check | **Closed.** Mechanics are P3-1 |
| P2-4 R7 on Windows | hosted Windows runner gets a separate user; the dabeest leg is narrowed; the R7 row is reworded (:148) | **Closed** |
| P3-1 binding scoped to its carrier | 1.2 sentence and row | Closed |
| P3-2 prompt text | 2.0(c): canonical, control-stripped, punycode | Closed |
| P3-3 D-E object rules | 1.1: per-process name, fail closed on a foreign owner, residual | Closed |
| P3-4 self-minting gate constructors | 1.1 removes them, with a source check | Closed |
| P3-5 dual marks | 1.5, 1.7, 2.2 marked | Closed |
| P3-6 Python client rows | 2.1 rows for both clients | Closed |
| P3-7 concurrent-client locking | 2.0(b) | Closed |
| P3-8 Pageant double | 0.3 feasibility check | Closed |
| P3-9 Windows and HTTP rows ordering | moved to 2.4 | Closed |
| P3-10 R4 pass rule | §3.1 :89 includes `GIT_CONFIG_GLOBAL` and `GH_TOKEN` | Closed |

## 2. Findings on revision 3

### P2-1: The admission tuple is not defined tightly enough. Path matching, the account, and push scope each leave a way past the rule.

**(a) Path canonicalization.** `--allow-destination <scheme://host[:port]/path>` (:431) does not say whether the path is an exact match or a prefix. It also gives no canonical form for:
- SCP-style paths, `~`, `.git` suffixes and case;
- percent-encoding;
- `.` and `..` segments.

Without these, an allowance of `/owebeeone/` admits `/owebeeone/../other-org/repo`, which the core chose. Whether that reaches another repository depends on the server's path normalization, and the client never sees it.

**(b) The account.** `Destination.ssh_username` and `https_username` come from the core. The username is part of who the client authenticates as. The HTTPS helper lookup is keyed by user and host, so a core-chosen username can select a second stored account, for example an admin token, for an allowed path. That breaks §1's "never by the core … as whom". The tuple omits the username, and the "client-chosen identity" rule covers key paths only.

**(c) Push scope.** "Push-class admit receive-pack for the named repositories only" (:424) cannot be enforced as written:
- the client knows only member ids or operands;
- the core maps those to URLs;
- one allowed set serves both fetch and push.

So a push-class command admits receive-pack to every allowed tuple, for example all 16 members under one prefix, whichever member the user named.

**Fix:**
- Define path canonicalization, refusing dot segments and encoded separators, and state exact or prefix matching.
- Make the username a client choice, or put it in the tuple.
- Either add a separate push allowance (`--allow-push` / `allow_push=`), or record against D-H that a push may reach any allowed tuple.

### P2-2: §3.1 has no `--server` rows, although 2.0(c) says it does. Under the fail-closed default, the "both placements" measurements and R7's CI rows cannot run as written.

- **The claim.** Line 435 says "§3.1's `--server` rows name `github.com` and the fixtures explicitly".
- **The recipe.** §3.1 (lines 160–255) defines no `--server` variant: no server start, no address, no allowed set, no row. The recipe is "fixed here; later phases rerun it unchanged".
- **What depends on it:**
  - 3.4 measures R1–R3 "in both placements";
  - Phase 3's exit and R8 require both placements;
  - 2.5's rows need allowed sets for the fixture hosts and ports.
- **The effect.** Under the empty default, every such run fails closed. Running them anyway means inventing the allowed set ad hoc, outside the recipe.
- **Fix.** Add the `--server` variant to §3.1:
  - the start command and address;
  - the exact allowed tuples (fixtures with their ports; github.com paths for the fetch legs; the operator's scratch repositories for push);
  - the service class each workload exercises.
- **Note on host-only grants.** A grant that names only a host, which the line-435 wording suggests, is a full grant. Say so where it is used.

### P3-1: The separate-user stdio leg needs its exact mechanics stated

- **sudo flags.** Use `sudo -n -H -u <core-user> env -i … gwz server stdio`.
- **Instrumentation.** sudo strips `LD_PRELOAD` and `DYLD_*`, so §3.1's `connect()` counter must be injected after sudo.
- **Inherited agent.** Some sudoers configurations keep `SSH_AUTH_SOCK`. 2.3's refusal would then fail the leg, which is safe but not a test of anything.
- **Precondition row.** Assert that the canary key file and key directory are unreadable by the core user before the leg runs.
- **Windows.** "Or the Windows equivalent" is unnamed. Name the mechanism: a local test user with a generated password, launched with credentials and redirected stdio (`CreateProcessWithLogonW`, or .NET `ProcessStartInfo`). Add it to 0.3's feasibility checks, as Pageant and `ssh -a` already are.

### P3-2: "No credential `SendEnv`" is not portably enforceable through ssh options

- **Why options don't suffice.** The user's `~/.ssh/config` `SendEnv` lines cannot be cleared on older OpenSSH, including the version bundled with Windows. Removing them with `-F none` would also drop the host settings the login needs.
- **The robust form.** Launch `ssh` with credential variables removed from *its* environment. `SendEnv` only sends variables that are present. Keep `SSH_AUTH_SOCK` for ssh's own login; `-a` stops it being forwarded.
- **Missing list.** Define the credential variables that the client scrubs and `server stdio` refuses (`GH_TOKEN`, `GITHUB_TOKEN`, `GIT_ASKPASS`, `SSH_ASKPASS`, `GCM_*`, …).
- **Why the core-side refusal isn't enough.** It runs after the value has already reached the core machine.

### P3-3: The transport-lane decision in 2.0(a) needs a binding constraint on control progress

- **The risk.** If 2.0(a) chooses long-poll, transport control (Window, Cancel, terminal frames) travels inside call replies on the call lane. There it sits behind in-band diff and log spools, outside `session-channel`'s control reserve. 2.1's saturation row covers only the framing.
- **Fix.** State that either option keeps transport control in the control reserve. Add a 2.2b row: cancel during a saturated spool attachment.

### P3-4: Service classes need a closed client-side table, with unclassified requests refused

- **The table.** Classify each request type, not each command name:
  - fetch-class: `fetch`, `pull`, remote reads;
  - push-class: `push`, including its preflight and post-push reads.
- **Unclassified requests.** Requests not in the table are refused under `--server`. Examples: a new command, `forall`, hooks. `forall` runs client-side with member paths the core reports.
- **Behaviour changes to document in 2.0(b) and 6.4:**
  - `materialize` that would clone, and `local clone`/merge, are refused (clone-family and local routes);
  - repository-configured identities (`gwz auth identity`) are refused under `--server`, as "core-named identity path is refused" implies.

### P3-5: The scope of the `gwz.server.allow` setting is unstated

If the setting lives in global git configuration, it applies to every server the user ever addresses. State whether it is keyed by server address, and document in 6.4 that a host-only entry grants every path on that host.

## 3. Is the plan executable as written under the recommended options?

Under D-H (b), D-I yes and the D-C reversal: **almost, but not as written.**

- **Phases 0, 1 and 2.0–2.4** are executable as written.
- **2.5 and 3.4 would stall on P2-2.** The fail-closed default refuses every `--server` run, because no recipe supplies the allowed sets.
- **P2-1 does not stop execution,** but leaves the R7 claim "admitted only for the client's allowed … and service class" (:148) weaker than stated.

Closing P2-1 and P2-2 in text, and ideally the five P3s, makes the plan executable end to end. No operator decision is needed beyond the open D-H, D-I, D-C, D-J and D-K.

## 4. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `fb0062e3eec64dcb814c67cfa7d2a8c7a25c0c55c5aca967c0f075bf2abb4854`, 754 lines, status `??`, unchanged from the start. Nothing was modified during the review.
