# R4b remediation R2 checked-artifact consumer checkpoint

Date: 2026-08-14

Status: **lead-owned interface freeze proposed for independent review; no R2
production conversion begins until both critical reviews accept it**.

## 1. Purpose and sequencing

R1 is accepted at workspace/core/CLI tuple
`7e4ccdf80c808367171b1df1a5210c89ac36ec68` /
`c239df33ddf567d4eaca182ca8231ed575e95769` /
`3cca145c0b32410f250f640730ed7ca18f1da59f`. It froze the checked-artifact
capability, protocol, catalog, admission, namespace, and managed-parent
interfaces. It intentionally did not install production providers or convert
consumers.

R2 makes those interfaces executable and replaces every successful legacy
checked-artifact/managed-parent path. R3-R6 in
`GwzM5-8R4bP1P2-RemPlan-4.md` remain required after R2. R4b-G does not begin
until those phases and their independent settled-tree reviews pass.

R2 does not:

- change the public GWZ protocol;
- enable the merge-v1 decoder, writer, migration, or dispatcher;
- change a merge lifecycle state, action, or durable phase;
- make an unsupported filesystem best-effort safe;
- auto-create a Git/user-owned parent such as `.git/info`; or
- expose a raw provider, filesystem identity, reservation, namespace backend,
  proof constructor, or checked protocol record to a consumer.

## 2. Current implementation boundary

The R1 contracts are production-compiled but not production-executable:

- `WorkspaceRuntimeBootstrapV1` has only a test implementation;
- the pre-catalog platform provider is compile-only and rejects every call;
- `CatalogBootstrapV1` has no filesystem implementation;
- catalog recovery and action admission have classifiers but no physical
  drivers;
- `ActionNamespace` has no real retained-handle backend;
- `LeafObserver` has no real implementation; and
- `ManagedParentBootstrap` has no real provider.

The live code still depends on `CheckedArtifact`, its random scratch names,
family-prefix scan, `prepare_parent`, the Windows no-op parent sync, and
`WorkspaceMutatorLock`'s recursive `.gwz/locks` creation. R2 replaces those
paths; it is not an aggregate-test-only checkpoint.

## 3. Consumer-facing facade

Only `checked_artifact` owns the orchestration facade. Callers receive opaque
results or invoke complete high-level operations. The facade exports no R1
protocol or provider type.

```text
CheckedActionOwner = closed private enum
  Merge { workspace_id, merge_id, durable_operation_id }
  Stash { workspace_id, stash_id }
  CommitMarker { workspace_id, marker_id }
  V1Journal { workspace_id, merge_id, journal_binding }

CheckedManagedPurpose = closed crate-visible enum
  MergeStore
  MergeArchive
  PreservationBundles
  RootPreservationMarkers

CheckedArtifactRequest {
  owner,
  root_kind,
  normal_ascii_components,
  operation = Observe | Replace | Remove,
  expected,
  goal,
}

CheckedParentRequest {
  owner,
  declared_purposes,
}
```

Consumers construct those values only through purpose-specific functions. No
function accepts an action digest, request-owner hash, schedule, reservation,
provider identity, canonical path identity, object identity, observed mode,
or evidence token.

The coordinator derives:

```text
request_owner_binding = SHA-256(
  "gwz-checked-owner-v1\0" || canonical owner tuple
)

action_digest = SHA-256(
  "gwz-checked-action-v1\0" ||
  request_owner_binding ||
  operation kind || root kind || normal ASCII protocol path ||
  expected state/hash || goal state/hash || declared purpose set
)
```

The action digest is independent of which declared parents are currently
missing. That makes it stable after a partial bootstrap. On restart, a
resident or retired catalog action for the binding wins over replanning. A new
action preflights only the missing suffixes. A fully existing, exact declared
parent is proof-only and is not given a fake bootstrap row.

The action owner is durable, not the transient resume/abort invocation ID. A
v1 journal binding is the exact pending owner/action/phase identity already
stored in the record. A consumer cannot substitute an invocation ID for a
durable owner.

## 4. Runtime bootstrap and lease

The physical runtime owner implements `WorkspaceRuntimeBootstrapV1`. The
existing public `WorkspaceMutatorLock` becomes a compatibility wrapper holding
its opaque lease.

The runtime bootstrap owns only:

- the workspace root and its actual Git directory, including linked
  worktrees;
- the fixed Git-directory bootstrap guard;
- `.gwz`;
- `.gwz/locks`; and
- `.gwz/locks/workspace-mutator.lock`.

It uses retained no-follow handles and additive no-replace creation. Racing
creators reopen and validate the winning object. The final advisory lock is
held before the bootstrap guard is released. A symlink, reparse point, wrong
kind, replacement, or nonconvergent winner fails without using checked
authority.

Compatibility remains exact:

- `try_acquire` returns `Ok(None)` on contention;
- `acquire` maps contention to `UnsupportedOperation` with the existing
  message;
- the unlocked final lease file remains stable;
- `path()` and `lock_path()` remain unchanged; and
- `Drop` releases only the advisory lock.

Dry-run and validation failures that currently precede lock acquisition remain
bootstrap-free. Create/init may bootstrap only after the workspace Git
repository exists. Tests using a non-Git temporary directory are converted to
real repositories rather than inventing `root/.git` semantics.

## 5. Retained filesystem provider

One provider transaction owns identity, component equivalence, rename domain,
no-follow walking, collision inputs, leaf observation, and namespace
durability. These facts are not split among consumer-visible adapters.

The platform profiles remain exactly the R1 profiles:

- Linux: local ext4, `FS_IOC_GETFSUUID`, retained-fd persistent handle, and
  per-parent `FS_CASEFOLD_FL`;
- macOS: persistent volume/object identity and actual per-parent lookup mode;
- Windows: NTFS volume GUID, `FILE_ID_INFO`, and per-directory case-sensitivity
  mode.

Every normal path component is ASCII and at most 255 bytes. Non-ASCII protocol
paths reject before catalog or managed-parent mutation. There is no Unicode or
filesystem-name fallback.

The pre-catalog transaction gathers lossless tracked worktree and Git-index
facts for the complete private domain. Exact, ancestor, descendant,
platform-equivalent, staged, conflict-stage, skip-worktree, gitlink, and
tracked-type collisions reject before private state exists. The provider then
revalidates the retained root, every component identity/mode, rename domain,
and collision digest immediately before catalog bootstrap.

## 6. Catalog bootstrap and global enumeration

The catalog executor consumes only `RevalidatedPreCatalogPermitV1`. It owns the
fixed scratch, active, staging, final, and retired roles and executes the
accepted aggregate classifier one edge at a time.

Every read is bounded before allocation. Every file/directory is opened
no-follow and checked for canonical kind, identity, metadata, name, owner
marker, and exact record. An allowed staging infrastructure-record repair is
followed by a fresh five-role observation. A rejected aggregate performs zero
mutation.

Successful bootstrap returns an opaque retained catalog containing the exact
catalog root, catalog anchor, roaming anchor home, retired root, provider
binding, and global enumeration capability. The global enumeration classifies
every child into the exhaustive infrastructure, active action, scheduled
scratch, retired, malformed-recognized, or foreign grammar. It never filters
by a family prefix before applying the global budget.

## 7. Action admission and capacity

The coordinator first looks up the derived action/owner binding in the bounded
global catalog. It resumes an exact existing action; any conflicting or
ambiguous action stops.

For a new action it performs this exact durable sequence:

1. derive the immutable managed-parent plan from read-only observations;
2. derive the complete schedule and reservation from that plan;
3. persist admission `Idle -> Preparing`;
4. create the one indexed staging action directory;
5. write and flush the resident reservation;
6. publish staging to the deterministic final action name without replacement;
7. persist admission `Preparing -> Idle`;
8. reobserve the complete catalog; and
9. obtain `AdmittedActionV1` only from idle + missing staging + exact final
   reservation with no extra children.

Retry reuses the same names and capacity. It never chooses a nonce. Capacity
includes all barrier, managed-generation, marker, cleanup, and terminal
retirement slots before the first action mutation.

## 8. Namespace, leaf, and authority execution

The concrete retained-handle backend implements every `ActionNamespace`
operation, including both forward and restart observations for managed
component installation and marker retirement. Default “unavailable” managed
operations are not reachable in production.

An existing durable leaf is read through one retained handle, bounded,
fingerprinted, flushed, reobserved through the same parent, and checked for
identity/content/length stability. Durable absence uses the matching
two-sided namespace proof. Source and goal payloads are streamed; payload size
is not confused with protocol-record size.

Replacement/removal executes only after an admitted action and an
owner-private coherent authority observation. Publication, detach,
retirement, duplicate recovery, cleanup, and proof-only completion use only
scheduled namespace roles. On Windows, correctness-critical removal is
durable retirement; namespace edges use write-through rename and the accepted
catalog-anchor/barrier-intent/roaming-anchor protocol. No successful path calls
the legacy Windows no-op parent sync.

## 9. Managed-parent bootstrap

The managed-bootstrap provider supports only the four frozen purposes. It
observes the deepest retained existing parent and returns a plan only for a
missing suffix. Purposes are unique and in enum order.

Overlapping missing suffixes are not independently requested in one action.
For example, a missing `.gwz/merge` is prepared as `MergeStore`; a
`MergeArchive` action is legal only when `.gwz/merge` already exists because a
source merge record has been validated.

The owner preflights, admits, binds, and executes the exact plan. Each missing
component has a staged directory, ownership marker, installed observation,
durable successor, prior-generation retirement, marker retirement, and final
reproof. Restart consumes the resident intent and scheduled slots. It never
replans from a partially completed live path.

Writers receive an opaque retained-parent proof. They may write only through a
facade operation that revalidates that proof. A path string or successful
`exists()` check is not parent authority.

## 10. Consumer conversion table

| Consumer | Declared purpose / operation | Frozen ordering |
| --- | --- | --- |
| `WorkspaceMutatorLock` | runtime bootstrap only | acquire final lease before open-operation gate and mutation |
| first merge record | `MergeStore` and `PreservationBundles` when missing | both parents durable before record; record durable before Git |
| merge record rewrite | exact existing `MergeStore` | no parent creation; unknown fields and exact reread preserved |
| terminal archive source-only | `MergeArchive` when missing | terminal/source validation before bootstrap; no creation for both-absent/destination-only |
| ordinary or merge stash bundle | `PreservationBundles` when missing | parent durable before first bundle/native stash mutation |
| commit or merge marker | `RootPreservationMarkers` when missing | all ordinary preflight first; parent durable before member/root mutation |
| workspace marker/lock checked mutation | checked workspace artifact action | exact source/goal and existing managed parent |
| `.git/info/exclude` | checked Git-directory artifact action | observe existing Git-owned parent; never bootstrap `.git/info` |
| v1 checked store/root/bundle paths | same purposes and artifact actions | test-gated until A1; no legacy raw writer |

`artifact::write_atomic` remains for unrelated ordinary artifacts. Managed
callers receive a prebootstrapped/retained-parent write seam; they no longer
rely on its recursive parent creation. `CheckedArtifact::prepare_parent`,
random checked-artifact scratch names, and family-prefix-only enumeration have
no successful converted caller and are removed after parity tests pass.

## 11. Implementation packages and ownership

R2 is implemented in dependency order:

1. **R2-A — runtime bootstrap.** Concrete capability-neutral bootstrap and
   `WorkspaceMutatorLock` compatibility wrapper.
2. **R2-B — retained filesystem substrate.** Shared provider contract plus
   Linux, macOS, and Windows identity/equivalence/namespace implementations;
   complete collision and leaf observation.
3. **R2-C — catalog and admission.** Physical catalog bootstrap, bounded global
   enumeration, action admission, reservation, retirement, and recovery.
4. **R2-D — managed namespace.** Real `ActionNamespace` backend and
   managed-parent provider/state machine.
5. **R2-E — facade and consumers.** Coordinator-derived owner/action binding,
   merge store/archive, stash bundle, marker/root, and checked workspace/Git-
   directory conversions.
6. **R2-F — closure.** Remove legacy successful paths and run complete local
   and native-platform fault/restart gates.

R2-A and the three R2-B platform implementations may proceed independently
after this interface is accepted. R2-C begins after the shared R2-B contracts
pass. R2-D begins after R2-C admission settles. R2-E consumer lanes begin only
after the facade and opaque retained-parent result are lead-owned and tested.

Expected cohesive owners are:

- `checked_artifact/bootstrap/runtime/`;
- `checked_artifact/capability/pre_catalog/provider/` with OS children;
- `checked_artifact/catalog/`;
- `checked_artifact/admission/`;
- `checked_artifact/namespace/backend/` with OS children;
- `checked_artifact/bootstrap/managed/provider/`;
- `checked_artifact/coordinator/`; and
- focused `checked_artifact/production_tests/` owners.

Existing `identity.rs` and any near-500-line provider are not extended as
concept dumps. A file approaching 1,000 lines triggers a cohesion review; a
new owner targets fewer than 500 lines. A sub-500 file is split earlier if it
starts owning unrelated concepts.

## 12. TDD and fault closure

Each package begins with failing production-shaped tests. The independent
expected fault fixture covers every `CheckedArtifactFaultKeyV1` and is not
derived from the runtime iterator. Each row records consumer, stage, first-run
result, fresh-process retry, exact terminal bytes/identity, catalog
cardinality, physical mutation count, and scratch-name set.

Required matrices include:

- runtime creation/recovery, process contention, symlink/wrong-kind, and
  winner convergence;
- every private-domain tracked/index collision with zero pre-catalog mutation;
- all catalog aggregate and admission crash shapes;
- source/goal payloads above one MiB plus protocol-limit-plus-one rejection;
- leaf flush, parent barrier, identity/content/name substitution, durable
  absence, and duplicate recovery;
- all four managed purposes and every component/generation/marker boundary;
- repeated same-boundary crashes past nominal capacity with stable slots;
- full retired capacity while an already admitted action completes with
  deletion disabled;
- executed `K=1,N=8` and `K=8,N=8` maximum schedules, including all 64
  barriers;
- parent identity/mode replacement and ASCII alias collision; and
- unchanged merge/stash/commit ordering, errors, unknown fields, and restart
  results.

## 13. Platform evidence

Local tests are necessary but do not substitute for native behavior:

- macOS x86-64 and arm64 execute identity/equivalence, sensitive/insensitive
  lookup, replacement/mode-change, and converted-consumer tests;
- Linux x86-64 and arm64 execute converted consumers on the R0-L ext4
  provider, including unmount/remount, sensitive/casefold, unsupported mounts,
  permission denial, and hard-link alias cases; and
- Windows NTFS executes per-directory sensitivity, non-ASCII rejection,
  handle/ACL/read-only failures, write-through publish/retire, runtime
  concurrency, catalog/barrier recovery, managed bootstrap, duplicates, and
  the complete fault/restart set.

Every exact-SHA attestation contains the platform/filesystem profile, executed
case-key digest and count, source/manifest digests, and zero skipped required
rows. A release build alone is not this evidence.

## 14. Stop conditions

R2 stops for design review if implementation requires:

- consumer construction of a raw digest, reservation, identity, observation,
  evidence value, or namespace backend;
- a new protocol record, slot, purpose, action, or durability phase;
- an unbounded read or infallible large allocation;
- random scratch allocation on retry;
- managed mutation before capability/collision/catalog/admission acceptance;
- recursive parent creation or legacy Windows parent-sync success;
- a compatibility shim that lets one consumer bypass the coordinator;
- a filesystem fallback outside the closed support table; or
- production merge-v1 decode, write, migration, or dispatch reachability.

## 15. Acceptance gate

R2 is complete only when:

1. all six R1 layers have real production implementations;
2. every inventoried workspace/Git-directory and managed-parent consumer uses
   the facade or a documented non-managed ordinary writer;
3. the legacy successful paths and their dead-code allowances are removed;
4. the independent fault-key set equals the executed production-case set;
5. focused and full core tests, Clippy, formatting, protocol regeneration,
   privacy/call-graph checks, document checks, and LOC/cohesion checks pass;
6. exact native platform behavior evidence passes on one settled tree;
7. installed `gwz` commits that exact workspace/core/CLI tuple; and
8. two independent filesystem and state/protocol re-reviews report no open
   P0/P1/P2 defect.

Only then may RemPlan-4 R3 begin. R4b-G remains behind R3-R6.
