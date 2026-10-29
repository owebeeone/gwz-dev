# Step 1.6 integration fix-up (gwz-transport `055d091..dd93098`, gwz-core `082722d5..844f9511`): code-axis review

**Verdict: NO-GO.** P0 0, P1 0, P2 2 (one of them architectural), P3 3.

Both P2s have small, bounded fixes. Once they land with their closure rows, I commit to GO; the P3s can ride along or be recorded.

**Tuple (the same at start and end, members clean):**
- ROOT `8d1c8b2f`
- gwz-transport `dd93098400`, gwz-core `844f951160`
- gwz-cli `7edd9f0547`, gwz-py `ebad37b6ab`
- gwz-core-evidence `455f990739`, whose one untracked entry is the same unrelated directory as before

**What I ran and read:**
- `cargo test -p gwz-connection` at `dd93098`: 77 passed, including both N-P3-1 rows. The scratch target directory is deleted.
- In scope and read: the crate diff, `ssh_adapter/worker/reuse.rs`, the SSH adapter and worker diffs, `https_adapter.rs`, the `SchemeRouter` forwarding in `connection_factory.rs`, `held.rs` and `runner.rs`, and `git diff 9734f3f8 082722d5 -- ssh_local.rs`.
- Remote matrix `root-p1-20261026-a` reports OVERALL PASS, and its candidate-test log contains the new rows.

## Findings

### P2-1 (ARCHITECTURAL): a cross-binding proof is not bounded by the opening's own deadline
- **Where:**
  - gwz-core `ssh_adapter/worker/reuse.rs`, in `authorize`: the proof deadline is `now + settings.connect_timeout_ms`.
  - gwz-transport `crates/gwz-connection/src/ports.rs`: `ReuseCommand` carries no deadline.
  - `instance.rs`: nothing times out an `authorizing` entry. `Pending.deadline` is applied only through `request_until`, and the pool request has already been taken by then.
- **Invariant broken:**
  - Contract §7.1: the job follows the latest-waiting-request budget, and "a request's own deadline is never extended".
  - Reuse design §4: the proof runs "under the request's stall clock and absolute deadline".
  - The old worker does this: `start_proof` uses `latest(waiters)` and `settle_proofs` fails expired waiters alone.
- **How it happens:** an opening is submitted with an allocation deadline D, then leases another binding's idle connection with little time left. The agent stalls or shows a confirmation prompt. The opening waits until `now + connect_timeout` instead of D. If `connect_timeout_ms` is 0, the wait is bounded only by the stall clock.
- **Impact:** on the connection-host path, an open can overrun its allocation deadline by up to the connect timeout. This diverges from the old worker's semantics on the path that is now live.
- **Fix:**
  - The host times out `authorizing` entries at `Pending.deadline`: it calls `cancel_reuse`, `release_unused`, fails the opening with the allocation timeout, and includes those deadlines in `next_deadline`.
  - Or `ReuseCommand` carries the deadline and the adapter caps its job at the earlier of the two. The host-side check is needed in either case.
- **Closure test:** a crate row where a held proof meets the opening's deadline: the opening fails with the allocation timeout, `cancel_reuse` is issued and the lease goes back to idle. Plus an adapter row where the job's aggregate deadline is at most the opening's deadline.

### P2-2: concurrent leases by one binding each start their own proof job
- **Where:** `reuse.rs`, in `authorize`. Each `ReuseCommand` becomes its own `Running` job. `Proofs::has_key` and `has_host` only help once a proof has passed, so concurrent leases never benefit.
- **Invariant broken:**
  - Contract §7.1: "Concurrent waiters in a binding share the proof job."
  - Reuse design §4's cost bound: at most one agent round trip, and one confirmation, per operation and key.
  - The old worker shares jobs by (binding, key, host) in `screen`. The implementer lists this as not done.
- **How it happens:** binding A leaves N idle connections behind. Binding B fans out N concurrent opens. The worker starts N jobs: N agent signatures, and with an `ssh-add -c` key, N confirmation prompts. The jobs also draw N slots from the shared job budget. When the budget runs out the answer is `Unprovable`, which forces new setups that need the same budget.
- **Impact:** the user sees many prompts and reuse is defeated on fan-out. This happens in any command that runs two operations against the same host.
- **Fix:** have `Running` hold a list of commands. `authorize` joins an existing job with the same (binding, key, host). Settle every command in the list; `cancel_reuse` removes one command, and the job keeps running while any remain.
- **Closure test:** an adapter row with an agent: K concurrent leases by binding B produce exactly one sign request and K `Authorized` completions.

### P3-1: the trust re-check uses only the lowercased pool-key host
- **Where:** `reuse.rs`, in `authorize`: the name list is `vec![name.to_ascii_lowercase()]`.
- **Invariant broken:** the setup and the old worker's `trust_names` also check the host as the URL wrote it (TR2.18).
- **How it happens:** known_hosts lists the host only in the URL's mixed case. Setup trusts it. A cross-binding lease then gets `untrusted`, is `Revoked`, and every idle connection with that host and host key is retired. The fresh setup succeeds again.
- **Impact:** the connection is reported as revoked when nothing changed, idle connections are torn down, and every cross-binding lease pays a full handshake. It fails closed. The implementer lists this as not done.
- **Fix:** record the host as written on the worker `Entry` at connect time (from the URL in the connect command's opening) and pass both names.
- **Closure test:** a mixed-case known_hosts row is `Authorized`.

### P3-2: retirement and verdicts can hit connections the host has already leased, and that open then fails with an I/O error
- **Where:** `reuse.rs`, in `retire_matching` (it filters on the worker's `Phase::Idle`) and in `settle_proofs` (it reports `Authorized` without re-checking the entry). `worker.rs:217-218`: `start` on a non-idle entry fails the exchange.
- **Invariant broken:** with the old worker, a lease that wins the race with retirement sees `lost()`, becomes `Dead`, and is retried fresh (`ssh_worker.rs:292`). The adapter's `Idle` phase cannot tell which connections the host has leased.
- **How it happens:** binding B's proof of a key is refused and siblings are retired. Before the `IdleClosed` reaches the host, either:
  - a same-binding open leases a sibling (no proof needed), or
  - another binding's proof of that sibling reports `Authorized`.

  `start` then fails, and the open gets an I/O error instead of asking again.
- **Impact:** an operation fails in a narrow but deliberately created window.
- **Fix:** `settle_proofs` reports `Revoked` or `Unprovable` when its connection is no longer `Idle`. When `start` finds the entry retired, it reports in a way the host treats as a lost lease and asks again, matching the crate's no-setup-record path.
- **Closure test:** an adapter row where a sibling is leased during retirement: the open is served on a new connection.

### P3-3: no connection-host row covers the agent possession leg, the secret-bearing one
- **Where:** gwz-core `transport_host/connection_host/tests.rs`. The new rows use `Authenticator::KeyFile` with `AgentSource::None` and exercise trust only. The `p1_6_` rows in `reuse_auth.rs` exercise only the old worker.
- **How it happens:** in `authorize`, force `possession` to `None`. All of the connection-host rows still pass, so on the live path the red row "key removed from the agent leads to a fresh connection" is not shown.
- **Fix and closure test:** a connection-host row with the private `ssh-agent` (Unix) or pipe agent (Windows):
  - binding B's proof signs once;
  - after the key is withdrawn, the verdict is `Revoked`, the siblings are `IdleClosed`, and B gets a new connection.

## Checked and holding

- **Late completions and cancellation:** completions are matched by proof id and connection. `cancel_reuse` sets `waited = false`; a cancelled proof that passes is still recorded for the binding, and one that fails retires nothing. Contract §7.1 is met here.
- **Clocks:** the adapter's `now` and `ReuseAuthority`'s clock share one origin (`ssh_adapter.rs:234-235`, `native_connector(origin)`).
- **Evidence on connections:** `ReservedResource` forwards the reuse evidence. Password and helper connections keep none, so their reuse is `Unprovable` and they serve only the binding that used them.
- **Explicit keys:** a selected-key connection needs no possession proof, relying on the pool's identity equality. This is equivalent to the old worker, except that the old worker's explicit equality assertion is dropped.
- **HTTPS:** `authorize_reuse` is `Authorized` only for connections whose scope record says nothing authenticated them. A missing record, or a helper-token or logon-session scope, gives `false` (`Unprovable`), which fails closed (contract §7.2).
- **Router:** `SchemeRouter` sends `authorize_reuse` to the half that holds the connection and `cancel_reuse` to both halves. Proof ids are unique per instance, so this is harmless.
- **Crate fix-ups:**
  - A lease with no setup record is discarded and counted against the misfit bound, so it is never brought back to life.
  - `Revoked` now asks fresh with no exclusion, matching the reuse design §4 table. This closes my round-2 note about the Revoked row.
- **`ssh_local.rs` merge resolution (`9734f3f8..082722d5`):** a faithful move into `native_connector`. The `ReuseAuthority` is built once per connector, and the evidence is attached on the selected-key and agent paths, none on the password and helper paths. No behaviour changed.
- **Code rules:** no new globals, `cfg_if` only, control-flow bodies braced. The new rows live in `cfg(test)` modules under `src/`.

## Risks and observations (not findings)

- **Push order:** gwz-core's pin and allowlist name `dd93098`, which is local and not pushed. Under the bump rule gwz-transport must be pushed first. ROOT's local `origin/main` reference for gwz-transport reads `22b53dc`, which looks stale.
- **Matrix identity:** the matrix summary records the base HEADs (`082722d5` and `055d091`), not the object commits. That the content is equal rests on the coordinator's statement and on the new rows appearing in `candidate-test.log`; there is no content digest.
- **Already recorded as not done:**
  - The HTTPS adapter still uses the instance's helper and ignores the operation's `AuthContext` (1.3c/C2(b)).
  - There is no HTTPS retention row.
  - The `Denied` coverage is a helper unit row, not end to end.
- **For contract §15:** the SSH adapter now holds the 1.6 proof (shared with the old worker through `ReuseAuthority` and `Proofs`). The two items above (P2-1 deadline, P2-2 job sharing) should be recorded until they are fixed.
