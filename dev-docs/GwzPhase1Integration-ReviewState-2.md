# Phase 1 integration fix-up — STATE-AXIS RE-VERDICT (round 2)

**Review object:** gwz-transport `dd93098..3980469` and gwz-core `844f9511..22aebcee`, both committed locally and not pushed.
**Root:** `/Volumes/projects/limbo/gwz-dev`. The tuple was the same at start and end (gwz-core `22aebceeb4`, gwz-transport `3980469918`), and both worktrees are clean.
**Date:** 2026-10-27
**Verdict:** **GO** on the State axis. P0 0, P1 0, P2 0, P3 0 new. This depends on the pending matrix: no summary for `root-p1-20261027-b` has been filed in `gwz-dev-logs` yet, and this verdict assumes it passes.

**Tests run:**
- `cargo test -p gwz-connection`: 84 passed.
- gwz-core candidate shape (`connection_host`, `ssh_adapter`, `reuse_agent`, `p1_6`): 50 passed, 1 ignored (the open HTTPS route defect, owned by 1.3c/C2(b)).

## Closure table

| My finding | Counterexample | Closure on this tree | Status |
|---|---|---|---|
| **P2-1** (architectural): a proof in progress ignored the binding's cancellation and the opening's deadline | B's operation is cancelled while its proof is held. Previously A's connection was closed, the opening stayed waiting, and a late `Revoked` or `Unprovable` opened a new connection for the cancelled binding. The deadline also ran past its own end. | **Cancellation:** `cancel_binding` now ends B's waiting openings before the pool cancels B's owner. Each proof is cancelled with `cancel_reuse`, the lease goes back to idle with `release_unused` (A's connection is kept), and the opening ends `Cancelled`. `ask_again` and `ask_fresh` now refuse when the binding's shared cancelled flag is set. A rejected frame in `host.rs` `route` now goes through `cancel_binding`. Host close: flags plus `fail_in_flight`, which cancels and discards. **Deadline:** an opening still waiting for its proof past its submit deadline now ends `AllocationTimeout`, and `next_deadline` includes those deadlines so the driver wakes. The adapter runs the proof under the earlier of `remaining_ms` and its own connect bound. **Rows:** cancel, deadline, `next_deadline`, `remaining_ms`, retry refused when cancelled, rejected frame. | **Closed** |
| **P3-1**: one proof job per lease | K concurrent leases by one binding produced K sign requests | `Running.commands`: `authorize` joins a running job with the same binding, key, host and spellings. `cancel` removes only that one waiter, and a pass is still recorded. The K-lease, one-sign row is green. | **Closed** |
| **P3-2**: a proof could pass, or a sibling be retired, against a connection the host had leased | A sibling was retired while leased, and `Authorized` was reported on a connection that was disposing | `retire_matching` skips connections some running proof waits on (`busy`). `Claims` has the lease and the retirement take one lock, so exactly one wins. A retired connection makes `start_exchange` return `NotConnected`; the crate treats that as a lost lease and asks again, bounded by `per_user_host + 1`. `Authorized` is downgraded to `Unprovable` when the connection is no longer idle. The `Claims` unit rows and the fake-adapter crate row are green. | **Closed** |
| **P3-3**: trust re-checked under the lowercased host only | A hashed known_hosts entry for a mixed-case host spelling failed the re-check, giving `Revoked` and a retirement storm | `Evidence.written_host` is recorded at setup in `native_connector`. `trust_names` checks both spellings, and the spellings are part of the key for joining a job. | **Closed** |
| **Note:** proof records keyed by `Debug` output | — | `binding_name` now uses the identifier's fields (`{host:032x}.{serial}`). | **Closed** |

## Implementer's departures

- **`remaining_ms` (a span) instead of an absolute deadline: accepted.**
  - The host's clock and the adapter's clock have different origins, so a span is the honest form.
  - Queue delay can push the adapter's deadline slightly late. The host's own expiry still governs: it ends the opening at its deadline and cancels the proof.
- **`Claims` instead of a deterministic race row: accepted.**
  - Lease and retirement are serialized under one lock, and `begun` and `forget` clear the record at the right points.
  - The unit rows cover every ordering of lease and retirement; the crate row covers the host's response to `NotConnected`.
  - Residual: if `start_exchange` claims a connection but the worker dies before taking it up, that claim stays and blocks only that connection's retirement. Harmless.
- **Rejected frame routed through `cancel_binding`: correct.**
  - It closes a path my P2-1 did not name, where a late `Authorized` could still serve a binding the host had cancelled. A red-first row covers it.

## New findings

None.

**Attacks that failed on this tree:**
- **Joined waiters past the job's deadline.** A waiter that joins a job with a later deadline than the job's gets `Unprovable` early, which fails closed (no authority is extended).
- **Abandoned jobs.** A job left with no waiters runs on only until its bounded deadline, retires nothing on refusal, and still records a pass.
- **Revoked on a shared job.** All its waiters are discarded and each asks fresh; the siblings are retired first.
- **The Authorized-to-serve window.** If a retirement lands between the `Authorized` report and `serve`, `Claims` refuses the start and the opening asks again.
- **Deadline already passed.** `remaining_ms` 0 times the job out immediately, and the host expires the opening on its own.

**Carried, not this patch:**
- HTTPS adapter: it ignores the operation's `AuthContext` (1.3c/C2(b)), and there is no HTTPS retention row.
- `Denied` has no end-to-end row in gwz-core.
- `ExchangeSpec::git` still has no production caller (adoption 2.5).
