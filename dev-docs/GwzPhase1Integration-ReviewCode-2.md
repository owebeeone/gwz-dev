# Phase 1 integration (step 1.6 SSH adapter proof): code-axis re-verdict, round 2

**Verdict: GO**, provided matrix `root-p1-20261027-b` passes. Both P2s and all three P3s from my last round are closed on this tree. I found one new P3, which is not architectural and does not block.

**Tuple** (the same at start and end, members clean): gwz-transport `3980469918` (`dd93098..`), gwz-core `22aebceeb4` (`844f9511..`).

**Is this the content the matrix runs?** Yes. Run b's gwz-transport diff digest `83d2d3f2…` equals `git diff dd93098 3980469`. The three untracked gwz-core files hash the same as the committed ones. The tracked gwz-core diff equals run b's `30acc931…` except for the pin and the allowlist `reconciled_commit`, which are not compiled.

**What I ran:** `cargo test -p gwz-connection` at `3980469` gives 84 passed; the scratch target directory is deleted. The matrix-b result is not in yet: there is no `remote-root-p1-20261027-b` directory.

## Closure table

| Finding | Fix | Counterexample re-checked | Status |
|---|---|---|---|
| **P2-1** (architectural): a proof was not bounded by the opening's deadline | Host side: `expire_authorizing` ends a waiting opening at `Pending.deadline` with `AllocationTimeout`; it cancels the proof and returns the lease to idle, and `next_deadline` now includes those openings. `cancel_authorizing` runs before the pool cancels the binding. Adapter side: `ReuseCommand.remaining_ms`, and `proof_deadline` takes the earlier of that and the connect bound. | Crate rows: `…the_openings_deadline_during_a_held_proof_ends_it_with_an_allocation_timeout`, `…the_drivers_next_deadline_includes…`, `…the_proof_command_carries_what_is_left…`. Adapter: pure-function row on `proof_deadline` (including connect bound 0). | **Closed** |
| **P2-2**: concurrent leases by one binding each ran their own proof job | `Running.commands`: a lease joins an existing job with the same binding, key, host and host-name list. `cancel_reuse` removes only that lease; the job keeps running and a pass is still recorded. | Connection-host row: 3 concurrent leases produce `signs() == 1` and 3 sessions served. | **Closed** |
| **P3-1**: trust re-check used only the lowercased host | `Evidence.written_host` is recorded in `native_connector`; `trust_names` checks both spellings, as the old worker does. | Unit row on `trust_names`. | **Closed** |
| **P3-2**: retirement and verdicts could hit connections the host had leased | `Claims` (one lock) means a connection the host leased is never retired, and a retired one is refused with `NotConnected`. The crate's `serve` treats that as a lost lease, discards it and asks again within the misfit bound. Proofs skip connections other proofs wait on, and a lease whose connection is no longer idle gets `Unprovable`, not `Authorized`. | `Claims` unit rows and the crate row `…a_lease_the_adapter_finds_retired_at_start_asks_again…`. A discarded lease goes to `Closing`, so it cannot be offered again in the same pass. | **Closed** |
| **P3-3**: no live-path row for the agent possession check | Connection-host rows with a private agent: key withdrawn revokes the connection and its siblings; a held key is proved once and the proof is kept for the binding. | Present in the diff. | **Closed** |

## The implementer's departures

- **`remaining_ms` instead of an absolute deadline: accepted.** The worker computes its deadline when it receives the command, so it can run late by the time the command spends in the queue. That does not matter, because the host now enforces the opening's deadline itself.
- **`Claims` plus a fake-adapter crate row: accepted.** The two sides take one lock, so exactly one of them wins, and the unit rows show it.
- **No real-host deadline row: accepted.** The host-side enforcement is covered by crate rows.
- **A rejected frame now goes through `cancel_binding`: accepted.** `route` holds no host, pair or instance lock at that call, so there is no lock-order risk, and there is a red-first row.
- **To record in contract §15 (not a finding):** a shared job runs under the **first** waiter's deadline, not the latest waiter's, and it is not restarted for later joiners. A joiner with more time left can therefore get `Unprovable` early and open a new connection. That fails closed, but it departs from the old worker and from the latest-waiting-request rule.

## New finding

### P3-1 (new, not architectural): a proof that ends on an idle-lost connection puts a dead connection back in the pool as idle
- **Where:**
  - gwz-transport `instance.rs`: `reused` (`Unprovable` arm) and `end_authorizing` call `release_unused` without checking `state.connections`.
  - gwz-core `reuse.rs` `settle_proofs`: the new `still_idle` check reports `Unprovable` for a connection that is gone.
- **What goes wrong:** a connection the host has leased for a proof (with an opening waiting on it) can still be lost while idle, because the peer closes it. The `IdleClosed` completion removes the crate's setup record, but the pool refuses `idle_closed` for a leased entry (`lifecycle.rs:228-232`). The proof then ends `Unprovable` (or the opening is cancelled or times out), and `release_unused` puts the dead connection back in the pool as idle.
- **Impact:**
  - The dead entry holds a capacity slot until it is evicted, expires after 60 s, or is leased again.
  - The next lease of it goes down the no-setup-record path, costing one discard and one ask-again, and a `Close` for a connection the adapter no longer holds.
  - This is bounded and nothing leaks.
- **Fix:** whenever an `authorizing` lease ends without being served, check whether its setup record still exists. If it does not, release as `Discarded` (and use `lost_lease` for the opening) instead of `release_unused`.
- **Closure test:** a crate row that holds a proof, calls `idle_closed_of(connection)`, then ends the proof `Unprovable`. The pool's idle count must not include that connection, and the opening must be served on a new one.

**Next step:** confirm matrix `root-p1-20261027-b` passes. Push gwz-transport `3980469` before the gwz-core pin, under the bump rule.
