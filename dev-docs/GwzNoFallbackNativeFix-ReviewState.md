# Native local-fetch correction and isolated Rust integration — STATE-AXIS REVIEW

**Review object:** N1/N2 implementation at core `01d6f6624472620c215693243f7ac3865aeb31a4`, git2-rs `4c1caabbce7d56426c763dd94114052302b23e4c`, libgit2 backport `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`, and upstream-facing libgit2 commit `fe618d0f5de9e506b9714643afc42d2fcba6e984`; acceptance pending, 2026-09-20  
**Baseline:** root `62c2f122c28ededdefb7af32058f4b058b018dcb`; core range `e45025d622c0d5993d4daba609a68d1cee463c45..01d6f6624472620c215693243f7ac3865aeb31a4`; Rust range `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68..4c1caabbce7d56426c763dd94114052302b23e4c`; C ranges `49e408b3208bc3093757a1c2db938d3590f3f412..b172e3d187a4b6866fd9f696f40a1b8e7f56d348` and `0551dfd4ad989b6a3d5683c0d4cf326c6efef929..fe618d0f5de9e506b9714643afc42d2fcba6e984`. All source was read from immutable Git objects.  
**Date:** 2026-09-20  
**Axis:** State — failure-state classification, write/publication ordering, recovery grammar, source isolation, submodule provenance, and lock-graph integrity. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I inspected:

- The accepted design and its execution-evidence section, including package boundaries, recorded red/green results, source identities, budgets, and deferrals.
- Both C diffs and their parents. Each changes the same single condition in `foreach_reference_cb` and adds the same 243-line Clar test patch; their stable patch IDs match.
- The complete new Clar fixtures in `tests/libgit2/network/fetchlocal.c`, plus the pinned `local_download_pack` ordering and revwalk/object/tag error paths.
- The Rust candidate’s full changed range: root manifest, `.gitmodules`, four ordinary upstream `libgit2-sys` files, and the C gitlink.
- Core’s proof runner, ten Python guards, lockfile and pins, fixture manifest, Rust local-fetch test, and README.
- Exact upstream sys comparison against `6c93812dbc1c34aef6e6464a645545b4a4299807`. The four ordinary imported files are byte-identical; the fifth sys path is the intentionally replaced C gitlink.
- Binding interactions only: the candidate does not change the previously accepted two binding files, and their recorded hashes remain unchanged.

I ran the two State-permitted focused checks:

- `python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py`: **10 passed**.
- `rustfmt +1.95.0 --check gwz-core/tests/transport_native/tests/local_fetch.rs`: **passed**.

I did not rerun C or Cargo builds. I inspected the committed evidence recording successful focused/full C suites on both branches and successful eight-test source/archive proof runs.

The exact tuple matched at start and end. Final trees were root `25a805a4c3218b4be09bed71670155d3d2637852`, core `cc17b7102c80fa06ae2a29dd1bbcd36570a3f979`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. The initialized nested C checkout was exactly `b172e3d...` and clean. Root contained only the three explicitly allowed untracked review prompts.

## 2. Invariant analysis

The native correction preserves the accepted error grammar. `GIT_ENOTFOUND`, `GIT_EINVALIDSPEC`, and `GIT_EPEEL` are the only cleared results. On these pinned sources, direct trees/blobs produce `GIT_EINVALIDSPEC`, correctly parsed tags ending in trees/blobs produce `GIT_EPEEL`, and the two documented stock cases retain `GIT_ENOTFOUND`. Syntax-malformed tags return fetch-visible `GIT_ERROR` with tag error detail and are not masked.

The failure ordering is fail closed. Receiver references are classified before `git_odb_write_pack` is created. The malformed-tag test therefore fails before pack installation; it also asserts that the wanted destination remains absent and both existing hint refs retain their original OIDs. Successful tests force a genuinely absent wanted commit, verify the transferred commit and destination ref, preserve all receiver hints, and include direct/annotated tree and blob hints, commit controls, receiver-only objects, missing targets, and mismatched tag target types. Explicit tree, blob, and annotated-tag wants remain transferable and their target objects are present afterward.

The Rust four-row test independently forces one hint form per repository pair. Stock archive mode must return the exact `InvalidSpec` or `Peel` failure with no destination, while patched source mode must publish the wanted commit. It also asserts libgit2 1.9.7 and vendored linkage, preventing a system library from silently satisfying the oracle.

Source admission is closed around immutable identities. The runner reads release, sys, and C trees through `ls-tree` and batched `cat-file`, validates returned object IDs/types/lengths, checks modes and complete checkout file sets, verifies both parent gitlink and nested C `HEAD`, and copies the already-read bytes into temporary isolation. Dirty, extra, missing, ignored, symlink-substituted, wrong-sys, wrong-C, or mismatched metadata states fail before Cargo. Root and nested Git metadata and retained build output are excluded from the admitted copy and cannot become build inputs.

Cargo source selection is also closed. Source mode supplies explicit path overrides for both git2 and `libgit2-sys`; the fixture enables vendored libgit2. Lock normalization requires exactly one git2 and one sys package at the pinned versions, permits removal of provenance only for those packages, and byte-compares every other package and dependency entry. Archive mode retains registry sys provenance. The checked-in lock and binding patch remain digest-pinned.

N1 precedes N2, the backport gitlink selects the reviewed C commit, and no production manifest, fallback path, runtime owner, or ref-publication contract changed.

## 3. Risks and next action

The preserved `GIT_ENOTFOUND` handling still accepts missing tag targets and declared/actual target-type mismatches. Type-consistency hardening remains mandatory before fallback removal and is not closed by this verdict. Publication, clean remote-only reproduction, all-consumer/platform qualification, broader cancellation and atomicity requirements, and production activation also remain pending. Current execution evidence is local macOS arm64 qualification, and both fork commits remain unpublished.

Accept this bounded N1/N2 implementation. The next gate must preserve the exact reviewed source identities and treat any publication, native pin, dependency, or fallback-removal change as a separate reviewed object.
