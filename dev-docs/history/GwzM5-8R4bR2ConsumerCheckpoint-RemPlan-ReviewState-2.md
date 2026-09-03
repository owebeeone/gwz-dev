# R4b R2 consumer remediation checkpoint — second independent state re-review

Date: 2026-08-14

## Exact tuple reviewed

- workspace root: `d0f61af45c0c97243793de5760e5c292571a2bd7`
- `gwz-core`: `04d2c1f35eef466445abfad9f8d5333de8441970`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The tuple was clean and exact before inspection and remained at these commit
identities throughout the review.

Primary correction target:
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

Prior state review:
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState.md`.

## Verdict

**NO-GO for R2 consumer conversion.**

P0: 0. P1: 0. P2: 2. P3: 0.

The correction closes the same-owner purpose/plan substitution and both prior
P3 findings. It also substantially improves owner issuance: record IDs and
source bytes can no longer be supplied independently, and production managed
preflight consumes a sealed owner/request/action tuple. Two state/architecture
defects remain. Archive authority is issued from any decoded record rather
than an opaque source-only terminal observation, and the checked/ordinary gate
still has ordinary-call and raw-successful-bypass forms that it does not
classify. The latter is also run only after release publication, not by a
pre-merge or local release gate.

## Closure of the prior findings

### Prior P2 owner/authority/exact-record observation: partially closed

`CheckedManagedActionV1` now seals the owner class, managed request, purpose
set, action digest, and owner binding
(`gwz-core/src/checked_artifact/coordinator/identity.rs:241-303`). A merge-start
owner can construct only the fixed merge-start request, while durable merge
purposes require a record-derived owner. `CheckedActionRequestV1` also repeats
the owner/authority check defensively (`identity.rs:305-328`).

The production record issuer reads one bounded exact record, decodes identity
fields from those same bytes, and does not expose a constructor that can pair
caller-selected IDs with unrelated bytes
(`workspace_ops/merge/record_wire/checked_owner.rs:14-81`). Different exact
unknown-field bytes produce different owner/action bindings. These repairs
close the original independent-ID/digest composition defect.

Archive authority is not yet restricted to the required source-only terminal
state; see P2-1.

### Prior P2 same-owner purpose/plan substitution: closed

Production `ManagedParentBootstrapOwnerV1::preflight_checked` accepts only a
`CheckedManagedActionV1` and derives the request, action digest, and owner
binding internally. The loose three-input preflight exists only in test builds
(`bootstrap/managed/owner.rs:42-67`). The plan persists its exact declared
purpose mask, and reservation derivation compares that mask with the action
request in addition to comparing action digest and owner binding
(`bootstrap/managed/owner.rs:77-90,149-175`;
`coordinator/schedule.rs:18-30`).

The negative same-owner substitution test deliberately builds a plan for one
purpose set under another action's digest and proves rejection before a
reservation. I found no production constructor that can reproduce the invalid
composition.

### Prior P2 checked/ordinary boundary and raw successful bypass: not closed

Raw `CheckedArtifact::acquire` and `prepare_parent` are now module-private, and
the five current entry functions have an explicit expected-caller inventory.
That is a useful narrowing of the surface, but the executable check is not
fail-closed; see P2-2.

### Prior P3 prefixed `.`/`..` IDs: closed

`validate_prefixed_id` now rejects `.` and `..` after the required prefix
(`coordinator/identity.rs:497-511`). Tests cover both workspace and operation
IDs (`interface_tests/coordinator_remediation.rs:375-391`).

### Prior P3 literal managed schedule vectors: closed

The coordinator fixtures now pin one-, two-, and four-purpose declarations,
the first-merge two-purpose plan, partial bootstrap, and combined parent/leaf
work. They assert literal schedule and reservation digests, 64 barriers,
cleanup masks, row ordinals/ranges, component totals, and generation capacity
(`interface_tests/coordinator_remediation.rs:191-373`). The pre-existing R1
schedule fixtures retain the `K=1,N=8` and `K=8,N=8` capacity extremes. This is
sufficient to close the prior test-shape finding.

## Findings

### [P2-1] Archive authority is issued without terminal/source-only arbitration

**Where:** remediation parent policy at lines 276-302;
`gwz-core/src/checked_artifact/coordinator/identity.rs:267-280`;
`gwz-core/src/workspace_ops/merge/record_wire/checked_owner.rs:55-81`;
`gwz-core/src/workspace_ops/merge/record_wire/location.rs:13-34,58-99`; and
`gwz-core/src/checked_artifact/interface_tests/coordinator_remediation.rs:23-27,154-164`.

**Violated contract.** `MergeArchive` may create only `done` beneath an exact
retained `.gwz/merge`, and requires an opaque proof that the canonical source
record is terminal while the archive destination is absent. The proof is
supposed to be the result of terminal/source arbitration, not merely proof that
some merge record bytes decode.

`CheckedManagedActionV1::for_archive` currently accepts the generic
`CheckedOwnerRecordObservation` and manufactures
`ValidatedArchiveSourceV1` directly from its owner binding and byte digest.
The observation validates bounds, decodability, and filename/record merge-ID
agreement only. It neither checks `CanonicalRecordKind::Open`, verifies a
terminal operation state, nor proves that the archived destination leaf is
absent. Although `CanonicalRecordLeaf` records `Open` versus `Archived`, the
issuer ignores that kind and accepts either leaf.

The focused positive test exposes the gap directly: its v0 fixture has
`state: executing`, yet `CheckedManagedActionV1::for_archive(&record)` is
expected to succeed.

**Failure scenarios.** A consumer can issue archive-parent authority while the
open operation is still executing. It can also obtain an observation from the
archived leaf in a destination-only or both-present state and use that as the
purported source proof. Managed-parent preflight then sees a retained
`.gwz/merge` and may plan `done`, even though the state that authorizes that
creation was never proved. This defeats the zero-mutation rejection promised
for nonterminal, destination-only, and contradictory archive states.

**Required correction.** Separate generic durable-record ownership from
archive-source authority. Issue the archive prerequisite only from one opaque
observation of `CanonicalMergeLocations` that proves, together:

1. the open leaf is exact and is `CanonicalRecordKind::Open`;
2. the decoded record is in an allowed terminal state;
3. the archived leaf is absent;
4. path identity and decoded merge identity agree; and
5. owner IDs and source digest are derived from that exact open record.

`CheckedManagedActionV1::for_archive` should accept only that opaque
source-only terminal value; it must not promote a generic record observation.
Add explicit executing/open, destination-only, both-present, archived-leaf,
wrong-kind, and changed-between-observations rejection tests, with zero
provider execution/admission/mutation assertions.

### [P2-2] The boundary checker can miss ordinary checked calls and raw successful bypasses

**Where:** remediation lines 305-333 and exit criteria 369-370;
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:14-50,63-119`;
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:24-91`;
`gwz-core/src/checked_artifact/entry.rs:12-67`;
`gwz-core/src/checked_artifact/mod.rs:101-112`;
`gwz-core/src/checked_artifact/transition.rs:17-128`; and
`gwz-core/.github/workflows/release.yml:5-13,72-78`.

**Violated contract.** The corrected checkpoint claims a complete,
fail-closed production entry/caller inventory, a complementary guarantee that
checked merge adapters have no raw successful writer, negative fixtures for
both classes, and a mandatory local/CI gate.

The checker inventories only text matching the fully qualified expression
`checked_artifact::entry::<symbol>`. Rust callers can import or re-export an
existing entry and invoke it unqualified. For example, adding
`use crate::checked_artifact::entry::acquire_merge_root_artifact;` and calling
`acquire_merge_root_artifact(...)` from an ordinary stash submodule is
invisible while the expected fully qualified caller remains present. The
negative ordinary-submodule test appends only a fully qualified comment, so it
does not exercise this valid Rust form.

Entry definition inventory is likewise limited to the exact text shape
`pub(crate) fn`. A re-export, macro-generated item, callable type, or another
visible item shape can escape without becoming an unknown definition.

The raw-bypass check scans only three adapter source files and only a short list
of direct filesystem spellings. It does not follow helper calls and does not
inventory existing raw writer APIs such as `crate::stash::write_bundle`.
Because `entry` returns a crate-visible `CheckedArtifact` whose transition
methods are also crate-visible, an allowed adapter can delegate either checked
mutation or an alternate raw successful write to an unscanned helper. The
checker proves only that a listed spelling is absent from the adapter's own
text, not that the selected preservation/publication/rollback/recovery path
has no successful bypass.

Finally, the check is wired to the `release` workflow, which runs after a
release is published or for manual tag verification. It is not run by the
repository's pull-request/push workflow, and `scripts/release.py` does not run
it. This can detect drift only after the release boundary that it is intended
to protect.

**Required correction.** Make the boundary structural rather than dependent on
call spelling. Keep the raw artifact and transition capability private to
`checked_artifact`, and expose complete purpose-specific operations from the
single entry module instead of returning a general crate-visible artifact.
Then inventory every externally visible item/re-export in that module and its
callers with a parser/compiler-supported check or an equally complete
source-level rule that rejects imports, aliases, and unqualified calls.

For raw-bypass closure, enumerate the complete successful writer call graph for
each selected merge path, including transitive helpers and store-owned writer
APIs, or arrange module visibility so those paths cannot call raw writers.
Negative fixtures must compile representative unqualified imports, re-exports,
transitive helpers, and non-`std::fs` raw writers and prove gate failure. Run
the boundary gate in pre-merge push/PR CI and from the local release gate,
before a tag or release is published.

## Verification

I independently ran on the exact core SHA:

- `cargo test -p gwz-core checked_artifact::interface_tests`: 93 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed; and
- `python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py`:
  6 passed.

The supplied settled-tree evidence additionally reports:

- `cargo test -p gwz-core --no-fail-fast`: 1,223 unit tests passed, one
  ignored, plus 45 integration tests passed;
- 32 preservation tests passed; and
- Clippy, formatting, protocol, documentation, compatibility, and current
  boundary gates passed.

The green boundary result does not contradict P2-2: the finding identifies
valid entry and bypass forms outside the checker's recognition grammar. The
green archive tests likewise include a positive assertion for the forbidden
executing-record case rather than a terminal/source-only rejection.

## Gate decision

The exact tuple is **NO-GO**. Close P2-1 and P2-2 on a new committed settled
tuple, preserve the now-correct sealed owner/purpose/plan and literal-vector
interfaces, and repeat both independent state/filesystem re-reviews. R2
consumer conversion should not begin while either authority can be issued
without its required durable state or the compatibility gate can pass an
ordinary checked entry/raw successful bypass.
