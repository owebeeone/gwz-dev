# R4b R2 consumer-checkpoint remediation plan

Date: 2026-08-14

Status: **the fourth settled re-reviews closed archive semantics and the named
writer aliases but found that a writer denylist and open trait callback cannot
prove a closed boundary. The fifth correction replaces that claim with a
complete positive source allowlist over the guarded modules, terminates the
production stash-observation call in a frozen read-only leaf, and makes the
local compiler gate non-skippable on the exact commit before tagging. No R2
production conversion may begin until this correction receives two independent
GO re-reviews on a committed settled tuple**.

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
- production `preservation_stashes` dispatch terminates in the guarded
  preservation-image leaf, whose selected path enumerates Git stash objects
  directly and uses only local decoding helpers; and
- source and compiler protections run in PR/push and release CI, while the
  local release script cannot skip them and reruns them on the exact
  version-bump commit before tag creation or push.

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

## 5. Provider-owned collision snapshot

The current caller-supplied index/worktree slices are removed. The closed owner
has two entry points:

```text
recover_or_create_workspace(workspace_root, lease_binding, bootstrap)
recover_or_create_git_directory(actual_git_directory, lease_binding, bootstrap)
```

The sealed platform provider correspondingly owns two observe/revalidate
pairs. Workspace observation opens the repository and index through retained
identities and gathers the complete lossless index plus tracked-worktree fact
set for the fixed workspace private domain. Git-directory observation proves
that the root is the actual retained Git directory and scans its fixed private
namespace without accepting an empty workspace fact list as a substitute.
Neither entry accepts a domain, index entry, worktree entry, or root-kind enum
from a caller.

The observation returns a 32-byte collision snapshot binding calculated from:

- root kind and fixed domain-version digest;
- retained repository/common-directory identity;
- retained index identity and exact index content digest for workspace roots;
- every complete, sorted lossless index fact;
- every complete, sorted tracked-worktree kind fact; and
- the platform-equivalence result for each private-domain comparison.

The existing 32-byte catalog-bootstrap collision field carries this complete
snapshot binding; no wire field is added. Its historical generated field name
is retained for wire compatibility, while Rust code and documentation call the
value `collision_snapshot_digest`.

Immediately before catalog bootstrap, revalidation reopens/reobserves the same
repository, index, tracked worktree, root, path components, modes, rename
domain, and equivalence facts and compares the complete binding. A changed
index or incomplete/unavailable observation rejects before bootstrap. The
revalidated permit borrows the retained transaction, so no caller can
interpose work at the mutation boundary.

Production-shaped tests prove that no callable API accepts hand-selected fact
lists. Real repositories cover exact, ancestor, descendant, equivalent-name,
stage 0..3, skip-worktree, gitlink, tracked-kind, and index-only collisions.
Changing the real index between observation and revalidation must prevent the
bootstrap callback and leave private and managed paths byte-identical.

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

Each checked entry or adapter module has a non-overridable compiler lint
boundary, but the lint's writer list is defense in depth rather than the
completeness proof. The proof is a positive, byte-exact allowlist of every
guarded source module. Any direct call, imported alias, same-name function
pointer, crate-local wrapper, nested helper, changed literal, or commented-out
guard changes the protected module digest and fails closed. The source checker
also pins the sole production `GitBackend` delegate for
`preservation_stashes` to `preservation_image`; that leaf directly enumerates
and decodes native stash objects without calling the open backend trait again,
the shared stash API, a shared repository opener, or an external OID parser.
The delegate macro source is itself in the positive allowlist, and the checker
rejects any production `GitBackend` implementation other than the single
frozen `Git2Backend` implementation. Test backends remain test-only.

PR/push CI and release CI run the source and compiler gates on their exact
checkout. The local release script has no compiler-skip option. It runs the
gates on its initial detached worktree and, after any version/lock commit,
resets that worktree to the exact new SHA and reruns the source checker, its
executable counterexamples, and all-target/all-feature Clippy before creating
or pushing the tag. Retrying an already-created tag likewise gates its exact
target before push.

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
7. Run focused tests, full `gwz-core` tests, Clippy, formatting, protocol and
   document checks; commit one exact workspace/core/CLI tuple.
8. Obtain two independent settled-tree re-reviews. Any P0/P1/P2 finding repeats
   this correction gate.

Only after GO/GO do the original R2-A through R2-F packages begin. R3-R6 and
R4b-G shared-router/cross-driver integration follow, then the two final
full-tree R4b reviews. M5b and A1 remain later.

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
   allowed, crate-local wrapper, or accepted production observer delegate;
9. archive authority rejects any syntactically valid terminal-labelled record
   that the production archived-record validator rejects;
10. PR/push, release CI, and local release use the same source and compiler
    gates against the exact tree being checked; local tag creation/push cannot
    skip the compiler gate and a version-bump commit is re-gated by exact SHA;
11. all focused/full/static checks pass on a settled tree; and
12. both independent re-reviews report no open P0/P1/P2 defect.
