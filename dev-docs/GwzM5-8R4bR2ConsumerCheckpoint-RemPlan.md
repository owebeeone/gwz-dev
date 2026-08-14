# R4b R2 consumer-checkpoint remediation plan

Date: 2026-08-14

Status: **the R2 interface correction gate is closed. R2-A and R2-B are
implemented and locally verified. R2-C0 is implemented and locally verified,
with its required independent critical-interface review pending. Runtime
bootstrap remains capability-neutral, dry runs remain bootstrap-free, and C0
enables no catalog namespace mutation. The retained pre-catalog provider now
owns workspace/Git-directory proof, platform identity and component
equivalence, the complete Git index/tracked-worktree snapshot, physical
private-namespace observation, and immediate retained-handle revalidation.
The lead-owned R2-C first-catalog correction in
`GwzM5-8R2CCatalogBootstrapAmendment.md` is settled and independently accepted
GO/GO with no P0-P3 findings. Physical R2-C proceeds in the C0-C3 sequence
frozen below.
Native-platform release evidence remains part of the R2-F closure gate rather
than a claim made by this local checkpoint**.

The eleventh independent state and code reviews were GO/GO with no P0/P1/P2.
They accepted the complete compiler route from `Cargo.toml` through `lib.rs`,
`workspace_ops/mod.rs`, and `merge/mod.rs` into the v1 tree, its positive
parent/root sentinel, the sealed production runtime, and the exact consumer
manifests. The sole code-review P3 was corrected and both independent focused
follow-ups were GO with P0-P3 all zero.

## 1. Scope and disposition

This amendment controls where it differs from
`GwzM5-8R4bR2ConsumerCheckpoint.md`. It addresses every finding in:

- `GwzM5-8R4bR2ConsumerCheckpoint-ReviewFS.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-ReviewState.md`.

The correction makes no public GWZ protocol change, does not enable merge-v1,
and does not add a durability phase. It narrows the checked boundary and
strengthens private R1/R2 interfaces before production implementation begins.

The controlling re-review reports are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState.md`.

The second re-review reports controlling this final correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-2.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-2.md`.

The third re-review reports controlling the fourth correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-3.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-3.md`.

The fourth re-review reports controlling the fifth correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-4.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-4.md`.

The fifth re-review reports controlling the sixth correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-5.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-5.md`.

The sixth re-review reports controlling the seventh correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-6.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-6.md`.

The seventh re-review reports controlling the eighth correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-7.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-7.md`.

The eighth filesystem re-review controlling the ninth correction is:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-8.md`.

The attempted eighth state review produced no report or verdict, so it does
not count toward the independent-review gate.

The ninth filesystem re-review controlling the tenth correction is:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-9.md`.

The attempted ninth state review likewise produced no report or verdict and
does not count toward the independent-review gate.

The tenth independent re-reviews controlling the eleventh correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-10.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-10.md`.

The eleventh independent GO re-reviews are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-11.md` (one P3 test
  maintenance finding); and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-11.md` (no findings).

The focused confirmations of the P3 maintenance correction are:

- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-11-P3.md`; and
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-11-P3.md`.

The independent GO reviews controlling physical R2-C are:

- `GwzM5-8R2CCatalogBootstrapAmendment-ReviewCode.md`; and
- `GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md`.

Their overlapping P2 findings are corrected as one architectural change:

- `CheckedManagedActionV1` seals the owner class, exact managed request,
  purpose bitset, action digest, and owner binding;
- production managed preflight accepts only that sealed value and derives its
  plan bindings internally;
- reservation derivation defensively compares the plan purpose mask with the
  action purpose mask;
- v0/v1 record owners consume one bounded, exact canonical record observation
  whose IDs and hashed bytes cannot be supplied independently;
- archive authority can only be issued from one stable canonical location pair
  with an exact terminal open source, absent destination, matching record/path
  identity, owner/digest derived from those exact source bytes, and successful
  validation by the complete archive decoder for that version;
- the general `CheckedArtifact`, fact, transition, and policy capabilities are
  private to `checked_artifact`; callers receive only purpose-specific facts or
  completed operations from `checked_artifact::entry`;
- checked bundle evidence and owner derivation are contained in the guarded
  adapter rather than delegated to an unguarded helper layer;
- the entry/caller gate inventories every visible entry plus every adapter
  import and call, rejects re-exports and capability escapes, and positively
  pins the complete bytes of every guarded source module; a new alias, wrapper,
  call, import, string, or commented-out guard therefore changes the allowlist
  rather than depending on discovery in a writer denylist;
- `preservation_stashes` is removed from the open `GitBackend` trait;
  authority-sensitive merge code uses a concrete crate-private observer that
  terminates in the guarded preservation-image leaf, whose
  selected path enumerates Git stash objects directly and uses only local
  decoding helpers;
- the separate preservation-plan caller is byte-pinned and compiler-guarded,
  while the complete v1 authority-observer root, descendant source set, and
  descendant bytes are covered by one deterministic tree digest and one
  inherited non-overridable compiler lint;
- every test-gated production v1 lifecycle source requires the
  private-supertrait-sealed `MergeAuthorityBackend`; only the reviewed
  `Git2Backend` implements that interface, while ordinary `GitBackend` remains
  open outside v1;
- the checked bundle adapter accepts no backend parameter, and the source gate
  freezes the complete crate-wide reference set for the concrete stash
  observer so a new wrapper or consumer fails closed;
- every Rust `#[path]` source-loading edge is positively inventoried, must
  resolve to a regular in-crate `.rs` file, `include!` is rejected, and
  protected source-tree digests include every descendant file regardless of
  suffix;
- the production `V1Runtime` has a private sealed supertrait implemented only
  for `ForwardRuntime`, `ReverseRuntime`, and `FinalizationRuntime`, each of
  which already requires `MergeAuthorityBackend`; arbitrary observation and
  execution doubles enter only through the separately named test runner;
- the complete v1 lifecycle tree and the mixed preservation-artifact adapter
  have exact byte manifests, while the v1 artifact-observation functions keep
  local compiler writer bans; therefore a loader alias, outside-`src` helper,
  new runtime, or changed service adapter requires an explicit reviewed
  manifest update regardless of source spelling;
- the compiler route into that tree is anchored by exact manifests for the
  crate target and each parent module from `Cargo.toml` through `merge/mod.rs`;
  the pinned parent also names a positive sentinel defined by the intended v1
  root, so a resident-but-unloaded protected directory is not accepted; and
- source and compiler protections run in PR/push and release CI, while the
  local release script cannot skip them and every new-tag branch converges on
  one exact-target gate immediately before tag creation or push.

The two P3 gaps were corrected in the same checkpoint: prefixed `.`/`..`
suffixes reject, and literal managed schedule/reservation fixtures cover
one-, two-, and four-purpose declarations, first-merge, partial-bootstrap, and
combined parent-plus-leaf work with exact ordinals, ranges, masks, capacities,
and digests.

| Finding | Disposition |
| --- | --- |
| State P2-1, unrecoverable first-write owners | pre-record merge parent work gets one deterministic workspace-purpose owner; generated stash and marker IDs remain ordinary; no checked leaf action is admitted from a generated ID until its durable merge record exists |
| State P2-2, ambiguous digest bytes | one versioned framed encoding, closed tags, exact field order, and literal vectors are frozen below |
| State P2-3, caller-selected schedule | one coordinator schedule algorithm with 64 barrier ordinals and a closed cleanup table is frozen below |
| State P2-4, checked/ordinary ambiguity | the exhaustive call-site boundary below keeps ordinary commands capability-neutral and names every checked merge class |
| Filesystem P2-1, caller-selected collision facts | the pre-catalog owner gets separate workspace/Git-directory entries; its sealed provider acquires and revalidates the complete snapshot and binds it into the permit |
| Filesystem P2-2, unmanaged/overlapping parents | purpose policy owns minimum retained prefixes, archive prerequisite issuance, and cross-row physical overlap rejection |

## 2. Durable owner boundary

There are three and only three private owner variants:

```text
ManagedParentsV1 {
  workspace_id,
  intent = MergeStart,
}

MergeRecordV0 {
  workspace_id,
  merge_id,
  operation_id,
  source_record_sha256,
}

MergeRecordV1 {
  workspace_id,
  merge_id,
  operation_id,
  source_record_sha256,
}
```

`ManagedParentsV1` is reproducible before the first merge record. It may only
prepare the fixed `MergeStore` and `PreservationBundles` parents for merge
start. It cannot publish a merge record, bundle, marker, workspace artifact, or
Git-directory artifact. The purpose set is action material rather than owner
material. A crash can therefore be resumed by a fresh process without knowing
the discarded in-memory merge or invocation ID. Once the deterministic parent
action completes, the existing merge store publishes the first record through
its own atomic, retained-parent writer. A crash before that publication creates
no user-visible operation; a retry may generate a new ID. A crash after it is
recoverable from the record.

`MergeRecordV0` and `MergeRecordV1` may be issued only from a bounded, exact
read of an already durable merge record. `source_record_sha256` is the hash of
those exact source bytes and changes after a durable successor is selected.
The coordinator receives the verified source bytes, not a consumer-supplied
digest, and derives this field internally. Those bytes already bind the exact
stored record identity, state, pending owner/action/phase row, payload, and
unknown-field container. The action digest independently binds the checked
root, path, operation, expected/goal facts, and purpose set. Together they are
the complete journal binding; there is no second opaque or caller-selected
`durable_step_sha256` input.

Ordinary stash IDs and ordinary commit-marker UUIDs are never checked owners.
Their current store-owned atomic writers remain behind the capability-neutral
workspace lease. A merge preservation bundle or merge marker is checked only
after a merge record exists and therefore uses a `MergeRecordV0` or
`MergeRecordV1` owner.

Fresh-process tests cover crashes before the first managed component, at every
admission handoff, after each parent completes, immediately before and after
the first record publication, and before and after every record-owned checked
action. They assert stable pre-record action name/binding, one catalog entry,
and no duplicate durable merge operation.

## 3. Canonical digest encoding

Both owner and action hashes use streaming framing; variable fields are never
concatenated without lengths and no aggregate preimage allocation is needed.

```text
header:
  domain bytes including its trailing NUL
  encoding version: u8 = 1
  variant: u8
  field count: u16 big-endian

field:
  tag: u8
  length: u64 big-endian
  value: exactly length bytes
```

Field tags are contiguous from zero and appear once in ascending order.
Unsigned integers are fixed-width big-endian. SHA-256 values are exactly 32
bytes. Workspace and operation IDs retain their `ws_` and `op_` prefixes;
merge IDs remain portable slugs. All three accept only ASCII alphanumeric,
`_`, `-`, and `.`, exclude empty suffixes and the `.`/`..` slug, and are at
most 255 bytes. Hashing performs no Unicode normalization. An ASCII path is a
`u16` component count followed by,
for each component, `u16` length and exact bytes. Each component remains
1..=255 bytes and the complete path remains within the existing checked-path
bound. An option is one field whose first byte is `0` for absent or `1`
followed by its framed value for present. A set is never iterated in caller
order; the only set below is a one-byte purpose bitset.

Owner domains and variant codes are:

```text
domain: "gwz-checked-owner-v1\0"
0 ManagedParentsV1: workspace_id, intent_code
1 MergeRecordV0: workspace_id, merge_id, operation_id,
                 source_record_sha256
2 MergeRecordV1: workspace_id, merge_id, operation_id,
                 source_record_sha256
```

The action domain is `"gwz-checked-action-v1\0"`, variant zero. Its fields are
exactly:

1. request-owner binding;
2. operation code: `Observe=0`, `Replace=1`, `Remove=2`, `ParentOnly=3`;
3. root kind: `Workspace=0`, `GitDirectory=1`;
4. normal ASCII protocol path, absent only for `ParentOnly`;
5. expected fact;
6. goal fact;
7. declared-purpose bitset.

A leaf fact is `Missing=0` or `Exact=1 || u64 length || 32-byte SHA-256`.
`Observe` requires equal expected and goal facts. `Replace` requires an exact
goal; `Remove` requires an exact expected and missing goal. `ParentOnly`
requires absent path, missing expected/goal, and a nonempty purpose bitset.
Purpose bits are `MergeStore=0`, `MergeArchive=1`,
`PreservationBundles=2`, and `RootPreservationMarkers=3`; higher bits reject.

The action digest deliberately excludes current parent missingness. The
managed plan and schedule records bind the exact observed missing suffix. A
resident exact action is therefore found before a new plan is considered.

Independent literal preimage and digest fixtures cover all three owner
variants, all four operation codes, both root kinds, missing/exact facts, every
single purpose and legal combined purpose set, boundary-length paths, every
one-field substitution, option presence, and sequence framing. Implementations
must consume the fixture rather than generate expectations from themselves.

## 4. Coordinator-owned schedule v1

No facade or consumer accepts barrier count, cleanup aliases, managed inputs,
or a schedule. For a new admitted action the coordinator uses this exact table:

| Operation and facts | Barrier ordinals | Cleanup aliases |
| --- | --- | --- |
| `ParentOnly` with a nonempty missing plan | all `0..63` | empty |
| `Replace`, missing to exact | all `0..63` | goal + authority |
| `Replace`, exact to different exact | all `0..63` | source + goal + authority |
| `Remove`, exact to missing | all `0..63` | source + authority |
| `Observe`, or exact source already equal to goal | no new action | empty |

All admitted actions conservatively reserve all 64 barrier slots. This fixes
ordinals across recovery and upgrades. Unused slots stay unmaterialized.
Cleanup masks use protocol order `source=bit0`, `goal=bit1`,
`authority=bit2`.

Managed rows occur in purpose-code order. A row uses the fixed purpose path,
the validated retained prefix, and only its physical missing suffix. Generation
and component ranges are assigned by the existing `ActionScheduleV1`
algorithm. Combined parent-plus-leaf actions execute every managed row to final
reproof before authority or leaf work. Their cleanup mask comes from the leaf
row of the table.

Catalog lookup precedes observing new missing suffixes. If an exact resident
reservation exists, its persisted schedule, plan digest, row order, component
ranges, cleanup mask, and barrier ordinals are authoritative. Recovery does not
recompute them from current missingness. If an exact retired completion exists,
the coordinator re-proves the requested final parent/leaf state. Only when no
exact resident or retired action exists may it observe and derive a new plan.

Literal schedule vectors pin empty/one/two/four-purpose plans, all leaf rows,
the first-merge two-purpose row, partial-bootstrap recovery, all 64 barrier
ordinals, cleanup masks, schedule digests, and exact capacity totals including
the `K=1,N=8` and `K=8,N=8` maxima.

## 5. Provider-owned pre-catalog and first-catalog bootstrap

R2-B's sealed platform provider remains the only source of repository,
common-directory, actual-Git-directory, index, tracked-worktree, durable
identity, invocation identity, lookup-mode, rename-domain, and physical
namespace facts. No production entry accepts caller-selected fact slices,
root-kind values, paths, raw lease bytes, or a bootstrap callback.

R2-C replaces the provisional path-plus-lease entries with one target-derived
owner entry:

```text
recover_or_create(catalog_target_lease)
```

`CatalogMutationLeaseV1` binds either the existing workspace final lock or the
fixed Git-target lock to retained target handles. `CatalogLeaseSetV1` prepares
all fixed runtime slots without holding a final lock, deduplicates and sorts
the complete target set, then acquires and revalidates every final lock before
the first catalog mutation. It releases the whole set in reverse order on
contention or failure and never re-enters a transient bootstrap guard after a
final lock is held.

The provider's closed result is either a live-only
`MissingGitPrivateParent(MissingCatalogParentPermitV1)` or a
`Ready(CatalogPermitV1)`. The missing-parent permit can perform only the fixed
no-replace parent-directory edge and cannot manufacture catalog identity,
scratch state, token state, or a ready permit. Complete pre-catalog re-entry is
the only route from that edge to `Ready`.

The ready permit separates three non-interchangeable bindings:

- `FreshObservationDigestV1` proves same-invocation retained-handle and
  rename-domain freshness immediately before each mutation;
- `DurableCatalogTargetDigestV1` proves restart-stable target and actual
  mutation-parent identity; and
- `HistoricalCollisionDigestV1` preserves the original complete collision
  evidence without permanently pinning unrelated later index/worktree state.

Every recovery pass recomputes the stable target digest, performs a new
complete collision scan, and uses historical values only from owner-classified
scratch/active/retired progress. The exact scratch name carries target digest,
historical digest, and random token, so zero-byte and partial-prefix recovery
does not guess. The active record reserves fixed staging. A bounded aggregate
enumeration classifies every reserved role and equivalent alias before one
physical edge; ordinary `.gwz` siblings count against capacity but are not
catalog facts.

Every durable record uses `CheckedDurablePathV1`; invocation identities and
rename domains remain live-only. Generated internal bindings, semantic and
shape vectors, digest material, and record-ID vectors change together before
A1. These are private pre-activation corrections, not public protocol or
released-state migrations.

## 6. Managed-parent ownership policy

The managed owner, not the platform provider, enforces this closed table:

| Purpose | Fixed path | Minimum exact retained prefix | Extra prerequisite | Creatable suffix |
| --- | --- | --- | --- | --- |
| `MergeStore` | `.gwz/merge` | `.gwz` (1) | deterministic merge-start owner or durable merge owner | `merge` only |
| `MergeArchive` | `.gwz/merge/done` | `.gwz/merge` (2) | opaque source-only terminal archive proof | `done` only |
| `PreservationBundles` | `.gwz/stash/bundles` | `.gwz` (1) | deterministic merge-start owner or durable merge owner | `stash/bundles` |
| `RootPreservationMarkers` | `gwz.conf/markers` | user-owned `gwz.conf` (1) | durable merge owner | `markers` only |

The provider returns retained handles/identities, not authority to weaken this
table. Exact retained path, component count, identities, modes, and fixed
purpose path are checked against the policy. A full existing path is proof-only
and is omitted from a new bootstrap plan.

`MergeArchive` can be requested only through a constructor that consumes an
opaque prerequisite issued by merge-store arbitration after proving exactly
one valid terminal source, absent destination, matching workspace/merge owner,
and exact source bytes. Syntactic decode and a terminal state label are not
sufficient: the exact bytes must also pass the complete version-specific
archived-record validator used by production v0 archive recovery or the
test-gated v1 equivalent. This rejects, for example, a `completed` envelope
that still contains pending continuation or other contradictory open-state
material. A boolean or path string cannot stand in for this prerequisite.

Before schedule derivation, the owner expands every missing suffix into
canonical physical component edges. Duplicate or ancestor/descendant overlap
between rows rejects. Thus `MergeStore + MergeArchive` cannot independently
own `merge`: if `merge` is missing the archive row violates its minimum; if it
exists the merge-store row is proof-only and absent. Missing `gwz.conf` always
rejects before admission or mutation.

Tests cover all purpose pairs, every legal retained count, missing
`gwz.conf`, archive both-absent, destination-only, source-only, and existing
destination states. The legal archive case plans exactly `done`. Every rejected
case proves no admission, private mutation, managed mutation, or Git mutation.

## 7. Exhaustive checked/ordinary call boundary

`WorkspaceMutatorLock` remains capability-neutral. Ordinary commands do not
enter the checked provider merely because they share a lock, marker, bundle,
workspace-boundary, or `.git/info/exclude` helper.

| Call class | Writer / owner | Checked provider? |
| --- | --- | --- |
| create/init/branch/materialize/stage/tag/pull/ordinary commit | existing capability-neutral workspace and Git writers | no |
| ordinary workspace lock/manifest/boundary rewrite | `sync_workspace_boundary` atomic writer under runtime lease | no |
| ordinary stash bundle | stash store atomic writer under runtime lease | no |
| ordinary commit marker | commit-marker atomic writer under runtime lease | no |
| first v0 merge record | deterministic checked parent preparation, then merge-store atomic writer through retained-parent proof | parents only; record leaf no |
| later v0 record rewrites and archive record bytes | merge-store durable writer preserving unknown fields | no checked leaf; archive parent may be checked |
| v0 merge preservation bundle/marker/root lock | `MergeRecordV0` purpose or leaf facade | yes |
| v0 merge `.git/info/exclude` preservation/publication/rollback | `MergeRecordV0` Git-directory leaf facade | yes |
| test-gated v1 preservation bundle/marker/root/Git-directory work | `MergeRecordV1` purpose or leaf facade | yes, unreachable from production dispatch until A1 |
| v1 record bytes | v1 store-owned durable writer | no checked leaf; its checked effects use the record owner |

The merge-specific facade is not installed below shared ordinary helpers. The
ordinary boundary writer and ordinary stash/commit paths remain documented
non-managed writers. Checked merge callers use explicit sibling entry points
and cannot fall through to those raw paths after conversion.

Call-graph gates enumerate every ordinary command and assert it cannot reach
checked capability, catalog, admission, namespace, or managed bootstrap code.
The complementary source inventory enumerates every selected merge
preservation, publication, rollback, and recovery entry, visible item, import,
and call. Checked bundle owner/evidence helpers are physically contained in
their guarded adapter, so an unguarded intermediate helper cannot be inserted
without changing that inventory.

Each checked entry, adapter, or authority-observation root has a
non-overridable compiler lint boundary, but the lint's writer list is defense
in depth rather than the completeness proof. The proof is a positive,
byte-exact allowlist of every guarded source module. The authority-observer
root additionally uses one deterministic aggregate over the sorted relative
paths and exact bytes of its complete Rust descendant tree. Adding, moving, or
changing a nested helper therefore fails closed along with any direct call,
imported alias, same-name function pointer, crate-local wrapper, changed
literal, or commented-out guard. The separate preservation-plan caller is
protected as a complete source module. The source checker pins the concrete
crate-private preservation observer, rejects restoration of the observer to
the open `GitBackend` trait, freezes every crate-wide reference to that
observer. The compiler-sealed production runtime—not a backend-name scan—keeps
the open trait out of the production v1 service. The concrete observer
directly enumerates and decodes native stash objects without calling the
backend trait, shared stash API, shared repository opener, or external OID
parser. Its entry, implementation, and checked bundle consumer are all in the
positive source allowlist. The path-edge scanner rejects direct unapproved
loaders as defense in depth. Completeness comes from the exact bytes of the
mixed artifact adapter and complete v1 tree, plus the compiler-sealed service
runtime and local compiler bans on the v1 observer functions. `Git2Backend`
retains an inherent observation method for ordinary
and test use. Alternative backends remain available for ordinary dependency
injection, but cannot implement or substitute this checked observation.

PR/push CI and release CI run the source and compiler gates on their exact
checkout. The local release script has no compiler-skip option. It runs the
gates on its initial detached worktree and routes every new tag—whether or not
a version commit was required—through one finalizer that resets to and verifies
the exact target SHA, then reruns the source checker, executable
counterexamples, and all-target/all-feature Clippy before tag creation or push.
Retrying an already-created tag likewise gates its exact target before push.
Atomic push uses the verified SHA as the source for both the remote branch and
tag refspec, so local branch movement cannot substitute a different commit
after the gate.
The PR/push workflow uses Python 3.11, the minimum standard-library runtime
needed to import and execute the release-boundary tests.

## 8. TDD implementation sequence

The interface correction precedes all original R2 packages:

1. **R2-I1 collision contract:** failing compile/behavior tests remove
   caller-supplied facts, split root entries, and bind/revalidate the snapshot.
2. **R2-I2 parent policy:** failing pair/minimum/prerequisite tests enforce the
   table and overlap rejection.
3. **R2-I3 identity and schedule:** add the private framed owner/action types,
   independent literal vectors, closed schedule derivation, and forbid raw
   schedule construction outside protocol tests/coordinator.
4. **R2-I4 boundary inventory:** pin the exhaustive checked/ordinary call map
   before a consumer conversion can compile.
5. **R2-I5 semantic/archive and compiler closure:** prove archive authority
   with genuine terminal fixtures plus contradictory terminal negatives;
   contain the checked helper graph; add adversarial compiler probes for raw,
   nested, same-name function-pointer, and crate-local writer aliases.
6. **R2-I6 positive boundary closure:** freeze complete guarded source, close
   the production read-only trait delegate at its concrete leaf, retain the
   compiler lint as defense in depth, and make exact tagged-commit gating
   mandatory. Pin the fourth-review `copy`, crate writer, backend writer,
   commented guard, skip flag, and post-version-commit counterexamples.
7. **R2-I7 concrete observer and tag convergence:** remove the open backend
   callback from the trait and checked observation, route all new tags through
   one exact-SHA finalizer, and execute the boundary suite on its declared CI
   Python.
8. **R2-I8 complete caller containment:** protect the preservation-plan caller
   and the complete v1 authority-observer source tree with both inherited
   compiler enforcement and a deterministic path-and-byte manifest. Pin the
   sixth-review direct-writer, nested-helper, same-name function-pointer, and
   new-helper-file counterexamples.
9. **R2-I9 sealed implementation and consumer closure:** admit only the
   compiler-sealed `Git2Backend` to v1 authority and physical execution,
   eliminate the checked bundle adapter's unused backend injection, freeze the
   complete concrete-observer reference set, and pin the seventh-review
   import-aliased alternative backend and unprotected-consumer counterexamples.
10. **R2-I10 compiler-loaded source closure:** freeze all approved `#[path]`
    edges and their regular in-crate `.rs` targets, reject `include!` and
    macro-produced nonliteral edges, include every descendant suffix in
    protected tree digests, and pin both eighth-review `.inc` counterexamples.
11. **R2-I11 sealed service and exact consumer modules:** private-seal the
    production `V1Runtime` to the three `MergeAuthorityBackend` runtimes,
    separate the generic test runner, byte-pin the complete v1 tree and mixed
    preservation-artifact adapter, and retain function-local compiler bans on
    v1 artifact observation. Pin the ninth-review `cfg_attr`, imported
    `include`, outside-`src` target, and split-runtime examples.
12. **R2-I12 compiler-root trust anchor:** byte-pin the crate target and every
    parent module edge through `merge/mod.rs`, bind the intended v1 root with a
    positive compiler sentinel, and pin the tenth-review alternate-root case
    plus redirects at each higher compiler edge.
13. Run focused tests, full `gwz-core` tests, Clippy, formatting, protocol and
   document checks; commit one exact workspace/core/CLI tuple.
14. Obtain two independent settled-tree re-reviews. Any P0/P1/P2 finding repeats
   this correction gate.

Items 1-14 are complete. R2-A and R2-B are implemented at the checkpoints
below. The independently accepted R2-C amendment now executes in four bounded
packages:

15. **R2-C0 durable schema and lease interfaces:** write failing schema,
    compile-boundary, wrong-target, shared-Git-target, lock substitution, and
    batch-order tests; replace every durable mixed-purpose path with
    `CheckedDurablePathV1`; introduce the three digest newtypes, closed
    preflight states, target-bound leases, and lease-set protocol; regenerate
    bindings/vectors atomically. No catalog namespace mutation is enabled.
16. **R2-C1 aggregate grammar and classifier:** write failing zero/prefix,
    malformed/equivalent/duplicate, capacity, staging-ownership, current
    collision, target-substitution, and completed-reopen tests; implement the
    dynamic scratch parser, bounded full-parent observation, owner-private
    aggregate facts, pure one-edge classifier, and literal state/fault matrix.
    The classifier has no filesystem writer.
17. **R2-C2 physical first-catalog owner:** write failing real-filesystem and
    restart fault tests; implement the missing-parent retry and each classified
    no-follow/no-replace/write/flush/publish/reobserve edge behind sealed
    `CatalogOwnerV1`; prove at most one edge per decision and return only an
    opaque retained complete catalog. This is the first package that enables
    catalog namespace mutation.

    The controlling catalog amendment now freezes the exact ten-slot completed
    layout and the ordered active-owned staging prefixes before the staging
    writer is implemented. `catalog-format-v1` and
    `retired-actions-descriptor-v1` carry the same canonical identity-pinned
    infrastructure record; the catalog-format record is the ownership marker,
    so C2 may not invent an unnamed or eleventh child.
18. **R2-C3 admission, reservation, and handoff:** connect the retained catalog
    to reservation lookup/publication and managed-parent handoff, preserve the
    accepted schedule/owner boundaries, run aggregate restart/fault matrices,
    and close the checked/ordinary call graph without enabling A1.
19. Run focused, full, structural, protocol/document, format, and all-target/
    all-feature Clippy gates after every package. Obtain independent critical
    interface review after C0 and settled implementation reviews after C2/C3.

R2-D through R2-F follow accepted R2-C. R3-R6 and R4b-G shared-router/
cross-driver integration follow, then the two final full-tree R4b reviews.
M5b and A1 remain later.

### R2-A implementation checkpoint — 2026-08-14

R2-A is complete locally. The checkpoint passed:

- 1,238 `gwz-core` library tests, with zero failures and one ignored test;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- the 52-case checked-artifact adversarial boundary suite and the six-case
  release-boundary suite; and
- Rust 1.95 formatting, all-target/all-feature Clippy with warnings denied,
  and the checked-artifact source-boundary checker.

The focused runtime suite also covers live contention, first-acquirer
convergence, linked-worktree Git-directory placement, wrong-kind and symlink
rejection, final-file replacement, parent substitution, changed linked-
worktree indirection, post-drop reacquisition, and dry-run non-creation.
Platform-native release runs remain required by R2-F before R2 as a whole is
closed.

### R2-B implementation checkpoint — 2026-08-14

R2-B is complete locally. The sealed provider now has real macOS, Linux ext4,
and Windows NTFS implementations for the accepted durable-identity,
per-directory equivalence, and rename-domain profiles. It retains the selected
root, actual Git directory, common Git directory, fixed private container when
present, and index file when present. The collision snapshot binds those
objects plus exact index bytes, complete sorted index metadata (including
object identity and stat fields), tracked-worktree kinds, and the physical
private namespace, then freshly reobserves the same facts immediately before
catalog bootstrap.

Production-shaped tests cover ordinary and linked worktrees, the actual
Git-directory entry, all four index stages, exact/ancestor/descendant and
ASCII-equivalent collisions, extended flags, gitlinks, regular/directory/
symlink worktree-kind drift, same-path object replacement, private-leaf drift,
optional Git-directory container appearance, symlink/wrong-kind rejection,
and zero pre-catalog mutation on every rejected row. The native macOS provider
also executes locally and issues the accepted persistent-object profile.

The exact-tree structural source gate inventories all four platform module
edges, and its 52-case adversarial suite passes. Linux x86-64/AArch64 and
Windows native execution remain mandatory R2-F evidence; a macOS cross-check
does not substitute for those runtime rows.

The settled local checkpoint passed 1,253 `gwz-core` library tests with zero
failures and one ignored test; the 10 diff-render, 29 protocol, 7 publish, and
2 rename integration tests; 16 focused retained-provider/collision tests; the
six release-boundary and 52 adversarial source-boundary tests; 86 retained-
reader tests with the 24-tuple manifest ready; seven compatibility rules and
14 checker tests; 133 merge-document assertions; Rust 1.95 formatting; and
all-target/all-feature Clippy with warnings denied.

### R2-C0 implementation checkpoint — 2026-08-14

R2-C0 is complete locally and is frozen for its required independent
critical-interface review. Every recovery-critical durable record now uses
the bounded, canonical `CheckedDurablePathV1` wire shape; live retained paths
remain a separate process-local capability. The durable adapter rejects empty,
oversized, noncanonical, and mixed-filesystem-profile component sequences.
Generated bindings, the generated corpus, and independent semantic vectors are
current.

The pre-catalog transaction now returns only the boxed closed states
`MissingGitPrivateParent` and `Ready`. Their live-only permits have disjoint
authority: the missing-parent permit can authorize only the fixed Git private-
parent creation and carries no ready-catalog material; the ready permit retains
the exact target lease, platform root, support and path profiles, durable and
invocation identities, rename domain, raw catalog-role observation, and three
distinct typed digests. The provisional generic callback, path-plus-lease
entry, and durable lease binding are absent.

`CatalogMutationLeaseV1` is bound to one revalidated workspace or actual Git
target. `CatalogLeaseSetV1` prepares every target under transient guards,
deduplicates and orders stable target keys, releases preparation guards before
the final phase, acquires the fixed final lock files in canonical order, and
releases them in reverse on contention or failure. Tests prove workspace-lock
compatibility, linked worktrees contending on their shared Git target, wrong-
kind and symlink rejection, lock and target substitution rejection,
reacquisition mismatch, canonical batch order, and release of earlier targets
when a later target contends. All C0 rejection paths leave the catalog roles
absent.

The source boundary now freezes the complete path-schema, catalog-lease, and
pre-catalog trees, exact lease reference sets, and the absence of the removed
provisional names. The settled local checkpoint passed:

- 1,265 `gwz-core` library tests, with zero failures and one ignored test;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- 97 checked-artifact interface tests, 15 production pre-catalog provider
  tests, and 11 catalog-lease tests;
- the six-case release-boundary suite and the 56-case adversarial source-
  boundary suite;
- protocol regeneration/currentness, Rust 1.95 formatting, `git diff --check`,
  the checked-artifact source checker, and all-target/all-feature Clippy with
  warnings denied.

No catalog directory, record, scratch object, staging object, or final object
can be created by C0. R2-C1 remains a pure aggregate grammar/classifier package;
R2-C2 is still the first package authorized to mutate the catalog namespace.

### R2-C0 independent review correction — 2026-08-14

The first immutable C0 checkpoint was not accepted. The independent code and
state/lease reviews both returned **NO-GO**: the lease could not bind or
revalidate a provider root after acquisition, an actual/common Git target was
still caller-selected, the batch and alias scans were unbounded, and one
canonical location observed under different identities could reach phase-one
runtime-slot preparation. The controlling reports are
`GwzM5-8R2C0Interface-ReviewCode.md` and
`GwzM5-8R2C0Interface-ReviewState.md`.

The correction replaces the path accessor with a consumed, lease-owned target
witness. The sole production provider route derives its root from that witness,
exactly compares the retained live and durable target/repository facts, and
revalidates the target plus named final-lock inode at permit issuance and every
future physical edge. Git requests now derive the common Git directory from a
repository/worktree purpose, so linked worktrees cannot select incompatible
actual/common targets. Permit constructors accept only a lease-bound provider
observation; a path-plus-lease or independently retained-root constructor is
not representable.

The correction also adds a sealed nonempty target batch with a 4,096-target
ceiling, maximum-plus-one iterator consumption, fallible allocation, and a
canonical-location ambiguity pass before preparation. Equivalent-alias proof
uses the literal lossless parent budgets and skips distinct-alias enumeration
on sensitive parents. Focused tests cover post-return workspace/Git target and
named-slot replacement, cross-target pairing, linked-worktree convergence,
both duplicate-identity orders, maximum/maximum-plus-one and infinite batches,
allocation failure, parent capacity, and Unix non-Unicode names. C0 remains
pending until the corrected settled tree passes every full gate and both
independent re-reviews report no open P0/P1/P2 finding; C1 must not start before
that acceptance.

The corrected pre-review tree passed:

- 1,279 `gwz-core` library tests, with zero failures and one ignored test;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- 97 checked-artifact interface tests, 19 production pre-catalog provider
  tests, and 21 catalog-lease tests;
- the six-case release-boundary suite and the 57-case adversarial source-
  boundary suite;
- protocol regeneration/currentness, the 133-assertion merge-document gate,
  17 document/compatibility checker tests, Rust 1.95 formatting,
  `git diff --check`, the checked-artifact source checker, and all-target/all-
  feature Clippy with warnings denied.

This evidence freezes the correction for re-review; it does not itself change
the pending verdict to accepted.

### R2-C0 second independent correction — 2026-08-14

The first correction was also not accepted. The second code re-review reported
one P2 and the second state/lease re-review reported one P1 plus the same P2;
their controlling reports are `GwzM5-8R2C0Interface-ReviewCode-2.md` and
`GwzM5-8R2C0Interface-ReviewState-2.md`. Rust's stable slice sort allocated
hidden scratch outside the typed fallible reserve path. More importantly,
deriving a common Git target discarded the original repository/worktree
membership, and deduplicating linked worktrees could discard all evidence for
one selected request.

The second correction uses exactly two allocation-free in-place unstable
sorts, with source and interface guards forbidding a return to allocating
stable sort. A maximum 4,096-entry batch now exercises both order passes and
deduplicates successfully. Every Git request retains its canonical request,
optional worktree, actual Git directory, and common Git directory with the
complete durable/live/mode/domain association. Requests sharing a common
target share its one lock while all membership witnesses survive grouping and
are revalidated before/after preparation, after final acquisition, at
`begin_preflight`, and at every future permit edge.

Focused negative tests cover membership substitution after initial retention
in both input orders, after phase-one preparation, after successful return for
one request, and after successful common-target deduplication in both orders.
All reject before catalog mutation while the common target and named lock
remain intact. The second corrected pre-review tree passed:

- 1,286 `gwz-core` library tests, with zero failures and one ignored test;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- 98 checked-artifact interface tests, 19 production pre-catalog provider
  tests, and 27 catalog-lease tests;
- the six-case release-boundary suite and the 57-case adversarial source-
  boundary suite;
- protocol regeneration/currentness, the 133-assertion merge-document gate,
  17 document/compatibility checker tests, Rust 1.95 formatting,
  `git diff --check`, the checked-artifact source checker, and all-target/all-
  feature Clippy with warnings denied.

The immutable second-correction checkpoint is workspace root
`093857634e931696a2178f12970a7743ebe20e5d`, `gwz-core`
`1c5e42db9851c96b801e5aa7e6996207ecd6c514`, and `gwz-cli`
`3cca145c0b32410f250f640730ed7ca18f1da59f`. The independent third reviews
`GwzM5-8R2C0Interface-ReviewCode-3.md` and
`GwzM5-8R2C0Interface-ReviewState-3.md` both returned **GO** with zero P0, P1,
P2, or P3 findings. R2-C0 is accepted and R2-C1 is unblocked.

### R2-C1 implementation checkpoint — 2026-08-14

R2-C1 is implemented as a read-only aggregate grammar and classifier. The
dynamic scratch name has one exact ASCII spelling and three fixed 64-hex
bindings. Parent observation is a single lossless, bounded scan with literal
entry, native-name, encoded-name, and aggregate budgets. It rejects malformed,
duplicate, and case-equivalent reserved roles, including non-UTF-8 Unix names
and UTF-16 Windows names, before any catalog mutation is representable.

The lease-bound provider now issues three separate facts: a fresh observation
digest for current live collision state, a durable catalog-target digest for
the stable root and mutation-parent binding, and a historical collision digest
for the original attempt. Recovery obtains the historical digest only from an
exact scratch name, active record, or retired record and rejects conflicting
sources. Ready and missing-parent permits revalidate their complete observed
state; the missing-parent route remains disjoint and can authorize only the
future no-replace parent edge.

The pure classifier covers absent, scratch, active, staging, final, retired,
completed, ambiguous, wrong-owner, and drift outcomes with one closed next-edge
decision. C1 deliberately treats the interiors of present staging and final
directories as unowned physical facts: C2 must validate those interiors before
it can classify them as owned. No C1 production module contains a filesystem
writer, and R2-C2 remains the first package permitted to mutate the catalog
namespace.

The settled pre-review tree passed:

- 1,311 `gwz-core` library tests, with zero failures and one ignored test, in
  768.27 seconds;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- 108 checked-artifact interface tests, 35 pre-catalog provider tests, and 27
  catalog-lease tests;
- the six-case release-boundary suite and the 57-case adversarial source-
  boundary suite;
- protocol regeneration/currentness, the 133-assertion merge-document gate,
  17 document/compatibility checker tests, Rust 1.95 formatting,
  `git diff --check`, the checked-artifact source checker, and all-target/all-
  feature Clippy with warnings denied.

This evidence freezes C1 for independent settled-tree review. C1 is not
accepted, and C2 remains blocked, until both reviews report no P0, P1, or P2
finding.

### R2-C1 native-equivalence correction — 2026-08-14

The first state/filesystem review returned **GO** with zero findings, but the
first code/interface review returned **NO-GO** with one P2. Its exact APFS
counterexample used a non-ASCII long-s spelling that the filesystem resolved
to the canonical ASCII scratch object. C1 charged the entry and then discarded
it before equivalence classification, incorrectly exposing an absent scratch
state. The controlling reports are
`GwzM5-8R2C1AggregateClassifier-ReviewState.md` and
`GwzM5-8R2C1AggregateClassifier-ReviewCode.md`.

The correction makes native-name handling conservative and uniform. After
lossless budget charging, any non-ASCII entry in an admitted case-fold parent
is namespace ambiguity. A proven case-sensitive parent continues to admit a
bounded non-ASCII or non-Unicode entry as an unrelated name. The same rule now
guards the aggregate catalog scan, retained private-parent alias scan, and
catalog-lease final-slot alias scan. The catalog amendment pins this behavior
until a separately reviewed provider can prove complete platform-native
equivalence.

Regression tests cover synthetic Unix and Windows native names, long-s and
Kelvin substitutions, the physical macOS fixed-role and complete dynamic-
scratch aliases, retained-parent rejection before callback, final-lock alias
rejection, and preservation of sensitive-parent non-Unicode behavior. The
corrected pre-review tree passed:

- 1,315 `gwz-core` library tests, with zero failures and one ignored test, in
  776.27 seconds;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- 109 checked-artifact interface tests, 37 pre-catalog provider tests, and 28
  catalog-lease tests;
- the six-case release-boundary suite and the 57-case adversarial source-
  boundary suite; and
- protocol regeneration/currentness, Rust 1.95 formatting,
  `git diff --check`, the checked-artifact source checker, and all-target/all-
  feature Clippy with warnings denied.

C1 remains unaccepted and C2 remains blocked until both independent focused
re-reviews report no P0, P1, or P2 finding on the corrected immutable tuple.

### R2-C1 acceptance — 2026-08-14

The corrected immutable checkpoint is workspace root
`841367f9bc64b632a3f7b9a837a15ab886f2efc2`, `gwz-core`
`f416e6d7bf8b582087b3dd1512dacc91db066369`, and `gwz-cli`
`3cca145c0b32410f250f640730ed7ca18f1da59f`. The focused re-reviews
`GwzM5-8R2C1AggregateClassifier-ReviewCode-2.md` and
`GwzM5-8R2C1AggregateClassifier-ReviewState-2.md` both returned **GO** with
zero P0, P1, or P2 findings. The state review reported no P3; the code review's
sole P3 was a stale provenance sentence in the catalog amendment, corrected
without changing the reviewed semantic rule or implementation.

R2-C1 is accepted. R2-C2 is unblocked as the first package authorized to
mutate the catalog namespace; it must consume only the accepted C1 aggregate
decision and must not weaken the conservative native-equivalence rule.

### R2-C2 implementation and first-review remediation — 2026-08-15

The first immutable C2 interface checkpoint was workspace root
`04d4fc0bc9e203db83d15f13e73e3a7a26a72556`, `gwz-core`
`1ef95b4ade6fa402cf1f2101a84af493af6a7beb`, and `gwz-cli`
`3cca145c0b32410f250f640730ed7ca18f1da59f`. It was not accepted. The
independent code review reported two P2 findings and one P3; the independent
state/filesystem review reported two P1 and two P2 findings. Their controlling
reports are `GwzM5-8R2C2OwnerInterface-ReviewCode.md` and
`GwzM5-8R2C2OwnerInterface-ReviewState.md`.

The correction makes the owner edge an unforgeable value constructed only by
`CatalogOwnerV1`; all permit execution consumes that value, and only the owner
generates a fresh scratch token. Exact reference-set and whole-tree gates now
reject an unrelated checked-artifact sibling that attempts to call a physical
catalog edge. Existing scratch repair opens without truncation, compares the
fresh identity and expected-prefix bytes, verifies the same named object, and
only then truncates through that handle. Active publication similarly opens,
verifies, and flushes the exact scratch before no-replace publication and
destination reobservation.

The owner now implements the complete classified path: missing Git parent,
scratch, active, staging creation and every ordered staging prefix, final
publication, active retirement, and exact completion. The ten-entry interior
reader charges native names before classification, rejects aliases, extras,
wrong kinds, and unowned staging/final directories, and derives both canonical
infrastructure records from the retained physical identities. The anchor is
published B-to-A, exercised A-to-B-to-A, and reobserved at each namespace
edge. Every ordinary owner edge returns only the target witness and repeats
full lease-bound preflight before further mutation.

Completion retains the final directory and every authority-bearing interior
file/directory handle under the still-live target lease. Revalidation proves
both each retained handle and its current named association, including
byte-identical inode replacement, before the opaque catalog can be consumed by
C3. Deterministic tests cover scratch, staging, active, final, and interior
substitution; staging/final content drift; every whole-edge restart; zero and
partial next-file recovery; and all three intermediate anchor-move states.

The platform contract no longer claims that a successful Windows directory
`sync_parent` proves an empty-directory namespace edge durable. Empty Git-parent,
staging, and retired-action directories are authority-free, restart-idempotent
prefixes. Every authority-carrying Windows regular file uses write-through plus
handle flush, and every publication uses the existing write-through rename
primitive. Native Windows and Linux power-loss evidence remains mandatory in
R2-F; this macOS implementation checkpoint does not substitute for it.

The corrected settled pre-review tree passed:

- 1,338 `gwz-core` library tests with zero failures and one ignored test in
  792.29 seconds;
- the four integration binaries: 10 diff-render, 29 protocol, 7 publish, and
  2 rename tests;
- the 58-case checked-artifact adversarial source-boundary suite;
- protocol regeneration/currentness, the 133-assertion merge-document gate,
  and 23 document/compatibility/release tests; and
- Rust 1.95 formatting, `git diff --check`, the direct checked-artifact source
  checker, and all-target/all-feature Clippy with warnings denied.

This evidence freezes the corrected tree for review; it does not accept C2.
Both independent settled-tree re-reviews must report no P0, P1, or P2 finding.

### R2-C2 second-review publication correction — 2026-08-15

`GwzM5-8R2C2OwnerInterface-ReviewCode-2.md` found that the implementation
dropped a verified source handle before each pathname rename and then claimed
read-only rejection even for a same-user replacement injected in that
interval. The catalog amendment now resolves the underlying platform contract
rather than adding another check-before-rename patch.

All scratch, final-directory, retirement, and anchor publication must route
through one sealed source-associated primitive. It opens no-follow and checks
the expected source identity immediately before the edge and keeps that source
capability live through publication. Windows must rename that exact checked
handle. Linux and macOS retain the checked handle and use the relative
no-replace path operation under the amendment's explicit cooperating-GWZ and
same-user-private-namespace boundary; no portable atomic conditional-by-object
rename is claimed there. Pre-capability substitutions remain typed read-only
ambiguity on every platform. The structural gate must reject direct raw rename
calls from the catalog provider outside the primitive.

The re-review's P3 also remains binding: the 21 fault-key inventory becomes a
real parameterized interruption/re-entry matrix. Each key must trigger its
physical point, drop live process/lease state, reacquire the full lease, and
prove convergence to the retained catalog or a typed read-only ambiguity.
String labels and enum equality remain only coverage guards.

## 9. Exit gate

This remediation is complete only when:

1. no pre-catalog entry accepts caller-provided collision/domain fact lists;
2. collision permits bind and freshly revalidate the complete observed
   snapshot;
3. managed policy rejects forbidden retained prefixes and every overlapping
   physical suffix before admission;
4. pre-record actions use only the reproducible parent-only owner;
5. all record-owned checked actions derive identity from exact durable source
   bytes; no caller can provide the source digest or a second opaque step hash;
6. owner/action and schedule literal vectors pass independently;
7. ordinary command call graphs remain outside checked-provider gating;
8. selected merge checked paths have no raw successful bypass, including an
   unlisted writer alias, function pointer whose source-level name is otherwise
   allowed, crate-local wrapper, alternative backend observer override,
   unsealed production runtime, or loader indirection outside the exact v1 and
   artifact-adapter byte manifests; the compiler-root chain must prove the
   protected v1 tree is selected rather than merely resident;
9. archive authority rejects any syntactically valid terminal-labelled record
   that the production archived-record validator rejects;
10. PR/push, release CI, and local release use the same source and compiler
    gates against the exact tree being checked; local tag creation/push cannot
    skip the compiler gate, every new tag target is re-gated by exact SHA, and
    the declared CI Python can execute the complete boundary suite;
11. all focused/full/static checks pass on a settled tree; and
12. both independent re-reviews report no open P0/P1/P2 defect.
