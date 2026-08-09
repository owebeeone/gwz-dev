# R4b state-machine and recovery review

Date: 2026-08-09

Reviewed scope: the settled `GwzM5-8R4bTransitionDesign.md`, the R4b sections
of `GwzM5-8Refactor.md` and `GwzM5-8ChangeBudget.md`, the amended frozen I2
record/action-journal contracts, and the committed R3/R4a lifecycle baseline at
`54627fd`.

## Verdict

**GO.** No P0, P1, P2, or P3 finding remains.

The transition vocabulary, predecessor sets, request-specific compounds,
proof bindings, cursor rules, restart behavior, archive/GC ownership, and
implementation gates are sufficiently closed to authorize R4b-T under the
review sequencing in the design. This verdict does not authorize production v1
activation; A1 remains the separate activation gate.

## Findings

### P0

None.

### P1

None.

### P2

None.

### P3

None.

## State and recovery coverage

The final review checked the following boundaries against both the frozen
contracts and the reachable R3/R4a behavior:

- operation and participant predecessor closure, including M4's one-or-more
  conflict `AwaitingResolution` aggregate, terminal-success finalization,
  failure/error Halted aggregates, the exact later `Planned -> Unattempted`
  stop footprint, and distinct retry-versus-resolution failure handling
  (`GwzM5-8R4bTransitionDesign.md` §§6.1–6.2);
- record-wide single forward-action ownership, prohibition in effective
  preservation, exact action retirement, and atomic abort/preserve abandonment
  entry (`GwzM5-8I2RecordContract.md` §§7–8);
- acceptance freeze-before-classification, immutable exact lock/audit bytes,
  no-publication completion, born/unborn root cases, and all seven adapted-v0
  resumption shapes (design §§6.3–6.4 and §13);
- publication write-ahead ownership, evidence adoption before handoff, the
  four exact file prefixes, pre/post-index distinction, and the degenerate
  terminal `Marker` case where lock and boundary already equal baseline
  (design §6.4);
- literal recovery origins, retained action ownership, stale-proof rejection,
  `Executing -> Halted -> RecoveryRequired(Halted)` after an ambiguous retained
  failure owner, and rejection of unrepresentable between-action reverse
  ambiguity (design §6.5; `GwzM5-8I2ActionJournalContract.md` §2);
- publication-first rollback, reverse selected-target order, exact named phase
  edges, pre-acceptance selected-root metadata restoration, post-clear baseline
  re-observation, and terminal `VerifiedRollbackExhausted` authority (design
  §6.6; action-journal contract §3);
- preservation's released two-pass owner order, stable evidence rows, root
  prefix installation, persisted action-before-classifier discipline, live
  cursor-prefix proof for unpersisted no-op/reset positions, and exhaustion
  proof before `Preserving -> RollingBack` (design §6.7; action-journal contract
  §4);
- the single dispatcher/resolver restart loop, post-executor re-observation,
  diagnostic-only executor errors, and record/owner/action/phase/payload-bound
  opaque proofs (design §§7–8);
- source-lineage checked writes, exact transition footprints, unknown-field
  survival/retirement, immutable terminal-byte archive movement, read-only
  status, archive-derived cleanup worklists, partial GC restart, and retained
  stash/bundle evidence (design §§9–10 and the physical ownership matrix);
- v0/v1 call-graph isolation, production-disabled v1 installation/migration,
  checkpoint dependencies, review gates, and the R4b budget ceilings (design
  §§11–15, the R4b refactor plan, and the R4b change budget).

## Challenges closed during review

The following material issues were found in earlier drafts and are closed in
the settled documents:

| Challenge | Settled resolution |
| --- | --- |
| Halt after a multi-target failure could expose an invalid Halted record with later Planned rows | The failure/stop variants atomically update the exact later Planned suffix; resolve-conflict failure preserves its authoritative Conflicted evidence. |
| A not-started forward action could be cleared without durable abort/preserve intent | The exact-not-started retirement now atomically enters RollingBack or Preserving with the matching checked entry proof; the I2 retirement rule was amended consistently. |
| More than one forward owner, or a forward owner in Preserving, produced valid but unrecoverable shapes | R4b-T now rejects both shapes at checked open; preparation also cannot install a second owner. |
| Halted outcome compounds could enter rollback/preservation without global preflight and publication handoff | Each compound carries the origin-specific prepared entry bound to the anticipated exact post-outcome model. |
| Publication file prefix alone could skip or loop index staging | Completion requires exact index/worktree alignment; `Boundary` pre/post-stage and the R4a degenerate terminal `Marker` case are separate exact observations. |
| Executor failure either trusted unverified output or retried forever | The immediate attempt token is diagnostic-only; a fresh matching observation selects Completed, Ambiguous, or exact NotStarted behavior. |
| Recovery could record a derived Halted origin directly from durable Executing | A retained halt cause is first persisted as Halted, stale proof is discarded, and the owner is reobserved before entering recovery with literal Halted origin. |
| Rollback or preservation could classify before persisting an owner | Every new reverse/preservation owner is committed and reread before its matching classifier can authorize mutation. |
| A later/wrong reverse or preservation owner could bypass cursor order | Rollback cursor contradictions reject at checked open; preservation begins are cursor-checked and an existing journal additionally requires a live bound prefix proof for every earlier position. |
| Preservation could enter rollback between actions before both passes finished | `PreparedRollbackEntry::FromPreserving` requires `VerifiedPreservationExhausted`, and the dispatcher completes preservation even when the incoming request is ordinary abort. |
| Selected-root rollback before acceptance was rejected, while post-clear completion could repeat forever | The I2 journal amendment uses selected-root plus exact operation-baseline authority before acceptance; post-clear exact baseline re-observation feeds the terminal exhaustion proof. |
| R4b-X package ordering conflicted with the stricter budget review gate | The checkpoint table and exact DAG now require the T/S/A interfaces before F or X and match the change budget. |

## Gate condition

The GO verdict depends on retaining the stop conditions and executable matrices
in design §§13–14. Any implementation need for a generic rewrite, a new durable
phase/field/action, an unowned mutation, an ordinal phase shortcut, a widened
proof constructor, or a v1-to-v0 mutable projection reopens architecture review.
