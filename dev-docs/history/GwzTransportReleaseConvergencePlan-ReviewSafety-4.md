# Transport execution plan (revision 4, sha256 1a9acf7a…) — SAFETY-AXIS CLOSURE CHECK

**Object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 4, 846 lines, sha256 `1a9acf7a0e7ff2359a23cb33edb3e67fb15a0620df62ef3c8edc4d6141a310ff`. It is an uncommitted draft (`??`).
**Date:** 2026-10-25.
**Axis:** Safety. This is a closure check, not a new round. It is read-only and peer-blind. It is filed verbatim as `GwzTransportReleaseConvergencePlan-ReviewSafety-4.md`.

**Verdict: NO-GO.** P2: 2, P3: 4. Every round-3 finding is closed.

The two P2s come from how D-L is recorded: two statements are missing about what a carrier-backed handle may carry. They are **not a new architectural root cause**. D-L stands as decided, and each P2 closes with one sentence plus one red row in 2.0(a) or 1.2.

**Pre-commit:** GO when N-1 and N-2 are closed in text.

## 1. Closure of round 3

| Round 3 | Revision 4 text | Status |
|---|---|---|
| P2-1 admission tuple | 2.0(c) :479–487 covers four points: canonicalization (`.`/`..` and encoded separators refused; SCP form, `~`, `.git` and case normalized per scheme); exact match, or prefix when the allowance ends in `/`; username chosen by the client; a separate push allowance (`--allow-push`, `gwz.server.allow-push`, `allow_push=`) | **Closed** (see P3-c on the username default) |
| P2-2 no `--server` rows | §3.1 :232–242: `--server` variant with explicit tuples for each workload; github.com fetch-only; fixtures with ports; scratch repositories for push; no host-only grants | **Closed** |
| P3-1 separate-user launch | 2.0(b) :466–469 names `sudo -n -H -u … env -i` and `CreateProcessWithLogonW`; feasibility check is in 2.0's list | **Closed** (see P3-d on where the hook lives) |
| P3-2 credential variables | 2.0(c) :497–500 strips the named list from `ssh` and `sudo` children, with `-a`; `server stdio` refuses them | **Closed** |
| P3-3 control reserve | 2.0(a) :441–443 constraint; 2.2b row :536–537 | **Closed** |
| P3-4 service classes | 2.0(b) :469–475: a closed table by request type; unclassified types refused; the named commands each given a disposition | **Closed** |
| P3-5 allow scope | settings keyed by server address; a host-only grant is documented as a full grant (:491–494) | **Closed** |

## 2. Safety read of D-L (carrier-backed `HostHandle` in the core)

**What holds.** Pool, reuse and admission policy stay at the client's owner (§1 :62–65, D-L :781–787), and `Operation` and `Exchange` keep no public constructor. A carrier-backed handle in the core does not by itself place a credential in the core. But the recorded text leaves open what the handle's calls carry across the carrier, and two of those calls can carry policy.

### N-1 (P2): `admit(PreparedOperation)` would let the core supply the client's profile, authentication context and policy.

- **What the contract says.** The contract's `PreparedOperation` holds the `EndpointRoute`, the `EndpointProfile`, the `OperationPolicy` (capacity, timeout, retry) and an opaque `AuthContext` "captured on the original caller entry" (contract §7; plan 1.5).
- **What D-L says.** "Admit → acquire → register → open travel over the carrier" (:63).
- **What is unstated.** No text says which side constructs `PreparedOperation`.
- **What could go wrong.** If the core's handle forwards one, the less-trusted side chooses:
  - the profile (home, agent source, CA);
  - the authentication context;
  - `--max-per-host` and retry policy.
  That contradicts §1's "never by the core … as whom".
- **Fix.** The carrier-backed admit carries only an operation reference. The client's owner builds `PreparedOperation` from its own invocation and capture. Add a red row: a core-supplied profile or policy field is refused.

### N-2 (P2): Under D-L and 5.4, core-side gwz-git opens generic `ExchangeSpec::Http`, but 2.0(c) admits only `GitService`-typed opens.

- **The conflict.**
  - Contract §7.2 and §9: gwz-git maps service semantics to a generic `ExchangeSpec { origin, method, target, headers }`.
  - Plan 5.4 :646–647: gwz-git receives "the same `Operation` under `local` and `cli`".
  - 2.0(c) :487 and the 2.4 row: generic HTTP is refused.
- **The two outcomes as written:**
  - the client refuses every HTTPS open from 5.4 in `cli` placement, which stalls Phase 5 for HTTPS; or
  - an implementation relaxes admission to generic HTTP, which reopens round 2's concern about core-chosen method, target and headers carrying the client's credential.
- **Fix.** In 2.0(a), state that the carrier-backed `open` sends only the existing typed Open (scheme, destination, `GitService`, identity selected by the client). The core-side handle maps a gwz-git `ExchangeSpec` to it through a closed table of Git smart-HTTP service paths and refuses anything else. Add rows in 2.4: an unmapped `ExchangeSpec` is refused core-side, and the client still refuses generic HTTP.

### P3-a: Two smaller D-L points should be stated

- **Admission side.** 2.0(c) :421 says admission "lives in the client endpoint (the `cli` `HostHandle`…)". With two `HostHandle`s now, state that admission is enforced only at the client's owner on receipt, never in the carrier-backed handle.
- **Snapshot.** `HostHandle::snapshot()` on the carrier-backed handle must be refused, or redacted to counters. Otherwise it exposes the client's endpoint and profile metadata to the core.

## 3. Safety read of D-M (per-host default 32)

D-M is acceptable as a chosen default.

### P3-b: The default rests on partial measurements and can fail repositories outright

- **HTTPS.** The sweep measured HTTPS only up to 16, on three hosts. 32 for HTTPS is an extrapolation. §3.1's R1 uses CAP = 32 for both schemes.
- **Ordinary sshd limits.** A typical sshd (`MaxStartups 10:30:100`) drops early connections well below 32. With `--max-retries 0` the adaptive limit never lowers, so those repositories fail outright (`TransportPlacement.md`, per-host limits).
- **Agent prompts.** Confirmation-required agent keys (`ssh-add -c`, Pageant confirm) can raise up to 32 concurrent prompts.
- **Fix.** Record these in 6.4. Either run the HTTPS sweep to 32, or state the extrapolation.

## 4. Other residuals from the new text

### P3-c: The client-chosen SSH username has no default

The allowance syntax `scheme://host[:port]/path` carries no user, and the core's `ssh_username` is refused. State where the username comes from: `user@` in the allowance, else the client's SSH configuration, else `git`. Without that, every SSH row fails or an implementation falls back to the core's value.

### P3-d: The test-only launch hook must stay out of production code

The `sudo`/`CreateProcessWithLogonW` hook belongs in the test harness, not the client binary, by the same source check as 2.1's no-override rule. It handles a generated password.

## 5. Object check

`shasum -a 256` at end gives `1a9acf7a0e7ff2359a23cb33edb3e67fb15a0620df62ef3c8edc4d6141a310ff`, 846 lines, status `??`, unchanged from the start. Nothing was modified during the review.
