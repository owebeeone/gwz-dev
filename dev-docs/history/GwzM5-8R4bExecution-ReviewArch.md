# GWZ M5-8 R4b-X participant execution architecture review

Date: 2026-08-10

Status: **GO; P0: 0, P1: 0, P2: 0, P3: 0**

## Scope and verdict

This is an independent architecture and interface re-review of the settled
R4b-X participant/continue/recovery implementation. It covers participant
preparation and physical execution, Continue and restart reconciliation,
exact recovery-origin verification, the no-ff prepared-merge interface,
transition footprints, participant path safety, checked-store/lease/proof
authority, v0/v1 isolation, and the package budget and ownership shape.

R4b-P preservation/rollback/status/archive consumers, R4b-G aggregate wiring,
production dispatch, and A1 activation remain out of scope. No implementation
code was modified by this review.

**GO.** No P0, P1, P2, or P3 finding remains. The implementation is suitable
for its declared R4b-P and R4b-G dependants. This review approves a scope
correction from the original 900-line production estimate to a **1,050-line
R4b-X production ceiling**. The existing 1,100 moved-production, 1,500
test/tool/doc, 14 production-path, and 12 test/tool/doc-path ceilings remain
adequate.

## Prior-finding closure

### No-ff preparation, validation, execution, and restart are exact

Read-only preparation receives the merge mode. `ForceMergeCommit` on a
fast-forward graph freezes a `GitPreparedMerge::Commit` containing the exact
source tree and complete author and committer signatures. The durable prepared
variant is then the sole validation and execution authority: `FastForward`
advances directly, while `Commit` creates the frozen two-parent commit even
when the graph could fast-forward. This avoids a redundant mode parameter at
the already exact execution seam.

Validation rechecks the attached branch, exact before/source commits, clean
repository state, result class, tree and signatures. Execution repeats those
checks while holding the branch-ref transaction, verifies ordered parents,
message, tree and signatures, publishes the ref, and verifies the final
checkout. The concrete tests prove both initial no-ff execution and restart
adoption of the exact prepared merge commit without participant reexecution.

### Participant outcome and failure footprints are closed

`VerifyUpToDate` is adopted directly from a fresh exact observation and never
enters the physical executor. Fast-forward, true-merge, expected-conflict and
resolved-conflict results are accepted only through the matching persisted
action and exact live reconciliation.

The failure footprint now derives its participant fields from the actual
semantic change. An initial failure may write outcome and error, while a
repeated `Failed -> Failed` retry can replace only the error and operation
state. Resolution failure still cannot mutate authoritative conflict outcome
fields. Focused footprint tests execute both ordinary and owned-retry variants.

### Conflict, Continue, and preparation drift are fail-closed

Expected native conflict is accepted only when HEAD, branch, `MERGE_HEAD`,
index conflict stages and worktree form match the deterministic original
merge. The durable conflict snapshot contains the exact normalized path and
SHA-256 of each ordinary conflict-marker file. The concrete conflict test
compares the persisted path and hash to the live bytes.

Continue over an unresolved conflict returns `MergeValidationFailed`, leaves
the operation in `AwaitingResolution`, installs no pending owner, and performs
no Git mutation. Once every conflict is resolved and staged, preparation
freezes the exact resolution commit and the checked resolution primitive
revalidates the same parents, index, worktree, signatures and message.

Semantic preparation drift before owner persistence constructs ambiguity from
the current checked record. It enters `RecoveryRequired` with literal
`Executing` origin, leaves the participant owner absent, preserves Git and
record evidence, and performs no physical action. Operational failures remain
ordinary typed failures rather than being relabeled as semantic ambiguity.

### Recovery uses live origin-specific evidence

Recovery dispatch first matches the exact durable origin:

- `Executing`, `AwaitingResolution`, and `Halted` use forward participant
  verification;
- `Finalizing` delegates to the accepted R4b-F exact acceptance/publication
  verifier; and
- `Preserving` and `RollingBack` are rejected by `ForwardRuntime` because
  their exact journal verification belongs to R4b-P.

A pending participant is reconciled through its exact durable action. Recovery
then still observes every other selected participant; it does not return after
checking only the owner. The cross-member regression proves that an exact
pending owner cannot mask drift in another selected repository and that
rejection leaves the record byte-identical. Between-owner recovery verifies
every selected participant against the state-specific continue or abort
predicate. Only after all live evidence matches does the observer issue
`VerifiedRecoveryOrigin`; the transition merely restores the recorded literal
origin and clears recovery context.

### Participant paths and checked execution are safe

Durable member paths are reparsed as `MemberPath` values and every existing
component is checked with `symlink_metadata`. A symlink or non-directory in
the repository path is rejected before Git execution, with member context,
and the external repository remains unchanged.

The service acquires one retained `V1MutationLease`, reloads the checked record
under that lease, persists and rereads the complete participant owner before
execution, and requires byte-identical record reload after the action. The
executor verifies the persisted action again immediately before invoking only
checked Git primitives. Restart adopts an exact completed result, retries only
an exact not-started action, and sends every mixed state to recovery.

## Authority, architecture, and version isolation

The concrete runtime is a thin observer/executor adapter around the accepted
dispatcher and reducer interfaces. It does not introduce a second service
loop, mutable v1 record escape hatch, generic writer, phase graph, action kind,
or recovery state. Observation proofs remain bound to the checked record,
request, owner and source digest; physical authority remains bound to the same
record and retained lease.

The v1 lifecycle remains reachable only through `#[cfg(test)] mod
v1_lifecycle`. Searches of the reviewed owners find no v0 common-view
projection, v0 persistence call, raw v1 writer, upgrader, or production
lifecycle dispatch. The shared Git change preserves released normal behavior
through the existing allow-fast-forward preparation path. R4b-X adds no wire
field, phase, transition, production writer, CLI behavior, or protocol delta.

## Budget and cohesion review

The conservative production charge is **987 lines across 13 production
paths**, calculated as follows:

| Charge | Lines |
| --- | ---: |
| Dedicated forward owners (`610 + 87 + 101`) | 798 |
| Finalization recovery delegation added to the accepted F observer | 33 |
| Git prepared-mode/prepared-result path (`34 + 1 + 35 + 17`) | 87 |
| Symlink-safe participant status path | 23 |
| Lifecycle/authority module wiring | 5 |
| Whole-function conservative charge for repeated-failure footprint | 41 |
| **Total** | **987** |

The 13 paths are the three dedicated forward owners; lifecycle, authority and
observer wiring; the finalization observer; transition footprint; four Git
backend/contract owners; and participant status observation. No production
code was relocated, so moved-production charge is **0/1,100**.

Focused test Rust is **976 lines across four paths**: 826 forward tests, 71
failure-footprint tests, 77 Git no-ff tests, and two module declarations. This
review memo is **194 lines**, placing the conservative focused test/tool/doc
charge at **1,170/1,500** across five paths. The
production and evidence path counts are within 13/14 and 5/12 respectively.

The revised general god-file policy uses 1,000 lines as a judgment trigger,
allows a small cohesion-justified overrun, and targets owners below 500 only
when a split is architecturally warranted. The 610-line forward observer is a
single exact participant/recovery observation boundary; the 826-line focused
test owner is one concrete runtime fixture and restart suite. Both remain
below the trigger and are cohesive. Splitting either solely for size would
scatter shared fixtures or proof classification without creating an
independent responsibility. No split is required. The physical executor is
101 lines and the shared Git owners are 455 and 393 lines.

The original 900-line estimate omitted exact finalization-origin delegation,
mode-aware no-ff preparation and frozen-result execution, cross-participant
recovery verification, path-component safety, and the corrected failure
footprint. These are required by the accepted architecture and add no feature
scope. A 1,050-line ceiling leaves 63 lines of review margin and is the
smallest practical rounded correction; it is not tolerance for R4b-P/G work.

## Verification

The exact settled tree passed:

- concrete R4b-X forward runtime: 16/16;
- scoped Git G12 prepared-merge suite: 16/16;
- complete `gwz-core` library: 925 passed, 1 ignored;
- production-only `cargo check -p gwz-core --lib`;
- strict `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`;
- `cargo fmt --all -- --check`;
- root and `gwz-core` `git diff --check`;
- merge-document consistency: 8 sources and 87 assertions; and
- merge checker unit suite: 17/17.

## Final disposition

**GO. P0: 0; P1: 0; P2: 0; P3: 0.** R4b-X now has one checked participant
execution and Continue architecture, exact prepared-result no-ff semantics,
origin-specific whole-selection recovery evidence, symlink-safe participant
paths, exact restart adoption, preserved v0 behavior, and no normal-build
activation. The reviewed 1,050-line production ceiling should be recorded
before R4b-P consumes this interface.
