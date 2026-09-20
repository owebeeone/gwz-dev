# N1/N2 native local-fetch correction and isolated Rust integration — CODE-AXIS REVIEW

**Review object:** N1/N2 implementation at core `01d6f6624472620c215693243f7ac3865aeb31a4`, git2-rs `4c1caabbce7d56426c763dd94114052302b23e4c`, libgit2 backport `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`, and upstream-facing libgit2 branch `fe618d0f5de9e506b9714643afc42d2fcba6e984`; acceptance pending, 2026-09-20  
**Baseline:** root `62c2f122c28ededdefb7af32058f4b058b018dcb`; core range `e45025d622c0d5993d4daba609a68d1cee463c45..01d6f6624472620c215693243f7ac3865aeb31a4`; git2-rs range `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68..4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 ranges `49e408b3208bc3093757a1c2db938d3590f3f412..b172e3d187a4b6866fd9f696f40a1b8e7f56d348` and `0551dfd4ad989b6a3d5683c0d4cf326c6efef929..fe618d0f5de9e506b9714643afc42d2fcba6e984`. Sources were read from immutable Git objects.  
**Date:** 2026-09-20  
**Axis:** Architecture, call graphs, compatibility, error paths, native-source admission, and proof integrity. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I inspected both complete C diffs, the final C files, commit parentage, revwalk/object/tag error paths, the Rust-fork diff, exact sys import, submodule gitlink and metadata, and all changed core proof files. I also read the accepted design’s execution evidence and the root checkpoint.

The two C commits have the required exact parents. Each changes only `local.c` and `fetchlocal.c`, with identical 243-line test additions. The test suffixes are byte-identical across branches. The Rust candidate directly descends from the accepted binding candidate.

I ran the permitted focused checks:

- `python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py`: 10 tests passed.
- `rustfmt +1.95.0 --check gwz-core/tests/transport_native/tests/local_fetch.rs`: passed.

No full C suite or Rust source/archive proof was rerun during this review. Their recorded commands, identities, red/green sequence, and results were inspected.

The tuple remained unchanged at the end. Final tree IDs were root `25a805a4c3218b4be09bed71670155d3d2637852`, core `cc17b7102c80fa06ae2a29dd1bbcd36570a3f979`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Core, git2-rs, and libgit2 were clean; root contained only the three explicitly excluded untracked owner prompts.

## 2. Invariant analysis

The production correction matches the accepted call graph. Both branches replace the impossible comparison with `GIT_ERROR_INVALID` by the two actual noncommittish returns, `GIT_EINVALIDSPEC` and `GIT_EPEEL`, while retaining existing `GIT_ENOTFOUND` suppression. No class or message matching was introduced, and allocation, parsing, pack, and publication failures outside that explicit set still propagate.

The C regression fixtures force real transfer: the requested commit is absent from the receiver while the receiver’s direct or annotated noncommit hint exists in the source. They cover tree/blob hints, tags ending in each, commit and tag-to-commit controls, a receiver-only object, explicit tree/blob/tag wants, parsed target-type mismatch, missing tag target, and syntax-malformed tag data. Destination OIDs and transferred objects are checked; hint refs remain unchanged. The malformed case asserts fetch-visible `GIT_ERROR`, tag-class detail, absent destination, and unchanged hints, closing the prior P3 oracle correction. Recorded pre-fix failures separately reached `GIT_EPEEL` and `GIT_EINVALIDSPEC`; the complete focused and offline suites then passed after the one-line correction.

The isolated Rust matrix independently distinguishes stock and fixed behavior. Each row constructs a fresh receiver, confirms the wanted commit is absent, places the same direct or annotated tree/blob hint in source and receiver, and checks stock’s exact Peel/InvalidSpec failure or the patched source’s successful destination and object transfer. Both paths assert libgit2 1.9.7 and vendored native code.

The N2 composition is exact. The four ordinary `libgit2-sys` files have no diff from upstream `6c93812dbc1c34aef6e6464a645545b4a4299807`; the fifth imported path is deliberately replaced by gitlink `b172e3d…`. Root Cargo selects path `=0.18.8`, `.gitmodules` selects the operator fork, and the accepted binding files retain their recorded hashes.

Source admission is fail-closed over the build inputs. It reads release, sys, and native trees through `--no-replace-objects`, recursive `ls-tree`, and validated batched `cat-file`; verifies parent gitlink and child HEAD; compares the checkout’s complete path set, bytes, executable modes, and symlink targets; then builds only an isolated copy. Missing, extra, dirty, substituted, or mismatched native/sys inputs fail before Cargo. The fixture enables `vendored-libgit2`, and the runtime tests reject any system-native build.

The lock guard permits only registry-to-path provenance removal for git2 and, in source mode, libgit2-sys. It requires exactly one package at the pinned version and byte-compares every remaining lock entry. Focused guards reject version, dependency, package, source, sys metadata, checkout, and native-tree drift. Archive mode retains the exact crate digest and registry sys; source mode patches both packages to the admitted copies. Existing binding tests remain in the same run.

Changed paths and actual line counts stay within the accepted ownership and 120% ceilings. No production manifest, fallback branch, CLI/core interface, protocol, publication, or activation path changed.

## 3. Risks and next action

The preserved `GIT_ENOTFOUND` ambiguity, unpublished fork commits, single-host evidence, and absent all-consumer/platform qualification remain explicit gates. This review establishes only the bounded native correction and isolated Rust integration.

Accept this N1/N2 candidate after the parallel axes return GO, then keep publication, type-consistency hardening, all-platform qualification, dependency activation, and fallback removal behind their recorded later reviews.
