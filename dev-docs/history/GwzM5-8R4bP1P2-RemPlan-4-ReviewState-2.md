# R4b-P fourth remediation amended state-machine re-review

Date: 2026-08-12

Reviewed in full:

- `GwzM5-8R4bP1P2-RemPlan-4.md` (amended checkpoint);
- `GwzM5-8R4bLinuxCapabilitySpike.md`;
- `GwzM5-8R4bP1P2-RemPlan-4-ReviewState.md`; and
- the frozen R4b reverse-lifecycle, action-journal, aggregate-authority, and C7
  contracts.

I did not read or coordinate with the filesystem reviewer. This is a design
review; no production or test source was changed.

## Verdict

**NO-GO for R1.**

The amendment materially improves the architecture. The selected-root owner
table now names the physical-`After` handoff, terminal order has a normative
example, managed-parent creation has a real catalog-backed transition, C7 is
stage-typed, and prefix authority is request/owner/action/phase-bound. The
earlier diagnostic-precedence finding is closed in design.

Five P2 design defects remain. Two are narrow but foundational: the frozen
Linux tuple asks `name_to_handle_at(flags = 0)` to identify a retained handle,
which that API cannot do, and checkout status is still grouped into a domain
that cannot support a worktree-only overlay. C7's stage types are correct, but
the retained five-by-four executable claim still includes fact/attempt crosses
that cannot exist at the invocation guard. The pre-catalog workspace-lock
bootstrap is named but not specified and is circular if its “barrier” means
the catalog-backed Windows barrier. Finally, the implementation order creates
the private catalog before the tracked/index collision guard that is supposed
to precede any private-tree mutation.

## Findings

### [P2-1] The Linux tuple's `flags = 0` handle query cannot bind a retained object handle

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:158-178`;
- `GwzM5-8R4bLinuxCapabilitySpike.md:14-31`; and
- `GwzM5-8R4bLinuxCapabilitySpike.md:73-100`.

**Violated contract.** Checked identity must be derived from the already
retained no-follow object handle. A path lookup is not authority for that
object. The remount probe must execute the exact runtime provider tuple.

**Evidence.** Both documents freeze `name_to_handle_at(flags = 0)` while
describing the result as the handle for the retained object. Linux supports a
retained-descriptor query only when `path` is empty and `AT_EMPTY_PATH` is set.
With an empty path and flags zero the call fails `ENOENT`; with flags zero and
a nonempty path it performs a new pathname lookup relative to `dirfd`. It no
longer identifies the already opened object and can race a substitution between
the no-follow open and handle acquisition. The Linux man-pages contract states
the four path/dirfd forms and explicitly requires `AT_EMPTY_PATH` for the empty
path/retained-fd form: [name_to_handle_at(2)](https://man7.org/linux/man-pages/man2/open_by_handle_at.2.html).

The UUID half is otherwise sound: current Linux UAPI defines
`FS_IOC_GETFSUUID` as the external UUID and returns `struct fsuuid2 { len,
uuid[16] }`: [Linux `fs.h`](https://github.com/torvalds/linux/blob/master/include/uapi/linux/fs.h).

**Failure scenario.** Open parent or leaf `A` no-follow, replace the pathname
with object `B`, then execute the frozen flags-zero pathname query. The provider
can combine `B`'s file handle with the retained invocation facts for `A`, or
must add a path revalidation loop that the supposedly handle-owned identity
interface does not specify.

**Required correction.** Freeze the provider call as
`name_to_handle_at(retained_fd, "", ..., AT_EMPTY_PATH)` and exclude
`AT_SYMLINK_FOLLOW`, `AT_HANDLE_FID`, and path relookup fallbacks. Require
`fsuuid2.len == 16`, a nonzero UUID, positive supported handle type, and
1..=128 bytes. The runtime and pre-R1 remount probe must use this exact call.
Add a same-invocation substitution test proving that renaming/replacing the
original pathname after the retained open does not change the returned object
identity, plus negative `AT_EMPTY_PATH`/permission/unsupported rows.

### [P2-2] `IndexAndStatus` still makes a worktree-only overlay impossible

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:427-474`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:488-517`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:695-707`.

**Violated contract.** The selected-root pending-`After` window assigns
lock/marker worktree facts to publication evidence while every index fact is
owned by the terminal participant. An overlay must suppress exactly the
alternate owner's domain and nothing else.

**Evidence.** The new observation is path-keyed, but `CheckoutPathFact.status`
combines staged, unstaged, untracked, conflict, and rename facts. The only
domains are `Worktree(path)` and `IndexAndStatus(path)`, and the plan explicitly
says a worktree overlay does not erase status. An evidence-owned lock worktree
that differs from the terminal commit normally produces an unstaged/deleted/
untracked worktree status. The worktree overlay replaces the bytes but leaves
that status, so the positive handoff still rejects. Using `IndexAndStatus`
would hide terminal-participant index drift that evidence no longer owns.

The selected-root ownership table itself is now coherent; the status domain
cannot implement it.

**Required correction.** Split the authoritative fact by mutation side:

```text
CheckoutPathFact {
    index: entries + semantic flags + staged/conflict/index-side facts,
    worktree: kind/bytes + unstaged/untracked/unreadable/worktree-side facts,
    nested_gitlink: live HEAD and nested status in the worktree domain,
}

CheckoutOverlayDomain = Index(path) | Worktree(path)
```

Represent a rename with its lossless source/destination relationship inside
the corresponding index or worktree side; do not put it in a shared status
bag. A worktree overlay must replace the exact worktree value and its
worktree-side status only. It must retain all index stages, semantic flags,
conflicts, and index-side rename/type facts. Update the positive lock/marker
handoff test to require non-clean worktree status on the overlaid paths, then
cross it with independent index drift and unrelated worktree/gitlink drift.

### [P2-3] C7's stage types are feasible, but its five-by-four executable audit is still not

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:534-609`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:709-719`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:825-828`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs:73-117`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:51-82`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:261-267`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/dispatcher/invocation.rs:108-135`.

**Violated design goal.** Every expected C7 key must correspond to a real
production stage and be executable without forged authority or a tuple-only
count.

**Evidence.** The amendment correctly gives `InvocationRejectionCase` no
`fresh_form`, because the second-execution guard accepts only an action and
prior invocation history. It then still requires a conceptual five-attempt by
four-fresh-fact audit “partitioned” across that key type. There is no consumed-
second × `After`, `Ambiguous`, or operational-error invocation case: those
fresh observations do not issue a second action and therefore never reach
`before_execute`. A consumed-second × `Before` row is also not an attempt passed
to reconciliation; it is a separate defense-in-depth guard reached only if an
upstream stage asks to execute the same action again.

Likewise, stale/mismatched values are rejected before `attempt_class`, whose
production values are only none, matching success, and matching failure. The
new `BindingRejectionCase` handles that correctly, but its distinct stale,
mismatched-request, and mismatched-action cases are not one attempt class times
four facts.

**Required correction.** Remove the fixed five-by-four executable cardinality
from sections 3.10 and the inherited C7 contract. Declare independent stage
domains instead:

- service reconciliation: `None | MatchingSuccess | MatchingFailed` crossed
  with each row's representable fresh facts;
- binding gate: stale record, mismatched request, and mismatched action crossed
  only with fresh observations that production can issue for the retained/new
  record;
- invocation gate: repeated execute after first success and first failure,
  independent of a fresh-fact Cartesian product; and
- proof-only, action-free, and platform rows with their own alphabets.

If the pure reconciliation table retains `StaleOrMismatched` or
`ConsumedSecond` as documentation, mark those decisions as pre-relation stage
sentinels and do not count them as service cells. Prefer removing unreachable
variants from `attempt_class` entirely. The expected-key set must assert these
stage-specific cardinalities rather than preserving the historical number 20.

### [P2-4] `WorkspaceRuntimeBootstrap` is still a name, not a non-circular pre-catalog protocol

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:350-372`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:637-689`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:758-768`;
- `gwz-core/src/operation/workspace_mutator_lock.rs:15-55`; and
- `gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs:25-51`.

**Violated contract.** First-use bootstrap must have a complete authority and
durability path that does not depend on the catalog or lease it is establishing.
The ext4-only checked identity capability must affect only checked merge
transitions, not every command using the workspace-wide mutation guard.

**Evidence.** The plan says the mutation guard creates `.gwz/locks` and the lock
file, then, after obtaining the lock, “flushes, barriers, and records exact
runtime-root identities.” It never defines the record/grammar, the identity
kind, or the Windows barrier. The ordinary Windows `barrier(parent)` defined in
the same section requires a durable catalog `BarrierIntentV1` and roaming
anchor, but catalog bootstrap deliberately occurs only after the workspace
lease. Using that barrier is circular; not using it leaves “barriers” as an
unreviewed exceptional path.

This seam is shared by commit, stage, tag, materialize, and other mutators, not
only the disabled v1 merge path. If “exact runtime-root identities” means the
ext4-only `DurableIdentityProvider`, a workspace on XFS/Btrfs/older ext4 would
lose all those existing commands, contradicting the spike's activation claim
that only the affected checked merge transition is unsupported.

The late clarification for `CatalogBootstrapV1` does close the catalog root's
own pre-catalog exception. It does not close this earlier layer: the mutation
guard must durably establish `.gwz/locks` and the lock file before it can hold
the lease under which `CatalogBootstrapV1` runs. The missing contract is for
that runtime/lock bootstrap and its direct Windows durability edge, not for the
now-specified catalog bootstrap.

**Required correction.** Freeze `WorkspaceRuntimeBootstrap` in section 3.6,
not only in the R1 list. It must be capability-neutral and pre-catalog:

1. use a fixed bounded grammar outside the private catalog for the exact
   `.gwz/locks` directory and lock-file staging/final names;
2. use Unix no-replace creation plus parent sync, and a specifically frozen
   Windows direct write-through no-replace publish/reobserve path that does not
   call `BarrierIntentV1` or use the roaming anchor;
3. acquire and retain the OS advisory lock, then close/recover any exact
   pre-lock scratch under that lock;
4. retain invocation-local no-follow identities only; it must not issue
   checked reboot authority or call the ext4 provider;
5. reject symlink/wrong-kind/foreign forms and define every before/after
   create/publish/reopen fault row; and
6. only after the lease exists may a checked merge request run the durable
   identity/equivalence capability check and then bootstrap a catalog.

State explicitly that non-checked commands stop after the ordinary mutation
guard and are not gated by the checked provider. Add the pre-catalog runtime
grammar to C7/platform rows and its exact owner to `bootstrap.rs`.

### [P2-5] Private-catalog bootstrap is ordered before the tracked/index collision guard

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:92-156`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:350-360`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:427-439`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:675-704`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:814-819`.

**Violated contract.** The private-prefix correction requires tracked or
index-visible control-path collisions to reject before GWZ mutates that
worktree. Unsupported/collision preflight must precede private hierarchy
creation.

**Evidence.** `CatalogBootstrapV1` creates and publishes
`.gwz/checked-artifacts` and its infrastructure. The collision guard is
specified later as occurring “before the first checked action,” but the
implementation sequence builds and converts the catalog in R2 and does not add
the collision preflight until R3. Catalog bootstrap is already a workspace
mutation under the tracked private prefix; calling it as acquisition preflight
does not make it non-mutating.

**Failure scenario.** The selected root index tracks
`.gwz/checked-artifacts/protocol`, while the worktree directory is absent (a
tracked deletion). Starting a checked workspace-policy action can create and
populate that directory before the later guard reports the index collision.
The failure path has modified the user's tracked checkout—the exact class the
reserved-prefix redesign is intended to retain and reject.

**Required correction.** Freeze this order for a workspace artifact:

1. acquire the capability-neutral workspace lease;
2. run read-only durable-provider/path checks and a lossless Git index/tracked
   collision check for the entire private control namespace;
3. on any unsupported/collision result, return with no catalog or managed-path
   mutation;
4. bootstrap/recover the private catalog;
5. recover or run a declared managed-parent bootstrap; and
6. enumerate the catalog and issue checked-action authority.

Git-directory policy may use its separately proven non-worktree rule, but must
still run capability checks before catalog creation. Move collision preflight
into the R1/R2 interface checkpoint and conversion, before catalog bootstrap;
R3 should test its interaction with complete checkout, not introduce it for
the first time. Add an absent-worktree/present-index collision regression and
an exit criterion that unsupported or tracked-collision rows leave the private
root absent byte-for-byte.

### [P3-1] The terminal cursor sentence still reverses one of its two orientations

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:476-486`.

The normative example closes the earlier ambiguity. One sentence nevertheless
says the cursor is immediately “before that suffix/prefix.” With stored
`[a,b,c]`, completed `{c}`, and current `b`, the cursor is immediately before
the suffix in forward order but immediately **after** the prefix in reverse
order. Say exactly that. This is P3 wording because the example already makes
the intended validator unambiguous and the live aggregate independently checks
every terminal row.

### [P3-2] The privacy gate should name a prefix-specific issuer, not the generic authority issuer

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:611-635`.

The prefix privacy design is otherwise sound. Clarify that the compile gate
forbids construction/import of a private `RollbackPrefixIssuer` (or the prefix
constructor) outside the reverse observer. The existing generic
`AuthorityIssuer` is also used for other legitimate bound transitions,
including resolver-owned failure reduction, so a broad source/static ban on
“the issuer” would either fail valid code or become a brittle grep exception.
Use Rust visibility/compile-fail proof for constructor privacy and a separate
semantic call-graph assertion that executors receive only the non-authoritative
`require -> ()` seam.

## Previous ReviewState finding closure

| Prior finding | Amended result |
| --- | --- |
| P2-1 stage-typed C7 | **Partially closed.** The key sum and production-stage harness are the correct architecture, but the impossible five-by-four audit remains (new P2-3). |
| P2-2 selected-root overlay contradiction | **Closed in the ownership table.** The pending-`After` index/worktree owners are now explicit. The checkout status representation still prevents implementation of the valid worktree overlay (new P2-2). |
| P2-3 global dirty summary | **Partially closed.** Facts are path-keyed, but staged and worktree status remain inseparable under `IndexAndStatus` (new P2-2). |
| P2-4 managed-parent preflight | **Closed for catalog-backed managed parents.** `ManagedParentBootstrapIntentV1`, physical steps, recovery, ownership, and conversion are specified. The earlier pre-catalog workspace runtime bootstrap remains undefined/circular (new P2-4). |
| P3-1 reverse-order wording | **Materially closed.** The example is normative; only the minor before/after sentence remains (new P3-1). |

Additional checks:

- preservation ambiguity now uses the common relation and matching diagnostics
  cannot override fresh ambiguity: **closed in design**;
- every stored terminal row is both validator-constrained and independently
  observed: **closed in design**;
- the selected-root physical-`After` owner transfer and no-index-overlay rule:
  **closed in design**, subject only to the status-domain correction;
- rollback-prefix request/owner/action/phase binding and observer-only issuance:
  **closed in design**, with the P3 issuer-name clarification above; and
- the Linux UUID/support-table/remount decision is conservative and auditable:
  **closed in direction**, subject to the retained-handle flag correction in
  P2-1.

## Required amendments before GO

1. Change the Linux provider and probe to retained-fd `AT_EMPTY_PATH` handle
   acquisition.
2. Split checkout status into exact index-side and worktree-side authority so
   a worktree overlay suppresses its own status without hiding index drift.
3. Replace the inherited 20-cell executable claim with independently closed
   service, binding, and invocation-stage cardinalities.
4. Specify a capability-neutral, non-catalog-backed workspace runtime/lock
   bootstrap and keep the ext4 provider out of ordinary mutator locking.
5. Move the read-only tracked/index collision gate before private-catalog
   bootstrap and before any private-tree mutation.
6. Apply the two P3 wording/privacy clarifications.

The design should return for one more focused confirmation after these
amendments. It is close, but entering R1 now would force implementation teams
to invent security-boundary behavior independently—the patch-chain failure
this checkpoint is intended to prevent.
