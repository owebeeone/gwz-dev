# R4b R2 consumer-checkpoint remediation — sixth independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or A1 consumer activation.**

P0: 0. P1: 0. P2: 1. P3: 0.

Both FS-5 defects are corrected in their named forms: preservation-stash
observation is no longer an overridable `GitBackend` method, and the PR/push
workflow no longer imports `tomllib` under Python 3.10. The concrete observer
and its leaf are byte-pinned. The complete authority-sensitive v1 call graph is
not closed, however. An unprotected authority observer can insert a local raw
writer before calling the pinned observer; both the structural gate and
all-target/all-feature Clippy accept the compiling change.

## Exact tuple reviewed

- workspace root: `d981236a5127d1a633d5535e030052dc9d176391`
- `gwz-core`: `a2d9c00d80cc39f7a66c0a8ddc77411febbdcda9`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean before review. The
adversarial mutation was made only in an exact-commit export under `/tmp`; no
reviewed production source was changed.

## FS-5 findings re-tested

### FS-5 P2-1: open backend observer — closed in its named form

`GitBackend` no longer defines `preservation_stashes`
(`git/gitbackend/contract.rs:450-478`). A public qualified or imported-alias
alternative backend therefore cannot override that observation. The remaining
ordinary/test convenience method is inherent to `Git2Backend`, not part of the
open trait (`git/gitbackend.rs:150-159`).

Authority-sensitive merge callers on the reviewed tree use the crate-private
`observe_preservation_stashes_read_only` function. That function is in
digest-protected `gitbackend.rs` and calls the digest-protected
`preservation_image::preservation_stashes` leaf directly, without trait
dispatch (`gitbackend.rs:161-175`). The leaf validates the merge ID, enumerates
stash objects, decodes commits/trees/blobs, and performs no successful write.

The checker pins the concrete function body, rejects restoration of a method
named `preservation_stashes` in the trait contract, and rejects direct use of
that open method name in merge code
(`check_checked_artifact_boundaries.py:442-469`). The five focused committed
tests covering current inventory, concrete routing, open-method reintroduction,
merge re-entry, and protected-leaf redirection all passed.

This closes the specific qualified-backend counterexample from FS-5, but not
the complete caller graph; see P2-1.

### FS-5 P2-2: Python runtime mismatch — closed

The PR/push workflow now selects Python 3.11 before running all three boundary
commands and then Clippy (`checked-artifact-boundary.yml:19-31`). Python 3.11
contains `tomllib`, so the prior Python-3.10 import failure is removed. The
same release-boundary suite ran 6/6 under the available Python 3.12 workspace
runtime, and the workflow-contract test pins `python-version: "3.11"` and the
subsequent Clippy command. There is no control-flow skip between the Python
commands and Clippy.

## Release finalizer audit — accepted

The release correction satisfies the reviewed exact-target properties:

- `gate_exact_release_commit` resets an external worktree to the expected SHA,
  checks SHA and cleanliness before the mandatory gates, and repeats both
  checks afterward (`release.py:372-400`);
- every new-tag path, including the no-version-change path, converges on
  `finalize_new_release`, which gates before creating the tag or pushing it
  (`release.py:403-413,518-546`);
- an existing-tag retry verifies tag, branch HEAD, and package version, then
  performs the same exact-target gate before an optional push
  (`release.py:505-510`); and
- atomic push uses the verified SHA as the explicit source for both
  `refs/heads/<branch>` and `refs/tags/<tag>`, so later local branch or tag
  movement cannot substitute an ungated object (`release.py:322-346`).

The six focused release tests passed, including pre/post SHA drift, finalizer
ordering, and exact atomic refspec assertions. The seven publish-workflow tests
passed. I found no P0/P1/P2 defect in this release finalizer.

## Finding

### [P2-1] Unprotected v1 authority observers can wrap the pinned reader with a raw writer

**Where:**

- `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:14-35,430-469`
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs:160-181`
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase/evidence.rs:18-31`

**Violated contract.** RemPlan sections 7-9 require every authority-sensitive
v1 preservation observation to route through the crate-private byte-pinned
reader and protected leaf without a direct, aliased, nested-helper, or wrapper
writer. Exit criterion 8 explicitly includes an unlisted alias and crate-local
wrapper.

Only `gitbackend.rs`, `preservation_image.rs`, and the checked-bundle adapter
are digest-protected for this observer path. Other authority-sensitive v1
observers call the concrete reader from unprotected modules. The checker only
searches those modules for the exact identifier `preservation_stashes`; it does
not freeze their call graphs or put them under the module-level Clippy forbid.

In an exact exported tree, I changed `observe_plain_stash` in
`v1_lifecycle/.../preservation/phase.rs` to call a new local helper. The helper
used a same-name function pointer to perform a raw write and then called the
pinned observer:

```rust
fn unreviewed_observe_preservation_stashes_read_only(
    path: &std::path::Path,
    merge_id: &str,
) -> ModelResult<Vec<crate::git::GitPreservationStashEvidence>> {
    let map_transition = std::fs::write;
    let _ = map_transition(path.join("raw-authority-writer"), b"bypass");
    crate::git::observe_preservation_stashes_read_only(path, merge_id)
}
```

The selected v1 authority path therefore still reaches the required pinned
reader and leaf, but first performs an unchecked successful write. Both
mandatory gates accepted the compiling counterexample:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
Finished `dev` profile [unoptimized + debuginfo] ...
```

This code is presently behind the test-gated v1/A1 module, but it is exactly
the authority-sensitive A1 code this checkpoint is intended to freeze before
activation. Deferring the issue would make the boundary claim false at its
activation gate.

**Recommended correction.** Close the complete authority-sensitive observation
graph, not only the final reader. Either move all stash-evidence interpretation
behind one byte-pinned concrete facade whose callers receive completed typed
facts, or add every authority observer and its transitive local helpers to the
positive source/compiler boundary. Add a compiling regression that inserts a
local/helper writer before the legitimate concrete-reader call; merely banning
the old method name does not exercise this bypass.

## Focused evidence

Reviewed-tree commands included:

```text
gwz status --json
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD

../.venv/bin/python scripts/checks/check_checked_artifact_boundaries.py
../.venv/bin/python -m unittest scripts/checks/test_release_boundary.py -v
cargo test --test publish_workflow
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings

../.venv/bin/python -m unittest -v \
  ...test_current_source_inventory_is_classified \
  ...test_checked_bundle_observer_does_not_dispatch_through_open_backend \
  ...test_open_backend_observer_cannot_reenter_merge_authority \
  ...test_preservation_observer_cannot_reenter_the_open_trait \
  ...test_production_observer_delegate_cannot_leave_its_protected_leaf
```

Results: source checker passed; release boundary 6/6; publish workflow 7/7;
selected structural tests 5/5; reviewed-tree all-target/all-feature Clippy
passed. The exact-export adversarial source checker and Clippy also passed,
which is the P2 failure evidence. Per task scope and the parent's prior full
suite evidence, I did not rerun the long complete core suite.

## Exit decision

The sixth correction cannot receive GO until the complete authority-sensitive
v1 observation graph is fail-closed against a wrapper or pre-observation raw
writer. Commit that correction on a new exact tuple and repeat both independent
settled-tree reviews.
