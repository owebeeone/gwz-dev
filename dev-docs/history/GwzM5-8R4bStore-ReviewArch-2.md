# R4b-S checked-store architecture remediation re-review

## Scope and conclusion

This is an independent re-review of the settled R4b-S remediation against the
four findings in `GwzM5-8R4bStore-ReviewArch.md`, the frozen checked-store
contract in `GwzM5-8R4bTransitionDesign.md` §§9–13, and the I2 §8
unknown-field identity and retirement rules. I reviewed the participant-drift
authority/reducer/effect/store path, the 53-effect store matrix, the archive
publication primitive and reconciliation paths, the lineage/file-shape tests,
the revised budget and ownership split, and production isolation.

**GO. P0: none; P1: none; P2: none; P3: none.** All four prior findings are
closed. R4b-S is suitable for interface acceptance and dependent package work.

## Prior-finding closure

### Duplicate-aware participant drift is exact through the store boundary

`ParticipantDriftIdentity` now carries the I2 identity in full: kind, every
expected/live field, and occurrence index. The bound payload carries that
identity together with the member and typed row
(`authority/drift.rs:6-52`). Both record and clear verify that the payload row
matches the supplied semantic identity. Record replaces the named occurrence,
appends only the next valid occurrence, and rejects an occurrence gap; clear
removes the named occurrence (`transition/reduce/mod.rs:237-283,382-417`). The
effect and retirement vocabulary retain the same member plus full identity
instead of reducing it to drift kind.

Before overlay, the store removes only the exact retired locator and rebases
later identical occurrences down by one. It then accepts a rebased survivor
only when the new manifest contains that exact locator and value
(`store/unknown.rs:17-53,90-118`). The manifest transformation itself applies
the same exact-occurrence retirement/rebase and rejects locator collisions
(`record_wire/unknown_fields/mod.rs:111-131`). Thus neither clearing duplicate
can attach its unknown to the other duplicate, and a same-identity diagnostic
replacement retains the locator while a distinct identity is appended rather
than overwriting an existing row.

The focused regression clears occurrence 0 and occurrence 1 in separate real
store commits and proves the other duplicate keeps its own distinct unknown.
The record-path regression proves a different full identity does not replace
or steal the existing row's unknown
(`tests/store/drift.rs:15-114`). This closes the first P1.

### Every transition effect crosses the checked store with unknowns

The predecessor harness now exposes one successful typed old/next/effect case
for every declared effect. The store matrix deduplicates by `EffectKind`,
asserts exactly 53 cases, and sends every case through
`CheckedV1Store::load_open`, the test-only checked preparation seam, and
`CheckedV1Store::commit` (`tests/store/matrix.rs:17-69`). It compares both the
exact raw YAML value and exact post-commit `UnknownFieldManifest`.

The seeder covers the complete extractor-owned live set present in each case:
record, baseline, participants, pending action, commit spec and both
signatures, errors, conflict/preservation/drift rows, recovery and rollback or
preservation journals (including owner and stash-object identity), operation
drift, accepted metadata/source/lock/audit/integration/final-checkout/root/base
and baseline hashes, publication candidate, candidate hashes, and root
preservation (`tests/store/matrix.rs:114-185`). Accepted lock-member extensions
are seeded only in candidate-free cases and are simultaneously installed into
the exact accepted-lock YAML and digest; candidate-bearing cases keep their
already-bound accepted/candidate lock bytes valid. Immutable typed extensions
needed by validation are carried before the store overlays the full manifest
(`tests/store/matrix.rs:187-260`). The focused matrix passes all 53 commits.

This is the required transition-wide survivor/retirement gate and closes the
second P1.

### Archive publication is atomic, no-replace, and race-safe on release families

The hard-link/unlink publication interval is gone. `rename_noreplace` uses
`renameat2(RENAME_NOREPLACE)` on Linux (covering x86-64 and arm64 release
families), `renamex_np(RENAME_EXCL)` on macOS, and `MoveFileExW` with
`MOVEFILE_WRITE_THROUGH` and without `MOVEFILE_REPLACE_EXISTING` on Windows
(`durable_fs.rs:53-105,147-180`). Unsupported families fail closed. No hard-link
archive path remains.

Archive first verifies the exact regular source and canonical `done`
directory, then performs the no-replace rename. Success syncs the affected
directories and rereads the exact destination. If a destination wins the
check-to-rename race, only an exact-byte destination permits source removal;
different or non-regular destinations reject with the source retained
(`store/archive.rs:59-83`). Destination-only and both-copy retries reconcile
only exact bytes. The primitive regression proves an existing destination is
never clobbered, while the archive regressions cover exact publication, both
crash shapes, mismatch rejection, and symlinked `done`. This closes the first
P2.

### Same-byte replacement and non-regular shapes have focused evidence

The lineage regression physically replaces the checked source with a new file
containing identical bytes and proves the prepared commit still succeeds.
The non-regular regression replaces an open source with a directory and makes
an archive destination a directory; both fail closed, the open directory is
untouched, and the archive source retains its exact bytes
(`tests/store/shapes.rs:11-73`). Existing symlink and different-byte tests
complete the adversarial shape set. This closes the second P2.

## Budget, ownership, and isolation

The revised R4b-S ceiling is 1,050 net production-bearing lines, 250 moved
production lines, 1,000 focused test/tool/doc lines, 15 production paths, and
8 test/tool/doc paths. The ledger records approximately +970 production lines,
about 145 moved lines, and the corrected 15-path inventory. The four focused
store test owners are 488, 127, 291, and 73 lines, totaling 979. The four store
production owners total 598 lines. Every reviewed production and test owner is
below 500 lines; the largest production owner is `authority.rs` at 495 and the
largest test owner is `dispatcher_attempt_matrix.rs` at 498. The revised
package stays within every numeric and path ceiling.

The lifecycle remains reachable only through `#[cfg(test)] mod v1_lifecycle`
in `merge/mod.rs`. The reviewed tree introduces no raw v1 writer, v1-to-v0
serialization route, production lifecycle dispatch, wire field, phase, or
protocol change. Normal v0 persistence and runtime dispatch remain isolated.

## Verification

Executed on the final settled bytes:

- `cargo test -p gwz-core v1_lifecycle::tests::store --lib`: 16 passed;
- `cargo test -p gwz-core v1_lifecycle --lib`: 66 passed;
- `cargo test -p gwz-core --lib`: 868 passed, 1 ignored;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed;
- `cargo fmt --all -- --check`: passed;
- `git diff --check`: passed;
- merge document consistency: 8 sources / 87 assertions passed; and
- merge checker tests: 17 passed.

The platform-specific archive branches were additionally inspected against
the four release platform families. Local foreign-target exploratory builds
could not proceed through unrelated native `libz-sys`/`openssl-sys` sysroot
dependencies, so they are not reported as executable release-platform gates;
the native macOS no-clobber and archive tests above passed.

## Gate decision

**GO for R4b-S interface acceptance.** Finding count: **P0 0 / P1 0 / P2 0 /
P3 0**. The checked store may now be consumed by the sequenced dependent
packages.
