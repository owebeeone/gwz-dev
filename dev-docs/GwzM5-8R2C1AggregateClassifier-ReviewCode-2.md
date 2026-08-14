# R2-C1 native-equivalence correction independent code/interface re-review

Date: 2026-08-14 (Australia/Sydney)

## Verdict

**GO**

Severity counts:

- P0: 0
- P1: 0
- P2: 0
- P3: 1

The original P2 is closed. On a case-fold parent, every bounded native name is
charged before any non-ASCII spelling rejects as ambiguity in the aggregate
catalog scan, retained private-parent alias scan, and catalog-lease final-slot
alias scan. On a proven sensitive catalog parent, bounded non-ASCII and raw
non-UTF-8 names remain charged unrelated entries. Physical macOS long-s and
Kelvin probes now fail closed without changing the observed entry, including
complete 241-byte dynamic scratch spellings. Synthetic Windows UTF-16 vectors
exercise the same conservative boundary.

No P0, P1, or P2 remains, so this correction is suitable to unblock the next
gate. The one P3 is documentation-only: the amendment's accepted-review hash
preamble was not updated after this native-equivalence rule was added.

## Reviewed checkpoint and scope

The review began from the requested exact, clean committed tuple:

- workspace root: `841367f9bc64b632a3f7b9a837a15ab886f2efc2`
- `gwz-core`: `f416e6d7bf8b582087b3dd1512dacc91db066369`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

I read `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, the current controlling
`GwzM5-8R2CCatalogBootstrapAmendment.md`, the R2-C1 checkpoint and correction
in `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`, and the original code review.
I reviewed the complete core correction delta from
`74747661f2f030a590e115af0daf392e008c06f6` through the requested core SHA.

Destructive and review-only probes ran only in isolated exact-SHA exports:

- pristine: `/tmp/gwz-r2c1-correction-pristine11.LWSqHy/root`
- adversarial: `/tmp/gwz-r2c1-correction-review11.OjRCCB/root`

Each export contained exact root, core, and CLI archives plus the clean
`taut-shape-rs` dependency. Each used a separate Cargo target directory. No
implementation, design, plan, lock, marker, or peer-review file was modified.

## Original P2 closure

The previous failure was a physical APFS case-fold alias that was budgeted and
then discarded as unrelated because its native spelling was non-ASCII. The
corrected flow closes that gap at every relevant observation edge:

- `catalog/enumeration.rs:157-175` charges both owned synthetic names and
  physical `OsStr` names first, then calls the common non-ASCII case-fold
  rejection before ASCII classification. `:260-280` applies the same rule to
  native fixed-name comparisons. Sensitive-mode `None` still reaches the
  unrelated-row result.
- `capability/pre_catalog/provider/retained.rs:348-382` uses the same bounded
  name budget and fallible native comparison for each retained-parent entry.
  A sensitive parent skips distinct-alias enumeration, as required by the
  amendment; a case-fold parent charges every enumerated name and rejects a
  non-ASCII spelling before issuing a bound observation.
- `bootstrap/runtime/catalog_lease/alias.rs:25-67` charges the case-fold
  final-slot parent entry count, native name, and aggregate encoded bytes
  before its explicit native-ASCII check. Non-ASCII is ambiguity. The Unix and
  Windows helpers at `:70-82` examine raw bytes and UTF-16 code units without a
  lossy Unicode conversion. A sensitive parent requires no alias enumeration,
  matching amendment section 4.

This order is important: capacity failure wins before semantic alias rejection,
so an attacker cannot evade the literal scan bounds by supplying native names
that will later reject. The conservative non-ASCII rule also cannot expose the
old false `Absent` state: aggregate observation stops before Ready facts or a
recovery decision can be issued.

The corrected test coverage matches the counterexample:

- `interface_tests/catalog_classifier.rs:131-148` supplies Unix long-s and
  Windows long-s/Kelvin UTF-16 vectors. Each rejects in case-fold mode and is a
  charged, unrecognized foreign name in sensitive mode.
- `provider/catalog_tests/grammar.rs:99-143` physically substitutes long-s in
  a fixed role and in a complete dynamic scratch name on macOS. When the host
  resolves the alternate spelling to the canonical object, full observation
  rejects and the entry bytes remain unchanged.
- `provider/production_tests.rs:628-640` proves retained-parent non-ASCII
  rejection occurs before the owner callback and leaves the catalog absent.
- `catalog_lease/tests/bounds.rs:87-110` proves Kelvin rejects at the final-slot
  scan under case-fold mode and remains acceptable under sensitive mode.
- A review-only exact-export test repeated full preflight with Kelvin in both a
  fixed role and a complete scratch name on this case-insensitive macOS volume.
  Both canonical spellings resolved to the created objects, both preflights
  rejected, and both payloads remained byte-identical.

I found no alternate path that classifies a non-ASCII native name before this
rule, converts it lossily, or issues a Ready/missing permit after rejection.

## Other correction properties

- **Sensitive names:** catalog-parent scanning still charges native Unix bytes
  and Windows UTF-16 units before treating bounded non-ASCII/non-Unicode names
  as unrelated. The existing physical Linux raw `foreign-\xff` test preserves
  the lossless sensitive-parent behavior. Sensitive fixed-slot scans perform no
  unnecessary distinct-alias enumeration, consistent with amendment section 4.
- **Read-only failure:** the committed physical and production tests assert
  unchanged bytes, no callback, and no catalog creation on the new rejection
  paths. The review-only APFS probe independently confirmed unchanged bytes.
- **Grammar and classifier:** the correction does not change the exact
  241-byte scratch grammar, record protocol, digest separation, recovery
  provenance, or one-edge classifier matrix. Present staging/final interiors
  remain unowned `Other` facts until C2 validates them; the correction adds no
  route to adopt them.
- **No C1 writer:** production changes are comparison, charging, and rejection
  logic only. The pure-tree writer sentinel remains at
  `interface_tests/catalog_classifier.rs:351-365`; fixture writes remain
  test-only. C2 is still the first package allowed to mutate catalog roles.
- **Source gates:** the catalog, pre-catalog provider, and catalog-lease tree
  hashes were updated together with their semantic tests. The exact-tree
  checker passed, and selected mutation tests proved the inventory, recursive
  trees, exact lease references, removed callback seam, and masking resistance
  remain fail-closed.
- **Contract consistency:** amendment lines 124-131 and plan lines 845-884
  now specify the same conservative rule and explicitly keep C1 read-only.
  The code behavior matches that rule, including the amendment's distinct
  sensitive-parent no-enumeration provision at lines 215-221.

## Finding

### P3 — amendment review-provenance preamble is stale after the semantic correction

The amendment header says its two accepted reviews cover semantic SHA-256
`bb59775646954b6e1ec8981794071397ca8ee5f3da4d2f9dd4d9592aceb92094` and that
only the provenance preamble was added afterward (`GwzM5-8R2CCatalogBootstrapAmendment.md:8-13`).
The current file's SHA-256 is
`5168f728e86ecb7d06b79ea4033dc80e5dacbf9e519a06f782a2808d63c1992f`, and its
body now includes the new native-equivalence rule at lines 124-131. The claim
that only the preamble changed after the reviewed semantic revision is
therefore no longer true.

This does not invalidate the implementation correction: the plan accurately
labels C1 unaccepted pending these focused re-reviews, and this report reviews
the changed rule directly. It is nevertheless misleading durable provenance.

Required correction text: replace the last sentence of the provenance
preamble with text stating that the native-equivalence correction at lines
124-131 was added after the original accepted semantic revision, identify the
focused `ReviewCode-2` and `ReviewState-2` reports as the reviews controlling
that correction, and record the new reviewed semantic SHA-256 after both
reviews accept a settled amendment.

## Commands and results

All reported Rust evidence was rerun with separate per-export target
directories. An initial shared-target attempt was discarded after it exposed
cross-export test-binary reuse; none of its output is used below.

- Tuple/status: root and both member `git rev-parse HEAD` plus porcelain status
  checks — exact requested tuple; initially clean.
- Exact exports: `git archive <exact-sha> | tar -x` for root, core, CLI, and the
  clean path dependency — succeeded.
- Core correction inventory: `git diff --stat
  74747661f2f030a590e115af0daf392e008c06f6..f416e6d7bf8b582087b3dd1512dacc91db066369`
  — 8 files, 150 insertions, 7 deletions.
- `CARGO_TARGET_DIR=/tmp/gwz-r2c1-correction-pristine11.LWSqHy/target cargo
  +1.95.0 test -p gwz-core --lib
  checked_artifact::interface_tests::catalog_classifier` — **11 passed, 0
  failed**.
- The same pristine target, provider `catalog_tests` filter — **14 passed, 0
  failed**.
- The same pristine target, catalog-lease test filter — **28 passed, 0
  failed**.
- Separate adversarial target, review-only physical Kelvin fixed-role and
  dynamic-scratch full-preflight test — **1 passed, 0 failed**.
- Focused committed filters for physical long-s, `non_ascii`, and
  `case_fold_alias_scan` — respectively **1/1, 4/4, and 3/3 passed**.
- `python3 scripts/checks/check_checked_artifact_boundaries.py` —
  `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`.
- Five selected boundary mutation tests (current inventory, exact lease
  reference set, recursive lease tree, removed provisional callback, and
  comment/string masking) — **5 passed** in 18.616 seconds.
- `cargo +1.95.0 fmt --all -- --check` — passed.
- Root and core committed-range `git diff --check` — passed.

The settled plan separately records the full corrected pre-review matrix. This
focused review did not duplicate that long matrix because the correction is
confined to the audited native-name paths and pinned test/source-gate updates.

## Final tuple and workspace status

After creating this report, the tuple was rechecked and remained:

- root `841367f9bc64b632a3f7b9a837a15ab886f2efc2`
- core `f416e6d7bf8b582087b3dd1512dacc91db066369`
- CLI `3cca145c0b32410f250f640730ed7ca18f1da59f`

Both member trees were clean. Root status contained only this report and the
peer review's expected untracked
`dev-docs/GwzM5-8R2C1AggregateClassifier-ReviewState-2.md`; I did not read,
modify, or remove the peer report. The only workspace path written by this
review is this report.
