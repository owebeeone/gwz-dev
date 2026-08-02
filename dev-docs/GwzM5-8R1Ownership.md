# GWZ M5–M8 R1 Ownership Table

Status: ready for execution. R0 was accepted by exact-SHA retained-reader run
`30765936845` against core commit
`b7b49b4d05009e71a3606d2566febacd88d3a767`.

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
| R1-C1 result/finalization | `participant_semantics/result.rs` | response and finalization classification only |
| R1-C2 continue eligibility | `participant_semantics/continue_eligibility.rs` | continue/retry planning only |
| R1-C3 abort/rollback | `participant_semantics/rollback.rs` | abort preflight and participant rollback only |
| R1-C4 status projection | `participant_semantics/status.rs` | status classification/drift projection only |

No subtask edits `model.rs`, `merge/mod.rs`, another subtask's call sites, Git
backend code, durable storage, or driver code. Shared functions are proposed to
the lead rather than duplicated.

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
