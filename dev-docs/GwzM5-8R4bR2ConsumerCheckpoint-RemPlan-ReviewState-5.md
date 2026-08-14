# R4b R2 consumer-checkpoint remediation — fifth independent state/release re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

The fourth-review findings are substantially corrected. The protected adapter
boundary is now a positive byte-exact allowlist, the selected production
`preservation_stashes` route ends in a pinned read-only leaf, and the local
release script no longer exposes a compiler skip and does re-gate a newly
created version-bump commit.

Two P2 defects remain. The claimed sole-production-`GitBackend` check recognizes
only one unqualified spelling and accepts a compiled, publicly reachable second
backend whose preservation observer writes to disk. Separately, a new release
whose manifest already has the requested version reaches `ensure_tag` (and
optional push) without the exact-SHA gate used by the version-bump and
existing-tag branches.

P0: 0. P1: 0. P2: 2. P3: 0.

## Exact tuple reviewed

- workspace root: `4a060c0cb44eb2e5bfab80f49e54b46b873f2f7b`
- `gwz-core`: `b6f429a4842e2927e2629728f11207409b6901f8`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` and direct `git rev-parse HEAD` checks agreed on
this tuple at dispatch. I rechecked all three identities after inspection and
adversarial probes; they were unchanged and clean before this report was
written. This report is my only workspace edit. All mutation probes ran in a
detached temporary worktree of the exact core commit.

## Closed fourth-review findings

### Positive byte-exact containment: closed for the selected source set

I independently recomputed SHA-256 over all six protected files. Every raw-byte
digest exactly matches `PROTECTED_SOURCE_DIGESTS`:

- `checked_artifact/entry.rs`;
- `git/gitbackend.rs`;
- `git/gitbackend/preservation_root/files.rs`;
- `git/gitbackend/preservation_image.rs`;
- `workspace_ops/merge/preserve/checked_bundle.rs`; and
- `workspace_ops/merge/root/artifact_facts.rs`.

This is a complete positive check over the selected files, not a writer-name
denylist. Consequently, direct writers, imported or crate-local aliases,
same-name function pointers, nested owner/evidence helper writers, a changed
delegate, a changed helper, and even a commented-out module guard all alter
protected bytes and fail the source gate. The committed 25-test adversarial
boundary matrix passed, including each of those shapes. The executable
module-level `forbid` check masks comments and strings, so a commented guard
cannot satisfy it.

General `CheckedArtifact`, fact, policy, and transition capabilities remain
private to `checked_artifact`; callers see only purpose-specific entry
operations and facts. I found no new re-export or capability escape.

### Pinned preservation dispatch and read-only leaf: closed for `Git2Backend`

The byte-pinned `git/gitbackend.rs` dispatches
`preservation_stashes` directly to
`preservation_image::preservation_stashes`. The byte-pinned leaf validates the
merge ID, opens the repository, enumerates stash commits, reads commit parents,
trees, and blobs, decodes those objects with helpers in the same pinned source,
and returns evidence. The complete selected call path contains no filesystem or
Git mutation. A reroute or a new external helper changes protected bytes and is
rejected.

This conclusion is deliberately limited to the one production backend the
gate is supposed to make exclusive. Finding P2-1 shows that exclusivity itself
is not established.

### Archive semantic authority and capability contracts: no regression

Archive authority still comes only from the retained canonical open/archive
location pair. Acquisition rejects named-parent or leaf replacement and
rereads both leaves before returning. V0 authority requires an exact terminal
open source, absent destination, filename/record identity agreement, and
successful `decode_archived_v0` validation of the exact source bytes. The
test-only v1 authority uses the corresponding complete v1-capable decoder.
Owner identity and digest remain derived from those exact bytes rather than
independent caller input.

The focused archive matrix passed 24/24 and the coordinator-remediation matrix
passed 8/8, including the contradictory-terminal rejection and canonical
location contention cases. I found no route from a generic record observation
to archive authority.

### Fourth-review release defects: closed on the corrected branches

`release.py` exposes no `--no-clippy` or equivalent compiler bypass.
`run_checked_boundary_gates` always runs the source checker, its adversarial
tests, the release-boundary tests, and
`cargo clippy --all-targets --all-features -- -D warnings` with
`CLIPPY_CONF_DIR` set to the selected cargo root.

After a version/lock bump, the script commits, reacquires the new SHA, hard
resets a detached cargo worktree to that SHA, verifies equality, and reruns the
mandatory source/compiler gates before `ensure_tag`. The existing-tag retry
branch uses the same exact-commit helper before optional push. `--push` is
downstream of those branch-specific gates. The PR/main-push boundary workflow
and release workflow both run the structural tests and the all-target,
all-feature compiler gate. The seven static publish-workflow tests passed.

## Findings

### [P2-1] Qualified or aliased production `GitBackend` implementations evade the sole-backend gate

**Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:453-464`;
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:197-204`;
`gwz-core/src/git/gitbackend.rs:34,66-100`; and
`gwz-core/src/workspace_ops/merge/preserve/checked_bundle.rs:20-29,137-159`.

**Violated contract.** The remediation says the selected generic bundle
adapter is safe because there is exactly one production `GitBackend`, its
`preservation_stashes` delegate is pinned, and that delegate terminates in the
read-only pinned leaf. The implementation inventory searches only this literal
form:

```text
impl GitBackend for
```

Rust also accepts fully qualified, imported-alias, and macro-expanded trait
paths. The committed negative test exercises only the unqualified literal.

In an isolated copy of the exact core tree I added a public production unit
type under `gitbackend::backend`, imported
`crate::git::GitBackend as UncheckedGitBackend`, and implemented
`UncheckedGitBackend` for it. I forwarded the 25 required trait methods to
`Git2Backend`, then overrode `preservation_stashes` to execute
`std::fs::write` and return an empty evidence vector. The type is publicly
reachable through the existing `pub use backend::*` and `pub use gitbackend::*`
chain. The generic checked-bundle adapter accepts any `B: GitBackend` and calls
this overridable method.

Both mandatory checks nevertheless exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CLIPPY_CONF_DIR=. cargo clippy --all-targets --all-features -- -D warnings
Finished `dev` profile ...
```

The compiler permits the writer because the crate-level lint is allowed
outside the five guarded modules. Repeating the probe with a fully qualified
`impl crate::git::GitBackend for ...` also escaped the structural inventory.

**Impact.** A compiled, non-test, publicly constructible alternative backend
can make the supposedly read-only observer mutate a preservation root while
the source inventory and compiler gate remain green. It can be passed to the
generic checked-bundle path without changing any byte-pinned adapter or the
pinned `Git2Backend` delegate. Thus the concrete leaf is closed, but the trait
dispatch entering it is not.

**Required correction.** Inventory implementations by compiler-resolved trait
identity rather than an unqualified regex, or remove generic trait dispatch
from this authority-sensitive adapter and accept the sealed concrete observer.
At minimum, the gate must reject unqualified, fully qualified, imported-alias,
and macro-generated production implementations, with compiled negative
fixtures. A source-only regex expanded for several spellings is not a complete
Rust trait-implementation proof.

### [P2-2] The already-versioned new-tag path omits exact-target SHA proof before tag and push

**Where:** `gwz-core/scripts/release.py:435-477,479-505`;
`gwz-core/scripts/checks/test_release_boundary.py:25-65`; and
`gwz-core/tests/publish_workflow.rs:66-81`.

**Violated contract.** The release correction promises that no tag or push can
occur before the mandatory source/compiler gates have run on the exact target.
That is true for a newly created version-bump commit and an existing-tag retry.
It is not proved for a new tag when `Cargo.toml` already contains the requested
version.

On that branch `run_gates` runs, `bump_cargo_version` returns false, and control
flows directly to `ensure_tag(tag, head)`. There is no
`gate_exact_release_commit`, hard reset, or even final `rev-parse` equality
check. A mocked executable control-flow probe produced exactly:

```text
run_gates(...)
ensure_tag(v0.10.3, aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa)
```

and no exact-gate event. The static publish test uses `rfind` to show that *an*
exact-gate call occurs textually after the version commit and before the tag;
it does not execute or assert the false branch. The Python release tests cover
the helper in isolation but not `main` branch coverage.

This is observable without changing the release script: if `HEAD` changes
after the initial SHA capture, `make_standalone_worktree` independently checks
out the later `HEAD`; gates can pass on that later tree while `ensure_tag` tags
the earlier captured SHA. A standalone checkout has the symmetric race during
the long gate run. The optional atomic push then publishes the unproved target.

**Impact.** One supported release control-flow path can create and push a tag
whose target SHA was not the SHA on which the mandatory source tests and
all-target/all-feature Clippy ran. This violates the exact-tree property the
release amendment introduced; green workflow/static tests do not cover it.

**Required correction.** Make exact-target identity a postcondition on every
path before `ensure_tag`, preferably by moving one unconditional
`gate_exact_release_commit(cargo_root=cargo_root, expected_head=head)` after the
version branch and before tag creation. If avoiding a duplicate compiler run
on an unchanged tree is important, make `run_gates` bind and return the checked
SHA and verify that same SHA immediately before tagging. Add executable `main`
tests for new-tag/already-versioned, version-bump, existing-tag retry, and
`--push`, including deliberate SHA drift.

## Verification evidence

I ran the following focused evidence on the exact settled tuple:

- raw SHA-256 recomputation for all six protected sources: 6/6 exact;
- `python3 scripts/checks/check_checked_artifact_boundaries.py --source src`:
  passed;
- combined boundary and release Python matrix: 28/28 passed (25 boundary,
  three release);
- archive record-wire matrix: 24/24 passed;
- coordinator-remediation matrix: 8/8 passed;
- static publish-workflow tests: 7/7 passed;
- imported-alias/qualified second-backend probe with a raw writer: source
  checker incorrectly passed and exact all-target/all-feature Clippy also
  passed; and
- mocked already-versioned new-tag control-flow probe: reached `ensure_tag`
  without `gate_exact_release_commit`.

I did not rerun the long full-core suite solely for this review. The focused
checks exercise the corrected boundaries and both counterexamples directly.

## Gate decision

Do not start R2 production conversion on this tuple. Close trait-implementation
identity at the compiler/type level and make exact-SHA proof unconditional
before local tag creation or push. Preserve the now-closed positive source
allowlist, read-only concrete leaf, archive semantic authority, capability
privacy, and non-skippable compiler gate, then repeat both independent settled
re-reviews.
