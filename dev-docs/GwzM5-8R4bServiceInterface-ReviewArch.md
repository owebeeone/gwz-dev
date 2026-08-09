# R4b-F/X service architecture and interface review

## Verdict

**GO. P0: none; P1: none; P2: none; P3: none.** The settled R4b-F/X
service checkpoint satisfies the architecture and interface contracts reviewed
from `GwzM5-8R4bTransitionDesign.md` §§3, 4, 7--9, and 12--14. It is suitable
for the later R4b-F/X/P observer and executor adapters.

This was a read-only implementation review. The only file authored by this
reviewer is this memo.

## Scope

The direct review covered:

- `v1_lifecycle/service.rs` and its `service/execution.rs` helper;
- `v1_lifecycle/tests/service.rs` and `tests/service_sequence.rs`;
- `BoundPhysicalAction::authorize` and attempt construction;
- the Continue-only `Conflicted` participant selector;
- the authority-owned `V1Invocation` refinement; and
- the checked reload and outcome-footprint dependencies required to validate
  the complete service sequence.

The review focused on retained lease lifetime, exclusive checked-store use,
proof and attempt replay resistance, once-per-invocation execution, commit and
reload ordering, policy ownership, termination, and downstream interface
compatibility.

## Lease and checked-store boundary

Read-only dispositions are classified without taking the mutator lock. Once an
invocation requires mutation, `run` acquires the canonical-root
`V1MutationLease`, reloads the open record, and retains that lease until the
service returns. The unlocked classification record is never used to prepare,
commit, authorize, or execute a mutation. Source inspection finds exactly one
non-test lease-acquisition call, in the service.

Every transition is still `prepare(&lease, &current, transition)` followed by
`store.commit(&lease, &current, rewrite)`. The loop always adopts the checked
record returned by the commit before another dispatch or observation. There is
no raw YAML/model writer, mutable-record escape, or v0 persistence path in the
consumer.

Every mutating executor receives the same retained lease. Archive is handled
through `CheckedV1Store::archive`, including both the physical-action route and
the closed `ArchiveReady` response route; the service never performs a raw
rename or serialization. This leaves the later P consumer one checked archive
primitive and no second archive authority.

After a physical action, `reload_unchanged` reopens from the checked canonical
location and rejects unless digest, location, typed record, raw YAML, and
unknown-field manifest all still match. Thus an executor cannot rewrite the
record between execution and observation, while a same-byte source replacement
retains the identical durable authority required by the store model.

## Physical-action and attempt authority

`BoundPhysicalAction::authorize` revalidates the sealed binding against the
current checked record, stable observation owner, `execute` action, and
`authorized` phase. `record_attempt` repeats that check while consuming the
action capability. The service exposes only the resulting exact action to the
executor and retains only the bound diagnostic attempt afterward.

The attempt is passed only to the immediate `resolve_observation` call and is
removed with `Option::take`. A later record digest, owner, action, request, or
phase cannot replay it. Executor success or failure remains diagnostic only:
fresh `Completed` observation is outcome authority, `Ambiguous` is recovery
authority, and matching `NotStarted` is the only basis for owned failure or
no-progress handling.

`V1Invocation::before_execute` compares the complete physical action identity
and rejects a second execution in the same service call. Preparation owners are
similarly fenced. Invocation history is non-durable and discarded on return,
so it prevents a local retry loop without preventing a later invocation from
reobserving a retained durable owner.

## Ordering and liveness

The service is a closed interpreter over dispatcher actions:

1. `Observe` obtains one bound fact and resolves it;
2. `Apply` prepares and checked-commits, then runs the authority-owned stop
   check;
3. `Execute` reauthorizes, fences, executes once, binds the diagnostic, and
   reloads the unchanged source before fresh observation; and
4. `Respond` or `Reject` returns without redispatch.

There is no independent operation-state or phase match in the consumer.
Invocation-only history and stop policy live beside the dispatcher in
`authority/dispatcher/invocation.rs`, as now required by design §§7 and 13.
The consumer's only physical-kind branch selects the checked archive primitive;
it does not choose lifecycle order or fabricate a record transition.

The settled stop checks close every local durable cycle relevant to this
checkpoint:

- preparation failure and owned participant failure commit `Halted` and
  return;
- a newly conflicted retry continues later frozen targets once, then commits
  and returns `AwaitingResolution`;
- newly entered `RecoveryRequired` returns instead of immediately resuming;
- `ResumeStart` over an existing conflict commits/returns the stopped state
  without resolving it;
- a completed retained halted owner uses the exact halted-outcome compound and
  resumes without an invalid intermediate record; and
- successful reconciliation of a retried `Failed` owner retires its prior
  error under the exact declared footprint for all five result shapes.

Success with no observable progress rejects with the durable owner retained;
failure plus matching participant `NotStarted` commits one failure/halt batch;
neither path can execute the same action again in the current invocation.

## Continue selector and downstream compatibility

The durable selector admits `Planned`, `Failed`, and `Unattempted` for both
forward requests, but admits `Conflicted` only for `Continue`. `ResumeStart`
therefore preserves released start behavior: it skips an existing conflict,
continues later targets in immutable order, and stops at awaiting resolution.
`Continue` alone selects conflict resolution.

The invocation refinement handles the distinct case where a retry creates a
new conflict: it skips only the already visited owner for the remainder of that
call, continues later eligible targets in order, and forgets the visit fence on
restart. It cannot create a proof, authorize execution, reorder targets, or
change durable state without returning a typed transition to the kernel.

The observer interface receives a checked record and bound observation
request. The executor interface receives the retained lease, checked record,
and exact authorized action. These interfaces cover participant, publication,
preservation, rollback, and archive action kinds without exposing record
construction or persistence, so later F/X/P adapters do not need consumer-side
state policy or a wider capability.

## Budget and ownership

A conservative charge of the complete new service owners is 372 production
lines (`service.rs` 156, `service/execution.rs` 47, and the authority invocation
guard 169), before small checked-store, selector, footprint, and wiring deltas.
This remains comfortably below both the R4b-F 950-line and R4b-X 900-line
production ceilings even when the shared interface is charged to either
checkpoint. The extracted production movement is far below each 1,100-line
ceiling, and the affected production-path set is below 14.

The two service suites total 916 lines and the focused retry-effect sibling is
46 lines, below either 1,500-line test/tool/doc ceiling and below 12 paths.
Every affected responsibility owner is now below 500 lines: the largest scoped
test owner is `service_sequence.rs` at 475, `effect.rs` is 476 after its focused
split, and the largest affected production owner is the pre-existing
`authority.rs` boundary at 495.

## Findings closed during review

The moving checkpoint initially exposed and then closed:

- lease authority not reaching the physical executor;
- archive execution bypassing the service's checked-store sequence;
- same-invocation halt, awaiting-resolution, and recovery redispatch loops;
- newly conflicted retries blocking later frozen participants;
- changed record bytes reaching post-execution resolution;
- completed halted owners being stranded under `ResumeStart`;
- an incomplete effect footprint when a successful retry cleared its prior
  error; and
- the retry-effect test owner crossing the 500-line ownership limit.

The final tree contains focused regression coverage for each interface edge.

## Verification

Executed on the final settled bytes:

- `cargo test -p gwz-core v1_lifecycle::service --lib`: 9 passed;
- `cargo test -p gwz-core v1_lifecycle --lib`: 76 passed;
- `cargo test -p gwz-core --lib`: 879 passed, 1 ignored;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed;
- `cargo fmt --all -- --check`: passed;
- root and `gwz-core` `git diff --check`: passed;
- merge document consistency: 8 sources / 87 assertions passed; and
- merge checker tests: 17 passed.

## Gate decision

**GO for the next R4b interface checkpoint.** Finding count: **P0 0 / P1 0 /
P2 0 / P3 0**.

## Addendum — 2026-08-10 — sequential publication actions

### Re-review verdict

**GO. P0: none; P1: none; P2: none; P3: none.** A concrete-finalizer
integration exposed one legitimate service sequence not represented by the
original attempt rule: the marker, lock, boundary, and index mutations are
distinct physical actions owned by one durable `PublishingCandidate` record.
The settled correction admits that sequence without weakening record, owner,
request, completion, ambiguity, or once-per-invocation authority.

### Exact authority boundary

An attempted action still has to match the fresh observation request's checked
record digest, workspace/merge/operation identity, lifecycle request,
observation kind, and stable owner. `resolve_observation` relaxes only physical
action equality, and only when the fresh classification is `NotStarted`.

That relaxation is not caller-selected. `BoundExactObservation` seals the
newly observed physical action into its payload, and `resolve_physical`
independently verifies the action-specific proof, phase, current publication
shape, and observation kind. Under unchanged record bytes, participant,
preservation, rollback, and archive owners still resolve only their exact
persisted action. Publication is the one owner whose verified live prefix can
legitimately select a different next marker/lock/boundary/index action without
an intervening record rewrite.

If the fresh `NotStarted` action equals the attempted action, the resolver
still routes to `no_progress`: participant failure can construct only its
bound failure/halt batch, and all other cases reject with the durable owner
unchanged. If the sealed next action is different, the resolver issues a new
`BoundPhysicalAction`; the prior attempt is consumed by the resolver and the
service replaces it only after executing that new capability.

`V1Invocation::before_execute` continues to fence the complete action
identity against the whole invocation history. It permits the required
`WriteMarker -> WriteLock` handoff, rejects an immediate repeated action, and
also rejects a later `WriteMarker -> WriteLock -> WriteMarker` cycle. A new
invocation forgets only this local history and must reconstruct authority from
the durable record and fresh exact observation.

### Completion and ambiguity remain exact

The physical-action relaxation does not apply to `Completed`, `Ambiguous`, or
`PreservationAmbiguous`. Those classifications must carry a physical action,
and `BoundExecutionAttempt::matches` still requires that exact action in
addition to the record/request/owner binding. A completed publication fact
bound to `WriteLock` therefore rejects a retained `WriteMarker` attempt.
Ambiguity continues to override either executor diagnostic only for the exact
attempted participant, publication, preservation, or rollback action; it
cannot use the same-owner publication exception.

The updated design §7 now states the same boundary explicitly: same-action
`NotStarted` is no progress; a different action may proceed only from a sealed
exact-prefix observer under the same record-bound owner, and diagnostics or
callers cannot choose the successor.

### Ownership and verification

The focused sequence coverage was split into
`tests/publication_attempt_sequence.rs` (222 lines), leaving
`dispatcher_reconciliation.rs` at 377 lines. `authority/resolver.rs` is 439
lines. All affected responsibility owners remain below 500 lines.

Executed on the settled addendum snapshot:

- `cargo test -p gwz-core
  v1_lifecycle::tests::publication_attempt_sequence --lib`: 4 passed;
- `cargo test -p gwz-core
  v1_lifecycle::tests::dispatcher_reconciliation --lib`: 5 passed;
- `cargo test -p gwz-core
  v1_lifecycle::tests::dispatcher_attempt_matrix --lib`: 3 passed;
- `cargo test -p gwz-core v1_lifecycle::service --lib`: 9 passed;
- `cargo test -p gwz-core v1_lifecycle --lib`: 79 passed;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed; and
- `cargo fmt --all -- --check`: passed.

The sequential-publication correction is accepted with finding count **P0 0 /
P1 0 / P2 0 / P3 0**.
