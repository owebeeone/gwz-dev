# Step 1.6 cross-binding reuse authorization: CODE-AXIS RE-VERDICT, round 2

**Verdict: GO.** All six round-1 findings are closed on this tree. There are no new findings (P0–P3: 0 open).

**Revised tuple** (the same at start and end; members clean):
- gwz-transport `055d091` = `origin/main`. Range reviewed: `9fbe7448..055d091`. The base `9fbe7448` is 1.5's accepted remediation and contains `a21e972`.
- gwz-core `9734f3f8`. Range reviewed: `726e9062..9734f3f8`.
- Evidence `f18023cc` (`rem1/`). Its one untracked entry is the same unrelated `2026-09-22-alpha-setup-timeout/` as in round 1.

**Commands run** (load 3.7–4.1):
- `cargo test -p gwz-connection`: 75 passed.
- `cargo test -p gwz-transport --test pool_unused`: 5 passed.
- In gwz-core with the transport build flag on, the `p1_6_` rows (`SSH_AUTH_SOCK` unset): 16 passed and 1 ignored (the known-defect row). No warnings in the files this step touched.
- The review target directory has been deleted.

## Prior-finding closure

| Round-1 finding | Fix on this tree | Original counterexample re-run | Status |
|---|---|---|---|
| **P2-1** (architectural): a late verdict from a dropped proof served another binding's lease; no way to cancel a proof | `ProofId` is minted by the host (`next_proof`, never reused) and carried in both `ReuseCommand` and `Completion::Reuse`. `reused()` now matches on proof id **and** connection. `cancel_reuse(proof)` is called on abandon and on host seal (`fail_in_flight`). The fakes end proofs one command at a time (`end_reuse_of`). | Row `p1_6_rem_a_late_verdict_of_a_dropped_openings_proof_serves_no_other_binding`: B's proof is held and B's opening dropped; C then leases the same connection. B's late `Authorized` leaves C unserved. C's own `Revoked` gives C a new connection. B's proof is cancelled. | **Closed** |
| **P3-1**: an agent answering `rsa-sha2-512` with `ssh-rsa` was treated as revoked, retiring connections | `verify_proof` (`agent_keys.rs`) now returns `Proof::Unsupported` for an RSA reply named `ssh-rsa`, which becomes `Unprovable`. | Row `p1_6_rem_an_agent_that_answers_ssh_rsa_is_unprovable_and_keeps_the_connections`, with a downgrade fixture on Unix and the pipe agent's `OneByteRsa` on Windows. | **Closed** |
| **P3-2**: RSA keys outside 2048–8192 bits were sent to the agent before being refused | `provable(blob)` reads the modulus size before any agent request. | RSA-1024 is now a case in the possession row: result `Unprovable`, zero sign requests. | **Closed** |
| **P3-3**: the helper-token result was an uncaptured `eprintln` | The connection leg is asserted. The route leg is now asserted as a named open defect (`OPEN: routes keyed by operation name; owner 1.3c/C2(b)`). An ignored row records the desired behaviour. | Ran: the asserted row passes; the ignored row shows as ignored with that reason. | **Closed** (route defect recorded, owned by 1.3c/C2(b)) |
| **P3-4**: no crate rows for the merged lease order or for sealing during a proof | Two new crate rows. | `p1_6_rem_a_misfit_lease_another_binding_last_used_is_given_back_with_no_proof` (no proof is asked for). `p1_6_rem_sealing_the_host_during_a_proof_closes_the_opening_and_ignores_the_late_verdict` (opening gets `Closing`, proof cancelled, late verdict starts nothing). | **Closed** |
| **P3-5**: cancelling the last waiter cancelled the shared proof job | A job with no waiters is kept until it ends under the deadline it started with (`runner.rs`). If it passes, the result is recorded for the binding (`passed` and `record`). A later lease by the same binding joins it. | Row `p1_6_rem_a_proof_outlives_its_cancelled_waiter_and_is_kept_for_the_binding`: total agent signs go up by exactly 1. | **Closed** |

## The authorized pool change

The remediation plan authorized a narrow gwz-transport pool change, `release_unused` plus `Request::exclude`. I checked it:
- It is limited to an `idle_since` field on an entry (restored only on an unused release), the exclusion check in idle selection, and a bound of `MAX_EXCLUDED = 64` enforced by `valid()`.
- A lease that is dropped is still released as `Discarded`.
- gwz-core's `PoolHost::release_unused` discards a connection found lost or not reusable.
- Tests:
  - `pool_unused.rs`: exclusion behaviour, the bound, and idle time kept across an unused release.
  - Crate row `p1_6_rem_a_bounced_connection_expires_from_its_last_real_release`, and a gwz-core row of the same name: the bounced connection expires 60 s after its last real release.
  - Crate row `p1_6_rem_a_binding_reuses_its_own_idle_connection_beside_one_it_cannot_prove`: two connects, not one per open.
  - gwz-core row `password_helpers::p1_6_rem_a_binding_reuses_its_own_helper_connection_beside_another_bindings`.

## New findings

None.

## Things the lane owner should record (not defects)

- **Retry after a revocation changed.** After a revoked proof, the retry now asks again excluding that connection instead of asking for a fresh one. This applies in the crate (`reused` revoked branch, then `ask_again`) and in gwz-core (`settle_proofs` refused/untrusted branch, then `excluding`; `held.rs` `Revoked`). This differs from the wording of the reuse design §4 outcome table, "a fresh checkout that takes no idle connection". The remediation plan names this behaviour only for unprovable and no-evidence leases. The effect is sound: matching idle connections were already retired, and any other idle connection still needs its own proof, at a cost comparable to a new handshake. But the table's Revoked row should be amended in the contract's §15 1.6 entry.
- **Correction to my round-1 suggested §15 text:**
  - The proof is now correlated by the host-minted `ProofId` and cancelled with `cancel_reuse`.
  - Bounced leases go back to idle with `release_unused` and are retried with exclusion (fresh only past the 64-connection exclusion bound).
  - The route-keying defect stays open with owner 1.3c/C2(b).
