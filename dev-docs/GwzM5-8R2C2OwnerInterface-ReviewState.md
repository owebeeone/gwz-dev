# R2-C2 owner interface state/filesystem review

Date: 2026-08-14

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 2 |
| P2 | 2 |
| P3 | 0 |

Any P0/P1/P2 is a blocking result under the requested gate.

## Reviewed immutable tuple

- workspace root: `04d4fc0bc9e203db83d15f13e73e3a7a26a72556`
- `gwz-core`: `1ef95b4ade6fa402cf1f2101a84af493af6a7beb`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`
- root parent reviewed: `45058dc`
- core parent reviewed: `f416e6d`

The initial root, core, and CLI statuses were clean. The complete root and core C2 deltas, the controlling catalog amendment, the R2-C2 plan checkpoint, the C2 owner/provider/interface code, and the inherited C1 production observation/classification path were reviewed. No implementation, design, plan, lock, or marker file was modified.

## Findings

### [P1] The physical owner deterministically stops after active publication and production completion is unreachable

`CatalogOwnerV1::execute_one` implements only `WriteOrRewriteScratch` and `PublishActive`; all later non-complete decisions return `"later R2-C2 recovery edge is not implemented"` (`gwz-core/src/checked_artifact/catalog/bootstrap.rs:52-79`). A fresh workspace therefore takes the two tested edges, is freshly observed with exact active and missing staging, classifies as `PrepareOrRewriteStaging`, and fails. The exact active record remains durable, so every restart returns to the same failure.

This is not only a missing writer. The production adapter deliberately maps every observed staging or final directory to `CatalogDirectoryFactV1::Other` (`gwz-core/src/checked_artifact/capability/pre_catalog/provider/aggregate.rs`), while `Complete` requires exact final plus exact retired state. Consequently the `Complete` arm at `catalog/bootstrap.rs:65-69` cannot be reached from physical production facts even if an exact completed ten-slot catalog is already on disk.

The three new real-filesystem tests stop after missing-parent creation, scratch creation, or active publication. There is no physical implementation or restart/fault evidence for the ordered seven staging prefixes, the exact ten-slot completed layout, anchor B-to-A and A-to-B-to-A durability exercise, staging-to-final identity preservation, active retirement, exact completed reopen, extra/wrong-kind/alias rejection inside staging/final, or return of a completed retained catalog. This directly misses R2-C2 item 17 and amendment Sections 5, 7, and 8.

Required correction: connect one retained physical aggregate owner to all five durable bootstrap transitions. It must classify and execute each ordered staging-prefix repair, publish final no-replace while preserving the staged directory identity, retire active only after exact final validation, and recognize exact completed state through a bounded ten-slot interior observation. Re-enter complete preflight after every single mutation. Add fresh, every-prefix, every-edge crash/restart, completed-reopen, wrong-kind/extra/alias, and no-replace race tests that drive `recover_or_create` all the way to `Complete`.

### [P1] `OpaqueRetainedCatalogV1` does not retain the completed catalog object needed by C3

The purported retained result contains only `Box<CatalogPermitV1>` (`gwz-core/src/checked_artifact/catalog/bootstrap.rs:13-16`). That permit retains the target/root/repository/common/private-parent handles and raw role facts, but no final catalog directory handle. Physical namespace observation opens staging/final directories only transiently, records encoded identities, and drops the handles (`capability/pre_catalog/provider/namespace.rs:91-136`). The completed branch would therefore return a stale preflight snapshot plus the mutation-parent handle, not a retained exact final catalog.

This cannot give C3 safe capability access without reopening the final pathname after return. Such reopening loses the object association that the opaque retained type is supposed to carry and creates a final-name substitution interval. Merely keeping the target lease does not bind an opened final object, and exposing the parent handle would violate the sealed interface.

Required correction: have the completion observation retain and revalidate the exact final directory handle and the identity-pinned ten-slot evidence under the still-live target lease, then move that owner-private retained object into `OpaqueRetainedCatalogV1`. Give C3 only narrow owner-private operations over that retained capability; do not expose raw handles or allow C3 to supply/reopen a path.

### [P2] Existing-scratch rewrite and active publication mutate by pathname after the freshness check

Both permit edges first call `revalidate_observation`, but then discard the fresh observation and invoke a name-based provider operation (`capability/pre_catalog.rs:154-187,190-215`). For a partial scratch, `write_or_rewrite_scratch` opens the leaf with `truncate(true)` before it checks the newly opened file's identity or bytes (`provider/mutation.rs:58-70`). A replacement regular file installed after revalidation is therefore truncated and overwritten. For exact scratch, `publish_active_record` performs a path-only no-replace rename and never checks that the source currently named by the leaf is the observed scratch object (`provider/mutation.rs:94-119`). A substituted object can be moved into the reserved active role before the next preflight rejects it.

No-follow protects against following a symlink and destination no-replace protects an existing active name, but neither proves source-object association. The retained raw row already contains the observed identity; the mutation seam does not consume it. This violates the live-freshness/substitution requirement and can turn a classification race into mutation rather than read-only rejection.

Required correction: issue edge-specific retained source capabilities from the immediately fresh observation. For rewrite, open without truncation, compare identity/kind/current expected-prefix bytes, and only then truncate/write through that same handle. For publication, bind the no-replace rename to the freshly verified source identity and verify the published identity before accepting the edge; a source substitution must not be treated as owned progress. Add deterministic hooks at post-revalidation/pre-open, post-open/pre-truncate, and pre-rename boundaries and assert read-only failure for substituted/wrong-kind sources.

### [P2] Active publication does not flush a recovered exact scratch file before its durable rename

`publish_active_record` renames the scratch leaf and flushes only the parent directory (`provider/mutation.rs:94-119`). It never opens, rereads, identity-checks, or `sync_all`s the source. The prior in-process write path does flush the file, but an exact scratch can be recovered after an interruption before a previous file flush completed, or observed from still-dirty cache after a process-only crash. Publishing and syncing only the directory does not establish durability of that recovered file's contents before the name becomes the sole active ownership record.

Required correction: reopen the exact source no-follow, prove its expected identity and canonical bytes, flush the file successfully, then perform the no-replace publication and parent flush. Add a fault edge between exact scratch bytes and source-file flush, followed by recovery/publication and a second restart, and require exact active recovery.

## Positive evidence retained

- The target mutation lease remains borrowed through the witness/permit loop, and callers cannot supply a raw path, token, expected record, provider, or callback.
- `MissingCatalogParentPermitV1` remains disjoint from ready authority. Its implemented edge creates only fixed `gwz` through the retained actual-Git-directory handle and flushes that parent; the next owner iteration performs full preflight. An `AlreadyExists` race is not adopted from the old permit and is classified on re-entry.
- Fresh scratch creation uses `create_new`, no-follow open, canonical record bytes, file flush, reread verification, and parent flush.
- Active publication uses platform no-replace rename and a post-rename parent flush.
- Existing focused interface and boundary checks continue to prove sealed surface shape, permit-field disjointness, and compiler/source-boundary integrity. These positives do not close the findings above.

## Commands and evidence

All commands ran from the exact requested tree unless a directory is shown.

```text
cat AGENTS.md
cat AGENTS_GWZ.md
cat gwz-core/AGENTS.md
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD
git status --short
git -C gwz-core status --short
git -C gwz-cli status --short
gwz status --json
git show --stat --summary 04d4fc0bc9e203db83d15f13e73e3a7a26a72556
git -C gwz-core show --stat --summary 1ef95b4ade6fa402cf1f2101a84af493af6a7beb
git diff 45058dc..04d4fc0 -- dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md
git -C gwz-core diff f416e6d..1ef95b4 -- scripts/checks/check_checked_artifact_boundaries.py src/checked_artifact
cargo test --lib checked_artifact::catalog::bootstrap::tests -- --nocapture
  PASS: 3 passed, 0 failed (1,317 filtered out)
cargo test --lib checked_artifact::interface_tests::capability_permit -- --nocapture
  PASS: 6 passed, 0 failed (1,314 filtered out)
python3 scripts/checks/check_checked_artifact_boundaries.py
  PASS: checked-artifact boundary ok (15 visible entries, 5 classified modules)
cargo fmt --all -- --check
  PASS
git diff --check
  PASS
git -C gwz-core diff --check
  PASS
```

One attempted `cargo test` invocation supplied two positional filters; Cargo rejected that command before running tests. It was immediately replaced by the successful module-filtered capability-permit command shown above and is not counted as a product failure.

## Final tuple and status

The final tuple must remain the exact tuple above. The only expected workspace status entry after this review is this report; `gwz-core` and `gwz-cli` must remain clean.
