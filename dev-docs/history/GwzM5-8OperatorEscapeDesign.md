# GWZ M5-8 Operator Escape and Drifted-Acceptance Remediation Design

Date: 2026-08-15

Status: **DRAFT — pending review.** This document answers pre-A1 checklist
item 3 of `GwzM5-8ProgressReviewF5.md` §9 ("write the operator escape story
for permanently unresolvable states … and the drifted-acceptance remediation
path"), tracked as an open obligation in `CurrentProgramCheckpoint.md`
("Pre-A1 checklist"). It is a design, not an implementation plan: it freezes
the operator-facing semantics and the contract deltas they require, and it
names the decisions the program owner must make before any package is cut.

Authority relationships: `GwzM5-8Refactor.md` remains the behavioral
authority; the I2 contracts (`GwzM5-8I2RecordContract.md`,
`GwzM5-8I2ActionJournalContract.md`, each as amended by
`GwzM5-8R4bInterfaceAmendment-1.md` and `GwzM5-8R4bInterfaceAmendment-2.md` —
the operative wire authorities per their 2026-08-11 supersession banners)
remain wire-authoritative; `GwzM5-8R4bTransitionDesign.md` remains normative
for the v1 transition architecture. Nothing in this document changes any of
them by itself. Every wire, transition, protocol, or store change proposed
here is marked as such and requires its own interface amendment and dual
review before implementation (rulebook-mandated tier for amendments).

Grounding: this design was derived from the documents above plus direct reads
of the implemented state machine — `OperationState` and its v0 legality
(`gwz-core/src/workspace_ops/merge/model/lifecycle.rs:54-64`, `:83-117`), the
v1 record and journals (`model/v1/record.rs:20-52`, I2 journal contract §1),
the v1 dispatcher (`v1_lifecycle/authority/dispatcher.rs:221-297`), the
reverse-lane observers (`v1_lifecycle/authority/observe/reverse/`), the v0
abort/preserve/continue semantics (`abort/`, `preserve.rs`,
`participant_semantics/`), the store (`store/mod.rs`, `store/persistence.rs`),
the open-merge gate (`src/operation/open_merge_gate.rs`), and the released CLI
surface (`gwz-cli/src/globalargs/parser.rs:374-422`,
`gwz-cli/src/merge_render.rs`). Line numbers are against gwz-core `117117e`
(current lane tip; ahead of the checkpoint tuple by CI-cache commits only).

## 1. Problem

The v1 lifecycle is deliberately exact-or-nothing. Every durable rewrite
requires an exact predecessor and a sealed proof from an exact live
observation; ambiguity either enters `RecoveryRequired` with the pending
journal retained or — between preservation/rollback actions — returns a typed
error **with no rewrite at all** (`GwzM5-8R4bTransitionDesign.md` §6.5,
§6.7). Termination is equally exact: `AbortOperation` requires
`VerifiedRollbackExhausted` covering every participant and the exact
selected-root baseline (§6.1), `BeginRollback` from `Preserving` requires
`VerifiedPreservationExhausted` over the complete two-pass cursor (§6.1, §7),
`CompleteOperation` requires exactly complete publication (§6.1), and archive
accepts only a validated terminal record
(`v1_lifecycle/store/archive.rs:22`; dispatcher rejects `Archive` for any
open state, `authority/dispatcher.rs:308`).

The consequence: when the world can no longer be returned to any exact form
the record recognizes — a member repository deleted, history force-rewritten
and pruned, a preservation stash dropped, publication files hand-mangled —
the record can never terminalize, and there is no documented operator path
out. Meanwhile the open record blocks nearly every mutating workspace
command: `pull`, `push`, `commit`, branch/tag/stash/repo mutation,
`materialize`, snapshot capture, and any new merge are all `Block`ed by the
open-merge gate (`src/operation/open_merge_gate.rs:64-76`, error text at
`:93-99`). A permanently unresolvable merge therefore bricks the coordinated
workspace, not just the merge.

This is not hypothetical for a multi-repo tool: a user deleting a member
checkout to "start fresh" is routine, and the released v0 line (v0.10.5) can
already wedge this way today (§4.4, §9.1). "Exact or nothing" is the right
automatic policy; the nothing branch needs an operator story before v1
records become real at A1 (`GwzM5-8ProgressReviewF5.md` §3.3).

## 2. Design doctrine

Six rules govern every escape in this document. They are chosen so the
escapes strengthen, rather than undermine, the program's core invariants.

1. **Escapes are a separate consent lane, never a fallback.** No automatic
   path may select an escape. An escape runs only from an explicit operator
   request carrying explicit destructive consent (§7.2). The lifecycle
   dispatcher, observers, and reducers remain byte-for-byte ignorant of them.
2. **Escapes never fabricate lifecycle evidence.** No escape writes a
   participant outcome, acceptance, publication progress, or recovery origin
   that observation did not prove. Where an escape terminalizes without
   proof, it records *operator override* as first-class typed evidence
   (§5.3), so no future reader can mistake an overridden record for a clean
   one. This preserves the Refactor §21 stop condition "an ambiguous live
   state is being converted into success or safe retry" — an override is
   neither; it is a third, honestly-labeled thing.
3. **Evidence is preserved by default; destruction must be named.** The
   escape ladder is ordered quarantine-before-abandon. Preserved artifacts —
   backup refs, native stashes, bundles, record bytes — are never deleted by
   any escape. Anything an escape gives up on is enumerated, per owner, in
   the printed destruction manifest and in the durable override evidence.
4. **Escapes never touch member-repository user data.** The complete
   inventory of merge-owned artifacts an escape may even *name* is closed
   (§7.1). Everything else in a member repository is out of bounds, always.
5. **Escapes are idempotent and re-enterable.** Each escape is either a
   single atomic step (quarantine) or resumes from persisted per-owner
   dispositions (force-abandon). A crash mid-escape lands in a state the
   same escape recognizes and continues.
6. **The record plane stays append-honest.** Quarantined and archived bytes
   are immutable; the only in-place rewrite an escape performs is a typed v1
   transition through the existing kernel/store (force-abandon's terminal
   write). No escape hand-edits YAML.

## 3. Taxonomy of stuck states

### 3.1 The funnel that produces stuck states

All stuck states are produced by one funnel, visible in the dispatcher
precedence (`authority/dispatcher.rs:221-297`): terminal → archive/respond;
`RecoveryRequired` → recovery observation; pending preservation/rollback
journal → cursor observation; pending forward action → its observation; then
state dispatch. Every observation must classify the live world as exactly
`NotStarted`, exactly `Completed`, or `Ambiguous`
(`GwzM5-8R4bTransitionDesign.md` §8), and:

- ambiguity **under a persisted owner** enters `RecoveryRequired` with the
  journal retained; leaving recovery (`ResumeFromRecovery`) requires an exact
  proof that the recorded origin state is restored (§6.5);
- ambiguity **between reverse actions** (state `Preserving`/`RollingBack`,
  no pending journal) is unrepresentable in the frozen wire and returns a
  typed error with no rewrite (§6.5 "deliberately not persisted", I2 journal
  contract §2 legality table);
- preservation additionally requires `VerifiedPreservationCursorPrefix` —
  a live proof that every earlier cursor position is complete or exactly
  unnecessary — before any pending preservation action may even be
  classified (§6.7; `PreservationEvidenceMismatch`,
  `src/model/mod.rs:70`); and
- the only terminal edges are `CompleteOperation` (exact publication
  completion) and `AbortOperation` (exact rollback exhaustion) (§6.1).

A stuck state is therefore always "the live world can no longer produce the
exact observation the next required proof needs." The taxonomy below
enumerates the concrete producers.

### 3.2 Resolvable classes (R) — no escape needed

These halt states have a designed exit and are listed only to bound the
escape scope:

| Class | State/shape | Exit |
| --- | --- | --- |
| R1 | `Halted` with typed participant error (`has_halt_cause`, `authority/dispatcher.rs:372-377`) | fix cause, `--continue` retry (`ParticipantState` retry legality, `model/lifecycle.rs:120-148`) or abort |
| R2 | `AwaitingResolution` with exact conflict evidence | resolve, stage, `--continue` |
| R3 | `RecoveryRequired` whose origin shape is restorable (crash windows under a persisted owner; the whole write-ahead design exists for these) | restore/leave world untouched; `ResumeFromRecovery` after exact re-observation |
| R4 | Crash between durable phases of preservation/rollback with artifacts intact | the before/after classifiers re-derive position exactly (I2 journal §4; Amendment-2 §3.4) |
| R5 | v1 open record under a pre-A1 binary (`required_wave` typed rejection; `record_wire/decode.rs:86` today) | upgrade the binary; not an operator-surgery class |
| R6 | Workspace mutator lock contention | OS advisory lock releases on process death (`src/operation/workspace_mutator_lock.rs:25-34`); wait/retry |

### 3.3 Permanently unresolvable classes (U) — escape required

"Permanently" means: no sequence of supported GWZ commands can reach
`Completed` or `Aborted`, and the blocking condition is outside GWZ's power
to restore (deleted/pruned/rewritten external state), or outside the wire's
power to represent.

| Class | Producer | Where the machine refuses | Stuck evidence |
| --- | --- | --- | --- |
| U1 — member repository permanently missing | user deletes/moves a member checkout; disk loss | v0: continue blocked (`participant_semantics/continue_eligibility.rs:108-115`), abort blocked unless the participant had no owned mutation (`participant_semantics/rollback.rs:167-180`). v1 is stricter: even the no-mutation terminalization re-observes the live checkout (`abort/participants.rs:135-165` requires `clean_checkout` of the recorded branch/before-commit), and the reverse-entry preflight treats an operational read failure as an error, never `false` (`GwzM5-8R4bReverseLifecycleInterface.md` §2, §5) — so **any** selected participant in a deleted repository blocks rollback entry and exhaustion | `ParticipantDriftKind::RepositoryMissing` (`model/v0.rs:228-243`); typed operational errors |
| U2 — member repository mangled beyond exact recognition | force-push + prune of recorded commits; worktree replaced; `.git` corruption; foreign rewrite of the integration ref where old objects are gone | every participant observation is a permanent "third form": continue ineligible, rollback observation ambiguous forever (I2 journal §3 closed observations); `ResumeFromRecovery` unconstructible | drift kinds `HeadRewound`/`HeadDiverged`/`ObjectMissing`/`ForeignIntegrationState`…; `RollbackEvidenceMismatch` |
| U3 — preservation wedge | an *earlier* preservation artifact is destroyed after creation: backup ref deleted, native stash dropped (`git stash drop`/aggressive native gc), bundle bytes mangled; or the exact preimage can never be re-proved | `Preserving` between actions: `VerifiedPreservationCursorPrefix` unobtainable → `PreservationEvidenceMismatch` with **no rewrite and no recovery representation** (`R4bTransitionDesign.md` §6.7:745-754, §6.5:619-623); `BeginRollback` from `Preserving` gated on `VerifiedPreservationExhausted` (§6.1) — the dispatcher completes the cursor "regardless of an incoming abort request" (§7) | typed `PreservationEvidenceMismatch`; v0 analogue: preserve-abort refusals that already tell the user to "preserve … manually before aborting" (`preserve.rs:350`, `:402`, `:432`, `preserve/plan.rs:104`, `:228`) |
| U4 — rollback wedge | evidence/root forms destroyed mid-rollback: recorded candidate/baseline file forms unreproducible, selected-root manifest/lock reread cannot match (symlinked/noncanonical/hand-edited), participant reverse observation permanently third-form | `RollingBack` between actions: typed error, no rewrite (§6.5); with a pending journal: recovery entered but `origin=RollingBack` resume needs the exact retained journal shape live (§8 of the reverse interface) | `RollbackEvidenceMismatch`, `TerminalRollbackMismatch` |
| U5 — unrestorable recovery origin | `RecoveryRequired` with retained journal whose live precondition is gone forever (stash object pruned between `CreateStash` and adoption; ref target overwritten and old commit pruned) | `ResumeFromRecovery` requires the exact unchanged origin state (§6.5); reverse-origin verifiers reuse the exact phase classifiers (reverse interface §8) | recovery error with row unchanged, forever |
| U6 — drifted acceptance / publication third form | post-acceptance divergence: live root no longer matches frozen acceptance; accepted-unborn root became born (I1 memo §4); publication prefix in a mixed/third form from external interference | forward: "reports typed drift and publishes nothing; it never derives a different candidate" (`GwzM5-8Refactor.md` §9:835-846); prefix table row "any mixed/third form → bound ambiguity; no mutation" (`R4bTransitionDesign.md` §6.4); reverse: evidence rollback needs the exact recorded forms (§6.6) — may itself wedge into U4 | `AcceptanceInputDrift`, `RecordedEvidenceDrift` (`acceptance/publication.rs:117-233`, `v1_lifecycle/authority/observe/finalization.rs:57-88`, `:417`) |
| U7 — record-plane damage | open record bytes unreadable/corrupt; **two or more** open `.yaml` records under `.gwz/merge/` (manual copy, backup-restore residue) | `discover_open` errors on any unreadable record and on multiplicity (`store/mod.rs:81-96`), which fails the open-merge gate itself — **every gated command including `merge --status/--continue/--abort` dies at discovery** (status paths themselves return typed `MergeRecordUnreadable`, e.g. `status/snapshot.rs:177`, rather than degrading to a partial view) | `MergeRecordUnreadable`; "multiple merge records exist under …" (`store/mod.rs:90-95`) |
| U8 — terminal-plane wedges | (a) archive source and destination copies exist with **different** bytes — archive rejects without deleting either (reverse interface §10); (b) archived cleanup worklist meets a backup ref with a different live target or a permanently unavailable repository — targeted GC "stops and retains the archive" forever (I2 journal §5; reverse interface §11) | by design: no mutation is permitted; retention is permanent | archive mismatch rejection; GC stop |

Class U8 does not block the workspace (the gate keys on *open* records only,
and `done/`/subdirectories are invisible to discovery,
`store/mod.rs:269-285`); it is a permanent-retention annoyance, not a brick.
U1-U7 all leave an open record in place and therefore brick every `Block`ed
command in `open_merge_gate.rs:64-76`.

One asymmetry deserves emphasis because it surprised this review: **v1 is
strictly harder to wedge out of than v0.** v0 permits abort past a missing
repository when the participant had no owned mutation
(`participant_semantics/rollback.rs:167-180`); v1's exactness (entry
preflight over every member, live re-observation even for no-mutation
terminalization) closes that leniency. The escape story is therefore *more*
necessary after A1, not less.

## 4. Escape operations

Three operations, in strictly escalating destructiveness. Each subsection
gives preconditions, exact effects, deliberately broken invariants and how
the break is recorded, and idempotency/re-entry.

### 4.1 Overview and placement

| Operation | Destroys | Unblocks workspace | Terminalizes record | Wire change needed | Reaches v0 records |
| --- | --- | --- | --- | --- | --- |
| `quarantine` | nothing | yes | no (record parked, immutable) | none | **yes** |
| `force-abandon` | the guarantee that abort meant verified restoration | yes | yes (`Aborted` + override evidence) | yes (v1 only, §6) | no (v0 wire frozen) |
| manual surgery | operator-dependent | yes | n/a | none (documented procedure) | yes |

The ladder is ordered: `force-abandon` requires that its preflight would
also have permitted quarantine, and the recommended runbook is always
quarantine first, decide later. Manual surgery exists for exactly the states
where the tool itself cannot run (U7) and for U8(a).

### 4.2 Quarantine — evidence-preserving unblock

**Intent.** Move the open record out of the open slot, byte-exact, into a
third store location that discovery and the open-merge gate never consult,
so the workspace resumes normal operation while every byte of merge evidence
— record, refs, stashes, bundles, conflicted worktrees — survives untouched
for forensics or later manual disposition.

**Mechanism.** A checked byte-exact move of
`.gwz/merge/<merge-id>.yaml` → `.gwz/merge/quarantine/<merge-id>.yaml`,
plus a written sidecar `.gwz/merge/quarantine/<merge-id>.escape.yaml`. This
is the same primitive family as the S-owned exact terminal-byte archive move
(open→`done/`, `store/persistence.rs:43-48`) with two deliberate
differences: it does **not** require a terminal or even decodable record,
and it writes the sidecar. Because `record_files` collects only `*.yaml`
directly under `.gwz/merge/` (`store/mod.rs:269-285`), a `quarantine/`
subdirectory is invisible to open discovery exactly as `done/` is today. The
sidecar lives only under `quarantine/`; it must never be hand-moved into the
open directory, where any `*.yaml` is a record candidate by the same rule
(the §4.4 runbook says so explicitly).

**Preconditions.**

- explicit request naming the merge id; the id must match the single open
  record's filename (when the record decodes, also its `merge_id` field, the
  existing `validate_open_merge_id` rule);
- the workspace mutator lock is held for the whole operation
  (`WorkspaceMutatorLock::acquire`, `src/operation/workspace_mutator_lock.rs`);
- destructive consent per §7.2 (quarantine destroys nothing, but it removes
  the gate's protection of in-flight state — consent is for *that*);
- byte stability: read bytes, digest, re-verify digest immediately before
  the rename (same TOCTOU posture as the checked store, §9 of the
  transition design);
- **no decode requirement.** Quarantine is byte-level so that U7 records
  (unreadable, wrong-version, future-envelope) can be quarantined. When the
  bytes do decode as a supported open record, quarantine additionally
  records the decoded state/pending-owner summary in the sidecar; and
- multiplicity: when more than one open `*.yaml` exists (U7), quarantine
  accepts an explicit `--file <name>` selection and moves exactly one file
  per invocation. This is the only tool path that can act under the
  multiple-records error.

**Exact effects.**

- record bytes: moved, byte-identical (digest re-verified after move; fsync
  file and both directories, mirroring archive's crash rules in reverse
  interface §10);
- sidecar written atomically before the record move, containing: operator
  string (from request meta), UTC timestamp, tool `writer_version`, the
  record's SHA-256, the operator-supplied reason (optional, recommended
  required — [Q8]), the decoded summary when available (state, pending
  journal kind/phase, per-participant states, drift), and the reproduction
  of the exact typed error that motivated the escape when the caller
  supplies it;
- journal: untouched (it lives inside the record bytes);
- preserved artifacts: untouched — backup refs
  `refs/gwz/merge/<merge-id>/<owner-key>/head`, native stash
  `stash_<merge-id>`, bundles `.gwz/stash/bundles/stash_<merge-id>.yaml`
  (`src/stash.rs:291-296`), root candidate/marker/boundary files, member
  worktrees: all left exactly as they are. Quarantine prints their inventory
  (§6.3) but mutates none of them;
- workspace: open-merge gate no longer fires (no open record); a new merge
  may start. Derived artifact names embed the merge id (I2 journal §2), so a
  new merge cannot collide with quarantined artifacts unless the same id
  recurs, which ids' uniqueness rule already prevents;
- status: `merge --status <id>` learns a third source, projecting
  "quarantined" from bytes only (never live repositories), alongside the
  open/archived arbitration table of reverse interface §9 — see [Q2] for
  the protocol shape; and
- GC: retention and targeted GC must treat `quarantine/` as invisible and
  never delete quarantined records or artifacts referenced by them ([Q5]
  interacts: today GC only reads `done/`, `store/gc.rs:8`, so invisibility
  is the default; the design makes it an explicit tested rule).

**Deliberately broken invariants, and their recording.** Quarantine breaks
exactly one system-level invariant: "an unfinished coordinated merge blocks
coordinated mutation until recovered" (the gate's purpose,
`open_merge_gate.rs`). The break is recorded by the sidecar and by the
record's continued existence in `quarantine/`. It does **not** break record
immutability, evidence retention, or wire validity — a quarantined record is
the same record. Member repositories remain in whatever half-merged state
they were in; quarantine's printed manifest says so per member, loudly
(§6.3), because from the user's perspective *that* is the dangerous
residue: branches may sit at merged-but-never-published commits.

**Idempotency / re-entry.** The operation is a single atomic rename plus a
pre-written sidecar. Crash windows: (a) sidecar written, record not moved —
re-run detects the sidecar with matching digest and completes the move; (b)
record moved, response lost — re-run finds no open record and an existing
quarantine entry with matching digest: reports success idempotently. A
quarantine request for an id already quarantined is a no-op success with the
existing sidecar echoed.

**Restore (un-quarantine).** The exact inverse, permitted only when: the
open slot has no record, the quarantined bytes still match the sidecar
digest, and the record decodes as a supported open record. Restore is how an
operator who has *repaired the world* (recreated a repository from a remote,
restored a stash from the bundle) re-enters the normal lifecycle: after
restore, ordinary `--continue`/`--abort` resume through the unchanged exact
machinery. Restoring bytes that were hand-edited while in quarantine is
refused by the digest check — an edited record is manual surgery (§4.4),
never a restore. Restore refuses while any other merge is open.

### 4.3 Force-abandon — terminalize without full proof

**Intent.** For records whose reverse path is provably wedged (U1-U5 and
the reverse half of U6), produce a durable terminal `Aborted` so the merge
can archive and eventually GC — while doing every part of the rollback that
*can* still be proved exactly, and recording precisely which owners were
abandoned without proof.

**Position in the machine.** Force-abandon is not a parallel lifecycle. It
is the ordinary reverse lifecycle plus two closed extensions, both operator
gated:

1. **Per-owner disposition.** Where the ordinary cursor demands an exact
   observation of owner *k* and that observation is permanently unavailable,
   the operator may mark owner *k* `Unrecoverable`. The mark is durable,
   carries the evidence (the exact typed error and the live facts observed),
   and moves the cursor past that owner. Every owner not marked is processed
   by the unchanged exact machinery — force-abandon may not skip an owner
   the machinery can still handle (precondition below).
2. **Overridden terminal edge.** When the cursor is exhausted-or-marked for
   every owner, the terminal write is a new transition
   `AbortOperationOverridden { override: PreparedOperatorOverride }` —
   the analogue of `AbortOperation` whose proof obligation is
   "`VerifiedRollbackExhausted` restricted to unmarked owners, plus the
   complete marked-owner disposition table." State becomes `Aborted`.

Both extensions are new closed vocabulary: one new journal/disposition field
and one new transition variant with an exact predecessor set and frozen
footprint, per the transition design's own rule that a needed compound "must
be a new closed enum variant with an independently reviewed footprint"
(§6.8). The wire consequences are collected in §6.

**Preconditions.**

- the record decodes as a supported open v1 record (v0 is excluded — §6.2);
- a fresh full preflight (the same read-only global preflight the reverse
  entry already performs, reverse interface §5) classifies **at least one**
  owner as permanently unobservable *and* the operator names each such owner
  explicitly in the request. Owners the preflight can still observe exactly
  are never markable: if everything is tractable, the command redirects to
  plain `--abort` — the escape must never be the convenient path;
- destructive consent per §7.2, bound to the record digest — consent
  is issued against exactly the bytes whose destruction manifest was
  printed, and any concurrent rewrite invalidates it;
- preservation-first ordering is respected: from `Preserving`, tractable
  preservation owners are still driven to completion before any rollback
  entry, exactly as the dispatcher does today (§7 of the transition
  design); marking a preservation owner `Unrecoverable` requires the same
  named-owner consent; and
- the workspace mutator lock and the v1 mutation lease are held throughout
  (the escape runs inside the ordinary service loop).

**Exact effects.**

- record: per-owner `Unrecoverable` dispositions persisted write-ahead
  (before the cursor moves past the owner — the same write-ahead discipline
  as every other mutation owner, §6.9 of the transition design); terminal
  write flips state to `Aborted` and installs the immutable
  `operator_override` evidence block (§6.1). Participants that were rolled
  back exactly carry their ordinary `Aborted`/`RolledBack` terminal rows;
  marked participants keep their last honest state (e.g. `Merged`,
  `Conflicted`) — **no fabricated terminal outcome** — with the override
  block recording why they were left;
- journal: pending action of a marked owner is retired into the override
  evidence (verbatim copy of the abandoned action), never silently cleared;
- preserved artifacts: nothing deleted. Backup refs, stashes, and bundles of
  marked owners are recorded in the override block and remain in the
  archived record's cleanup worklist exactly as today, so GC's existing
  exact-target rule continues to protect them (I2 journal §5);
- member repositories: tractable owners are restored exactly (ordinary
  rollback); marked owners are **left untouched** — force-abandon never
  "best-efforts" a mutation it cannot verify. What remains behind per member
  is printed and durably recorded;
- archive: the terminal record archives through the unchanged exact-byte
  primitive. `ArchivedTerminalOutcome` stays `Aborted`
  (`GwzM5-8Refactor.md` §15.3.3) — the override evidence rides inside the
  archived bytes, and the archived/status projection must expose an
  `operator_override` presence marker ([Q2], §6.3); and
- protocol/status: open and archived status render the override loudly
  (§6.3): overridden abort is never displayed as a plain abort.

**Deliberately broken invariants, and their recording.** Exactly one:
"`Aborted` means every participant and the selected-root baseline were
verified restored" (§6.1 `AbortOperation` row). The break is recorded
durably, structurally, and at every projection layer: the
`operator_override` block (operator, timestamp, reason, per-owner
dispositions with evidence, digest of the record at consent time), the
non-terminal participant rows left in place, and the status/archive
projections. A future reader — human, GC, or M6-M8 adapter — can always
distinguish verified-clean `Aborted` from overridden `Aborted` by one field
presence check. Invariants **not** broken: acceptance immutability,
candidate/evidence immutability, unknown-field survival, single-writer
exclusion, archive byte-exactness, GC's never-delete-mismatched-ref rule.

**Idempotency / re-entry.** Every disposition and every ordinary rollback
step is write-ahead persisted; a crash at any point re-enters through the
ordinary dispatcher (the record is still `Preserving`/`RollingBack` with
its journals and dispositions), and a re-issued force-abandon request
resumes — already-marked owners stay marked, already-rolled-back owners are
recognized by the unchanged exact observers. Consent, however, does **not**
survive the crash: the resumed invocation re-prints the remaining
destruction manifest and requires fresh digest-bound consent, because the
record digest has advanced.

### 4.4 Manual surgery — the runbook when the tool cannot act

Manual surgery is for the states where no tool path exists even with this
design: U7 shapes quarantine's `--file` selector cannot express, U8(a)
both-copy divergence, damage inside `quarantine/` itself, and any v0 wedge
encountered on a binary predating these escapes. It is a documented,
copy-paste-safe procedure, not code. The design freezes its content; its
shipping location is [Q6]. The procedure (full text to live in user docs):

1. **Stop all GWZ and git activity in the workspace.** The tool's own locks
   cannot protect a hand operation.
2. **Snapshot everything first**: copy `.gwz/merge/` (records, `done/`,
   `quarantine/`) aside; run `git for-each-ref refs/gwz/` and
   `git stash list` in every member and the root; copy
   `.gwz/stash/bundles/`. Nothing below is permitted until this exists.
3. **Unblock the workspace** by moving — never deleting, never editing in
   place — the open record(s) from `.gwz/merge/*.yaml` into
   `.gwz/merge/quarantine/` (create the directory if absent). This is the
   by-hand equivalent of §4.2 and is recognized by later tool versions
   (minus the sidecar, which `--status` reports as "quarantined without
   sidecar").
4. **Both-copy archive divergence (U8a)**: compare
   `.gwz/merge/<id>.yaml` and `.gwz/merge/done/<id>.yaml`. The open copy is
   authoritative for an interrupted archive *unless* it is the corrupt one
   (reverse interface §10 keeps both precisely so a human can decide). Move
   the copy you reject into `quarantine/` under a distinct name; never
   delete it.
5. **Member cleanup inventory** — the complete list of what GWZ owns inside
   a member repository, and the only things a human should ever remove there
   on GWZ's behalf: refs matching
   `refs/gwz/merge/<merge-id>/<owner-key>/head`; the native stash whose
   message is exactly `gwz:stash_<merge-id>: merge preservation`; nothing
   else (I2 journal §2 derived-name rules). Half-merged integration branches
   are the *user's* branches — GWZ documentation must tell users how to
   reset them (`git reset --hard <before_commit>` using the values printed
   by `merge --status`/the quarantine manifest) but the runbook never
   instructs deletion of user data.
6. **Never touch**: anything under members' `.git/` beyond the two items
   above, `done/` archives, quarantined bytes (copy out to inspect), or
   `.gwz/stash/bundles/` (they may be the only surviving copy of user work).
7. **Re-entry**: after repairing the world, either restore the record
   (§4.2 restore; digest must match) and resume the normal lifecycle, or
   leave it quarantined forever. Editing record YAML by hand and putting it
   back in the open slot is expressly unsupported: the checked store's
   lineage checks make the outcome undefined-hostile, and the runbook says
   so.

Precedent: the released tool already directs users to manual preservation in
the preserve-abort refusal messages (`preserve.rs:350`, `:402`, `:432`;
`preserve/plan.rs:104`, `:228`) without anywhere documenting *how*. The
runbook closes that documentation debt for v0 users regardless of every
other decision in this document (§9.1).

## 5. Drifted-acceptance remediation

### 5.1 The shape of the problem

After `FreezeAcceptance`, the accepted workspace — exact lock bytes, member
audit, root input — is immutable (I2 record contract §6), and every later
step *applies* frozen bytes or reports typed drift: "If the live repository
no longer matches accepted evidence, it reports typed drift and publishes
nothing; it never derives a different candidate" (`GwzM5-8Refactor.md`
§9:835-846). Re-freezing acceptance and regenerating candidates are stop
conditions (§21), not options. So when drift is detected post-acceptance,
the machine's whole answer is a typed error — correct, and terminally
unhelpful without an operator path, which the F5 review called out
explicitly (`GwzM5-8ProgressReviewF5.md` §3.3 "Related:").

### 5.2 Detection surface

Post-acceptance drift is detected, never polled, at these existing
boundaries — remediation must not add new mutation-time detection, only
surface these better:

| Boundary | Detector | Typed result |
| --- | --- | --- |
| resuming `Finalizing` (recovery origin check) | `verify_finalization_recovery_origin` (`v1_lifecycle/authority/observe/finalization.rs:57-88`) | `RecoveryEvidenceMismatch` wrapping `AcceptanceInputDrift`/`MergeDrift` |
| root read-back before evidence/candidate mutation | committed-text and root observations (`acceptance/publication.rs:117-233`, `observe/finalization.rs:405-430`) | `AcceptanceInputDrift`, `RecordedEvidenceDrift` |
| publication prefix classification | R4a prefix + raw-index classifier (`R4bTransitionDesign.md` §6.4 table) | mixed/third form → bound ambiguity, `Finalizing` recovery origin |
| accepted-unborn root becomes born | I1 memo §4; Refactor §9 | drift; new commit never adopted as parent |
| read-only status | live-drift projection (reverse interface §9: state, persisted drift, read-only live drift) | `operation_drift`/participant drift rows in `merge --status` (`merge_render.rs:81-86`, `:139-145`) |

### 5.3 The remediation decision tree

Exactly three exits exist, because the invariants admit no fourth:

1. **Un-drift the world, then resume forward.** Acceptance is input-frozen,
   so the only way forward is restoring the live state to the accepted
   form. This is the *preferred* exit and the one pure tooling gap today:
   the operator cannot restore what the tool does not print. Requirement:
   on any post-acceptance drift error, and in `merge --status`, print the
   **expected exact forms** next to the live ones — expected root
   commit/branch, expected candidate/baseline file SHA-256s, expected
   staged-index form — sourced from the frozen acceptance/candidate, which
   the record already carries byte-exact. With those values a competent
   operator can `git reset`/restore files/re-clone and re-run
   `--continue`; `ResumeFromRecovery` then re-observes exactness and the
   ordinary lifecycle finishes publication. No wire change; render/status
   change only (§6.3).
2. **Give up the publication, keep the merge work: ordinary abort
   (preserve-abort first).** `--abort --preserve` snapshots residual work
   into backup refs/stash/bundle, then rollback undoes evidence by the
   recorded exact forms. This exit is available exactly while the reverse
   observations still classify (the recorded evidence forms still exist).
   When the drift that broke forward *also* broke reverse — hand-mangled
   root files, pruned objects — this exit degrades to U4/U6 and the ladder
   applies.
3. **Escape.** Quarantine (always available) or force-abandon (when
   terminalization is wanted and reverse is wedged), per §4. The
   drifted-acceptance case needs no drift-specific escape semantics; it
   composes: acceptance stays immutable through both escapes (quarantine by
   byte-freeze, force-abandon because no transition may touch it —
   transition design §6.3), so post-mortem forensics always retain the
   exact accepted workspace that reality diverged from.

Remediation-by-mutation of the record — editing acceptance to match the new
reality, "adopting" a born root, re-planning the candidate — is rejected as
a matter of doctrine (§2 rule 2) and of frozen contract (Refactor §21). If a
future wave wants "re-open and re-finalize from current reality," that is a
new forward feature with its own checkpoint, not an escape.

### 5.4 Composition table

| Drift situation | First choice | If that fails | Last resort |
| --- | --- | --- | --- |
| live root moved off accepted base, objects still present | exit 1 (reset to printed expected form, `--continue`) | exit 2 | quarantine |
| accepted-unborn root now born | exit 1 (operator deletes the foreign first commit only if it is theirs to delete; else exit 2) | exit 2 | quarantine |
| candidate/evidence files hand-edited | exit 1 (restore exact bytes from printed hashes — the record carries the exact candidate YAML) | exit 2 | force-abandon |
| recorded objects pruned/unavailable | — (exit 1 impossible) | exit 2 if reverse forms intact | force-abandon |
| member repo of a selected participant deleted after acceptance | — | — | quarantine, then force-abandon |

## 6. Contract and surface deltas

### 6.1 Wire (I2 amendment; v1 only; pre-A1 is the last cheap moment)

Force-abandon requires, and quarantine does not require, wire changes:

- one new optional top-level v1 field, `operator_override`, joining the five
  in `model/v1/record.rs:39-49` (collision rule of I2 record contract §8
  extends to the sixth name; unknown-field retirement: immutable once
  present, survives archival);
- `Unrecoverable` per-owner disposition rows (inside `operator_override`,
  written ahead of cursor movement past a marked owner, carrying the retired
  pending action verbatim plus the observed live facts);
- one new transition variant `AbortOperationOverridden` with exact
  predecessor set (`RollingBack`, or `Preserving` when every unmarked owner
  is exhausted and rollback entry itself is marked), frozen footprint, and
  the full §13 matrix additions (predecessor matrix, footprint test,
  unknown-survival, restart/fault rows); and
- protocol: append-only additions for status/field-10 projection (override
  presence + disposition summary; a `quarantined` record source), plus
  either new `MergeOp` values or new flags on `Abort` ([Q2]) — Taut schema
  append rules per the I2 protocol contract.

Because the v1 writer is production-disabled until A1
(`R4bTransitionDesign.md` §3), amending the v1 wire now costs a contract
amendment and matrix work but zero migration — the same "last cheap moment"
argument the F5 review made for the durable preservation cursor
(`GwzM5-8ProgressReviewF5.md` §3.3, §9 item 4). After A1 ships, this becomes
a v1→v1 same-version corridor problem or a v2 feature. If item 4's durable
cursor amendment proceeds, these should travel in the same amendment.

### 6.2 v0 reach

v0 gets quarantine and the runbook only. Force-abandon for v0 is rejected:
the v0 wire is a released, frozen format; adding a marker field or drift
kind is a same-version newly-writable shape that old readers must prove
behaviorally equivalent (Refactor §15.1) — and an unknown enum variant is a
decode break, not equivalence. Terminalizing a v0 record as plain `aborted`
without any marker would fabricate a clean abort, violating §2 rule 2.
Quarantine needs no v0 wire change because it never rewrites the record.

### 6.3 CLI surface sketch

Conventions honored: one `merge` command with mutually exclusive lifecycle
flags (`clirequest/merge.rs:7-15`), long flags only, no interactive
prompts, machine output parity across human/JSON/JSONL and both drivers
(`gwz-cli` and `gwz-py`, P3 ownership), and the existing global
`--destructive` consent convention ("GWZ refuses destructive changes unless
this is explicit", `globalargs/parser.rs:120-123`) which merge currently
rejects wholesale (`validate.rs:55-56`) — that rejection is retained for
every op except the escapes.

```text
gwz merge --quarantine [<merge-id>] [--file <name>] [--reason <text>] --destructive
gwz merge --restore <merge-id> --destructive
gwz merge --abort --force-abandon --give-up-on <member-id> [--give-up-on …]
          [--reason <text>] --destructive [--confirm <digest8>]
```

- `--quarantine` with no id resolves the single open record; `--file` exists
  solely for the U7 multiplicity case. `--reason` recommended-required [Q8].
- `--force-abandon` composes with `--abort` (and respects `--preserve`
  semantics for tractable owners). `--give-up-on` must name every owner the
  preflight classed unobservable — naming an observable owner is an error;
  omitting an unobservable one is an error listing it. `@root` and
  `@publication` are legal owner names for the root/publication owners.
- **Two-step digest-bound consent** for force-abandon: without `--confirm`,
  the command performs the full preflight, prints the destruction manifest
  (below), makes **no change**, and exits nonzero with
  `confirm token: <first 8 hex of record sha256>`. Re-invocation with
  `--confirm <token>` proceeds only if the record digest still matches —
  consent is bound to exactly the bytes whose consequences were shown.
  Quarantine/restore, which destroy nothing, need only `--destructive`.
- Everything the operator needs to decide is printed by the no-op first
  pass and by `--status` on a stuck record, extending the existing renderer
  blocks (`merge_render.rs:56-62` recovery-commands hint,
  `:64-79` preservation artifacts, `:81-86` drift):

```text
stuck: preservation cannot re-prove owner 'libs-a' (backup ref deleted)
  error: preservation_evidence_mismatch: …exact text…
why each escape:
  quarantine      parks the record; nothing destroyed; workspace unblocked
  force-abandon   terminalizes; gives up on: libs-a
per-member consequences of force-abandon:
  libs-a   (libs/a)   ABANDONED merged-not-published; branch 'main' left at 4f2c…;
                      recorded before-commit 91aa…; backup ref RETAINED; stash RETAINED
  libs-b   (libs/b)   rolled back exactly to 77de…
  @root    (.)        evidence commit rolled back; baseline manifest/lock restored
retained evidence: refs/gwz/merge/m-20260815-1/…  stash_m-20260815-1  bundle …yaml
```

- New `MergeOp` values vs flags-on-`Abort` is [Q2]; either is append-only in
  Taut. Both drivers ship the surface in the same release with parity
  fixtures, per the P3 pattern (reverse interface §9).

## 7. Safety rails and audit trail

### 7.1 The closed touchable-artifact inventory

An escape may read anything but may name/mutate only: open record bytes for
the addressed merge id; the `quarantine/` twin and sidecar; and — for
force-abandon's *tractable* owners only, through the unchanged executors —
the artifacts already in the §6.9 physical-mutation ownership matrix of the
transition design. The following are refusals, enforced and tested, not
guidance:

- never mutate member-repository user data: no branch deletion, no reset of
  a marked owner, no worktree/index write outside an exactly-proved
  rollback/preservation step;
- never delete preserved evidence: backup refs, native stashes, bundles,
  quarantined records, archives — no escape flag reaches them; deletion
  remains solely GC's, under GC's existing exact-target rule (I2 journal
  §5) — see [Q5] for the one proposed GC extension;
- never rewrite bytes: no in-place record edit, no archive/quarantine
  reserialization, terminal and quarantined bytes immutable forever;
- never fabricate: no participant outcome, acceptance, publication progress,
  or recovery origin an observer did not prove (§2 rule 2);
- never run un-consented: `--destructive` absent → typed refusal echoing
  what would be required (merge's existing posture, `validate.rs:55-56`);
  force-abandon additionally digest-bound (§6.3);
- never run un-serialized: mutator lock + (v1) mutation lease held
  throughout; contention is a refusal, not a wait; and
- never be selected automatically: no dispatcher/service path may return an
  escape as a next action; absence is gated by the same closed-vocabulary
  tripwires that police transitions today (`R4bTransitionDesign.md` §6).

### 7.2 Consent model

Two tiers: `--destructive` (session-explicit, protocol-carried
`policy.destructive`, `protocol/generated.rs:1921`) for gate-removal-class
operations (quarantine/restore), and `--destructive` **plus** digest-bound
`--confirm` for invariant-breaking operations (force-abandon). Consent never
persists: each invocation, and each post-crash resumption, re-earns it
against the current digest (§4.3).

### 7.3 Audit trail requirements

- durable: quarantine sidecar (§4.2) and `operator_override` block (§4.3)
  carry operator, timestamp, tool version, reason, digest, and per-owner
  evidence; both are immutable once written; the override block survives
  archival inside the terminal bytes;
- projected: open status, archived status, and field-10 projections expose
  quarantine/override presence in every renderer (human, JSON, JSONL, both
  drivers); an overridden abort or quarantined record is never rendered as
  its clean counterpart;
- evented: the JSONL event stream emits one event per escape decision
  (manifest printed, consent granted, owner marked, terminal written,
  record moved) so driver logs reconstruct the sequence; and
- program-level: escapes performed during the program's own operation of
  real workspaces are recorded in the ledger like any other deviation
  (rulebook L1-16 precedent).

## 8. What escapes must refuse to do — worked negative cases

| Attempt | Response |
| --- | --- |
| force-abandon while plain abort would succeed | typed refusal: "no owner is unrecoverable; use `gwz merge --abort`" |
| force-abandon naming an observable owner in `--give-up-on` | typed refusal naming the owner and its current exact classification |
| force-abandon on a v0 record | typed refusal; hint: quarantine |
| quarantine `--file` of a `done/` or `quarantine/` path | typed refusal; open slot only |
| restore over an existing open merge | typed refusal (single-open-record invariant, `store/mod.rs:87-95`) |
| restore of hand-edited quarantined bytes | typed refusal (sidecar digest mismatch); manual-surgery pointer |
| any escape deleting a backup ref/stash/bundle | no such code path exists; GC-only, unchanged |
| `--gc` of a quarantined id | typed refusal: quarantined records are not archives |
| escape under a concurrent GWZ mutator | lock refusal (existing behavior) |

## 9. Findings the program owner should not design around

Per the findings-quality bar, these are stated loudly rather than absorbed:

1. **The released v0 line can wedge today with no tool escape and no
   documentation.** v0.10.5 users who delete a member repository after a
   merged-participant state have: continue blocked
   (`continue_eligibility.rs:108-115`), abort blocked
   (`rollback.rs:167-180`), and every mutating workspace command blocked by
   the gate (`open_merge_gate.rs:64-76`). The only real-world exit is an
   undocumented `mv` of `.gwz/merge/<id>.yaml`. The §4.4 runbook (and, when
   scheduled, v0-reachable quarantine) is owed to *current* users
   independently of A1 ([Q9] scopes the shipping vehicle).
2. **U7's worst shape has no full tool escape even in this design.** When
   `.gwz/merge/` itself is unreadable (permissions, filesystem damage) the
   tool cannot even enumerate records; only the runbook applies. Accepted
   as inherent: a tool cannot repair the plane it stands on.
3. **U8(a) both-copy archive divergence is deliberately left to a human.**
   Automating the pick would rewrite terminal history on a heuristic;
   the runbook step 4 is the design.
4. **U8(b) GC retention wedge remains permanent unless [Q5] is accepted.**
   Without a `--forget-refs`-style release, an archive whose backup ref was
   externally retargeted is retained forever by design. This is safe and
   merely untidy; the open question is whether untidy-forever is acceptable.
5. **v1 tightened U1 relative to v0** (§3.3 note): the no-mutation
   missing-repository abort leniency of v0 does not survive into v1's exact
   entry preflight (`abort/participants.rs:135-165`). Either force-abandon
   ships with A1, or A1 knowingly ships a lifecycle that is stricter than
   the released one at exactly the wedge users actually hit. This design
   recommends the former; §10 makes it a decision, not an assumption.

## 10. Open questions for the program owner

- **[Q1] Wire placement of override evidence.** New sixth top-level field
  `operator_override` (recommended: collision-checked, immutable, archived)
  versus reusing `operation_drift` rows (rejected in this draft: drift is
  diagnostic, "never mutation authority", transition design §6.8, and
  overloading it would make drift rows load-bearing). Confirm the field
  approach and its exact shape at amendment time.
- **[Q2] Protocol shape.** Append `MergeOp::{Quarantine, Restore}` (+ a
  force flag on `Abort`) versus flags-only on existing ops; and the exact
  field-10/status projection additions (override presence, disposition
  summary, `quarantined` source). P3's parity matrix is accepted and
  settled — reopening it is a scope decision.
- **[Q3] Escape package placement.** Recommended: the wire/transition
  amendment rides pre-A1 (with the [F5 §9 item 4] durable-cursor amendment
  if that proceeds); implementation lands as its own reviewed package after
  R4b-G, before or at A1, since force-abandon is production-reachable only
  when v1 records are (A1) while quarantine is production-reachable
  immediately. Decide whether quarantine ships ahead on the v0 line.
- **[Q4] Restore strictness.** This draft: byte-identical restore only,
  edited records are permanently manual. Alternative: a checked
  "re-admit" that fully re-validates edited bytes as a supported open
  record. Rejected here (hand-edited records defeat every lineage
  guarantee), but the maintainer may want the door formally noted shut.
- **[Q5] GC release valve for U8(b).** Proposal: `gwz merge --gc <id>
  --forget-refs --destructive` deletes the *archive* while leaving every
  mismatched/unavailable ref in place (no repository mutation; refs leak,
  namespaced and inert). Accept, reject, or defer.
- **[Q6] Runbook shipping location.** User docs page + a pointer line in
  the stuck-state error/status output (recommended), versus dev-docs only.
- **[Q7] Naming.** `--quarantine`/`--restore`/`--force-abandon` versus
  alternatives (`--park`, `--shelve`, `--abandon`). Cosmetic but frozen
  into protocol/help text at freeze time.
- **[Q8] Mandatory `--reason`.** Recommended mandatory for force-abandon,
  optional for quarantine. Confirm.
- **[Q9] v0 quarantine scheduling.** Quarantine needs no v0 wire change,
  but shipping it to the v0 line before A1 is a release-planning decision
  (it touches store/discovery/status and both drivers). Decide whether the
  v0.10.x line gets quarantine + runbook, or runbook only.
- **[Q10] Preservation-owner marking.** Force-abandon from `Preserving`
  marks preservation owners `Unrecoverable` to reach rollback entry
  (§4.3). Confirm that a marked *preservation* owner also implies the
  matching rollback-side owner may be marked without a second consent
  round, or require both explicitly (this draft: name once, applies to
  both passes, printed as such).
