# R4b R2 consumer-checkpoint remediation — sixth independent state/release re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

Both specific State-5 defects are corrected. Authority-sensitive stash
observation no longer dispatches through `GitBackend`, and every release path
that creates or pushes a tag now uses an exact clean-tree gate and explicit
gated-SHA push refspecs.

One P2 containment defect remains. Three authority-sensitive merge callers of
the concrete observer are outside both the positive source allowlist and the
module-level compiler boundary. A raw writer inserted immediately before the
observer in the v1 authority/evidence caller passes the structural checker and
all-target/all-feature Clippy. Freezing the observer and leaf therefore does not
freeze the complete authority path that consumes their result.

P0: 0. P1: 0. P2: 1. P3: 0.

## Exact tuple reviewed

- workspace root: `d981236a5127d1a633d5535e030052dc9d176391`
- `gwz-core`: `a2d9c00d80cc39f7a66c0a8ddc77411febbdcda9`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed workspace status and direct `git rev-parse HEAD` checks agreed on
this tuple at dispatch and again after inspection and executable probes. The
core and CLI trees remained clean. An independent filesystem State-6 report
appeared later as an untracked root document; it changed no reviewed source or
commit identity. I independently reproduced the finding below in a detached
temporary worktree of the exact core SHA. This report is my only workspace
edit.

## State-5 findings re-tested

### Open backend observer substitution: closed in the current source

`GitBackend` no longer declares `preservation_stashes`. The production
`Git2Backend` retains only an inherent method for ordinary and test use.
Authority-sensitive merge code instead calls the crate-qualified
`crate::git::observe_preservation_stashes_read_only` function, whose
byte-pinned implementation directly enters the byte-pinned
`preservation_image` leaf.

I enumerated the production merge call sites:

- `workspace_ops/merge/preserve/checked_bundle.rs`;
- `workspace_ops/merge/preserve/plan.rs`;
- `workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs`; and
- `workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase/evidence.rs`.

All four use the crate-qualified concrete observer. Remaining method calls to
`Git2Backend::preservation_stashes` are in Git tests; remaining direct
`decode_stashes` calls are internal to the concrete Git implementation. A
fully qualified, import-aliased, or macro-generated alternative
`GitBackend` implementation cannot override or shadow the crate-qualified
function. The concrete observer and its leaf contain no backend callback and
only enumerate/read stash commits, trees, and blobs with local decoding
helpers.

The structural tests correctly reject restoration of the old method name to
the trait or a production merge caller, and reject changing the concrete
observer or its pinned leaf. This closes the exact State-5 alternative-backend
counterexample. It does not close the surrounding caller modules; that is the
new finding below.

### Release exact-target convergence: closed

Every new tag now converges through `finalize_new_release`, which runs
`gate_exact_release_commit`, creates the tag at the explicit expected SHA, and
only then optionally pushes. The existing-tag retry separately runs the same
exact gate before optional push.

The exact gate:

1. hard-resets a detached cargo worktree to the expected SHA;
2. requires that SHA and a clean tree before the mandatory boundary suite;
3. runs the source checker, adversarial source tests, release-flow tests, and
   all-target/all-feature Clippy with the selected tree as `CLIPPY_CONF_DIR`;
4. reacquires the SHA and clean status after the suite; and
5. rejects pre- or post-gate drift and dirt.

I drove `main` under mocks for all six relevant branches. The observed event
orders were:

| Release form | Without `--push` | With `--push` |
| --- | --- | --- |
| existing tag at HEAD | exact gate | exact gate, push captured SHA |
| new tag, version already current | initial gates, exact gate, tag captured SHA | initial gates, exact gate, tag captured SHA, push same SHA |
| new version commit | initial gates, version test/commit, exact gate new SHA, tag new SHA | initial gates, version test/commit, exact gate new SHA, tag new SHA, push new SHA |

Independent gate probes established that pre-gate SHA drift and pre-gate dirt
reject before the boundary suite, while post-gate SHA drift and post-gate dirt
reject after one suite execution. A clean detached tree resets first and passes
both identity checks. The no-push form creates only the already-gated local tag.

`push_release` no longer names moving local refs. The captured command was
equivalent to:

```text
git push --atomic origin \
  <gated-sha>:refs/heads/main \
  <gated-sha>:refs/tags/v1.2.3
```

Thus local branch or tag movement after the gate cannot substitute another
source object, and remote rejection leaves both refs unchanged.

### Python and workflow gates: closed

The PR/main-push boundary workflow now selects Python 3.11, which supplies
`tomllib` when the release-boundary tests import `release.py`. I ran the exact
declared source and release suite with CPython 3.11.14: the source checker
passed and all 33 Python tests passed. The workflow still runs the same
all-target/all-feature Clippy command. Release CI installs Python `3.x`, runs
the same source/release checks on the checked-out release tag, then tests and
Clippy. The seven static publish-workflow assertions passed.

No compiler skip is exposed. Archive semantic authority, checked capability
privacy, owner/purpose binding, and canonical-location acquisition were not
changed by this correction; inspection found no regression in those contracts.

## Finding

### [P2-1] The frozen observer does not contain its authority-sensitive callers

**Where:** remediation section 7 and exit criterion 8
(`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`);
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py` protected-source
and open-observer checks; and
`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase/evidence.rs:17-47`.

**Violated contract.** The remediation says every authority-sensitive merge
observation is contained by the concrete protected observer. The observer and
leaf are protected, and `checked_bundle.rs` is protected, but these production
callers are not:

- `preserve/plan.rs`;
- `v1_lifecycle/.../preservation/phase.rs`; and
- `v1_lifecycle/.../preservation/phase/evidence.rs`.

They have neither a positive source digest nor
`#![forbid(clippy::disallowed_methods)]`. The checker searches those files only
for the exact standalone token `preservation_stashes`; it does not inventory
their complete bytes, imports, calls, helpers, or code surrounding the concrete
observer call. The crate root deliberately allows `disallowed_methods` outside
the selected guarded modules.

In a detached copy of the exact core commit, I inserted this immediately before
the concrete observer in `phase/evidence.rs`:

```rust
std::fs::write(plan.path.join("unchecked-authority-write"), b"bypass")
    .map_err(crate::git::io_error)?;
```

The two mandatory checks both exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CLIPPY_CONF_DIR=. cargo clippy --all-targets --all-features -- -D warnings
Finished `dev` profile ...
```

The inserted code is production-compiled. This is not an observer override:
it is a raw successful mutation in the authority path immediately before the
fixed read. The same unprotected caller can write immediately after observation
but before it converts those facts into durable preservation evidence. It can
also introduce a differently named open backend callback because the checker
bans only the removed method's old token rather than positively pinning the
caller graph.

**Impact.** An authority-sensitive preservation plan, recovery observation, or
evidence-completion path can mutate an arbitrary filesystem object outside the
purpose-specific checked entry while every PR, push, local-release, and release
CI boundary remains green. The concrete leaf proves only what its own function
does; it does not prove that the authority action which calls and consumes it
has no adjacent writer or substituted observation.

**Required correction.** Extend the positive boundary through every production
caller that obtains or converts checked preservation evidence. Either:

- add `plan.rs`, `phase.rs`, and `phase/evidence.rs` (and their relevant local
  helper graph) to the byte-exact source allowlist and compiler-forbidden
  modules; or
- move the complete observe/validate/evidence-conversion operations behind a
  small protected concrete facade whose caller cannot perform authority work
  before or after it.

Pin the complete caller set rather than scanning one obsolete method name. Add
executable negatives for a direct writer, imported/crate-local alias,
same-name function pointer, nested helper, and differently named backend
callback in each remaining authority class. Preserve the concrete leaf and
release finalizer corrections.

## Focused verification

- exact tuple and clean core/CLI state: verified before and after probes;
- CPython 3.11.14 structural/release suite: 33/33 passed;
- current structural checker: passed with 15 visible entries and five
  classified modules;
- static publish-workflow suite: 7/7 passed;
- exact current-tree format and all-target/all-feature Clippy: passed;
- six mocked `main` release branches: correct exact-gate/tag/push ordering;
- clean, pre/post SHA drift, pre/post dirty, and detached reset release probes:
  correct pass/rejection ordering;
- explicit atomic push refspec probe: both branch and tag sourced from the same
  gated SHA; and
- isolated authority-caller raw-writer probe: structural checker and compiler
  gate incorrectly passed.

I did not duplicate the long full-core suite; the focused checks directly
exercise the changed observer and release boundaries and the counterexample.

## Gate decision

Do not begin R2 production conversion on this tuple. Extend containment from
the read-only leaf through every authority-sensitive caller and evidence
conversion, then repeat both settled-tree re-reviews. The concrete observer,
exact release finalizer, explicit gated-SHA push, Python 3.11 workflow, archive
semantic authority, and capability-privacy corrections should be preserved.
