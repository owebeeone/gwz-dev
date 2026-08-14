# R2-C1 aggregate-classifier filesystem/state review

Date: 2026-08-14

Review type: independent settled-tree filesystem, restart, and type-state
review of R2-C1.

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

Required corrections: **none**.

This verdict is limited to the immutable tuple below and to C1's deliberately
read-only aggregate grammar/classifier boundary. It does not accept a catalog
filesystem writer or claim C2 complete.

## Settled review tuple and scope

The review began on the requested clean, lock-matched tuple:

```text
workspace root 515d7f0eb4422a843bed1edbd2a71aae891435af
gwz-core       74747661f2f030a590e115af0daf392e008c06f6
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

The complete `gwz-core` change from
`1c5e42db9851c96b801e5aa7e6996207ecd6c514` was reviewed, together with the
root checkpoint change, `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`,
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`, the accepted catalog amendment,
and both accepted amendment reviews. The amendment reviewed here has SHA-256
`e45756a7187a1f17374bd98f89bd4d96a3364e8fb057113b0663bd36ec432a22`;
its semantic revision remains the accepted one identified in its provenance
preamble.

## Findings

None.

## Correctness evidence

### Complete bounded physical observation

`catalog/enumeration.rs` implements the frozen 4,096-entry,
255-native-unit, 510-encoded-byte, and 2,088,960-aggregate-byte limits. The
physical provider uses one parent iterator and charges every entry before
classification. It retains no per-entry representation for unrelated names;
only the at-most-four recognized outer roles are retained. Unix names are
charged as raw bytes and tested with non-UTF-8 input. Windows names are charged
as UTF-16LE byte length, and ASCII recognition does not perform lossy Unicode
conversion.

The grammar admits one exact lowercase 241-byte scratch component with three
64-hex fields. Malformed recognized-family names, multiple scratch attempts,
duplicate fixed roles, and noncanonical ASCII-case equivalents reject during
the scan. Ordinary siblings consume the enumeration budget but do not become
catalog facts or historical-baseline material.

`provider/namespace.rs` performs only no-follow metadata/open operations on
recognized roles. Regular records are read with a `limit + 1` bound; oversize,
wrong-kind, symlink, disappearance, malformed canonical record, and retired
wrong-kind states remain typed non-ownership. Unrelated siblings are never
opened. The resulting rows are sorted allocation-free and the fresh digest
binds exact opened identities, bytes, retired fact, enumeration counts, index
facts, live identities, lookup modes, and rename domains.

### Restart-stable ownership and current collision state

The scratch parser independently recovers the durable target digest,
historical collision digest, and nonzero token from the name. The classifier
reconstructs the exact expected record for zero bytes and every strict
expected prefix; other scratch bytes reject. A current durable-target mismatch
rejects before an expected record is accepted.

`provider/digests.rs::select_historical` accepts historical material only from
one consistent scratch name, exact active record, or exact retired record.
Conflicting sources, malformed records, wrong kinds, and target mismatch
reject. A fresh attempt with no durable source uses the current complete
collision snapshot. Once progress exists, the historical digest comes from
that progress, while `observe_workspace` still performs a new complete
reserved-path collision scan and `FreshObservationDigestV1` still binds the
current index/worktree and namespace facts. Thus ordinary later index drift
does not strand recovery, but a current exact/ancestor/descendant,
case-equivalent, stage/flag/gitlink, worktree-kind, or dynamic-scratch-family
collision blocks read-only.

The exact active and retired decoders enforce canonical bytes, record ID,
closed names, support-profile consistency, bounded durable paths, and nonzero
ownership tokens. `CatalogAttemptBindingV1` additionally matches the current
root kind, support profile, durable target, mutation-parent identity, and
durable parent path. Copying recovery evidence to another durable target is
therefore not adoption.

### Retained identity, no-follow, and revalidation semantics

The ready path retains the lease target, repository/common-directory
relationship, actual mutation parent, index when applicable, and complete raw
role observation. Permit issuance revalidates the bound target and repeats the
full provider observation. `CatalogPermitV1::revalidate_observation` first
revalidates the lease, named target relationship, retained handles, durable and
invocation identities, modes, and rename domains; it then performs a fresh
complete observation and requires the same fresh digest. Target, parent,
related-Git, index, role identity/bytes, ordinary-entry counts, or namespace
substitution therefore yields no authority.

This sequencing is read-only in C1. A later C2 physical edge must still call
the retained revalidation immediately before its no-follow/no-replace
linearization point, as required by the amendment; C1 exposes no mutation that
could bypass that requirement.

### Missing-parent state remains disjoint

An absent Git `gwz` parent produces only
`MissingGitPrivateParent(MissingCatalogParentPermitV1)`. Its digest binds the
lease-derived Git relationships, live/durable identities, mode, rename domain,
fixed absent child, equivalent-alias proof, and bounded scan counts. It carries
no durable catalog-target digest, historical digest, scratch token, raw child
roles, expected record, or ready attempt binding. Appearance, wrong kind,
alias, target drift, or relationship drift makes its revalidation fail.

No missing-parent operation is implemented in C1. The permit can neither be
converted to `Ready` nor classify a catalog attempt; C2 remains responsible
for the one fixed parent edge followed by complete pre-catalog re-entry.

### Aggregate closure and the deliberate C1 interior result

The pure classifier has one closed accepted tuple for each abstract edge:

- no roles, zero/prefix scratch, or exact scratch;
- one matching active record with missing/owned-prefix/exact staging;
- one matching active record with exact final;
- exact retired plus exact final completion.

Every other tuple is `Ambiguous`, including multiple sources, inconsistent
records, unconsumed roles, simultaneous staging/final presence, wrong owner,
and target drift.

C1 intentionally maps every physically present staging or final directory to
`CatalogDirectoryFactV1::Other`; it does not invent marker, infrastructure,
child-set, or directory-identity ownership from outer presence. Consequently,
an active record plus an empty/partial staging directory and an active or
retired record plus a physically present final directory block in C1. This is
the safe result: C2 must perform the bounded ten-slot interior observation and
derive `ActiveOwnedPrefix` or `ExactOwned` only after matching the active
record, token, markers, retained parent/path, child set, identities, and exact
infrastructure record. Until then, C1 cannot proceed or adopt the directory.

The final-directory observer's bounded retired-record read does not weaken
that closure: it may preserve a unique historical attempt source, but the
outer final fact remains `Other`, so even exact retired evidence cannot return
`Complete` before C2 supplies the separate exact-interior proof.

### Read-only C1 boundary

The new `checked_artifact/catalog.rs` tree contains no filesystem type or
writer primitive. The production pre-catalog provider opens only retained
directories/files for observation. Crate-wide reference inspection shows the
new production preflight and classification entries have no non-test consumer;
the earlier infrastructure compile-proof provider always returns ambiguity and
has no physical implementation. No C1 path creates, rewrites, renames,
removes, flushes, or publishes a catalog role.

The exact-tree boundary checker positively pins the complete catalog and
pre-catalog source trees and passed. Therefore R2-C2 remains the first package
capable of catalog namespace mutation.

## Independent checks and adversarial probes

The committed core tree was exported directly from the exact SHA to
`/tmp/gwz-r2c1-review.36Hjxf`. Focused tests and static gates ran from that
export, with no implementation/design/plan file changed in the workspace:

```text
git -C gwz-core archive 74747661f2f030a590e115af0daf392e008c06f6 | tar -x -C <isolated-export>

cargo test --lib checked_artifact::interface_tests::catalog_classifier -- --nocapture
  10 passed; 0 failed

cargo test --lib checked_artifact::capability::pre_catalog::provider -- --nocapture
  35 passed; 0 failed

cargo test --lib checked_artifact::interface_tests::catalog_recovery -- --nocapture
  5 passed; 0 failed

python3 protocol/regen.py --check
  OK; committed protocol artifacts are current

python3 scripts/checks/check_checked_artifact_boundaries.py
  ok; 15 visible entries, 5 classified modules
```

Three additional review-only tests were appended only inside the disposable
exact-SHA export and run with `cargo test --lib adversarial_ -- --nocapture`:

1. an active record plus a physically present empty staging directory, and an
   active record plus a physically present empty final directory, each return
   `Ambiguous` while their bytes/directories remain unchanged;
2. a canonical zero-byte scratch copied to another repository target is
   rejected and remains byte-identical; and
3. a 17-byte valid scratch prefix followed by a current tracked collision at
   `.gwz/checked-artifacts` is rejected without rewriting the prefix or
   publishing active state.

Result: **3 passed; 0 failed**.

Additional read-only/static commands included:

```text
git -C gwz-core diff --check 1c5e42db9851c96b801e5aa7e6996207ecd6c514..74747661f2f030a590e115af0daf392e008c06f6
rg -n '<filesystem-writer primitives>' gwz-core/src/checked_artifact/catalog.rs gwz-core/src/checked_artifact/catalog
rg -n 'preflight_catalog_target|classify_observed|revalidate_observation' gwz-core/src
gwz status --json
```

The diff check and writer scan were clean. The exact checkpoint's already
recorded 1,311-test full suite was not redundantly repeated; this review ran
the proportional state, provider, recovery, protocol, structural, and added
adversarial gates above.

## Final tuple and workspace status

Immediately after writing this report, the commit tuple was rechecked as:

```text
workspace root 515d7f0eb4422a843bed1edbd2a71aae891435af
gwz-core       74747661f2f030a590e115af0daf392e008c06f6
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz-core` and `gwz-cli` remained clean and lock-matched. The workspace root
had only this newly written untracked review report; no implementation,
design, plan, lock, marker, or peer-review file was modified.
