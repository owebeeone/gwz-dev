# R4b R2 consumer-checkpoint remediation — third independent state/architecture re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

The archive-authority correction is closed, and the structural privacy,
visible-entry inventory, executable negative fixtures, and publication wiring
materially improve the checked/ordinary boundary. One P2 remains: the claimed
complete checked-adapter call graph stops at a fixed one-level helper list. A
raw successful writer in an already reachable deeper helper passes the gate.

P0: 0. P1: 0. P2: 1. P3: 0.

## Exact tuple reviewed

- workspace root: `c5a01316d79fcd9a80acb6727c61a4028544b373`
- `gwz-core`: `8ba1d52c5a6dbf99f76e8411e1db721397929ac9`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean before review. The
three commit identities remained unchanged through source inspection, the
copied-tree adversarial probe, and focused verification. This report is the
only workspace edit made by the review.

## Prior finding closure

### Archive authority: closed

The prior archive-arbitration P2 is structurally closed.

- `CanonicalMergeLocations` acquires the canonical open and archived leaves,
  retains parent and leaf identity, repeats both reads, and rejects parent,
  identity, presence, or content changes during acquisition
  (`gwz-core/src/workspace_ops/merge/record_wire/location.rs:102-162`).
- The private archive observation requires an absent destination, an exact
  source whose kind is `Open`, a decoded terminal state, and agreement between
  the canonical filename and decoded merge ID. It retains the exact source
  bytes with the decoded owner identity
  (`gwz-core/src/workspace_ops/merge/record_wire/checked_owner.rs:18-34,112-178`).
- `CheckedManagedActionV1::for_archive` accepts only that opaque archive-source
  observation and derives both owner binding and source digest from it; it can
  no longer promote a generic decoded record observation
  (`gwz-core/src/checked_artifact/coordinator/identity.rs:269-282`).
- The focused matrix rejects nonterminal states, both-absent,
  destination-only, both-present, and wrong-kind source arrangements and
  accepts terminal v0 and test-gated v1 sources. The corresponding interface
  tests pass.

This satisfies the RemPlan's stable terminal-open/source-only authority
contract. I found no remaining path that can directly construct the opaque
archive prerequisite from generic record fields.

### Sealed owner/purpose/plan, IDs, and schedules: no recurrence

I found no recurrence of the defects closed by the second correction:

- `CheckedManagedActionV1` still seals owner class, purpose-specific managed
  request, action identity, and plan binding before managed preflight;
- exact record-owner observations still keep decoded IDs and the bounded source
  bytes inseparable;
- prefixed IDs still reject `.` and `..` suffixes; and
- the literal managed schedule/reservation matrices remain present and passed
  in the 95-test interface suite.

The general `CheckedArtifact`, fact, transition, and policy types are now
private to `checked_artifact`, while `checked_artifact::entry` returns only
purpose-specific facts, transitions, or completed results. This closes the
prior direct general-capability escape.

## Finding

### [P2-1] The boundary gate does not inventory the complete transitive helper graph

**Where:** RemPlan section 7 and exit gate 8
(`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:336-345,370-383`);
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:133-291,514-544`;
`gwz-core/src/workspace_ops/merge/preserve/checked_bundle.rs:27-28,55,74-75`;
`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:674-685`; and
`gwz-core/src/workspace_ops/merge/preserve/plan.rs:710-740`.

**Violated contract.** The correction claims that the source gate inventories
the complete adapter call graph, rejects transitive writers, and proves that
selected merge checked paths have no raw successful bypass. The committed
checker instead hard-codes only two transitive helper bodies:
`expected_bundle` and `open_repo`.

`checked_bundle` calls `expected_bundle`, whose inventoried body calls
`super::plan::v1_owner_evidence`. The checker records that call name but does
not follow or inventory `v1_owner_evidence` itself. The traversal therefore
stops while the checked bundle's reachable call graph continues.

I copied the reviewed `gwz-core/src` tree outside the workspace, inserted this
executable statement at the start of `v1_owner_evidence`, and ran the committed
gate against the copy:

```rust
let _ = std::fs::write("raw-checked-bypass", b"bypass");
```

The checker exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

This is not a comment, string, import-alias, or spelling-only probe. It is a
real raw writer in a helper already reached by the checked v1 bundle adapter.
The v1 path is test-gated today, but the RemPlan explicitly classifies it as a
checked path that will become production-reachable at A1. The gate also has no
fixed-point assertion that each selected preservation/publication/rollback/
recovery path reaches its exact record-owner constructor; it pins only the
listed entry, adapter, and two helper bodies.

**Impact.** A future change can add a raw successful writer below an inventoried
helper, or move checked logic into a deeper helper, while PR, push, and release
gates all remain green. The gate wiring is correct, but it publishes a false
assurance for the architectural boundary it is meant to freeze before parallel
R2 conversion.

**Required correction.** Close the reachable graph rather than one manually
selected depth. Either:

1. use Rust module/type visibility so every helper reachable from a checked
   adapter is structurally unable to invoke raw writer capabilities; or
2. build a parser/compiler-supported traversal to a fixed point, resolving
   local imports and calls, and fail on every unclassified reachable callee.

Whichever approach is chosen must add an executable negative fixture for the
exact `checked_bundle -> expected_bundle -> v1_owner_evidence -> raw writer`
shape and assert the exact owner-constructor reachability for every selected
merge path. The existing visible-entry, import, capability-escape, direct
adapter, PR/push, and local-release checks should remain.

## Verification

I independently ran on the exact core SHA:

- `cargo test -p gwz-core checked_artifact::interface_tests`: 95 passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed;
- `python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries`:
  13 passed;
- `cargo test -p gwz-core --test publish_workflow`: 6 passed;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed; and
- copied-tree deeper-helper raw-writer probe: incorrectly accepted by the
  boundary checker.

The green committed boundary checks do not contradict P2-1: their fixed
inventories end before the adversarial writer, so they never inspect it.

## Gate decision

Do not begin R2 consumer conversion on this tuple. Close the complete
transitive-call and exact-owner-reachability gap on a new committed settled
tuple, preserve the now-closed archive and owner/purpose/plan/ID/schedule
contracts, and repeat both independent state/filesystem re-reviews. No public
GWZ protocol change or new durability phase is required.
