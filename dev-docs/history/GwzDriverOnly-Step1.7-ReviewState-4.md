# Step 1.7 driver-only construction: STATE-AXIS RE-CHECK (round 4, short form)
**Review object:** gwz-core `a681cf26`, amended from `25941ce9`, which was GO in `GwzDriverOnly-Step1.7-ReviewState-3.md`. Its parent is `b0ac8de7`. **Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p1-7`. Root `34251a9c` and gwz-transport `96938616` are unchanged. **Date:** 2026-10-30
**Axis:** State. Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewState-4.md`.
**Verdict: GO.** 0 P0, 0 P1, 0 P2, 0 P3 open. My round-3 GO carries to `a681cf26`. The `-d` Windows receipt is still queued, and the release gate stays open until it is green.

**Tuple.** Checked at the start and again at the end (21:34 AEDT): root `34251a9c`, gwz-core `a681cf26`, gwz-transport `96938616`. The gwz-core worktree was clean, and the tuple did not move.

## Prior-finding closure table

| ID | Status on `a681cf26` | Evidence |
|---|---|---|
| Round 1: P1-1 (two SCP classifiers led to native SSH), P2-1 (judged URL not equal to bound URL), P3-1 (scratch-directory ordering) | Closed and unchanged | No product file changed in `25941ce9..a681cf26`. |
| Round 2: P2-1 (identity selection on `scp_like`) and P3-1 (clone judge blind to `includeIf "gitdir:"`) | Closed and unchanged | No product file changed. The two conditional-include rows that hold round-2 P3-1 now run with a guarded fixture; see the next row. |
| Windows receipt `p1-7-20261030-c`: both `includeIf` rows failed (`clone file://`: left `GitCommandFailed`, right `UnsupportedOperation`) | **Fixed. Classification: fixture-only, confirmed.** | See the evidence list below this table. |

Evidence for the Windows receipt failure:

- **The rewrite never applied.** On dabeest, `rem3-probe2.log` tested eight `gitdir` spellings against a repository created in the temp directory. The fixture's spelling, built with `canonicalize()` (`gitdir://?/C:/…/hlifx0a1b/`), gave `matched=false`. The forms with `\\?\` stripped (`gitdir:C:/…/hlifx0a1b/`, plus `/i` and per-harness variants) gave `matched=true`. The 8.3 spellings (`HLIFX0~1`) did not match. `repo.path()` reports the long, non-verbatim form. So on Windows the include never applied, the clone kept `https://example.invalid/…`, and it failed at the fixture's unresolvable host (`GitCommandFailed`, fail-closed). The created-remote observation (`identity::refuse_unjudged_remote`) was never exercised, so the failure says nothing against the product.
- **That code is platform-neutral.** It compares `created.url()` with the judged URL, uses `local_only_route`/`bound_route`, and has no path handling, so a fixture that applies the rewrite exercises the same code on every platform.
- **Green runs.** After the fix, dabeest passed 24/24 focused rows (`rem3-green-dabeest.log`), and the Mac passed 24/24 (`rem3-mac-green.log`).

## Changed-range analysis (`25941ce9..a681cf26`)
- **One file changed:** `src/transport_host/driver_only_tests/rewrites.rs` (+60/−21). It is test-only, with no product, script or `cfg` change.
- **Shared helper.** Both conditional-include rows now call `include_rules_under_temporary(home, rules_path, rules)`. It does four things:
  - (a) It appends a marker key `gwzrules.conditional = applied` to the rules file.
  - (b) It spells the temp directory as libgit2 compares git directories, via `spelled_as_a_git_directory`: forward slashes, `//?/` stripped, and `//?/UNC/x` turned into `//x`.
  - (c) It writes `includeIf "gitdir/i:<dir>/"`. The case-insensitive form is needed because Windows paths are case-insensitive.
  - (d) It creates a probe repository under `std::env::temp_dir()` and asserts the marker is visible, so a pattern libgit2 does not match fails by name before the row runs.
- **Judgement.** The helper converts the vacuous-pass risk into a loud failure. The implementer's Mac mutant (`rem3-mac-mutant.log`) sets the condition to `gitdir/i:/Volumes/gwztmp/nomatch/`, and both rows fail with "the conditional include … was not applied … the rewrite under test would never be made".
  - The probe repository, the clone target (`harness.temp`, a `TempDir` under the same `temp_dir()`) and `read_remote_file`'s scratch `tempdir()` all live under the same root as the condition, so the marker proves the condition covers the repositories under test.
  - **Residual:** the marker proves the include applies to a repository under the temp root. It does not separately prove it applies to each target. Because all of them are under that root and the pattern is a prefix, this is sufficient.
  - The rows still run in the re-executed child with their own `HOME`/`USERPROFILE`, so there is no process-global mutation in the shared binary.
- **No new architectural root cause.** The receipt failure was a fixture path-spelling defect. The product's observation of created remotes is unchanged and was never reached in the failing run.

## Evidence (this round)
- **Read:** `git diff 25941ce9 a681cf26`, `dev-docs/GwzDriverOnly-Step1.7-RemPlan-3.md`, and the logs `rem3-probe2.log` (dabeest spelling matrix), `rem3-red-dabeest.log`, `rem3-green-dabeest.log`, `rem3-mac-green.log` and `rem3-mac-mutant.log`.
- **Run on the lane** (`CARGO_TARGET_DIR=…/p1-7-review-state`, `TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `GWZ_TEST_GIT=real`, `cargo +1.95.0 test --lib --locked -- driver_only route_tests`): **31 passed, 0 failed.** That includes all five `rewrites` rows; both conditional-include rows pass on macOS with the marker guard active.
- **Not run:** the `-d` Windows receipt (queued; not awaited, as instructed).

## Verdict
**GO on `a681cf26`.** The round-3 GO carries forward unchanged. The green `-d` Windows receipt is still required before release.
