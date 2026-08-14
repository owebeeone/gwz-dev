# R4b R2 consumer-checkpoint remediation — ninth independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 2. P3: 0.

The committed ninth correction does make the current literal `#[path]` set
explicit, rejects the two direct non-`.rs` spellings from the eighth review,
requires their observed targets to resolve to in-crate `.rs` files, and hashes
every descendant file suffix in the protected authority-observer tree. The
private marker itself also remains compiler-sealed to `Git2Backend`.

Two P2 boundary defects remain. First, the source-loader inventory is still a
non-recursive spelling scan over `src/**/*.rs`: `cfg_attr` can materialize a
`path` attribute, the built-in `include` macro can be imported under another
name, and the approved target outside `src` is never scanned. Each route hid a
reachable raw writer and concrete-observer call from both mandatory gates.
Second, the v1 backend check rejects only the literal token `GitBackend` inside
the v1 directory. A crate-local alias declared outside that directory can
therefore admit the open trait to a new complete `V1Runtime`; the structural
checker and exact all-target/all-feature Clippy accepted that counterexample as
well.

## Exact tuple reviewed

- workspace root: `4c325a07be3b367f6bc0dc5fd6b4a2371e8803f2`
- `gwz-core`: `da57559f9ce17d12fec72239f4012cf737589140`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` and direct `git rev-parse HEAD` checks agreed on
this exact clean tuple before review. All adversarial probes were made in
exact-SHA exports below `/tmp`; no reviewed production file was changed. This
report is the only workspace edit made by this reviewer.

## Ninth-correction audit

### Current filesystem and module inventory

`scripts/checks/check_checked_artifact_boundaries.py:51-112` lists the current
literal path edges. For every direct spelling found under `src`, the checker
resolves the target, confines it to the crate, checks its `.rs` suffix and file
kind, and compares the resulting set with that allowlist. The committed tree
passes that inventory.

`source_tree_digest` at `:505-519` now includes every file below the protected
authority-observer descendant directory rather than only `*.rs`. The exact
current authority tree contains only the reviewed Rust descendants, and its
path-and-byte digest passes. This correctly closes the eighth-review attack
when it uses the direct spellings covered by the new regressions.

The remaining defect is that `:559` chooses the files to inspect using only
`source.rglob("*.rs")`; discovered targets are validated but never added to a
recursive source graph. The regexes at `:407-413` also recognize only a direct
`#[path = "..."]`, bare `include!`, or direct `std::include`/`core::include`
spelling. Finding P2-1 gives compiling counterexamples.

### V1 backend boundary

`git/gitbackend/authority_backend.rs:5-19` remains sound: the private
supertrait can be implemented only in its declaring module, and only
`Git2Backend` has the marker and authority implementation. The current
`ForwardRuntime`, `ReverseRuntime`, `FinalizationRuntime`, archive/status
helpers, authority observer tree, and physical executors use
`MergeAuthorityBackend`. Ordinary `GitBackend` remains open.

The consumer gate is not equally strong. At
`check_checked_artifact_boundaries.py:633-642` it searches only for the exact
identifier `GitBackend` in v1 production sources. Meanwhile
`v1_lifecycle/service.rs:17-36` permits any type implementing the independent
`ExactObserver` and `PhysicalExecutor` traits to become a `V1Runtime`, and
`git/mod.rs:12` already establishes a general re-export seam outside the
directory searched for open-backend use. Finding P2-2 demonstrates that an
open-trait alias and split runtime cross this boundary without modifying the
protected authority tree or private marker.

### Compatibility and release checks

The current archive semantics and checked-artifact interface matrix remain
green. The contradictory terminal-labelled v0 record rejects, the exact
terminal-open/absent-destination case succeeds, and all 96 focused interface
tests pass.

The release plumbing itself retains the accepted exact-target shape:

- PR and main-push CI run Python 3.11, the structural checker, its regression
  suite, release-boundary tests, and exact all-target/all-feature Clippy;
- release CI runs the same source and compiler gates;
- `release.py:349-410` has no compiler skip, checks the target SHA and clean
  tree before and after the boundary, gates immediately before tag creation,
  and pushes the verified SHA explicitly; and
- all 6 release-boundary tests and all 7 publish-workflow tests pass.

Those properties do not make the release gate complete: every exact-export
counterexample below was accepted by the same structural checker and Clippy
commands that PR, release CI, and local release execute.

## Findings

### [P2-1] The source graph can still be extended through unrecognized loader forms and an unscanned approved target

**Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:48-112`,
`:407-413`, and `:554-592`; `gwz-core/src/lib.rs:41-44`;
RemPlan sections 1, 7, 8 item 10, and exit criterion 8.

**Violated contract.** The ninth correction says every compiler-loaded Rust
source edge is frozen, `include!` is rejected, each approved target is a
regular in-crate `.rs` file, and the inventory matches the compiler-loaded
source graph. The implementation enumerates syntactic direct spellings in
`src/**/*.rs`; it does not enumerate the effective attribute/macro graph and
does not scan the target it approves at
`../protocol/corpus/rust/vectors.rs`.

**Executable counterexamples.** I exported exact core SHA
`da57559f9ce17d12fec72239f4012cf737589140` and routed the already selected
test-gated `v1_preservation_image` through an `.inc` helper which first called
`std::fs::write` and then
`crate::git::observe_preservation_stashes_read_only`. Three independent source
routes were accepted.

1. `cfg_attr` materialized the forbidden path attribute without matching
   `PATH_ATTRIBUTE_START`:

   ```rust
   #[cfg(test)]
   #[cfg_attr(test, path = "unreviewed_observer.inc")]
   mod unreviewed_observer;
   ```

2. A grouped import renamed the built-in source-loading macro without matching
   `INCLUDE_SOURCE_LOADER`:

   ```rust
   #[cfg(test)]
   use std::{include as load_unreviewed_source};
   #[cfg(test)]
   load_unreviewed_source!("unreviewed_observer.inc");
   ```

3. I placed the writer/observer helper directly in the already approved
   `protocol/corpus/rust/vectors.rs` target and called it from
   `v1_preservation_image`. `src/lib.rs` retained its exact approved path edge,
   but the target is outside the `source.rglob("*.rs")` root and is absent from
   the concrete-observer reference scan.

For each form the structural command printed:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

Exact `CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets
--all-features -- -D warnings` also exited zero. The helpers compiled on the
same `cfg(test)` surface as v1 and were called immediately before the reviewed
authority path.

The committed tests at
`test_check_checked_artifact_boundaries.py:323-356` cover only a direct
`#[path]`, direct `include!`, and a macro definition containing the literal
`#[path` token. They do not cover effective `cfg_attr(path = ...)`, a renamed
built-in macro, or recursive scanning of the outside-`src` approved target.

**Impact.** A checked-in mutation can execute a raw writer immediately before
the concrete preservation observation while PR, release CI, local release,
the structural inventory, and compiler lint all report success. The fixed
observer caller set and the claimed compiler-loaded source closure are
therefore not enforced.

**Required correction.** Build one recursive, cycle-checked inventory of every
effective compiler-loaded source rather than scanning only direct spellings
under `src`. At minimum, reject `cfg_attr` forms capable of producing `path`,
reject every import/re-export/alias of the built-in `include` macro, and scan
every approved target recursively regardless of whether it is below `src`.
Preserve target path, multiplicity, canonical identity, regular-file status,
suffix, and bytes in the positive manifest. Add compiling negative regressions
for all three exact forms above, called from the existing
`v1_preservation_image` path.

### [P2-2] A renamed open backend can implement a complete v1 runtime outside the sealed consumer check

**Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:633-642`;
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:358-370`;
`gwz-core/src/git/mod.rs:10-12`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs:17-36`; RemPlan
sections 1 and 7, implementation item 9, and exit criterion 8.

**Violated contract.** Every test-gated production v1 lifecycle source which
accepts a backend must require the sealed `MergeAuthorityBackend`, and a new
wrapper or consumer must fail closed. The checker instead treats absence of
the literal token `GitBackend` in the v1 directory as proof of that property.
It does not resolve aliases or positively inventory the complete v1 runtime
adapter set.

**Executable counterexample.** In a second exact-SHA export I added this alias
outside the scanned v1 directory:

```rust
#[cfg(test)]
pub(crate) use gitbackend::GitBackend as UnsealedV1Backend;
```

I then added a `SplitRuntime<O, B>` to `v1_lifecycle/service.rs`. Its
`ExactObserver` delegated to any reviewed observer, while its
`PhysicalExecutor` was bounded only by `B: UnsealedV1Backend`, invoked
`backend.status(...)`, and returned `ExecutionDiagnostic::Success`. A small
constructor returned `impl V1Runtime`; the blanket implementation at
`service.rs:36` therefore proved this was a complete runtime accepted by the
service, not merely an unused backend helper. An implementation of the open
trait can give the nominal `status` callback arbitrary mutation semantics.

Neither v1 source contained the text `GitBackend`, the private marker and
protected authority-observer tree were unchanged, and both mandatory commands
exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
Finished `dev` profile ...
```

I also changed the existing finalization physical-executor bound through the
same alias in a separate exact export; after routing around the sealed
verification helper, the checker and exact Clippy again exited zero. The
committed negative at
`test_check_checked_artifact_boundaries.py:358-370` detects only direct textual
replacement with the name `GitBackend`.

**Impact.** The sealed trait correctly prevents an alternative type from
implementing `MergeAuthorityBackend`, but the source gate does not ensure that
v1 consumers continue to require that trait. A new runtime can pair reviewed
observations with open-backend physical behavior or report successful
execution without the sealed verifier, and all release gates accept it.

**Required correction.** Enforce the backend seal at the compiler-visible v1
runtime/adapter interface, not through a token denylist. Positively freeze all
production `V1Runtime`, `ExactObserver`, `PhysicalExecutor`, factory, wrapper,
and backend-binding implementations or make the service accept only a private
sealed production runtime whose backend association is `MergeAuthorityBackend`.
Inventory crate-wide aliases/re-exports which can name the open trait. Add a
compiling negative regression for the outside-directory alias plus complete
split runtime above; the structural gate must reject it independently of
whether the private marker itself remains unimplementable.

## Focused command evidence

Reviewed-tree commands included:

```text
gwz status --json
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD

../.venv/bin/python scripts/checks/check_checked_artifact_boundaries.py
../.venv/bin/python -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v
../.venv/bin/python -m unittest scripts/checks/test_release_boundary.py -v
cargo +1.95.0 test checked_artifact::interface_tests
cargo +1.95.0 test archive_authority_rejects_a_terminal_label_with_a_contradictory_v0_envelope
cargo +1.95.0 test archive_authority_requires_one_terminal_open_source_and_an_absent_destination
cargo +1.95.0 test --test publish_workflow
CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets --all-features -- -D warnings
cargo +1.95.0 fmt --all -- --check
git diff --check 3569a3d819c47256f8da326d4e48c27bd96e81a4..da57559f9ce17d12fec72239f4012cf737589140
```

Results:

- structural boundary suite: 41/41 passed;
- checked-artifact interface matrix: 96/96 passed;
- release-boundary suite: 6/6 passed;
- publish-workflow suite: 7/7 passed;
- focused archive tests: 1/1 and 1/1 passed;
- current checker, formatting, diff check, and exact all-target/all-feature
  Clippy: passed;
- reachable `cfg_attr(path = ...)` non-`.rs` writer/observer: checker and
  Clippy incorrectly passed;
- renamed built-in `include` non-`.rs` writer/observer: checker and Clippy
  incorrectly passed;
- writer/observer in the outside-`src` approved path target: checker and
  Clippy incorrectly passed; and
- open-backend alias plus complete split `V1Runtime`: checker and Clippy
  incorrectly passed.

## Gate decision

Do not begin R2 production conversion on this tuple. Preserve the current
direct-edge validation, all-suffix authority-tree digest, private backend
marker, semantic compatibility checks, and exact release convergence, but
replace the spelling-only source and backend consumer checks with complete
compiler-enforced or positive recursive boundaries. Repeat both independent
settled-tree reviews after correction. This review returns **NO-GO**.
