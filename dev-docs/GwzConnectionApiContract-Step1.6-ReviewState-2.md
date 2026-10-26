# Step 1.6 cross-binding reuse authorization — STATE-AXIS RE-VERDICT (round 2)

**Review object:** gwz-transport `9fbe7448..055d091` (crate and the narrow pool change) and gwz-core `726e9062..9734f3f8`. Evidence: `f18023cc`, `runs/2026-10-25-p1-6/rem1/`.
**Tuple:** the same at start and end: gwz-core `9734f3f8b5`, gwz-transport `055d091b4b`, evidence `f18023ccba`. Both worktrees are clean.
**Date:** 2026-10-26
**Axis:** State
**Verdict:** **GO.** P0 0, P1 0, P2 0, P3 1 (new, non-blocking).

**What I ran:**
- `cargo test -p gwz-connection` in gwz-transport: 75 passed, including all five `p1_6_rem_*` rows.
- `cargo test --test pool_unused`: 5 passed.
- `RUSTFLAGS='--cfg gwz_transport_candidate' cargo test --lib p1_6_` in gwz-core, without the inherited `SSH_AUTH_SOCK`: 16 passed, 1 ignored. The ignored row is the named open defect.
- I deleted my review target directory afterwards.

**What I read:**
- The remediation diffs.
- The rem1 README, its mutation list, and the Linux summary (OVERALL PASS).
- The Windows final-gates table: receipt `c` PASS, with the same 4 named host gaps as round 1.

## Prior-finding closure table

| ID | Round-1 counterexample | Closure on this tree | Status |
|---|---|---|---|
| **S-P2-1** (architectural; = C-P2-1) | B's proof is dropped mid-run, C leases the connection, and B's late `Authorized` serves C | The host mints a `ProofId` per `authorize_reuse` call. `ReuseCommand` and `Completion::Reuse` both carry it, and `reused()` now matches on proof and connection together, so a late completion can only end its own proof (`instance.rs:636–650`). `abandon` and `fail_in_flight` call `cancel_reuse(proof)`. Adapters cannot forge a `ProofId` (`ProofId(pub(crate) u64)`). Closure row `p1_6_rem_a_late_verdict_of_a_dropped_openings_proof_serves_no_other_binding` is green; mutation MR1 (ignore the proof) is killed; the seal row plus MR7 cover `cancel_reuse` on seal. gwz-core's worker was never exposed, because its waiters belong to their own binding's `SharedProof`. | **Closed** |
| **S-P2-2** | An unprovable connection is bounced back to idle, its idle clock restarts, and every other binding's opens go fresh, skipping their own idle connections | Pool: `release_unused` restores the `idle_since` stamp from the idle lease. `Request::exclude` (at most `MAX_EXCLUDED` = 64) is honoured in `schedule`; past the bound the request goes fresh. Both paths bounce with `release_unused` and ask again with the bounced connection excluded: crate `ask_again`, and gwz-core `screen`, `settle_proofs`, `fail_waiters` and the expiry path. The binding's own idle connection is offered next. The bounced connection expires from its last real release. Rows: crate own-reuse and expiry rows, `pool_unused` ×5, gwz-core `password_helpers::…own_helper_connection…` and `reuse_auth::…bounced_connection_expires…` (P-521). Mutations MR2 (re-stamping) and MR3 (exclusion ignored) are killed. One cost remains, bounded: until the bounced connection's idle time runs out, each new open by the other binding still starts one proof job for it (an agent connection, no sign request). | **Closed** |
| **S-P3-1** (= C-P3-2) | RSA keys outside 2048–8192 bits were sent to the agent | `provable(blob)` reads the modulus size before any agent request. The RSA-1024 case asserts zero sign requests, and the Windows pipe fixture now lists the key without backing it. | **Closed** |
| **S-P3-2** (= C-P3-5) | The shared job was dropped with its last waiter | A job with no waiters now runs on under its own deadline, and a pass is recorded into `Proofs` (`runner.rs:612–628`). Row `…proof_outlives_its_cancelled_waiter…` is green. | **Closed** |
| **S-P3-3** (= C-P3-3) | The token probe was in no filed log, and the row's claim was overstated | The row asserts the current route-leg defect by name (`OPEN: … owner 1.3c/C2(b)`). An `#[ignore]` row holds the desired behaviour. `helper-token-nocapture.log` is filed, and the README says "connection leg only". | **Closed** |

## New findings

**N-P3-1 — A revoked proof now retries non-fresh, where the design says fresh (not architectural).**
- **Where:** crate `instance.rs` (`reused`, the `Revoked` arm calls `ask_again` with an exclusion); gwz-core `held.rs::Unserved::Revoked` (refused or untrusted waiters retry with an exclusion).
- **Rule departed from:** reuse design §4's outcome table says a negative proof "continues with a fresh checkout that takes no idle connection". Its "Why retire" rationale is that a denied confirmation should be asked only once, not once per pooled connection.
- **gwz-core:** benign. `retire_idle` marks the siblings that share the key or host key before the retry. A sibling the pool still offers is `lost()`, so the open goes to `Dead` and then fresh, and no second proof starts.
- **Crate:** the outcome depends on completion order, which the port does not specify. If an adapter reports `Reuse{Revoked}` before the siblings' `IdleClosed`, the retry can lease a sibling with the same key. That runs `authorize_reuse` again, which means another confirmation prompt for a key just denied.
- **Severity:** fail-closed. No authority is gained, only an extra proof or prompt.
- **Correction:** use `fresh` (exclusion cleared) on the `Revoked` arm in both paths, or write into `ports.rs` that a sibling's `IdleClosed` must come before the `Revoked` completion.
- **Closure:** a crate row where the fake reports `Revoked` before the siblings' `IdleClosed` with two same-key idle connections, asserting `reuses().len() == 1`.

## Observations (not findings)

- **Kept proof jobs have no cancellation at binding end.** The gwz-core worker has no binding-end signal. Each kept job is bounded by its own deadline, the supervisor's job cap and shutdown's drain of `proving`.
- **The final gates ran on uncommitted worktrees, not the commits.**
  - The Linux matrix and Windows receipt `c` ran on gwz-core `726e9062` and gwz-transport `9fbe7448`, each plus the uncommitted rem1 patch, with a pin override. Neither ran at the committed `055d091`.
  - The coordinator states that this worktree content equals the commits. I did not verify that by digest this round.
  - The macOS focused rows I ran were on the committed tree.
- **Seal and misfit rows are crate-only, against the fake adapter.** gwz-core's worker has no seal path until the 1.3b/1.3c adapters wrap it.
- **The crate's ask-again loop depends on the pool honouring `exclude`.** With a pool that ignored it, the loop never ends; the pool rows guard this (MR3).
- **Carried, unchanged:** O1, HTTPS routes keyed by operation name (owner 1.3c/C2(b)); O2, no maximum connection age (owner C3).

GO on this tuple. N-P3-1 can ride with the next touch of these files.
