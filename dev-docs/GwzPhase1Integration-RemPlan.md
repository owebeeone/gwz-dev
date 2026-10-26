# Phase 1 root integration fix-up: remediation plan (round 1)

Date: 2026-10-27. Lane owner's plan after [Code](GwzPhase1Integration-ReviewCode.md) (NO-GO: 2 P2, one architectural; 3 P3) and [State](GwzPhase1Integration-ReviewState.md) (NO-GO: 1 P2 architectural; 3 P3) on gwz-transport `055d091..dd93098` and gwz-core `082722d5..844f9511` (root; committed locally, not pushed). Both pre-committed to GO on bounded remedies. One patch; the same reviewers re-verdict.

**Architectural root-cause count on this object: 1.** Code P2-1 ⊂ State P2-1, found blind by both axes: an opening waiting in `authorizing` is governed by neither its binding's cancellation nor its own deadline; the proof runs under the adapter's connect timeout.

Convergences: Code P2-1 ⊂ State P2-1; Code P2-2 = State P3-1 (proof job not shared within a binding; Code rates it P2 → blocking); Code P3-1 = State P3-3 (trust re-check on the lowercased host only); Code P3-2 = State P3-2 (retirement and verdicts against connections the host has leased).

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| Code P2-1 ⊂ State P2-1 (architectural) | In `cancel_binding`, BEFORE `pool.cancel_operation`, sweep that binding's `authorizing` entries: `cancel_reuse(proof)`, `release_unused(lease)`, fill the slot `Cancelled`. Sweep an `authorizing` entry whose `Pending.deadline` passes the same way with `AllocationTimeout`, and include those deadlines in `next_deadline`. `ReuseCommand` carries the opening's deadline; the SSH adapter runs the proof under the earlier of it and its own bound. `ask_again`/`ask_fresh` refuse for a cancelled binding. | crate `exchange.rs` `cancel_binding`, `instance.rs`, `ports.rs`; gwz-core `ssh_adapter/worker/reuse.rs` | Crate rows (fake): cancel during a held proof → opening Cancelled at once, `cancelled_reuses == [proof]`, `c` idle and not closed, `connects()` unchanged after a late `Revoked`; deadline during a held proof → `AllocationTimeout` at the submit deadline, `c` idle, `cancel_reuse` issued. Adapter row: the job's deadline ≤ the opening's. Mutation: remove the sweep from `cancel_binding` → row fails. |
| Code P2-2 = State P3-1 | `Running` holds a list of commands; `authorize` joins a running job with the same (binding, key, host); every waiting `ProofId` is settled; `cancel_reuse` removes one command and the job runs on while any remain (a pass is still recorded). | `ssh_adapter/worker/reuse.rs` | Adapter row with the private agent (Unix `ssh-agent` on a private socket, Windows pipe agent): K concurrent leases by binding B → exactly one sign request and K `Authorized`. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| Code P3-1 = State P3-3 | Record the host as the URL wrote it on the worker entry / evidence at setup; re-check trust under both names, as the old `trust_names` did. Row: mixed-case known_hosts → `Authorized`. | `reuse.rs`, the SSH adapter's connect path |
| Code P3-2 = State P3-2 | `settle_proofs` reports `Unprovable` when the entry is no longer `Idle`; `retire_matching` skips connections with a running proof (or ends their proofs `Revoked`); when `start` finds the entry retired the host sees a lost lease and asks again (as the crate's no-setup-record path). Row: a sibling leased during retirement → the open is served on a new connection. | `reuse.rs`, `ssh_adapter/worker.rs:217-218` |
| Code P3-3 | Connection-host row with the private agent: B's proof signs once; after the key is withdrawn the verdict is `Revoked`, siblings `IdleClosed`, B gets a new connection. Row must fail if `possession` is forced to `None`. | `connection_host/tests.rs` |
| State note | Key proof records by binding fields, not by derived `Debug`. | `reuse.rs` |

## Recorded, not this patch

HTTPS adapter ignores the operation's `AuthContext` (1.3c/C2(b)); no HTTPS retention row; `Denied` end-to-end row in gwz-core; the adapter path has no production caller of `ExchangeSpec::git` yet (adoption 2.5). Contract §15 1.6 row records that the SSH adapter now holds the proof.

## Process

One patch on top of root (new commits on gwz-transport `dd93098` and gwz-core `844f9511`; no rewrite); focused rows + crate clippy; remote matrix `root-p1-20261027-a` detached with a content digest; the lane owner commits, the reviewers re-verdict their own findings; then push gwz-transport, Windows receipt on the final gwz-core, push the rest. Round count after this patch: 1 of 2.
