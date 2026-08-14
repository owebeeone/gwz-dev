# R2-C first-catalog bootstrap amendment

Date: 2026-08-14

Status: **accepted for physical R2-C implementation after independent GO/GO
review**.

The controlling reviews are
`GwzM5-8R2CCatalogBootstrapAmendment-ReviewCode.md` and
`GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md`. Both report zero
P0/P1/P2/P3 findings against semantic revision SHA-256
`bb59775646954b6e1ec8981794071397ca8ee5f3da4d2f9dd4d9592aceb92094`.
The later native-equivalence correction in section 3 is controlled by
`GwzM5-8R2C1AggregateClassifier-ReviewCode-2.md` and
`GwzM5-8R2C1AggregateClassifier-ReviewState-2.md`; both report no P0, P1, or
P2 finding. The reviewed post-correction amendment bytes had SHA-256
`5168f728e86ecb7d06b79ea4033dc80e5dacbf9e519a06f782a2808d63c1992f`.
Only this expanded review-status provenance preamble was added afterward.

This amendment controls the first-catalog portions and the cross-cutting
durable-path schema portions of
`GwzM5-8R4bR1InterfaceCheckpoint.md`,
`GwzM5-8R4bR2ConsumerCheckpoint.md`, and
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` where they differ. It changes only
private, test-gated checked-artifact infrastructure. It does not change the
public GWZ protocol, enable merge-v1, or add a merge lifecycle phase.

## 1. Why the correction is required

The frozen interface cannot implement its claimed restart behavior as written.
`CatalogBootstrapRecordV1` binds both a randomly issued ownership token and the
pre-catalog collision snapshot. The original fixed scratch file may become
visible with zero or partial bytes before either value is recoverable from its
contents. After restart:

- a new random token produces different expected bytes and a different record
  ID;
- the live runtime lease is deliberately non-durable and cannot recover the
  persisted historical lease binding; and
- raw namespace observation now includes the scratch entry created by the
  interrupted attempt, so it cannot reproduce the original collision digest.

The current record also persists invocation-only identity/path facts. Those
facts are valid for retained-handle substitution checks inside one process but
are not a power-loss identity contract. A partial scratch cannot reconstruct
their historical bytes, and a complete record must not require a reopened
durable object to reproduce an invocation-local representation.

Calling such bytes `PartialExpectedPrefix` without first reconstructing the
historical expected record either rejects a valid interrupted bootstrap or
permits an unbound foreign file to be overwritten. Neither result satisfies
the accepted zero-guessing recovery contract.

There is a second ordering gap for Git-directory artifacts. Their fixed `gwz`
private parent may be absent, but a lifetime-bound permit cannot safely mutate
children through a parent handle that did not exist when the permit was
issued.

R2-C stops at this interface boundary rather than implementing either gap as
caller convention or a special-case patch.

## 2. Controlling decisions

The correction has seven parts:

1. the exceptional pre-owner scratch name carries the restart-stable target
   digest, historical collision digest, and random bootstrap ownership token;
2. the live lease is target-bound and never serialized or accepted beside an
   independent caller path;
3. the provider separates historical pre-scratch evidence from the current
   collision/freshness proof required before every mutation;
4. creation of a missing Git-directory `gwz` parent is authorized by a
   distinct live, single-purpose pre-parent permit and forces full pre-catalog
   re-entry before any child mutation or durable target digest exists;
5. live freshness identity and rename-domain proof are separated from
   restart-stable durable record identity rather than serializing one
   process's handle facts; and
6. the exact active record reserves the fixed staging name, making an empty or
   expected-prefix staging directory a recoverable active-owned state; and
7. the catalog target lease has fixed workspace/Git lock slots and a canonical
   two-phase multi-target acquisition protocol.

The active record remains the durable owner of staging and final
infrastructure. Action admission, managed bootstrap, checked leaves, and
ordinary artifacts do not gain random scratch names.

## 3. Exceptional scratch grammar

The fixed catalog roles remain:

```text
checked-artifacts-catalog-bootstrap-v1.active
checked-artifacts-catalog-bootstrap-v1.staging
checked-artifacts
```

The pre-active scratch role becomes exactly:

```text
checked-artifacts-catalog-bootstrap-v1.scratch.<target-hex>.<collision-hex>.<token-hex>
```

All three suffixes are exactly 64 lowercase ASCII hexadecimal characters
encoding 32 bytes. No alternate spelling, uppercase form, short form, extra suffix, or
additional candidate is valid. The complete entry remains below the
255-byte component bound at exactly 241 ASCII bytes. A platform that cannot
prove a component limit of at least 241 rejects before private mutation.

The physical namespace owner performs bounded full-parent enumeration before
classification without allocating per unrelated entry. The literal bounds are
4,096 total parent entries, 255 native name units per entry, 510 encoded name
bytes per entry, and 2,088,960 encoded name bytes in aggregate. Unix charges
raw name bytes; Windows charges the exact UTF-16LE byte length. A non-Unicode
Unix name remains lossless bytes rather than a skipped entry. Exceeding any
bound is typed capacity ambiguity before mutation.

One observation performs at most four outer-role metadata/open probes
(scratch, active, staging, final) and one bounded retired-record read. Owned
staging/final validation then uses only the ten fixed infrastructure slots and
their existing record-byte budgets. Unrelated parent entries are name-charged
but never opened or retained.

Zero candidates means no scratch. Exactly one canonical candidate supplies
the durable target digest, historical collision digest, and ownership token
needed to reconstruct the expected record even when the file is empty. Multiple,
malformed-recognized, equivalent-name, wrong-kind, or over-budget candidates
are ambiguity and permit no mutation.

The admitted case-fold profiles do not treat ASCII-only folding as a complete
native lookup-equivalence proof. After charging a bounded native name, a
case-fold parent therefore rejects every non-ASCII entry as namespace
ambiguity unless a later reviewed provider supplies complete native
equivalence semantics. A proven case-sensitive parent continues to retain a
bounded non-ASCII or non-Unicode entry as an unrelated charged name. This rule
applies consistently to the catalog parent, retained private-parent aliases,
and catalog-lease final-slot aliases.

The workspace Git collision domain reserves the entire canonical scratch
family as well as the fixed active, staging, and final roles. Index and tracked
worktree exact, ancestor, descendant, stage, flag, gitlink, kind, and
platform-equivalent collisions still reject before private mutation.

This random name is the sole first-owner exception to the normal fixed scratch
rule. It exists because no earlier durable catalog owner is available to carry
the random token. Once active publication succeeds, the scratch entry is gone
and every later edge uses fixed names plus the active record's token.

The canonical scratch entry is the first durable ownership declaration inside
GWZ's reserved private namespace. A fresh creator may publish it only after an
absence/collision proof and only with no-replace creation. Recovery treats a
canonical name plus expected-prefix bytes as protocol state; all other forms
are foreign ambiguity. This is a namespace ownership boundary, not a claim of
authentication against a same-user process deliberately forging GWZ's private
on-disk protocol.

## 4. The live lease is target-bound and non-durable

An advisory lease is a live capability, not durable record content. The
catalog-bootstrap schema and scratch name contain no lease bytes. A private
live epoch may participate in `FreshObservationDigestV1`, but it is discarded
with the process and never crosses a durable codec.

The fixed final lock slots are:

```text
workspace target:     .gwz/locks/workspace-mutator.lock
Git-directory target: <retained-target-git-dir>/gwz-catalog-mutator-v1.lock
```

The Git-target slot is capability-neutral runtime grammar, not a catalog role
or checked authority record. Its empty regular file may remain unlocked after
process exit. The lease owner opens/creates it no-follow through the retained
target Git-directory handle, rejects equivalent aliases, symlinks/reparse
points and wrong kinds, obtains an exclusive nonblocking advisory lock, and
then revalidates the target directory identity plus the named lock entry and
opened file identity. Contention returns no lease and performs no catalog
mutation. The file lock and retained target handles live until the complete
checked command releases its lease set.

`CatalogMutationLeaseV1` retains and binds the exact workspace root or common
Git-directory target on which its advisory lock was acquired. A Git-target
request names a repository or worktree, and the sealed lease owner derives its
common Git directory; callers cannot choose between linked-worktree actual and
common Git directories. Each request also retains a separate membership
witness for its canonical request directory, optional worktree, actual Git
directory, and common Git directory, including their durable identity, live
invocation identity, rename domain, and lookup mode. Requests that share one
common target share one final lock but retain every contributing membership
witness; canonical-target deduplication never discards one. `CatalogOwnerV1`
consumes the lease through a sealed `begin_preflight` transition, revalidates
every membership witness, the retained target, and the still-named final lock
inode, and derives the target path and retained provider handles from that
opaque witness. It never accepts an independent path or provider root beside a
valid lease:

```text
recover_or_create(catalog_target_lease)
```

The ordinary `WorkspaceMutatorLock` compatibility wrapper continues to acquire
only its existing workspace final lock. A one-target checked workspace path may
borrow that exact held lease. It cannot fabricate a Git target. Any Git target
or checked multi-target command uses `CatalogLeaseSetV1`; R2-C never pairs a
target from one lease with another lease.

Checked multi-target acquisition is fixed now rather than deferred to callers.
The coordinator derives one complete, nonempty target batch read-only. The
literal maximum is 4,096 targets for one command, matching the global bounded-
observation ceiling while remaining far above a practical GWZ workspace. The
batch consumes at most maximum-plus-one iterator items, uses fallible reserved
allocation throughout, and rejects overflow or allocation failure before any
catalog mutation. The owner first groups by canonical target location: exact
full bindings deduplicate, while one location observed with different target
identity or repository membership rejects before phase-one preparation. It
then sorts the distinct targets by the canonical bytes of `(support_profile,
target_durable_identity, root_kind)`. Both canonical-location grouping and
final target ordering use in-place allocation-free unstable sorts with total
comparators; allocating stable sort is forbidden by the structural gate.

Equivalent-final-lock alias proof is finite and lossless. A sensitive parent
requires no distinct-alias enumeration. An ASCII-case-fold parent charges each
native name against the same literal 4,096-entry, 255-native-unit-per-name,
510-encoded-byte-per-name, and 2,088,960-aggregate-byte limits used by the
bounded parent grammar. Capacity ambiguity rejects before final acquisition;
non-Unicode Unix names and native Windows names are never skipped or converted
lossily.

Acquisition has two non-interleaved phases. Preparation visits targets in that
order and, while holding no final target lease, uses each target's transient
runtime bootstrap guard to converge only the fixed runtime directories/final
lock file or Git-target lock file; it releases that guard before visiting the
next target. Final acquisition then opens, locks, and revalidates every final
slot in canonical order and releases all locks in reverse order on any failure.
No transient bootstrap guard may be acquired or re-entered after the first
final target lock is held. No catalog/private mutation occurs until the whole
set is locked and every target identity/membership binding is revalidated.
The lease-owned witness repeats target, repository, runtime-directory, and
named-lock revalidation before permit issuance and immediately before every
physical catalog edge; the ready and missing-parent permit constructors accept
only the resulting bound observation.

Contention, duplicate path with different identity, membership drift, or
reacquisition mismatch releases the whole set without catalog mutation. A
failure detected before preparation is fully read-only. A race detected after
phase-one preparation may leave only the already-authorized fixed, capability-
neutral runtime lock grammar; it cannot reach a catalog/private role. A
checked multi-target path may not acquire `WorkspaceMutatorLock` first and
append out-of-order leases. `CatalogLeaseSetV1` alone exposes target-specific
borrowed `CatalogMutationLeaseV1` values and retains every final lock until
command completion.

No public constructor exposes a live epoch, target substitution, token, or
production synthetic lease witness. Contention or target-association failure
is read-only. The owner requests randomness only after it has a ready catalog
permit; random-source failure therefore rejects before scratch mutation but
does not make a separately completed private-parent edge invalid.

## 5. Live freshness and historical collision evidence

The platform provider continues to observe and retain every physical fact
specified by R2-B: repository/common-directory identity, index identity and
bytes, lossless index rows, tracked-worktree kinds, private-parent identity,
every fixed role, and every recognized scratch-family candidate.

The ready catalog permit carries three non-interchangeable bindings:

- `FreshObservationDigestV1` includes invocation identities, current rename
  domains, enumeration budget totals, and every raw reserved-role/equivalent-
  alias fact. It exists only inside the retained transaction and must match
  immediate reobservation before mutation. Ordinary sibling names are charged
  but are not retained as catalog facts.
- `DurableCatalogTargetDigestV1` contains root kind, support profile, durable
  root/repository/common-directory/actual-mutation-parent identities, fixed
  durable parent path, and lookup modes. It excludes index/worktree state,
  invocation identity, and rename domain. Recovery always recomputes this
  digest and requires exact equality.
- `HistoricalCollisionDigestV1` frames the durable target digest plus the
  historical index durable identity and bytes, complete index/worktree rows,
  collision-domain version, fixed path spellings, and initially absent catalog
  roles. Invocation identities and rename domains are excluded. The scratch
  name and catalog record both store this digest.

The three digests have separate domain labels and private newtypes. None can be
supplied by a caller or converted into another. The historical value is
evidence of the collision-free pre-scratch observation, not a permanent pin on
the workspace index. After any bootstrap role is durable, recovery obtains the
historical value from the scratch name or exact active/retired record and also
performs a new complete collision scan. An unrelated current index/worktree
change is allowed; a current overlap with the reserved namespace rejects.

The internal catalog-bootstrap record is corrected before A1 so it contains
only restart-stable identity/path bindings. Invocation identity and rename
domain remain on the permit and retained objects but are removed from the
durable record. Its canonical parent-path field is replaced by a durable path
descriptor containing fixed ASCII spelling, canonical spelling, parent lookup
mode, and durable parent identity, never a handle-local identity or live
rename-domain proof. Generated internal bindings and shape vectors change
together; no released record is migrated.

This is a cross-cutting internal-record correction, not a catalog-only alias.
The current `CheckedCanonicalPathIdentityV1` includes invocation identity and
rename domain but is referenced by authority, barrier, managed-bootstrap, and
catalog-bootstrap records. It is therefore replaced in every durable record by
`CheckedDurablePathV1`. `CanonicalPathIdentityV1` remains the live retained-
transaction type and is never serialized. R2-D may not build managed recovery
on the old mixed-purpose shape.

The exact affected durable fields are `CheckedAuthorityV1.artifact_root`,
`CheckedCatalogBootstrapV1.retained_parent_path`,
`CheckedBarrierIntentV1.target_path_profile`,
`CheckedManagedBootstrapComponentV1.installed_path`, and
`CheckedManagedParentBootstrapIntentV1.retained_parent_path`. No second path
encoding remains reachable from a durable-record root.

The corrected internal schema is exact:

```text
CheckedDurablePathComponentV1
  1 original_ascii
  2 parent_mode
  3 canonical_ascii
  4 parent_durable_identity

CheckedDurablePathV1
  1 components

CheckedCatalogBootstrapV1
  1  root_kind
  2  support_profile
  3  durable_target_digest
  4  historical_collision_digest
  5  retained_parent_identity
  6  retained_parent_path: CheckedDurablePathV1
  7  staging_name
  8  final_name
  9  catalog_anchor_a_name
  10 catalog_anchor_b_name
  11 record_id
  12 bootstrap_ownership_token
```

The schema has no compatibility aliases for the unactivated shape. Decoder,
encoder, semantic corpus, shape coverage, digest material, and exact record-ID
vectors change atomically.

`CheckedDurablePathV1` has one semantic envelope everywhere it is used. It has
at least one component and a canonical encoding of at most 4 KiB. Each original
component is normal ASCII of 1..=255 bytes, with no NUL, separator, `.` or
`..`; mode is one of the two closed values; canonical bytes equal the original
under sensitive mode and ASCII lowercase under case-fold mode; and each parent
durable-identity variant matches the enclosing retained-parent/profile chain.
Decode validates every rule and reproduces byte-identical canonical encoding.
No durable-record adapter may weaken or duplicate this envelope.

The record's `retained_parent_identity` is the durable identity of the actual
directory through which scratch, active, staging, and final entries are
mutated: workspace `.gwz` or Git-directory `gwz`. It is not the surrounding
workspace or actual Git-directory root. The durable parent-path descriptor
binds that mutation parent back to the retained root. The permit retains both
objects and revalidates their live invocation identities and current rename
domain immediately before each owner-controlled edge.

On a fresh attempt every catalog role is physically absent. On recovery, the
aggregate owner proves each present role belongs to the one expected bootstrap
attempt while independently proving the current collision domain:

- scratch: the canonical name supplies target digest/historical digest/token,
  the current stable target digest matches, and its bytes are zero-length, an
  expected prefix, or the exact reconstructed record;
- active: the bounded canonical record decodes, its target digest matches the
  current stable target facts, and all stable permit/path bindings match;
- staging: the exact active record durably reserves the fixed staging name.
  Missing, an empty no-follow directory, or the exact expected marker/
  infrastructure prefix is active-owned and repairable. Extra children,
  wrong-kind, equivalent-name, or content outside that prefix is ambiguity;
- final: the active record, token, retained parent/path, marker, object
  identities, and infrastructure record classify the directory; and
- retired: the exact active record is present at the fixed catalog retirement
  slot and the final infrastructure remains exact.

Any reserved physical fact not consumed by one listed proof is ambiguity.
Ordinary `.gwz/locks`, `.gwz/merge`, `.gwz/stash`, and other names outside the
reserved catalog grammar are charged to the parent-enumeration budget but are
not catalog roles or historical-baseline fields. A changed durable root/parent,
lookup mode, alias, reserved foreign child, or unmatched role rejects. A
changed live invocation identity or rename domain rejects that invocation;
retry may re-establish new live facts for the same durable objects.

The owner performs one aggregate observation, derives at most one expected
record, proves current collision freedom, chooses one recovery edge, and
reobserves after every mutation. It never builds an expected record from a new
historical digest or token when durable recovery evidence exists. Exact
retired+final completion reopens under current collision/freshness proof; it
does not require today's unrelated index bytes to equal the historical digest.

## 6. Missing Git-directory private parent

Workspace catalog bootstrap requires the retained `.gwz` parent already
created by runtime bootstrap. For a Git-directory catalog, an absent `gwz`
parent has one allowed result:

```text
CreatePrivateParentAndRetry
```

This result is not represented by the normal catalog permit. The provider's
closed preflight result is exactly:

```text
CatalogPreflightV1
  MissingGitPrivateParent(MissingCatalogParentPermitV1)
  Ready(CatalogPermitV1)
```

`MissingCatalogParentPermitV1` is live-only and single-purpose. It retains the
target-bound Git lease and actual-Git-directory handle plus an opaque
`MissingParentObservationDigestV1`. That digest binds the support profile,
durable and invocation identities of the repository/common/actual Git
directories, fixed `gwz` original and canonical spelling, lookup mode, current
rename domain, bounded parent-enumeration counters, and the raw proof that the
fixed child and every equivalent alias are absent. It contains no durable
catalog-target digest, historical collision digest, random token, expected
record, child handle, or catalog authority.

The missing-parent permit has no namespace operation other than the fixed
no-replace directory edge below. Its constructor is private to the retained
provider, and immediate reobservation must reproduce its complete live digest
before that edge. It cannot be converted to `CatalogPermitV1`; the only route
to `Ready` is complete pre-catalog re-entry after retaining the now-present
parent and computing all three ready-permit digests.

The owner creates only that fixed directory with no-replace semantics through
the retained actual-Git-directory handle, flushes the parent edge, and returns
without creating a catalog scratch, active record, staging directory, or final
catalog. The coordinator then repeats complete observe, collision validation,
retention, and immediate revalidation and obtains a permit that retains the
new parent.

An interrupted parent edge is restart-idempotent: absent is creatable, an
exact no-follow directory is reusable only as a parent, and wrong-kind,
equivalent-name, substituted, or unmatched/unowned child facts overlapping the
reserved catalog grammar reject. A no-replace race returns to full pre-catalog
classification without treating the observed entrant as owned. Exact
owner-classified recovery roles proceed through the aggregate table. Presence
of the container alone never authorizes adopting any child.

## 7. Private interface consequences

R2-C introduces cohesive owners rather than adding physical callbacks to the
pure protocol classifiers:

- `checked_artifact/catalog/bootstrap/` owns dynamic scratch parsing,
  aggregate discovery, one-edge execution, and retained catalog return;
- `checked_artifact/catalog/enumeration/` owns bounded global catalog grammar;
- `checked_artifact/admission/` owns reservation and handoff execution; and
- the retained provider exposes owner-private observations and namespace
  operations without returning raw handles or mutation capability to callers.

The lifetime-bound ready permit carries the opaque raw-role observation
required by bootstrap in addition to the three typed digests. Production code
cannot construct, normalize, or replace that observation. The catalog owner
performs its own retained-handle observation again before choosing and
executing an edge; the permit's raw rows are evidence of the immediately
preceding pre-catalog transaction, not authority to skip aggregate
reobservation. The missing-parent permit is the disjoint live shape in Section
6 and cannot enter this path.

The generic production `CatalogBootstrapV1` callback and caller-supplied
bootstrap value are removed. They cannot own the provider reobservations
required after their first mutation. One sealed `CatalogOwnerV1` owns the
retained provider, pre-catalog checks, aggregate classifier, namespace
mutations, and post-edge reobservation. Its only production entry is:

```text
recover_or_create(catalog_target_lease)
```

The owner executes at most one physical mutation per classified edge and
revalidates live freshness plus current collision freedom before the next edge.
The Git-parent edge consumes only `MissingCatalogParentPermitV1` and returns an
internal `RetryPreCatalog`; every other successful edge requires a ready permit
and loops through fresh aggregate observation. Only
`Complete(OpaqueRetainedCatalogV1)` escapes. A retry carries no retained
permit, catalog, handle, token, expected record, or authority.
Synthetic providers and direct pure-classifier inputs remain `#[cfg(test)]`.

The infrastructure-record wire fields remain unchanged. The internal canonical
bootstrap record, every internal durable-path reference, generated
checked-artifact bindings, and private scratch grammar change together before
A1 activation; there is no released v1 durable state to migrate and no public
GWZ protocol change.

## 8. TDD and exit gates

Interface tests precede physical implementation and must prove:

1. zero-byte and every bounded partial scratch prefix recover under a new live
   process by using the historical values encoded in the canonical name;
2. fresh random values are never substituted for an observed recovery attempt;
3. duplicate, malformed, equivalent, wrong-kind, and foreign scratch rows are
   read-only ambiguity;
4. the historical collision digest is recovered only from owner-classified
   bootstrap progress and is never recomputed from a later invocation, while
   the separately recovered target digest must match current durable target
   facts;
5. a new invocation representation for the same durable objects can recover,
   while a durable identity or lookup-mode change cannot, and every mutation
   separately re-establishes a same-invocation rename-domain proof;
6. unrelated index/worktree changes do not strand incomplete or complete
   catalog recovery, while any current reserved-path collision, parent drift,
   alias, or reserved-namespace change prevents mutation;
7. active-owned partial staging can be repaired, while an unowned staging or
   final directory cannot be adopted;
8. missing Git-directory parent creation performs no child mutation and the
   missing-parent permit cannot construct a ready permit, scratch name, token,
   or expected record, and the next pass uses a newly retained/revalidated
   parent to compute all three ready-permit digests;
9. every declared fault edge converges to the same complete retained catalog
   or a typed, read-only ambiguity; and
10. no production signature accepts raw lease bytes, token bytes, raw role
   rows, expected records, bootstrap callbacks, or synthetic
   observations; and
11. a valid lease for one target cannot be paired with another target; linked
   worktrees resolving to one retained Git target contend on the same final
   slot; and wrong-kind, equivalent-name, substituted-target, substituted-slot,
   and post-lock identity drift return no lease and perform no catalog
   mutation; and
12. checked multi-target preparation holds no final lease, final acquisition
   never re-enters a transient bootstrap guard, the complete deduplicated set
   is locked and revalidated in canonical order before the first catalog
   mutation, and contention or later-target failure releases the whole set in
   reverse order without mutation.

The schema gate additionally proves no durable message references
`CheckedCanonicalPathIdentityV1`, and no invocation identity or rename-domain
field is reachable from a durable-record root.

R2-D remains blocked until these tests, the real macOS implementation, the
structural boundary suite, formatting, and all-target/all-feature Clippy pass.
Linux and Windows native fault evidence remains an R2-F release gate.
