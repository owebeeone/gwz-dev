# R4b-P fourth remediation filesystem-interface re-review 2

Date: 2026-08-12

Reviewed independently:

- amended `GwzM5-8R4bP1P2-RemPlan-4.md` at SHA-256
  `42773514753844e473d1bc118c449e0ec57e2ad785e595d01800bfcc21a22fd1`;
- `GwzM5-8R4bLinuxCapabilitySpike.md` at SHA-256
  `ddb04c0b21224bed8bdd512a4f37586cda0e381f03fa6eb570067a333bf899b3`;
- every finding in `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS.md`; and
- the frozen checked-artifact restart/durability contracts only where needed
  to detect a new circular or unclosed physical state.

I did not read or coordinate with the state-machine reviewer. This was a
read-only design review; I did not modify production or test code.

## Verdict

**NO-GO for R1/R2 implementation.** There is no P0 finding. One P1
restart-capacity defect and one P2 pre-catalog bootstrap gap remain.

The amendment closes the original review's roaming-anchor defect. The fixed
catalog anchor breaks the intent-publication circularity, and the durable
`BarrierIntentV1` binds the roaming anchor to one target parent before it can
leave home. The literal home/target table recovers an outbound crash even for
an authority-free proof and prevents another action from creating a second
anchor. The direct write-through `CatalogBootstrapV1` exception also gives the
first private catalog a plausible non-catalog-backed origin.

The Linux ext4-only decision, fallible byte allocation, and exact Windows
flush-handle contract close the other ReviewFS findings. The required Linux
probe has not yet executed, so R1 remains gated on that evidence, but its
fail-not-skip remount decision is now an executable R0-L contract rather than
an unresolved provider choice.

Two issues remain. First, unique scratch may be retained indefinitely while
every scratch entry consumes a fixed global catalog budget. Repeated crashes
can therefore make GWZ's own valid state exceed the budget after authority
publication and strand recovery. Second, the runtime lock hierarchy is said
to be barriered before the lease and catalog exist, but no pre-catalog Windows
barrier or restart table owns that edge.

## Findings

### [P1-1] Valid retained scratch can exhaust the bounded catalog and strand an active action

**Where:** RemPlan-4 lines 106-132, 151-156, 294-299, and exit criteria 3-4.

**Contradiction.** Every private-root entry consumes the fixed entry/name/
metadata budget. Authority, intent, goal, and bootstrap scratch names carry a
random nonce and may be retained indefinitely; correctness is expressly not
allowed to depend on scratch GC. The plan also promises that GWZ's own valid
scratch never becomes ambiguous merely because a crash exposed it. Those
properties cannot all hold for a fixed complete-catalog entry budget.

**Failure scenario.** Publish one checked-action authority, then repeatedly
crash after creating or flushing a nonce goal scratch but before goal
publication. Each file is valid inert scratch, so the current grammar retains
it and the retry creates another unique name. Eventually the private root has
more entries than the complete-catalog bound. Acquisition can then either:

- stop before finding/proving the active authority, stranding its durable
  action;
- exceed the promised enumeration bound; or
- require scratch GC to make the active action recoverable.

The first and third choices violate restart-to-terminal closure, and the
second violates the bounded catalog contract. This is the same class of
self-stranding problem that section 3.4 correctly removes from large payloads,
but it now occurs in catalog cardinality. A similar admission problem exists
for the fixed-budget retired subtree: cleanup needs retirement slots after a
managed mutation, while physical deletion remains best effort.

**Required correction.** Freeze capacity admission and reservation before any
authority, internal intent, or managed mutation. An admitted action must own
enough deterministic active and retirement slots for its worst-case complete
transition, including intent/alias/authority cleanup. GC failure may prevent a
new action from being admitted, but it may not prevent an already admitted
action from reaching terminal recovery.

Unpublished scratch also needs a bounded lifetime shape. Acceptable designs
include one deterministic action-bound scratch slot per kind with exact safe
reuse/reset, or an inert scratch subtree that is excluded from complete active
catalog enumeration and consumed only by bounded GC batches. A random new
catalog entry on every retry plus indefinite retention is not bounded. The
restart matrix must inject the same pre-publication crash enough times to
cross the nominal capacity and prove that the active authority still reaches
terminal state without relying on best-effort GC.

### [P2-1] `WorkspaceRuntimeBootstrap` is named but has no non-circular Windows barrier protocol

**Where:** RemPlan-4 lines 350-369, 421-425, 658-668, 683-686, and exit
criteria 6-7.

**Gap.** The amended design correctly gives the first private catalog a direct
write-through `CatalogBootstrapV1` exception and gives later managed parents a
catalog-backed intent. The earlier workspace runtime bootstrap, however, is
only described as additive creation of `.gwz/locks` and the lock file followed
by no-follow reopen, flush, and “barriers” before the guard returns the
workspace lease.

On Windows the section defines only two barrier anchors:

- the fixed catalog anchor, which does not exist until after private-catalog
  bootstrap; and
- the roaming anchor, which requires both that catalog and a durable
  `BarrierIntentV1` under the workspace lease.

Using either for the runtime hierarchy creates the cycle
`lease -> runtime barrier -> catalog/intent -> lease`. Merely observing or
flushing a directly created directory/file is not the required parent-
namespace barrier, and the plan supplies no fixed pre-catalog names, record,
physical-state table, or recovery rule for a crash or concurrent creator at
this edge. The `WorkspaceRuntimeBootstrap` name in R1 does not itself freeze
that interface.

**Required correction.** Add a literal pre-lease
`WorkspaceRuntimeBootstrapV1` protocol, or explicitly narrow the lease-file
durability claim and prove why it needs no such barrier. If durability remains
required, the protocol must use only direct write-through no-replace edges and
fixed, globally discoverable pre-catalog names/anchors; it cannot call the
catalog-backed roaming barrier. Freeze:

- final/staging/active names and their retained parent identities;
- missing, exact, partial, substituted, and concurrent-winner forms for
  `.gwz`, `.gwz/locks`, and the final lock file;
- which process may adopt a winning no-replace result and how all contenders
  converge on the same lock object before checked mutation;
- crash recovery before and after each directory/file publication, file
  flush, namespace barrier, lock acquisition, and lease return; and
- the transition from this exceptional grammar to
  `CatalogBootstrapV1` without moving or replacing a locked file.

The current `CatalogBootstrapV1` and catalog-backed
`ManagedParentBootstrapIntentV1` directions are otherwise coherent. This
finding is confined to the pre-lease runtime substrate.

## ReviewFS finding closure

| Prior finding | Re-review result |
| --- | --- |
| P1-1 roaming anchor is undiscoverable after outbound crash | **Closed.** `BarrierIntentV1` is made durable by the fixed home-local catalog anchor before outbound, binds the exact target parent/name, and has a literal home/target recovery table. A second action must recover it first. |
| P2-1 catalog has no transient grammar | **Structurally closed, with new P1-1 capacity defect.** Active family, infrastructure, internal intent, scratch, retired, and malformed/foreign categories are explicit; scratch is never decoded as authority. Their finite-budget lifetime still needs admission/reservation. |
| P2-2 Linux identity is not an executable capability | **Closed in design.** The first provider is explicitly local ext4 with runtime `FS_IOC_GETFSUUID`, nonzero 16-byte UUID, bounded persistent handle, component casefold query, negative support equality, and fail-not-skip x86-64/ARM64 remount evidence before R1. Evidence execution remains a mandatory gate, not a design finding. |
| P2-3 ordinary observation can allocate infallibly | **Closed.** `isize::MAX`/container capacity is checked, `try_reserve_exact` is required before reading, allocation failure is typed, and exact length/EOF/identity revalidation remains mandatory. |
| P2-4 Windows exact flush rights are unspecified | **Closed.** One no-follow `GENERIC_READ | GENERIC_WRITE` handle supplies exact bytes, full identity, and `FlushFileBuffers`; ACL/read-only/flush failure occurs before any anchor move and cannot fall back. |

## Focused architecture audit

| Area | Result |
| --- | --- |
| Component-wise equivalence | **GO.** ASCII protocol paths and per-parent Linux/Windows plus volume-qualified macOS modes retain stable family visibility without Unicode approximation. |
| Fixed catalog anchor | **GO.** Its two home-local names are always discoverable, and direct write-through first-catalog bootstrap avoids requiring that anchor before it exists. |
| `BarrierIntentV1` and roaming anchor | **GO.** Binding, ordering, duplicate identity rule, recovery-before-next-action, and fault vocabulary close outbound/return crashes without an outer family authority. |
| First catalog bootstrap | **GO.** The fixed pre-catalog record and staging names are scanned before catalog access and use direct durable no-replace edges. |
| Runtime-root bootstrap | **NO-GO (P2-1).** The pre-lease Windows namespace barrier still has no owner or physical table. |
| Managed-parent bootstrap | **GO.** A catalog-backed immutable intent owns each staged component, marker, publication, successor, and cleanup cursor; user-managed parents remain non-creatable. |
| Catalog/payload bounds | **NO-GO (P1-1).** Reads and payloads are bounded correctly, but valid unbounded scratch accumulation can exceed the complete-catalog cardinality bound. |
| Linux ext4 capability | **GO in design, evidence pending by rule.** Ext4 is deliberately the only admitted provider and both release architectures must pass exact remount and negative-table gates before R1. |
| Windows exact leaf and namespace edges | **GO subject to the runtime exception.** Exact file flush, no-replace publish, write-through retirement, catalog intent, roaming barrier, and post-edge reobservation are frozen; only the pre-catalog runtime barrier is unresolved. |

## Exit decision

Do not begin R1 production edits. Add finite catalog-capacity admission/
reservation so valid crash residue cannot self-strand an admitted action, and
freeze the pre-lease Windows runtime-bootstrap protocol without invoking a
catalog-backed barrier. Then repeat this focused filesystem review and execute
the already specified R0-L Linux evidence. No v1 wire field or merge lifecycle
phase is required for either correction.
