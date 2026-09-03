# R2-C1 aggregate classifier independent code/interface review

Date: 2026-08-14 (Australia/Sydney)

## Verdict

**NO-GO**

Severity counts:

- P0: 0
- P1: 0
- P2: 1
- P3: 0

The exact R2-C1 tree is structurally bounded and read-only, and its ordinary
ASCII state machine is coherent. It is not safe on a supported
case-insensitive macOS filesystem, however: native non-ASCII names are charged
and then discarded before platform-equivalence classification. A physically
platform-equivalent scratch role can therefore be reported as absent and a
ready permit can classify the namespace as `WriteOrRewriteScratch`. This
contradicts the amendment's equivalent-name ambiguity rule and blocks C2.

## Reviewed checkpoint and scope

The review started from the requested exact, clean committed tuple:

- workspace root: `515d7f0eb4422a843bed1edbd2a71aae891435af`
- `gwz-core`: `74747661f2f030a590e115af0daf392e008c06f6`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

I read `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, the controlling
`GwzM5-8R2CCatalogBootstrapAmendment.md`, the R2-C0 accepted reviews, and the
R2-C1 checkpoint in `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`. I reviewed
the complete core delta from
`1c5e42db9851c96b801e5aa7e6996207ecd6c514` and the root delta from
`093857634e931696a2178f12970a7743ebe20e5d` (the plan's accepted C0 base).
Destructive/adversarial probes were confined to the exact-SHA export at
`/tmp/gwz-r2c1-review-code11.XyVEbE/root`; no implementation, design, plan,
lock, marker, or peer-review file in the workspace was modified.

## Finding

### P2 — non-ASCII platform-equivalent reserved names are skipped on a supported case-fold filesystem

`CatalogParentScannerV1::observe_os_str` first charges the native name, then
reduces it through `NativeAsciiNameV1` (`catalog/enumeration.rs:165-180`). Both
the Unix branch (`:314-320`) and Windows branch (`:325-330`) return `None` for
any non-ASCII native name, and `observe_ascii(None)` returns an unrelated row.
The physical provider consequently executes `continue` without a reserved row
(`capability/pre_catalog/provider/namespace.rs:31-46`). This is not a sound
equivalence decision: the macOS provider reports a case-insensitive parent as
`PathComponentMode::AsciiCaseFold`
(`capability/pre_catalog/provider/platform/macos.rs:44-49`), while APFS lookup
equivalence is not limited to ASCII spellings.

The counterexample was reproduced on this supported case-insensitive APFS
host, in the isolated exact-SHA export:

1. Native names `U+017F` (long s) and `U+212A` (Kelvin sign) resolved to the
   same objects as ASCII `s` and `k`; canonical-equivalence sanity checks using
   `Path.exists()` plus `os.path.samefile()` returned `True, True` for both.
2. An exact 241-byte scratch name was changed only by replacing one ASCII `s`
   in its fixed family prefix with `U+017F`, then created in the retained
   `.gwz` parent.
3. The canonical scratch spelling resolved to that same file, proving actual
   filesystem equivalence.
4. A fresh lease-bound `preflight_catalog_target` nevertheless returned
   success rather than ambiguity. The review-only assertion requiring an
   error failed with `platform-equivalent scratch spelling must be ambiguity`.

The downstream reason is observable in the committed code: after the skipped
row, `select_historical` accepts an empty reserved-row set as a fresh current
baseline (`provider/digests.rs:245-252`), and the all-missing aggregate selects
`WriteOrRewriteScratch` (`catalog/classifier.rs:210-225`). Because a new
attempt uses a different random token, its canonical scratch name is distinct
from the hidden equivalent-name attempt and can be created alongside it. The
hidden reserved candidate may then remain outside subsequent aggregate and
fresh-role evidence. This violates the bounded full-parent proof,
equivalent-name rejection, unique recovery provenance, and fail-closed
pre-mutation requirements. The same ASCII-only reduction is also a portability
risk for native UTF-16 case-fold parents.

Required correction:

- Do not convert a non-ASCII native name to an unrelated row before proving it
  is not platform-equivalent to any fixed reserved role or the scratch-family
  grammar. Retain the bounded native spelling for that equivalence decision and
  use semantics matching the admitted filesystem profile.
- A safe interim rule is to reject every non-ASCII name as capacity/namespace
  ambiguity after charging it when the parent mode is case-fold, while still
  allowing bounded non-ASCII foreign names on a proven sensitive parent. That
  is conservative but does not skip an equivalent alias.
- Apply the same fail-closed rule/proof to the retained `.gwz`/`gwz` parent
  alias scan and catalog-lease named-slot alias scan; they use the same
  ASCII-only assumption and otherwise leave a cross-edge inconsistency.
- Add physical macOS tests for long-s/Kelvin substitutions in a fixed role and
  a complete dynamic scratch name, native Windows UTF-16 equivalence vectors,
  and assertions that no ready/missing permit or mutation decision is issued.
  Preserve tests showing raw non-UTF-8 Unix names are losslessly charged on
  sensitive parents.

## Other reviewed properties

- **Exact scratch grammar:** `CatalogScratchNameV1` is exactly 241 ASCII bytes,
  with three 64-character lowercase hexadecimal fields, two separators,
  byte-for-byte reconstruction, and a nonzero ownership token. Short, long,
  uppercase, non-hex, extra-suffix, and zero-token forms reject.
- **Bounds and ordinary losslessness:** the scan charges every entry before
  classification and enforces 4,096 entries, 255 native units, 510 encoded
  bytes per entry, and 2,088,960 aggregate bytes. Unix charges raw bytes and
  Windows charges UTF-16 units times two. The finding is specifically that
  charged non-ASCII names are then semantically skipped on case-fold parents.
- **Malformed/duplicate ASCII roles:** malformed scratch-family names,
  duplicate roles, uppercase/equivalent ASCII spellings, wrong kinds, and
  over-budget observations reject read-only.
- **Digest separation and provenance:** fresh, durable-target, and historical
  digests have distinct types and domain labels. Durable recovery provenance
  is selected only from one consistent scratch name, exact active record, or
  exact retired record; target mismatch and source/token conflict reject.
  Current collision/index state is recomputed, while unrelated current index
  changes do not replace a persisted historical digest.
- **Permit closure and revalidation:** Ready and
  `MissingGitPrivateParent` remain disjoint and lease-owned. Revalidation
  repeats target, retained-root, repository/common-directory, named-role,
  index, namespace, and fresh/missing digest checks. The missing permit exposes
  neither catalog digests nor a conversion to Ready.
- **Classifier:** the pure match table exposes one decision for each admitted
  absent/scratch/active/staging/final/retired edge and maps unlisted or
  conflicting facts to `Ambiguous`. Scratch bytes accept only zero/every
  expected prefix/exact reconstructed bytes, and active/retired records must
  agree on the complete attempt.
- **Present staging/final interiors:** treating them as `Other` in C1 is safe
  and contract-consistent. The production aggregate cannot claim
  `ActiveOwnedPrefix` or `ExactOwned` from an outer directory alone, so it
  returns ambiguity rather than adopting contents. C2 must perform the ten-slot
  retained interior validation and fresh revalidation before constructing
  either owned directory fact.
- **Protocol decode bounds:** outer and retired record reads use the catalog
  16-KiB limit plus one, oversize is explicit, and canonical decoding repeats
  the shared bounded reader. Durable-path/schema/vector tests confirm the
  restart-stable record shape.
- **Collision domain:** the workspace index collision domain reserves all four
  fixed paths plus the complete dot-suffixed scratch family, including exact,
  ancestor/descendant, stage, flag, gitlink, kind, and ASCII case-fold
  collisions. The P2 shows that physical native-name equivalence is not yet
  equally complete.
- **No C1 writer:** `checked_artifact/catalog.rs` and its complete descendant
  tree contain only grammar/classification. The provider observation path opens
  no-follow handles read-only. The only mutations observed in C1-related files
  are `#[cfg(test)]` fixture setup. Staging/final production facts remain
  unowned until C2.
- **Source gates:** both the catalog and pre-catalog module trees are pinned as
  recursive path-and-byte manifests; catalog lease reference sets and removed
  provisional names are exact. The gates passed and would fail closed on tree
  drift, but byte pinning cannot make the pinned equivalence assumption sound.

## Commands and results

All Rust commands below ran against the isolated exact-SHA export, using the
workspace Rust 1.95.0 toolchain and the shared ignored target directory.

- Tuple/status: `git rev-parse HEAD` and member `rev-parse/status` checks —
  requested tuple exact; initially clean.
- Export: `git archive <exact-sha> | tar -x` for root, `gwz-core`, `gwz-cli`,
  plus the workspace's clean `taut-shape-rs` path dependency — success.
- `cargo +1.95.0 test -p gwz-core --lib checked_artifact::interface_tests::catalog_classifier`
  — **10 passed, 0 failed**.
- `cargo +1.95.0 test -p gwz-core --lib checked_artifact::capability::pre_catalog::provider::catalog_tests`
  — **13 passed, 0 failed**.
- Focused durable record/schema/vector/codec filters — respectively **8/8,
  2/2, 1/1, and 6/6 passed**.
- `python3 scripts/checks/check_checked_artifact_boundaries.py` —
  `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`.
- Five selected source-boundary adversarial tests (current inventory, exact
  lease reference set, recursive lease tree, removed callback name, and
  comment/string masking) — **5 passed**.
- `cargo +1.95.0 fmt --all -- --check` — passed.
- Root and core committed-range `git diff ... --check` — passed.
- Direct APFS native-equivalence probe — long-s/ASCII-s, Kelvin/ASCII-k, and
  composed/decomposed-e-acute each resolved to the same object.
- Review-only exact-export provider regression
  `review_probe_non_ascii_platform_equivalent_scratch_is_rejected` —
  **failed as expected**, because `preflight_catalog_target` returned success
  for the platform-equivalent noncanonical scratch spelling.
- `cargo +1.95.0 check -p gwz-core --lib --target x86_64-pc-windows-msvc` —
  inconclusive environmental probe: Rust target was installed, but
  `libz-sys` could not compile because this macOS host has no Windows C
  sysroot/headers or vcpkg. This did not affect the platform-neutral UTF-16
  grammar test, which passed.

## Final tuple and workspace status

Immediately before writing this report, the exact tuple was unchanged:

- root `515d7f0eb4422a843bed1edbd2a71aae891435af`
- core `74747661f2f030a590e115af0daf392e008c06f6`
- CLI `3cca145c0b32410f250f640730ed7ca18f1da59f`

Tracked workspace and both member trees were clean. A peer's expected untracked
`dev-docs/GwzM5-8R2C1AggregateClassifier-ReviewState.md` appeared during the
review and was not read, modified, or removed. The only additional workspace
path written by this review is this report.
