# GWZ M5–M8 R1 Ownership Table

Status: **R1 implemented and independently reviewed; integration gate green.**
R0 was accepted by exact-SHA retained-reader run `30765936845` against core
commit `b7b49b4d05009e71a3606d2566febacd88d3a767`.

R1 has no intentional runtime, wire, record, event, error, or mutation-order
delta. Every Rust move uses the installed `rust-split` workflow from
`GwzGodFileRefactorPlan.md`, with its explode/round-trip evidence retained in
the package handoff.

## Frozen interfaces

- `handle_merge` and `handle_merge_with_events` signatures and lifecycle event
  order;
- `MergeStore` methods, default unsupported errors, and visibility;
- `WorkspaceMutationGuard` public behavior and lock/open-gate ordering;
- v0 record bytes, unknown-field carry-forward, and archive rename behavior;
- `OperationState` and `ParticipantState` variants and transition relations;
- Git/filesystem mutation order; and
- all protocol, Rust CLI, Python, human, JSON, and JSONL projections.

An owner that needs one of these changed stops and returns the request to the
lead. R1 does not introduce a replacement model or a compatibility adapter.

## Execution order and ownership

| Task | Owner and writable files | Read-only dependencies | Tests | Handoff |
| --- | --- | --- | --- | --- |
| R1-A runtime move | runtime owner: `merge/mod.rs`, new `merge/runtime/{mod,dispatch,open_gate,mutation_guard}.rs` | request validation, recovery discovery, operation gate, lock implementation | existing `merge/mod.rs` tests plus focused gate/mutation tests moved with their owner | mechanical move first; no call-site policy rewrite |
| R1-B store move | store owner after R1-A: `merge/mod.rs`, `merge/store.rs`, `merge/store/**`, new `merge/store/persistence.rs` | durable-fs helpers, model types, event emitter | store unit tests and archive/retention/GC tests | move `MergeStore`, filesystem store, and eventful persistence helpers without byte changes |
| R1-C participant policy | policy owner, parallel after R1-A declaration: new `merge/participant_semantics/**` and only the named lifecycle call sites assigned in its subtask | `model.rs`, R0 transition/action tables, status observations | exhaustive pure tables plus unchanged lifecycle tests | centralize one decision at a time; no orchestration or mutation code moves |
| R1-D integration | lead: module declarations/re-exports, package ledger, inventory closure note | all R1 handoffs | full core/CLI/Python/protocol/format/clippy/document gates | verify exact behavior and report moved LOC separately |

R1-A and R1-B are sequential because both edit `merge/mod.rs`. R1-C may run in
parallel only after the lead has registered and frozen its module boundary; its
subtasks receive disjoint lifecycle call-site files. The lead alone resolves
imports or visibility at integration.

## Participant-policy subtask split

The R0 inventory is the decision catalogue. Subtasks are assigned only after a
lead-owned pure API table names the input, output, and exhaustive state rows.

| Subtask | Intended decision owner | Permitted call-site family |
| --- | --- | --- |
| R1-C1 result/finalization | `participant_semantics/result.rs` | response counts/wire state, finalization, marker validation, root finalization validation, and preservation result ownership; it supplies the result classifier consumed by the C4-owned root-abort call site |
| R1-C2 continue eligibility | `participant_semantics/continue_eligibility.rs` | start/continue aggregate state, continue eligibility, and retry planning |
| R1-C3 abort/rollback | `participant_semantics/rollback.rs` | abort eligibility/preflight, pending-adoption handoff, and participant rollback |
| R1-C4 status projection | `participant_semantics/status.rs` | status state policy, ordered drift/facts, missing repositories, all `root/abort.rs` edits and root status overrides, and conflicted-target stage admission |

No subtask edits `model.rs`, `merge/mod.rs`, another subtask's call sites, Git
backend code, durable storage, or driver code. Shared functions are proposed to
the lead rather than duplicated.

## Lead-owned participant API checkpoint

This checkpoint freezes R1-C after the R1-A runtime boundary exists and
incorporates both independent interface reviews. The new types are
crate-private pure policy; they are not record, protocol, driver, or backend
interfaces. Every state function exhaustively matches all ten existing
`ParticipantState` variants and adds no fallback arm. `model.rs` remains the
single owner of transition legality and record construction; R1-C does not
move or replace those decisions.

### Base state table

The base table is necessary but does not override the precedence and recovery
tables below.

| State | Continue | Rollback | Expected status head | Head-drift guidance | Root attempted | Changed result |
| --- | --- | --- | --- | --- | --- | --- |
| `Planned` | retry integration | mark aborted, no Git mutation | before commit | restore before commit or abort | no | no |
| `UpToDate` | settled | mark aborted, no Git mutation | resulting commit | restore recorded result | yes | no |
| `FastForwarded` | settled | reset integrated result, then rolled back | resulting commit | restore recorded result | yes | yes only when result differs from before |
| `Merged` | settled | reset integrated result, then rolled back | resulting commit | restore recorded result | yes | yes only when result differs from before |
| `Conflicted` | resolve conflict | abort native merge, then aborted | before commit | restore recorded result | yes | no |
| `Failed` | retry integration | mark aborted, no Git mutation | before commit | restore before commit or abort | yes | no |
| `Unattempted` | retry integration | mark aborted, no Git mutation | before commit | restore before commit or abort | no | no |
| `Continued` | settled | reset integrated result, then rolled back | resulting commit | restore recorded result | yes | yes only when result differs from before |
| `Aborted` | reject terminal | already complete | before commit | restore recorded result | yes | no |
| `RolledBack` | reject terminal | already complete | before commit | restore recorded result | yes | no |

`result.rs` owns the exhaustive state-to-wire/count mapping and typed result
classes used by all success/integration/conflict predicates. Its immutable
record helper identifies a changed result only for `FastForwarded`, `Merged`,
or `Continued` when `resulting_commit != before_commit`. Marker validation and
root finalization accept exactly `UpToDate`, `FastForwarded`, `Merged`, and
`Continued`; preservation owns work only for the three integrated-result
states. The existing root-evidence rollback success predicate uses that same
four-state success class. No caller keeps a parallel state match.

### Continue policy and aggregate precedence

`continue_eligibility.rs` returns
`ContinueDisposition::{ResolveConflict, RetryIntegration, Settled,
RejectedTerminal}` for the base table. It also owns two separate aggregate
functions over raw `record.participants.values()`; neither aggregate derives
from `ContinueDisposition`:

| Aggregate | First precedence | Second precedence | Otherwise |
| --- | --- | --- | --- |
| post-start | any `Failed` → `Halted` | any `Conflicted` → `AwaitingResolution` | `Finalizing` |
| post-continue | any `Failed` → `Halted` | any `Planned`, `Unattempted`, or `Conflicted` → `AwaitingResolution` | `Finalizing` |

`Failed` wins in every mixed row. Both exhaustive single-state tests and mixed
precedence tests are required. The domain remains every durable participant,
not only `selected_targets`.

Continue caller ordering remains:

1. reconcile and durably adopt pending actions;
2. build the status snapshot;
3. reject operation drift before participant eligibility;
4. reject participant eligibility before pending/state dispatch;
5. dispatch any still-pending durable action before ordinary state dispatch;
6. after execution, reject observed drift before aggregate-state selection.

Thus a `Settled` state never bypasses live drift and pending-action adoption
can change the state seen by the base table.

### Rollback policy and abort precedence

`rollback.rs` owns these pure types:

```rust
enum RollbackClass {
    RequiresExactBeforeNoGit, // Planned, Failed
    NoOwnedMutation,          // UpToDate, Unattempted
    NativeConflict,           // Conflicted
    IntegratedResult,         // FastForwarded, Merged, Continued
    Complete,                 // Aborted, RolledBack
}

enum AbortPreflightDecision { Reject, Proceed, AlreadyApplied }
enum RollbackGitAction { None, AbortConflict, ResetIntegrated }
struct ParticipantRollbackDecision {
    git_action: RollbackGitAction,
    terminal_state: Option<ParticipantState>,
}
```

`RequiresExactBeforeNoGit` is distinct from `NoOwnedMutation`: both eventually
make no Git rollback call, but only `UpToDate` and `Unattempted` tolerate a
missing repository. `ParticipantRollbackDecision` suppresses Git when
preflight proved an action already applied while retaining the base terminal
state: externally restored conflict still becomes `Aborted`; an externally
restored integrated result still becomes `RolledBack`; terminal rows have no
next state.

`AbortPreflightDecision` preserves the exact current order:

1. ineligible is always `Reject`, including a terminal row;
2. eligible `Aborted`/`RolledBack` is `AlreadyApplied`;
3. all other already-applied checks require live commit equal to
   `before_commit` and a non-empty drift list;
4. `Conflicted` is already applied only when every drift is
   `MergeStateMissing`;
5. `FastForwarded`/`Merged`/`Continued` is already applied only when the
   observed operation state was already `RollingBack` and every drift is
   `TargetRefChanged` or `HeadRewound`;
6. all other eligible rows are `Proceed`.

The observed operation state is captured before any new transition to
`RollingBack`. Empty drift never passes through vacuous `all()`.

Abort pending-action precedence is mandatory:

0. operation drift rejects before any participant inspection, preserving its
   error and lack of member context;
1. status classifies every pending action before ordinary eligibility/no-op;
2. ambiguous reconciliation rejects before record or Git mutation;
3. pending targets are excluded from ordinary verified-no-op classification;
4. exact completion/conflict is adopted into the participant state and
   persisted before rollback;
5. rollback classification uses the adopted state, so a completed pending
   true merge becomes `Merged` and is reset rather than merely marked aborted;
6. Git action, transition validation, per-row persistence, and member-event
   ordering remain in abort orchestration.

### Status facts, missing repositories, and overrides

`status.rs` owns an exhaustive `StatusStatePolicy` containing typed expected
head source, conflict role, head-drift guidance, and root-attempted role. It
also projects ordered drift and normalized recovery facts; it does not call a
backend. Observation code supplies Git results and these two distinct typed
conflict-validation outcomes:

- native conflict is valid for abort; and
- the current resolution is valid for commit.

Each invalid outcome retains its backend error text, appends the existing
`IndexModified` drift in the current position, and disables the same
eligibility fields as today. Resolution validation remains conditional on the
base continue decision and successful abort validation.

Ordinary drift order remains: missing objects, branch, target ref, HEAD
relation, repository/native operation, index, then worktree. Duplicate drift
kinds remain unless the existing `push_once` overlay explicitly deduplicates
a blocker. Continue and abort blocker ordering remains derived from that drift
order, with the conflict-resolution `IndexModified` overlay appended once.

Missing-repository projection is frozen separately from ordinary expected-head
policy:

| State | Abort eligible without pending action | Abort eligible with pending action | Continue eligible |
| --- | --- | --- | --- |
| `UpToDate`, `Unattempted` | yes | no | no |
| every other state | no | no | no |

Every missing row reports `before_commit` as expected head, including the four
ordinary result-bearing states. With a pending action, drift order is
`RepositoryMissing` then `PendingActionAmbiguous`, while both eligibility
blocker lists contain only `RepositoryMissing`. All 20 state × pending-action
cells are table-tested.

The following verified root reconciliations remain explicit typed overrides,
not ordinary participant inference:

- exact root finalization clears conflict paths/drift and forces both continue
  and abort eligible; and
- exact interrupted root evidence rollback clears the operation-level root
  candidate drift and its observation normalizer forces abort eligible while
  preserving the current continue projection.

The root override helpers live under the status semantic owner and are called
only after their existing root-specific evidence proofs succeed. The
conflicted-target stage gate consumes the same status conflict role rather
than retaining another equality test. R1-C4 alone edits `root/abort.rs`; it
consumes C1's frozen success-result classifier without giving C1 overlapping
call-site ownership.

### Test and mutation boundary

Each policy module receives an exhaustive ten-row table. Continue adds mixed
aggregate precedence tests. Rollback adds the full state × already-applied
matrix plus conflict/integrated no-op matrices across operation states. Status
adds state × pending missing-repository coverage, conflict-validation outcome
coverage, drift/blocker ordering, and root override tests. Existing real-Git,
pending-reconciliation, lifecycle, event-order, byte fixture, and retained-
reader tests remain unchanged.

Git preparation/execution, backend observation, pending-action decoding and
reconciliation, record transitions, event emission, and persistence remain in
their existing orchestration owners. Pure policy returns decisions and facts;
it never performs mutation. Any caller needing a new state/fact, changed
precedence, changed error timing, or changed decision stops R1 for interface
review.

## Acceptance

R1 is accepted only when:

1. `merge/mod.rs` is thin wiring/readable orchestration and its cross-cutting
   responsibilities have the owners named above;
2. every current participant decision in the R0 inventory has one semantic
   owner and an exhaustive table test;
3. moved LOC, semantic additions/deletions, and touched files remain within the
   change-budget ledger or have a recorded scope review;
4. R0 byte/state fixtures are unchanged; and
5. the complete technical gate is green with no protocol regeneration delta.

## R1 integration closure

R1 was integrated on 2026-08-04 from root revision
`4f44848ba656d5c9c76a2e446ef5fc8e74c6d409` and core revision
`f41ad3d9de6f47b15f65d1b2cae26e2fdabca477`. It changes no runtime feature,
wire value, durable record, response/event/error projection, or Git/filesystem
mutation order.

### Lossless movement evidence

The installed `rust-split 0.1.1` workflow was used before applying the runtime
and store ownership moves. Ordered `explode` chunks reconstructed each original
input byte-for-byte:

| Input | Chunks | Original and reconstructed SHA-256 |
| --- | ---: | --- |
| `merge/mod.rs` | 58 | `d633184577bed468f683228dcf35ac10880382f8c595f96f4b7995f7d8d554fd` |
| `merge/store.rs` | 32 | `e955ea61cdebdedb7ef9b43b7024002e88c3807e67e001a7718b6a1479e9bd71` |

Formatting was applied only after the pure moves compiled and their focused
tests passed. `merge/mod.rs` is now 62 lines of module wiring, exports, and the
existing member-finished event helper. Runtime dispatch, open-gate, mutation
guard, store persistence, and participant decisions have the owners frozen
above.

### Scope review and module bounds

The participant policy produced a measured +460 physical production lines and
1,489 focused test lines. That exceeded the provisional +250/400 budget and
therefore stopped at the required scope-review checkpoint. Both independent
post-implementation reviewers accepted revised ceilings of +500 net
production, 650 conservatively classified moved/centralized production lines,
and 1,550 test lines. The corresponding ledger entry is in
`GwzM5-8ChangeBudget.md`.

Test code was moved out of production modules and split by semantic owner. No
new R1 test file exceeds 500 lines. The one reviewed production exception is
`participant_semantics/status.rs` at 518 production lines under a 550-line
ceiling; both reviewers found it cohesive and advised against scattering its
ordered status/recovery projection merely to meet a round number.

### Verification evidence

- two independent implementation reviews report no remaining P0/P1/P2 defect;
- all 139 merge unit/lifecycle tests pass, including 36 pure participant-policy
  tests, 13 runtime tests, and 7 store tests;
- the complete `gwz-core` package gate passes 712 tests with one intentional
  ignore across unit and integration targets;
- `cargo test --workspace --all-targets` passes, including Rust CLI workflows,
  protocol freshness, and release-workflow tests;
- `cargo clippy --workspace --all-targets -- -D warnings` and
  `cargo fmt --all -- --check` pass;
- the native Python extension rebuilds in the workspace `uv` environment,
  generated Python protocol output is current, and all 325 Python tests pass;
- the merge-document consistency gate passes 7 sources and 68 assertions; and
- installed `gwz` reports only the intended root planning documents and `gwz-core`
  structural/policy files, while the root/core/CLI/Python diff-hygiene gate is
  clean.

The R1 acceptance conditions are satisfied. R2a is the next executable package;
it must retain the v0-safe integration/message scope and may not absorb the
later branch, source-provenance, or partial-composition dimensions.
