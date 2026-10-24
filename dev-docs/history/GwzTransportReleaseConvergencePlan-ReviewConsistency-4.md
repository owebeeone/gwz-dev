# Transport execution plan (revision 4, sha256 1a9acf7a…) — CONSISTENCY CLOSURE CHECK

**Review object:** `dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 4, uncommitted, 846 lines, sha256 `1a9acf7a0e7ff2359a23cb33edb3e67fb15a0620df62ef3c8edc4d6141a310ff`.
**Baseline:** root `0288be15`, gwz-core `461ddf4b`.
**Date:** 2026-10-25.
**Axis:** Consistency. This is a closure check, not a new round. Read-only; the Safety axis runs in parallel.

**Verdict: NO-GO.** P2 2, P3 3. No new architectural root cause.
- **What is closed.** All six round-3 findings. D-L matches 1.2, 2.0(a) and 5.4.
- **What stops GO.** Both P2s are text-level and have fixes stated:
  - D-L's contract amendment is recorded incompletely, and no step builds the carrier-backed handle.
  - D-M's per-host default of 32 makes R1's pass rule unable to fail on its own 16-member workload.
- **Pre-commit:** GO on those two fixes, with no new P2.

## 1. Closure table

| Round 3 | Status | Evidence in revision 4 |
|---|---|---|
| **R3-A1** (architectural) | **Closed as decided (D-L)**; see R4-P2-1 | §1 terms; §1.1 contract §4/§12 row; 1.2 ("the binding's driver half lives in the core process behind the carrier-backed `HostHandle`"); 2.0(a) served-core composition; 5.4 "in-process under `local`, carrier-backed under `cli`"; §10 D-L |
| R3-P2-1 session design §5 | **Closed** | §1.1 row and 2.2b name the §5 clauses adopted (records, admission, §5.2 direct workers, §5.1 class W); one workspace per server; persistent client stays Phase 7; re-sized ~650 + 500 |
| R3-P2-2 client launch forms | **Closed** | 2.0(b) adopts `stdio` and `ssh://…` (server design :87–88) as amended; test-only launch hook for `sudo -n -H -u … env -i` and `CreateProcessWithLogonW`; both moved to 2.0's feasibility list (0.3 entry) |
| R3-P3-1 costs | **Partly closed** | Phase headers now match §9. §9's totals are still wrong (R4-P3-1) |
| R3-P3-2 stale status text | **Partly closed** | Diagram and Phase 0 review bullet updated. §12's column still reads "Disposition in revision 2" (R4-P3-3) |
| R3-P3-3 2.0(a), 1.2, paths | **Partly closed** | 2.0(a) wording fixed; exact-or-prefix path matching defined with canonicalization; 1.2 gains "The host owns endpoint instances and pools", but the old sentence "The host owns pools and endpoint sessions" is kept beside it (R4-P3-3) |

## 2. Is D-L consistent?

**With 1.2, 2.0(a) and 5.4: yes.**
- In 1.2 the binding's driver half lives in the core process and the client's owner creates the endpoint half.
- 2.0(a) has the core build the carrier-backed handle, so `run_git_via_gwz_git` receives an ordinary `Operation`.
- 5.4 states the one code path.
- Contract §3's edges are unchanged, as D-L says.
- Pool, reuse and admission stay with the client, which matches §1's threat model.

**With the contract clauses it amends: incomplete (R4-P2-1).**

## 3. Findings

### R4-P2-1 — D-L's contract amendment is incomplete, and no step builds the carrier-backed handle
D-L records amendments to contract §4 (line 155) and §12 only. A `HostHandle` whose `admit → acquire → register → open` cross the carrier also changes three other accepted clauses:

- **Contract §5.** `register` "is synchronous, non-cancellable, does no I/O, invokes no port/user callback". Revocation by host or domain close is "under the same authority lock used by register", with exactly "two legal race outcomes". Across a carrier, register needs I/O to the client's owner or a deferred commit. The client's `HostOwner::close` cannot take a lock in the core, and carrier loss adds a third outcome. None of this is stated.
- **Contract §7.** `PreparedOperation` contains "an opaque, bounded AuthContext capability captured on the original caller entry" and the `EndpointProfile`. In a served core the original caller is the client, and §1 forbids the core from holding those values. So the core cannot construct the `PreparedOperation` it would pass to `admit`.
- **Contract §4 and §4.1.** The carrier lives outside gwz-connection, so building a carrier-backed handle needs either:
  - a constructor and a port in gwz-connection, beyond "`HostPorts` is exactly four";
  - or a `HostHandle` that is a trait rather than a concrete type.

  §1.1's row says only that "a `HostHandle` may be carrier-backed".

No step implements the handle:
- 1.2 places the driver half;
- 2.0(a) is a document;
- 2.2b builds the operation service;
- 2.3 builds the server.

None of them builds the carrier-backed `HostHandle` or its client-side counterpart, which receives a remote admission and creates the endpoint half.

**Fix.**
- Extend the §1.1 contract row and D-L's "Amends" list to §5 (register and revocation across the carrier, including carrier loss as a revocation) and §7 (the client builds `PreparedOperation` at its entry; the core's admission refers to it).
- Name the constructor or port in gwz-connection.
- Assign the implementation to a step. 2.2b, or a new 2.2c after 1.2, both marked dual.

### R4-P2-2 — D-M (cap 32) makes R1's pass rule unable to fail on its 16-member workload
- §3.1 now sets "CAP = 32 at github.com". R1's rule is "creations ≤ CAP and peak ≤ CAP; `reused: true` on every operation after the CAP-th creation".
- With 16 members, the fetch creates 16. The push can create 16 more with no reuse (32, which is ≤ CAP). The rule applies only after the 32nd creation, so it never applies in that run.
- R1's outcome, reuse "across repositories and phases", can therefore pass with no reuse between fetch and push.
- The baseline sequence shows what is at stake: 64 creations, which is 16 per step.
- R2's rule has the extra clause "none after call 2" that keeps it honest. R1's rule does not.

**Fix.** Make R1's bound `creations ≤ min(CAP, members)`, and require "no creation in the push and read steps once the fetch has created its connections" (allowing for idle-closed replacements, as R6 does).

### R4-P3-1 — §9's totals are still wrong
The rows add to 127–157, not the stated 129–158.

### R4-P3-2 — §3.1 inconsistencies introduced by the baseline text
- **Workload C's size.** Workload C is defined at 16 members (line 192). The baseline reports "one Python `Client` over 52 fetches of one member".
- **Protocol counts.** "Physical and protocol counts must agree", but `--json` drops `reused` and `connection_id` until step 3.0 (Phase 3). Phase 1 and Phase 2 rows (R3, R4, R7's "client count = R1's") can rely on physical counts only. Either move 3.0 before 1.8, or say that the agreement rule applies from 3.0 on.
- **Windows SSH push.** "Windows push rows are HTTPS-only until the fixture is fixed", but no step fixes the fixture. R1's W/SSH push cell has no route to a number. The §3 rule ("a GO without the row's numbers is not a GO") needs a step for the fixture, or a written operator exception.

### R4-P3-3 — Leftover text
- 1.2 still says "The host owns pools and endpoint sessions through `EndpointFactory`" next to the corrected sentence.
- §12's column header still reads "Disposition in revision 2".

## 4. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `1a9acf7a0e7ff2359a23cb33edb3e67fb15a0620df62ef3c8edc4d6141a310ff`, 846 lines. This matches the pin given at the start. Nothing was modified.
