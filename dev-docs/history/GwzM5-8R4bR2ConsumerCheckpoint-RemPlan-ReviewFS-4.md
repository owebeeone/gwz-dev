# R4b R2 consumer-checkpoint remediation — fourth independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 2. P3: 0.

The fourth correction closes the contradictory-terminal archive-authority
defect from FS-3. It also proves that Clippy resolves the two specifically
listed indirect aliases used by the committed probes. The checked writer
boundary remains bypassable, however: its denylist is not a complete writer
boundary, its structural guard can be satisfied by a comment, and a checked
adapter's accepted backend call reaches an unguarded implementation. The local
release command can additionally omit the compiler gate and still create and
push a release tag.

## Exact tuple reviewed

- workspace root: `4533b29c34b0b7fb75deee08990e5bfab1a5b425`
- `gwz-core`: `8f12016264adde26477a7885770929607eed8195`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean before review. All
adversarial mutations were made in exported copies under `/tmp`; no reviewed
source was modified.

## Closure of FS-3 P2-1: complete archive semantics

**Closed.** The v0 archive issuer now obtains identity and state from the exact
open bytes, rejects an open state, and passes those same bytes and decoded
merge ID through `decode_archived_v0` before it can construct
`CheckedArchiveSourceObservation`
(`workspace_ops/merge/record_wire/checked_owner.rs:119-141`). That is the
production archive validator and therefore applies the complete v0 terminal
envelope and projection invariants rather than trusting the state discriminant.
The test-gated v1 issuer likewise invokes its complete version-specific archive
decoder (`checked_owner.rs:152-179`).

I independently exported the exact core commit and added a temporary test with
the minimal contradictory `state: completed` envelope used by FS-3. The test
first confirmed `decode_archived_v0` rejects it, then acquired the canonical
open/absent location pair and confirmed
`observe_checked_archive_source_v0` also rejects it without creating `done`:

```text
test ...contradictory_terminal_v0_cannot_issue_archive_authority ... ok
test result: ok. 1 passed; 0 failed
```

The committed positive fixture now uses a genuinely validated archived-v0
fixture, and its explicit contradictory-terminal negative passes. I found no
independent decode path capable of minting v0 archive authority after the
complete validator rejects the exact bytes.

## Partial closure of FS-3 P2-2

The compiler probes do reject function-item aliases for methods actually named
in `clippy.toml`. In exact exported copies, all-target Clippy rejected both:

```rust
let map_transition = std::fs::write;
let owner_error = crate::artifact::write_atomic;
```

with resolved names `std::fs::write` and
`gwz_core::artifact::write_atomic`. The checked-bundle evidence and owner
helpers previously delegated to `preserve/artifacts.rs` and `preserve/plan.rs`
have also been moved into the guarded adapter.

Those improvements do not close the complete selected writer graph; see P2-1.

## Findings

### [P2-1] The selected checked writer boundary still has three compiling fail-open forms

**Where:**

- `gwz-core/clippy.toml:1-24`
- `gwz-core/src/lib.rs:4-7`
- `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:14-19,414-421,501-523`
- `gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:1,39-51`
- `gwz-core/src/workspace_ops/sync_workspace_boundary.rs:84-86`
- `gwz-core/src/workspace_ops/merge/preserve/checked_bundle.rs:137-159`
- `gwz-core/src/git/gitbackend/preservation.rs:373-380`

**Violated contract.** RemPlan sections 1 and 7 and exit criterion 8 require a
complete, fail-closed selected adapter/helper graph with no direct, aliased,
crate-local, or nested-helper raw successful writer. Compiler name resolution
is useful only after the complete writer set and complete transitive scope have
been established. Neither is true on this tuple.

#### A. An unlisted crate-local writer alias passes both mandatory gates

`clippy.toml` denies `artifact::write_atomic`, `stash::write_bundle`, and
`sync_workspace_boundary`, but it does not deny other crate-local functions
that successfully reach those writers. One such function is the crate-visible
`publish_workspace_exclude_candidate`, which publishes arbitrary caller bytes
to `.git/info/exclude` through `artifact::write_atomic`.

In an exported exact-SHA tree I inserted the following into the already
classified `artifact_facts::write_checked` adapter:

```rust
let map_transition = crate::workspace_ops::publish_workspace_exclude_candidate;
map_transition(root, "unchecked boundary replacement")?;
```

The alias assignment is not a source-level call, and `map_transition` is an
already accepted call name. The source gate returned:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

`CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets --all-features -- -D warnings`
also exited zero. This is a compiling ordinary writer in the guarded adapter
and directly disproves the claimed complete crate-local writer denial.

#### B. An accepted adapter call reaches an unguarded writer implementation

The checked-bundle adapter calls the accepted
`backend.preservation_stashes(...)` method. Its Git2 implementation lives in
`git/gitbackend/preservation.rs`, outside the four protected compiler modules
and outside any transitive source inventory. I added this executable line to
that exact implementation in another exported tree:

```rust
std::fs::write(path.join("raw-helper-bypass"), b"bypass").map_err(io_error)?;
```

The selected checked-bundle path reaches this implementation. Nevertheless,
the structural checker and the exact all-target Clippy command both exited
zero. Physically containing two former helper functions did not close the
actual backend/helper graph to a fixed point.

#### C. Commenting out the compiler boundary is accepted as protection

The source checker tests for the raw substring
`#![forbid(clippy::disallowed_methods)]`; it does not parse or mask the line.
In a third exported tree I changed the guard to:

```rust
// #![forbid(clippy::disallowed_methods)]
```

and inserted the original same-name `std::fs::write` alias. Because the crate
root explicitly allows this lint, the commented guard disabled protection.
The structural checker still reported `ok`, and the same all-target Clippy
command exited zero. The committed removal test covers deletion, not a
non-executable occurrence.

**Impact.** A checked preservation, publication, rollback, or recovery path
can perform an ordinary filesystem mutation while every PR/push, release-CI,
and local compiler/source command reports success. The checked entry types may
remain private while the caller mutates through another public writer or an
accepted unguarded callee. R2 lanes therefore cannot rely on this checkpoint
to prevent raw successful paths from reappearing.

**Required correction.** Make the protected boundary architectural rather
than a hand-maintained list of leaf names:

1. remove crate-visible successful ordinary writers from the guarded modules'
   reachable namespace, or expose them through a capability type unavailable
   to checked adapters;
2. classify and protect the complete transitive implementation graph of every
   accepted adapter call, including Git backend trait dispatch, to a fixed
   point;
3. parse lint attributes as Rust syntax (or make their absence a compile
   failure), rather than accepting a raw substring; and
4. add compiling regressions for the exact unlisted crate-local alias,
   checked-adapter-to-backend writer, and commented-guard probes above.

Merely appending the discovered function names to `clippy.toml` would leave
the same open-world defect for the next wrapper or trait implementation.

### [P2-2] Local release can skip the mandatory compiler boundary and still tag and push

**Where:**

- `gwz-core/scripts/release.py:26-32,339-373,379-388,448-481`
- `gwz-core/tests/publish_workflow.rs:49-61`

**Violated contract.** RemPlan section 1 and exit criterion 10 require the same
source and compiler gates to run against the exact local release worktree
before publication. The default path does this correctly: it builds a detached
worktree from exact `HEAD`, runs both source checks there, sets
`CLIPPY_CONF_DIR` to that worktree, and invokes all-target/all-feature Clippy.

But `release.py` publicly accepts `--no-clippy`. `run_gates` then logs
`skipping cargo clippy`, returns success, and the caller proceeds to version
commit, tag creation, and optional atomic push. `python3 scripts/release.py
--help` confirms this released option. The workflow test asserts only that the
Clippy command text and worktree environment exist; it does not reject the
skip route.

**Impact.** A local `release.py --push ... --no-clippy` can publish a branch
and immutable release tag without running the only compiler-resolved half of
the checked boundary. PR/push CI may subsequently report a failure, but that
is not a pre-tag local release gate and does not satisfy the checkpoint's
publication invariant.

**Required correction.** Make the checked-boundary Clippy invocation
non-skippable in release mode. If `--no-clippy` must remain for unrelated
development diagnostics, it must refuse tag creation and push, or a separate
small mandatory boundary compiler target must still run. Add a test that the
skip option cannot reach `ensure_tag` or `push_release` without the boundary
compiler command succeeding.

## Gate wiring that is otherwise closed

- PR and `main` push CI check out the event commit and run the structural
  checker, its tests, and all-target/all-feature Clippy with
  `CLIPPY_CONF_DIR="$PWD"`.
- release CI checks out the selected tag and runs the structural checker and
  all-target/all-feature Clippy against that tag.
- the normal local release path creates a detached worktree from exact
  `HEAD`, runs the source checker and tests with that worktree as `cwd`, and
  runs Clippy with that worktree as both Cargo root and `CLIPPY_CONF_DIR`.

I found no wrong-checkout or ambient-config substitution in those normal
paths. P2-2 concerns the explicit local omission path.

## Verification

On the exact committed tree:

- `cargo test -p gwz-core checked_artifact::interface_tests::coordinator_remediation -- --test-threads=1`:
  8 passed;
- `cargo test -p gwz-core workspace_ops::merge::record_wire::archive::tests -- --test-threads=1`:
  24 passed;
- `cargo test -p gwz-core --test publish_workflow -- --test-threads=1`:
  6 passed;
- `python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v`:
  19 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed with
  15 visible entries and 5 classified modules;
- `cargo +1.95.0 clippy --all-targets --all-features -- -D warnings`: passed;
  and
- `cargo fmt --all --check`: passed.

Independent exported-tree probes:

- contradictory terminal v0: authority rejected, 1 test passed;
- listed `std::fs::write` same-name function item: all-target Clippy rejected;
- listed `gwz_core::artifact::write_atomic` alias: all-target Clippy rejected;
- unlisted `publish_workspace_exclude_candidate` alias: structural checker and
  all-target Clippy both passed incorrectly;
- raw writer in the selected `preservation_stashes` backend implementation:
  structural checker and all-target Clippy both passed incorrectly; and
- commented `forbid` plus the original same-name `std::fs::write` alias:
  structural checker and all-target Clippy both passed incorrectly.

Green committed tests do not contradict P2-1: their compiler probes exercise
only listed methods and their boundary-removal probe removes the marker rather
than leaving it in non-code. They do not exercise an accepted call's backend
implementation. Green workflow tests do not contradict P2-2 because they do
not exercise `--no-clippy`.

## Gate decision

Do not begin R2 production conversion on this tuple. Close the successful
writer capability and complete transitive dispatch boundary, make the lint
guard syntactic/non-optional, and make the local release compiler boundary
non-skippable before tagging. Commit one new settled workspace/core/CLI tuple
and repeat both independent state and filesystem re-reviews. No public GWZ
protocol change or new durability phase is required.
