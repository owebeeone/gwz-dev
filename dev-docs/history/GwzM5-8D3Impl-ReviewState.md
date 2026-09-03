# STATE-AXIS REVIEW — D3 durable preservation-cursor implementation (round 1)

Date: 2026-08-22. Axis: **STATE** (adversarial, round 1 of a two-round cap;
program-level dual gate — the preservation cursor is durable-recovery kernel
territory). Object: `git -C gwz-core diff 0b059e8 -- src/workspace_ops` —
36 files, +1,229/−100 — **plus one untracked file the stated object does not
capture**: `src/workspace_ops/merge/model/v1/validate/preservation_marker_tests.rs`
(304 lines; its `mod` declaration rides the diff at `validate/mod.rs:63`,
the file itself is `??` in `git status`). Reviewed as part of the D3 delta;
the object-framing gap is finding P2-1. Other lanes' uncommitted
`checked_artifact` files were not read and are out of scope. Peer-blind:
`GwzM5-8D3Impl-ReviewCode.md` was not read.

Contract set, as reviewed: `GwzM5-8DurableCursorAmendment.md` (ACCEPTED
2026-08-16, end to end) and the four annotated contracts **as they stand
today**, carrying both the D3 annotations and the 2026-08-22
operator-escape annotations (accepted): `GwzM5-8I2ActionJournalContract.md`
(banner :7-21; composed §2 sentences :204-215 with escape replacements
:217-251; §4 write-edge annotation :450-461),
`GwzM5-8I2RecordContract.md` (banner :40-41; in-row collision :447-448;
retirement row :464; §9 exit rows :521-528),
`GwzM5-8R4bTransitionDesign.md` (§6.7 :817-849 + escape gain :851-856),
`GwzM5-8R4bReverseLifecycleInterface.md` (§6 :358-362). Also consumed:
`GwzM5-8OperatorEscapeAmendment.md` §7 items 2/3 (the composition
jurisdiction), `GwzM5-8M5bNoFfDesign.md` :876-884 (order-independence
instruction, State P2-2 of that review), `CurrentProgramCheckpoint.md`
(D3 adoption :545-552), and the store/authority substrate
(`v1_lifecycle/store/rewrite.rs`, `authority/binding.rs`, `preserve/plan.rs`,
`merge/gc.rs`, `preserve/checked_bundle.rs`).

Executed evidence (this host, macOS, CARGO_TARGET_DIR per assignment):

- **Four M5b filters** (`v1_lifecycle::tests::{forward, no_ff_determinism,
  reverse_no_ff, no_ff_wire}`): **37 passed / 0 failed** — matches the
  implementer's 37/37 claim; none of the four suite files appears in the
  D3 diff (unmodified over `0b059e8`).
- Targeted D3 + F-1 sweep (marker validation, unknown-fields, archive
  cleanup, prefixed/reverse preservation, gc, journal vocabulary, effect,
  authority, dispatcher reconciliation, store, g23 continue_v0_gate /
  gc / preserve / characterization_preservation_v0): **211 passed / 0
  failed**.
- **Full `cargo test --lib`**: **1,466 passed / 0 failed / 1 ignored**
  (866.85s).

---

## 1. Mandate 1 — cursor semantics as durable truth (§2.2; crash windows)

**§2.2 invariants, as implemented.** `validate_markers`
(`model/v1/validate/preservation.rs:129-166`): rule 1 (S+N never coexist)
at :139-141; rule 2 (R requires N or S) at :144-146; the `noop_commit`
equation (recorded `backup_commit` when the backup pair is present, else
the immutable owner anchor) at :148-159; the `reset_commit` = anchor
equation at :161-165; `is_oid` on both. The empty row stays invalid
independent of markers (:121-127). Pending-action cross-checks: pending
`BackupRef` vs `noop_commit` (:192-196), pending `Stash` vs `noop_commit`
(:303-307), pending `ResetAttachedRef` vs `reset_commit` (:249-253).
Immutability: every marker write edge runs
`marker_write_preserves_prior_row` (`reduce/preservation.rs:316-341`,
called at :244-247 for the reset retirement and :303-305 for the two
standalone arms) — the four inherited fields byte-constant, a present
marker may neither change nor disappear, and a row created by a marker
write may carry markers only. The untracked
`preservation_marker_tests.rs` pins the **structurally exhaustive
sixteen-shape table** (:100-116), both value equations, non-oid rejection,
the three cross-checks, exact spellings/declaration order, and
absent-by-default encoding.

**Value-equation consistency by construction.** The writer's values come
from `marker_row` (`cursor.rs:362-393`): `row.backup_commit` else
`plan.anchor`; `plan.anchor` for `reset_commit`. The validator's values
come from `owner_anchor` (`validate/preservation.rs:396-419`). Both sides
derive from the identical record fields with identical precedence —
participant `resulting_commit`; `"@root"` = `publication.composition_commit`
else `resulting_commit` (`preserve/plan.rs:423-431` vs
`validate/preservation.rs:401-410`); publication root =
`composition_commit` (plan.rs:464 vs :415-418). No live input enters
either side, so a legal writer cannot produce a §2.2-illegal value.

**The write pipeline is validated-before-publish.** `store/rewrite.rs`
`commit()` — lease + base-digest check (:29-33), source-byte re-read and
identity check (:35-39), footprint `verify_known_diff` (:41-43), unknown
overlay (:45), temp write + fsync (:49-55), **staged reread through
`StoredV1Record::from_open_bytes` → `validate_v1_record`** (:56-71;
`checked.rs:65-100`) — the full §2.2 surface gates the bytes **before**
`rename_durable` publishes (:78) — then post-publish reread + verify
(:87-95). Consequence: **no §2.2-illegal shape can ever become durable
through this pipeline**; an illegal reduce output dies at the staged
reread with the temp removed (fail-closed wedge, no brick, no torn state).
Publication is a single atomic rename: after any crash the record is
complete-old or complete-new bytes — the RecordContract §9 atomicity row
(:541-542) — so **every crash window restarts to a single-valued cursor
state**.

**The three write edges, window by window.**

1. *Standalone noop emit* (`RecordArtifactNoop`; observer
   `cursor.rs:44-46` → `artifact_noop_intent` :395-421; reducer
   `record_marker` :274-314). Crash before publish → old bytes; restart
   re-proves live (§4 item 2, byte-for-byte today's checks:
   `backup_complete` :263-265 tail, `stash_complete` :303-307 live image,
   `reset_complete` :320-334) and re-derives the **same** marker value
   (record-derived anchor/backup_commit are immutable) — idempotent
   re-write. Crash after publish → decode-complete at restart.
2. *Executed-reset retirement with backfill* (`FinishResetAttachedRef`;
   observer `phase.rs:418-431` computes the evidence at `R::Complete`,
   `backfill = !(noop present ∨ stash pair)` :423; reducer `finish`
   Action::Reset now **requires** `reset_commit` in the payload
   (:234-240) and prior-row preservation :244-247; footprint extends to
   {pending_preservation, owner evidence row}, `footprint.rs:264`). The
   bit and the backfill ride the one atomic retirement rewrite — no torn
   half exists on disk at any instant. Crash before → pending reset
   retained at its durable phase; restart re-observes the retained journal
   (the action's own observer, unchanged) and re-issues the identical
   retirement write. Crash after → R durable, journal gone. Rule 2 can
   never be violated across this edge: the backfill fires exactly when
   neither N nor S is present, so the output is `B+N+R`/`N+R`/`B+S+R`/
   `S+R`, never `B+R`/`R` — and the store validates it pre-publish anyway.
3. *No-op reset emit* (`RecordResetNoop`; observer `cursor.rs:52-56` →
   `reset_noop_intent` :423-449; same reducer arm). The R value (anchor)
   was live-proven `live_commit == plan.anchor` in the same dispatch
   (`reset_complete` :322-326). Crash before → live re-proof at restart:
   if the branch is still at the anchor, the same write re-issues; if the
   branch moved, `reset_complete` is false and `reset_intent` refuses
   fail-closed ("reset owner has no durable backup target",
   `cursor.rs:538-540`) for a no-backup owner — a typed
   `PreservationEvidenceMismatch` at the owner's **own** position, no
   mutation. Live-state divergence between the check and the publish
   (branch moves at T1.5) leaves R truthfully claiming the *position*
   retired while the *world* regressed — exactly the contracted §3.3
   latency move, caught at rollback-entry preflight (mandate 3 below).

**The hunted window — two dispatches deriving different prefixes.** Not
constructible for D3-native records, on four independent grounds: (i) the
derivation is a pure function of (record bytes, live state), and every
consumer reads the one atomically-published file; (ii) durable markers are
immutable and monotone (only added), so any position once decode-retired
answers identically in every later dispatch; (iii) each proof is
digest-bound (`binding.rs:44-51` `same_record` = `source_digest` equality;
`:94-106` also hashes the payload) and `commit()` re-checks
`rewrite.base_digest() != current.source_digest()` (:29) plus source-byte
identity (:35-39) — a stale prefix/intent can never apply over newer
bytes; (iv) the write-ahead rule (mandate 4) forbids any same-dispatch
consumption of new evidence. The only prefix-verdict flips possible are on
the **live-fallback** legs of marker-less degraded records — which is §4
item 2's contracted "today's outcomes verbatim", re-verified per dispatch,
and D3's own writer closes that surface by converging every visited owner
to a marker-bearing row (one write per owner per dispatch,
`cursor.rs:38-46`).

Verdict on mandate 1: **holds**.

## 2. Mandate 2 — decode-terminal prefix (§3.2) and the §3.3 rescope

Per-kind forms as implemented (`cursor.rs`):

- backup position: durable backup pair **or** `noop_commit`, else live
  `protected_commit == anchor` (:255-266) — §3.2 bullets 1 and 3;
- stash position: `noop_commit` (:290-293); or durable stash pair **and**
  no pending preservation action *at that position* — implemented as no
  pending `Stash` for this owner (:273-281, :294-302); else the verbatim
  live image (:303-307). A pending `ResetAttachedRef` sits at the reset
  position, so a stash pair beside it reads decode-complete — **the
  dissolved root-reset carve-out**, now the general rule (the old
  root-handoff special case at pre-D3 `cursor.rs:264-275` is gone, and its
  answer is reproduced: same true);
- reset position: `reset_commit` (:316-318), else the verbatim live forms
  (:320-334).
- The earlier-owner sweeps consume exactly these predicates:
  `require_artifact_complete` (:239-253) inside `verify_pending_prefix`
  (:113-170; the all-owner sweep for pending resets :152-166), so pending
  and action-free paths derive one prefix.

**Adversarial rows constructed and answered:**

- *Marker-bearing owner behind a marker-less one* (later N-row, earlier
  marker-less dirty owner): unreachable from D3's writer — the artifact
  loop is strictly in-order and cannot pass owner *i* without first
  returning *i*'s stash intent or noop write (`cursor.rs:29-47`), so a
  later owner's N-row entails every earlier owner already carries
  stash-or-noop. Hand-fabricated instances hit the **retained live guard**
  `reject_later_durable_owner` (:61-76), which counts any later row
  (marker rows included) as durable: typed mismatch, no mutation —
  precisely §3.3's "retained as a live guard, unchanged" for marker-less
  earlier owners, and TransitionDesign §6.7's "an absent or artifact-only
  earlier row beside a later durable row is not a decode contradiction; it
  remains on the live re-proof path" (:829-830).
- *Regressed durably-skipped owner*: N durable, then worktree dirt →
  backup/stash decode-complete; reset position live-proves (its own
  position), R written; exhaustion; rollback-entry preflight refuses —
  mandate 3. N durable, then branch move → plan-derivation ancestor gate
  ("rewound or diverged", `plan.rs:671-681`) for non-descendant moves, or
  the own-position reset refusal (:538-540) / preflight for descendant
  moves. Never a mutation, never an inverted direction.
- *Torn backfill*: impossible — the backfill is a field of one atomic
  rewrite (edge 2 above); there is no durable instant carrying R without
  its same-write N/S witness, and rule 2 validation would refuse the bytes
  pre-publish anyway.
- *Is there a durable shape where a marker-bearing owner is NOT complete,
  falsifying the guard's rescope?* No. N attests both artifact positions
  retired at its write instant, R the reset position; regression after the
  write is the contracted U3 latency move, not incompleteness of the
  *cursor* — and every **contradictory** marker-bearing shape (S+N, R
  alone, value mismatch, marker/pending conflicts, non-oid) rejects at
  decode with no repository read (`validate_markers` + cross-checks;
  pinned by the sixteen-shape suite). The one decode-legal shape whose
  markers can be *silently dropped* is fabrication-only (P3-2): a
  stash-bearing marker row paired with a pending earlier-position action
  no real writer can journal (an S-without-B row entails
  `protected == anchor` at stash time — record-derived and immutable — so
  `backup_complete` stays true and no backup is ever journaled for it).

Verdict on mandate 2: **holds**; the §3.3 rescope's unreachability claim
is true for every writer-reachable shape.

## 3. Mandate 3 — the U3 demonstration

**The two §8.4 tests, verified semantically:**

- `reverse_preservation/faults.rs:104-172`
  (`later_pending_owner_cannot_advance_after_completed_prefix_regresses`,
  rewritten): earlier owner `mem_a` durably complete (stash pair,
  decode-terminal per §3.2), regressed with new work; the resumed run now
  **advances the later owner on its own exact observations** and stops
  `Stopped(RecoveryRequired)` — not the old per-dispatch
  `PreservationEvidenceMismatch`. Asserted: the new work file survives,
  `mem_a`'s evidence is byte-constant, and `mem_a` is **not re-stashed**
  (stash count still 1) — no artifact fabricated over user work.
- `reverse_preservation/invariants.rs:100-165`
  (`new_work_in_an_earlier_skipped_owner_is_rejected_before_later_evidence_mutates`,
  rewritten): `mem_a` retired durably as a marker-only row; interference
  after; the resumed pass converges markers, exhausts, and refuses at
  **rollback-entry preflight** — `MergeRecoveryRequired`, member `mem_a`,
  message contains "rollback before nor after state". Asserted: `mem_a`'s
  row carries **no fabricated artifact** (all four inherited fields None),
  `mem_b`'s stash pair survives, the new work file survives.

**My own adversarial sequences (beyond the tests):**

1. *N durable → branch moves (descendant) → restart, R not yet written*:
   plan derivation passes (`protected_commit` falls back to `live_commit`
   for a no-backup owner, `plan.rs:660-664`, making the
   neither-protected-nor-anchor gate vacuous there; the anchor-ancestor
   gate :671-681 passes for descendants); artifact positions decode-skip;
   `reset_complete` false; `reset_intent` refuses — "reset owner has no
   durable backup target" (`cursor.rs:538-540`), typed
   `PreservationEvidenceMismatch` (`preservation.rs:131-134`), **no
   mutation, work intact**. Note the precision: this mismatch is at the
   regressed owner's **own** next position — §3.3's "detected at that
   owner's own next action ... refuse fail-closed" — not the removed
   cross-owner wedge. Pre-D3 the same state would have journaled a backup
   of the moved head and then **executed a reset over it**; post-D3 the
   machine refuses instead. The direction moved *toward* refusal, never
   toward mutation.
2. *N+R durable → branch moves (non-descendant)*: plan derivation refuses
   ("rewound or diverged") before any classification.
3. *N+R durable → worktree dirt (the §8.5 shape)*: both passes
   decode-skip; the owner has **no own next action left**; exhaustion
   proceeds; `exhausted()` (`cursor.rs:577-611`) runs
   `preflight_entry_with_handoff` **inside the same observation**, so the
   refusal lands before `BeginRollback` mutates anything — the sole
   remaining catcher refuses fail-closed. Mechanism code-verified; the
   named §8.5 pin is missing (P1-1).
4. *Dropped native stash of a durably-complete owner*: still caught by the
   unchanged bundle-prefix verification — `verify_bundle_prefix` at
   `cursor.rs:27` / `verify_pending_bundle_prefix` :120, over
   `checked_bundle.rs`'s live stash observation (:146-171). Durable rows
   never substitute for live artifact bytes where an artifact is claimed
   (§3.3 bullet 2, retained).

**No inversion found.** In every sequence the failure direction is
refusal-before-mutation; the only outcome class that changed is the
contracted one (cross-owner wedge → later-owner progress + terminal
refusal with user work intact). No path fabricates an artifact (pinned by
both §8.4 tests and `phases.rs:36-59`'s "a skip marker must never
fabricate a preservation artifact").

Verdict on mandate 3: **holds** (with the §8.5 pin owed — P1-1).

## 4. Mandate 4 — write-ahead dispatch rule (§3.1)

"Only a subsequent dispatch may classify anything at a later position" is
enforced at four independent layers:

1. **Observer**: `observe_cursor` returns the marker intent immediately
   (`cursor.rs:44-46`, :52-56) — one durable step per dispatch, no
   observation of later positions in the returning dispatch.
2. **Reducer**: `record_marker` (`reduce/preservation.rs:274-314`) is
   action-free by construction — requires `pending_preservation` None on
   **both** old and new records and `payload.pending` None (:287-291), the
   canonical position (:299), the claimed marker present (:300-302), and
   prior-row preservation (:303-305). It cannot journal, advance, or
   retire anything.
3. **Footprint**: `RecordArtifactNoop`/`RecordResetNoop` expect exactly
   the owner's evidence row and reject any pending-journal presence
   (`footprint.rs:208-216`); `FinishResetAttachedRef` expects exactly
   {pending_preservation, owner row} (:264).
4. **Binding**: the proof binds the source digest (`binding.rs:44-51`),
   and `commit()` re-checks digest and source bytes — the new evidence is
   only ever consumed by a dispatch that re-loaded the published record
   (`rewrite.rs:87-96` returns the validated published state; the
   dispatcher loop re-enters observation on it — exercised end-to-end by
   `phases.rs:36-59`, which drives marker writes through `run()` to
   `Terminal(Aborted)`).

**Semantic side of the flagged performance regression**: none. The extra
dispatch per skipped owner adds one evidence-only rewrite between
positions; position retirement order is unchanged (same loops, same
in-order early returns); the marker rows project into no protocol field
(no `status`/field-10 change anywhere in the diff), never enter bundle
bytes (`checked_bundle.rs:148-151` skips rows without stash ids), and
change no classification outcome wherever live state is unchanged. Where
live state *did* change between the marker write and a later dispatch, the
behavioral delta is exactly the contracted U3 trade. Write count matches
§9's pricing: at most one `noop_commit` write per skipped owner plus one
standalone `reset_commit` write per no-op reset — bounded by two writes
per owner; executed resets ride the existing retirement write. Cost is
A1-pricing lane, not blocking here.

Verdict on mandate 4: **holds**.

## 5. Mandate 5 — terminal/GC plane and erratum discipline

**Zero-code-change claim verified**: `merge/gc.rs` is not in the diff; the
retain logic stands verbatim at `gc.rs:365-381`. Trace of every marker
shape through `retain_remaining_stashes` (:375-381): backup pair cleared
unconditionally; rows retained iff a full stash pair remains. So `N`-only
and `N+R` rows retire with their row at the existing edge; `B+N`/`B+N+R`
become marker-only after the clearing and retire the same way; `B+S+R`/
`S+R` survive as `S+R` with the marker riding the archived row, inert
thereafter — exactly §2.2's terminal-plane fate. The archived-cleanup
worklist derivation accepts marker-only rows
(`record_wire/archive/cleanup.rs:163-173`): they contribute no worklist
entries, never block backup-ref enumeration or archive deletion (pinned by
the three new tests in `archive/tests/cleanup.rs:364-438`, plus the
v0-inert empty-row pin :443-465). The shared-arm shape (one `collect_owner`
for `from_v0` and `from_v1`) is what the amendment §5 specifies verbatim.

**Erratum discipline**: no durable rewrite of markers exists or is
implied — markers are written at the three defined edges only, are
immutability-checked at each, and are consumed by GC solely via the
pre-existing row-retirement edge. No post-hoc marker repair path was
added anywhere in the diff.

Gap: the **post-GC-rewrite half of §8.6 has no test** — no fixture drives
marker rows through `post_gc_record` (the diff's gc test changes are
fixture field additions only). Folded into P1-1's itemization (weight:
P2-grade — the trace above is short and the edge is untouched code).

Verdict on mandate 5: **holds**, pin owed.

## 6. Mandate 6 — composition sweep

- **F-1 gate byte-identical + green**: the T-6 "v0 forged-action resume
  gate" surfaces (`merge/continue_op/*`, `tests/g23/continue_v0_gate.rs`)
  are absent from the diff — byte-identical over `0b059e8` — and the gate
  suites ran green inside the 211-test sweep (including
  `v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`
  and `v0_resume_rejects_forged_no_ff_mode_row`).
- **M5b 37/37 green, unmodified**: run above; the four suite files are
  untouched by the diff. The frozen design's order-independence
  instruction (`GwzM5-8M5bNoFfDesign.md:876-884` — P-REV suites "must not
  assert the absence of preservation-cursor rows and must tolerate the
  adopted D3 wire landing first") is satisfied in the delivered direction:
  D3 landed first and the M5b suites pass over its rows.
- **Escape-annotation composition (special jurisdiction)**: the escape §7
  item 2 replacement makes the earlier-position proof a three-source
  disjunction — "by durable completion facts where present, by an
  operator-override preservation-side disposition on that owner where one
  exists, and by exact bound live observation where absent"
  (ActionJournal :235-242) — and item 3 adds §6.7's "a durable
  operator-override mark retires its owner's remaining positions ... for
  cursor-prefix purposes only; it never satisfies
  `VerifiedPreservationExhausted`" (TransitionDesign :851-856). **D3 is
  the exact mark-free base case**: the predicates implement arms one and
  three verbatim and read no disposition. Marks are not implemented
  anywhere (no `operator_override` reads), and they are not made
  unimplementable: the disposition arm slots into the three per-position
  predicates (`cursor.rs:255-335`) as an additional early return over a
  record field D3 never touches; `VerifiedPreservationExhausted` issuance
  has exactly one site (`cursor.rs:577-611`) for the escape's
  no-dispositions gate; the wire surfaces are disjoint (in-row marker
  names vs the escape's sixth top-level name — D3's `map_v0_to_v1` in-row
  leg matches `identity.kind == "preservation_evidence"` only,
  `unknown_fields/mod.rs:80-97`, leaving the top-level five-name list
  untouched for the escape to extend); and no D3 validation rejects a
  record for carrying escape content. Non-contradiction also holds in the
  other direction: neither §8.4 test constructs an exhaustion proof on a
  disposition-bearing record (none exist yet).
- **v0 line byte-identical**: both fields are
  `#[serde(default, skip_serializing_if = "Option::is_none")]`
  (`model/v0.rs:164-176`); marker-less rows serialize byte-identically
  (pinned by `markers_are_absent_by_default_on_the_wire` and the
  pre-amendment round-trip pin); no v0 construction site sets them
  (`preserve/artifacts.rs:382-390` writes None); the g23 v0
  characterization suites are green. The **migration-ineligibility surface
  is extended exactly as contracted and otherwise untouched**: the
  evidence-row known-key set forks by version
  (`extract/common.rs:12-47`; V0 keeps the four inherited names, V1 adopts
  the two markers), the v0 manifest surfaces in-row markers and
  `map_v0_to_v1` refuses on them (`unknown_fields/mod.rs:80-97`), a
  marker-less v0 record still migrates, and the v1 set adoption is what
  lets the first marker write pass the overlay's unauthorized-unknown
  check — all five behaviors pinned in
  `unknown_fields/tests/preservation.rs:149-247`.
- **Exact-evidence banners**: untouched. The live fallback consumes the
  same `v1_preservation_image` predicate the exact-evidence amendment
  governs (private-area exclusion, blob-exact recovery edges); D3 neither
  reads nor writes that boundary, and no preimage-frame or hash surface
  appears in the diff.
- **Protocol contract**: no delta anywhere in the diff (nothing in
  status/field-10 territory) — §2.3's "no protocol change" holds.

Verdict on mandate 6: **holds** (subject to P2-1's object-framing repair).

---

## 7. FINDINGS

**P0 — none.** No durable-state unsoundness and no composition break was
found. Single-valued restart holds at every write edge; the decode-terminal
derivation answers the amendment's forms on every adversarial shape
constructed; the failure direction never inverts; the terminal plane and
the four-contract composition (D3 + operator escape) are clean.

**P1-1 — the amendment's own acceptance suite is partially undelivered
(named §8 obligations / RecordContract §9 exit rows :521-528).**
Delivered: §8.1 (sixteen-shape table, equations, spellings, encoding,
write-edge immutability pins), §8.2(c) at reducer granularity
(`prefixed_preservation.rs:271-334` B→B+N+R retirement backfill;
`journal_vocabulary.rs:169-289` B+S→B+S+R; plus the rule-1 no-backfill pin
:1589-1635 of the diff), §8.3 (all five rows), §8.4 (both tests), §8.6's
worklist half. **Missing, with no trace under any vocabulary in the
tree**: (a) **§8.2(a)/(b)** — the same fixture restarted with markers vs
markers-stripped classifying identically, the (a) path proven
image-capture-free for earlier owners **by backend call counting**, and
the crash-injected idempotent marker re-write ("re-proves live and
converges"); (b) **§8.5** — the post-`reset_commit` preflight-only
interference case (the highest-latency legal detection window; mechanism
code-verified in §3 above, pin absent; RecordContract §9 :527 names it
verbatim); (c) **§8.6's post-GC half** — no fixture drives marker rows
through `post_gc_record` (`gc.rs:365-381`); (d) **§8.7** — bundle-identity
invariance with and without markers (`checked_bundle.rs:148-151` verified
by reading; the §5 compatibility bullet says "§8 pins this" and nothing
does). (a) and (b) are the STATE-axis core — they are the executable form
of the §4 "no second semantics" degradation guarantee and the §3.3
latency-move guarantee; without them those claims rest on this review's
code-reading alone. (c) and (d) are P2-grade individually but belong to
the same contract row. **Remediation (test-only; no production defect
found, no production line need change)**: land the four legs as named
suites in their §8-designated homes (`prefixed_preservation.rs`,
lifecycle matrix rows, gc evidence, `checked_bundle` evidence), or obtain
an explicit program-level re-scope parking them as named debts with a
checkpoint row — silence is not a disposition for contract exit rows.

**P2-1 — the review object does not capture the delta it claims.**
`model/v1/validate/preservation_marker_tests.rs` (304 lines — the §8.1
suite) is **untracked**, so `git -C gwz-core diff 0b059e8 --
src/workspace_ops` compiles only because the tracked `mod` declaration
(`validate/mod.rs:63`) references a file outside the object. Any
downstream consumer of the same object spec (the Code axis, the acceptance
ritual, a future bisect) sees a delta that does not build or a suite that
silently does not exist. Remediation: bind the file into the delta
(`git add`) before acceptance/parking so the "complete D3 delta" claim is
true of one artifact; re-state the object as diff + the named file until
then.

**P3-1 — the edge-2 backfill of §3.1 is implemented as structurally
vacuous, not as written.** `reset_noop_intent` passes
`marker_row(record, plan, /*noop*/ false, /*reset*/ true)`
(`cursor.rs:433`), while amendment §3.1 prescribes the backfill "at both
reset edges". It is genuinely unreachable-needed: the reset loop runs only
after the artifact loop has given every owner stash-or-noop
(`cursor.rs:29-57`), and rule-2 validation would refuse a bare-R output
pre-publish if a future refactor ever broke that order (fail-closed wedge,
never corruption). Remediation: either mirror the retirement edge's
backfill for symmetry, or pin the loop-order invariant with a comment +
debug assertion at `reset_noop_intent` so the vacuity stays a theorem
rather than an accident.

**P3-2 — fabrication-only marker-immutability gap at the non-marker
evidence edges.** `backup_done` (`phase.rs:268-285`) and `stash_evidence`
(`phase/evidence.rs:48-62`) construct whole rows with `noop_commit`/
`reset_commit` hard-None; a hand-fabricated, decode-legal record pairing a
marker-bearing stash row with a pending earlier-position action (e.g.
`S+R` row + pending `BackupRef` — decode-legal since the row has no
recorded backup target and no noop) would have its `reset_commit` silently
dropped by the finish write. Unreachable from any real writer
(S-without-B entails `protected == anchor`, record-derived, so
`backup_complete` never journals a backup there), and D3 *narrowed* the
class (the noop cross-checks kill the N-bearing variants at decode).
Remediation option for closure: extend the §2.2 cross-checks with
"pending `BackupRef`/`Stash` contradicts a stash pair or `reset_commit`
on the same row", making the whole family decode-dead.

**P3-3 — minor consistencies.** (a) `faults.rs`'s rewritten test keeps the
name `later_pending_owner_cannot_advance_after_completed_prefix_regresses`
while now asserting that the later owner **does** advance and the run
stops at `RecoveryRequired` — rename to match the U3 semantics.
(b) The `from_v0` archived-cleanup leg now accepts fabricated
marker-bearing v0 rows (shared arm, amendment-conformant per §5's own
wording, but the §5 "v0-inert" phrase describes legitimate writers, not
the acceptance surface — worth one clarifying word at the next docs pass).
(c) Gross delta ≈1,533 diff lines + 304 untracked ≈ 1,433 net new lines vs
§9's ~700-1,100 allocation — ChangeBudget reconciliation at implementation
review is §9's own provision; reconcile the row.
(d) The §6.7 "working names" `RecordArtifactNoop`/`RecordResetNoop` are
implemented verbatim — this review records the spellings as settled.

---

## 8. VERDICT

**NO-GO (round 1).** The production implementation is, by this axis's
reading, **sound**: every crash window restarts to a single-valued cursor
state; the decode-terminal derivation is the amendment's, including the
carve-out dissolution and the mark-free base case of the escape-composed
contract text; the U3 direction never inverts on any sequence constructed;
the write-ahead rule is enforced at four layers; the terminal plane is
zero-code-change as claimed; M5b (37/37), the F-1 gate, and the full suite
(1,466/0) are green. The NO-GO is carried **solely** by P1-1 — the
amendment's own §8/RecordContract-§9 acceptance suite is missing its two
STATE-critical legs (§8.2(a)/(b) equivalence + call-counting +
crash-idempotence, §8.5) plus two P2-grade pins (§8.6 post-GC half, §8.7)
— together with the P2-1 object repair.

Pre-committed re-verdict conditions (a focused round-2 GO follows on
their satisfaction, no fresh full pass required):

1. The four P1-1 legs landed green in their §8-designated homes — or an
   explicit program-level re-scope row parking them as named debts.
2. `preservation_marker_tests.rs` bound into the delta (P2-1).
3. P3-1/P3-2/P3-3 dispositioned (fix, pin, or recorded note — reviewer
   accepts any explicit disposition).

---

# ROUND-2 FOCUSED RE-VERDICT — 2026-08-22

Axis: STATE (same reviewer). Scope: my round-1 findings only, per the
two-round cap. Object as re-framed: the tracked diff vs `0b059e8` in the
live tree — now 41 files, +2,192/−103, with the two previously-untracked
test files intent-to-add staged (` A` in `git status`) and therefore
inside the diff. Inputs: the implementer's round-2 report (per-finding
disposition table, P3-2 rationale, gate tails), plus direct reads of every
round-2 production delta and the two new test files, plus fresh runs on
this host. `src/checked_artifact/**` churn (other lane) not gated on.

Runs executed this round: durable_cursor + gc_tests + faults + invariants
**20/0** (9.42s); marker/cleanup/unknown-fields/prefixed **37/0**; M5b
four-filter **37/0** with all five M5b files `git status`-clean
(byte-identical); F-1 gate **3/0**. Not re-run, with cause stated: the
576s `root_fault_matrix` (implementer's partition B ran it against the
prebuilt release binary with `find -newer` attribution; no D3-relevant
source postdates it) and the full lib suite (round-1's 1,466/0 plus this
round's focused deltas plus the implementer's partitioned 565/0 cover the
composition surface).

Accuracy correction to my own round-1 record: the round-1 "211 passed"
targeted sweep's `v1_lifecycle::tests::reverse_preservation` and
`v1_lifecycle::tests::gc` filters matched zero tests — those modules mount
at `v1_lifecycle::reverse::preservation::tests` and
`v1_lifecycle::archive::gc_tests` respectively. Round-1 conclusions are
unaffected (the 1,466/0 full run executed them, and the semantic analysis
was by direct reading), but the filter names in that section are wrong as
written; this round ran the correct mounts.

## Per-finding disposition

**P1-1 — RESOLVED.** All four legs plus the §8.6 post-GC pin are
delivered with real substance, in §8-designated homes, and green:

- **§8.2(a)/(b)** —
  `durable_cursor::restart_with_and_without_durable_markers_classifies_identically`
  (`reverse_preservation/durable_cursor.rs:100-149`): the same two-owner
  fixture driven end-to-end (a) with `N+R` rows and (b) stripped per
  §8.2's exact instruction (an `N`-only row deletes entirely); identical
  `Terminal(Aborted)` dispositions asserted; the (a) path proven
  **image-capture-free** (`marked_captures == 0`) at the real capture
  seam, with the (b) path as the paying control (`> 0`). The counter is a
  `#[cfg(test)]` thread-local whose single increment sits inside the
  already-`#[cfg(test)]` `v1_preservation_image`
  (`preserve/artifacts.rs:663-690`) — zero release-production lines;
  thread-local is the right scope for the parallel harness. Accepted
  reading of "driven to every cursor position": the (b) leg live-proves
  every position, the (a) leg decode-skips every position, and the crash
  leg below restarts from the intermediate first-marker position — the
  three predicates have no further position-distinct behavior.
- **§8.2 crash** —
  `a_crash_between_the_live_pass_and_its_marker_write_reproves_and_converges`
  (:154-213): first durable write captured, crash injected by restoring
  the pre-run bytes (a faithful model of the atomic-rename-not-durable
  window — complete old bytes), restart converges to the same terminal
  and re-writes the byte-identical marker value. Idempotence pinned.
- **§8.5** —
  `interference_after_reset_commit_is_caught_only_by_rollback_entry_preflight`
  (:224-286): `N+R` pre-installed, then the attached ref moves to a
  **descendant** — and the fixture geometry makes the catcher identity
  structural, not merely asserted: `integrated_fixture` builds
  `protected` as a child of `result` (= the anchor)
  (`reverse_preservation/mod.rs:80-95`), so the plan-derivation ancestor
  gate provably passes and nothing before exhaustion-side preflight can
  refuse. Asserted: non-`Aborted` fail-closed outcome, branch untouched,
  no backup ref, no stash — the highest-latency legal detection case,
  with the preflight's identity additionally message-pinned by the
  invariants.rs U3 test ("rollback before nor after state").
- **§8.7** —
  `expected_bundle_bytes_are_identical_with_and_without_markers`
  (:293-378): a genuine stash from a driven pass stopped before the
  reset; bundle bytes written and file-compared byte-equal without
  markers vs with `S+R` on the stash owner and `N+R` on the other.
- **§8.6 post-GC** —
  `gc_tests::post_gc_retention_retires_marker_only_rows_and_keeps_markers_on_stash_rows`
  (`v1_lifecycle/tests/gc.rs:328+`), driven directly through
  `post_gc_record`. The enabling seam is a visibility widening to
  `pub(in crate::workspace_ops::merge)` with the body untouched
  (`merge/gc.rs:365-381` diff verified) — accepted. Its new doc comment
  also records a genuine erratum the round surfaced: `post_gc_record`
  shapes the GC **response projection** only; no post-GC durable record
  rewrite exists (the archive is deleted at `store.gc`). That is strictly
  stronger than my round-1 mandate-5 reading — terminal history is
  byte-immutable, so the erratum discipline ("no durable rewrite
  implemented or implied") holds doubly. The one-word amendment erratum
  belongs to the docs pass.

**P2-1 — RESOLVED.** Both files are intent-to-add staged and inside
`git diff 0b059e8 -- src/workspace_ops` (41 files, +2,192/−103,
self-contained). Staged content confirmed as the reviewed content:
`preservation_marker_tests.rs` is 303 lines / 13 tests with the round-1
census (sixteen-shape table, both equations, three cross-checks, wire
pins) and re-ran green; `durable_cursor.rs` (452 lines) was read in full
this round.

**P3-1 — RESOLVED.** `reset_noop_intent` (`cursor.rs:423-445`) now
computes the backfill predicate —
`is_none_or(|row| row.noop_commit.is_none() && row.stash_id.is_none())`,
De-Morgan-identical to the retirement edge's — instead of hard-coding
`false`, with the loop-order theorem documented at the site. §3.1's "at
both reset edges" is now literal; behavior today unchanged (the predicate
is provably always false under the current loop order).

**P3-2 — CLOSED; the implementer's adjudication is ruled CORRECT and
adopted.** My proposed decode-reject ("pending `BackupRef`/`Stash` beside
a stash pair or `reset_commit` on the same row") is withdrawn: my round-1
premise that the family was fabrication-only was wrong. `plan.rs:660-663`
derives `protected_commit` as `backup_commit` **else `live_commit`** for
a no-backup row, so an `S`/`S+R` owner whose branch later moves to a
descendant legitimately re-enters the backup position on the live
fallback and journals a `BackupRef` — the exact shape §4 item 2 requires
to keep "today's outcomes verbatim"; my `reset_commit` half would have
decode-bricked it, and the stash-pair half would have contradicted a
pre-existing D3-unchanged shape. The actual defect I named — **silent
marker erasure** at the whole-row artifact-edge successors — is
eliminated at both edges: `backup_done` carries both markers via
`prior_marker` (`phase.rs:274-281`, `:397-407`) and `stash_evidence`
carries them at `evidence.rs:56-59`, so **no durable state can lose a
marker and §2.2 immutability now holds at every write edge**. Residual,
recorded and non-blocking: on the reachable `S+R`-plus-descendant-move
sequence, the carried `reset_commit` composes with `backup_done`'s
**pre-existing** stash-pair drop into a §2.2-illegal `B+R` that the
staged reread refuses pre-publish — a fail-closed, escape-lane-recoverable
refusal replacing what round 1's code did on the same sequence (silent
`R` erasure followed by a machine-initiated ref reset). Strictly the
safer direction. Named follow-up for the docs pass / next package, not
this object: carry the stash pair across `backup_done`'s successor too
(yielding legal `B+S+R` and restoring progressive retirement there),
and/or amend §3.2's normative list before any decode-side hardening —
the same deferral the implementer recorded against the Code axis's
prose-overstatement finding.

**P3-3 — RESOLVED.** (a) Rename landed:
`regressed_completed_prefix_refuses_fail_closed_without_wedging_later_owner`
(`faults.rs:86`); the old identifier no longer exists in the tree —
settle-record mapping as stated. (b) Closed in code, stronger than the
docs word I asked for: the `CleanupMarkers::{Unknown,Known}` version fork
(`record_wire/archive/cleanup.rs:58-80`, arm at `:201-206`) makes the
"v0-inert" sentence true of the acceptance surface — on the v0 leg a
fabricated marker never legitimizes an otherwise-empty row
(`ContradictoryEvidence`, byte-identical to `0b059e8` behavior; §2.3
"the value is never adopted"), while the v1 leg keeps the §5 marker-aware
arm; pinned by the new cross-leg test, and the v1 variant is correctly
dead code until A1. One-word docs note rides the docs pass. (c) Budget,
recorded here for the settle record's ChangeBudget row: object over
`0b059e8` = **+2,192/−103 across 41 files**, split **production +621/−50**
— comfortably inside §9's ~700–1,100 allocation read against production —
and **tests +1,571/−53**, of which the §8 acceptance suite alone is 755
lines; the overage is test mass purchased by the round-1 P1, priced to
the row. (d) The spellings `RecordArtifactNoop`/`RecordResetNoop` stand
settled.

## RE-VERDICT

**GO (round 2).** Grades: P1-1 resolved; P2-1 resolved; P3-1 resolved;
P3-2 closed as adjudicated, with the recorded residual and named
follow-up; P3-3 resolved on all four sub-items. The round-2 production
deltas are confined to the reviewed set (marker carry-across, computed
backfill, cleanup fork, one visibility widening, one `#[cfg(test)]`
counter, one test-only re-export — `preserve.rs` diff verified) and none
alters any round-1 soundness conclusion; the M5b surface is byte-identical
and 37/0; the F-1 gate is 3/0; the amendment's §8 acceptance suite now
exists in full and is green. No round-1 finding remains open.
