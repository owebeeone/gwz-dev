# M5-8 I2 durable preservation-cursor amendment (per-owner no-op skips + reset completion)

Date: 2026-08-16. Revised: 2026-08-16 (dual-review remediation round 1:
State P1-1, P1-2, P2-1, P2-2, P2-3, P3-1, P3-2, P3-3 + Code P2-1, P3-1,
P3-2, P3-3 and P4 notes — document-only; no code, no other files).
Banner-application edit: 2026-08-16 (acceptance; §7 deltas applied to the
four contracts; Code R2-1/R2-2/R2-3 and State P4-1 wording repairs).

Status: **accepted 2026-08-16** — dual review round 1: State NO-GO (2×P1 +
3×P2 + 3×P3) and Code NO-GO (1×P2 + 3×P3) against the round-1 text, with
peer-blind convergence (Code P2-1 ≡ State P1-2 — the same defect);
document-only remediation applied; focused re-verdicts **GO on both axes**
(`GwzM5-8DurableCursor-ReviewState.md` — all five pre-committed conditions
resolved, both P1s closed by construction — and
`GwzM5-8DurableCursor-ReviewCode.md`; re-verdicts appended to both
reports). The four amended contracts carry acceptance annotations dated
2026-08-16 (§7). Implementation may trail into the A1 package per the
adopted decision. The §5 F5-lesson compatibility statement stands: no hash
frame, coverage, or projection changes. Code's three round-2 wording notes
(R2-1/R2-2/R2-3) and State's P4-1 are applied in this banner-application
edit.
The underlying decision is adopted: `GwzM5-8A1DecisionPacket.md` Decision 3
("Adopt the minimal durable cursor — per-owner no-op skip rows + a
reset-completion bit … via an I2 journal/record amendment riding the same
pre-A1 train as the operator-escape wire changes", §0 row 3, §3.5). Per that
decision the **wire amendment must land pre-A1** ("Pre-A1 is the last cheap
moment", `GwzM5-8ProgressReviewF5.md` §3.3 :206; §9 item 4 :451) while
**implementation may trail into the A1 package**. §7 lists the exact
acceptance-time annotations — applied to the four contracts on acceptance,
2026-08-16 — following the
idiom `GwzM5-8ExactEvidencePlatformAmendment.md` and
`GwzM5-8R4bInterfaceAmendment-1/2.md` established (amendment banners on the
amended documents; the amendment file is controlling where they disagree).

Evidence base: direct reads of the frozen I2 texts as amended
(`GwzM5-8I2ActionJournalContract.md`, `GwzM5-8I2RecordContract.md`,
`GwzM5-8I2CompatibilityContract.md`, `GwzM5-8R4bTransitionDesign.md` §6.7,
`GwzM5-8R4bReverseLifecycleInterface.md` §6) and of the implemented cursor
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/cursor.rs`,
`preserve/plan.rs`, `preserve/artifacts.rs`, `preserve/checked_bundle.rs`,
`model/v0.rs`, `model/v1/validate/preservation.rs`,
`v1_lifecycle/transition/reduce/preservation.rs`). Line numbers are against
the current lane tip.

---

## 1. What this amends

The v1 preservation wire deliberately persists neither of the two facts the
restart cursor needs. The frozen sentences, verbatim:

- `GwzM5-8I2ActionJournalContract.md` §2 :158-160 — "Preservation has an
  additional live condition because this wire format intentionally persists
  neither no-op owner skips nor a separate reset completion bit."
- Ibid. :161-166 — "Before a pending preservation action is classified,
  advanced, or executed, an exact bound observation must also prove that
  every earlier position in the two-pass cursor is complete or currently
  unnecessary. A later pending owner with any earlier incomplete or
  ambiguous position authorizes no rewrite or physical mutation. This
  clarifies the existing deterministic order; it adds no field or journal
  variant."
- `GwzM5-8R4bTransitionDesign.md` §6.7 :742-745 — "I2 does not persist a
  no-op row when an owner needs no artifact, nor a separate completion bit
  after a reset journal clears. Consequently, a pure decoder cannot
  distinguish a legitimately later pending owner from the same bytes paired
  with a live earlier owner that has become incomplete."
- `GwzM5-8R4bReverseLifecycleInterface.md` §6 :342-345 — "Before any pending
  action is classified, advanced, or executed, the observer issues the
  existing `VerifiedPreservationCursorPrefix` only after every earlier live
  position is exactly complete or exactly unnecessary."

The compensation in the implementation is wholly live and repeats per
dispatch of a `Preserving` record: `observe_cursor`
(`cursor.rs:14-45`) re-derives every owner plan
(`v1_preservation_owners`, `preserve/plan.rs:397` — live ref and commit
reads per owner) and then, per owner, `backup_complete` (`cursor.rs:241-249`
— durable ref row **or** the live fact `protected_commit == plan.anchor`),
`stash_complete` (`cursor.rs:251-283` — a **full live preservation image
capture** in *both* arms: with durable stash ids at :276-277 and without
them at :279-282; `v1_preservation_image`, `preserve/artifacts.rs:663-680`,
byte-reads the worktree), and `reset_complete` (`cursor.rs:285-305` — no
durable bit at all: live `plan.live_commit == plan.anchor` at :290-297,
plus another live image capture in the descendant-backup arm at :298-304).
`verify_pending_prefix` (`cursor.rs:99-156`) repeats the earlier-owner
sweep before any pending action runs — for a pending reset, over **all**
owners (:138-141). The F5 review named this "the design's weakest joint"
(`GwzM5-8ProgressReviewF5.md` §3.3 :197-206), asymmetric with rollback's
decode-derivable cursor (`GwzM5-8I2ActionJournalContract.md` :155-158).

This amendment persists exactly the two missing facts as additive fields on
the existing per-owner preservation evidence rows, makes the durable
artifact pairs decode-terminal for prefix purposes, and retains today's
live proof as the fallback wherever a durable fact is absent. It adds **no
journal variant, no pending-action kind, no phase value, and no top-level
record field**.

## 2. Wire

### 2.1 Exact field delta

The per-owner preservation evidence row — one stable row per owner at
participant `preservation` (`model/v0.rs:80`) and publication
`root_preservation` (`model/v0.rs:186`), accessed via `v1_owner_evidence`
(`preserve/plan.rs:714-745`) — gains two trailing optional fields:

```rust
struct PreservationEvidence {
    pub backup_ref: Option<String>,
    pub backup_commit: Option<String>,
    pub stash_id: Option<String>,
    pub stash_object_id: Option<String>,
    // New in this amendment. Absent-by-default on the wire.
    #[serde(default, skip_serializing_if = "Option::is_none")]
    pub noop_commit: Option<String>,
    #[serde(default, skip_serializing_if = "Option::is_none")]
    pub reset_commit: Option<String>,
}
```

- **Names/spellings.** Exactly `noop_commit` and `reset_commit`
  (snake_case, matching the row's existing keys).
- **Position.** Appended after `stash_object_id`; YAML emission order is
  declaration order.
- **Encoding.** Full unabbreviated lowercase hexadecimal commit ids —
  SHA-1 ids contain 40 lowercase hex characters and SHA-256 ids contain 64,
  the same validator family as `backup_commit`. Both are **absent when
  unset** (`skip_serializing_if`), never `null` — this keeps every byte
  stream produced by writers that do not set them identical to today's.
- **Not booleans.** The ActionJournal calls the second fact a "reset
  completion bit"; it is spelled as a commit id because all free-looking
  wire values in this contract family are validated derivations, never
  ambient authority (`GwzM5-8I2ActionJournalContract.md` :168-170). Each
  field's exact required value is fixed in §2.2, so a decoder can reject a
  fabricated marker without any live observation.

### 2.2 Validated derivations

- `noop_commit`, when present, must equal the recorded `backup_commit` when
  the backup pair is present, and otherwise the immutable owner anchor (the
  participant result — the composition commit for a selected root after
  composition — or the publication composition commit, exactly the §2
  anchor derivation of the ActionJournal contract).
- `reset_commit`, when present, must equal the immutable owner anchor.

Two structural rules are normative and generate the legality table; the
table enumerates all sixteen combinations under them:

1. a stash pair and `noop_commit` may never coexist, regardless of any
   other field on the row; and
2. `reset_commit` requires artifact-pass completion evidence on the same
   row — `noop_commit` or a stash pair.

Legal field combinations for one row (B = backup pair, S = stash pair,
N = `noop_commit`, R = `reset_commit`):

| Row content | Legal | Meaning |
| --- | --- | --- |
| all absent | **no** (unchanged) | an empty row remains invalid (`validate/preservation.rs:121-123`); absence of the row is the empty state |
| B / S / B+S | yes (unchanged) | pre-amendment shapes remain valid — required for graceful degradation (§4) |
| N | yes | artifact pass retired this owner with no artifact needed |
| B+N | yes | backup created; stash position proven unnecessary |
| N+R / B+N+R / S+R / B+S+R | yes | reset position also retired (executed or unnecessary) |
| S+N / B+S+N / S+N+R / B+S+N+R | **no** | a stash pair contradicts "no artifact needed" (rule 1), regardless of `reset_commit` |
| R alone / B+R | **no** | rule 2: `reset_commit` requires artifact-pass completion evidence on the same row (`noop_commit` or the stash pair); unreachable for any post-amendment writer because both reset edges backfill `noop_commit` in the same write (§3.1), and pre-amendment writers cannot write `reset_commit` at all |
| any value mismatch against §2.2 equations | **no** | typed evidence mismatch, no mutation |

Both fields are **immutable once written**, joining the row's existing
immutability discipline ("one stable row per owner; the same row survives
as ref/stash fields fill and survives archival",
`GwzM5-8I2RecordContract.md` §8 :385). Pending-action cross-checks: a
pending `BackupRef` or `Stash` action for an owner whose row carries
`noop_commit`, or a pending `ResetAttachedRef` for an owner whose row
carries `reset_commit`, is an owner/phase/pass contradiction and rejects at
decode. Multi-row rejection (`validate/preservation.rs:86-88`) and
selected-root/publication-root collision rules are unchanged.

**Terminal-plane fate.** Immutability spans the record's open lifetime and
archival ("survives archival", §8 :385). At the post-GC record rewrite the
markers follow the row-retirement half of today's discipline:
`gc.rs:365-381` clears `backup_ref`/`backup_commit` and drops rows
without a stash pair, and a row whose remaining content is markers only
retires at that existing edge — while a marker on a surviving
stash-bearing row (`S+R`; `B+S+R` before the field clearing) persists in
the archived record, inert thereafter. Markers contribute no cleanup-worklist
entries and never block backup-ref deletion, worklist derivation, or
archive deletion; the archived-cleanup worklist derivation must accept
marker-only rows (`collect_owner`'s all-None-pair rejection at
`record_wire/archive/cleanup.rs:163-167` gains a marker-aware arm — §5,
§8, §9). The extension is v0-inert: no v0 record carries markers, so the
`from_v0` leg of the shared derivation never sees the new arm.

### 2.3 v0 boundary and unknown-field survival

- **v0 never writes these fields.** No v0 code path constructs them, and
  absent-by-default encoding means every v0-written record byte stream is
  identical to today's, including v0 rows that carry preservation evidence
  (`model/v0.rs:158-164` is the shared row struct; its four existing fields
  and their encoding are untouched).
- **v0 collision rule (extends RecordContract §8).** Presence of
  `noop_commit` or `reset_commit` inside a v0 record's preservation
  evidence row makes migration ineligible; the value is never adopted,
  overwritten, or moved — the same doctrine §8 :369-373 applies to the
  five top-level v1 names. Mechanism, stated for the shared-struct
  reality: once §2.1 lands, the two names parse into **typed** fields even
  when a v0 record carries them — but manifest membership is governed by
  the **extractor's known-key set**, not the typed parse (raw-YAML
  extraction runs on the value tree regardless of what serde accepts).
  The evidence-row known-key set
  (`record_wire/unknown_fields/extract/common.rs:237`, today exactly the
  four old names) therefore **forks by version**: the v1 set adopts the
  two names; the v0 set must not — so in a v0 record the two names **do**
  surface in the v0 unknown manifest, and that manifest membership is the
  collision trigger, consumed by `map_v0_to_v1`
  (`record_wire/unknown_fields/mod.rs:64-81`, today the five top-level
  names only), which gains the in-row leg. Read-only and archival paths
  leave such bytes untouched, per the compatibility contract's
  byte-preserving rules.
- **Archived-v0 projection is unaffected.** Archived v0 records use only
  the archive decoder/projection (`GwzM5-8I2CompatibilityContract.md`
  :163-164) and no v0 record legitimately contains the fields; nothing in
  the archive projection changes.
- **Unknown-field survival.** The new fields live inside the container the
  §8 retirement table already governs; sequence identity remains "by its
  stable owner (`participant:<id>` or `publication-root`, with at most one
  row)" (`GwzM5-8I2RecordContract.md` §8 :394-396). Unknown descendants
  beside the new fields survive rewrites bound to that identity, exactly as
  today; the new fields survive archival with their row. Rows written by a
  post-amendment writer and reread by the current v1 decoder before the
  implementation lands would surface the two names as unknown fields — a
  non-case in practice, because no v1 writer is released (§5), but the
  unknown-field suites still pin the survival behavior (§8).
- **No protocol change.** Preservation evidence rows are not projected into
  status/field-10; the protocol contract's discriminants and strings are
  untouched (contrast the operator-escape train, which does carry protocol
  deltas — §6).

## 3. Semantics

### 3.1 What each field attests, and where it is written

**`noop_commit` — the per-owner no-op skip.** Attests: at the moment the
artifact pass (backup-ref position, then stash position) retired this
owner, every artifact position **not** evidenced by a durable artifact pair
on this row was proven exactly unnecessary by the same closed observations
the cursor uses today — `protected_commit == anchor` for the backup
position (`cursor.rs:247`) and an exact clean preservation image for the
stash position (`cursor.rs:279-282`; the clean/dirty fork the stash intent
already computes and then discards at `cursor.rs:344-349`, where a clean
owner is refused a stash). Written **write-ahead relative to cursor
advancement**: when the artifact pass proves owner *k*'s remaining
positions unnecessary, the dispatch persists owner *k*'s skip marker as its
one durable step — through the ordinary atomic
rewrite-publish-reread-verify discipline of RecordContract §8 — and only a
subsequent dispatch may classify, journal, or execute anything at a later
position. One record write per skipped owner, bounded by owner count.
Owners whose stash was actually created get **no** `noop_commit`: their
artifact-pass completion is decode-derivable (§3.2), and a stash pair
preceded by no backup pair entails the backup position was a no-op, because
the stash journal was only writable after that position passed
(`verify_pending_prefix`, `cursor.rs:124-137`).

**`reset_commit` — the reset completion bit.** Attests: this owner's reset
position is retired — the attached ref stands at the immutable owner anchor
with the reset position never to be revisited. Written at exactly the two
edges that today discard the fact:

1. **Executed reset:** in the same atomic rewrite as the reset journal's
   retirement (`finish_reset_attached_ref`,
   `transition/reduce/preservation.rs:202-236` — the write that today
   clears `pending_preservation` at :231 and requires an evidence-free
   payload at :221-223; at **whole-action** granularity the reset is the
   one action that retires with no durable trace anywhere — backup
   evidence installs at its own finish, stash ids at the `CreateStash`
   advance — while at arm granularity the stash finish arm is equally
   evidence-free by then). The ActionJournal's retirement rule
   ("Retirement occurs only with the verified result/progress write",
   :152-153) is unchanged; the bit rides that same write.
2. **No-op reset:** when the reset pass proves the position unnecessary
   live — no durable backup target beyond the anchor and
   `plan.live_commit == plan.anchor` (`cursor.rs:290-297`) — the dispatch
   persists `reset_commit` write-ahead, exactly like the skip marker.

**Marker backfill at both reset edges.** When the owner's row at a reset
edge carries neither `noop_commit` nor a stash pair — the shape a record
written before this amendment presents after §4's live re-proof, where the
retained pending action blocks any earlier marker write — the same atomic
rewrite that writes `reset_commit` also writes `noop_commit`, valued per
the §2.2 equations. The backfilled fact is not invented: at both edges the
owner's artifact-pass completion was live-re-proven in the same dispatch
(`verify_pending_prefix` sweeps all owners for a pending reset,
`cursor.rs:138-141`; the action-free reset loop runs only after the
artifact loop passes). One write, one durable step, and the write produces
only §2.2-legal shapes — `B+N+R` / `N+R`, never `B+R` or `R` alone. The
same discipline is uniform outside the reset edges: the action-free
artifact pass writes `noop_commit` for any marker-less owner it
live-proves, on degraded (pre-amendment) records exactly as on new ones —
so degraded records converge to marker-bearing rows as the cursor revisits
them, one write per owner per dispatch, and no write edge can ever emit an
illegal shape.

Mechanically these are evidence-only record rewrites inside `Preserving`:
no physical mutation occurs, so no pending action is journaled for them —
the durable write **is** the step. The transition vocabulary gains two
evidence-write reducer arms (working names `RecordArtifactNoop` and
`RecordResetNoop`; exact spellings are settled at implementation review)
plus the extended (up to two-field) footprint of
`finish_reset_attached_ref` — `reset_commit`, and `noop_commit` when the
backfill fires; the
TransitionDesign transition/footprint matrices gain the corresponding rows.
No state changes, no new predecessor edges.

### 3.2 Durable artifact pairs become decode-terminal for the prefix

Persisting the two markers alone would not retire the dominant cost:
`stash_complete` live-images an earlier owner **even when its durable stash
ids are present** (`cursor.rs:276-277`), because under the frozen text a
completion fact may have physically regressed and the wire could not say
so. This amendment therefore also fixes the prefix reading of the existing
fields: for cursor-prefix purposes, an earlier position is complete when
its durable evidence says so —

- backup position: durable backup pair present;
- stash position: durable stash pair present **and** no pending
  preservation action at that position (ids install mid-action atomically
  with phase advancement, so "ids present, action retired" is decodable —
  a crash between `CreateStash` and retirement leaves the pending action in
  the record and is classified by the action's own observer, unchanged);
- either artifact position: `noop_commit` per §2.2;
- reset position: `reset_commit` per §2.2.

With that, `stash_complete`/`reset_complete` stop capturing live images for
earlier owners (`cursor.rs:251-305` → `v1_preservation_image`,
`preserve/artifacts.rs:663-680`), and the root-reset carve-out at
`cursor.rs:264-275` — which exists only to stop the pending owner's **own**
stash-*position* clean check from rejecting a journaled intermediate root
form — dissolves into the general rule: the pending action sits at the
reset position, so that stash position reads decode-complete.
`VerifiedPreservationCursorPrefix` becomes decode-derived
over durable bytes, symmetric with rollback's cursor, with the live
fallback of §4 for bytes the durable facts do not cover.

### 3.3 INVARIANT — per-action live exactness at execution is retained

**Nothing this amendment adds ever authorizes a physical mutation from
record bytes alone, and no observation that today authorizes a mutation is
weakened.** The durable cursor removes exactly one class of live work: the
full-tree live-image re-proof of *earlier* owners per dispatch
(`cursor.rs:251-305` → `v1_preservation_image`). Retained unchanged:

- every pending action's own closed before/after observation at its own
  execution edge — stash preimage equality and the immediate pre-mutation
  recheck (`GwzM5-8R4bInterfaceAmendment-2.md` §5.3), reset
  `expected_commit`/`restore_commit` ref facts with exact clean forms
  (ActionJournal §4), backup-ref exact-target classification;
- the bundle-prefix verification and its live stash observation per
  stash-bearing owner (`checked_bundle.rs:152-171`,
  `cursor.rs:27,106`) — durable rows never substitute for live artifact
  bytes where an artifact is claimed;
- owner-plan derivation's live ref/commit reads (`preserve/plan.rs:397`);
- rollback-entry preflight and the full-tree per-member reverse observers
  (`abort/preflight.rs:69,83,98`, `abort/participants.rs:95-192`,
  `ensure_clean_recovery_state`), and terminal exactness.

Consequence: a regressed earlier owner — new user work, an external touch,
a mangled artifact — is no longer detected at every later position's
classification; it is detected at that owner's **own next action** or at
**rollback-entry preflight**, both of which still refuse fail-closed before
any mutation. Detection latency moves; the failure direction never does.
The trade is deliberate and is the U3 payoff: a transient ambiguity in a
skipped earlier owner no longer blocks classification of a later pending
owner with `PreservationEvidenceMismatch`
(`GwzM5-8OperatorEscapeDesign.md` §3.3 U3 row :160). Symmetrically, the
guard `reject_later_durable_owner` (`cursor.rs:47-62`) no longer fires for
an owner whose retirement is **durably recorded** — the wedge it produced
("preservation owner acquired new work ahead of a later durable owner") was
precisely the live-cursor artifact this amendment removes. For marker-less
earlier owners the guard is **retained as a live guard, unchanged**: an
absent (or artifact-only) earlier row beside a later durable row is a
legitimate pre-amendment shape — today's cursor live-skips a clean owner
writing no row at all (`cursor.rs:29-38`, :247, :279-282) — and only a
live fact separates it from regression, so that shape stays on the §4
live-fallback path and is never rejected from record bytes. What hardens
at decode is exactly the **marker-bearing** contradiction surface:
fabricated or mismatched marker values (the §2.2 equations),
marker/pending-action conflicts, a stash pair coexisting with
`noop_commit`, and `reset_commit` without artifact-pass evidence on the
same row — rejected by checked-open validation per the amended
TransitionDesign §6.7 (:746-749; §7.3's "marker contradictions") without
any repository read. A present-but-all-absent row remains invalid
independent of any later row (§2.2 row 1 — unchanged; it always was).

## 4. Restart legality

How restart consumes the cursor, position by position, for the record's
pending or next preservation work:

1. **Durable fact present** (artifact pair per §3.2, `noop_commit`,
   `reset_commit`): the earlier position is complete-or-unnecessary at
   decode. No live re-proof of that position is required or performed.
   Skip rows legalize skipping the re-proof of completed no-op positions;
   reset bits legalize skipping the re-proof of retired reset positions.
2. **Durable fact absent** (records written before this amendment lands;
   a crash after a position was live-proven but before its marker write;
   pre-A1 test records): today's full re-proof path applies **verbatim** —
   `protected_commit == anchor`, exact clean live image,
   `live_commit == anchor` — with today's outcomes, including ambiguity
   returning typed `PreservationEvidenceMismatch` with no rewrite and no
   mutation. This is graceful degradation, not a second semantics: the
   live observation, where it runs, is byte-for-byte the one the frozen
   text already mandates. A crash between a live pass and its marker write
   therefore re-proves once and re-writes the marker — idempotent, and
   never worse than the status quo. On a degraded record with a retained
   pending reset, the earlier-owner sweep stays live per dispatch
   (observation writes nothing) until the journal retires; the retirement
   write itself backfills `noop_commit` per §3.1, and subsequent
   action-free sweeps write the remaining markers — degraded records
   converge, and no reset edge can produce a §2.2-illegal shape or a stuck
   retirement.
3. **Contradiction** (**marker-bearing only**: the §2.2 illegal
   combinations and value mismatches, and marker/pending-action
   conflicts): rejected at decode as typed evidence mismatch; no rewrite,
   no physical mutation — the fail-closed arm is unchanged in direction
   and moves earlier (from live observation to record validation). An
   absent or artifact-only earlier row beside a later durable row is
   **not** a decode contradiction — it is the legitimate pre-amendment
   shape of item 2, handled there under the retained live guard (§3.3).

The pending owner's own intra-owner prefix (e.g. a pending stash requiring
its backup position complete, `cursor.rs:124-137`) keeps its current cheap
checks unchanged; the expensive earlier-owner sweep
(`require_artifact_complete`, `cursor.rs:225-239`; the all-owner sweep for
pending resets, :138-141) consumes durable facts first per §3.2.
Exhaustion (`cursor.rs:433-467`) is reached when every position is durably
or live proven retired; the `VerifiedPreservationExhausted` gate into
rollback (`GwzM5-8R4bReverseLifecycleInterface.md` :326-329) and the
rollback entry preflight behind it are untouched.

## 5. Compatibility

Stated per the F5 lesson of the exact-evidence amendment (its §3: a hash
frame kept while its coverage changed had to be declared
incomparable-by-design): **this amendment changes the coverage or meaning
of no existing hash, field, frame, or projection.** Verified surface by
surface:

- **Envelope.** `schema: gwz.merge-operation/v1`,
  `record_schema_version: 1` unchanged. Purely additive optional fields in
  one shared row struct; no version bump. No installed v1 compatibility
  promise is weakened because no released v1 writer exists — the v1 lane is
  production-disabled until A1 (`merge/mod.rs:23-24`, `plan.rs:396-397`;
  the same argument `GwzM5-8R4bInterfaceAmendment-2.md` §3.3 and the escape
  design §6.1 :561-563 already rely on). Landing pre-A1 is what keeps this
  a zero-migration change; after A1 it becomes the corridor/v2 problem the
  packet prices out (F5 §3.4 :216-219).
- **Existing row fields.** `backup_ref`, `backup_commit`, `stash_id`,
  `stash_object_id`: meaning, encoding, and every existing validation rule
  unchanged. Pre-amendment row shapes remain valid (§2.2 table).
- **Preimage frame.** `gwz.merge-preservation-preimage/v1` grammar, frame
  id, and coverage untouched — this amendment never touches hash input.
  No incomparability window is created (the exact-evidence amendment's
  discontinuity note remains the only one).
- **Bundle identity.** Canonical bundle derivation ignores evidence rows
  without stash ids (`checked_bundle.rs:146-151`), so skip/reset markers
  never enter bundle bytes; expected-bundle output is byte-identical with
  or without the new fields (§8 pins this).
- **Record digest.** None exists ("no record self-hash",
  `GwzM5-8I2RecordContract.md` §1 :48-50); authority proofs bind the live
  record digest per issue and are process-internal, not wire.
- **v0 / archived v0 / protocol.** Per §2.3: v0 writers never emit the
  fields, v0 byte streams are unchanged, archived-v0 projection is
  unaffected, migration ineligibility extends the existing collision
  doctrine, and no protocol field changes.
- **Terminal plane (archived cleanup and GC).** Marker-only rows are a new
  archived shape and the terminal plane must accept it. The cleanup
  worklist derivation's all-None-pair rejection arm (`collect_owner`,
  `record_wire/archive/cleanup.rs:163-167`, shared by `from_v0` and the
  A1-destined `from_v1`) gains a marker-aware arm: an `N`-only or `N+R`
  row is valid, contributes no backup refs and no stash evidence, and must
  not error — without this, one fully-noop owner would make **every**
  archived merge's worklist derivation fail, blocking all targeted cleanup
  (a permanent-retention U8-class growth). The post-GC rewrite consumes
  markers with their row at the existing retention edge
  (`gc.rs:365-381`; §2.2 terminal-plane fate). Both deltas are v0-inert:
  no v0 record carries markers. Tested per §8; owners priced in §9.

## 6. Interaction — the operator-escape amendment window

The operator escape design proposes its own pre-A1 I2 wire deltas — the
sixth top-level `operator_override` field, `Unrecoverable` per-owner
disposition rows, the `AbortOperationOverridden` transition, and protocol
appendments (`GwzM5-8OperatorEscapeDesign.md` §6.1 :544-559) — and states
that "If item 4's durable cursor amendment proceeds, these should travel in
the same amendment" (§6.1 :566-567; [Q3] :749-754: the escape
wire/transition amendment "rides pre-A1 (with the [F5 §9 item 4]
durable-cursor amendment if that proceeds)"). The decision packet's
sequencing adopts the shared train (§4 step 2).

Both trains, explicitly:

- **This amendment (durable cursor) stands alone.** It is complete,
  reviewable, and implementable with no reference to any escape-design
  field, transition, or protocol delta. Its wire surface (two fields inside
  the existing evidence row) is disjoint from the escape deltas (a new
  top-level field, a new transition variant, protocol appendments); no
  field, sentence, or invariant here depends on the escape text landing.
- **The escape-design wire deltas MAY share this amendment window.** If the
  escape amendment proceeds on schedule, the two texts ride one dual-review
  round and one matrix wave ("One amendment round, one matrix wave, two
  debts retired", decision packet §3.4) — they amend adjacent sentences of
  the same contracts, so a shared round also avoids a banner-stacking
  conflict. If the escape amendment slips, **this amendment proceeds
  regardless**: no hard coupling in either direction. The only shared
  object is the amendment window itself.

Decisions 1 and 2 of the packet are orthogonal here: cursor liveness uses
filter-aware dirty flags, not raw-byte equality, so the CRLF question does
not change this amendment's shape (packet §1.4, §3.4).

## 7. Contract deltas applied at acceptance

On acceptance, the amended documents receive banners dated with the
acceptance date, and the following exact text changes. This document is
controlling wherever the frozen texts say the wire persists neither fact.

1. **`GwzM5-8I2ActionJournalContract.md`** — banner: "Amended by
   `GwzM5-8DurableCursorAmendment.md`: the preservation evidence row
   persists per-owner no-op skips (`noop_commit`) and the reset completion
   bit (`reset_commit`); the preservation cursor prefix is decode-derived
   with live fallback." §1 gains the **complete six-field
   `PreservationEvidence` definition** — the shared row struct is printed
   in neither I2 contract today (it is inherited via RecordContract §1's
   unchanged v0 body and lives at `model/v0.rs:158-164`), so the
   acceptance-time edit introduces the full struct into §1's wire types,
   annotated that the first four fields are the inherited v0 fields and
   the last two are this amendment's. In §2,
   the sentence at :158-160 is replaced by: "Preservation persists per-owner
   no-op skips and reset completion as validated evidence-row derivations;
   a narrower live condition remains only where a durable completion fact
   is absent." The sentence at :161-163 gains "— by durable completion
   facts where present, and by exact bound live observation where absent —"
   after "must also prove". The closing sentence at :165-166 is replaced
   by: "This adds two evidence-row fields and no journal variant; the
   deterministic order is unchanged." §4 gains the two write edges of
   §3.1 of this amendment.
2. **`GwzM5-8I2RecordContract.md`** — banner of the same form. §8: the
   preservation-evidence retirement row extends to "the same row survives
   as ref/stash/no-op/reset fields fill and survives archival"; the
   collision doctrine extends to the two new names inside evidence rows
   (§2.3); §9 gains the exit-test rows of §8 of this amendment.
3. **`GwzM5-8R4bTransitionDesign.md`** — §6.7: the "I2 does not persist …"
   paragraph (:742-749) is superseded by the durable-facts reading
   (contradiction rejection now includes marker contradictions); the
   `VerifiedPreservationCursorPrefix` paragraph (:751-761) reads
   "an exact observation proving every earlier position complete or
   unnecessary, from durable completion facts where present and live
   observation where absent". Transition/footprint matrices gain the two
   evidence-write arms and the extended reset-retirement footprint (§3.1).
4. **`GwzM5-8R4bReverseLifecycleInterface.md`** — §6 :342-345: "only after
   every earlier live position is exactly complete or exactly unnecessary"
   becomes "only after every earlier position is exactly complete or
   exactly unnecessary — durably where recorded, live where not".

Cited, not edited: `GwzM5-8I2CompatibilityContract.md` (its eligibility
gates and archive projection are unchanged; the collision extension lives
in RecordContract §8), `GwzM5-8I2ProtocolContract.md` (no delta), the
escape design, and the decision packet.

## 8. Test obligations

Named future suites, owed by the implementation package (which may land
with A1 per the adopted decision); the contract-level rows join
RecordContract §9 at acceptance:

1. **Cursor round-trip** (`model/v1/tests.rs`,
   `model/v1/validate/preservation_tests.rs`): exact YAML spellings and
   absent-by-default encoding; every legal and illegal combination of the
   §2.2 table — now structurally exhaustive over all sixteen shapes,
   including `S+N+R` and `B+S+N+R` — with both value equations;
   **immutability of all six row fields across the new rewrite edges**:
   the whole-row `set_evidence` replacement
   (`reduce/preservation.rs:259-264`) makes constancy
   discipline-enforced, so the four inherited fields are pinned
   byte-constant across marker writes and the reset-retirement/backfill
   write, and the two markers across every subsequent rewrite;
   marker/pending-action contradictions; multi-row and owner-collision
   rejection unchanged.
2. **Restart with/without-cursor equivalence**
   (`v1_lifecycle/tests/prefixed_preservation.rs`): the same fixture driven
   to every cursor position, restarted (a) with durable markers and (b)
   with the markers stripped to the pre-amendment shape — stripping an
   `N`-only row deletes the row entirely, producing the legitimate
   absent-earlier-row shape, which must classify identically via the live
   fallback (never reject at decode); identical classification verdicts
   wherever live state is exact; the (a) path proven image-capture-free
   for earlier owners by backend call counting; crash injected between a
   live pass and its marker write re-proves live and converges (idempotent
   marker write); and (c) **degraded-record pending-reset retirement**: a
   marker-less record with a retained pending `ResetAttachedRef`, resumed
   by the amended binary — live re-proof, execution or After
   classification, retirement **succeeds** with per-case outcomes: a
   marker-less `B` row retires to `B+N+R` via the §3.1 backfill, and a
   marker-less `B+S` row retires to `B+S+R` (no backfill — the stash pair
   is the artifact-pass evidence); `N+R` arises only at the standalone
   no-op reset edge, never at retirement, because a pending reset
   requires the recorded backup target; no rejected write, no stuck
   state.
3. **Unknown-field survival rows**
   (`record_wire/unknown_fields/tests/preservation.rs`): unknown
   descendants beside the new fields survive by stable-owner identity;
   the two names present inside a **v0** record's evidence row surface in
   the **v0 unknown manifest** (the known-key set forks by version per
   §2.3: the v0 set does not adopt the two names) and that manifest
   membership triggers migration ineligibility, never adoption; the v1
   known set adopts the two names, so they never appear in a v1 manifest
   and the first marker write passes the overlay's unauthorized-unknown
   check; archival carries the fields with their row.
4. **U3 wedge-surface reduction demonstration** (lifecycle matrix rows):
   external dirt/touch on a durably-skipped earlier owner no longer blocks
   classification of a later pending owner (today: typed
   `PreservationEvidenceMismatch` per dispatch), while the same
   interference still refuses fail-closed at that owner's own next action
   and at rollback-entry preflight; a dropped native stash of an earlier
   owner is still caught by the unchanged bundle-prefix verification.
5. **Post-`reset_commit` preflight-only interference** (the
   highest-latency legal detection case): owner *k*'s no-op reset proven
   and `reset_commit` written; the branch then moves (or the worktree
   dirties) before restart. The reset position is decode-skipped and the
   owner has **no own next action left** — exhaustion proceeds, and
   rollback-entry preflight is the sole remaining catcher
   (`abort/preflight.rs` per-member full-tree observers): it refuses
   fail-closed with no mutation.
6. **Cleanup/GC over marker rows** (`record_wire/archive` and `gc`
   evidence): worklist derivation (`collect_owner`) over archived records
   whose rows carry `noop_commit`/`reset_commit` succeeds — `N`-only and
   `N+R` rows contribute no worklist entries and must not error — other
   owners' backup refs still enumerate, archive deletion proceeds once
   every recorded ref is observed absent, and the post-GC rewrite retires
   marker-only rows at the existing retention edge (`gc.rs:365-381`).
7. **Bundle-identity invariance** (`checked_bundle` evidence): expected
   bundle bytes identical for the same artifact set with and without
   skip/reset markers.

## 9. Budget and review routing

Decision-packet allocation (§3.3): ~700-1,100 LOC implementation + tests +
matrices; review tier "I2 interface amendment → mandated dual review".
This document routes to that dual review — **Code and State axes** — with
the packet, the cited contracts, and `cursor.rs` as review inputs. Expected
implementation owners (estimate, reconciled against
`GwzM5-8ChangeBudget.md` at implementation review, not binding here):
`model/v0.rs` (row struct), `model/v1/validate/preservation.rs`,
`v1_lifecycle/transition/reduce/preservation.rs`,
`authority/observe/reverse/preservation/cursor.rs` (and its `phase`
children), `record_wire/archive/cleanup.rs` (marker-aware worklist arm,
§5), `gc.rs` (marker fate at the existing retention edge, §2.2),
`record_wire/unknown_fields/extract/common.rs` (the evidence-row
known-key set forks by version: the v1 set gains the two names — without
which the **first marker write fails** the overlay's
unauthorized-unknown-field check, fail-closed and self-catching — while
the v0 set must not adopt them, or the §2.3 collision rule loses its
trigger), and `record_wire/unknown_fields/mod.rs` (`map_v0_to_v1` in-row
collision leg), plus the test owners named in §8. The two
`record_wire/unknown_fields` owners and the two terminal-plane owners are
flagged **now, ahead of the ChangeBudget reconciliation**. Persistence
traffic, counted precisely: at most one marker write per owner per pass —
one `noop_commit` write per skipped owner in the artifact pass, and one
standalone `reset_commit` write per **no-op** reset position (executed
resets ride the existing retirement write; the §3.1 backfill adds a field
to an existing write, not a write) — bounded by two writes per owner.
Implementation-review note (from the Code axis): marker immutability
across the whole-row `set_evidence` replacement
(`reduce/preservation.rs:259-264`, the one clone-and-modify rewrite path)
is discipline-enforced, not structural — the §8.1 pins plus the store's
known-diff/reread verification are the enforcement.

## 10. Status

ACCEPTED (history follows). Round 1 returned NO-GO on both axes;
revision 1 applied
every finding of `GwzM5-8DurableCursor-ReviewState.md` (P1-1 via the §3.1
marker backfill; P1-2 via the §3.3/§4.3 rescope to marker-bearing
contradictions with the retained live guard; P2-1 via §2.2/§5/§8.6/§9;
P2-2 via §2.2's structural rules; P2-3 via §8.5; P3-1/2/3 via
§2.3/§8.1/§9) and of `GwzM5-8DurableCursor-ReviewCode.md` (P2-1 — same
defect as State P1-2, aligned to §7.3's narrower rule, which is unchanged;
P3-1 via §3.1; P3-2 via §7.1; P3-3 via §9; P4 notes via §3.2/§9 and the §1
citation corrections). **ACCEPTED 2026-08-16**: the State re-verdict is GO
(all five pre-committed conditions resolved, both P1s closed by
construction) and the Code re-verdict is GO (P2-1 resolved exactly to the
prescribed correction, §7.3 byte-identical); Code's round-2 wording notes
R2-1/R2-2/R2-3 and State's P4-1 are applied in the banner-application
edit, which also applied the §7 deltas to the four contracts. The wire
delta is frozen **pre-A1** (banners dated 2026-08-16);
implementation may trail into the A1 package per the adopted Decision 3.
Review did not overturn the decision; the packet's recorded fallback
(§3.5: Option B, the cheap live probe, plus C-style closure of F5 §9
item 4) is moot and F5 §9 item 4 is closed by this acceptance.
