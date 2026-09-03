# R4b-F/X service interface state-machine review

## Verdict

**GO.** The settled R4b-F/X checkpoint has no open P0, P1, P2, or P3
finding in the reviewed interface. The service terminates at every durable stop
boundary, cannot execute one retained owner twice in one invocation, preserves
the exact attempt/observation authority rules, and preserves released Start
behavior through `ResumeStart`.

This was a read-only implementation review. The only file authored by the
reviewer is this memo.

## Scope and basis

The primary scope was:

- `v1_lifecycle/service.rs` and its extracted `service/execution.rs` helper;
- `v1_lifecycle/tests/service.rs` and the settled
  `tests/service_sequence.rs` sequence matrix;
- `BoundPhysicalAction::authorize` and `record_attempt`;
- the Continue-only `Conflicted` selection in `next_forward_participant`; and
- the authority-owned `V1Invocation` guard that now refines the pure durable
  dispatcher.

The direct store reload, resolver, and outcome-footprint dependencies were
also inspected because they close the physical-execution and retry-result
boundaries required by design sections 6.2, 7--9, and 12--14.

## Closed-loop result

| Service branch | Settled behavior | Liveness result |
| --- | --- | --- |
| `Respond` | Returns the checked record and closed disposition. Archive completion first executes the checked archive primitive. | Terminates. |
| `Reject` | Returns the typed error without another dispatch. | Terminates. |
| Direct `Apply` | Prepares and checked-commits the typed transition, runs the invocation stop check, then redispatches only if work remains. | `AwaitingResolution`, invocation-owned `Halted`, and newly entered `RecoveryRequired` stop; other legal transitions strictly advance. |
| Observation-produced `Apply` | Uses the same prepare/commit/stop sequence as direct `Apply`. | No separate consumer policy or unguarded retry path exists. |
| `Execute` | Reauthorizes the exact bound action against the current record, checks the invocation execution fence, executes once, records one bound attempt, and reloads the exact same source bytes before redispatch. | A different post-execution record rejects; an identical source proceeds to the immediate observation. |

The only durable back-edges are deliberately bounded:

- `AwaitingResolution/Halted -> Executing` begins a new Continue attempt;
- an ambiguity with an executing retained halt cause commits `Halted`, obtains
  a fresh observation under that digest, commits
  `RecoveryRequired(origin=Halted)`, and stops; and
- a later invocation may verify and resume a recovery origin, but a newly
  entered recovery never resumes again inside the same invocation.

The invocation guard closes the otherwise possible local cycles. It stops
after a preparation or owned-action failure, after the newly-conflicted
sequence commits `AwaitingResolution`, and immediately after recovery entry.
It forgets its visit history on restart, so a fresh invocation may perform an
intended retry without weakening durable owner ordering.

## Observation and attempt matrix

The bound attempt is accepted only by the immediate fresh observation for the
same request, owner, action, phase, record digest, and physical action. It is
consumed once by `resolve_observation`.

| Fresh observation | No attempt | Successful executor diagnostic | Failed executor diagnostic |
| --- | --- | --- | --- |
| `Completed` | Applies the typed result/progress transition. | Applies the observed result; the diagnostic is not result authority. | Applies the observed result; a late executor error is ignored. |
| `Ambiguous` | Enters representable recovery, or first commits the required `Halted` predecessor. | Same; success cannot fabricate completion. | Same; the diagnostic is not recorded as an outcome. |
| `NotStarted` | Resolves one bound physical action. | Rejects no progress with the persisted owner unchanged. | For a participant, constructs the exact owned failure/halt batch; for a non-participant, rejects with the owner unchanged. |

Abort/preserve abandonment remains separate: it requires a matching bound
participant `NotStarted` proof, a matching prepared entry, and no execution
attempt. It cannot be reached from an executor diagnostic.

Preparation is also bounded. A successful preparation persists the complete
participant owner before classification. A preparation failure atomically
writes `Failed`, the exact error, the later `Unattempted` suffix, and `Halted`,
then returns a stopped disposition. A repeated preparation of the same owner
in one invocation is rejected unless the invocation dispatcher is handling the
defined newly-conflicted sequence by continuing later frozen targets.

## Physical owner and byte-lineage checks

`BoundPhysicalAction::authorize` accepts only a capability whose sealed binding
still matches the current checked record, stable owner, `execute` action, and
`authorized` phase. `record_attempt` repeats that exact check before consuming
the capability and issuing the diagnostic-bearing attempt token.

The invocation execution fence compares the complete physical action identity
and refuses a second execution. After the first execution,
`reload_unchanged` requires byte digest, canonical location, typed record, raw
YAML, and unknown-field manifest to remain identical. This prevents a changed
record from reaching `Respond`, `Apply`, or a different observation before the
attempt is validated. A same-byte replacement retains identical authority as
required by the checked-store threat model.

The result matrix then guarantees:

- success plus `Completed` advances the owner;
- failure plus `Completed` also advances from observation authority;
- either diagnostic plus `Ambiguous` stops in recovery;
- success plus `NotStarted` rejects without retry;
- participant failure plus `NotStarted` durably halts with the owner retained;
  and
- a subsequent invocation may retry a still-`NotStarted` retained owner, but
  the current invocation cannot execute it twice.

## Continue and ResumeStart behavior

The durable selector includes `Planned`, `Failed`, and `Unattempted` for both
forward requests, and includes `Conflicted` only for `Continue`. Consequently,
`ResumeStart` skips an existing conflict, continues later members and the root
in frozen order, commits `AwaitingResolution`, and returns the stopped result.
`Continue` selects the conflict for resolution.

The invocation refinement also preserves the released case where a retry
creates a new conflict: that newly conflicted owner is not prepared again in
the same call; later eligible targets still run, and only then does the service
commit and return `AwaitingResolution`.

No Start regression remains:

- a conflict is a successful stopped response, not a service error;
- a preparation or physical-action failure is a successful `Halted` response;
- a stopped `ResumeStart` returns without redispatching;
- a completed retained owner in `Halted` is reconciled through the exact
  halted-outcome-and-resume compound, avoiding an invalid action-free halted
  intermediate; and
- the released conflict-continues and root-last characterization tests remain
  green.

## Findings closed during review

The settled snapshot closes every blocking sequence found while the interface
was moving:

- the preparation-failure `Executing -> Halted -> Executing` loop;
- stopped `ResumeStart` being redispatched as an illegal request;
- re-preparation of a newly conflicted retry before later targets;
- the direct `AwaitResolution -> BeginExecution` same-invocation cycle;
- recovery entry being resumed in the same invocation;
- changed record bytes bypassing attempt validation after execution;
- completed retained-owner reconciliation under `ResumeStart`; and
- retry outcome rejection when clearing the prior participant error.

The design now explicitly owns these rules in the authority-layer invocation
guard and includes the invocation-history and source-reload requirements in
sections 7 and 13. The five participant result shapes also have exact footprint
coverage for prior-error retirement.

## Verification

All gates passed on the settled snapshot:

- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle -- --nocapture`
  -- **76 passed**;
- `cargo test -p gwz-core workspace_ops::merge::start::execution_tests -- --nocapture`
  -- **3 passed**;
- `cargo test -p gwz-core workspace_ops::tests::g23::root_start::member_conflict_does_not_prevent_root_last_execution -- --nocapture`
  -- **1 passed**;
- `cargo fmt --all -- --check` -- passed; and
- `cargo clippy -p gwz-core --all-targets -- -D warnings` -- passed.

Final decision: **GO for the next R4b interface checkpoint.**

## Addendum — 2026-08-10 sequential publication re-review

### Verdict

**GO.** The corrected shared R4b service interface has no open P0, P1, P2,
or P3 finding for same-owner sequential publication. A fresh exact
`NotStarted` observation may authorize the next marker/lock/boundary/index
action without a record rewrite, but only through observer-sealed authority.
The same physical action remains a no-progress result and cannot be executed
again in the invocation.

### State-machine and restart result

The exception is narrow and preserves the durable owner boundary:

- the retained attempt must still match the exact source digest,
  workspace/merge/operation identity, lifecycle request, observation kind,
  and `@publication` owner;
- only `NotStarted` may omit equality with the attempted physical action;
  `Completed` and `Ambiguous` retain exact attempted-action matching;
- the replacement action is re-derived by `resolve_physical`, which requires
  a matching `VerifiedPublicationAction`, its exact action-specific phase,
  and the current `PublishingCandidate` record with candidate and evidence;
- an immediate repeat of the attempted action enters `no_progress` before a
  new execution capability is issued; a non-adjacent repeat is rejected by
  `V1Invocation`'s full physical-action history; and
- after a real restart the in-memory history is intentionally absent, but the
  durable record and a fresh exact prefix/index classification remain the
  sole authority for the next safe action.

The service consumes the prior attempt during resolution. If a different
sealed successor is authorized, `execute_owned` reauthorizes it against the
same checked record, executes it once, records the new attempt, and
`reload_unchanged` proves that the durable owner bytes did not change. The
sequence therefore permits physical progress within one durable
`PublishingCandidate` owner without creating an unrecorded cursor or allowing
an executor diagnostic to choose the successor.

Caller forgery remains closed. `VerifiedPublicationAction::issue` is private
to the authority module, the proof binding and `BoundExactObservation` fields
are private, and the widening constructors are `cfg(test)` only. The concrete
finalization observer derives the action from the exact file-prefix and
index/worktree classifier before issuing the token; the runtime caller can
neither supply a raw successor action nor mint the sealed observation.

### Completion edge closed during re-review

The concrete observer initially exposed one P1 completion defect: after a
successful `StageIndex`, production `CandidatePublished` carried no physical
identity, so the retained attempt was rejected and a second invocation was
needed to record completion. The settled interface now derives exactly
`StageIndex` from `CandidatePublished`. A matching stage attempt applies the
completion transition, while a marker/lock/boundary attempt cannot consume
that completion. This closure does not widen the different-action
`NotStarted` exception.

The transition design section 7 now states the same contract: identical
post-attempt `NotStarted` is no progress, while a different action under the
same record-bound owner requires a sealed proof of the intervening exact
physical prefix. Callers and executor diagnostics cannot select the action.

### Findings and verification

| Severity | Open findings |
| --- | --- |
| P0 | None |
| P1 | None |
| P2 | None |
| P3 | None |

Strict gates on the coherent settled tree:

- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle::tests::publication_attempt_sequence -- --nocapture`
  — **4 passed**;
- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle::tests::dispatcher_attempt_matrix -- --nocapture`
  — **3 passed**;
- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle -- --nocapture`
  — **79 passed**;
- `cargo fmt --all -- --check` — passed; and
- `cargo clippy -p gwz-core --all-targets -- -D warnings` — passed.

Final addendum decision: **GO with no open P0-P3 finding.**
