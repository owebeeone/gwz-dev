# R2-C1 native-equivalence correction state/filesystem re-review

Date: 2026-08-14

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

Required corrections: **none**.

The peer's original APFS P2 is reproduced on its immutable tree and closed by
the complete correction. This verdict applies only to:

```text
workspace root 841367f9bc64b632a3f7b9a837a15ab886f2efc2
gwz-core       f416e6d7bf8b582087b3dd1512dacc91db066369
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

## Scope and controlling state

I read `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, the original state and code
reviews, the updated catalog amendment, and the updated R2-C1 checkpoint plan.
I reviewed the complete core correction from
`74747661f2f030a590e115af0daf392e008c06f6` to
`f416e6d7bf8b582087b3dd1512dacc91db066369`, including all eight changed
source/test/gate files.

The amendment and plan now consistently freeze the conservative rule: a
case-fold scan first charges the native spelling and then rejects every
non-ASCII entry; a proven sensitive catalog parent retains a bounded
non-ASCII/non-Unicode name as an unrelated fact. The earlier design rule that
a sensitive parent needs no distinct-alias enumeration remains intact for the
private-parent and final-lock alias-only scans.

The updated amendment SHA-256 is
`5168f728e86ecb7d06b79ea4033dc80e5dacbf9e519a06f782a2808d63c1992f`.

## Findings

None.

## Correction evidence

### Original P2 reproduced and closed

On this case-insensitive APFS host, both long-s (`U+017F`) and Kelvin sign
(`U+212A`) resolve to the same objects as ASCII `s` and `k`; direct
`exists`/`samefile` probes returned `True/True` for both pairs.

A review-only test against an exact export of the original core SHA created
`checked-artifactſ` in the retained `.gwz` parent, verified that the canonical
`checked-artifacts` spelling resolved to the same file, and required
`preflight_catalog_target` to reject. It failed at that assertion: the old tree
issued preflight success, reproducing the peer's P2.

The same physical fixed-role condition and a complete 241-byte dynamic
scratch name with a long-s substitution both reject on the corrected tree.
The alias bytes remain unchanged, no active record is published, and no Ready
permit or recovery decision escapes.

### Lossless charge before case-fold rejection

`CatalogParentScannerV1` now performs native accounting before the new
case-fold rejection in both production `OsStr` observation and synthetic
Unix/Windows grammar input. Unix charges raw byte length, including invalid
UTF-8; Windows charges the exact UTF-16 unit count and twice that encoded byte
length. Only after those bounded counters succeed does a non-ASCII case-fold
name return namespace ambiguity.

This closes fixed roles, malformed/dynamic scratch-family aliases, and
unrelated non-ASCII names uniformly. The conservative unrelated-name rejection
is intentional: admitted platform mode currently proves that the parent is
case-folding, not complete native equivalence semantics.

Sensitive catalog parents retain the prior behavior. Bounded non-ASCII and
raw non-UTF-8 Unix names are charged and remain foreign, with no reserved role
or scratch candidate. Synthetic UTF-16 long-s/Kelvin rows likewise reject only
in case-fold mode.

### Private-parent and final-lock consistency

The retained `.gwz`/`gwz` parent alias scan charges through the same
`CatalogNameBudgetV1` and now consumes the fallible
`native_name_matches_ascii` result. A non-ASCII entry on a case-fold root
therefore rejects before the child is retained and before either Ready or
Missing preflight can issue. A review probe acquired the Git target lease,
then introduced a Kelvin-name entry before preflight; the missing-parent path
rejected and did not create `gwz`.

The catalog-lease alias scanner independently charges native units, encoded
bytes, entry count, and aggregate bytes before rejecting non-ASCII. The helper
is invoked for the transient guard/final slot during preparation and again for
the final slot during phase-two acquisition. Two physical probes confirmed
both sides:

- a non-ASCII entry present before preparation returned no lease, preserved
  its bytes, and created neither the final lock nor a catalog parent; and
- after phase one created the canonical Git final-lock file, a fault hook
  renamed it to the APFS-equivalent `gwz-catalog-mutator-v1.locK`. Phase two
  rejected before returning a lease; the equivalent lock object remained
  unchanged and no catalog parent appeared.

The latter establishes that the correction is not confined to the initial
preparation scan. Failure after phase-one preparation may retain only the
already-authorized capability-neutral runtime lock grammar, exactly as the
accepted C0/amendment contract permits; it cannot reach a catalog role.

### Read-only failure and permit nonissuance

All corrected checks occur during bounded enumeration or retained observation.
They open no catalog writer. Physical probes covered:

- non-ASCII workspace-root input blocking the retained private-parent path;
- fixed and dynamic scratch aliases blocking Ready;
- post-lease Git-root input blocking Missing;
- pre-preparation lease failure; and
- post-preparation/final-acquisition lease failure.

In each case the adversarial bytes/name remained present and unchanged. No
scratch, active, staging, final catalog, or Git `gwz` parent was created.

### C1 boundary remains unchanged

The correction changes native grammar/alias observation only. It does not
change `catalog/classifier.rs`, provider aggregate mapping, namespace role
opening, digest selection, permit shapes, or any catalog physical owner.
`checked_artifact/catalog.rs` and its descendants still contain no filesystem
writer primitive, and the structural tree manifests were updated and passed.

Physically present staging and final interiors therefore remain
`CatalogDirectoryFactV1::Other` in C1. A repeated isolated probe with an exact
active record plus either empty staging or empty final returned `Ambiguous`
and preserved both the active bytes and directory. C2 must still prove the
ten-slot interior before constructing `ActiveOwnedPrefix` or `ExactOwned`.

## Commands and results

All destructive/adversarial work ran only in disposable exports of the exact
old or corrected core SHA. The workspace implementation/design/plan/review
files were not edited.

Original-tree reproduction:

```text
cargo test --lib review_state2_original_apfs_alias_must_reject -- --nocapture
  FAILED as expected: preflight_catalog_target(witness).is_err() was false
```

Corrected exact-tree committed gates:

```text
cargo test --lib checked_artifact::interface_tests::catalog_classifier -- --nocapture
  11 passed; 0 failed

cargo test --lib checked_artifact::capability::pre_catalog::provider -- --nocapture
  37 passed; 0 failed

cargo test --lib checked_artifact::bootstrap::runtime::catalog_lease::tests::bounds -- --nocapture
  6 passed; 0 failed

cargo test --lib checked_artifact::interface_tests::catalog_recovery -- --nocapture
  5 passed; 0 failed

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)

cargo fmt --all -- --check
  passed
```

Six review-only corrected-tree probes were appended only to the export and run
with `cargo test --lib review_probe_ -- --nocapture`: **6 passed, 0 failed**.
They covered fixed/dynamic APFS aliases, private-parent Ready nonissuance,
post-lease Missing nonissuance, both lease phases, and staging/final
fail-closed behavior.

Additional checks:

```text
git -C gwz-core diff --check 74747661f2f030a590e115af0daf392e008c06f6..f416e6d7bf8b582087b3dd1512dacc91db066369
git diff --check 515d7f0eb4422a843bed1edbd2a71aae891435af..841367f9bc64b632a3f7b9a837a15ab886f2efc2
rg -n '<writer primitives>' gwz-core/src/checked_artifact/catalog.rs gwz-core/src/checked_artifact/catalog
```

Both committed-range diff checks and the C1 writer scan were clean. The
checkpoint's recorded full 1,315-test suite was not redundantly repeated.

## Final tuple and status

Immediately after writing this report, the tuple was reverified as:

```text
workspace root 841367f9bc64b632a3f7b9a837a15ab886f2efc2
gwz-core       f416e6d7bf8b582087b3dd1512dacc91db066369
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz-core` and `gwz-cli` remained clean and lock-matched. The root had only
this requested untracked report. No implementation, amendment, plan, lock,
marker, original review, or peer file was modified.
