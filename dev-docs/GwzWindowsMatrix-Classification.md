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

## Run 2 — 31881596373 on `39b66aa` (post W2 + Linux-lint fixes)

Totals: **1201 passed / 116 failed / 1 ignored** + integration 27/2 = 118
unique (−8 net). `refs/heads/main` occurrences fell 20 → **1** (one
straggler with a non-`new_git` fixture path — first task of the next
W-package). The net drop is smaller than the 19 W2 clears because
formerly-W2 tests now progress into the W3/W4 walls: `os error 5`
panics rose to 37, confirming the classification's cascade prediction
(W5 collapse direction). Also on this push: the boundary workflow's
**first recorded green run** (`31880974224`, after the Linux
`redundant_guards` fold at `6edb9cb`) — the standing red since 2026-08-14
is resolved and the per-commit lane gate has its first successful CI
execution. Remaining matrix work: W1 (index-path containment, 28-class),
W3 (access-denied, now ~37), W4 (sharing violations), W6 (2 protocol
currency) — the R2-F product packages.

## Run 3 — 31882396053 on `4297c17`: W1 CLOSED

Totals: **1224 passed / 93 failed / 1 ignored** + integration 27/2 = 95
unique (−23 net). `index path is not inside` occurrences: **0** — the W1
class is fully cleared by the canonicalized-parent comparison. Remaining
leaders: W3 error-5 (preservation artifact 16, bundle 9, plus workspace/
test-artifact residue) and W4 error-32 (12 bare + 10 at "publish final
catalog" — the catalog seam's directory rename meeting a held handle
without delete-sharing on Windows; the failing edge is already named by
its label, so W4's catalog slice needs sharing-mode reasoning at that
edge, not more instrumentation). W5 assertion residue: 12.

## Run 4 — 31882850251 on `94fc8ca`: W4 catalog slice CLOSED, W3 named

Totals: 1224/93 (class shift at constant count). "publish final catalog"
error-32: **0** (was 10) — the caller's staging capability now drops
before the rename edge. The 31-site instrumentation named every surviving
error-5 in one run: **all 35 are `sync family entry`** (rebarrier_exact:
read-only reopen + `sync_all`, which Windows' FlushFileBuffers forbids).

## Run 5 — 31883193051 on `cf1ccaa`: W3 CLOSED

Totals: **1255 passed / 62 failed** + 27/2 = 64 unique (−31).
`os error 5`: **0** — the family-entry re-sync is Unix-only per the
write-through doctrine, with the anchor barrier unchanged. Remaining:
error-32 sharing (13 bare + 10 at "reopen publication source directory" —
the primitive's second capability lacking DELETE-share against the
rename handle, fixed next commit via `open_dir_share_delete` + 5 other),
7 assertion residue, 3 NotFound, 2 matrix-row escapes to diagnose.

## W1 fix (landed at `4297c17`)

W1 root cause confirmed at `provider/index.rs`: raw `PathBuf` equality
between GWZ's recorded git-directory spelling and libgit2's reported index
path — guaranteed to diverge under Windows 8.3 short names
(`RUNNER~1`-class temp paths). Fixed by comparing canonicalized *parents*
(the always-present anchor; the index file itself may not exist) plus an
exact `index` file-name check. W3 instrumentation plan for the next
session: the error-5 opens route through `CheckedArtifact::acquire`'s
read-only observation chain (`observation.rs` acquire → ambient root Dir →
`durable_identity`/`canonical_path_identity` → traverse), so the failing
syscall is not identifiable from the current error text; add the failing
operation name to the io-error labels in that chain (labels only, no
behavior), push, re-dispatch, then fix the identified Windows semantics
(candidates: directory opens without backup semantics in the identity
helpers; read-only-attribute interactions in replace/quarantine paths).
