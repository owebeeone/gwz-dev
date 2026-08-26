# R2-E E0.1 — the reach traces (dated note, 2026-08-26)

Read-only, per `GwzM5-8R2E-Plan.md` §3 E0.1. Both answers below are
inputs the E0.2 semantics amendment cites. Traced at gwz-core main
`94da3e5` (= the v0.11.0 surface plus post-tag workflow/pin commits;
no production source differs from the tag).

## (a) DurableObjectIdentity production reach — NOT REACHED in v0.11.0; the exposure arrives at E4.1

**What production executes today.** Every merge mutation path
(abort/mod.rs:44, continue_op.rs, gc.rs:167, runtime/mutation_guard.rs)
acquires `WorkspaceMutatorLock`, which since its landing wraps
`checked_artifact::try_acquire_workspace_runtime`
(operation/workspace_mutator_lock.rs:36 — production, above the
`cfg(test)` module at :61). That acquisition
(bootstrap/runtime/mod.rs:84-140) performs: workspace path
resolution, ambient directory retention, repository revalidation,
the bootstrap guard file + advisory lock, and runtime/locks
directory + lease-file creation. **It constructs no catalog-lease
witness and executes no identity probe** — neither
`name_to_handle_at` (`identity.rs:267`, the `DurableObjectIdentity`
probe that failed on the ubuntu-22.04 image) nor the statx-`MNT_ID`
`rename_domain` (`identity.rs:304`).

**Where the probes live.** All durable-identity and rename-domain
construction sits in the catalog lease and below:
`bootstrap/runtime/catalog_lease/{witness,association,target}.rs`
(witness fields `durable_identity`/`related_git_durable_identity`,
`platform.rename_domain(...)` at witness.rs:60-67,
association.rs:150/:184, target.rs:154-156/:222) and the capability
providers under `capability/`. Entry is
`WorkspaceRuntimeLease::catalog_mutation_lease()`
(bootstrap/runtime/mod.rs:52), whose only consumer chain is
`recover_or_create` — which has **zero production callers** (the A1
must-not-flip item 1, verified at activation and unchanged since;
`workspace_mutator_lock.rs:50` exposes the lease to the `cfg(test)`
module only in practice). The v1 lifecycle never crosses in at all:
`v1_lifecycle/` contains zero non-test `checked_artifact`
references; its "checked store" is its own module persisting through
`durable_fs` (store/rewrite.rs:6-10).

**Answer to O11:** no v0.11.0 user — `--no-ff` included — can meet
the `DurableObjectIdentity` refusal. The ubuntu-22.04 verify failure
was a test-fixture-only surface, exactly as the release record
hoped. **The exposure arrives with E4.1** (the first production
catalog activation): from that landing, any Linux filesystem that
refuses persistent file handles (the 22.04-image class: certain
tmpfs/overlay/NFS/older filesystems) meets the fail-closed
capability refusal wherever the catalog lease is taken.

**Two consequences, binding on the plan:**

1. **The capability-refusal UX is an E4.1 hard precondition** (plan
   O11/E6.3 folds into E4.1's gate): a typed, actionable message
   naming the filesystem capability, a runbook class for it, and a
   release-notes line — before, not after, the first production
   caller.
2. **The blast radius is an explicit E0.2 amendment decision**: does
   E4.1 probe at every mutator-lock acquisition (every mutation
   refuses on such filesystems) or only on the operations that
   consume the catalog (checked-artifact actions refuse; ordinary
   merges proceed)? The conservative reading of the §10 table's
   "runtime bootstrap only" row is the second; the amendment must
   say so in frozen text either way.

## (b) The §11.3 gate on first production catalog activation — restated

`GwzM5-8R2DSettledTuple.md` §11.3 ("Settle caveats the 5.2 dual
should weigh", :798+) binds the activation as follows; E0.2 consumes
each row:

1. **`BarrierIntentV1::issue` (BINDING, ordering constraint):** the
   observe-or-refuse pattern must land **before the roaming anchor
   gains its first admitted-action caller**. Plan consequence, now
   explicit: **E2.2 strictly precedes any E4 row that admits
   actions touching the roaming anchor** — the plan's nominal
   E1-E3 → E4 order is load-bearing, not stylistic.
2. **`authority_name` self-checking weigh:** R2-E must weigh a
   self-checking rename or a compensating observation **when the
   family gains its next consumer** — an E4-adjacent design
   decision, to be taken in E0.2 or explicitly deferred to the
   consuming E4 step with the weigh recorded.
3. **Anchor nits** (accepted same-user boundary): `verify`'s
   unbounded `read_to_end`; the non-canonical retired-ordinal
   parse; `platform/unsupported.rs` claiming the Linux support
   profile (fail-closed shields it — and note the adjacency to (a):
   the capability-refusal UX work should sweep this claim). O12 /
   E6.2.
4. **The ten multi-component writer rows'** native Windows/ARM
   execution ledger debt (expected-green, entry owed at the next
   matrix dispatch) — discharged at the next E-landing's matrix,
   recorded in the classification ledger.
5. **The 481-item frozen surface behind the seven blankets** —
   reconciled at E7 per the plan.

## Disposition

E0.1 is complete; no tree was mutated. E0.2 drafts the §3.5/§4.3
semantics amendment against this note (the 38 keys' injection
semantics; the O6 observe-or-refuse resolution; the O8
archive-equivalence decision; the (a)-consequence blast-radius
decision; the (b) row consumptions), then E0.3's dual reviews it.
