# R2-C catalog-bootstrap amendment state review

Date: 2026-08-14

Review type: independent critical interface/type-state review of
`GwzM5-8R2CCatalogBootstrapAmendment.md` against the accepted R1/R2 documents
and the current `gwz-core` protocol and ownership types.

This report supersedes its earlier version for amendment SHA-256
`8604113976245554584171f4a971fe69a6e6f0d40db2cd6d6d163c438a6daf60`.
That version missed the absent-private-parent/ready-target-digest circularity.
The current amendment materially changes the capability state machine, and the
verdict below applies only to the corrected hash recorded here.

## Verdict

**GO** for the amendment to control physical R2-C implementation.

This is an interface verdict, not an implementation-completion claim. The
current code deliberately still has the pre-amendment shapes, and the
amendment correctly keeps R2-D blocked until its TDD, physical implementation,
structural, format, Clippy, and native-platform gates pass.

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

Required corrections: **none**.

## Settled review state

The final amendment reviewed was 483 lines with SHA-256
`bb59775646954b6e1ec8981794071397ca8ee5f3da4d2f9dd4d9592aceb92094`.
The repository tuple was:

- workspace root: `c0c5d7d3173de93459cd9b0bed473a00d162824b`;
- `gwz-core`: `dba602ae1d3e7f0c27a321f15a791d3ec7d98845`; and
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`.

The focused re-review began with the lead-owned amendment, the independent code
review, and this state-review path untracked, plus
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` modified. All member repositories
were clean and lock-matched. Those pre-existing paths were preserved; only this
report was replaced.

## Correctness evidence

### First-owner provenance and partial scratch recovery

The exceptional scratch grammar now carries three independently typed values:
the restart-stable target digest, historical collision digest, and random
ownership token. Its exact canonical name is 241 ASCII bytes, remains within
the proved component budget, is published no-replace only after absence and
collision proof, and is discovered by one bounded full-parent enumeration.
Duplicate, malformed-recognized, equivalent, wrong-kind, and over-budget rows
are closed read-only ambiguity.

This is sufficient for zero-byte and every bounded prefix recovery without
guessing. The historical digest and token reconstruct the original expected
record, while the separately recovered `DurableCatalogTargetDigestV1` is
always recomputed from current stable root/repository/common-directory/
mutation-parent facts. The separation is important: benign current index or
worktree drift cannot strand recovery, but moving or copying a prefix-only
scratch onto a substituted durable target cannot cause adoption.

The amendment explicitly states the reserved private namespace trust boundary:
a canonical name plus expected-prefix bytes is protocol state, while deliberate
same-user forgery of the private on-disk protocol is not claimed as an
authentication threat. Under that declared boundary, recovery does not adopt
an unclassified foreign row.

### Lease and collision semantics across restart

The live advisory lease is no longer durable record material. The only
production entry consumes an opaque, target-bound `CatalogMutationLeaseV1` and
derives the target and retained handles from it; no independent path can be
paired with a valid lease. Multi-target acquisition has a complete canonical
sort/dedup/revalidate/reverse-release rule and performs no private mutation
until the full batch is held.

`FreshObservationDigestV1`, `DurableCatalogTargetDigestV1`, and
`HistoricalCollisionDigestV1` have distinct domains and cannot be supplied or
converted by callers. Current collision freedom is re-proved before every
mutation. The historical digest remains durable attempt evidence rather than a
permanent pin on unrelated index bytes, and exact retired-plus-final completion
reopens under current freshness/collision proof. Ordinary `.gwz` siblings are
budget-charged but are neither catalog roles nor silently incorporated into
the historical record.

### Aggregate closure, staging ownership, and Git-parent re-entry

The role owner derives at most one expected record from scratch, active, or
retired evidence and rejects every unconsumed reserved fact. The exact active
record durably reserves the fixed staging role, so an empty or exact-prefix
staging directory is recoverable after the staging-create fault edge; an
unowned staging/final directory, extra child, wrong kind, alias, or content
outside the expected prefix is not adopted. Final and retired completion still
require the token, record ID, retained parent/path, markers, object identities,
and exact infrastructure record.

An absent Git-directory `gwz` parent has one edge only:
`CreatePrivateParentAndRetry`. It creates and flushes the fixed container
no-replace, creates no child, returns no authority, and forces complete
observation, collision validation, retention, and revalidation on re-entry.
An exact directory can be reused only as a parent; unmatched/unowned reserved
children remain ambiguity. This closes both the nonexistent-parent handle gap
and the container/child adoption gap.

The corrected state model removes the circularity missed by the earlier
review. The provider returns a closed `CatalogPreflightV1` sum:

```text
MissingGitPrivateParent(MissingCatalogParentPermitV1)
Ready(CatalogPermitV1)
```

The missing-parent variant is live-only and has no mutation-parent identity
from which a ready `DurableCatalogTargetDigestV1` could be fabricated. Its
opaque digest binds the retained actual-Git-directory/repository/common
identities, invocation identities, spelling/mode/rename-domain facts, bounded
enumeration counters, and exact child/alias absence proof. Its only operation
is fixed no-replace `gwz` mkdir plus parent flush. It contains no target digest,
historical digest, random token, expected record, child handle, or catalog
authority; it has no conversion to `CatalogPermitV1`.

Whether mkdir succeeds or loses a no-replace race, no child is classified as
owned. The only path to `Ready` is a complete new pre-catalog observation that
retains/revalidates the now-present parent and computes the ready permit's
three digests. This makes the missing-parent edge implementable without
guessing a fact that cannot exist yet and without weakening re-entry.

### Fixed target locks and batch ordering

The correction also makes the lease implementation concrete. Workspace and
Git-directory targets have disjoint fixed final lock slots. The Git slot is a
capability-neutral runtime file opened/created no-follow through the retained
Git-directory handle, with alias/kind/identity checks before a target-bound
lease can escape.

Multi-target acquisition has two non-interleaved phases: all transient guard
preparation completes while no final lease is held; then every final slot is
locked and revalidated in canonical target order. No guard can be entered once
final acquisition begins, no catalog mutation occurs before the whole set is
held, and any failure releases in reverse order. The compatibility wrapper can
lend only its exact workspace lease and cannot fabricate or append a Git lease.
These rules close both lease/root substitution and lock-order deadlock seams.

### Durable schema and ownership boundary

The amendment pins the exact `CheckedDurablePathComponentV1`,
`CheckedDurablePathV1`, and corrected `CheckedCatalogBootstrapV1` field tags.
Invocation identity and rename-domain proof stay on live retained transactions;
the durable path shape carries only canonical spelling, lookup mode, and
durable parent identity. The correction explicitly covers all five durable
path references in authority, catalog bootstrap, barrier, managed component,
and managed-parent intent records, with one shared semantic envelope and no
compatibility alias for the unactivated shape.

One sealed `CatalogOwnerV1` replaces the generic production callback and owns
the provider, pre-catalog proofs, aggregate classifier, mutations, and
post-edge reobservation. Production cannot accept raw lease/token bytes, raw
role rows, expected records, bootstrap callbacks, or synthetic observations.

## Current-code comparison

The current `gwz-core` tree confirms that these are necessary pre-R2-C changes,
not already-landed behavior:

- `src/checked_artifact/bootstrap.rs` still declares generic
  `CatalogBootstrapV1`;
- `src/checked_artifact/capability/pre_catalog.rs` still accepts caller-selected
  `[u8; 32]` lease bytes and a bootstrap callback;
- `protocol/checked_artifact.taut.py` and
  `src/checked_artifact/protocol/catalog_bootstrap_record.rs` still serialize
  invocation identity, rename domain, and the mixed live/durable canonical path;
- `src/checked_artifact/catalog_names.rs` still has the fixed scratch spelling;
  and
- `src/checked_artifact/bootstrap/runtime/mod.rs` already retains the workspace
  and actual Git-directory handles, providing the ownership basis that the new
  target-bound catalog lease must expose without widening it to callers.

The current aggregate classifier's accepted tuple remains the baseline closed
table: scratch missing/prefix, exact active, active-owned staging/final, and
exact retired completion, with every unlisted tuple ambiguous. The amendment
changes how the expected record and physical ownership evidence are obtained;
it does not weaken that closure.

## Read-only checks and commands

Commands run from the workspace root unless noted:

```text
gwz status --json
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD
git status --short
git -C gwz-core status --short
git -C gwz-cli status --short
shasum -a 256 dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md
sed -n '1,500p' dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md
rg -n 'foreign|adopt|guess|scratch|private namespace|ownership|collision' dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md dev-docs/GwzM5-8R4bR2ConsumerCheckpoint.md dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md
rg -n 'invocation|rename domain|CanonicalPath|CheckedCanonicalPath' dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md dev-docs/GwzM5-8R4bR2ConsumerCheckpoint.md gwz-core/protocol/checked_artifact.taut.py gwz-core/src/checked_artifact
rg -n 'CatalogPreflightV1|MissingCatalogParentPermitV1|MissingParentObservationDigestV1|DurableCatalogTargetDigestV1|RetryPreCatalog|CreatePrivateParentAndRetry|CatalogMutationLeaseV1|CatalogLeaseSetV1|bootstrap guard|final target' dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md
python3 -c 's="checked-artifacts-catalog-bootstrap-v1.scratch." + "0"*64 + "." + "0"*64 + "." + "0"*64; print(len(s))'
```

Focused checks from `gwz-core`:

```text
python3 protocol/regen.py --check
cargo test --lib catalog_recovery
```

Results:

- protocol regeneration check: pass; committed current (pre-amendment)
  artifacts are internally current;
- focused catalog recovery: 5 passed, 0 failed, 1,249 filtered out; and
- exact scratch-name calculation: 241 bytes.

The Rust/protocol commands were not repeated in this focused re-review because
the implementation tuple was unchanged. They establish the same current-code
baseline, not completion of the amendment's new TDD gates. The full corrected
amendment was reread, its capability-state references were cross-checked, and
the scratch-name bound was reasserted as 241 bytes.

## Final workspace recheck

The final `gwz status --json` recheck showed the same clean, lock-matched member
tuple and exact root/core/CLI commits recorded above. Root status remained the
lead-owned modified remediation plan plus the untracked amendment, peer code
review, and this replaced state review. No implementation or peer/lead-owned
file was modified by this review.
