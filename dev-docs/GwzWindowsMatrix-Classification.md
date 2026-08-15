# Windows matrix classification — run 31880022755

Date: 2026-08-15
Evidence: workflow `windows-matrix.yml` (dispatch-only, non-publishing,
contents:read), run `31880022755` on `74041ac`, windows-2022,
rustc 1.95.0. Totals: lib suite **1193 passed / 124 failed / 1 ignored**;
integration `29`-suite 27/2; all other suites green. 126 unique failures.
This supersedes the C1-era "98 failures" record in
`GwzMergeCheckpoint-v0.10.5.md`: the tree grew (C2 + gates) and two former
classes died — **zero `os error 87` occurrences** (the `d84a30d`/`a350746`
corrections, verified incorporated at `f532b1a`, hold on a real runner)
and no compile failures (the tree builds clean on Windows for the first
time on record).

## Classes, by root cause

| # | Class | ~Count | Sample | Kind | Disposition |
| --- | --- | ---: | --- | --- | --- |
| W1 | Git index-path containment: "index path is not inside the actual Git directory" | 28 | pre_catalog preflight/catalog tests | product (path canonicalization: verbatim/`\\?\`, 8.3, case) | R2-F product package: canonicalize both sides with the existing no-follow identity helpers before containment compare |
| W2 | `refs/heads/main` not found | 19 | v1_lifecycle reverse/rollback, g15 | test fixtures (runner lacks `init.defaultBranch`; libgit2 defaults to `master`) | test-only package: fixtures create the branch explicitly |
| W3 | Access denied (os error 5) on preservation artifacts/markers | ~28 | g15 root preservation, reverse_preservation | product (Windows ACL/read-only attribute vs reopen-for-DELETE pattern) | R2-F product package: open disposition audit on artifact reopens |
| W4 | Sharing violation (os error 32) | ~13 | catalog publish/final, durability suites | product (retained handles vs rename/delete sharing semantics) | R2-F product package: share-mode audit where retained handles overlap renames |
| W5 | Exact assertions + NotFound + residue | ~36 | mixed; incl. 10 `left == right`, 5 path-NotFound, cascades of W1-W4 | mixed (path spelling in expectations; cascade) | reclassify after W1-W4 land; most are expected to collapse |
| W6 | Protocol currency (2) | 2 | `generated_*_protocol_is_current` | infra (CRLF/generator env on Windows) | normalize line endings in the currency comparison or gate the test to Unix with the pinned-CI variant kept |

## Consequences

- The **resume-order classification item is complete**; per
  `GwzMergeCheckpoint-v0.10.5.md` item 5, R4b-G remains gated until the
  platform matrix is *accepted*, i.e. W1-W4 product packages and the W2
  fixture package land and a re-run is green (or a reviewed rescope says
  otherwise). W2 is mechanical; W1/W3/W4 are real Windows-semantics work
  in the checked-artifact/preservation layer and belong to the R2-F lane.
- The per-commit lane gate and boundary/retained-readers push workflows
  ran on the same push; their outcomes are recorded in the checkpoint.
- The dispatched workflow is retained for re-runs: each fix package ends
  with a fresh dispatch and an updated classification here.
