# R4b-P reverse-lifecycle interface review — state, restart, and ownership

Date: 2026-08-10

Reviewer: independent state-machine/interface review

Verdict: **NO-GO**

The draft has the right overall direction: it preserves write-ahead ownership,
uses the accepted checked service/store, keeps the two-pass preservation cursor,
uses the frozen rollback order, treats executor diagnostics as non-authoritative,
supports destination-only archive recovery, and uses the immutable archive as
the GC worklist. It is not yet an implementable interface. Four P1 gaps leave a
legal transition misdescribed, lose restart-required stash facts, leave protocol
ownership contradictory, and allow archive bytes without filesystem provenance
to become ref-deletion authority.

No implementation lane may start from this draft. The corrections below do not
require a new durable field, journal phase, transition, or checked-store writer.

## 1. Review basis

This review checked the draft against:

- `GwzM5-8R4bTransitionDesign.md`, especially §§6.1–6.7, 8, 12–15;
- `GwzM5-8I2ActionJournalContract.md`, especially §§2–5;
- `GwzM5-8I2ProtocolContract.md`, especially §§3–4;
- `GwzM5-8Refactor.md`, especially §§15.3, 17/R4b, and 22.4;
- `GwzM5-8ChangeBudget.md`, especially the R4b stop gate;
- the current transition resolver, authority issuers, checked service/store,
  R3 archive decoder/worklist, accepted forward/finalization observers, and v0
  preservation/abort/status/GC helpers.

Decisive current-tree evidence includes:

- `authority/resolver.rs:308-334`: completed participant observations carry an
  entry only for the two `Halted` Abort/Preserve compounds; the ordinary path
  accepts only `EntryFact::None`;
- `model/v1/journal.rs:72-96`: the durable stash action retains identity, HEAD,
  and `preimage_sha256`, but no dirty summary;
- `I2ActionJournalContract.md:269-279`: canonical bundle rows, including dirty
  summary, are derived from the journal owner and decoded canonical preimage;
- `protocol/gwz.taut.py:1502-1512`: the installed `MergeResponse` currently ends
  at field 9;
- `ChangeBudget.md:479-485`: R4b has no normal-build behavior, CLI, wire, or
  protocol delta;
- `record_wire/archive/mod.rs:25-44`: the R3 archive decoder accepts bytes and
  therefore carries no filesystem provenance; and
- `v1_lifecycle/store/rewrite.rs:134-148`: the checked store already has the
  necessary regular, canonical, no-symlink read rule that P can reuse or
  narrowly extract.

## 2. Findings

### P0

None found.

### P1-1 — the retained-forward-owner compound table is not state-exact

Draft §4 says every exact completed/expected-conflict owner supplies an outcome
plus a prepared rollback/preservation entry, and then says an owner may not be
reconciled and entry-prepared as two writes. That is not the accepted state
machine.

`RecordHaltedOutcomeAndBeginRollback` and
`RecordHaltedOutcomeAndBeginPreservation` are legal only from `Halted`.
`Executing` must first use ordinary `RecordParticipantOutcome` with
`EntryFact::None`; after the checked commit, the next service iteration prepares
entry against the new digest. The current resolver enforces exactly that. The
draft as written either makes the legal `Executing` path reject or encourages a
new compound transition, which is forbidden.

Exact correction: replace §4's live-owner table and its “two unrelated writes”
sentence with the operation-state matrix in §4.1 below. State explicitly that
the no-two-write rule applies to not-started abandonment and to a completed
owner while `Halted`; it does not prohibit the frozen ordinary
`Executing -> RecordParticipantOutcome -> fresh entry` sequence.

### P1-2 — the preservation seam loses facts required after restart

`GitPreservationImage` exposes staged/unstaged/untracked facts, but
`GitPreservationStashEvidence` exposes only object id, message, HEAD, and
preimage hash. After `CreateStash` is observed complete and its result is
durably advanced to `RestoreRoot` or `WriteBundle`, the worktree is clean and
the journal contains no dirty summary. A process restart at `WriteBundle`
therefore cannot produce or verify the canonical row required by I2, whose
dirty summary must be derived from the decoded canonical preimage. Hash equality
does not recover the summary.

Exact correction: make every observation method error-bearing and make stash
evidence carry the same decoded image summary as the pre-stash image, for
example:

```text
GitPreservationDirtySummary {
  staged: bool, unstaged: bool, untracked: bool
}
GitPreservationImage {
  preimage_sha256,
  dirty: GitPreservationDirtySummary
}
GitPreservationStashEvidence {
  object_id: GitObjectId,
  message,
  head_commit,
  image: GitPreservationImage
}

preservation_image(...) -> ModelResult<GitPreservationImage>
preservation_stashes(...) -> ModelResult<Vec<GitPreservationStashEvidence>>
checkout_matches_commit(...) -> ModelResult<bool>
```

The Git implementation must derive `image` from the named stash trees, not
from current clean status or display paths. The bundle classifier/builder must
use that decoded image for the current row and for every existing completed row
whose exact canonical bundle prefix is being verified. This is a read-only seam
widening, not a durable-field change.

### P1-3 — field-10 ownership is left contradictory and unbudgeted

I2 requires optional field 10 `record` on durable-record successes. The current
schema has only fields 1–9. The R4b budget simultaneously freezes no wire or
protocol delta, no CLI behavior, and only a test-reachable v1 lifecycle. Draft
§8 and P3 cannot leave this as an implementer choice, and implementing generated
Rust/Python plus JSON/JSONL/human parity inside P would violate the explicit R4b
boundary and the 180/300 status allocation.

Exact correction under the current ledger:

1. R4b-P P3 owns a version-neutral internal semantic projection for checked
   open v1 and R3-decoded archived v0/v1 records, plus read-only status tests.
   It does not edit Taut, generated bindings, current `MergeResponse`, CLI,
   Python, JSON, JSONL, or human rendering.
2. A1 (or a separately named, budgeted pre-A1 protocol checkpoint) appends the
   already frozen field 10, maps the P projection, and passes the complete I2
   inclusion/omission and cross-driver matrix before production activation.
3. `GwzM5-8Refactor.md`/the budget ledger must name that ownership explicitly
   before R4b-P is accepted. If the lead instead chooses P ownership, the R4b
   wire/protocol declaration and numeric/path budget must first be revised and
   independently re-reviewed.

This does not permit field 10 to be omitted at activation. It separates the
R4b internal semantic prerequisite from the user-visible protocol change.

### P1-4 — GC does not prove the archive file is valid deletion authority

Draft §10 says to read the named archive and decode its immutable worklist, but
does not require a canonical, no-follow acquisition of those bytes. The R3
decoder accepts an arbitrary byte slice. A symlinked `done` parent or archive
leaf could therefore supply valid external bytes and authorize deletion of
workspace Git refs.

Exact correction: before the first decode and again before archive deletion,
targeted GC must prove:

- the workspace root and `.gwz/merge/done` parent chain are canonical real
  directories, with no symlink traversal;
- the named path is the canonical `<validated-merge-id>.yaml` leaf;
- the leaf is a regular non-symlink file; and
- the final reread is byte-identical to the initial authority bytes and
  redecodes to the same validated projection/worklist.

Use the checked store's `read_regular` rule or a narrow shared extraction; do
not add a raw archive mutation entry point. Symlinked parent/leaf, nonregular
leaf, path mismatch, or replacement retains the archive and every remaining
ref. Apply the same read rule to destination-only archive/status reads, even
though only GC turns those bytes into mutation authority.

### P2-1 — status lacks source arbitration and optimistic lineage

Draft §8 names open and archived sources but not what happens when both paths
exist, differ, or change while live observations are collected. Current v0
loading prefers open by iteration order; that is not a frozen v1 status
contract. A checked record can also be rewritten between the initial load and
the completed live snapshot because status intentionally takes no mutation
lease.

Exact correction:

- freeze the read-only source-location matrix in §4.5;
- for open status, load the checked source, collect live observations, then
  call an unchanged-source reread; on changed bytes, discard the snapshot and
  retry the whole read once or return the typed contention/recovery error;
- never return a snapshot assembled from one durable digest and another
  lifecycle state; and
- status remains read-only and does not acquire `V1MutationLease` or reconcile
  an archive crash shape.

### P2-2 — exact root-artifact observation must reject noncanonical file forms

The current placeholder `rollback_exhausted` authority uses `fs::read` for the
manifest and lock (`authority/observe.rs:56-80`), which follows symlinks and
checks bytes only. Draft §6 says “exact baseline bytes” but does not freeze file
kind/path checks or require replacing that placeholder.

Exact correction: every root normalization/restoration, evidence step,
selected-root step, and final rollback-exhaustion observation must use
no-follow canonical-parent checks and exact regular-file bytes (including the
accepted executable/mode rule where applicable). An operational read error is
an error, not `false`; a symlink, directory, executable/noncanonical form, or
mixed prefix is ambiguous/evidence mismatch and issues no proof. Add the rows
to the rollback and preservation matrices rather than inheriting v0
`fs::read`/`.exists()` behavior.

### P2-3 — P2's reverse-recovery ownership depends on P1

Draft P2 owns all reverse recovery, but `origin=Preserving` is required to reuse
the phase classifier and earlier-prefix proof owned by P1. Implementing it in
parallel either duplicates preservation policy or imports an unsettled P1
surface.

Exact correction: P1 owns preservation observation plus the
preserving-origin verifier; P2 owns rollback observation plus the
rolling-back-origin verifier. A thin reverse router may be integrated only
after both verifier interfaces settle. Alternatively, P2's preserving branch
is explicitly blocked on P1. It may not implement a local compatibility
classifier.

### P3

None independent of the P1/P2 corrections above.

## 3. What is accepted without correction

The following draft decisions match the frozen contracts and should remain:

- `ReverseRuntime` cannot persist and the service loop remains the only
  lifecycle interpreter;
- a persisted owner is reread before exact classification or execution;
- fresh post-action observation, not executor return, advances a journal;
- preservation uses artifact pass, whole-prefix verification, then reset pass
  in the same durable owner order;
- `PublicationRoot` is excluded when a selected root participant owns the
  colliding root ref;
- incoming Abort cannot abandon partial preservation and needs
  `VerifiedPreservationExhausted` before `BeginRollback(FromPreserving)`;
- rollback order is publication evidence, reverse selected participants, then
  selected-root metadata, with the exact frozen phase edges;
- preservation/rollback between-action ambiguity is not rewritten into an
  unrepresentable recovery row;
- archive orchestration reopens and decodes the destination, including the
  destination-only restart case; and
- GC performs full preflight before its first ref deletion, tolerates an absent
  prefix after a crash, never deletes stashes/bundles, never rewrites terminal
  history, and deletes the archive only after all refs are absent.

## 4. Required executable matrices

These matrices are part of the corrected interface, not optional test ideas.
Each success row must also be crossed with source-digest change, operational
observation error, executor `Success`/`Failed`, and a fresh post-attempt
`NotStarted`/`Completed`/`Ambiguous` observation where a physical action exists.

### 4.1 Abort/Preserve retained-owner matrix

| Durable state | Exact live class | Required observation/transition |
| --- | --- | --- |
| `Executing` | completed outcome or exact expected conflict | `VerifiedParticipantOutcome + EntryFact::None`; `RecordParticipantOutcome`; next checked digest performs fresh global entry preflight |
| `Executing` | not started | `VerifiedParticipantNotStarted + matching prepared entry`; atomic `AbandonNotStartedAndBegin*` |
| `Executing` without retained halt cause | ambiguous | `EnterRecovery(origin=Executing)` with owner retained |
| `Executing` with retained halt cause | ambiguous | persist `Halt`, discard stale proof, reobserve; only then may enter `RecoveryRequired(origin=Halted)` |
| `Halted` | completed outcome or exact expected conflict | outcome plus matching anticipated entry; atomic `RecordHaltedOutcomeAndBegin*` |
| `Halted` | not started | not-started proof plus matching anticipated entry; atomic `AbandonNotStartedAndBegin*` |
| `Halted` | ambiguous | `EnterRecovery(origin=Halted)` with owner retained |
| either | wrong owner, foreign conflict, mixed/third form | typed rejection/ambiguity; no entry proof and no owner retirement |

Cross every row with Abort and Preserve. Entry preparation must cover all
selected participants, root/publication handoff, canonical paths, and relevant
artifacts against the exact anticipated model and source digest.

### 4.2 Preservation phase/cursor matrix

| Pending position | Exact before | Exact after | Third/mixed |
| --- | --- | --- | --- |
| backup ref | ref absent | exact derived ref at exact target | different target/unreadable |
| `NormalizeRoot` | exact recorded publication prefix/index | exact normalized candidate/baseline form | any mixed file/index/worktree form |
| `CreateStash` | no matching stash and current image equals recorded preimage | exactly one matching typed object/message/HEAD/decoded image and exact clean post-stash form | duplicate/foreign/disagreeing stash or residual/new work |
| `RestoreRoot` | exact normalized root form plus durable stash ids | exact recorded publication prefix/index restored | any mixed form |
| `WriteBundle` | absent bundle or exact canonical earlier-row prefix | exact canonical prefix plus current decoded-image row | any other bytes/row/order/identity |
| stash `Complete` | exact after bundle and native evidence | completion proof only; no physical action | missing/changed evidence |
| reset `ResetRef` | exact attached expected commit and clean post-stash checkout | exact attached restore commit and clean anchor checkout | detached, dirty, native state, foreign ref/index/worktree |
| reset `RestoreRoot` | exact reset root form | exact recorded prefix restored | any mixed form |
| reset `Complete` | exact ref/checkout and root form | completion proof only; no physical action | missing/changed evidence |

For every row, cross root-prefix absent/present, participant root versus
`PublicationRoot`, SHA-1/SHA-256, non-UTF-8 paths, every forbidden semantic
index flag/form, process restart before/after the physical action and before/
after the result write. Every named pending position additionally requires a
`VerifiedPreservationCursorPrefix` proving all earlier positions complete or
unnecessary. Add a whole-prefix row before reset pass and a complete-two-pass
row before `VerifiedPreservationExhausted`.

### 4.3 Rollback cursor/action matrix

| Cursor owner | Exact order/phases | Required terminal handling |
| --- | --- | --- |
| publication evidence | `EvidenceCommit -> Boundary -> Lock -> Marker -> Index -> Complete` | one persisted phase owns one checked mutation; completion alone sets `evidence_rolled_back` |
| participant, reverse durable order | `AbortConflict` or `ResetIntegrated` only from immutable row | exact clean before/after checkout classification; finish writes only `Aborted`/`RolledBack` fields |
| no-mutation participant | `Planned`, `UpToDate`, `Failed`, or `Unattempted` current pure cursor | `RecordNoMutationAbort`; no fabricated journal or Git action |
| selected-root metadata | `Manifest -> Lock -> Complete` | operation-baseline bytes are authority before acceptance; acceptance, if present, must cross-check |
| exhausted | no pending journal and pure cursor complete | no-follow exact baseline reread for selected root, then `VerifiedRollbackExhausted` and `AbortOperation` |

For every mutating phase test exact before, exact after, dirty/mixed, foreign
ref/HEAD, native state, noncanonical parent/leaf, symlink, operational error,
and crash on both sides of the physical and durable result boundaries. Assert
that no participant or selected-root action can start before evidence rollback
is terminal and no selected-root step can skip/reverse/repeat.

### 4.4 Recovery matrix

| Literal origin | Durable owner | Verification owner | Result |
| --- | --- | --- | --- |
| `Preserving` | exact pending preservation action required | P1 phase classifier plus exact cursor prefix | exact representable before/after issues `VerifiedRecoveryOrigin`; ambiguity/error leaves row unchanged |
| `RollingBack` | exact pending rollback action required | P2 pure cursor plus matching phase classifier | exact representable before/after issues `VerifiedRecoveryOrigin`; ambiguity/error leaves row unchanged |
| forward origin | exact forward owner/selected set | accepted X verifier | delegate; P must not duplicate policy |
| `Finalizing` | exact finalization shape | accepted F verifier | delegate; P must not duplicate policy |
| preserving/rolling back between actions | no pending reverse journal | ordinary P observer | ambiguity is unrepresentable: typed error, no rewrite, no fabricated owner |
| any origin mismatch | wrong/missing owner or derived resume action | none | reject before proof issuance or mutation |

Every successful resume restores only the literal origin and clears recovery
context; ordinary dispatch then reobserves the still-retained action.

### 4.5 Read-only status and archive-location matrix

| Open source | Archive destination | Status result | Archive-request result |
| --- | --- | --- | --- |
| absent | absent | named not-found / unqualified idle | not found |
| valid nonterminal | absent | checked open projection plus live observation | reject nonterminal |
| exact terminal | absent | open (`archived=false`) projection | checked archive, reopen destination |
| absent | valid terminal | archive-only (`archived=true`) projection; no live access | decode destination-only success |
| exact terminal | identical terminal bytes | open (`archived=false`) projection without mutation | checked reconciliation removes source, then reopen destination |
| any source | different/malformed destination | typed contradiction, no mutation | typed contradiction, preserve both |
| nonterminal source | any present destination | typed contradiction, no mutation | typed contradiction, preserve both |
| noncanonical/symlink parent or leaf | any | typed unreadable, no live access | typed rejection, no deletion |

For open rows, require the optimistic unchanged-source reread after live
observation. Archived rows must succeed with every participant repository
absent and must not touch archive bytes.

### 4.6 Partial archive-GC matrix

| Initial archive/worklist/ref state | Permitted result |
| --- | --- |
| unreadable, unsupported future, noncanonical parent/leaf | retain archive; delete no refs |
| any path/name/owner/target invalid or colliding | retain archive; delete no refs |
| any repository unavailable or any ref at different target | retain archive; delete no refs |
| all refs absent | exact final reread/redecode, delete archive, fsync directory |
| absent prefix plus exact remaining refs | full preflight, checked-delete exact refs one at a time |
| all refs exact | full preflight, checked-delete one at a time |
| crash/error after a proper subset | archive bytes unchanged; absent subset is complete on restart; continue remaining refs |
| archive changes after preflight or during deletion | stop; retain changed/current path and remaining refs |
| ref reappears or differs before final check | retain archive; never delete the changed ref |
| exact archive, no backup refs, stash/bundle evidence only | delete archive after archive-only validation; never delete stash/bundle |
| unlink succeeds but directory fsync fails | report failure; restart must safely accept archive absent or reappeared, with all refs already absent |

Run the matrix for v0 and v1 archive bodies, SHA-1 and SHA-256 targets, selected
root/publication-root collision rejection, and source paths missing only after a
prior partial crash. Assert exact archive byte identity before every failed
case and until the final all-absent deletion boundary.

### 4.7 Protocol inclusion matrix (A1 or separately budgeted checkpoint)

Field 10 is present for start after durable creation, open status, archived
status, continue, preserve-abort, abort, and id-qualified GC successes. It is
absent for dry-run, idle status, pre-record failure, unqualified GC, and every
top-level error response. Run exact Rust/Python/generated-corpus, JSON, JSONL,
human, retained-reader, sorting/null/empty-list, and v0/v1 discriminant parity.

## 5. Parallel-start decision

**P1–P4 may not start in parallel from the current draft.** The interface is
NO-GO with open P1 findings, and the budget makes an unowned protocol change a
package stop condition.

After the lead incorporates the corrections and both independent reviews
accept the amended interface:

- P1 preservation plus preserving-origin recovery may start;
- P2 rollback plus rolling-back-origin recovery may start in parallel with P1;
- P3 internal read-only status/projection may start in parallel, but field-10
  schema/binding/renderer work remains at the explicitly ledgered A1/pre-A1
  checkpoint;
- P4 archive/GC may start once the canonical archive reader/deletion-authority
  seam is frozen; and
- the thin reverse-recovery router and aggregate restart matrix wait for both
  P1 and P2 verifiers.

Thus the four semantic lanes can mostly proceed concurrently after correction,
but the draft's current claim that P2 independently owns all reverse recovery
and P3 may choose protocol timing is not acceptable.

## 6. Acceptance gate for re-review

Re-review is GO only when the lead draft:

1. installs the exact state-specific compound matrix;
2. widens the read-only stash evidence seam with restart-complete decoded image
   summary and `ModelResult` failure propagation;
3. assigns field-10 implementation to a named, budgeted checkpoint without
   changing the R4b normal-build boundary;
4. freezes canonical no-follow archive acquisition before GC authority;
5. freezes status location arbitration and unchanged-source reread;
6. requires canonical regular root-artifact observation rather than current
   byte-only `fs::read` placeholders;
7. splits preserving versus rolling-back recovery ownership across P1/P2 (or
   records the dependency explicitly); and
8. makes all matrices in §4 executable evidence within the controlling package
   and path ceilings, or obtains a reviewed ledger revision before work starts.

