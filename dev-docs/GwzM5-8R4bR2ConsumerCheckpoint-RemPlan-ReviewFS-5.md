# R4b R2 consumer-checkpoint remediation — fifth independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 2. P3: 0.

The fourth-review archive and release-boundary defects are closed, and the
selected `Git2Backend::preservation_stashes` implementation now terminates in
the frozen read-only leaf. The claimed single-production-backend boundary is
still bypassable, however: the implementation inventory recognizes only the
literal spelling `impl GitBackend for`. A fully qualified implementation can
override the selected observer with a raw writer, compile cleanly, pass both
mandatory gates, and enter the public generic merge path. In addition, the new
release-boundary test makes the pinned Python-3.10 PR/push workflow fail before
Clippy because `release.py` imports the Python-3.11-only standard-library
`tomllib` module.

## Exact tuple reviewed

- workspace root: `4a060c0cb44eb2e5bfab80f49e54b46b873f2f7b`
- `gwz-core`: `b6f429a4842e2927e2629728f11207409b6901f8`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean before review. All
adversarial mutations were made in exact-commit exports under `/tmp`; no
reviewed production source was changed.

## Closed fourth-review findings

### Complete positive source allowlist and concrete observer leaf

The checker now hashes all six guarded source files, masks comments before
testing the non-overridable lint, pins the delegate, and attempts to inventory
the production backend implementation set
(`scripts/checks/check_checked_artifact_boundaries.py:430-464`). I independently
changed one byte in each digest-protected file; every change was rejected:

```text
rejected byte change: checked_artifact/entry.rs
rejected byte change: git/gitbackend.rs
rejected byte change: git/gitbackend/preservation_root/files.rs
rejected byte change: git/gitbackend/preservation_image.rs
rejected byte change: workspace_ops/merge/preserve/checked_bundle.rs
rejected byte change: workspace_ops/merge/root/artifact_facts.rs
```

The committed adversarial suite also rejects the named same-name standard and
crate-local aliases, a raw writer inserted into the concrete observer,
delegate redirection, an unqualified second backend implementation, and a
commented-out forbid marker. Its 25 tests passed on the reviewed source.

The selected production call is now:

```text
checked_bundle::expected_bundle
  -> B::preservation_stashes
  -> Git2Backend delegate in gitbackend.rs
  -> preservation_image::preservation_stashes
  -> decode_stashes
  -> decode_stash
  -> flatten_tree / encode
```

For `Git2Backend`, this graph terminates inside the digest-protected
`preservation_image.rs` leaf (`gitbackend.rs:100`,
`preservation_image.rs:285-455`). It opens the repository, enumerates stash
objects, reads commits/trees/blobs, constructs in-memory evidence, and does not
call a raw writer or re-enter the open trait. This closes the concrete-leaf
part of FS-4 P2-1, subject to the implementation-set bypass in P2-1 below.

The digest table and checker deliberately live in the same reviewed repository.
Updating a guarded source and its recorded digest together is therefore a
review-visible allowlist update, not an independent cryptographic trust
anchor. I reproduced that property but do not classify it as a defect under
the stated review-control threat model.

### Exact release target and non-skippable compiler gate

`release.py` exposes no `--no-clippy` option. `gate_exact_release_commit`
reacquires/verifies the expected SHA and always calls the source tests plus
all-target/all-feature Clippy (`release.py:339-374`). Existing-tag retries call
that exact gate before push (`release.py:466-471`); a new version commit is
read back by SHA and gated before `ensure_tag` and push
(`release.py:479-505`).

An independent mocked control-flow probe reported:

```text
existing-tag: exact gate before push
new-version: exact new-commit gate before tag before push
```

The committed release-boundary tests pass under the workspace Python 3.12
environment. PR/push and release CI both contain source-test and Clippy steps
against their checked-out tree (`checked-artifact-boundary.yml:6-31`,
`release.yml:72-82`). This closes FS-4 P2-2's skip and tag-order paths; the
Python-version wiring defect is reported separately below.

## Findings

### [P2-1] A fully qualified production `GitBackend` implementation bypasses the single-backend gate and can enter the checked path

**Where:**

- `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:453-464`
- `gwz-core/src/workspace_ops/merge/runtime/dispatch.rs:25-51`
- `gwz-core/src/workspace_ops/merge/preserve/checked_bundle.rs:137-159`
- `gwz-core/src/git/gitbackend/contract.rs:471-478`

**Violated contract.** RemPlan sections 7-9 require the checker to reject every
production `GitBackend` implementation other than the frozen `Git2Backend`, so
an accepted observer call cannot dispatch outside the frozen read-only leaf.

The checker counts only this textual form after masking comments:

```python
re.findall(r"\bimpl\s+GitBackend\s+for\b", text)
```

Rust permits the semantically identical fully qualified form
`impl crate::git::GitBackend for UnreviewedBackend`. In an exact exported tree I
added a public wrapper backend using that form, delegated its required methods
to `Git2Backend`, and overrode `preservation_stashes` with:

```rust
std::fs::write(path.join("unreviewed-writer"), b"bypass")
    .map_err(|error| ModelError::new(ErrorCode::IoError, error.to_string()))?;
self.0.preservation_stashes(path, merge_id)
```

Both mandatory gates accepted the tree:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
Finished `dev` profile ...
```

The counterexample is production-reachable rather than merely an unused extra
type. `handle_merge` and `handle_merge_with_events` are public and generic over
any `B: GitBackend` (`runtime/dispatch.rs:25-51`), while the guarded bundle
adapter dynamically calls `backend.preservation_stashes(...)`
(`checked_bundle.rs:157-159`). A caller can therefore pass this public backend
to the real merge service and cause the raw writer on the selected checked
path. Clippy does not reject it because the override is outside the five
compiler-protected modules.

**Recommended correction.** Do not infer trait implementation identity with a
single spelling-sensitive regex. Prefer sealing the production backend path or
making the checked adapter consume a concrete read-only observer capability
whose only constructor/implementation is private and frozen. If the source
gate remains a supporting check, recognize qualified/import-aliased trait paths
and add compiling, public, selected-path counterexamples—not only an empty
unqualified implementation that cannot compile.

### [P2-2] The PR/push boundary workflow is pinned to Python 3.10 but now imports `tomllib`

**Where:**

- `gwz-core/.github/workflows/checked-artifact-boundary.yml:19-31`
- `gwz-core/scripts/checks/test_release_boundary.py:10-18`
- `gwz-core/scripts/release.py:44`

**Violated contract.** RemPlan exit criteria 10-11 require the source and
compiler gates to run successfully in PR/push CI on the settled tree.

The PR/push workflow explicitly installs Python 3.10, then runs
`test_release_boundary.py`. That test imports `release.py`, which imports
`tomllib`; `tomllib` entered the standard library in Python 3.11. Running the
committed workflow command with the local Python 3.10.15 reproduced:

```text
test_release_boundary (unittest.loader._FailedTest) ... ERROR
ModuleNotFoundError: No module named 'tomllib'
Ran 1 test in 0.000s
FAILED (errors=1)
```

Consequently the PR/push boundary job fails before its Clippy line on every
ordinary Python-3.10 runner. This is fail-closed, not a release bypass, but it
breaks the mandatory CI gate and blocks all pushes/PRs that require it.

**Recommended correction.** Either pin this workflow to Python 3.11 or newer,
matching the release script's actual requirement, or use a supported 3.10
compatibility import/dependency. Add a workflow-contract assertion that the
configured Python version can import and execute all three boundary commands.

## Focused command evidence

Reviewed-tree and isolated commands included:

```text
gwz status --json
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD

python scripts/checks/check_checked_artifact_boundaries.py
python -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings

../.venv/bin/python -m unittest scripts/checks/test_release_boundary.py -v
python -m unittest scripts/checks/test_release_boundary.py -v   # Python 3.10 reproduction
../.venv/bin/python /tmp/fs5_release_order_probe.py
../.venv/bin/python /tmp/fs5_digest_probe.py
```

The committed structural suite passed all 25 tests. The reviewed tree's source
checker passed. All-target/all-feature Clippy passed on the qualified-backend
counterexample, which is the P2-1 failure evidence. The release-boundary suite
passed 3/3 under Python 3.12 and failed at import under the workflow's Python
3.10, which is the P2-2 evidence. Per review scope, I did not rerun the full
core suite solely for this settled re-review.

## Exit decision

The correction cannot receive GO while the public checked path can dispatch to
an uncounted production backend override and the required PR/push workflow is
not executable with its configured Python version. Remediate both P2 findings,
commit a new exact tuple, and repeat the two independent settled-tree reviews.
