# Connection-host reuse integration fix-up (gwz-transport dd93098, gwz-core 844f9511) — STATE-AXIS REVIEW

**Review object:**
- gwz-transport `055d091..dd93098`
- gwz-core `082722d5..844f9511`
- the hand-resolved `ssh_local.rs` (`9734f3f8..082722d5`)

**Root:** `/Volumes/projects/limbo/gwz-dev`. The tuple was the same at start and end: gwz-core `844f951160`, gwz-transport `dd93098400`. Both worktrees are clean.

**Date:** 2026-10-27

**Verdict:** **NO-GO.** P0 0, P1 0, P2 1 (architectural), P3 3. I commit in advance to GO once P2-1 is closed by the bounded remedy below, with its rows green and no new P0–P2.

**Evidence:**
- `cargo test -p gwz-connection`: 77 passed.
- gwz-core candidate shape, `cargo test --lib connection_host`: 21 passed.
- The root matrix `root-p1-20261026-a` says OVERALL PASS. Its summary records the HEADs as `082722d5` and `055d091` (the bases); the coordinator states its worktree content equals this object.

## Findings

### P2-1 (architectural) — A proof in progress escapes the opening's cancellation and deadline

**Root cause:** once an opening holds a lease and waits for its reuse proof, only two things act on it: dropping the opening (`abandon`) and sealing the host (`fail_in_flight`). Cancelling the binding does not reach it, and neither does the opening's own deadline.
- Before 1.6's crate code had a real adapter this was latent. I missed it in round 2. The SSH adapter added here makes it live.

**Location:**
- `crates/gwz-connection/src/exchange.rs:201` (`cancel_binding` calls only `pool.cancel_operation`)
- `instance.rs`: `authorizing` is never swept; `Pending.deadline` is used only by `request_until`; `next_deadline` covers only the pool's deadlines
- `ReuseCommand` carries no deadline, so the SSH adapter runs the proof under its own `connect_timeout_ms` from the moment it starts (`ssh_adapter/worker/reuse.rs::authorize`)

**Invariants violated:**
- Contract §7.1: a cancelled or timed-out proof returns a still-clean leased connection to idle *before* the binding is cancelled, and a request's deadline is never extended.
- Contract §8: an operation's cancellation affects that binding only, not another binding's idle connections.
- Design §4 table: the request is cancelled or its deadline passes → connection back to idle, request fails Cancelled or Timeout.

**Interleavings:**
- **Cancellation.** A leaves `c` idle. B leases `c` and its proof is held. B's operation is cancelled.
  1. `pool.cancel_operation` closes `c`, so A's healthy connection is destroyed.
  2. B's opening stays in `authorizing` and does not fail Cancelled.
  3. When the proof's verdict arrives:
     - If it is `Authorized`, B is served an exchange on a closed lease.
     - If it is `Revoked` or `Unprovable`, `ask_fresh`/`ask_again` submit a new pool request under B's owner. The pool keeps no record that the owner was cancelled, so it opens a new connection: network setup, and an agent sign or helper, for a cancelled operation.
- **Deadline.** The proof deadline is the adapter's connect timeout (30 s by default) counted from when the proof starts. An opening waiting in `authorizing` outlives its 30 s allocation deadline by up to another 30 s.
  - With `connect_timeout_ms` = 0 and `io_timeout_ms` = 0, a hung agent or unanswered confirm prompt holds the lease, the opening and the binding's `Live` indefinitely.

**Impact:**
- Credential use and network effects after the user cancelled.
- Another binding's idle connection is destroyed.
- The deadline is extended, and with timeouts disabled the opening can wait forever.

**Correction (bounded):**
- In `cancel_binding`, before `pool.cancel_operation`, sweep that binding's `authorizing` entries in this order: `cancel_reuse(proof)`, `release_unused(lease)`, fill the slot with `Cancelled`.
- Do the same sweep with `AllocationTimeout` when an `authorizing` entry's `deadline` passes, and include that deadline in `next_deadline`.
- Pass the opening's deadline in `ReuseCommand` and run the proof under the earlier of it and the adapter's bound.
- `ask_again`/`ask_fresh` refuse when the binding is cancelled.

**Closure rows (crate, against the fake):**
- Cancel during a held proof:
  - the opening ends Cancelled at once;
  - `cancelled_reuses == [proof]`;
  - `c` is idle and not closed;
  - `connects()` is unchanged after a late `Revoked`.
- Deadline during a held proof: `AllocationTimeout` at the submit deadline, with `c` idle.
- Mutation: remove the sweep from `cancel_binding`; the row must kill it.

### P3-1 — The adapter starts one proof job per lease, not one shared job per binding (implementer-listed)

**Location:** `ssh_adapter/worker/reuse.rs::authorize`. It pushes one `Running` per `ReuseCommand` and never joins an existing one.

**Invariant violated:** contract §7.1, "Concurrent waiters in a binding share the proof job". The old worker's `SharedProof` did this.

**Impact:** fail-closed, but costly.
- N concurrent leases need N agent signs.
- A confirm key prompts N times at once, for example 32 prompts on a 32-member fan-out.
- N slots are taken from the setup supervisor's budget, which setup jobs share.

**Correction:** join a running job keyed by (binding, key, host), and report each waiting `ProofId`.

**Gate:** must close before the adapter is adopted (2.5).

### P3-2 — A proof that passes on a connection retired or lost meanwhile fails the open instead of retrying

**Location:**
- `retire_matching` retires any adapter entry in phase `Idle`, including a sibling another binding has leased while its proof is running.
- `settle_proofs` still reports `Authorized` for a connection that is now disposing.
- `serve` then calls `start_exchange`, which succeeds asynchronously, and the exchange fails.

**Old path:** `lost()`, then `Dead`, then a fresh retry.

**Impact:** the open fails spuriously; no authority is gained.

**Correction:** report `Unprovable` for a proof whose entry is no longer `Idle`. Either skip connections with a running proof in `retire_matching`, or end their proofs `Revoked`.

### P3-3 — Trust is re-checked under the lowercased key host only (implementer-listed)

**Location:** `reuse.rs::authorize`, `Check.trust` names `[key.host lowercased]`. The setup checked the host as the URL wrote it (TR2.18).

**Impact:** a hashed known_hosts entry for a mixed-case written host fails the re-check. The result is `Revoked`, and every idle connection to that host with the same host key is retired on each cross-binding lease. Fail-closed, but it causes churn.

**Correction:** record the written name in the evidence at setup and check both names, as the old `trust_names` did.

## Attacks that failed

- **Late verdicts.**
  - The host matches `(connection, ProofId)`.
  - The adapter's `cancel` sets `waited = false`, so a cancelled proof reports nothing.
  - A pass is still recorded for the binding (contract §7.1).
  - `ProofId` cannot be constructed outside the crate.
  - The router sends `cancel_reuse` to both halves; ids are unique per instance, so this is harmless.
- **Proof records.** They are keyed by the derived `Debug` of `BindingId { host: HostNamespace(u128), serial }`, which is unique per binding.
  - Robustness note: a future redacting `Debug` would merge every binding's proofs. Prefer a field-based key.
- **Fail-closed direction.**
  - A missing entry, missing evidence, a job that cannot start, or an error or timeout from the job all give `Unprovable`.
  - A refused key or untrusted host key gives `Revoked`, with siblings retired first, before the `Revoked` completion. A proof the host cancelled retires nothing.
  - The crate's `Revoked` arm now asks fresh with no exclusion, which closes the earlier 1.6 N-P3-1.
  - A lease with no setup record is discarded and the opening asks again, which closes 1.5's N-P3-1.
  - HTTPS: an anonymous scope is `Authorized`; a per-exchange scope or a missing record gives `false` (`Unprovable`).
- **Key-file connections.**
  - Their possession check is skipped. This is sound because pool identity is `Explicit(token)`, and isolated identities embed the token (`url-password-{n}`, `helpers-disabled:explicit:{token}`).
  - The `ssh_local.rs` merge keeps 1.6's evidence capture as it was, now inside `native_connector`.
- **Disposal.**
  - `dispose_all` clears the running jobs; dropping a job cancels it.
  - The host has already sealed, and `fail_in_flight` cancels and discards its `authorizing` entries.
- **Retained secrets.**
  - Proof records, `Running` and the evidence hold public key blobs and host keys only.
  - The nonce stays on the job's stack.
- **Bounds.**
  - Proof records: 256 bindings × 64.
  - Proof jobs come from the supervisor's budget; a full budget gives `Unprovable`.
  - Exclusion lists are capped at `MAX_EXCLUDED`.
- **Old worker.** `Screened::Denied` gives a fresh retry with no exclusion, and the siblings are retired before it.
- **Hygiene.** `cfg_if!` only, no globals, and the code is candidate-build only. There is still no production caller of `ExchangeSpec::git`, so the adapter path is not wired.

## Next action

1. Close P2-1 in the crate: the cancel and deadline sweep, the deadline in `ReuseCommand`, and the two rows plus the mutation.
2. Optionally fold in P3-2 and P3-3 now.
3. Record P3-1 as a gate on adopting the adapter (2.5).
4. Re-review on this axis limited to that diff. GO is pre-committed if it stays as bounded as described.
