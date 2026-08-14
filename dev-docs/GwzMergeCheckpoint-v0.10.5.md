# GWZ merge checkpoint — v0.10.5

Date: 2026-08-15

## Purpose

This checkpoint separates a releasable, cross-platform v0 merge update from the
unfinished v1/M5–M8 catalog and recovery work. It is the restart point for that
work; it is not evidence that R2-C2 or the v1 writer is release-ready.

## Released line

The coordinated v0.10.5 release is deliberately based on the last narrow
pre-v1 implementation checkpoint:

| Deliverable | Included source checkpoint | v0.10.5 tag commit |
| --- | --- | --- |
| `gwz-core` | `b0781b4` (`Install strict merge record envelope decoding`), followed by standalone-lock refresh `45f1107` | `4bcf13c148ba4ad5fcbd55460d386e65ddd2b653` |
| `gwz-cli` | `9e23d0a6aae412dec4863a1446226509518ff9ae` | `fbb04ca31a45faf364246d33a31a16b32c7a176f` |
| `gwz-py` | `23a6de13f12449bf22197901d8ab4c349e1432c7` | `0dc00ea1e674e5585e91965849fd0d84b5883e66` |

The release includes the stable coordinated v0 merge lifecycle, coordinated
custom merge messages, strict merge-record envelope decoding and diagnostics,
and automatic repository selection for `diff --tagged`. It does not include
the test-gated v1 writer, catalog bootstrap, checked-artifact expansion, or
M5b/A1 activation.

Local release evidence before tagging was:

- core: 710 unit tests passed, one ignored, plus integration suites of 10, 27,
  4, and 2 tests; Clippy, formatting, and protocol checks passed;
- CLI: 138 tests passed across unit, diff, local workflow, publish, release,
  and rename suites; and
- Python: 329 tests passed, followed by protocol checks, package build,
  clean-venv wheel installation, and an end-to-end package smoke test.

The core checkpoint also passed a full Windows 2022 preflight before the final
tag. The published release workflows remain the authoritative platform and
artifact evidence:

- core release run `31820283530` passed on Ubuntu 22.04 and Windows 2022;
- CLI release run `31820277166` built and published Windows x86-64, macOS
  x86-64/ARM64, and Linux x86-64/ARM64 artifacts; and
- Python publish run `31820277012` built, tested, and smoke-tested Windows
  x86-64, macOS x86-64/ARM64, and Linux x86-64/ARM64 wheels, then published
  v0.10.5 to PyPI.

The default-branch core release workflow compatibility commit is `b4e83e0`.
It skips the later checked-artifact-only step for exactly v0.10.5 while keeping
the remaining historical release gates mandatory. The three disposable
`v0.10.5-preflight.*` tags were removed after the real release passed.

The v0.10.4 GitHub releases remain as immutable traceability points but are
marked superseded and must not be used.

## Development line retained for resumption

The development workspace remains on the later, unreleased line rather than
being reset or merged with the narrow release branch. At checkpoint creation
the important tuple was:

- workspace root: `d3cf7da35ed27ad40665391e1c843ec81e3789d5`;
- `gwz-core`: `abfbd4d7336debf0689c0f16bacf0a71d50e15ab`;
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- `gwz-py`: `929efb0d440206772155cadda89748e9431f9ca8`.

R2-C0 is accepted. R2-C1's aggregate classifier and native case-fold
correction received independent no-P0/P1/P2 acceptance. R2-C2 is implemented
far enough to expose the physical publication design but remains **NO-GO**.

## Exact R2-C2 blocker

The blocking review is
`GwzM5-8R2C2OwnerInterface-ReviewCode-2.md`. After the final source-object
verification, publication drops the verified handle and performs a
path-selected rename. A deterministic fault hook can replace the path in that
interval, causing foreign bytes to be moved into the authoritative destination
before the operation rejects them. The same verify/drop/path-rename seam must
be corrected for:

1. scratch to active;
2. staging to final;
3. active to retired; and
4. anchor moves.

Do not continue by layering another path re-check onto these edges. Resume by
designing one identity-preserving, handle-bound or otherwise substitution-proof
publication primitive, use it for every edge, then run interruption/recovery
tests for every declared fault key. The existing fault-key inventory alone is
not an executed fault matrix.

The resumed design resolved the cross-platform boundary in
`GwzM5-8R2CCatalogBootstrapAmendment.md` Section 4.1. One sealed primitive now
owns source acquisition, identity comparison, and publication. Windows must
rename the exact checked handle. Unix retains the checked handle across the
relative rename under an explicit cooperating-GWZ/same-user namespace trust
boundary because Linux and macOS expose no admitted portable atomic
compare-object-and-rename operation. This is the selected architectural
correction; it is not permission to add another unbound path check.

## Windows backlog for the v1 line

The accepted C1 tree was intentionally not released. Windows validation first
found two compile defects in checked-artifact code:

- native names were incorrectly formatted/compared as UTF-8 `String` values;
  isolated correction commit `a350746`; and
- unstable Windows metadata identity APIs were used instead of the existing
  stable no-follow Win32 identity helpers; isolated correction commit
  `d84a30d`.

Those corrections live in the disposable release-diagnostic clone and were
not blindly merged into the development line. With them applied, the Windows
suite compiled but reported 98 failures (1,187 passed, one ignored), mainly in
checked-artifact/v1 behavior, including Windows `os error 87` cases and tests
that assume `refs/heads/main`. Treat that matrix as a real implementation and
test-design backlog, not a reason to weaken the platform gate.

## Resume order

1. Reproduce and correct the R2-C2 post-verification publication race with one
   shared primitive and focused adversarial tests.
2. Execute the complete per-fault interruption/recovery matrix, then obtain two
   independent settled-tree reviews of R2-C2.
3. Port or supersede the two isolated Windows compile corrections deliberately.
4. Run Windows early and classify all remaining v1/checked-artifact failures
   before adding more catalog behavior.
5. Only after R2-C2 and the platform matrix are accepted, continue R4b-G,
   M5b, and A1 activation in the dependency order defined by
   `GwzM5-8Refactor.md`.

The narrow v0.10.5 release and the advanced development line therefore have
different histories by design. Preserve both tuples; do not merge the release
commit into R2-C2 merely to make the graphs look linear.
