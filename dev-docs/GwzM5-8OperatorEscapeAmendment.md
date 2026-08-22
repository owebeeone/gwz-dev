# M5-8 I2 operator-escape amendment (force-abandon, quarantine/restore, GC forget-refs)

Date: 2026-08-16. Round-2 revision: 2026-08-22.

Status: **ACCEPTED 2026-08-22 at GO/GO** (Code and State axes, cross-model;
the amendment tier of `GwzProcessOptimization.md` §4.2 :104-118, per §4.3;
round 2 of the two-round cap, no round 3 required). Review history: round 1
returned a dual **NO-GO/NO-GO** — Code 3 P1 / 5 P2 / 6 P3, State 6 P1 /
5 P2 / 4 P3, zero P0 on either axis. **One** merged remediation revision
applied every round-1 P1 and P2 of
`GwzM5-8OperatorEscapeAmendment-ReviewCode.md` (P1-1..P1-3, P2-1..P2-5) and
`GwzM5-8OperatorEscapeAmendment-ReviewState.md` (P1-1..P1-6, P2-1..P2-5),
plus the one-line P3s of both, and landed at gwz-dev `20f1654` ("Apply the
escape amendment round-2 remediation"). The round-2 focused re-verdicts
returned **GO/GO with zero unresolved findings** on either axis — appended
to the two review documents above, with no condition attached to either
GO — and §10.3's R-1, R-2, and R-3 are **CONFIRMED unconditionally on both
axes** (R-2's round-1 condition discharged by the applied [P1] fixes).
§10.4 maps every finding to its exact edit. **No §0 decided input changed.**
This train is **second-lane and NON-GATING for A1** per the accepted
`GwzM5-8ThinA1Amendment.md`; the §7 contract deltas are applied to the five
amended documents at this acceptance, and **implementation lands as its own
reviewed package(s)** after R4b-G, at or before A1 ([Q3]). This document is
the
wire/transition/protocol amendment for the operator escape story frozen in
`GwzM5-8OperatorEscapeDesign.md`, with all ten of that design's §10 open
questions **decided by the program owner on 2026-08-16** (§0). It is the
third pre-A1 train on the same I2 contracts, after
`GwzM5-8ExactEvidencePlatformAmendment.md` (accepted 2026-08-16) and
`GwzM5-8DurableCursorAmendment.md` (accepted 2026-08-16); §6.4 is the single
composition-authority statement A1 reviewers should read for how the three
trains compose. Per the accepted `GwzM5-8ThinA1Amendment.md` (2026-08-22,
:59-60) this train is **second-lane and non-gating for A1**; [Q3]'s pre-A1
wire placement stands as the decided schedule, not as an A1 gate.

Authority: `GwzM5-8Refactor.md` remains the behavioral authority; the I2
contracts as amended remain wire-authoritative; `GwzM5-8R4bTransitionDesign.md`
remains normative for the v1 transition architecture. This document is
controlling wherever those texts and this one disagree, and only for the
clauses quoted verbatim in §1. Per the adopted schedule the **wire amendment
lands pre-A1**; **implementation lands as its own reviewed package(s) after
R4b-G, at or before A1** ([Q3]).

Evidence base — direct reads on the current lane tip (round 1: `36754a5`,
clean tree; this round-2 revision re-verified every citation on 2026-08-22
against gwz-core `c13f773`, whose merge-lane non-test sources are unchanged
since the round-1 review baseline `90d3f8a`, and against the current
contracts: `GwzM5-8I2RecordContract.md` gained the M5b-W1 banner 2026-08-22
and **every anchor in it shifted by +21** — all its citations below are
re-pinned; the other four amended documents are unshifted), not on the
design's citations, which moved (§10.1 lists every correction):
`GwzM5-8OperatorEscapeDesign.md` (end to end), the four amended contracts,
`GwzM5-8M5bNoFfDesign.md` §8.2, `GwzM5-8A1DecisionPacket.md` §3.4/§4,
`GwzMergeWedgeRunbook-v0.md`, and the tree:
`gwz-core/src/workspace_ops/merge/model/v0.rs`,
`model/v1/record.rs:18-51`, `model/v1/validate/lifecycle.rs:56`,
`model/v1/canonical.rs:53-59`, `model/lifecycle.rs:54-64`, `:83-117`,
`store/mod.rs:81-98`, `:269-285`, `store/persistence.rs:43-48`,
`store/gc.rs:8`, `gc.rs:162-207`, `:365-381`,
`v1_lifecycle/store/archive.rs:11-86`,
`v1_lifecycle/authority/dispatcher.rs:17-24`, `:206-219`, `:221-297`, `:308`,
`:338`, `v1_lifecycle/transition/mod.rs:89-98`, `:115-124`,
`record_wire/archive/cleanup.rs:149-197`,
`src/operation/open_merge_gate.rs:51-76`, `:93-99`,
`src/operation/workspace_mutator_lock.rs:14-21`, `:35-38`,
`protocol/gwz.taut.py:207-212`, `:546-608`, `:730-737`, `:749-757`,
`:773-777`, `:796-806`, `:1163-1168`, `:1599-1610`, `:1696-1707`,
`src/protocol/generated.rs:244-268`, `:1341-1532`, `:1919-1921`, `:3017-3043`,
`:4395-4428`, `:4865-4907`, `gwz-cli/src/globalargs/parser.rs:119-125`,
`:373-422`, `gwz-cli/src/clirequest/merge.rs:7-31`,
`gwz-cli/src/clirequest/invocation.rs:80-83`,
`gwz-core/src/workspace_ops/merge/validate.rs:55-56`,
`merge/finalize.rs:93-97`, `merge/start/record.rs:83`.

---

## 0. Decided inputs

All ten questions of `GwzM5-8OperatorEscapeDesign.md` §10 are **DECIDED**
(program owner, 2026-08-16). Q1-Q7 adopt the design's own recommended option
exactly as its §10 states it; Q8 and Q10 are operator decisions that **differ
from the design's draft lean** and are marked as such; Q9 is a shipping plan
and carries no wire content.

| Q | Decision | Provenance | Recorded in |
| --- | --- | --- | --- |
| Q1 Wire placement | New **sixth top-level field `operator_override`** — collision-checked, immutable-once-written per row, archived. Reusing `operation_drift` rows stays rejected (drift is diagnostic, "never mutation authority"). | Design §10 [Q1] recommendation adopted verbatim | §2.1, §2.2 |
| Q2 Protocol shape | **Append `MergeOp::{Quarantine, Restore}` plus escape flags on `Abort`** (the design's first-listed option, matching its own §6.3 CLI sketch), plus the field-10/status projection additions: override presence + disposition summary and a `quarantined` record source. | Design §10 [Q2] first option adopted; the design flags that P3's settled parity matrix is thereby reopened — bounded in §0.1 | §2.5, §0.1 |
| Q3 Package placement | **Wire/transition amendment rides pre-A1**; **implementation is its own reviewed package after R4b-G, before or at A1.** Quarantine is production-reachable immediately, force-abandon only when v1 records are (A1). | Design §10 [Q3] recommendation adopted; consistent with `GwzM5-8A1DecisionPacket.md` §4 steps 2-3 | §9 |
| Q4 Restore strictness | **Byte-identical restore only.** Hand-edited quarantined bytes are permanently manual surgery; the "checked re-admit of edited bytes" door is **formally shut**. | Design §10 [Q4] draft position adopted; the maintainer's request that the door be "formally noted shut" is honored | §3.2 |
| Q5 GC release valve | **Accepted.** `gwz merge --gc <id> --forget-refs --force` deletes the *archive* while leaving every mismatched/unavailable ref in place; no repository mutation; refs leak namespaced and inert. | Design §10 [Q5] proposal accepted | §3.4 |
| Q6 Runbook location | **User docs page plus a pointer line in the stuck-state error/status output.** | Design §10 [Q6] recommendation adopted | §2.6, §3.5 |
| Q7 Naming | **`--quarantine` / `--restore` / `--force-abandon`.** Frozen into protocol and help text at this freeze. | Design §10 [Q7] first option adopted; already user-committed by `GwzMergeWedgeRunbook-v0.md` §6.3, which publishes `.gwz/merge/quarantine/` | §2.5, §10.2 (internal-vocabulary caveat) |
| Q8 `--reason` | **OPERATOR DECISION, differs from the design's lean.** `--reason` is **OPTIONAL on every escape command** (quarantine, restore, force-abandon, `--forget-refs`). When supplied it is recorded **verbatim** in the `operator_override` evidence (and in the quarantine sidecar). The design recommended mandatory-for-force-abandon; that recommendation is **not adopted**. | Program owner, 2026-08-16 | §2.2 (`reason` field + inert-text rules), §3.3 |
| Q9 v0 scheduling | **Runbook-first shipping plan, no wire content.** (a) The v0-line runbook `GwzMergeWedgeRunbook-v0.md` publishes after its staged verifications complete (its Appendix C `[UNVERIFIED]` checklist); (b) tool-supported quarantine ships with the **next regular dev-line release**, not as a v0.10.x special; (c) **no v0-line maintenance backport** unless the operator later requests one. | Program owner, 2026-08-16 | §9.3 |
| Q10 Preservation-owner marking | **OPERATOR DECISION, differs from the design's draft.** Per-owner `Unrecoverable` dispositions are consented **per side** — preservation side and rollback side independently. The tool may collapse the two into a **single prompt** only when one side's failure **provably implies** the other (closed basis set, §3.3.4), and the collapsed consent text must **name both destructions explicitly**. The design's draft ("name once, applies to both passes") is **not adopted**. | Program owner, 2026-08-16 | §2.2 (per-side record shape), §3.3.3-§3.3.5, §3.3.6 (half-marked owner) |

### 0.1 [Q2] — what this reopens in the settled P3 projection scope, and how it is bounded

P3's ownership is settled (`GwzM5-8R4bReverseLifecycleInterface.md` §9
:482-495: P3 owns "the Taut append, generated Rust/Python bindings, the one
semantic projection owner, response population, renderers, and parity fixtures
before A1"; `GwzM5-8ChangeBudget.md` prices it at 18 charged paths). Adopting
[Q2]
**reopens that settled scope in exactly five places**, and in no others:

1. **Request vocabulary.** `MergeOp` gains two values
   (`gwz.taut.py:207-212`, five values today) and `MergeRequest` gains one
   nested optional field carrying the escape parameters (§2.5.1).
2. **A third record source.** `GwzM5-8R4bReverseLifecycleInterface.md` §9
   opens "Status has two sources and no mutation path" (:441) and its source
   arbitration table has exactly two axes. Quarantine adds a third, bytes-only
   source. The frozen `MergeRecordProjection.archived` field (field 2, a
   non-optional BOOL, `gwz.taut.py:1165`, `generated.rs:3019`) **cannot express
   three states** — hence an appended `source` enum field (§2.5.2).
3. **Projection payload.** `MergeRecordProjection` gains override and
   quarantine sub-messages (§2.5.2).
4. **Field-10 population list.** The frozen list
   (`GwzM5-8I2ProtocolContract.md` §3 :202-205) gains the quarantine, restore,
   and force-abandon responses.
5. **Error allocation.** Four appended codes (§2.5.4).

**Bounding — the six rules this amendment binds itself to:**

- **Append-only, no renumbering.** No existing discriminant, field number,
  message shape, message name, rendering rule, or error code changes. Every
  addition takes the next free slot: `MergeOp` 5-6; `MergeRequest` field 8;
  `MergeResponse` field 11; `MergeRecordProjection` fields 6-8; error codes
  62-65 (61 is the current maximum, contiguous 0..61,
  `gwz.taut.py:546-608`, `generated.rs:1341-1532`).
- **`archived` stays authoritative and unchanged.** The appended `source`
  is optional; the pinned equivalence is
  `archived == (source == archived)`, a quarantined record renders
  `archived=false`, and `MergeResponse.open` (field 4) renders `false` for a
  quarantined record. Fixtures pin all three together.
- **No live access from the new source.** The quarantined projection is
  derived from quarantined bytes and the sidecar only, never from any
  repository, exactly as the archived projection is (Refactor §15.3.3).
- **No redistribution of ownership.** P3 remains the sole projection owner,
  the sole Taut-append owner, and the sole renderer owner. This amendment adds
  rows to P3's existing matrix; it moves no responsibility to P1/P2/P4.
- **No terminal-outcome vocabulary change.** `MergeTerminalOutcome`
  (`completed=0, aborted=1`) and `ArchivedTerminalOutcome` are **untouched**:
  an overridden abort is `aborted` plus an override presence marker, never a
  third outcome (§5.4).
- **Deferrable without a corridor.** Taut appends are append-only, so the
  generated artifacts may land with the implementation package (at or before
  A1) without becoming a same-version corridor problem. Only the **record**
  wire (§2.1-§2.3) must freeze pre-A1.

Cost, stated for the reviewers rather than absorbed: this is the larger half
of the amendment's price, and it lands in the single most parity-sensitive
lane in the program (four renderers, two drivers, corpus goldens). §9 prices
it separately from the record/transition work for exactly that reason.

---

## 1. What this amends — verbatim frozen clauses

Every frozen sentence this amendment changes is quoted here first. Clauses
quoted as **UNCHANGED** are reproduced because a new edge is their analogue
and reviewers must see that the original obligation is not weakened.

**Anchors below are pre-acceptance**, as reviewed. Applying §7 at acceptance
(2026-08-22) inserted a dated head banner and dated §-local annotations into
each of the five contracts, so every anchor in this section now sits lower in
its file by the annotations preceding it. The **verbatim quote, not the line
number, is the identifying key** — every quoted sentence below was re-verified
byte-identical in the post-acceptance files, and each is now reachable from
the dated annotation placed immediately after it. Verified post-acceptance
re-pin offsets, by region within each file: RecordContract +11 (§1), +17
(§6), +30 (§7), +47 (§8), +61/+62 (§9); ActionJournal +23 (§2 through
:185-192), +59 (§2 :194-195 onward, §5); TransitionDesign +15 (§6 :325-328),
+44/+45 (§6 payload authority and the §6.1 rows), +57 (§6.6), +74 (§6.8,
§6.9), +124 (§7 :843-844), +134 (§7 :929-933), +143 (§13);
ReverseLifecycleInterface +10 (§9 :441-459), +47 (§9 :482-495), +41/+42
(§11); ProtocolContract +14 (§1), +36 (§2), +46 (§3), +69 (§4). Frozen prose
was **never rewritten in place** — a requirement, not a style choice: the
document-consistency gate pins RecordContract's "It adds five top-level
fields:" and "five top-level v1 collisions" as required strings, so the
amended readings can only live in annotations beside them.

### 1.1 `GwzM5-8I2RecordContract.md`

Anchors in this document are **post-W1-banner** (the 2026-08-22 M5b-W1 banner
shifted every line by +21; all re-verified for this revision). The W1
clarification itself composes with this amendment disjointly — §6.2 names the
composition.

**§1 :59-72 — the five top-level fields (AMENDED, §2.1):**

> `MergeOperationRecordV1` retains every v0 field, field name, enum spelling,
> and nested container at the same YAML path. It adds five top-level fields:

```rust
struct MergeOperationRecordV1 {
    // The complete MergeOperationRecordV0 body follows unchanged, except for
    // the v1 envelope and v1 validation of mode: no_ff.
    accepted_workspace: Option<AcceptedWorkspaceV1>,
    recovery_context: Option<RecoveryContextV1>,
    pending_rollback: Option<PendingRollbackActionV1>,
    pending_preservation: Option<PendingPreservationActionV1>,
    preservation_publication_handoff: Option<PreservationPublicationHandoffV1>,
}
```

**§7 :381-390 — forward-action abandonment (AMENDED, §3.3.7):**

> A durable forward action may also retire without an integration outcome only
> when an abort or preserve-abort request consumes the matching bound exact
> `NotStarted` observation. That same atomic rewrite records deliberate
> abandonment by clearing the action, leaving the participant's pre-action
> state/evidence unchanged, and entering `RollingBack` or `Preserving` under the
> matching checked entry proof. It never reports success, conflict, or failure
> for an action that did not run. There is no intermediate action-free
> `Executing`/`Halted` abandonment row. Continue never uses this path.
> Completed, expected-conflict, or ambiguous observations cannot be treated as
> abandonment.

**§8 :394-400 — the top-level collision doctrine (AMENDED, §5.2):**

> Migration first records every v0 unknown YAML path and raw value. The v1 body
> keeps all surviving v0 containers at the same path, so their unknown fields
> remain at that exact path and value. A v0 unknown top-level field named
> `accepted_workspace`, `recovery_context`, `pending_rollback`,
> `pending_preservation`, or `preservation_publication_handoff` collides with a
> v1 known field and makes migration ineligible; it is never adopted,
> overwritten, or moved.

**§8 :407-433 — the retirement table and sequence identities (AMENDED, §5.3);
the operative rows quoted (the evidence row :417; the identity sentence
:424-430):**

> | participant/publication-root preservation evidence | one stable row per owner; the same row survives as ref/stash/no-op/reset fields fill and survives archival |

> Sequence rewrites match unknown descendants by these exact identities before
> preserving their new positions: conflict evidence `(path, sha256)`;
> preservation evidence by its stable owner (`participant:<id>` or
> `publication-root`, with at most one row); candidate hash `path`; participant
> drift by `(kind, every expected/live field, occurrence index among identical
> keys)`; and operation drift by its unique `kind`.

**§6 :315-324 — the acceptance legality table (UNCHANGED).** An overridden
abort obeys the same two aborted rows (:323-324: "aborted after accepted
publication/preservation/rollback work | present"; "aborted before acceptance |
absent") with no exception.

### 1.2 `GwzM5-8I2ActionJournalContract.md`

**§2 :173-176 — retirement authority (AMENDED, §3.3.7):**

> An owner/kind must agree with participant, publication, acceptance, and
> operation-baseline evidence. Ambiguous observation changes only operation
> state/context; it never clears or advances a pending action. Retirement occurs
> only with the verified result/progress write.

**§2 :164-171 — the pending-action legality table (UNCHANGED).** Any journal
a mark retires is cleared by that mark's own atomic rewrite (§3.3.7), and the
overridden abort — like the ordinary one — requires that **no** journal
remain (§2.3.2), so the "direct or terminal state | neither pending reverse
action is legal" row remains true without exception. No row is added or
altered.

**§2 :178-181 — the rollback cursor's closed derivation-input list (AMENDED,
§3.3.2, §7 item 2). This sentence is decode law; without amending it, a
post-mark `BeginParticipantRollback` for the next unmarked owner would fail
its own post-write re-decode as "a later owner":**

> For rollback, owner/kind agreement includes the exact deterministic current
> cursor derived from durable participant terminal states, publication evidence,
> selected-root membership, and pending phase; a wrong or later owner is invalid
> at decode.

**§2 :185-192 — the two-arm prefix proof (the D3-amended sentence; AMENDED by
a third arm, §3.3.2, §7 item 2). Without the third arm, a marked earlier
preservation position would demand exact live observation of a destroyed
repository — precisely the wedge force-abandon exists to break:**

> Before a pending preservation action is classified, advanced, or executed, an
> exact bound observation must also prove — by durable completion facts where
> present, and by exact bound live observation where absent — that every
> earlier position in the
> two-pass cursor is complete or currently unnecessary.

**§5 :414-421 — archived cleanup ownership (AMENDED by [Q5], §3.4):**

> - absent is already complete;
> - the exact target authorizes checked deletion; and
> - a different target or unavailable repository stops cleanup and retains the
>   archive.
>
> The archive may be deleted only after every recorded ref is observed absent.
> Native stashes and stash bundles are never deleted by merge GC. An archive
> with no backup refs may be deleted after archive-only validation.

### 1.3 `GwzM5-8R4bTransitionDesign.md`

**§6 :325-328 — the closed vocabulary rule (AMENDED by one arm, §2.3):**

> There is no `SetField`, `SetState`, `SetPublicationStep`, arbitrary patch, or
> numeric phase comparison. Every enum is exhaustively matched. Adding a
> variant requires an explicit predecessor rule, field footprint, unknown-field
> effect, positive test, and cross-owner rejection tests.

**§6.8 :795-798 — the licence this amendment relies on (CITED, not amended):**

> Callers cannot compose a drift change with another transition in one write.
> If a future invariant requires an atomic compound transition, it must be a new
> closed enum variant with an independently reviewed footprint; it is not a
> generic composition facility.

**§6.1 :446, :448 — the two ordinary edges whose analogues this adds
(UNCHANGED — their proof obligations are NOT weakened, §2.3.4):**

> | `BeginRollback` | `Executing`, `AwaitingResolution`, `Halted`, `Finalizing`, or `Preserving`; no pending forward or preservation action remains; bound `PreparedRollbackEntry` includes exact global preflight and `VerifiedPublicationHandoff`; the `Preserving` origin additionally requires `VerifiedPreservationExhausted` | state becomes `RollingBack`; … |

> | `AbortOperation` | `RollingBack`; no pending action/journal; bound `VerifiedRollbackExhausted` proof covers every participant and exact selected-root baseline | state becomes `Aborted` |

**§6.6 :648-652 — the reverse-cursor derivation and its decode law (AMENDED,
§3.3.2, §7 item 3). The same defect as ActionJournal §2 :178-181: a closed
input list with no disposition arm, and checked-open validation enforces it:**

> Only the derived current owner can be started or advanced. The reverse cursor
> is fully derivable from participant terminal states, publication rollback
> evidence, selected-root membership, and the exact pending phase. Checked-open
> validation rejects a pending action whose owner, kind, terminal state, or
> phase is not that exact cursor.

**§7 :843-844 — the closed request form (AMENDED, §2.3.1):**

> `V1LifecycleRequest` is the closed internal form of resume-start, continue,
> abort, preserve, status, and archive work.

**§7 :929-933 — preservation-first ordering (AMENDED in scope only, §3.3.2):**

> When the current state is `Preserving`, the dispatcher completes the exact
> two-pass preservation cursor even between actions, regardless of an incoming
> abort request. It may return `BeginRollback` only after a fresh bound
> `VerifiedPreservationExhausted` proof. There is no implicit abandonment of a
> partially completed preservation plan.

**§6.9 :800-829 — the physical mutation ownership matrix (AMENDED by three
rows, §2.4).**

### 1.4 `GwzM5-8R4bReverseLifecycleInterface.md`

**§9 :441-459 — status sources (AMENDED, §2.5.2, §3.2.4):**

> Status has two sources and no mutation path:
>
> - an open v1 record is loaded through `CheckedV1Store`, then projected from the
>   checked typed record plus read-only live observations; and
> - a named archived v0/v1 record is decoded solely from archive bytes through
>   the R3 archive decoder and projected without consulting live repositories.

**§11 :529-546 — targeted GC (AMENDED by [Q5], §3.4):**

> - absent ref: already complete;
> - exact recorded target: eligible for checked deletion;
> - different target or unavailable repository: stop and retain the archive.
> …
> Archives with no backup refs may be deleted immediately after archive-only
> validation. Native stashes and preservation bundles are never deleted.

### 1.5 `GwzM5-8I2ProtocolContract.md`

**§1 :12 (error appends), §2 :86-119 (enum allocation), §3 :124-129 and
:202-205 (`MergeRecordProjection` and the field-10 population list) — all
AMENDED by append only (§2.5).** The operative frozen shape:

> ```text
> MergeRecordProjection {
>   1 source_version: MergeRecordVersion, 2 archived: bool,
>   3 terminal_outcome?: MergeTerminalOutcome,
>   4 acceptance?: MergeAcceptanceProjection,
>   5 recovery?: MergeRecoveryProjection,
> }
> ```

**§1 :46-53 — the context-legality sentences (AMENDED, §7 item 5). These
enumerate by code, not by table membership; without amending them, appended
codes 62-65 would fall under "every other error forbids context" while §2.5.4
requires them to carry it:**

> Codes 46/47 require applicable context; codes 49–61 require id
> and installed pair; code 48 includes every valid field available before the
> contradiction. […] Every
> other error forbids context. Codes 46–61 have absent member/detail/target
> fields.

**§3 :208-210 — the record-context range sentence (AMENDED, §7 item 5):**

> Its
> `GwzError.record_context` must carry the merge id and exact schema/version pair
> whenever the header is readable and the code is one of 45–61,
> `MergeDrift`, or `MergeRecoveryRequired`.

### 1.6 `GwzM5-8OperatorEscapeDesign.md`

**§2 rule 1 :75-78 — the separate-consent-lane doctrine (REFINED, §3.3.2):**

> 1. **Escapes are a separate consent lane, never a fallback.** No automatic
>    path may select an escape. An escape runs only from an explicit operator
>    request carrying explicit destructive consent (§7.2). The lifecycle
>    dispatcher, observers, and reducers remain byte-for-byte ignorant of them.

---

## 2. Wire

### 2.1 The sixth top-level field

`MergeOperationRecordV1` (`model/v1/record.rs:18-51`) gains one trailing
optional field, joining the five of RecordContract §1:

```rust
struct MergeOperationRecordV1 {
    // … the five existing v1 fields, unchanged …
    preservation_publication_handoff: Option<PreservationPublicationHandoffV1>,
    // New in this amendment. Absent-by-default on the wire.
    #[serde(default, skip_serializing_if = "Option::is_none")]
    operator_override: Option<OperatorOverrideV1>,
}
```

- **Name/spelling.** Exactly `operator_override` (snake_case, matching the
  five existing top-level names). The name is free tree-wide today.
- **Position.** Appended after `preservation_publication_handoff` and before
  the flattened `extensions` map; YAML emission order is declaration order
  (`record.rs:39-50`; the same rule
  `GwzM5-8DurableCursorAmendment.md` §2.1 fixed for the evidence row).
- **Encoding.** Absent when unset (`skip_serializing_if`), never `null` —
  every byte stream produced by a writer that does not set it is identical to
  today's.
- **Not a state.** Presence of the field never changes `state`, and `state`
  never implies presence except at the terminal legality rule of §2.2.4.

### 2.2 The `operator_override` block

```rust
struct OperatorOverrideV1 {
    // Append-only; ordered by owner key; unique by owner (each row carries
    // its two optional sides; unknown-descendant identity is (owner, side), §5.3).
    dispositions: Vec<OwnerDispositionV1>,
    // One-shot consent slots, each written by exactly one escape edge.
    #[serde(default, skip_serializing_if = "Option::is_none")]
    overridden_entry: Option<OperatorConsentV1>,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    overridden_abort: Option<OperatorConsentV1>,
}

struct OwnerDispositionV1 {
    owner: EscapeOwnerV1,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    preservation: Option<SideDispositionV1>,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    rollback: Option<SideDispositionV1>,
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum EscapeOwnerV1 {
    Participant { member_id: String },
    PublicationRoot,
    PublicationEvidence,
    SelectedRootMetadata,
}

struct SideDispositionV1 {
    kind: SideDispositionKindV1,          // exactly `unrecoverable` at this amendment
    consent: OperatorConsentV1,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    collapse: Option<ConsentCollapseV1>,
    evidence: AbandonEvidenceV1,
    // At most one retired action per side; verbatim copies, never summaries.
    #[serde(default, skip_serializing_if = "Option::is_none")]
    retired_preservation_action: Option<PendingPreservationActionV1>,  // preservation side only
    #[serde(default, skip_serializing_if = "Option::is_none")]
    retired_rollback_action: Option<PendingRollbackActionV1>,          // rollback side only
    #[serde(default, skip_serializing_if = "Option::is_none")]
    retired_forward_action: Option<PendingMergeAction>,                // rollback side only
}

#[serde(rename_all = "snake_case")]
enum SideDispositionKindV1 { Unrecoverable }

struct OperatorConsentV1 {
    operator: String,          // claimed actor id; see §2.2.3
    recorded_at_ms: String,    // decimal epoch-milliseconds, the `created_at` convention
    writer_version: String,
    record_sha256: String,     // the consent round's anchor: digest of the bytes the manifest was printed from (§3.3.3)
    confirm_token: String,     // the first 16 lowercase hex chars of record_sha256
    #[serde(default, skip_serializing_if = "Option::is_none")]
    reason: Option<String>,    // [Q8] optional; inert diagnostic text; verbatim
}

struct ConsentCollapseV1 {
    basis: ConsentCollapseBasisV1,
    implied_side: EscapeSideV1,   // this row's own side (§3.3.4); present iff collapsed
}

#[serde(rename_all = "snake_case")]
enum ConsentCollapseBasisV1 { RepositoryMissing, RepositoryUnreadable }
#[serde(rename_all = "snake_case")]
enum EscapeSideV1 { Preservation, Rollback }

struct AbandonEvidenceV1 {
    code: ErrorCode,             // the exact typed error that motivated the mark
    reason: String,              // one exact registered predicate reason, never free-form
    observed_at_ms: String,
    observed: Vec<ObservedFactV1>,   // ordered by kind; unique by kind
}

struct ObservedFactV1 { kind: ObservedFactKindV1, value: String }

#[serde(rename_all = "snake_case")]
enum ObservedFactKindV1 {
    RepositoryPath, RepositoryState,          // missing | unreadable
    ExpectedBranch, LiveBranch,
    ExpectedCommit, LiveCommit,
    ExpectedRef, LiveRefTarget, RefState,     // absent | retargeted | unreadable
    StashId, StashState,                      // absent | foreign | unreadable
    BundlePath, BundleState,                  // absent | mismatched | unreadable
}
```

#### 2.2.1 Encoding rules

- All object ids, refs, branches, paths, member ids, and SHA-256 strings use
  the validators already applied to their existing sources (RecordContract §2
  :194-197). SHA-256 values are lowercase 64-character hexadecimal.
- `recorded_at_ms`/`observed_at_ms` are decimal strings of the injected
  clock's epoch-milliseconds value — exactly the existing `created_at`
  convention (`merge/start/record.rs:83`,
  `clock.now_ms().0.to_string()`). No new time format is introduced.
- `confirm_token` is a **derivation**, not free text: exactly the first 16
  lowercase hex characters of `record_sha256`. Validation rejects any other
  value. (The design's §6.3 sketch used 8 hex characters; 16 is adopted so the
  token cannot be brute-matched against an unrelated record by an operator
  pasting a stale token — see §10.3.)
- `evidence.reason` is one exact reason string registered in
  `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json`, the same
  discipline the protocol contract already imposes on codes 49-61
  (`GwzM5-8I2ProtocolContract.md` §1 :82-84). It is never free-form.
- **Consent duplication is deliberate.** Each side row carries its own full
  `OperatorConsentV1` rather than a reference to an edge slot. There are no
  intra-record cross-references: every row is independently auditable, and no
  lineage hazard is created by a later row's rewrite.

#### 2.2.2 `reason` — the one carve-out from the no-free-strings doctrine

`GwzM5-8I2ActionJournalContract.md` §2 :194-195 freezes "All free-looking wire
strings are validated derivations, never ambient authority." `reason` is the
single deliberate exception, and it is bounded so the doctrine survives:

- It is **inert diagnostic text**, in the same class as `MergeRecordError.message`
  which RecordContract §8 :420 already declares non-identity ("diagnostic
  message text is not identity").
- It is excluded from **every** identity, equality, legality, cursor,
  exhaustion, footprint-diff, and proof computation. No decision anywhere
  reads it. Changing it changes no behavior; it cannot be made authority
  without a further amendment.
- Validation is structural only: valid UTF-8, at most 4096 bytes, no NUL and
  no C0 control character other than LF. Recorded **verbatim** otherwise — no
  trimming, normalization, case folding, or line-ending rewriting.
- It is optional on every escape command ([Q8]). Absence is a legal, complete
  record; renderers print `reason: (none supplied)` and never synthesize one.

#### 2.2.3 `operator` — claimed, never authenticated

`operator` is `meta.attribution.actor.actor_id`
(`gwz.taut.py:730-737`, `:806`), falling back to the literal `unknown` exactly
as the existing candidate path does (`merge/finalize.rs:93-97`). It is
**provenance, not authority**: GWZ has no actor authentication, and this
amendment adds none. Stated loudly rather than absorbed: the audit trail
answers "what was done, to what bytes, when, and with what consent shape", not
"who really typed it". §8.6 pins that no code path treats `operator` as an
access decision.

#### 2.2.4 Structural legality

Normative rules; the tests of §8.1 enumerate them exhaustively.

1. **Owner/side legality.** Only `Participant` owners may carry both sides.
   `PublicationRoot` may carry **preservation only**; `PublicationEvidence` and
   `SelectedRootMetadata` may carry **rollback only**. Any other combination
   is a decode contradiction.
2. **Owner domain.** Every `Participant { member_id }` must name a participant
   present in the record (including `@root` when the root is a selected
   participant). `PublicationRoot` is legal only when publication composition
   evidence exists and no selected `@root` owns the same root ref (the §2
   ActionJournal collision rule for the two root owners applies unchanged).
   `SelectedRootMetadata` is legal only for a selected-root operation.
3. **Uniqueness and order.** At most one `OwnerDispositionV1` per owner;
   rows are ordered by owner key (`selected_targets` order, then
   `publication-root`, `publication-evidence`, `selected-root-metadata`).
   Duplicates reject.
4. **Non-empty.** A present block must contain at least one disposition with
   at least one side. An `OwnerDispositionV1` with both sides absent is
   invalid — absence of the row is the empty state (the same shape rule
   `GwzM5-8DurableCursorAmendment.md` §2.2 row 1 applies to evidence rows).
5. **Retired-action agreement.** `retired_preservation_action` is legal only
   on a preservation side and must equal, byte-for-byte, the record's
   `pending_preservation` at the moment of the mark, whose owner must be this
   owner. `retired_rollback_action` is legal only on a rollback side, must
   equal `pending_rollback`, and its owner must be this owner.
   `retired_forward_action` is legal only on a rollback side of a
   `Participant` owner and must equal that participant's `pending_action`. A
   retired-action field whose content is not derivable from the record's own
   journal at mark time is a decode contradiction.
6. **Journal exclusivity preserved.** A side row carrying a retired action and
   a *live* journal of the same owner/kind cannot coexist: the mark write
   clears the journal in the same atomic rewrite (§3.3.7).
7. **Collapse legality.** `collapse` is present on a side row exactly when
   that side's consent arrived through the `:both` collapse (§3.3.4) rather
   than a separate naming, and each row's `implied_side` equals **that row's
   own side**. The `basis` must be corroborated by an
   `ObservedFactV1 { kind: RepositoryState, value: "missing" | "unreadable" }`
   on **both** sides of the same owner; both sides must carry `collapse` with
   the same `basis`; and both sides must carry the same `consent` (identical
   operator, timestamp, digest, token — constructible because a collapsed
   pair is written by **one** atomic `MarkOwnerUnrecoverable` rewrite
   (§2.3.2) carrying the round's single consent (§3.3.3), so no crash can
   split the pair and no second write can move the digest between them). Any
   other shape rejects.
8. **Terminal legality — the decode form, decidable from durable facts
   alone.** `overridden_abort` is present **exactly** when `state == Aborted`
   and the record was terminalized by the overridden edge. Its presence
   requires, for every owner in the closed domain, that each **durably
   applicable** side (§3.3.6's decode derivation: rollback sides by the
   closed cursor-visit rule; preservation sides **iff** that owner carries a
   preservation evidence row or a preservation-side disposition) is either
   proven-retired by durable ordinary evidence or marked. A position retired
   by live proof alone leaves no durable witness, is therefore not durably
   applicable, and is **never re-derived at decode**: the consuming edge's
   bound proof re-proved it live (§2.3.2, §3.3.6 write-edge form), and the
   decode form trusts the recorded edge — an open or archived overridden
   record is never read-invalid for lack of a live fact, and the terminal
   edge never deadlocks on a preservation plan that was actually fine.
   `overridden_abort` present with `state != Aborted`, or with any durably
   applicable side neither proven nor marked, is invalid. `overridden_entry`
   present requires that the record has been in `RollingBack` (or is
   terminal); it never appears alone on a record that never left
   `Preserving`/`RecoveryRequired`.
9. **Immutability.** Every written `OwnerDispositionV1` side and both consent
   slots are **immutable once written**, joining the record's existing
   immutability discipline. The block itself is **append-only**: dispositions
   may be added while the record is open; no row is ever edited, reordered
   into a different identity, or removed. There is no retirement edge for the
   block, in the open lifetime, at archival, or after GC.
10. **v1 only.** The field is invalid in a v0 record (§5.2).

### 2.3 Transitions

#### 2.3.1 One new sub-enum, three variants

`V1Transition` (TransitionDesign §6; `transition/mod.rs:89-98`) gains one arm,
so the escape vocabulary is structurally separate from the ordinary
vocabulary and no ordinary enum grows:

```rust
enum V1Transition {
    Operation(OperationTransition),
    Participant(ParticipantTransition),
    Acceptance(AcceptanceTransition),
    Publication(PublicationTransition),
    Recovery(RecoveryTransition),
    Preservation(PreservationTransition),
    Rollback(RollbackTransition),
    Drift(DriftTransition),
    // New in this amendment.
    Escape(EscapeTransition),
}

enum EscapeTransition {
    MarkOwnerUnrecoverable { mark: PreparedOwnerMark },
    BeginRollbackOverridden { entry: PreparedOverriddenRollbackEntry },
    AbortOperationOverridden { proof: VerifiedOverriddenRollbackExhaustion },
}
```

`V1LifecycleRequest` (`dispatcher.rs:17-24`) gains one variant,
`ForceAbandon`, carrying the bound escape request. `EffectKind`
(`transition/effect.rs:24`) gains the three mirroring effects.
`V1ResponseDisposition` gains `EscapeManifest` (the no-op first pass of §3.3.3).
(Code idiom: the `V1Transition` arms are `Box`ed —
`Operation(Box<OperationTransition>)`, `transition/mod.rs:89-98`; the sketch
above and the §7 item 3 edit prescribe the arm and its payload authority, not
its boxing.)

Each payload is an opaque bound value carrying the §8 proof binding, per the
frozen rule that "variants that carry data accept only opaque bound values,
never raw strings/records supplied by a lifecycle caller" (TransitionDesign §6
:334-337). No escape payload is constructible from a member id or a request
string.

#### 2.3.2 Predecessor and footprint table (the §6.1-style rows)

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `MarkOwnerUnrecoverable` | any open state (`Executing`, `AwaitingResolution`, `Halted`, `Finalizing`, `Preserving`, `RollingBack`, `RecoveryRequired`); request is `ForceAbandon`; bound `PreparedOwnerMark` carries the fresh full preflight classification proving the named owner/side — or, for a provable collapse, both sides — permanently unobservable, the current §3.3.3 round's digest-bound consent, and the verbatim journal copy when one is retired; the owner/side is not already marked | appends exactly one `OwnerDispositionV1` side — or, for a provable-collapse `:both` mark, **both sides of that one owner in the same rewrite** (§3.3.4; this is what makes rule 7's identical-consent pair constructible and un-splittable by a crash) — creating the row, and the block, when absent, and, in the **same** atomic rewrite, clears the retired journal field it copied (`pending_preservation`, `pending_rollback`, or the participant's `pending_action`; at most one exists under the §1.2 legality table). **No state change**, no participant outcome, no acceptance/candidate/evidence touch |
| `BeginRollbackOverridden` | `Executing`, `AwaitingResolution`, `Halted`, `Finalizing`, `Preserving`, or `RecoveryRequired`; **never `RollingBack`**; no forward or reverse journal remains (marked owners' journals were retired by their marks; unmarked owners' journals must be absent exactly as ordinary `BeginRollback` requires); bound `PreparedOverriddenRollbackEntry` = the ordinary `PreparedRollbackEntry` computed over the **unmarked** member domain, plus `VerifiedPreservationExhaustedOrMarked` when the origin is `Preserving`, plus the complete marked-side table, plus the current round's digest-bound consent (§3.3.3) | state becomes `RollingBack`; writes `overridden_entry`; clears `recovery_context` in the same write when the predecessor is `RecoveryRequired` (the ActionJournal §2 :145-147 rule that leaving recovery clears the context in the same write is honored, not excepted) |
| `AbortOperationOverridden` | `RollingBack`; no pending action/journal; bound `VerifiedOverriddenRollbackExhaustion` = `VerifiedRollbackExhausted` **restricted to unmarked owners**, plus the complete marked-owner disposition table, plus a preservation exhausted-or-marked **re-proof** over the write-edge preservation domain (§3.3.6 — exact live re-proof precisely where no durable witness exists), plus the current round's digest-bound consent (§3.3.3) | state becomes `Aborted`; writes `overridden_abort`. Participants rolled back exactly keep their ordinary `Aborted`/`RolledBack` terminal rows; **marked participants keep their last honest state** (`Merged`, `Conflicted`, …) — no fabricated terminal outcome. The rewrite is decode-legal under the checked-record legality delta of §7 item 3: with `overridden_abort` present, a participant may stand non-rollback-terminal **iff** its rollback side carries a disposition (`model/v1/validate/lifecycle.rs:56` is amended for exactly this shape; unchanged when `overridden_abort` is absent) |

Idempotence follows the frozen rule: it is handled by reading the current
state and selecting no mutation, never by same-state variants. A predecessor
not listed is rejected.

#### 2.3.3 Why the entry edge exists (a correction to the design)

`GwzM5-8OperatorEscapeDesign.md` §6.1 proposes a single new transition whose
predecessor set is "(`RollingBack`, or `Preserving` when every unmarked owner
is exhausted and rollback entry itself is marked)". Verified against the tree,
that set makes force-abandon **unreachable for the design's own flagship
wedge**: U1 (a member repository deleted) blocks the reverse-**entry**
preflight, which "treats an operational read failure as an error, never
`false`" (design §3.3 U1 row), so a `Halted`/`Executing`/`AwaitingResolution`/
`Finalizing` record with a deleted member can never reach `RollingBack` at
all — and `PendingRollbackActionV1` is legal only in state `rolling_back`
(ActionJournal §2 :164-171), so no participant can be rolled back either.

Two repairs were possible. Widening `AbortOperationOverridden` to every open
state was rejected: it would jump straight to `Aborted` and thereby **abandon
every tractable owner too**, violating the design's own rail that
"force-abandon may not skip an owner the machinery can still handle" (§4.3)
and maximizing rather than minimizing destruction. The adopted repair is the
separate `BeginRollbackOverridden` edge, which lets the unchanged ordinary
per-owner machinery restore every tractable owner before anything is given up.
It also gives [Q10]'s per-side consent a **structural** home rather than a
merely procedural one: the preservation-side marks are consumed by the entry
edge, the rollback-side marks by the terminal edge. Reviewer question R-1
(§10.3) asks both axes to confirm this widening.

#### 2.3.4 What is NOT weakened

- `OperationTransition::BeginRollback` and `::AbortOperation` keep their
  frozen §6.1 rows **byte-for-byte**, and the ordinary path can never
  terminalize an overridden record — **by definition, not by derivation**: a
  bound `VerifiedRollbackExhausted`, a bound `VerifiedPreservationExhausted`,
  and a `PreparedRollbackEntry` for the `Preserving` origin are **not
  constructible while any `operator_override` disposition exists**, whatever
  the live world shows (§7 item 3 writes this into the frozen note). The
  complete-domain wording alone would not suffice: with preservation-side-only
  marks, an ordinary rollback pass can genuinely satisfy the complete rollback
  domain — the definitional rule is what keeps a plain `Aborted` off an
  overridden record. And in a healed world (a wrongly-marked repository
  restored), ordinary `BeginRollback` from a non-`Preserving` origin remains
  obtainable and safe: it enters, cursor derivation still skips marked sides,
  nothing is executed for them, and the terminal edge still refuses — the
  record terminalizes only through force-abandon's own consent gate (§8
  item 7's healed-world row pins this; permanence per §10.3 R-3). Only the
  escape edges can terminalize such a record, and only with the current
  round's digest-bound consent.
- No escape transition may be returned by `next_action` for any request other
  than `ForceAbandon`; no observation, resolver, or executor path may
  construct an `EscapeTransition` payload. This is policed by the same
  closed-vocabulary tripwires that police transitions today.
- No escape transition performs, authorizes, or implies a physical mutation of
  a **marked** owner's repository. Marked owners are left untouched;
  force-abandon never best-efforts a mutation it cannot verify.
- Acceptance, candidate, evidence, publication progress, conflict evidence,
  and preservation evidence rows are immutable through every escape edge,
  exactly as through every other transition.

### 2.4 Physical-mutation ownership (TransitionDesign §6.9 additions)

| Physical mutation | Required persisted owner |
| --- | --- |
| open-record quarantine move (`.gwz/merge/<id>.yaml` → `.gwz/merge/quarantine/<name>.yaml`) | the **already-written quarantine sidecar** — §3.1 writes it atomically before the move, so it is the persisted owner this matrix's frame requires — recording the operator-consented request and the byte digest re-verified immediately before and after the move; **no lifecycle transition, no decode requirement** |
| quarantine restore move (the exact inverse) | operator-consented restore request, sidecar digest equality, empty open slot, and a successful supported-open-record decode |
| archive deletion with forgotten refs ([Q5]) | immutable validated archive plus operator-consented `--forget-refs`; every exactly-matching ref already observed absent after checked deletion; mismatched/unavailable refs observed and enumerated, never mutated |

No other physical-mutation row changes. In particular the escape lane adds no
new participant, publication, preservation, or rollback mutation owner: every
mutation it performs for a tractable owner is executed by the **unchanged**
executors under the **unchanged** owners.

### 2.5 Protocol (append-only)

#### 2.5.1 Request surface

- `MergeOp` (`gwz.taut.py:207-212`) appends `quarantine=5`, `restore=6`.
  Force-abandon is **not** a new op: it is a flag on `abort`, so the frozen
  mutual-exclusion rule of `clirequest/merge.rs:7-15` is unchanged.
- `MergeRequest` (`gwz.taut.py:1599-1610`, fields 1-7) appends **one** field:

```text
MergeRequest { … 8 escape?: MergeEscapeRequest }

MergeEscapeRequest {
  1 force_abandon?: BOOL,                       // abort only
  2 give_up_on: [MergeEscapeOwnerSide],         // abort + force_abandon only
  3 reason?: STR,                               // optional everywhere [Q8]
  4 confirm?: STR,                              // force-abandon second pass
  5 file?: STR,                                 // quarantine multiplicity selector
  6 forget_refs?: BOOL,                         // gc only [Q5]
}
MergeEscapeOwnerSide {
  1 owner: MergeEscapeOwner, 2 member_id?: STR, 3 side?: MergeEscapeSide,
}
MergeEscapeOwner:
  participant=0, publication_root=1, publication_evidence=2,
  selected_root_metadata=3
MergeEscapeSide: preservation=0, rollback=1
```

`side` is **required** for `participant` owners (the only two-sided owner) and
optional for the three single-sided owners, where it must match if present.

#### 2.5.2 Projection surface

```text
MergeRecordProjection {                 // fields 1-5 unchanged
  6 source?: MergeRecordSource,
  7 operator_override?: MergeOperatorOverrideProjection,
  8 quarantine?: MergeQuarantineProjection,
}
MergeRecordSource: open=0, archived=1, quarantined=2

MergeOperatorOverrideProjection {
  1 present: BOOL,
  2 entry_consent?: MergeOperatorConsentProjection,
  3 abort_consent?: MergeOperatorConsentProjection,
  4 dispositions: [MergeOwnerDispositionProjection],
}
MergeOperatorConsentProjection {
  1 operator: STR, 2 recorded_at_ms: STR, 3 writer_version: STR,
  4 record_sha256: STR, 5 reason?: STR,
}
MergeOwnerDispositionProjection {
  1 owner: MergeEscapeOwner, 2 member_id?: STR,
  3 preservation?: MergeSideDispositionProjection,
  4 rollback?: MergeSideDispositionProjection,
}
MergeSideDispositionProjection {
  1 kind: MergeSideDispositionKind,           // unrecoverable=0
  2 consent: MergeOperatorConsentProjection,
  3 collapse_basis?: MergeConsentCollapseBasis,  // repository_missing=0, repository_unreadable=1
  4 evidence_code: GwzErrorCode, 5 evidence_reason: STR,
  6 observed: [MergeObservedFact],
  7 retired_action_kind?: MergeRetiredActionKind, // preservation=0, rollback=1, forward=2
}
MergeObservedFact { 1 kind: MergeObservedFactKind, 2 value: STR }
MergeQuarantineProjection {
  1 file: STR, 2 record_sha256: STR, 3 sidecar_present: BOOL,
  4 consent?: MergeOperatorConsentProjection,
  5 decoded_state?: MergeOperationState,
}
```

`MergeResponse` appends optional field 11 `escape` carrying the structured
destruction manifest of the no-op first pass (§3.3.3), so machine callers see
exactly what the human renderer prints.

#### 2.5.3 Rendering rules (§4-style)

- A record whose `source` is `quarantined` renders `archived=false` and
  `MergeResponse.open=false`; the pinned equivalence
  `archived == (source == archived)` is fixture-enforced.
- An overridden abort is **never** rendered as a plain abort in any renderer
  (human, JSON, JSONL, both drivers): the terminal line carries
  `aborted (operator override)` and the per-owner rows carry
  `abandoned:preservation` / `abandoned:rollback` / `abandoned:both`.
- Open status of a record with dispositions but no `overridden_abort` renders
  `force-abandon in progress` plus the marked-side count.
- Human output never invents absent data; JSON/JSONL emit every projection key
  with `null`/`[]` per the frozen §4 rule.

#### 2.5.4 Error allocation

Appends after `terminal_rollback_mismatch = 61` (verified current maximum;
codes 0..61 contiguous):

| Numeric | Taut/Rust | Message body after `merge record '<id>' ` |
| ---: | --- | --- |
| 62 | `operator_override_invalid` / `OperatorOverrideInvalid` | `operator override is invalid: <reason>` |
| 63 | `escape_consent_required` / `EscapeConsentRequired` | `escape requires explicit consent: <reason>` |
| 64 | `escape_not_applicable` / `EscapeNotApplicable` | `escape is not applicable: <reason>` |
| 65 | `quarantine_state_invalid` / `QuarantineStateInvalid` | `quarantine state is invalid: <reason>` |

All four require record context, carry absent member/detail/target fields, and
take their `<reason>` from the registered lists in
`gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` — the same
discipline as codes 49-61 (§7 item 5 amends the ProtocolContract §1 :46-53
context-legality sentences and the §3 :208-210 range so the frozen text says
the same). Code 63 is the exit code of the no-op first pass (§3.3.3) and its
message carries the confirm token. Code 63 is **also** the refusal for a
wrong, stale, or digest-mismatched `--confirm` token — for force-abandon and
for `--forget-refs` alike — with distinct registered reasons for the three
shapes; code 62 is reserved for decode-time contradictions of a **written**
block and is never returned for a consent-flow refusal.

### 2.6 CLI surface

Corrected against the released parser: **there is no `--destructive` flag.**
The global consent flag is `--force`
(`gwz-cli/src/globalargs/parser.rs:119-125`, help text "Allow destructive
behavior when required. GWZ refuses destructive changes unless this is
explicit."), mapped to `policy.destructive = Allow` at
`gwz-cli/src/clirequest/invocation.rs:80-83`, and merge rejects it today at
`gwz-core/src/workspace_ops/merge/validate.rs:55-56` with "merge does not
support a force/destructive policy". The escape commands are the **only**
merge operations for which that rejection is lifted; it is retained verbatim
for every other merge op.

```text
gwz merge --quarantine [<merge-id>] [--file <name>] [--reason <text>] --force
gwz merge --restore <merge-id> [--file <name>] [--reason <text>] --force
gwz merge --abort --force-abandon
          --give-up-on <owner>[:<side>] [--give-up-on …]
          [--reason <text>] --force [--confirm <token>]
gwz merge --gc <merge-id> --forget-refs [--reason <text>] --force
```

- `--quarantine` with no id resolves the single open record; `--file` exists
  solely for the U7 multiplicity case and names exactly one file per
  invocation.
- Owner tokens: `<member-id>`, `@root`, `@publication-root`,
  `@publication-evidence`, `@selected-root-metadata`. Bare `@publication` is a
  typed refusal listing the three exact tokens (the design's §6.3 sketch used
  `@publication`, which is ambiguous across three distinct wire owners).
- Side tokens: `:preservation`, `:rollback`, `:both`. `:both` is the collapse
  shorthand and is **refused unless the §3.3.4 implication holds**; the
  operator may always name the two sides separately instead. A member owner
  named with no side is a typed refusal.
- `--reason` is optional everywhere ([Q8]).
- Long flags only, no interactive prompts, machine-output parity across
  human/JSON/JSONL and both drivers, per the frozen conventions.

---

## 3. Semantics

### 3.1 Quarantine

**Mechanism.** A checked byte-exact move of one `*.yaml` file from
`.gwz/merge/` into `.gwz/merge/quarantine/`, plus a sidecar. Discovery is
unaffected because `record_files` (`merge/store/mod.rs:269-285`) is a
non-recursive `read_dir` keeping only entries whose `Path::extension()` is
`yaml`; a `quarantine/` subdirectory is invisible exactly as `done/` is.
**Normative naming rule** (a real hazard the tree exposes): the quarantine
directory name must not end in `.yaml`, because a *directory* named `x.yaml`
under `.gwz/merge/` **is** collected by `record_files` and only rejected later
by the regular-file check at `store/mod.rs:230-241`. §8.4 pins this.

**Move discipline.** The same primitive family as the checked v1 archive move
(`v1_lifecycle/store/archive.rs:11-86`): read bytes, digest, write the sidecar
atomically first, re-verify the digest immediately before the move,
`rename_noreplace` (`durable_fs.rs:20-22`), `sync_dir` on **both**
`.gwz/merge` and `.gwz/merge/quarantine`, then re-read and re-verify the digest
at the destination.

**No decode requirement.** Quarantine is byte-level so U7 records (unreadable,
wrong-version, future-envelope) can be parked. When the bytes do decode as a
supported open record, the sidecar additionally records the decoded summary.

**Destination collision.** The destination is `<merge-id>.yaml`. If it exists
with **identical** bytes the operation is an idempotent success; if it exists
with **different** bytes the destination becomes
`<merge-id>.<first 8 hex of sha256>.yaml`; a further collision at that name is
a typed refusal (code 65). No quarantined byte is ever overwritten.

**Preconditions.** Explicit request naming the merge id (matching the open
record's filename, and its `merge_id` field when it decodes); workspace
mutator lock held throughout (`workspace_mutator_lock.rs:14-21`; contention is
a refusal, not a wait); `--force` present; `--file` required when more than
one open `*.yaml` exists. Quarantine is the only tool path that can act under
the multiple-records error (`store/mod.rs:90-96`,
`ErrorCode::MergeRecoveryRequired`, "multiple merge records exist under '{}'").

**Sidecar.** `.gwz/merge/quarantine/<same-stem>.escape.yaml`, with its own
envelope so it can never be mistaken for a record:

```yaml
schema: gwz.merge-quarantine/v1
quarantine_schema_version: 1
record_file: <name>.yaml
record_sha256: <64 hex>
consent: { operator, recorded_at_ms, writer_version, record_sha256, confirm_token, reason? }
decoded: { state, pending_owner?, pending_phase?, participants: […], drift: […] }   # when it decodes
motivating_error: { code, reason }                                                   # when supplied
```

The sidecar lives only under `quarantine/`; moving it into the open directory
is unsupported (it would be a record candidate by the `*.yaml` rule). A
hand-parked record has no sidecar; status reports "quarantined without
sidecar" and restore refuses (§3.2).

**Sidecar naming, lifecycle, and supersession.** The sidecar of `<name>.yaml`
is `<name>.escape.yaml`; a sidecar's **history name** is
`<stem>.<first 8 hex of its record_sha256>.escape.yaml` — the destination-
collision suffix rule above, extended to the sidecar plane. Three rules make
every sidecar state deterministic:

- **Supersession, never adoption.** A plain-name sidecar already present
  whose `record_sha256` does not match the bytes being parked belongs to a
  **different episode**: quarantine renames it byte-identically to its
  history name (content is never rewritten — the immutable-once-written
  discipline is a statement about bytes, which the rename preserves) and then
  writes its own fresh sidecar first, as above. A stale sidecar is never
  adopted as the current attempt's anchor. An identical existing sidecar is
  the idempotent-convergence arm of §4 row 2. A history-name collision with
  different bytes is a typed refusal (code 65).
- **Restore disposes.** Successful restore renames the sidecar to its history
  name in the same operation (§3.2) — it is never deleted and never left at
  the plain name.
- **Orphans are inert.** A sidecar with no record file beside it is an
  **orphan**: historical evidence only. It never makes a stem "quarantined"
  (§7 item 4's predicate is the record file), never blocks a fresh
  quarantine, and status reports it as historical without arbitrating on it.

**Exact effects.** Record bytes moved byte-identically; journal untouched (it
lives inside the record bytes); **no** preserved artifact touched — backup
refs `refs/gwz/merge/<merge-id>/<owner-key>/head`, native stash
`stash_<merge-id>`, bundles, root candidate/marker/boundary files, member
worktrees are all left exactly as they are, and the printed manifest names
them without mutating any; the open-merge gate no longer fires
(`open_merge_gate.rs:87-99`) so all thirteen `Block`ed commands resume.

**The one broken invariant.** "An unfinished coordinated merge blocks
coordinated mutation until recovered." The break is recorded by the sidecar
and by the record's continued existence under `quarantine/`. Record
immutability, evidence retention, and wire validity are **not** broken: a
quarantined record is the same record. The printed manifest states per member,
loudly, that branches may sit at merged-but-never-published commits.

### 3.2 Restore — byte-identical only ([Q4])

Permitted **only** when all of: the open slot contains no `*.yaml` at all
(not merely no valid record); the quarantined bytes still digest-equal the
sidecar's `record_sha256`; the sidecar is present and structurally valid; and
the bytes decode as a **supported open** record. The move is the exact
inverse, with the same digest/fsync discipline. In the same operation,
immediately after the record move, the sidecar is renamed to its history name
(§3.1) — the window between the two renames is §4 row 13, and the orphan it
can momentarily leave is inert and converges idempotently.

- **Edited bytes are permanently manual.** A digest mismatch is a typed
  refusal (code 65) with the manual-surgery pointer. The "checked re-admit of
  edited bytes" alternative is **formally shut**: hand-edited records defeat
  every lineage guarantee the checked store provides.
- **Sidecar-less entries cannot be restored by the tool.** A record parked by
  hand (`GwzMergeWedgeRunbook-v0.md` §6.3) has no digest anchor, so Q4's
  invariant is unverifiable; restore refuses with the runbook pointer and the
  operator may move it back by hand, which is documented manual surgery. This
  is the strict reading of [Q4] and is adopted deliberately, fail-closed.
- **Restore is the repaired-world re-entry.** After restore, ordinary
  `--continue`/`--abort` resume through the unchanged exact machinery. A
  record carrying `operator_override` dispositions may be quarantined and
  restored; it re-enters the escape lane, not the ordinary one.
- **Status third source — a success projection for decodable bytes only.**
  `merge --status <id>` projects a quarantined record from quarantined bytes
  and the sidecar only, never from live repositories — the same discipline
  the archived projection already obeys. **Quarantined bytes that do not
  decode as a supported record have no success projection**: a status success
  is a `MergeResponse` whose field 3 `state` is required
  (`gwz.taut.py:1699`), the projection's field 1 `source_version` is required
  with only `v0`/`v1` allocated, no `unknown` discriminant is appended, and
  "Human output never invents absent data" forbids synthesis. Status of such
  a record is therefore a **typed error** — code 65 with a registered reason;
  the message names the quarantine file and, when the sidecar exists, its
  recorded digest and decoded/motivating summary; `record_context` follows
  the frozen readable-header rule — mirroring the existing unreadable-open
  posture (`MergeRecordUnreadable`, `merge/status/snapshot.rs:176`). The
  decodable-quarantined success populates `MergeResponse.state` from the
  decoded record, `open=false`, `archived=false`, `source=quarantined`, and
  `MergeQuarantineProjection.decoded_state` (§2.5.2). §7 item 4's arbitration
  rows and §8 items 5/10 carry both arms.

### 3.3 Force-abandon

#### 3.3.1 Shape

Force-abandon is the ordinary reverse lifecycle plus three operator-gated
extensions: per-owner **per-side** dispositions (§2.2), an **overridden entry**
edge, and an **overridden terminal** edge (§2.3). It is not a parallel
lifecycle: every tractable owner is processed by the unchanged exact
machinery, and no owner the machinery can still handle may be marked.

#### 3.3.2 Doctrine refinement — what the dispatcher may read

The escape design's rule 1 (§1.6) says the "lifecycle dispatcher, observers,
and reducers remain byte-for-byte ignorant of them." Verified against the
tree, that is unimplementable in its literal form and would defeat the
design's own destruction-minimizing rail: the rollback cursor is derived from
durable facts (ActionJournal §2 :178-181), a marked owner is not durably
"done", and fabricating a terminal row for it is forbidden by rule 2 — so a
literally ignorant dispatcher would wedge on the marked owner and no tractable
owner behind it could ever be restored.

**Refined rule, normative:** cursor and prefix derivation **consume**
disposition rows as retirement facts, exactly as they consume terminal
participant rows and the durable-cursor markers. **No dispatcher, observer,
resolver, or executor path may create a disposition or select an escape
transition**; those are constructible only from an explicit `ForceAbandon`
request carrying digest-bound consent. Concretely:

- A marked side retires that owner's remaining positions on that side for
  every **cursor/prefix** derivation.
- A marked side **never** satisfies an ordinary **exhaustion** proof:
  `VerifiedPreservationExhausted` and `VerifiedRollbackExhausted` remain over
  the complete domain and stay unobtainable, so ordinary `BeginRollback` and
  ordinary `AbortOperation` can never consume a mark.
- A marked side **never** authorizes a physical mutation of its own owner.
  Nothing is executed for a marked side, ever.

Consequence, stated in the durable-cursor amendment's idiom: detection latency
for a wrongly-marked owner moves (its regression is no longer re-detected at
every later position), but the failure direction never does — every remaining
edge is still fail-closed, and the marked owner's own repository is never
touched. The trade is exactly what the operator consented to.

The TransitionDesign §7 :929-933 sentence is narrowed accordingly: the
dispatcher still completes the two-pass cursor "regardless of an incoming
abort request" for every **ordinary** request, and there is still no
**implicit** abandonment — abandonment exists only as an explicit, per-side
consented, durably recorded mark.

#### 3.3.3 Two-step digest-bound consent

Invoked without `--confirm`, the command runs the **complete** preflight,
prints the destruction manifest, **makes no change**, and exits nonzero with
code 63 carrying `confirm token: <first 16 hex of record sha256>`. Re-invoked
with `--confirm <token>`, it **re-runs the full preflight** — the digest gate
freezes the record, not the world: a basis that turned false between manifest
and confirm (a repository that reappeared) makes the named side observable
again, and the fresh classification refuses it with code 64, quoting the
current classification (§3.3.5) — and proceeds only while the record digest
still matches; consent is bound to exactly the bytes whose consequences were
shown, and any concurrent rewrite invalidates it. A wrong, stale, or
digest-mismatched `--confirm` token is a typed refusal, code 63 with a
registered reason (§2.5.4); nothing is written.

**The consent round.** One confirmed invocation is one **consent round**, and
the round has a single anchor: `record_sha256`, the digest of the exact bytes
the manifest was printed from. The round is validated **once** — token
against the current record bytes at invocation entry, under the workspace
mutator lock and the v1 mutation lease, both held for the whole invocation
(§3.3.5) — and its `OperatorConsentV1` is frozen at that validation
(`operator`, `recorded_at_ms` stamped then, `writer_version`, the anchor
digest, the derived token, the verbatim `reason`). **Every** consent row the
invocation writes — each mark's `SideDispositionV1.consent`,
`overridden_entry`, `overridden_abort` — carries that frozen consent
verbatim, as inert provenance (§5.5). This is what makes multi-row
invocations constructible: the second and later writes of a round move the
record bytes past the anchor, and their consent rows still carry the anchor
— never a digest the operator was not shown. "Fresh" and "current round's
digest-bound consent" in §2.3.2 mean exactly this: written by the round whose
token was validated at entry. Consent **never persists across rounds**: a
crash, an exit, or any release of the lock/lease ends the round, and every
re-invocation re-earns a new round against the then-current digest (§4 rows
1, 4, 11). Each `AbandonEvidenceV1.observed_at_ms` remains the per-mark
observation time — evidence is per-fact, consent is per-round.

The manifest is printed **per side**, one row per destruction:

```text
stuck: preservation cannot re-prove owner 'libs-a' (backup ref deleted)
  error: preservation_evidence_mismatch: …exact registered reason…
give up on (2 destructions across 1 owner):
  libs-a : preservation   the preservation artifact for this member will never
                          be created; its half-merged worktree state is left as
                          is; backup ref RETAINED; stash RETAINED
  libs-a : rollback       this member's branch 'main' will be LEFT at 4f2c… and
                          never restored to recorded before-commit 91aa…
keep (processed by the ordinary machinery):
  libs-b : rollback       rolled back exactly to 77de…
  @root  : rollback       evidence commit rolled back; baseline manifest/lock restored
retained evidence: refs/gwz/merge/m-20260815-1/…  stash_m-20260815-1  bundle …yaml
confirm token: 4f2c9ab1d0e37755
```

#### 3.3.4 Per-side consent and the closed collapse basis ([Q10])

Each destruction is consented separately: `--give-up-on libs-a:preservation`
and `--give-up-on libs-a:rollback` are two namings, and both must appear
before either side is marked.

**Collapse is permitted only when one side's failure provably implies the
other.** The basis set is **closed and exactly two**:

| Basis | Why the implication is provable |
| --- | --- |
| `repository_missing` | Both sides' observations are functions of the same repository handle. The fresh preflight classifies the member repository absent; every preservation observation (artifact pass, reset pass) and every rollback observation (`AbortConflict`, `ResetIntegrated`) for that owner is then permanently unobservable from the same single fact |
| `repository_unreadable` | Identical argument for an operational read failure at the repository level (`.git` corruption, permissions): the preflight's read failure is an error, never `false`, on both sides |

Anything narrower — a dropped stash, a pruned commit, a retargeted backup ref,
a rewritten integration ref, a mangled bundle — implies **at most one** side
and must be consented per side. There is no "operator judgement" arm.

**The collapse is a prompt affordance only; the record never collapses.** A
collapsed consent writes, in **one atomic rewrite** (§2.3.2), **two**
`SideDispositionV1` rows, each carrying the same `OperatorConsentV1` (the
round's frozen consent, §3.3.3) and a `ConsentCollapseV1` whose `basis` is
the proven implication and whose `implied_side` is **that row's own side** —
`collapse` presence records that this side's consent arrived through the
`:both` collapse rather than a separate naming (§2.2.4 rule 7). The consent
text of a collapsed prompt must **name both destructions explicitly** — the
manifest prints the two rows above in full, and the `:both` token is refused
when the basis does not hold, with a typed error naming the two side tokens
the operator must use instead.

#### 3.3.5 Preconditions

- The record decodes as a supported open **v1** record (v0 excluded, §5.1).
- A fresh full preflight classifies **at least one** owner side as permanently
  unobservable *and* the operator names every such side explicitly. Naming an
  observable side is a typed refusal (code 64) quoting its current exact
  classification; omitting an unobservable side is a typed refusal listing it.
  If nothing is unobservable, the command redirects to plain `--abort` — the
  escape is never the convenient path.
- `--force` present; digest-bound `--confirm` present on the second pass.
- Preservation-first ordering holds: from `Preserving`, tractable preservation
  owners are still driven to completion before any rollback entry.
- The workspace mutator lock and the v1 mutation lease are held throughout;
  the escape runs inside the ordinary service loop.

#### 3.3.6 Half-marked owners — record shape, validity, and rendering

A **half-marked owner** is an `OwnerDispositionV1` with exactly one side
present. It is a first-class, legal, and expected shape.

```yaml
operator_override:
  dispositions:
    - owner: { kind: participant, member_id: libs-a }
      preservation:
        kind: unrecoverable
        consent: { operator: alice, recorded_at_ms: "1755300000000",
                   writer_version: 0.11.0, record_sha256: 4f2c…, confirm_token: 4f2c9ab1d0e37755 }
        evidence:
          code: preservation_evidence_mismatch
          reason: preservation_backup_ref_absent
          observed_at_ms: "1755300000000"
          observed:
            - { kind: expected_ref, value: "refs/gwz/merge/m-1/libs-a/head" }
            - { kind: ref_state,    value: absent }
        retired_preservation_action: { kind: backup_ref, owner: …, name: …, target_commit: … }
      # rollback side deliberately absent: still pending, or completed exactly
```

**Applicability** decides whether an absent side is legal at the terminal
edge. It has two forms, each decidable in its own plane — the write edge
consumes live proofs it earns freshly; the decoder consumes durable facts
alone:

- The **rollback** side is applicable, in both forms, to every owner the
  rollback cursor visits: every selected participant, `PublicationEvidence`
  when publication evidence exists, `SelectedRootMetadata` for a
  selected-root operation.
- The **preservation** side, **write-edge form** (consumed by the §2.3.2
  bound proofs): applicable when the record entered preservation — state is
  or was `Preserving` (including `RecoveryRequired` with
  `origin_state: preserving`), or a preservation evidence row exists. The
  terminal edge's `VerifiedOverriddenRollbackExhaustion` discharges this form
  in full, per owner, by a durable completion fact, by a mark, **or by a
  fresh exact live re-proof** where neither durable witness exists — a
  position retired by live proof alone leaves none (D3's acknowledged
  crash-window shape, and every pre-D3-implementation record). Rule 8 can
  therefore never deadlock on a preservation plan that was actually
  complete: what cannot be shown durably is re-proven live by the consuming
  edge, while marking an observable side stays a typed refusal (§3.3.5) —
  no consent obligation is waived.
- The **preservation** side, **decode form** (consumed by §2.2.4 rule 8):
  applicable **iff** that owner carries a preservation evidence row or a
  preservation-side disposition — per-owner durable facts alone, with no
  "was `Preserving`" reconstruction, so write-legality and read-legality of
  an overridden record can never diverge and an archived read re-derives no
  live fact. The decode form trusts the recorded edge for exactly what the
  write-edge form re-proved live.
- A plain abort with no `--preserve` has **no** applicable preservation side
  anywhere, in either form, so no preservation mark is ever required or
  legal.

**Validity by phase:**

| Phase | Half-marked owner |
| --- | --- |
| open, any state | **legal**, unconditionally (subject to §2.2.4 owner/side legality) |
| `BeginRollbackOverridden` | legal; the edge consumes only preservation-side marks. A rollback-side-only owner is untouched by this edge |
| ordinary rollback work in `RollingBack` | legal; the unmarked side is processed by the unchanged machinery and may reach an ordinary exact terminal row |
| `AbortOperationOverridden` | legal **iff** the bound terminal proof discharges every applicable side in the write-edge form — per owner and side: durable ordinary evidence, a mark, or the fresh exact live re-proof of §3.3.6 (the decode form of §2.2.4 rule 8 then holds by construction). A participant marked on the preservation side whose rollback ran exactly carries a real `RolledBack`/`Aborted` terminal row **and** a preservation-side disposition — the common, healthiest overridden shape |
| archived / after GC | legal and immutable; never retired |

**Rendering.** Per-owner status is exactly one of `restored` (ordinary),
`pending`, `abandoned:preservation`, `abandoned:rollback`, `abandoned:both`.
A half-marked owner while open renders `preservation abandoned; rollback
pending`; after an exact rollback it renders `preservation abandoned; rolled
back exactly to <commit>`. The record-level line renders `aborted (operator
override)` and never plain `aborted`; a half-marked owner never renders as a
clean one.

#### 3.3.7 Journal and forward-action retirement

Every journal a mark retires is copied **verbatim** into the side row and
cleared in the **same atomic rewrite** — never silently dropped, never left
dangling. This adds one legal retirement path to two frozen sentences:

- ActionJournal §2's "Retirement occurs only with the verified result/progress
  write" gains "…or with the verified operator-override mark that copies the
  action verbatim into its disposition row in the same atomic rewrite."
- RecordContract §7's "A durable forward action may also retire without an
  integration outcome **only when** an abort or preserve-abort request
  consumes the matching bound exact `NotStarted` observation" gains the second
  path. The escape retirement is distinct and never impersonates the first:
  it consumes **no** `NotStarted` observation, it reports no success, conflict,
  or failure, it leaves the participant's state/evidence unchanged, and it is
  legible as an override by one field-presence check. A forward action is
  retirable only under the owner's **rollback-side** mark; an owner with a
  retained forward action and only a preservation-side mark is a typed refusal
  naming the missing rollback-side consent.

#### 3.3.8 Exact effects

Marked owners' repositories are **left untouched**. Nothing is deleted: backup
refs, native stashes, and bundles of marked owners stay exactly where they
are, and — a **correction to the design's §4.3** — the override block records
**no artifact names**. Retention is governed solely by the existing
preservation evidence rows and the archived cleanup worklist derived from them
(`record_wire/archive/cleanup.rs:149-197`), so GC's exact-target rule keeps
protecting them with **no second source of truth** to diverge. The archived
record archives through the unchanged exact-byte primitive;
`ArchivedTerminalOutcome` stays `Aborted` and the override rides inside the
archived bytes.

### 3.4 The GC forget-refs valve ([Q5])

`gwz merge --gc <id> --forget-refs --force`, on an archive whose worklist
meets a mismatched or unavailable ref:

1. Acquire the mutator lock and prove no open merge exists (unchanged,
   `merge/gc.rs:169-178`).
2. Acquire and validate the archive through the unchanged canonical no-follow
   reader; derive the immutable worklist (unchanged).
3. Preflight every ref. **Delete** every exact-target ref through the
   unchanged `delete_backup_ref_checked`. **Forget** — observe, enumerate,
   and leave exactly as observed — every ref classified *different target* or
   *unavailable repository*. Absent refs are already complete.
4. Re-read and re-validate the archive byte-identically, prove every
   *deleted-set* ref absent, then remove the terminal file and fsync.

Rails: no repository is mutated for a forgotten ref; native stashes and
bundles are never deleted (unchanged); without `--forget-refs` the frozen
"stop and retain the archive" behavior is **unchanged in every particular**.
The forgotten refs are namespaced under `refs/gwz/merge/<id>/` and inert.

**Audit honesty, stated rather than absorbed:** deleting the archive destroys
the only durable record of what was forgotten. The valve therefore requires
the same two-step consent as force-abandon — the first pass prints every ref
to be forgotten with its exact name, expected target, and observed
target/state, and makes no change. **The consent anchor is the validated
archive bytes**: the confirm token is the first 16 lowercase hex characters
of the archive's SHA-256 as read through the unchanged canonical no-follow
reader, and a token that no longer matches the current archive digest is a
typed refusal — code 63, registered stale-token reason — exactly as in
§3.3.3. Because this is the one consent that lands in no durable artifact
(the artifact it would ride is being deleted), the JSONL event stream emits
one event per forgotten ref **carrying the ref's exact name, expected target,
observed target/state, the archive digest, and the consumed confirm token**
— the driver log is the audit trail. After the archive is gone, the refs are
recoverable only by hand from the printed manifest or the driver log.

### 3.5 Error-text and status pointers ([Q6])

Every stuck-state typed error and every `--status` render of a stuck record
gains one pointer line naming the user-docs runbook page and the applicable
escape. The pointer is a rendering addition only: it changes no code, no
reason string, and no classification.

---

## 4. Restart and crash legality

Per new edge, exhaustively. "Ordinary machinery" always means the unchanged
dispatcher, observers, and executors.

| # | Crash point | Legality on restart |
| --- | --- | --- |
| 1 | after the manifest is printed, before any write | nothing is written. Re-run re-prints and re-earns consent; the digest is unchanged so the token is the same, but consent still does not persist |
| 2 | between the sidecar write and the quarantine move | the sidecar exists with a matching digest; re-run detects it and completes the move idempotently |
| 3 | after the quarantine move, before the response | no open record; a quarantine entry with matching digest exists; re-run reports idempotent success and echoes the sidecar |
| 4 | after a `MarkOwnerUnrecoverable` write | the record is open with a partially populated block and the marked owner's journal already cleared. **The ordinary dispatcher's outcome is unchanged in kind**: ordinary `--continue`/`--abort` still refuse to terminalize (the ordinary exhaustion proofs are **not constructible** while any disposition exists — definitionally, §2.3.4; the complete-domain wording alone would not suffice), while cursor derivation skips the marked side so ordinary rollback work on *unmarked* owners proceeds. A re-issued force-abandon resumes: already-marked sides stay marked, consent is re-earned against the new digest |
| 5 | between two `MarkOwnerUnrecoverable` writes | identical to 4; marks are independent, append-only, and order-insensitive within an owner. A collapsed `:both` pair is **one** write (§2.3.2), so no crash can split it; the next round's marks carry the new round's consent while already-written rows keep theirs |
| 6 | after `BeginRollbackOverridden`, before the first ordinary rollback action | state is `RollingBack` with `overridden_entry` present. Ordinary `--abort` resumes the tractable owners' rollback with no consent (nothing new is destroyed) and then refuses at the terminal edge; force-abandon resumes and re-earns terminal consent |
| 7 | between two ordinary rollback actions inside an overridden run | ordinary restart legality, entirely unchanged — the marked sides are durable and the unmarked owners are recognized by the unchanged exact observers |
| 8 | after `AbortOperationOverridden`, before archive | terminal `Aborted` with `overridden_abort`. The unchanged terminal/archive path applies; `store/archive.rs:15-24` still requires the lease and a terminal state |
| 9 | mid-archive of an overridden record | the unchanged four-way archive state machine applies (`archive.rs:11-86`): source-only, both-copies-identical, destination-only, or a typed rejection that deletes neither copy |
| 10 | during a restore move | either the quarantine copy or the open copy exists with a verified digest; a partial state is impossible (`rename_noreplace` + both-directory fsync). If both exist with identical bytes, the open copy is authoritative and the quarantine copy is retained (status renders it shadowed/historical, §7 item 4); different bytes is a typed refusal (manual surgery, runbook step 4) |
| 11 | during `--forget-refs` GC, after some deletions | restart treats absent refs as complete (unchanged), re-derives the same worklist from the unchanged archive, and re-earns consent before proceeding |
| 12 | between a mark write and the `--force-abandon` response | indistinguishable from 4 for every reader; the durable block is the only state |
| 13 | between the restore's record move and the sidecar's history-rename | the record is open and digest-verified; the plain-name sidecar is an orphan (no record file beside it) and is inert (§3.1). A re-run restore is an idempotent success that completes the history-rename; a later quarantine supersedes it per §3.1; status reports it as historical; nothing arbitrates on it |

**Invariant across all thirteen:** a crash never leaves the record in a shape the
ordinary machinery misreads as clean, and never leaves a marked owner's
repository mutated. Every partially-force-abandoned record is completable only
by force-abandon with fresh consent, or by quarantine, or by repairing the
world and… nothing else — an override is never un-marked (there is no
unmark edge in this amendment; see §10.3 R-3).

---

## 5. Compatibility

Stated per the F5 lesson of the exact-evidence amendment: **this amendment
changes the coverage or meaning of no existing hash, field, frame, or
projection.**

### 5.1 v0 reach

- **v0 gets quarantine, restore, `--forget-refs`, and the runbook. It never
  gets force-abandon.** The v0 wire is a released frozen format; adding a
  marker field or an enum variant is a same-version newly-writable shape that
  old readers would have to prove behaviorally equivalent (Refactor §15.1),
  and an unknown enum variant is a decode break, not equivalence.
  Terminalizing a v0 record as plain `aborted` without a marker would
  fabricate a clean abort. Force-abandon on a v0 record is a typed refusal
  (code 64) with the quarantine hint.
- **v0 never writes any of this.** No v0 code path constructs
  `operator_override`; absent-by-default encoding keeps every v0-written byte
  stream identical to today's. Quarantine, restore, and `--forget-refs` need
  no v0 wire change at all: they never rewrite a record.

### 5.2 Collision and migration ineligibility

Presence of a v0 unknown top-level field named `operator_override` is the same
class of collision as the five existing names: it makes migration ineligible
and the value is never adopted, overwritten, or moved. It is detected at the
raw YAML path during v0 unknown-field extraction; the **v0 top-level known-key
set does not adopt the name**, so it surfaces in the v0 unknown manifest and
that membership is the trigger, consumed by `map_v0_to_v1`
(`record_wire/unknown_fields/mod.rs`, today the five names). The **v1**
known-key set adopts it, without which the first override write would fail the
overlay's unauthorized-unknown-field check — fail-closed and self-catching, the
same mechanism the durable-cursor amendment documented for its two row names.

### 5.3 Unknown-field survival and retirement

- The block is a **new container** in the RecordContract §8 retirement table
  with the rule: *append-only; each owner/side row and each consent slot is
  immutable once written; survives archival; never retired.*
- Sequence identity for unknown descendants inside the block is
  **`(owner, side)`** for disposition sides and `owner` for the row itself —
  added to the §8 identity list beside conflict evidence `(path, sha256)`,
  preservation evidence by stable owner, candidate hash `path`, participant
  drift, and operation drift. Reordering may not rebind an extension to a
  different `(owner, side)`.
- Unknown descendants beside the new fields survive rewrites bound to those
  identities, exactly as elsewhere; the block survives archival with the
  record.
- A post-amendment record read by a pre-amendment v1 decoder would surface
  `operator_override` as an unknown top-level field — a non-case in practice
  because no v1 writer is released, but the unknown-field suites still pin the
  survival behavior (§8.5).

### 5.4 Terminal plane, archive, and GC

- **`ArchivedTerminalOutcome` and `MergeTerminalOutcome` are untouched.** An
  overridden abort is `Aborted` with an override presence marker in the
  projection; no third outcome value exists. This is deliberate: a third
  variant would break the frozen protocol enum `completed=0, aborted=1`.
- **The cleanup worklist derivation is untouched.** The override block
  contributes no worklist entries and names no artifacts (§3.3.8); the
  worklist is derived only from preservation evidence rows, whose marker-aware
  arm the durable-cursor amendment **requires** at
  `record_wire/archive/cleanup.rs:149-197` — specified by D3, **not yet
  landed**: `collect_owner` today still rejects a row with neither backup
  pair nor stash pair. The E packages must not assume D3's implementation has
  landed (§6.4's any-order rule).
- **GC never sees the block.** `merge/store/gc.rs:8` reads only `done/`;
  `quarantine/` is invisible by the same subdirectory rule as `done/`, which
  this amendment converts from an accident into a tested rule (§8.4). A `--gc`
  of a quarantined id is a typed refusal: quarantined records are not
  archives.
- **Correction to a landed statement.** `GwzM5-8DurableCursorAmendment.md`
  §2.2/§5 describe a "post-GC record rewrite" at `gc.rs:365-381`. Verified:
  `post_gc_record` is called only at `merge/gc.rs:196`, inside
  `post_gc_record(record).to_response(context)?` — it shapes the **response
  projection only**; nothing is written back, and the archived record is
  deleted outright at `:205`. No behavior of that amendment depends on the
  misdescription (its marker-fate rule is about rows that are being deleted
  with the archive anyway), but this amendment must not repeat it: **there is
  no post-GC durable rewrite of any record, so the override block has no
  post-GC fate to specify.** Routed as §10.1 item 7 for the durable-cursor
  amendment's next docs pass.

### 5.5 Hashes, frames, and digests

- **No hash frame changes.** The `gwz.merge-preservation-preimage/v1` grammar,
  frame id, and coverage are untouched — nothing in this amendment enters hash
  input. No new incomparability window is created; the exact-evidence
  amendment's discontinuity remains the only one.
- **Bundle identity is untouched.** Canonical bundle derivation reads
  preservation evidence rows with stash ids only; the override block never
  enters bundle bytes. §8.7 pins byte-identical expected-bundle output with
  and without the block.
- **No record self-hash is introduced.** RecordContract §1 :76-77's "There is
  no … record self-hash" stands. `consent.record_sha256` is the digest of the
  **round-anchor** bytes the manifest was printed from, not of the containing
  record. It is validated **once per consent round**, at the round's entry,
  against the then-current record bytes (§3.3.3); every consent row of that
  round carries it verbatim thereafter as inert provenance — including rows
  whose own rewrite base has moved past the anchor — and no reader ever
  recomputes it against the containing bytes. It is not an integrity frame
  and never becomes a cross-version identity.

### 5.6 Graceful degradation

- A record with dispositions but no terminal consent is a legal open record for
  every reader, including status, archive-refusal, and the gate. Ordinary
  commands behave as in §4 row 4.
- A quarantined record is invisible to discovery and to the gate by
  construction, so an older binary that predates quarantine simply sees no open
  merge — the exact behavior the v0 runbook already documents for a
  hand-parked record.
- A binary that predates `--forget-refs` retains the archive, unchanged.

---

## 6. Interaction

### 6.1 Non-collision with `GwzM5-8DurableCursorAmendment.md` (D3), both directions

**Surfaces are disjoint.** D3 adds two optional fields *inside* the per-owner
preservation evidence row (`noop_commit`, `reset_commit`) plus two
evidence-write reducer arms and an extended reset-retirement footprint. This
amendment adds a *top-level* field, a *new transition sub-enum*, and protocol
appends. No field, row, transition variant, reducer arm, footprint, hash,
projection field, or error code is shared.

**Direction A — D3 landed first (the actual order).** Every sentence D3
amended stays true: this amendment adds no evidence-row field, changes no
marker equation, and adds no journal variant. Where both amend the same
document they amend different sections — D3 took RecordContract §8's
preservation-evidence retirement row and the in-row collision doctrine; this
takes §1's field count, §7's forward-retirement sentence, §8's *top-level*
collision list, and a *new* retirement row. Banner stacking: this amendment's
banner is added beside D3's without editing D3's text, following the
Amendment-1/-2 precedent.

**Direction B — had this landed first.** D3's row markers are keyed by owner
inside the evidence container; disposition rows are keyed by `(owner, side)`
inside a different top-level container. D3's legality table, backfill rule,
and known-key fork would be unaffected in every particular.

**The one real semantic adjacency, stated precisely.** After D3, a
preservation cursor position is retired by a durable completion fact, else by
live proof. This amendment adds a **third** retirement fact for a position — an
operator-consented preservation-side mark — with a strict precedence and a
strict limit: durable fact → live proof → mark; and a mark retires a position
for **cursor/prefix** purposes only. It never satisfies
`VerifiedPreservationExhausted` (only the escape edge's
`VerifiedPreservationExhaustedOrMarked` consumes it) and it never authorizes a
physical mutation — the exact shape of D3's own §3.3 INVARIANT. D3's
`reject_later_durable_owner` guard is unaffected: a marked owner is durably
retired, so the guard's live arm is not reached for it.

### 6.2 Non-collision with `GwzM5-8M5bNoFfDesign.md` (M5b)

M5b is semantics-installation-only under a ratified zero-production-line
ceiling, with a declared zero wire delta and zero protocol delta. This
amendment is **mode-blind** (no override content, legality rule, or projection
varies with `mode: no_ff`) and **anchor-preserving** (owner anchors — the
participant result, the composition commit — are untouched), so no M5b §3.3
or §5.3 statement becomes false when it lands. Conversely, nothing M5b freezes
asserts the absence of a sixth top-level field or of an escape transition
sub-enum.

The one adjacency: M5b §5.3 and RecordContract §7 own the frozen
forward-action abandonment authority, and §3.3.7 above adds a second legal
retirement path to that sentence. It does not touch the *first* path in any
particular — the `NotStarted`-consuming abandonment keeps its exact proof, its
exact footprint, and its exact prohibition on reporting an outcome — and the
override path is distinguishable from it by one field-presence check. M5b's
suites must tolerate this wire landing first, per the same §7 P-REV
order-independence instruction M5b already binds itself to — an instruction
stated there for the D3 wire and **extended here** to this train by this
sentence.

**Composition with M5b-W1** (the 2026-08-22 RecordContract banner, applied at
M5b acceptance): W1 freezes signature **timestamps** into the forward
action's identity fields and makes restart adoption and completion matching
**field-wise** over the frozen fields (parents, message bytes, tree, both
identity lines) — an externally created twin commit matching every frozen
field may be adopted. The escape lane composes disjointly, on three facts:

1. **Disjoint worlds.** Adoption is an observable-world path; a mark requires
   permanent unobservability. The same fresh preflight that could observe a
   W1-adoptable twin makes `MarkOwnerUnrecoverable` for that owner/side a
   typed refusal (§3.3.5) — no owner is simultaneously eligible for both.
2. **No adoption subject after retirement.** A rollback-side mark that
   retires a forward action clears the journal in the same rewrite (§3.3.7);
   W1's field-wise matching runs against a *pending* action's frozen
   `PendingGitSignature`, and a marked owner has none. The disposition row's
   verbatim copy is evidence outside every derivation — no observer,
   adoption, or reconciliation path reads it (§2.3.4).
3. **Different equality planes.** §2.2.4 rule 5's retired-copy check is
   byte-equality of journal YAML at mark time; W1's is field-wise equality of
   a live commit against frozen signature fields. Neither rule reads the
   other's inputs, and neither weakens the other.

### 6.3 Exact-evidence amendment doctrine compliance

The escape lane introduces **no** checkout, capture, preimage, or cleanliness
predicate of its own. Every mutation it performs for a tractable owner runs
through the unchanged recovery-grade edges of Clause A (blob-exact,
filters-disabled) and the private-area-blind predicates of Clauses B/B′.
Consequently:

- **Fail-closed rails are respected, not bypassed.** Where a foreign
  non-idempotent clean filter wedges a checked mutation fail-closed, or where
  an ordinary Windows-CRLF worktree classifies `Ambiguous` before any
  mutation, the escape does **not** convert the refusal into an override. That
  wedge is a legitimate U-class producer whose remedy still requires an
  explicit, per-side, digest-bound consent naming the destruction.
- **No automatic selection.** No fail-closed refusal anywhere may return an
  escape as its next action (doctrine rule 1, unrefined in this respect).
- **No new incomparability.** The escape records and compares no preimage
  hash, so it widens no comparison window.
- **Escapes do not close D1/D2.** Shipping the ladder does not discharge the
  real-Windows satisfiability residual or the foreign-filter policy: those
  keep their own tripwires. The relationship is one-way — the smaller those
  residuals, the less load the ladder carries
  (`GwzM5-8A1DecisionPacket.md` :209-211).

### 6.4 Composition authority — the three pre-A1 trains (read this one paragraph)

Three amendments now bear on the same I2 contracts before A1:
`GwzM5-8ExactEvidencePlatformAmendment.md` (accepted 2026-08-16; semantics of
recovery-grade edges and private-area blindness; **no wire delta**),
`GwzM5-8DurableCursorAmendment.md` (accepted 2026-08-16; **two evidence-row
fields**, no top-level field, no protocol delta), and this one (**one
top-level field, one transition sub-enum, protocol appends**). Following the
idiom `GwzM5-8M5bNoFfDesign.md` §8.2 established, **this amendment's
composition position binds on the property — consent-gated,
evidence-additive, and ordinary-vocabulary-preserving — not on its internal
edge shapes.** It adds only operator-consented, append-only evidence and a
disjoint transition sub-enum; it changes no existing field, no existing
transition row, no proof obligation, no hash frame, and no existing projection
value. Text rounds within that property (for example the §2.3.3 entry-edge
shape, or the exact spelling of a disposition field) do **not** re-open M5b-IF
or D3, and do not require either to be re-reviewed. **The three trains may
land in any order**: their surfaces are pairwise disjoint, each is complete
and reviewable without the others, and each one's suites must tolerate the
other two's wire landing first. This paragraph is the single authority
statement A1 reviewers should read for how the three compose.

Two 2026-08-22 landings do not disturb this paragraph. The accepted
`GwzM5-8ThinA1Amendment.md` re-scopes the A1 **gate chain** only — this train
is second-lane and non-gating for A1 (its :59-60) — and reopens no I2 wire,
so it composes with all three trains by construction. The M5b-W1
RecordContract banner is document-only and composes per §6.2. Because M5b
§8.2's frozen sentence names two trains and cannot be edited, **at acceptance
`CurrentProgramCheckpoint.md` gains a one-line pointer naming this §6.4 as
the three-train composition authority** — the checkpoint is the live router
(rulebook §7.3), so an A1 reviewer routed by M5b's sentence still finds the
third train.

---

## 7. Contract deltas applied at acceptance

On acceptance the amended documents receive banners dated with the acceptance
date, plus these exact text changes. This document is controlling wherever the
frozen texts and this one disagree.

1. **`GwzM5-8I2RecordContract.md`** — banner: "Amended (acceptance date) by
   `GwzM5-8OperatorEscapeAmendment.md`: the record gains a sixth top-level
   field `operator_override` carrying append-only, per-side operator
   dispositions and two one-shot consent slots; the §7 forward-action
   retirement rule gains the override path; §8's top-level collision list and
   retirement table extend to the new name and container."
   - §1 :60 — "It adds five top-level fields:" becomes "It adds six top-level
     fields:", and the struct block gains
     `operator_override: Option<OperatorOverrideV1>,` after
     `preservation_publication_handoff`.
   - §1 :76-77 — "There is no sibling acceptance file, sidecar journal,
     cleanup journal, or record self-hash." gains the scoping annotation
     "(scoped to the record's own lifecycle storage under `.gwz/merge/` and
     `done/`)": the quarantine sidecar (§3.1) is store-plane escape evidence
     under `quarantine/`, consumed by no lifecycle path, and is not this
     sentence's "sidecar journal".
   - §7 :381 — "A durable forward action may also retire without an
     integration outcome only when an abort or preserve-abort request consumes
     the matching bound exact `NotStarted` observation." becomes: "A durable
     forward action may also retire without an integration outcome in exactly
     two cases: when an abort or preserve-abort request consumes the matching
     bound exact `NotStarted` observation; or when an operator-override
     rollback-side mark copies the action verbatim into its disposition row in
     the same atomic rewrite. Neither case reports success, conflict, or
     failure for an action that did not run, and neither changes the
     participant's state or evidence."
   - §8 :394-400 — the collision sentence's name list gains
     `operator_override` as a sixth name.
   - §8 retirement table gains: "| operator override | append-only; each
     owner/side disposition and each consent slot is immutable once written;
     survives archival; never retired |".
   - §8 sequence-identity list gains: "operator-override dispositions by
     `(owner, side)`".
   - §9 :440 gains this amendment's exit-test rows (§8 below).
2. **`GwzM5-8I2ActionJournalContract.md`** — banner of the same form.
   - §1 gains the escape wire types of §2.2 verbatim, annotated as the escape
     lane's closed vocabulary.
   - §2 :175-176 — "Retirement occurs only with the verified result/progress
     write." becomes "Retirement occurs only with the verified
     result/progress write, or with the verified operator-override mark that
     copies the action verbatim into its disposition row in the same atomic
     rewrite; no other path clears or advances a pending action."
   - §2 :178-181 — "For rollback, owner/kind agreement includes the exact
     deterministic current cursor derived from durable participant terminal
     states, publication evidence, selected-root membership, and pending
     phase; a wrong or later owner is invalid at decode." becomes "For
     rollback, owner/kind agreement includes the exact deterministic current
     cursor derived from durable participant terminal states,
     **operator-override rollback-side dispositions**, publication evidence,
     selected-root membership, and pending phase; a wrong or later owner is
     invalid at decode. A disposition retires its owner's remaining rollback
     positions for cursor purposes only; it never satisfies an exhaustion
     proof — `VerifiedRollbackExhausted` remains complete-domain and is not
     constructible while any disposition exists."
   - §2 :185-192 — the two-arm prefix sentence gains the third arm: "…must
     also prove — by durable completion facts where present, **by an
     operator-override preservation-side disposition on that owner where one
     exists**, and by exact bound live observation where absent — that every
     earlier position in the two-pass cursor is complete or currently
     unnecessary." with the appended carve-out sentence: "A disposition
     retires a position for cursor/prefix purposes only; it never satisfies
     `VerifiedPreservationExhausted`, and only the §6.10 escape edges'
     `…ExhaustedOrMarked` proofs consume it."
   - §2's legality table and the "Ambiguous observation changes only operation
     state/context" sentence are **unchanged** (annotated as such).
   - §5 gains the [Q5] paragraph: "Targeted GC additionally accepts an
     operator-consented `--forget-refs` release. It deletes every
     exact-target ref through the unchanged checked deletion, leaves every
     ref classified different-target or unavailable exactly as observed
     (no repository mutation), and may then delete the archive after proving
     every deleted-set ref absent. Without that explicit consent the
     stop-and-retain rule above is unchanged."
3. **`GwzM5-8R4bTransitionDesign.md`** — banner of the same form.
   - §6's `V1Transition` block gains the `Escape(EscapeTransition)` arm and
     the `EscapeTransition` definition of §2.3.1.
   - A new §6.10 "Escape transitions" carries the §2.3.2 predecessor/footprint
     table verbatim, prefixed by: "Escape transitions are constructible only
     from an explicit `ForceAbandon` request carrying digest-bound operator
     consent of the current consent round. No dispatcher, observation,
     resolver, or executor path may construct one." The block also carries
     the **checked-record legality delta**: "Under `state: Aborted` with
     `overridden_abort` present, a participant is legal in a
     non-rollback-terminal state **iff** its rollback side carries an
     `operator_override` disposition; with `overridden_abort` absent, the
     existing all-rollback-terminal rule
     (`model/v1/validate/lifecycle.rs:56`, error `TerminalRollbackMismatch`)
     is unchanged. The delta applies identically in the open decoder and the
     archived decoder — write-legality and read-legality never diverge."
   - §6.1's `BeginRollback` and `AbortOperation` rows are **unchanged**, with
     an added note: "A bound `VerifiedRollbackExhausted`, a bound
     `VerifiedPreservationExhausted`, and a `PreparedRollbackEntry` for the
     `Preserving` origin are **not constructible** while any
     `operator_override` disposition exists — definitionally, independent of
     what the live world shows; only §6.10's edges may terminalize, or enter
     rollback from `Preserving`, on such a record. Ordinary `BeginRollback`
     from the other origins remains obtainable in a healed world and is safe:
     cursor derivation still skips marked sides and the terminal edge still
     refuses."
   - §6.6 :648-652 — "The reverse cursor is fully derivable from participant
     terminal states, publication rollback evidence, selected-root
     membership, and the exact pending phase." becomes "The reverse cursor is
     fully derivable from participant terminal states, **operator-override
     rollback-side dispositions**, publication rollback evidence,
     selected-root membership, and the exact pending phase." The decode-law
     sentence that follows applies to the amended derivation unchanged.
   - §6.9 gains the three rows of §2.4.
   - §7 :843 — "resume-start, continue, abort, preserve, status, and archive
     work" becomes "resume-start, continue, abort, preserve, force-abandon,
     status, and archive work".
   - §7 :928-930 — "regardless of an incoming abort request" becomes
     "regardless of an incoming ordinary abort request", and the paragraph
     gains: "An explicit force-abandon request with digest-bound per-side
     consent may retire a cursor position through a durable
     `MarkOwnerUnrecoverable` write; there is still no implicit abandonment of
     a partially completed preservation plan."
   - §6.7's cursor paragraph gains: "A durable operator-override mark retires
     its owner's remaining positions on the marked side for cursor-prefix
     purposes only; it never satisfies `VerifiedPreservationExhausted` and
     never authorizes a physical mutation."
   - §13 gains the escape matrix rows (§8 item 8).
4. **`GwzM5-8R4bReverseLifecycleInterface.md`** — banner of the same form.
   - §9 :441 — "Status has two sources and no mutation path:" becomes "Status
     has three sources and no mutation path:", with a third bullet: "a
     quarantined record is decoded solely from quarantined bytes and its
     sidecar, and projected without consulting live repositories." The source
     arbitration table gains the quarantined rows, **total over the third
     axis** — "quarantine present" means a record file exists under
     `quarantine/`; an orphan sidecar is historical and never arbitrates
     (§3.1):
     - quarantine present + open absent + archive absent → quarantined
       projection when the bytes decode as a supported record; the §3.2
       typed error (code 65) when they do not;
     - quarantine present + open present, **identical bytes** → open
       projection; the quarantine copy renders shadowed/historical; status
       does not reconcile (the frozen identical-terminal-bytes posture);
     - quarantine present + open present, **different bytes** → typed
       contradiction; no mutation;
     - quarantine present + **archive present** (open present or absent) →
       typed contradiction naming every present path; no mutation (the
       frozen different-destination fail-closed posture);
     - quarantine present without sidecar → quarantined projection marked
       `sidecar_present: false` when the bytes decode (the same typed error
       when they do not); restore refused.
   - §11 gains the [Q5] paragraph of item 2 above.
5. **`GwzM5-8I2ProtocolContract.md`** — banner of the same form.
   - §1's error table appends codes 62-65 with the message bodies of §2.5.4
     and the registered-reason requirement.
   - §1 :46-53 — the context-legality sentences extend by exact edit, so the
     appended codes are inside the require-context set rather than under
     "every other error forbids context": "codes 49–61 require id and
     installed pair" becomes "codes 49–65 require id and installed pair", and
     "Codes 46–61 have absent member/detail/target fields." becomes
     "Codes 46–65 have absent member/detail/target fields." The sentence
     "Every other error forbids context." is unchanged and stays true.
   - §3 :208-210 — the record-context range sentence: "the code is one of
     45–61" becomes "the code is one of 45–65".
   - §2's enum allocation appends `MergeRecordSource`, `MergeEscapeOwner`,
     `MergeEscapeSide`, `MergeSideDispositionKind`,
     `MergeConsentCollapseBasis`, `MergeRetiredActionKind`,
     `MergeObservedFactKind`, and the two `MergeOp` values.
   - §3's `MergeRecordProjection` gains fields 6-8, the new messages of
     §2.5.2 are appended, `MergeRequest` gains field 8, `MergeResponse` gains
     field 11, and the field-10 population list gains "quarantine, restore,
     and force-abandon successes".
   - §4 gains the rendering rules of §2.5.3.

**Cited, not edited:** `GwzM5-8I2CompatibilityContract.md` (its eligibility
gates and archive projection are unchanged; the collision extension lives in
RecordContract §8), `GwzM5-8Refactor.md` §15.3.3 (the archived projection
shape is unchanged; the override presence rides the protocol projection),
`GwzM5-8M5bNoFfDesign.md`, `GwzM5-8DurableCursorAmendment.md`,
`GwzM5-8ExactEvidencePlatformAmendment.md`, `GwzM5-8A1DecisionPacket.md`, and
`GwzMergeWedgeRunbook-v0.md`. The escape **design** is superseded only where
§10.1/§10.2 say so.

---

## 8. Test obligations

Named future suites, owed by the implementation package(s); the
contract-level rows join RecordContract §9 at acceptance.

1. **Override round-trip and legality**
   (`model/v1/tests.rs`, `model/v1/validate/operator_override_tests.rs`):
   exact YAML spellings, field order, and absent-by-default encoding;
   the complete owner/side legality matrix of §2.2.4 rule 1 (four owner kinds
   × two sides, with the three single-sided owners' rollback/preservation
   arms rejected); owner-domain rejection (unknown member, `PublicationRoot`
   without composition evidence, `SelectedRootMetadata` without a selected
   root, the two root owners colliding); duplicate/misordered rows;
   both-sides-absent rows; retired-action agreement in all three arms;
   `confirm_token` derivation mismatch; **immutability of every written side
   and consent slot across every subsequent rewrite edge**.
2. **`reason` inertness** (`operator_override_tests.rs` + a call-graph
   check): a fixture pair identical except for `reason` produces identical
   classification, identical cursor derivation, identical exhaustion
   verdicts, identical footprint diffs, and identical proofs; the 4096-byte,
   NUL/C0 and UTF-8 validation rows; verbatim preservation of leading and
   trailing whitespace and embedded LF; absence rendered as
   `(none supplied)`, never synthesized.
3. **Half-marked-owner shapes** (`v1_lifecycle/tests/force_abandon.rs`) — the
   named suite [Q10] requires:
   (a) preservation-marked + rollback-pending, open — legal, renders
   `preservation abandoned; rollback pending`, and the ordinary rollback of
   that owner still runs to an exact terminal row;
   (b) preservation-marked + rollback-exact-terminal at
   `AbortOperationOverridden` — legal, participant carries a real
   `RolledBack`/`Aborted` row **and** the preservation disposition;
   (c) rollback-marked + preservation-not-applicable (a plain abort with no
   `--preserve`) — legal, and a preservation mark on that record is rejected
   as inapplicable;
   (d) rollback-marked + preservation-exactly-complete — legal;
   (e) preservation-marked only, at the terminal edge, with the rollback side
   applicable and neither proven nor marked — **rejected** with code 62
   (the refused write would create a §2.2.4 rule-8-contradicted **written**
   block, which is exactly what 62 is reserved for; it is **not** a
   consent-flow refusal, which is always 63 — §2.5.4);
   (f) a half-marked owner surviving archival and projecting identically from
   archived bytes;
   (g) an owner with a retained forward action and only a preservation-side
   mark — typed refusal naming the missing rollback-side consent;
   (h) the §7 item 3 checked-validator delta, both directions:
   `state: Aborted` + `overridden_abort` + a `Merged` participant whose
   rollback side is marked — decode-legal, from open bytes and archived bytes
   alike; the same shape with `overridden_abort` absent, or with that
   participant's rollback side unmarked — rejected
   (`TerminalRollbackMismatch`).
4. **Provable-collapse consent rows** (`force_abandon.rs`): `:both` accepted
   under `repository_missing` and under `repository_unreadable`, writing two
   side rows in **one atomic rewrite** (the footprint test proves the pair is
   one write) with identical consent (the round's frozen consent, §3.3.3) and
   `collapse.implied_side` equal to each row's **own** side (preservation row
   → `preservation`, rollback row → `rollback`); `:both`
   **refused** for every narrower basis (dropped stash, pruned commit,
   retargeted backup ref, rewritten integration ref, mangled bundle) with the
   error naming the two side tokens; a collapsed manifest asserted to name
   **both** destructions in its printed text (golden); a hand-written record
   whose `collapse` lacks the corroborating `RepositoryState` fact on both
   sides — rejected at decode.
5. **Quarantine round-trip byte-identity** (`v1_lifecycle/tests/quarantine.rs`
   plus a v0 arm): park → digest-equal at destination → restore →
   **byte-identical to the original**, for a v1 record, a v0 record, an
   undecodable record, and a future-envelope record; sidecar written before
   the move; both crash windows (sidecar-only, moved-only) converge
   idempotently; destination-collision suffixing; `--file` selection under the
   multiple-records error; restore refused over a non-empty open slot, over a
   digest mismatch, over a sidecar-less entry, and for a record that decodes
   as terminal; the sidecar lifecycle (§3.1/§3.2): restore renames the
   sidecar to its history name in the same operation (never deleted, never
   left at the plain name), the §4 row-13 orphan converges idempotently and
   blocks nothing, and a stale plain-name sidecar from a previous episode is
   superseded to its history name — never adopted, never overwritten; status
   of an undecodable quarantined record is the typed code-65 error, never a
   `MergeResponse`, in both the sidecar-present and sidecar-less arms
   (§3.2.4).
6. **Forged and tampered override evidence** (`operator_override_tests.rs`):
   every rejection is proven to need **no repository read** —
   `operator_override` on a v0 record; `overridden_abort` with a non-`Aborted`
   state; `overridden_abort` with an applicable side neither proven nor
   marked; `overridden_entry` on a record that never left `Preserving`;
   a disposition whose `retired_*_action` does not equal the record's own
   journal; a side row coexisting with a live journal of the same owner/kind;
   a `confirm_token` that is not the derivation of `record_sha256`; a
   `SideDispositionKind` outside the closed set; an `AbandonEvidence.reason`
   outside the registered list; a fabricated block on a record with no
   participants. Paired with an explicit honesty test-doc note: the record
   plane rejects **contradiction**, not authorship — a valid block always
   describes what the tool did, and `operator` is claimed identity (§2.2.3).
7. **Ordinary-path non-weakening** (`v1_lifecycle/tests/force_abandon.rs`):
   with marks present, ordinary `--abort` completes every tractable owner's
   rollback and then **refuses** at the terminal edge; ordinary
   `BeginRollback` from `Preserving` still refuses; `AbortOperation` cannot
   consume a mark; no escape transition is reachable from any non-
   `ForceAbandon` request (a dispatcher matrix row per request kind); the
   **healed-world row**: with marks present and the marked repository
   restored, ordinary `BeginRollback` from a non-`Preserving` origin enters,
   completes every unmarked owner exactly, executes nothing for the marked
   side, and the terminal edge still refuses (§2.3.4); force-abandon on a
   **v0** record — typed refusal, code 64, with the quarantine hint (§5.1).
8. **Escape restart/fault matrix** (TransitionDesign §13 rows): fault
   injection immediately before and after each of the thirteen §4 crash
   points, plus one footprint test per new variant proving the actual known
   semantic diff equals only its declared fields, plus unknown-survivor and
   retirement-manifest checks after each new transition. The **consent-round
   flow rows** (§3.3.3): (a) the first pass writes nothing and exits code 63
   carrying the token; (b) the correct token against an unchanged digest
   proceeds, and every consent row of a multi-row invocation carries the one
   round anchor verbatim; (c) a wrong, stale, or digest-mismatched
   `--confirm` token is a code-63 refusal writing nothing; (d) a collapse
   basis that turned false between manifest and confirm — the re-run
   preflight refuses the now-observable side with code 64.
9. **Unknown-field survival** (`record_wire/unknown_fields/tests/`): unknown
   descendants inside the block survive by `(owner, side)` identity across
   every append; the name present at v0 top level surfaces in the **v0**
   unknown manifest and triggers migration ineligibility, never adoption; the
   v1 known set adopts it so the first override write passes the overlay's
   unauthorized-unknown check; archival carries the block.
10. **Projection and parity** (P3 fixtures, both drivers): `source`/`archived`/
    `open` agreement across the three sources; an overridden abort never
    rendering as a plain abort in any of human/JSON/JSONL × Rust/Python;
    half-marked per-owner rendering strings; quarantined projection performing
    **zero** live observations (backend call counting); corpus goldens for the
    four new error codes and the manifest response; the quarantined-
    undecodable typed-error route rendering identically in human/JSON/JSONL
    across both drivers; named CBOR/corpus round-trip goldens for
    `MergeOp::{quarantine, restore}`, `MergeEscapeRequest`, and every new
    projection message (the frozen §4 agree-exactly rule, made self-evident).
11. **GC forget-refs** (`gc` evidence): exact-target refs deleted,
    mismatched/unavailable refs left byte-identical and enumerated, archive
    deleted only after the deleted set is observed absent; without the flag
    the stop-and-retain behavior is byte-identical to today's; `--gc` of a
    quarantined id refused; a `quarantine/` directory proven invisible to
    discovery, to retention, and to GC; a directory named `x.yaml` under
    `.gwz/merge/` proven rejected (§3.1).
12. **Bundle and frame invariance**: expected bundle bytes and every preimage
    hash identical with and without the override block present.
13. **CLI refusal and pointer fixtures** (`gwz-cli` + render goldens): bare
    `@publication` — typed refusal listing the three exact owner tokens
    (§2.6); a member owner named with no side — typed refusal; the [Q6]
    pointer line present in every stuck-state typed error and every stuck
    `--status` render (§3.5), goldened across human/JSON/JSONL and both
    drivers.

---

## 9. Budget, review routing, and shipping

### 9.1 Packages

Split for single-owner coherence (L1-06); each is independently reviewable
and independently shippable:

| Package | Content | Wire | Est. impl + tests |
| --- | --- | --- | --- |
| **E0** (this document) | the amendment | — | docs only |
| **E1 quarantine/restore** | store move primitives, sidecar, `--file` selection, third status source, CLI + protocol ops, the Q6 pointer lines | none (record wire untouched) | ~700-1,000 |
| **E2 force-abandon** | `operator_override` wire + validation, the three escape transitions and their proofs, escape lane orchestration, per-side consent and manifest | **this amendment's record/transition delta** | ~1,300-1,800 |
| **E3 projection parity** | Taut appends, generated Rust/Python, projection owner, four renderers, corpus/parity fixtures | protocol appends | ~600-900 (much generated) |
| **E4 forget-refs** | the [Q5] GC valve + manifest | none | ~200-300 |

Total ~2,800-4,000 including tests and matrices — materially larger than the
durable cursor's ~700-1,100, driven by E3's parity surface and E2's matrix
obligations. Reconciled against `GwzM5-8ChangeBudget.md` at implementation
review, not binding here. Expected owners to flag **now**, ahead of that
reconciliation: `model/v0.rs` and `model/v1/record.rs` (the sixth field),
`model/v1/canonical.rs:53-59` (`CanonicalV1State` mirrors the top-level
optionals and gains the sixth),
`model/v1/validate/` (a new override validator module, plus the §7 item 3
delta in `validate/lifecycle.rs:56`),
`v1_lifecycle/transition/{mod,effect,reduce}.rs`,
`v1_lifecycle/authority/dispatcher.rs` (the `ForceAbandon` request arm),
`merge/store/{mod,persistence}.rs` (quarantine move), `merge/gc.rs`,
`record_wire/unknown_fields/{extract/common.rs,mod.rs}` (the top-level
known-key fork), `protocol/gwz.taut.py` + both generated outputs,
`merge/{response,status/snapshot}.rs`, `v1_lifecycle/status.rs`,
`gwz-cli/src/{globalargs/parser.rs,clirequest/merge.rs,merge_render.rs}`,
`gwz-core/src/workspace_ops/merge/validate.rs` (the `--force` carve-out), and
the `gwz-py` renderer set. One activation change is chartered here rather
than left incidental: at A1 the `#[cfg(test)]` on `mod v1_lifecycle`
(`merge/mod.rs:23-24`) and the test-only `Serialize` derive on
`MergeOperationRecordV1` (`record.rs:19`) become unconditional — E2 cannot
write production records otherwise.

### 9.2 Review tier

**Mandated dual review, Code and State axes, cross-model**
(`GwzProcessOptimization.md` §4.2: amendments, interface freezes, and
durable-transition kernels all sit in the mandated tier; §4.3 for the model
split). Review inputs: this document, the escape design, the four amended
contracts, `GwzM5-8M5bNoFfDesign.md` §8.2, `GwzM5-8DurableCursorAmendment.md`,
and the tree paths of §9.1. The three reviewer questions of §10.3 are
pre-committed conditions: a GO must dispose of each explicitly.

### 9.3 Shipping plan ([Q9] — plan, not wire)

1. **Runbook first.** `GwzMergeWedgeRunbook-v0.md` publishes to user docs
   after its staged verifications complete — its `[UNVERIFIED — needs a staged
   reproduction]` items and Appendix C checklist are the gate. This discharges
   the §9.1 documentation debt owed to **current** v0.10.x users independently
   of everything else here, and it is the only escape those users get until
   (2) ships.
2. **Tool-supported quarantine ships with the next regular dev-line release**
   (package E1), not as a special cut. It requires no wire change and reaches
   v0 records.
3. **No v0.10.x maintenance backport** of quarantine, restore, or
   `--forget-refs` unless the operator later requests one. The runbook's
   hand procedure is the released line's supported escape until then, and
   §3.1's naming/discipline rules are written so a hand-parked record is
   recognized by later tool versions (minus the sidecar).
4. **Force-abandon (E2/E3) lands at or before A1**, after R4b-G, per [Q3].

Program-level: escapes performed by the program on real workspaces are
recorded in the ledger like any other deviation (rulebook L1-16 precedent).
One packaging note for the ledger: `GwzM5-8A1DecisionPacket.md` §4 step 2
prescribed a single combined D3+escape I2 amendment; the program shipped them
as separate trains (D3 accepted 2026-08-16; this one after it). The pre-A1
placement is honored; the split is a recorded deviation, not a change
request.

---

## 10. Findings, corrections, and reviewer questions

### 10.1 Design citations that today's tree contradicts

Re-verified at `36754a5`; the escape design's line references were taken
before three amendment landings and one refactor. Corrections, so no reviewer
re-derives them:

1. **`--destructive` is not a flag.** The design's §6.3 CLI sketch and §7.2
   consent model name a global `--destructive`. The released flag is
   **`--force`** (`gwz-cli/src/globalargs/parser.rs:125`; only its help text
   says "destructive"), mapped to `policy.destructive = Allow` at
   `clirequest/invocation.rs:80-83`. §2.6 uses the real flag. The v0 runbook
   already documents `--force` correctly.
2. **`validate.rs:55-56` is in gwz-core, not gwz-cli** —
   `gwz-core/src/workspace_ops/merge/validate.rs:55-56`.
3. **`v1_lifecycle/store/mod.rs:81-96` / `:269-285` is the wrong path.**
   `discover_open` and `record_files` live in **`merge/store/mod.rs`** (the
   v0-shared store) at `:81-98` and `:269-285`. The v1 file is 68 lines and
   holds `CheckedV1Store` only.
4. **`v1_lifecycle/store/persistence.rs:43-48` does not exist as described.**
   The real `merge/store/persistence.rs:43-48` is two `format!` path helpers.
   The exact-byte archive move is
   **`v1_lifecycle/store/archive.rs:11-86`** (a four-way state machine:
   `rename_noreplace`, `sync_dir` on both directories, digest re-verification);
   the v0 analogue is `merge/store/archived.rs:23-60`.
5. **`workspace_mutator_lock.rs:25-34` is the doc comment**, not the acquire
   path (`:14-21` / `:35-38`).
6. **`dispatcher.rs:308` is only the `Executing` arm** of the archive
   rejection; `Finalizing` rejects at `:338` and
   `AwaitingResolution`/`Halted` at the `:286` catch-all.
7. **There is no post-GC durable record rewrite.** `post_gc_record`
   (`merge/gc.rs:365-381`) shapes the **response projection** only
   (`:196`); the archived record is deleted at `:205`. This also makes
   `GwzM5-8DurableCursorAmendment.md` §2.2/§5's "post-GC record rewrite"
   phrasing inaccurate — routed to its next docs pass; no behavior of that
   amendment depends on it (§5.4).
8. **`store/mod.rs:90-96`** — the multiplicity error's code is
   `ErrorCode::MergeRecoveryRequired` (not an unreadable-class code) and an
   unreadable record hard-fails discovery **before** the count check
   (`:85`), so U7's two shapes surface as two different codes.
9. **Confirmed exactly as cited** (so reviewers need not re-check):
   `open_merge_gate.rs:64-76` and `:93-99`; `model/lifecycle.rs:54-64` and
   `:83-117`; `dispatcher.rs:221-297`; `v1_lifecycle/store/archive.rs:22`;
   `record_wire/archive/cleanup.rs:163-167`; `store/gc.rs:8`;
   `protocol/generated.rs:1921`; the protocol contract's "highest code 61".

### 10.2 Corrections to the design's substance

1. **The predecessor set of the overridden terminal edge was unreachable for
   U1** — repaired by the separate `BeginRollbackOverridden` edge (§2.3.3).
2. **Doctrine rule 1's "byte-for-byte ignorant" is unimplementable as
   written** and would maximize destruction — refined to "cursor derivation
   consumes marks; nothing automatic creates or selects one" (§3.3.2).
3. **The override block must not duplicate artifact names.** The design's
   §4.3 records backup refs/stashes/bundles of marked owners in the override
   block; that creates a second source of truth against the archived cleanup
   worklist. Adopted instead: the block names **no** artifacts; retention stays
   governed solely by the preservation evidence rows (§3.3.8).
4. **`@publication` is ambiguous** across three distinct wire owners; the CLI
   token set is split into `@publication-root`, `@publication-evidence`, and
   `@selected-root-metadata` (§2.6).
5. **The confirm token is widened from 8 to 16 hex characters** (§2.2.1).
6. **`quarantine` is already in use inside `gwz-core/src/checked_artifact/`**
   (17 occurrences: `quarantine_parent`,
   `CheckedArtifactFault::{Before,After}QuarantineSourceRetirement`, and
   staging messages). [Q7] stands — the term is already user-committed by the
   published runbook's `.gwz/merge/quarantine/` path, and renaming now would
   break the documented hand escape — but the implementation **must** prefix
   its own types and errors (`MergeQuarantine*`) and the two vocabularies must
   be disambiguated in a code comment at each site, so a reader never confuses
   merge-record parking with checked-artifact staging.
7. **`--reason` mandatory-for-force-abandon is not adopted** ([Q8]); the
   design's §4.2/§6.3 "recommended required" annotations are superseded.
8. **Marking once for both passes is not adopted** ([Q10]); the design's §10
   [Q10] draft ("name once, applies to both passes, printed as such") is
   superseded by per-side consent with the closed collapse basis (§3.3.4).

### 10.3 Reviewer questions (pre-committed conditions for a GO)

- **R-1.** Confirm the §2.3.3 repair: a **second** overridden edge
  (`BeginRollbackOverridden`) versus widening `AbortOperationOverridden`'s
  predecessor set. The trade is one extra transition variant against
  abandoning every tractable owner in the U1 case.
- **R-2.** Confirm §3.3.2's refinement of doctrine rule 1 — that cursor and
  prefix derivation may consume durable operator marks while nothing automatic
  may create or select one — and that §6.1's precedence
  (durable fact → live proof → mark) plus the exhaustion carve-out is the
  right line.
- **R-3.** Confirm that **no unmark edge** is in scope. A mark is permanent
  for the record's life; an operator who marked wrongly must quarantine and
  repair the world, then start a new merge. Adding an unmark edge later is a
  new amendment, not a text round.

---

### 10.4 Round-2 disposition (revision of 2026-08-22)

Every round-1 P1/P2 of both reports, and every P3 taken, mapped to its edit.
Line numbers are this document's own, post-revision. Convergent findings are
listed once with both ids. **No finding required breaching a §0 decided
input; none is left open.**

| Finding | Disposition (section:line) |
| --- | --- |
| Code P1-1 ≡ State P1-3 | The frozen cursor-derivation sentences are now quoted and amended: ActionJournal §2 :178-181 and :185-192 quoted at §1.2:234-253; TransitionDesign §6.6 :648-652 quoted at §1.3:289-298; exact replacement texts in §7 item 2 (:1677-1698) and item 3 (:1733-1740) |
| Code P1-2 | ProtocolContract §1 :46-53 quoted at §1.5:350-359; exact 49–65 / 46–65 edits in §7 item 5 (:1782-1788); cross-noted at §2.5.4:853-855 |
| Code P1-3 (with State P2-5) | Undecodable-quarantined status is a typed code-65 error, never a `MergeResponse`; decodable-only success projection — §3.2.4:1017-1035; arbitration rows §7 item 4 (:1760-1777); suites §8 items 5 (:1879-1885) and 10 (:1934-1936) |
| Code P2-1 | §1.2 annotation reworded — marks clear journals, the terminal edge requires none remain (:227-232) |
| Code P2-2 ≡ State P3-1 | Marker-aware arm retensed to "requires … not yet landed", plus the E-packages-must-not-assume note — §5.4:1431-1437 |
| Code P2-3 | Arbitration made total: archive×quarantine and all-three-present are typed contradictions naming every present path — §7 item 4 (:1772-1774) |
| Code P2-4 | `record.rs:79-110` corrected to :18-51 / :39-50 at evidence base :45, §2.1:383 and :400 |
| Code P2-5 | Stale/wrong/mismatched `--confirm` pinned to code 63 (62 reserved for written-block contradictions) — §2.5.4:855-858, §3.3.3:1097-1100; flow rows (a)-(d) in §8 item 8 (:1915-1922) |
| State P1-1 | The consent round: single anchor digest, validated once at round entry under lock+lease, frozen consent carried verbatim by every row of the round, crash ends the round — §3.3.3:1103-1125; §5.5:1462-1471; §2.3.2 rows :668-672; rule 7 :583-594; collapsed pair is one atomic write — §3.3.4:1159-1165, §4 row 5 :1349 |
| State P1-2 | Ordinary-proof non-constructibility made definitional (`VerifiedRollbackExhausted`, `VerifiedPreservationExhausted`, `PreparedRollbackEntry` from `Preserving`) — §2.3.4:703-723; frozen-note text §7 item 3 (:1722-1732) |
| State P1-4 | Checked-record legality delta for `Aborted`+`overridden_abort` (`model/v1/validate/lifecycle.rs:56`), open and archived decode alike — §7 item 3 (:1713-1721); §2.3.2 terminal row :672; §8 item 3(h) :1853-1859; evidence base :61 |
| State P1-5 | Rule 8 split into a durably-decidable decode form and a full-strength write-edge form with live re-proof where no durable witness exists; archived reads trust the recorded edge — §2.2.4 rule 8 :595-614; §3.3.6:1211-1246; §2.3.2 terminal row :672; §3.3.6 table row :1251 |
| State P1-6 | Sidecar lifecycle specified: history names, restore renames in-operation, supersession never adoption, orphans inert; crash row 13 added and the invariant extended to thirteen — §3.1:958-982; §3.2:1001-1006; §4:1360-1364; §7 item 4 predicate :1761-1763; §8 item 5 :1879-1885 |
| State P2-1 | Both-copies arbitration split by byte identity (identical → open wins, quarantine copy shadowed; different → contradiction) — §7 item 4 (:1767-1771); §4 row 10 :1357 |
| State P2-2 | `implied_side` pinned to the row's own side — §2.2:468, §2.2.4 rule 7 :583-588, §3.3.4:1159-1165, §8 item 4 :1860-1866 |
| State P2-3 | §3 :208-210 range 45–61 → 45–65 — quoted §1.5:361-367; edit §7 item 5 (:1789-1790) |
| State P2-4 | GC consent anchor = validated archive bytes; stale token = code 63; JSONL events carry ref, targets, archive digest, token — §3.4:1320-1331 |
| Code P3-1 | Citation-drift bundle fixed: §7 item 1 :381 (was ":362"); §1.2 :173-176 / §7 item 2 :175-176; §1.3 :325-328, :929-933 (and §3.3.2:1082), :800-829; §2.3.1 `effect.rs:24` (:653) and TransitionDesign §6 :334-337 (:662); §10.1 item 8 :90-96 (:2070); "18 charged paths" (:92); "§8 item 8" (:1745); ActionJournal recovery rule :145-147 (:671) |
| Code P3-2 | Boxing note added — §2.3.1:655-658 |
| Code P3-3 | RecordContract §1 :76-77 scoping annotation — §7 item 1 (:1645-1651) |
| Code P3-4 | Healed-world tightening — §2.3.4:714-720; §8 item 7 row :1905-1910 |
| Code P3-5 | v0 force-abandon code-64 row (§8 item 7 :1910-1912); bare-`@publication`, no-side refusal, and [Q6] pointer fixtures (§8 item 13 :1948-1954); named CBOR/corpus rows (§8 item 10 :1934-1939) |
| Code P3-6 | P-REV "extended here" (§6.2:1544-1547); `canonical.rs:53-59` owner (§9.1:1976-1977); §6.9 quarantine row names the sidecar as persisted owner (§2.4:738); §2.2 uniqueness comment aligned (:410-412); A1 cfg activation chartered (§9.1:1989-1993) |
| State P3-2 | Checkpoint pointer to §6.4 at acceptance — §6.4:1616-1625 |
| State P3-3 | A1DecisionPacket §4 step-2 packaging deviation recorded for the ledger — §9.3:2025-2030 |
| State P3-4 | `--confirm` re-runs the full preflight; reappeared basis refused with code 64 — §3.3.3:1091-1097 |

Tree-currency updates folded into the same revision: post-W1 RecordContract
anchors re-pinned throughout (+21; §1.1:155-158, evidence base :24-31);
thin-A1 second-lane/non-gating status recorded (:22-24, §6.4:1610-1616,
§11); the M5b-W1 composition named (§6.2:1548-1569); §8's suite paths
re-verified against the landed M5b suites (`v1_lifecycle/tests/` with its
`mod.rs`, the `model/v1/validate/*_tests.rs` convention, `model/v1/tests.rs`,
`record_wire/unknown_fields/tests/` — all real at gwz-core `c13f773`).

## 11. Status

**ACCEPTED 2026-08-22 at GO/GO** (Code and State axes, cross-model; round 2
of the two-round cap, no round 3 required). Round 1: **NO-GO/NO-GO** — Code
3 P1 / 5 P2 / 6 P3, State 6 P1 / 5 P2 / 4 P3, zero P0 on either axis. **One**
merged remediation revision, landed at gwz-dev `20f1654`, resolved every
round-1 P1 and P2 on both axes — Code [P1-1..P1-3], [P2-1..P2-5]; State
[P1-1..P1-6], [P2-1..P2-5] — plus the one-line P3s of both reports; §10.4 is
the finding-by-finding map, and no §0 decided input changed. Round 2:
**GO/GO with zero unresolved findings** and no condition attached to either
GO; both re-verdicts answer §10.3's R-1/R-2/R-3 **CONFIRMED
unconditionally**. The residual P4-class notes each axis recorded as
explicitly non-blocking are folded into this acceptance pass: §4 row 4's
parenthetical now cites the definitional rule rather than the superseded
derivational one (State), and §10.4's two drifting map pointers and §8 item
3(e)'s code-62 reading are corrected/clarified (Code). The ten [Q] inputs of
§0 remain decided and were never review questions. At this acceptance the §7
annotations are applied to the five contracts and the wire delta freezes
**pre-A1** — second-lane and non-gating for A1 per the accepted
`GwzM5-8ThinA1Amendment.md` — and implementation proceeds per §9.1/§9.3
after R4b-G, at or before A1, as its own reviewed package(s) ([Q3]).
