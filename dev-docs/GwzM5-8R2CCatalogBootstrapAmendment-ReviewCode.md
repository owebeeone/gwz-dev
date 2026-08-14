# R2-C catalog-bootstrap amendment code review

Date: 2026-08-14

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

The 241-byte dynamic scratch name, separate durable-target and historical-
collision digests, current collision reproof, fixed parent-scan budgets,
target-bound lease set, closed missing-parent preflight, and cross-cutting
durable-path schema close the restart, power-loss, portability, and caller-seam
defects reviewed. No P0-P3 finding remains in the final amendment.

## Findings

None.

## Confirmed design properties

- The scratch grammar is one canonical 241-byte ASCII component containing the
  durable target digest, historical collision digest, and random ownership
  token. This makes the exact record reconstructible from zero or partial
  scratch bytes after restart while independently rejecting durable target
  substitution.
- Historical collision evidence is no longer confused with live freshness:
  unrelated later index/worktree changes do not strand recovery, while a new
  complete collision scan and immediate retained-handle revalidation precede
  every mutation.
- Full-parent discovery is explicitly bounded at 4,096 entries and 2,088,960
  encoded name bytes, charges Unix raw names and Windows UTF-16LE names, retains
  non-Unicode Unix names losslessly, and does not open unrelated siblings.
  Ordinary `.gwz/locks`, `.gwz/merge`, and `.gwz/stash` siblings are correctly
  outside the catalog baseline.
- The fixed workspace and Git-directory final lock slots, non-interleaved
  preparation/final-acquisition phases, canonical target ordering, reverse
  release, shared linked-worktree Git lock, and opaque target-derived entry
  close the prior path-plus-lease and multi-target ordering seams.
- `CheckedDurablePathV1` replaces every durable reference to the mixed live
  canonical-path type and freezes tags plus the nonempty, ASCII, canonical,
  profile-consistent, 4-KiB semantic envelope. Live invocation identity and
  rename domains remain transaction-only.
- The closed `CatalogPreflightV1` separates
  `MissingGitPrivateParent(MissingCatalogParentPermitV1)` from
  `Ready(CatalogPermitV1)`. The live-only missing-parent permit carries no
  target/historical digest, token, expected record, child handle, or catalog
  authority; it authorizes only the fixed no-replace mkdir/flush edge and cannot
  convert to ready. Full re-entry is the sole route to the three-digest permit.
- The active record's fixed-name reservation, aggregate one-edge recovery,
  post-edge reobservation, and Git-parent retry are restart/power-loss sound.
- The wire changes are private and pre-A1. The amendment explicitly changes
  generated bindings, semantic/shape vectors, digest material, and record-ID
  vectors together, so no released-state migration or public GWZ compatibility
  shim is required.

## Commands and results

Initial workspace status, before this review report existed:

```text
 M dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md
?? dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md
```

During the review, an independent state reviewer added
`GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md`; it was not read or
modified by this review.

Final workspace status after writing this report:

```text
 M dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md
?? dev-docs/GwzM5-8R2CCatalogBootstrapAmendment-ReviewCode.md
?? dev-docs/GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md
?? dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md
```

Only `GwzM5-8R2CCatalogBootstrapAmendment-ReviewCode.md` was written by this
review. `git diff --check` reported no whitespace errors for the report.

Focused current-interface tests:

```text
cargo test -p gwz-core --lib checked_artifact::interface_tests::catalog_recovery
  5 passed; 0 failed
cargo test -p gwz-core --lib checked_artifact::interface_tests::capability_permit
  5 passed; 0 failed
cargo test -p gwz-core --lib checked_artifact::interface_tests::contracts
  10 passed; 0 failed
```

These are a green baseline for the existing interfaces; the amendment is not
implemented, so they do not prove its new recovery paths.

Static checks:

```text
computed dynamic scratch component length: 241 ASCII bytes
computed aggregate name budget: 4096 * 510 = 2,088,960 bytes
amendment SHA-256 reviewed: bb59775646954b6e1ec8981794071397ca8ee5f3da4d2f9dd4d9592aceb92094
```

Commit tuple rechecked immediately before writing this report:

```text
workspace root c0c5d7d3173de93459cd9b0bed473a00d162824b
gwz-core       dba602ae1d3e7f0c27a321f15a791d3ec7d98845
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

Review method included complete reads of `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`,
the current amendment, the accepted R1/R2 checkpoints and remediation plan,
and focused inspection of runtime lease, retained pre-catalog provider,
namespace observation, catalog record/schema, and aggregate recovery code.
No code or amendment file was modified.
