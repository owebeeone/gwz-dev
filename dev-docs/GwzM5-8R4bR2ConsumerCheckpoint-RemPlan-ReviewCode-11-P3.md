# R4b R2 consumer-checkpoint remediation — code review 11 P3 follow-up

Date: 2026-08-14

## Verdict

**GO. The prior P3 is closed.**

P0: 0. P1: 0. P2: 0. P3: 0.

The focused core delta makes the non-target Cargo metadata regression
self-maintaining. It parses the copied manifest's current package version,
chooses a guaranteed-distinct valid replacement, asserts that the fixture
bytes changed, and only then runs the boundary checker. The test can no longer
pass vacuously after a release bump. No production Rust, checker behavior,
compiler-root manifest, workflow, or release code changed.

## Exact tuple reviewed

- workspace root: `9f3a23fb59b3e3c85502e9cd0844ede00d770411`
- `gwz-core`: `c2d09bb59f8e1ce1bb4e531983b50fcd8ba5a1ba`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Direct `git rev-parse HEAD` checks matched this tuple before inspection. The
same three IDs were reverified after this report was written. The checkout was
initially clean, and this report is my only workspace write.

## Focused delta review

The exact core delta from
`bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2` to the reviewed core SHA changes
only `scripts/checks/test_check_checked_artifact_boundaries.py`: 9 insertions
and 6 deletions. It does not change production code or the structural checker.

At `test_check_checked_artifact_boundaries.py:409-422`, the corrected test now:

1. reads the copied `Cargo.toml` into `before`;
2. parses `package.version` with `tomllib`;
3. selects `99.0.0` unless that is already current, in which case it selects
   `98.0.0`;
4. replaces the exact current package version once;
5. asserts `before != after` before writing the fixture; and
6. requires the boundary checker to return success.

For every possible current version, the conditional replacement differs from
it: a current `99.0.0` maps to `98.0.0`, while every other version maps to
`99.0.0`. Both replacements are valid semantic versions. If Cargo formatting
or manifest structure changes so the textual substitution cannot be made,
`assertNotEqual` fails rather than allowing a false positive. The test still
changes only `[package].version`; the separate negative regression continues
to prove that `[lib].path` redirection is rejected.

The new `tomllib` import creates no new compatibility issue for this gate.
`check_checked_artifact_boundaries.py` already imported and required
`tomllib`, and the checked-artifact workflow explicitly uses Python 3.11.
Release CI uses Python 3.x and invokes the same checker and test suite.

## Prior finding disposition

ReviewCode-11 P3-1 found that the old test hard-coded
`version = "0.10.3"` and did not prove its replacement occurred. After a
future release bump, that test could have run the checker against an unchanged
fixture and passed vacuously.

The current implementation addresses both parts of that finding:

- it derives the old value from the parsed current manifest rather than a
  release-specific literal; and
- it asserts that the before/after manifest bytes differ before accepting the
  checker result.

The prior P3 is therefore closed. I found no new correctness, compatibility,
or test-maintenance issue in the focused delta.

## Commands and results

- `git status --short`; root/core/CLI `git rev-parse HEAD`: initially clean and
  exactly matched the requested tuple.
- `git -C gwz-core diff --name-only bbbfd283...c2d09bb`: reported only
  `scripts/checks/test_check_checked_artifact_boundaries.py`.
- `git -C gwz-core diff --numstat bbbfd283...c2d09bb`: reported 9 insertions
  and 6 deletions.
- Core and workspace-root `git diff --check HEAD^..HEAD`: passed.
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed with
  `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`.
- `python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_compiler_root_manifest_allows_non_target_metadata_change -v`:
  passed 1/1 in 3.254 seconds.
- A direct two-branch logic probe confirmed `0.10.3 -> 99.0.0` and
  `99.0.0 -> 98.0.0`, with inequality asserted in both cases.
- Final root/core/CLI commit checks: matched the exact requested tuple. Final
  status contained this code report and a concurrently created independent
  `ReviewState-11-P3` report; this code report remained my only workspace
  write.

One compound inspection command was initially issued from inside `gwz-core`
with a redundant `git -C gwz-core`, so that trailing inspection returned 128.
It was rerun from the workspace root and passed; the checker and focused unit
test in the same earlier invocation had already completed successfully.

Per the focused-review request, I did not repeat the long full Rust suite.
Production Rust is byte-identical to the previously reviewed core checkpoint.

## Gate decision

The P3 remediation is complete and introduces no new P0-P3 finding. This
focused exact-tuple follow-up is **GO**.
