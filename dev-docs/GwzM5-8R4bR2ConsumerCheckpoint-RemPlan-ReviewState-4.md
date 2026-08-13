# R4b R2 consumer-checkpoint remediation — fourth independent state/authority re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

The fourth correction closes the archive semantic-authority defect and the
specific transitive-helper, crate-local alias, and same-name
`std::fs::write` seams from the third reviews. General capability privacy and
the sealed owner/purpose/plan, ID, and schedule contracts have not regressed.

Two P2 boundary defects remain. The compiler gate denies a hand-written list,
not the complete raw-writer surface claimed by the plan; a same-name
`std::fs::copy` function pointer in a guarded adapter passes both mandatory
checks. The local release script can also skip the compiler gate and, when it
creates a version-bump commit, does not rerun that gate on the exact commit it
tags.

P0: 0. P1: 0. P2: 2. P3: 0.

## Exact tuple reviewed

- workspace root: `4533b29c34b0b7fb75deee08990e5bfab1a5b425`
- `gwz-core`: `8f12016264adde26477a7885770929607eed8195`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean at dispatch. All
three identities were rechecked after source inspection, adversarial probes,
and verification and remained exact. An independent filesystem State-4 report
appeared later as an untracked workspace-root document. It changed no reviewed
source or commit identity and was not an input to this independent review.
This report is my only workspace edit.

## Closed findings and regression audit

### Archive semantic authority: closed

Archive authority now consumes the complete archive semantic validator rather
than trusting a terminal state discriminant.

- Canonical acquisition still reads the open and archive leaves through
  retained real-parent and leaf identities, rereads both, and rejects
  appearance, disappearance, replacement, or content change
  (`gwz-core/src/workspace_ops/merge/record_wire/location.rs:102-162`).
- The opaque source issuer still requires an exact `Open` source, an absent
  destination, a terminal state, and filename/decoded merge-ID agreement while
  retaining the exact source bytes
  (`gwz-core/src/workspace_ops/merge/record_wire/checked_owner.rs:113-141`).
- V0 authority now calls `archive::decode_archived_v0` on those exact bytes.
  This is the same strict header, projection, terminal-envelope, and cleanup
  validation used by production archive recovery. Test-gated v1 authority
  correspondingly calls the complete v1-capable archive decoder
  (`checked_owner.rs:136-137,153-183` and
  `record_wire/archive/mod.rs:45-129`).
- Positive tests now use genuine archived-record fixtures. A syntactically
  decoded `completed` envelope with contradictory terminal material is an
  explicit negative and cannot construct an archive action.

The focused eight-test coordinator-remediation matrix and 24 archive tests
passed. I found no route that can promote a generic record observation into
archive authority or separate the derived owner/digest from the validated
source bytes.

### Prior State-3 transitive and alias shapes: closed

The exact State-3 helper seam is closed. Bundle construction and its
owner/evidence derivation moved from unguarded `artifacts.rs` and `plan.rs`
helpers into `preserve/checked_bundle.rs`. That complete local helper graph is
under `#![forbid(clippy::disallowed_methods)]`; the source inventory also pins
the module's complete imports, visible items, and calls.

I independently copied the exact core tree and ran these executable probes
through both the source checker and Clippy with the copied tree as
`CLIPPY_CONF_DIR`:

| Probe | Source inventory | Compiler gate |
| --- | --- | --- |
| `std::fs::write` inside nested `owner_evidence` | rejected | rejected as `std::fs::write` |
| local function pointer to `crate::artifact::write_atomic`, invoked through already-allowed name `owner_error` | passed | rejected as `gwz_core::artifact::write_atomic` |
| local function pointer to `std::fs::write`, invoked through already-allowed name `map_transition` | passed | rejected as `std::fs::write` |

The latter two establish that compiler name resolution, rather than the source
spelling, now closes the specific imported/crate-local and same-name alias
defects from the third reviews.

### Capability and coordinator contracts: no regression

`CheckedArtifact`, its policy, fact, and transition remain private to
`checked_artifact`; `entry` exposes only complete purpose-specific operations
and facts/results. Its protected adapter modules retain non-overridable
module-level `forbid` attributes, and the source gate rejects removal of those
attributes. I found no re-export of the general capability.

The exact-record observation, sealed `CheckedManagedActionV1`, defensive
owner/action/purpose/plan comparison, prefixed-ID validation, and literal
schedule/reservation vectors remain intact. The full 1,227-test unit suite
passed with only its one declared ignored test.

## Findings

### [P2-1] Compiler containment is only as complete as an incomplete raw-writer list

**Where:** RemPlan sections 1 and 7 and exit criterion 8
(`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:59-67,356-376,412-421`);
`gwz-core/clippy.toml:1-24`;
`gwz-core/src/lib.rs:1-8`; and
`gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:39-51`.

**Violated contract.** The fourth amendment says the Clippy configuration
denies the *complete raw-writer set* and therefore that no guarded checked path
has a raw successful bypass. `clippy.toml` lists selected `write`, `rename`,
remove, create, `OpenOptions`, capability, Git, and GWZ writer methods. It does
not deny `std::fs::copy`, which creates a missing destination or overwrites an
existing destination and is therefore a raw successful writer. Other mutating
standard-library operations are also absent, but one executable counterexample
is sufficient to disprove the claimed closure.

In an isolated copy of the exact core tree, I inserted these lines into the
guarded `write_checked` adapter:

```rust
let map_transition = std::fs::copy;
let _ = map_transition(relative, ".gwz/raw-bypass");
```

This deliberately reuses the adapter's already accepted `map_transition` call
name, exactly as the prior function-pointer attack did. The committed source
checker returned:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

The exact compiler command also exited zero:

```text
CLIPPY_CONF_DIR="$COPY" cargo +1.95.0 clippy \
  --all-targets --all-features -- -D warnings
```

The same module-level `forbid` that correctly rejects `std::fs::write` did not
diagnose `std::fs::copy` because it is absent from `disallowed-methods`.

**Impact.** A selected preservation, publication, rollback, or recovery
adapter can create or overwrite a file outside its purpose-specific checked
entry while the source, compiler, PR, push, and release assertions all remain
green. Physical helper containment does not make arbitrary filesystem APIs
unreachable; it only brings them under a deny list whose completeness is now a
security property but is neither closed nor independently frozen.

**Required correction.** Define and enforce a genuinely closed mutation
surface for guarded modules. At minimum, deny and add same-name executable
fixtures for `std::fs::copy` and every other safe Rust filesystem mutation
primitive (`hard_link`, directory removal, permission/length mutation,
platform link operations, and equivalent capability/crate-local writers).
The stronger correction is a compiler-supported allowlist or capability
boundary in which guarded adapters can name only purpose-specific checked
mutation entry points, so adding another standard-library, dependency, FFI, or
process writer does not require discovering it after the fact. Preserve the
three now-passing prior probes.

### [P2-2] The local release path can omit the compiler boundary and does not gate the exact tagged commit

**Where:** RemPlan section 7 and exit criterion 10
(`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:366-376,418-421`);
`gwz-core/scripts/release.py:339-373,375-393,448-481`; and
`gwz-core/tests/publish_workflow.rs:49-62`.

**Violated contract.** The fourth amendment says PR/push CI, release CI, and
the local release script run both the source and compiler gates against the
exact tree being checked. The local script exposes `--no-clippy`, passes it
into `run_gates`, and explicitly skips the compiler boundary while continuing
through version bump, commit, tag creation, and optional atomic push. The
source checker cannot substitute for that compiler gate; P2-1 and the prior
same-name alias demonstrate why.

There is a second exact-tree mismatch in the same path. `run_gates` executes
before `bump_cargo_version`. If the bump changes `Cargo.toml`/`Cargo.lock`, the
script creates a new release commit and reruns only `cargo test --locked` on
the detached cargo tree. It does not rerun the source/compiler boundary on the
new commit before tagging it. The existing publish-workflow test merely checks
that the Clippy command text and `CLIPPY_CONF_DIR` assignment exist; it does not
reject `--no-clippy` or assert a post-commit exact-tree gate.

**Impact.** The documented release command can create and push a tag without
the only compiler-resolved protection for aliases, and the successful
pre-bump result is not evidence for the exact tagged tree promised by the exit
gate. The main-push and post-publication release workflows may detect a problem
later, but they do not restore the required pre-publication authority boundary.

**Required correction.** Make the compiler boundary non-skippable for any
operation that can create or push a release tag; remove `--no-clippy` or make
it a diagnostics-only mode that cannot commit, tag, or push. After any version
bump and lock refresh, run the same source checker, executable boundary tests,
and `CLIPPY_CONF_DIR=<exact release worktree> cargo clippy --all-targets
--all-features -- -D warnings` on the exact release commit before tag creation.
Pin both properties in `publish_workflow.rs` or an executable release-script
fixture.

## Lower-priority findings

No P3 or P4 findings.

The current purpose-specific entry facade remains the explicitly pre-R2
legacy implementation described by the checkpoint; owner-bound admitted leaf
execution is still an R2-E conversion obligation. I did not treat the absence
of completed R2-E consumer call graphs as a regression in this interface-only
gate, but those converted paths must still prove their exact durable owner
before R2 closure.

## Verification

I independently ran on the exact core SHA:

- `cargo test -p gwz-core --no-fail-fast`: 1,226 unit tests passed, one
  ignored; 47 integration tests passed; doc tests passed;
- `cargo test -p gwz-core checked_artifact::interface_tests::coordinator_remediation -- --test-threads=1`:
  8 passed;
- `cargo test -p gwz-core workspace_ops::merge::record_wire::archive::tests -- --test-threads=1`:
  24 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed with
  15 visible entries and 5 classified modules;
- `python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries -v`:
  19 passed;
- `cargo test -p gwz-core --test publish_workflow -- --test-threads=1`:
  6 passed;
- `cargo fmt --all -- --check`: passed;
- `CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed;
- isolated nested-owner `std::fs::write` probe: compiler rejected;
- isolated same-name crate-local `write_atomic` alias probe: source passed,
  compiler rejected;
- isolated same-name `std::fs::write` function-pointer probe: source passed,
  compiler rejected; and
- isolated same-name `std::fs::copy` function-pointer probe: source passed and
  compiler incorrectly passed.

The green committed checks do not contradict either finding: P2-1 names a
mutator absent from the compiler deny list, and P2-2 is an explicit supported
path that omits or precedes the required compiler invocation.

## Gate decision

Do not begin R2 production conversion on this tuple. Close the raw-mutator
surface and make the exact tagged-tree compiler gate mandatory on a new
committed settled tuple. Preserve the now-closed archive validator,
helper-containment, capability-privacy, owner/purpose/plan, ID, and schedule
contracts, then repeat both independent state/filesystem re-reviews. No public
GWZ protocol change or new durability phase is required.
