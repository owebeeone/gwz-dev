# R4b R2 remediation — focused P3 test-maintenance confirmation

Date: 2026-08-14

## Verdict

**GO.**

P0: 0. P1: 0. P2: 0. P3: 0.

The sole core delta correctly repairs the eleventh code review's P3 test
maintenance finding. The non-target metadata regression now derives the
current package version, chooses a guaranteed-distinct valid replacement,
asserts that the fixture bytes actually changed, and still requires the real
compiler-root boundary checker to accept the modified manifest. No production,
state-machine, compiler-anchor, release, or static-gate implementation changed.

## Exact tuple reviewed

- workspace root: `9f3a23fb59b3e3c85502e9cd0844ede00d770411`
- `gwz-core`: `c2d09bb59f8e1ce1bb4e531983b50fcd8ba5a1ba`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Initial direct commit checks and `gwz status --json` matched this tuple. The
workspace and every member were clean, and every member matched its lock
entry. The tuple was reverified after this report was created; this report was
the review's only workspace write.

## Focused delta review

The complete core change from
`bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2` to the reviewed SHA is one file:

```text
M scripts/checks/test_check_checked_artifact_boundaries.py
```

At `scripts/checks/test_check_checked_artifact_boundaries.py:409-422`,
`test_compiler_root_manifest_allows_non_target_metadata_change` now:

1. reads the copied `Cargo.toml` into `before`;
2. parses `package.version` with `tomllib` rather than assuming `0.10.3`;
3. chooses `99.0.0` unless that is current, in which case it chooses
   `98.0.0`;
4. replaces the current version once and asserts `before != after`;
5. writes the changed temporary manifest; and
6. invokes the actual boundary checker and requires a zero exit status.

Both replacements are valid Cargo/SemVer versions and are distinct for every
possible current value: current `99.0.0` selects `98.0.0`; every other current
value selects `99.0.0`. If future manifest formatting prevents the intended
literal replacement, `assertNotEqual` fails rather than allowing the test to
become vacuous. The final `assertEqual(result.returncode, 0, result.stderr)`
continues proving the intended policy distinction: ordinary package version
metadata is accepted while `[lib]` remains fixed.

Adding `tomllib` is compatible with the already-required Python 3.11 CI
runtime. It is used only by this test module.

## Isolation from production and compiler anchors

`git diff --exit-code` confirmed no change in any of:

- `Cargo.toml`;
- `src/**`, including the production service and all v1 state/archive/
  preservation code;
- `.github/**`;
- `scripts/checks/check_checked_artifact_boundaries.py`;
- `scripts/checks/test_release_boundary.py`;
- `scripts/release.py`; or
- `tests/publish_workflow.rs`.

The existing compiler-parent digests still recompute exactly:

- `src/lib.rs`:
  `e035f8a53ddb589362972c85593cc0dff4b590129de38fe0fdb72ca1880f544e`;
- `src/workspace_ops/mod.rs`:
  `663b228d1f3fddc74853d3e26f9623a0d7d2009f172f53640697de35042a8124`;
- `src/workspace_ops/merge/mod.rs`:
  `d7b2662b8c18fa7d5b83e951a65be73498edcdc29565ca65e3f6d5cd069c1356`.

The production v1 service and boundary-checker files are likewise unchanged
from `bbbfd283`. Therefore the three-runtime production seal, test-only generic
entry, Cargo-to-v1 root chain, positive sentinel, preservation observation,
archive behavior, and release/static enforcement retain the already-reviewed
bytes and semantics.

## Focused checks

Run from `gwz-core`:

```text
git diff --check \
  bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2..c2d09bb59f8e1ce1bb4e531983b50fcd8ba5a1ba

PYTHONDONTWRITEBYTECODE=1 \
  python3 scripts/checks/check_checked_artifact_boundaries.py

PYTHONDONTWRITEBYTECODE=1 python3 -m unittest \
  scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_compiler_root_manifest_allows_non_target_metadata_change \
  -v

git diff --exit-code \
  bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2..c2d09bb59f8e1ce1bb4e531983b50fcd8ba5a1ba \
  -- Cargo.toml src .github \
  scripts/checks/check_checked_artifact_boundaries.py \
  scripts/checks/test_release_boundary.py scripts/release.py \
  tests/publish_workflow.rs
```

Results:

- diff whitespace check: passed;
- structural checked-artifact boundary: passed, reporting
  `ok (15 visible entries, 5 classified modules)`;
- exact repaired regression: 1/1 passed against the real checker; and
- production/compiler/release/static isolation diff: empty/pass.

A full Rust suite was intentionally not repeated because the delta is confined
to one Python test fixture and the prior exact production tuple already passed
the full suite. The focused test executes the complete structural checker on
its mutated temporary copy, which is the relevant behavior.

## Gate decision

The P3 maintenance issue is closed without changing any reviewed production or
compiler-boundary behavior. With no P0, P1, P2, or P3 finding, this focused
confirmation is **GO**.
