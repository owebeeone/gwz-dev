# R4b remediation R2 consumer checkpoint — independent state review

Date: 2026-08-14

Reviewed exact workspace/core/CLI tuple:

- workspace: `e7e3c1be0286e8d1dddf1f11a7228c020b365e87`
- `gwz-core`: `c239df33ddf567d4eaca182ca8231ed575e95769`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Primary review target:
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint.md`.

Controlling architecture and accepted boundary:

- `dev-docs/GwzM5-8R4bP1P2-RemPlan-4.md`;
- `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md`; and
- the accepted R1 state/filesystem review series.

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 4. P3: 0.

The checkpoint preserves the important R1 boundaries: lookup precedes new
planning, overlapping managed parents are not independently admitted, the
provider and proof vocabulary remain private, reads and catalog enumeration
remain bounded, the fault-key set is independently pinned, and merge-v1 remains
quarantined until A1. R2-R6 are also correctly retained ahead of R4b-G.

The four findings below are nevertheless contract blockers. They affect the
durable identity used to find an interrupted action, the byte-level digest that
names it, the schedule reserved before mutation, and the compatibility boundary
between checked merge work and ordinary commands. Those decisions cannot be
left to the parallel R2-C/R2-D/R2-E implementations.

## Findings

### [P2-1] First-write consumers do not have a recoverable durable owner

**Where:** consumer checkpoint lines 58-62, 93-114, 197-216, and 265-269;
`gwz-core/src/workspace_ops/merge/start.rs:70-83`;
`gwz-core/src/workspace_ops/merge/start/record.rs:73-83`;
`gwz-core/src/workspace_ops/handle_stash/commands.rs:21-27,67`;
`gwz-core/src/workspace_ops/handle_stash/shared.rs:275-277`;
`gwz-core/src/workspace_ops/handle_commit.rs:122-140,254-267,282-305`; and
`gwz-cli/src/unique_suffix.rs:1-10`.

**Violated contract.** The checkpoint says that an action owner is durable, is
not a transient invocation ID, and lets a resident or retired action win over
replanning. The actual first-write callers cannot reconstruct the proposed
owner after a crash before their first consumer record is durable:

- merge start obtains `merge_id` from an operation-scoped generator and copies
  the CLI invocation's newly generated `operation_id` into the record, but both
  exist only in memory until the first merge-record write;
- stash push generates a time-derived `stash_id` when the request omits one,
  then first persists it in the bundle; and
- ordinary commit generates a random UUIDv7 marker ID before preparing the
  marker parent and before any marker record exists.

The R1 resident reservation persists `ActionDigestV1`,
`RequestOwnerBindingV1`, and the schedule, not the canonical owner tuple from
which a new invocation could recover these generated values
(`checked_artifact/protocol/admission.rs:19-40`). The checkpoint's lookup is
only for the newly derived owner/action binding. It defines neither a global
recovery sweep that finishes an unrelated resident action nor a rule by which
the new command adopts the old action's resulting merge record, stash bundle,
or marker identity.

**Failure scenario.** `gwz stash push` without an explicit ID crashes after
admission, during parent bootstrap, or after checked bundle publication. A
fresh invocation generates another `stash_id`, derives a different owner and
action digest, and cannot select the resident action by the promised lookup.
It may stop on the old action, consume a second catalog reservation, or create
a second bundle instead of resuming the first command. The same gap exists
before the first merge record, with the additional fact that the proposed
`durable_operation_id` is the transient per-invocation ID until that record has
actually been published.

**Required correction.** Freeze one complete pre-record recovery design before
R2 starts. Acceptable shapes include:

1. durably establish and recover a canonical command/owner seed before catalog
   admission;
2. use a reproducible owner for pre-record parent/artifact actions and bind the
   eventual generated ID only after its owning consumer record is durable; or
3. define a bounded global recovery sweep that can execute every valid resident
   pre-record action from its durable records, followed by an exact caller-
   adoption rule for the recovered merge/stash/marker result.

The correction must cover crashes before the first managed component, after
each admission handoff, after parent completion, and before/after the first
consumer bytes. Fresh-process tests must prove the same action name, owner
binding, result identity, catalog cardinality, and no duplicate user-visible
operation.

### [P2-2] Owner and action digest material is not byte-exact

**Where:** consumer checkpoint lines 57-103 and 111-114; R1 reservation fields
at `gwz-core/src/checked_artifact/protocol/admission.rs:19-40`; and the current
v1 journal binding inputs at
`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/binding.rs:9-50`.

**Violated contract.** `canonical owner tuple` and the `||` expression do not
define a canonical byte encoding. Variant tags, field count and order, string
length framing, path-component framing, enum discriminants, absence versus an
empty value, expected/goal variants, purpose-set ordering, and validation
limits are all unstated. `journal_binding` is likewise not tied to an exact
derivation from the already persisted record owner/action/phase and record
identity.

These hashes are not diagnostic conveniences. They determine catalog names,
owner equality, reservation recovery, and whether a retry resumes or conflicts.
Simple concatenation of variable-length IDs admits alternate tuples with the
same preimage unless every field is explicitly framed. Independent R2 lanes
could also choose different, individually reasonable enum encodings.

**Required correction.** Freeze a private, versioned canonical encoding for
each owner variant and the complete action request. It must specify domain and
variant tags, unsigned length framing, exact field order, component boundaries,
closed enum values, optional-value encoding, purpose-set order/bitset, and all
input bounds. Define `V1Journal.journal_binding` from the exact stored record
identity and pending owner/action/phase vocabulary rather than naming it as an
opaque future choice. Add independent literal preimage/digest vectors for every
owner variant, operation, root kind, expected/goal variant, purpose set, and
single-field substitution. If implementing that encoding requires a new
persisted protocol field, the checkpoint's own stop condition requires design
review before implementation.

### [P2-3] The coordinator's immutable reservation schedule is not frozen

**Where:** consumer checkpoint lines 85-109, 197-216, 239-254, 282-302, and
320-345; controlling architecture lines 200-207 and 260-277; and
`gwz-core/src/checked_artifact/protocol/schedule.rs:148-180`.

**Violated contract.** The facade correctly prevents consumers from supplying
a schedule, but it does not define what schedule the coordinator derives for
`Observe`, `Replace`, `Remove`, parent-only work, or combined parent-plus-leaf
work. The accepted R1 type still requires the caller to choose
`barrier_count`, managed bootstrap inputs, and the cleanup alias set. Those
choices determine durable ordinals, action capacity, schedule digest, and all
restart names.

The phrase "complete schedule" is insufficient at the R2 interface freeze.
For example, the checkpoint does not say whether every action reserves all 64
barriers conservatively or how many barriers and cleanup aliases each legal
source/goal path reserves. It also does not freeze the exact purpose-to-
`ManagedParentSpec` order used when first merge-record creation declares two
purposes. A crash while admission is `Preparing` can leave only the persisted
schedule/reservation hashes and partial reservation bytes; retry must reproduce
the exact expected bytes, including after an executable upgrade.

**Required correction.** Add one versioned coordinator schedule table or
algorithm covering every facade constructor and every legal source/goal state.
It must freeze:

- barrier count and ordinal use, including whether the conservative value is
  always 64;
- cleanup aliases and terminal retirement slots;
- purpose-to-managed-spec path, order, and missing-suffix rules;
- combined managed-parent plus leaf-action ordering; and
- behavior when a resident reservation exists but current parent missingness
  has changed.

Pin exact schedule encodings/digests and capacity totals for every consumer,
the two-purpose first-merge case, proof-only existing parents, partial
bootstrap recovery, and the `K=1,N=8` / `K=8,N=8` limits. R2-C cannot settle
before this contract; R2-D/E must consume it rather than independently infer
it.

### [P2-4] The checked-versus-ordinary consumer boundary is ambiguous and can regress compatibility

**Where:** consumer checkpoint lines 137-149, 260-278, and 382-390;
controlling architecture lines 678-685; current ordinary boundary writer at
`gwz-core/src/workspace_ops/sync_workspace_boundary.rs:20-42,74-85`; its callers
in `handle_branch.rs`, `handle_commit.rs`, `handle_materialize.rs`,
`pull_head_member_preflight.rs`, and repository lifecycle/create/init paths;
and the separate checked merge preservation path at
`gwz-core/src/git/gitbackend/preservation_root/files.rs:29-92`.

**Violated contract.** The accepted architecture states that non-checked
ordinary mutators stop at the capability-neutral workspace lease and are never
gated by the ext4 checked provider. The new table nevertheless labels ordinary
stash bundles, ordinary commit markers, workspace marker/lock mutation, and
`.git/info/exclude` as checked conversions without defining which shared
callers remain documented ordinary writers.

This matters immediately for `.git/info/exclude` and the workspace lock: the
same ordinary boundary helper is called by branch, materialize, commit, pull,
repo lifecycle, create, and init, while merge preservation already has a
separate checked acquisition path. Converting the shared helper from the table
would make ordinary commands fail on a filesystem outside the checked support
table. Leaving it raw everywhere would bypass the coordinator during merge
publication/recovery. Ordinary stash and commit-marker conversion raises the
same unresolved support-table question explicitly rather than through a shared
helper.

**Required correction.** Replace the broad rows with an exhaustive call-site
classification:

- name the merge/v1 publication, preservation, rollback, and recovery calls
  that must use a checked workspace/Git-directory action;
- name the ordinary lock, boundary, marker, and bundle paths that remain behind
  the capability-neutral lease and document their writer; or explicitly amend
  the accepted compatibility/support contract if any ordinary command is now
  intentionally checked-provider-gated; and
- require call-graph tests proving that no ordinary branch/materialize/stage/
  tag/create/init path reaches checked capability/catalog code, while every
  selected checked merge path does.

The consumer table must also assign one exact `CheckedActionOwner` variant and
request constructor to each row (ordinary stash versus merge preservation,
ordinary marker versus merge marker, v0 merge versus v1 journal). That mapping
is part of durable identity, not an R2-E implementation detail.

## Accepted parts of the checkpoint

Subject to the findings above, the following portions are suitable to retain:

- the purpose-specific facade and prohibition on raw provider, identity,
  observation, reservation, evidence, and namespace values;
- runtime bootstrap before the open-operation gate while preserving dry-run
  and validation ordering;
- complete capability/collision preflight before private or managed mutation;
- bounded global catalog enumeration and lookup-before-new-admission ordering;
- source-validated `MergeArchive` creation and the rule that overlapping
  missing suffixes are not independently requested;
- opaque retained-parent proofs and revalidation at the writer seam;
- bounded/fallible protocol and payload handling;
- independent expected fault keys versus executed production cases;
- native platform evidence rather than release-build substitution;
- merge-v1 decoder/writer/dispatch quarantine; and
- R2-A through R2-F dependency ordering followed by R3-R6 and only then R4b-G.

## Review method and gate

This was a static interface/state review of the exact committed tuple. I traced
the proposed owner and request vocabulary through the current merge-start,
merge store/archive, ordinary and merge stash, ordinary and merge marker,
workspace lock/manifest, and Git-directory boundary callers, and compared the
result with the accepted R1 durable records and RemPlan-4 ordering/support
contracts. The core and CLI SHAs are unchanged from the accepted R1 tuple, so I
did not claim a redundant full-suite execution as R2 evidence; R2 has no
production implementation yet.

R2 production conversion may begin only after all four P2 contracts are frozen
in the checkpoint and two independent reviews of the amended exact tuple report
no open P0/P1/P2 finding.
