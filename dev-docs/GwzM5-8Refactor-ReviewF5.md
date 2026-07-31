# GWZ M5–M8 Refactor Proposal — Independent Review (F5)

Date: 2026-07-30. Reviewer: Claude (Fable 5).

Subject: `dev-docs/GwzM5-8Refactor.md` (2026-07-30 revision, status
"sixth-reviewed").

Method: independent review. I did not read
`GwzM5-8Refactor-Review*.md` (any of them), to avoid anchoring. Every factual
claim the proposal makes about the current M4 implementation was checked
directly against gwz-core at `dcbd0e5` (v0.10.2 line), including record model,
store, lifecycle call sites, release tags, and the commit history of the
relevant types. Findings first; §4 lists the characterization claims I
verified as accurate, because several findings depend on trusting that
baseline.

---

## 1. Findings

### [P1-1] "M5 remains a v0 writer" fails the proposal's own §15.1 gate for `--no-ff`: every released binary silently mis-executes a `mode: no_ff` open record

The proposal asserts M5 ships as a v0 writer
([GwzM5-8Refactor.md:1012](GwzM5-8Refactor.md) "R2a/M5 remain v0 because they
reuse existing integration-record semantics";
[:1533](GwzM5-8Refactor.md) "M5 remains a v0 writer";
[:1067](GwzM5-8Refactor.md) creation matrix "M5 → v0"). For `--no-ff` this is
contradicted by checkable facts:

- The v0 record already carries an operation-level `mode`
  ([model.rs:203](gwz-core/src/workspace_ops/merge/model.rs:203), serialized
  unless `Normal`), and `MergeExecutionMode` already compiles the `NoFf`
  variant ([model.rs:17](gwz-core/src/workspace_ops/merge/model.rs:17)).
- `mode` is consumed **only at planning**
  ([plan.rs:211-228](gwz-core/src/workspace_ops/merge/plan.rs:211)
  `enforce_mode`, ff-only). No continue, status, abort, or recovery path reads
  `record.mode` (verified by sweep: the only other reference is the write at
  [start/record.rs:74](gwz-core/src/workspace_ops/merge/start/record.rs:74)).
  Record validation checks only id/schema constants
  ([store.rs:132](gwz-core/src/workspace_ops/merge/store.rs:132)).
- Release archaeology: `mode` + `NoFf` entered the model in `36d03f9`
  ("Complete M4 merge prediction and ff-only"). Tags v0.10.0, v0.10.1,
  v0.10.2 (and rcs) contain it; v0.9.2 and earlier predate the field entirely.

Consequence for an M5-written open record with `mode: no_ff`, at the widest
recovery window (record created, a fast-forwardable participant not yet
holding a pending action — i.e., any crash or downgrade between record
creation and that participant's mutation):

- **v0.10.0–v0.10.2 readers** decode `NoFf` silently, ignore it during
  continue, re-derive the participant's action from live analysis, and
  **fast-forward** — where M5 semantics require a prepared two-parent commit.
  Different mutation selection, silent.
- **≤ v0.9.2 readers** treat `mode` as an unknown field: it lands in the
  `extensions` flatten and is faithfully preserved
  ([store.rs:240](gwz-core/src/workspace_ops/merge/store.rs:240)
  `carry_unknown`) while the binary behaves as normal mode. Same silent
  divergence — unknown-field preservation is precisely the mechanism that
  hides it.

So **no released binary either rejects or behaves equivalently**, which is
§15.1's own disqualifier ([:1017](GwzM5-8Refactor.md) "If any supported old
reader accepts a new shape and can make a different mutation or lifecycle
decision, I2 must select a new schema/version pair"), and by §15.1.1's own
definition ([:1026](GwzM5-8Refactor.md) — the version denotes required
executable semantics, not decodable fields) a no-ff record is *not* a v0
record. Note the planned M5 "forward-schema fail-closed diagnostic"
([:1674](GwzM5-8Refactor.md)) does not help: a no-ff record carries the same
`gwz.merge-operation/v0` / `0` pair that diagnostic keys on.

**Impact:** the approved immediate scope collides with its own gate at M5
time. Discovering it then forces re-sequencing after R2a is built, and the
version ladder (v1 = acceptance at A1) has no slot below v1 for an M5 bump.

**Recommended correction** — the proposal should decide this explicitly now,
in §15.1/§16.1/R2a, choosing one of:

1. **Split M5 (cleanest):** ship `-m` custom messages as v0 now — verified
   old-reader-safe, because the recorded `participant.commit_message` is
   already the recovery authority
   ([reconciliation.rs:160](gwz-core/src/workspace_ops/merge/continue_op/reconciliation.rs:160),
   [execution.rs:195](gwz-core/src/workspace_ops/merge/continue_op/execution.rs:195))
   — and move `--no-ff` activation to A1/v1, whose envelope makes every older
   binary fail closed. v1 is already defined as "M4/M5 integration plus
   acceptance" ([:979](GwzM5-8Refactor.md)), so no ladder change is needed.
2. **Design no-ff for old-reader equivalence and prove it:** no-ff changes
   behavior only for fast-forwardable participants (divergent ones true-merge
   under any mode), and their two-parent result is fully deterministic at
   planning (tree = source tree). Persist the prepared `TrueMerge` pending
   action with `commit_spec`
   ([pending.rs:22](gwz-core/src/workspace_ops/merge/pending.rs:22) executes
   specs) for **every mode-affected participant in the record-creation
   write**, closing the re-derivation window. Then the §15.1 matrix must
   prove, with actual binaries, that (a) v0.10.x tolerates multiple
   simultaneous pending actions and executes the spec, and (b) ≤ v0.9.2 —
   where `commit_spec` itself is an unknown field inside the action — rejects
   rather than degrades. Higher proof burden; may still fail.
3. **Cut the support window:** declare at I2-time (moved earlier) that no
   released binary is a supported reader of open no-ff records. This is
   §15.1-legal only via the "names every supported reader" clause, quietly
   voids the gate where it first matters, and needs an explicit release-noted
   "no downgrade with an open merge" contract. Weakest option.

**Regression test:** §15.1 matrix fixture — open v0-shaped record with
`mode: no_ff`, one fast-forwardable participant planned, no pending action —
run against actual v0.9.2 and v0.10.2 executables through status, continue,
and abort; require typed rejection or byte-identical no-ff outcomes.

### [P2-1] §15.1's contract language under-scopes the gate: "additive v0 field" misses newly writable values and the unknown-field-flatten hazard

The no-ff case is not a new field — it is a **newly writable value of a
pre-declared field**, and for pre-field binaries the entire field flattens
into `extensions`. §15.1 mostly says "additive v0 field(s)" and "the additive
fields removed"; the M5-specific conclusion at
[:1012](GwzM5-8Refactor.md) shows the narrow reading being applied in the
document itself. Two corrections:

- Restate the gate over "any newly writable shape — field, value, enum
  variant, or combination," and require the actual-binary matrix to span
  **every release generation whose decode behavior differs** (field-unknown
  generations like ≤ v0.9.2 vs variant-known generations like v0.10.x), not
  just "the previous release."
- Acknowledge that v0 already predeclares `NoFf` ahead of executable support —
  exactly the dormant-variant anti-pattern the proposal forbids for v1
  ([:107](GwzM5-8Refactor.md), [:2240](GwzM5-8Refactor.md)) — and state that
  the M5 gate must treat that pre-declaration as the liability it is rather
  than as grandfathered compatibility.

### [P2-2] `merge/mod.rs` — the largest lifecycle module — is absent from the target ownership map

§13 names `finalize.rs` (728 lines) as the concentration point and §14 draws
the target module map, but [mod.rs](gwz-core/src/workspace_ops/merge/mod.rs)
is 1,158 lines — 60% larger — and owns the genuinely cross-cutting material:
public dispatch (`handle_merge`, `handle_merge_with_events`), the open-merge
gate (`enforce_workspace_open_merge_gate`), the workspace mutation guard, the
`MergeStore` seam, and record/transition persistence helpers
(`persist_operation_transition`, `persist_merge_record`). R1's policy
extraction and R2a's intent seam will naturally route through this file. If it
has no assigned destination in the map, it becomes the next god-file while
finalization is being split. Assign its contents explicitly (gate/guard →
operation-runtime ownership; store seam + persistence helpers → model/store
ownership; dispatch kept under the ~300-line orchestration target).

### [P3-1] The M5 message-byte freeze has two claimed owners, one impossible

§16.1 ([:1525](GwzM5-8Refactor.md)) says "the interface checkpoint freezes the
exact final message bytes," but the only interface checkpoint in the package
order (I1) runs **after** M5 ships; R2a's task list
([:1658](GwzM5-8Refactor.md)) also claims the freeze. Name R2a (or an explicit
pre-M5 mini-decision) as the single owner and remove the checkpoint phrasing,
or M5's release gate is formally unsatisfiable.

### [P3-2] Retained-binary matrices have no operational specification

§15.1/§18.8 make actual released executables the load-bearing proof mechanism
for the entire versioning strategy, and P1-1 shows the matrix must span at
least two decode generations, not one. The proposal never says where retained
binaries come from (release artifacts by tag?), which platforms (the release
line tests Linux and Windows), where they are cached, or how CI invokes them.
Freeze this mechanism at or before I2; without it the matrices will be run ad
hoc or quietly skipped.

### [P3-3] `ArchivedAcceptanceProjection` cannot represent its own row AV0-F

Row AV0-F projects "LegacyComplete plus aborted operation outcome," but the
sketched enum ([:1284](GwzM5-8Refactor.md)) carries terminal outcome only
inside `NotAccepted`. Acceptance availability and terminal operation outcome
are orthogonal (the mapping table itself says so); the frozen type needs the
terminal outcome as a sibling of the availability variant for all four states,
or AV0-F/AV0-G collapse into ambiguity. Sketches are declared illustrative,
but this one drives an eight-row frozen mapping — one structural sentence in
§15.3.3 closes it.

### [P3-4] All durable-state work is gated on the two hardest open product designs

I2 → R4a → R3 → R4b → A1 all wait for full M6 **and** M8 interface approval
(§22.3 items 1–2), including branch-ownership/ABA policy and the complete skip
model — while v1 deliberately serializes none of their variants. If those
designs stall, the real durability wins (persisted acceptance, no-publication
archival, elimination of restart lock-byte regeneration) stall with them. The
churn-avoidance rationale is defensible, but the tradeoff is implicit.
Consider explicitly allowing I2 to freeze v1 on a narrower "M8 lock-domain
direction + M6 checkout-evidence direction" memo when the full designs are not
ready, with mandatory re-review before v2/v4 freeze. If the conservative gate
is retained, say why in §22.3 so the schedule coupling is a recorded decision
rather than an accident.

### [P3-5] The review basis is stale: a seventh review exists on disk

The status block ([:5-15](GwzM5-8Refactor.md)) says "sixth-reviewed … accepts
all findings from all six reviews," but `GwzM5-8Refactor-Review-7.md` now
exists alongside the six listed files. I did not read it (independence), but
the discrepancy is objective: either incorporate/rebut its findings and update
the status line, or mark it explicitly as post-dating this revision.

---

## 2. Verified-accurate M4 characterization

The proposal's credibility rests on its description of the current
implementation. I verified the following claims against gwz-core `dcbd0e5`;
all hold:

| Proposal claim | Verified against |
| --- | --- |
| Store deserializes directly into `MergeOperationRecord`, then validates constants; recursive unknown-field carry on rewrite (§15) | [store.rs:151-166](gwz-core/src/workspace_ops/merge/store.rs:151), [store.rs:240](gwz-core/src/workspace_ops/merge/store.rs:240) `carry_unknown` |
| Exactly four pending action kinds: verification, fast-forward, true-merge, conflict-resolution (§7.1, §18.1) | `PendingMergeActionKind` [model.rs:302-308](gwz-core/src/workspace_ops/merge/model.rs:302) |
| Prepared tree and exact signatures for deterministic commits (§7.1) | `PendingCommitSpec { tree_oid, author, committer }` [model.rs:281](gwz-core/src/workspace_ops/merge/model.rs:281) |
| `PublicationCandidate.lock_yaml` exists; candidate carries baseline lock, marker, boundary bytes and hashes (§15.3.2) | [model.rs:351-366](gwz-core/src/workspace_ops/merge/model.rs:351) |
| Publication progress tracks composition commit/tree, candidate hashes, evidence rollback, root preservation, preservation prefix (§15.3.2 rows D–I) | `PublicationProgress` [model.rs:326-349](gwz-core/src/workspace_ops/merge/model.rs:326) |
| Baseline has optional `lock_yaml` bytes, `root_head`, `root_branch` — grounding the born/unborn adapter rules and AV0-C's "optional baseline lock bytes" (§15.3, §18.7) | [model.rs:174-190](gwz-core/src/workspace_ops/merge/model.rs:174) |
| `MergePlan` freezes source ref, mode, baseline, ordered participants; participants freeze branch/before/source/message (§3) | [model.rs:151-171](gwz-core/src/workspace_ops/merge/model.rs:151) |
| Archives live under `.gwz/merge/done` with retention 20 (§15.3.3) | [store.rs:16-18](gwz-core/src/workspace_ops/merge/store.rs:16) |
| Participant lifecycle policy is spread across modules (§10, R1) | `ParticipantState::` in 19 non-test files, ~11 distinct state-match expressions |
| `finalize.rs` is a concentration point (§13) | 728 lines, above the proposal's own 500-line review limit |
| Bazel gates are real (§18.9) | `rust_test` targets present in gwz-core `BUILD.bazel` |

Two positive verifications worth recording: `-m` custom messages are already
old-reader-safe (recorded message is the recovery authority end-to-end), and
the closed v0 publication mapping's field references (candidate bytes,
`evidence_rolled_back`, `preservation_prefix`, `composition_commit/tree`)
correspond one-for-one to real record fields — §15.3.2 was clearly written
against the actual model, not an idealization.

## 3. Minor observations (no action required)

- §5 announces "six layers" and then tables seven questions (root publication
  and projection split). Cosmetic.
- The work packages carry no LOC budgets, unlike prior GWZ plans (the merge
  remediation plan budgeted every package). R0/R1/R2a would benefit from
  budgets, since "characterization" and "extraction" packages are the classic
  place for scope creep.
- The merge module is now ~15.3k lines across ~50 files; the proposal's
  restraint ("do not perform another workspace-wide mechanical split") is the
  right call, and the ownership map is otherwise well-targeted.
- §15.6's staged-write/rename/re-verify algorithm matches the durability
  idioms already present in the store (`rename_durable`, `sync_dir`) — good
  continuity.

## 4. Verdict

The architecture is sound and the sequencing discipline (characterize →
centralize → wrap → interface checkpoints → adapters with a hard-disabled
writer → consumption → single activation gate; version-per-semantic-wave with
actual-binary downgrade matrices; evidence-only archive projection) is
unusually rigorous. The M4 characterization is accurate everywhere I checked,
which is the strongest possible foundation for a refactor proposal.

One finding is blocking for the *approved immediate scope as written*:
**P1-1**. "M5 remains a v0 writer" is not currently true for `--no-ff` under
the proposal's own compatibility rules, and the contradiction is cheap to
resolve now (choose the split-M5/option-1 path, or commit to the
eager-prepared-action proof) but expensive to discover mid-M5. R0 and R1 are
unaffected and can proceed immediately; R2a is affected only in what it may
serialize for mode intent.

Everything else — the gate-language generalization (P2-1), the `mod.rs`
ownership gap (P2-2), and the P3 items — is refinement that fits naturally
into the next revision and the I1/I2 checkpoints.
