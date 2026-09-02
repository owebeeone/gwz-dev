# GwzM5-8 DR-1 E8.3 Reconciliation Design — peer-blind review, CODE/MECHANISM axis

Date: 2026-09-03. Reviewer: code/mechanism axis (peer-blind; the guarantee/state
axis is another reviewer's). Subject: `GwzM5-8DR1-Reconciliation-Design.md`
(DRAFT 2026-09-03). Tree read: gwz-core main **`d6830cd`** (the design cites
`ffd4f95`; its own note that no mechanism-cited file moved between the two is
consistent with what was read — every `file:line` below was checked at `d6830cd`
and matched the design's `ffd4f95` numbering). Time-box: 30 min, hard. Method:
reading only — no build, no suite, no probe driven, no file in `gwz-core/`
touched. Items not settled by the deadline are marked **UNVERIFIED**.

---

## Verdict

**GO-WITH-CONDITIONS** for adopting the design as E8.3's answer and chartering
S1. The load-bearing correction (§1.4) is **confirmed at the exact lines**. The
cure's *observer* half is sound; its *writer* half is missing, and the
transitions table's headline row is direction-inverted (P1-1 below). Neither
defect changes S1 (which is drive-and-record only) — they change what S2/S2b/S3
must be chartered to build, and they invalidate the "no `classify_table` row
edit" sizing assumption. Conditions are listed at the end.

---

## 1. The load-bearing correction (§1.4) — CONFIRMED

Order of evaluation on the E4.5-B residue (forward-pair authority + staged goal
under the marker's family; leaf absent), traced by reading:

1. `abort/evidence.rs:306-311` — pending `Marker` arm calls
   `artifact_facts::classify_remove(root, marker_path, candidate.marker_yaml)`.
2. `root/artifact_facts.rs:77-89` → `entry.rs:73-82`
   `classify_remove_merge_root_artifact` hard-codes `Bytes(expected)` →
   `classify_remove` → `classification.rs:89-101` `classify_remove_exact`
   (Bytes gate at `:93-99` passes) → `classify_exact(&Bytes(marker_yaml), None)`.
3. `classification.rs:125-143`: parent-open check `:130-136`,
   `parent_is_current` `:137-139`, **`inspect_family` at `:140`**, then
   **`if residue.foreign { return Ok(Ambiguous) }` at `:141-143`**.
4. `residue.rs:106-232` `inspect_family`: `action = self.action_key(expected,
   goal)` at `:115` (REVERSE key); `expected_authority_name` at `:117`; prefix
   filter at `:127` is `family_prefix` (direction-free, `authority.rs:230-232`)
   so the FORWARD residue **is enumerated**; the forward `.authority` fails
   `text == expected_authority_name` at `:166` and takes **`:179-181` `foreign =
   true`**; the forward `.goal` fails `goal_name(&family, &action, …)` at `:188`
   and `source_name` at `:198` and takes **`:204-206` `foreign = true`**.
   `authority` stays `None` (`:156`, never assigned).
5. `authority.rs:196-202` `family_key` hashes root identity + canonical path
   only; **`:204-223` `action_key` hashes `expected`'s discriminant+digest and
   `goal`'s** — direction lives entirely in `action_key`. Confirmed.

So the FIRST check that fires is `classification.rs:141-143`, with
`residue.authority == None`. `:155-174` and `:175-177` are unreachable in this
scenario. **The record's cite (`E45B-Report.md:35`, `execute.rs:71-78`, E8.1
§4.4 R3 paragraph — all read) names the wrong layer.** Confirmed as the design
states.

**A stronger fact the design does not state, and should (P2-1):** if the
`:141-143` gate were simply removed, this scenario would fall through to
`classify_table(ExistingRemove, authority=false, Absent, Absent, Missing)` =
**`After`** (`classification.rs:253`), not `Ambiguous`. I.e. the residue is not
merely invisible to `:175-177`; without the `foreign` gate the classifier would
already answer *the reverse operation is complete*. That is the correct answer
for an absent leaf — and it is the opposite of what the design's transitions
table prescribes (see P1-1).

## 2. The leaf × window matrix (§1) — verified with two gaps

Observer sites all match: `abort/evidence.rs` `file_states` `:269-323`,
`transition_file` `:325-332` (`Before|Recoverable → Candidate`, `After →
Baseline`, `Ambiguous → Other`), `classify_file` `:343-361` (`:356`
`missing_is_baseline`); `publication/live.rs::snapshot` `:94-` with the
`FileDigest::Regular(lock)` bail at `:110-112`, boundary `Missing → None` at
`:118-122`, `CandidatePublicationObservation::new` at `:123`; `regular_digest`
`:166-187` (no acquisition — `fs::symlink_metadata` + Sha256); `root/abort.rs`
`:122-135` / `:136-149` (`classify_write` on the pending step, raw `observe`
otherwise), writer `:380`; L11 `finalization.rs:439-458` `regular_file_equals`
with `NotFound → Ok(false)` at `:442`; L7 `preservation_root/files.rs:37-49`
one door both directions, called from `preservation_root.rs:477-482` /
`:483-488`, reader `observe_transition` `:51-` at `preservation_root.rs:207`,
refusal `:209-210`; L8 `checked_bundle.rs:64-112` (classify `:78-92`, write
`:93`, post-verify `:99-105`), header `:1` "Test-gated" stale — all confirmed.
The 16-door `entry.rs` inventory (`:36,:43,:53,:65,:73,:84,:92,:100,:109,:118,
:128,:139,:150,:313,:349,:375`) is complete — no other `pub(crate) fn` in the
file.

**Gap (P2-2): `abort/preflight.rs:110-118` is an unlisted observer of L5/L6.**
For `@root` selected aborts it does `artifact_facts::observe` on
`WORKSPACE_MANIFEST` and `LOCK_PATH` and returns `MergeRecoveryRequired` unless
both are `Bytes(_)` — **before** `root/abort.rs:123-145` is ever reached. A
detached manifest or lock (window (i) on L5/L6, "shipped, unaudited") is refused
here first. S3's `root/abort.rs` reconciliation alone cannot close L5/L6; the
matrix and S3 must name this door. (The design lists `preflight.rs:111` only
under §2.3 as a site that "moves with the signatures" — it does not, it is an
`observe` call with no `expected` parameter.)

**Gap (P3-1): §2.3's "13 external call sites that move" over-counts.** Of the
listed sites, `evidence.rs:82,:83,:285,:300,:314`, `root/abort.rs:131,:145`
and `preflight.rs:111` are `artifact_facts::observe` (no `expected`) and are
untouched by the `&[u8] → Option<&[u8]>` widening. The sites that move are
`evidence.rs:148,:154,:160,:277,:292,:307` and `root/abort.rs:123,:137,:380` —
**9**, not 13. Sizing of S2b is if anything smaller.

## 3. The cure (§2) — observer half sound, writer half missing

**Direction-free survey**: `family_key` `authority.rs:196-202` confirmed
direction-free; `family_prefix` enumeration `residue.rs:116,:127` confirmed;
`CheckedArtifactAuthority::decode` already called at `residue.rs:175`; the
`canonical_path_identity` carried in the record is path-derived bytes
(`identity.rs:166-`, component-wise; stored `authority.rs:27`, encoded `:131`,
decoded `:164`, capped 4 KiB `:165`), so a survey can resolve it. Sound.

**P1-1 — The transitions table (§2.4 A) is direction-inverted for the driven
row, and the writer gate makes every `Before` reconciliation un-executable.**

* Row "L2 marker, pending == Marker": the design reconciles an absent leaf with
  a counterpart forward residue to **`RFT::Before` ⇒ `FS::Candidate`**. The
  reverse operation is `Bytes(marker_yaml) → Missing`; its *pre*-state is
  "marker present", its *post*-state is "marker absent". The leaf **is absent**.
  The truthful reconciliation is `After` ⇒ `FS::Baseline` — which is also what
  the non-pending arm already says (`evidence.rs:356`, `missing_is_baseline`),
  and what `classify_table` `:253` would say absent the `foreign` gate (§1
  above). The design's own row 1 ("non-pending arm ⇒ Baseline — already
  tolerant") contradicts its row 2.
* Worse, `FS::Candidate` matches `marker_before` (`evidence.rs:250-252`,
  `[B,B,C,B]`), so `execute_v1_evidence_rollback` (`:125-172`) would run
  `artifact_facts::remove_exact(marker_yaml)` (`:160-164`) → `transition.rs:118`
  `remove_exact` → `classify_remove_exact` → `inspect_family` — which §2.2
  **keeps direction-bound by design** ("cannot be widened in place … widening it
  would weaken the writer") → `foreign` → `Ambiguous` → **`Err("removal
  evidence is ambiguous")` at `transition.rs:121-127`**. The same dead-end holds
  for the lock/boundary rows (`write_checked` → `replace_exact` →
  `classify_replace_exact` → `Ambiguous` → `Err` at `transition.rs:45-51`) and
  for L5/L6/L7's `Before` rows.
* **So the cure as written reconciles the observer and leaves the executor
  refusing.** RED-1 cannot turn GREEN on the §2.4 table. The writer needs a
  counterpart-aware arm: either (a) the reverse action *adopts and retires* the
  counterpart's residue under its own lease (restore the quarantined `.source`
  for `Bytes → Bytes` leaves, retire the staged `.goal` and `.authority`, then
  proceed), which is a new `ExactTransition` variant + new `classify_table`
  rows + the 232-line pin (`classification.rs:302-533`, verified) — exactly the
  edit S2b was chartered to avoid; or (b) the observer reconciles to `After`
  for absent-leaf cases and a separate retirement action (under the abort's
  already-acquired lease, I6) clears the residue. Either way §2.6 I2 ("the
  survey never repairs") is fine for the *survey*, but the design names **no
  actor that retires the counterpart residue**.
* **Consequence if unfixed (this is why it is P1, not P2):** for fixed-path
  families — lock (L3), boundary (L4), root manifest/lock (L5/L6), preservation
  leaves (L7) — an un-retired forward residue is `foreign` to every later
  action on the same family (`residue.rs:179-181`), so the *next* merge's
  forward `replace_exact` on the lock refuses at `transition.rs:45-51`. S3 as
  designed converts a one-merge wall into a **permanent** wall for L3-L7. (The
  marker's per-id path makes L2 the one leaf where the stranded residue is
  merely garbage.) I4's sweep predicate ("leaf matches neither endpoint") does
  not fire here — the absent leaf *equals* the forward action's `Missing`
  endpoint.

**Foreign residue adoption (I1)**: survey enumerates only under
`family_prefix(family_key)`; family_key binds root identity + canonical path
(`authority.rs:196-202`). Another root/path is not enumerated. Confirmed. The
ownership-token boundary text at `bootstrap/managed/provider.rs:51-61` is about
the managed-bootstrap resident record; its last sentence (`:59-61` — "a later
step that uses a resident record to decide adoption of state this action did
not create … must re-litigate the token then") is *exactly* what the survey
does, one subsystem over. **P3-2:** I1 should cite `:58-59` (same-user boundary
out of scope) explicitly rather than "untouched" — the guarantee reviewer's
call whether that suffices; noted, not duplicated.

**Record root stays the reconciliation root (I5)**: consistent; S5 is separate.

**Probes on listed operations (I6/I7)**: `file_states` already acquires a
`CheckedArtifact` on every arm via `artifact_facts::observe` →
`entry.rs:36-41` → `root_artifact` `:159-166` → `CheckedArtifact::acquire`
(legacy probe). Confirmed: S3 adds no acquisition. `service.rs:119-124` takes
`acquire_activated` at `:121` for `ResumeStart | Continue` only, `acquire`
otherwise. `snapshot` has 14 callers, all inside
`observe/finalization/publication.rs` (`:53,:88,:100,:123,:247,:256,:262,:272,
:366,:414,:441,:475,:522,:572`). **UNVERIFIED:** whether that module is reached
*only* under the activated lease (the design's chain via `finalization.rs:53`
→ `forward.rs:69` → `start.rs:89/:160` was not re-traced in the box). Note the
fallback holds regardless: a new `CheckedArtifact::acquire` in `snapshot` is
the *legacy* probe the abort already takes — not catalog activation — so the
ruling's red line is not crossed under either lease. No `acquire_activated`
appears in any cure call site.

## 4. The conversions (§3) — verified

`execute.rs:79` (marker, `write_atomic`), `:88` (lock), `:98` (boundary) with
the E4.7 residual comments at `:71-78`, `:82-87`, `:91-97` — confirmed; the
`:45/:48/:51` cites in the record are drifted as the design says. Record root:
`classify_open_record` at `store/mod.rs:216` (design says `:211-241`; fn at
`:216`), `discover_open` `:82`. A leaf-family survey keyed on the *leaf's*
canonical path cannot find a record whose id is unknown; S5's "scan all
`ca1-*` and decode for a `canonical_path_identity` under `.gwz/merge`" is the
only shape, and it is feasible because the identity is path bytes (above). The
family survey does not change RR §1a's ground (the leaf is its own discovery
root) — the analysis is right. **Conditional on P1-1**: the record root's
rewrite arm would ALSO need the writer-side counterpart arm, so S5's ~200 LOC
assumes S2/S3 already carry it.

## 5. The step plan (§7) — sizes plausible; two counts corrected

* `FileState` has 3 variants (`evidence.rs:175-179`); 11 predicates
  `:189-265` confirmed. **Shape literals: 29, not 35** (`initial_publication` 9,
  `boundary_before` 4, `boundary_after` 5, `lock_before` 2, `lock_after` 3,
  `marker_before` 2, `marker_after` 2, `index_before` 1, `index_after` 1). P3-3.
  The argument (a new variant forces a re-audit) stands.
* `classify_table` pin: `#[test] operation_tables_accept_only_the_declared_rows`
  `classification.rs:302-533` = **232 lines** confirmed.
* 806-LOC cross-check: spans re-summed — `live.rs` 122, `evidence.rs` 206,
  `artifact_facts.rs` 98, `classification.rs` ≈111 — plausible. **But P1-1
  moves work into `classify_table` + `transition.rs` + the pin**, which the
  cross-check excludes; S2+S3 should be re-budgeted with a `classify_table` row
  edit and pin re-pin assumed, and the stop-trigger reworded: the risk is a new
  `ExactTransition` variant, not (only) a new `FileState` variant.
* **P3-4 — §7 is internally inconsistent with §4.5:** I1 says "11 fns + 2
  structs + 4 members … ~450 removed" while §4.5 (and §0) say **12** functions
  and **≈350** lines removed. §4.5's figures are the measured ones.

## 6. The record corrections — verified

* `gc_archived` family = 12: `archive.rs` `gc_archived :145`,
  `gc_archived_with_hook :153`, `any_open_record_present :201`,
  `remove_archive :224`, `require_same_archive :244`, `require_real_directory
  :262`, `require_regular_file :274`, `io_error :290` (8); `gc.rs`
  `preflight_archived_cleanup :53` (design says `:31-130`; the fn is at `:53`,
  the doc block from `:31` — P3-5, cosmetic), `delete_preflighted_backup_refs
  :132`, `require_backup_refs_absent :144`, **`cleanup_error :169`** with its
  three uses at `:71,:88,:105` all inside `preflight_archived_cleanup` (4);
  `archive_result.rs` `:30,:37,:41,:49` (4 members). `preflight_archived_cleanup`
  has exactly one caller, `archive.rs:169` inside `gc_archived_with_hook`;
  `gc_archived` has no production caller. `tests/gc.rs`: 7 tests, six family
  (`:11,:27,:53,:89,:143,:194`), `:331` survives. **Confirmed.**
* Preservation bundle: exactly one checked writer,
  `reverse/execute/preservation.rs:74` → `checked_bundle.rs:93`; raw siblings
  `preserve/artifacts.rs:357` and `handle_stash/commands.rs:68,:86,:93,:122,
  :318,:327` (six, carve comment at `:67`). Preservation-root leaves: one door
  both directions (`files.rs:37-49` ← `preservation_root.rs:477-488`).
  **Confirmed — the hazard is at L7, not L8.**
* `catalog.rs`: `:44-48` = `)]`/`mod enumeration;`/`mod scratch;`/blank/comment;
  comment `:48-50`; `#[allow(unused_imports …)]` `:51-60`; `use` `:61-63`.
  **Confirmed stale, corrected span right.**

## 7. Interaction with E8.1 — consistent

E8.1 §4.4 note 1 (read): "Tiering admits more substrates, so more users reach
R2 and R3. The cures become more urgent, not different." The design's claim
that option (iii) is worst for this class follows directly. E8.1 §4.4's R3
paragraph carries the `:175-177` cite the design corrects — confirmed it is one
of the homes. E8.1 §4.4 rule 2's "enumerable, then degrade, never foreign,
never invisible" is the same principle P1-1 needs on the *writer* side.

---

## Findings

| # | Sev | Finding | Site |
| --- | --- | --- | --- |
| P1-1 | P1 | §2.4(A) reconciles absent-leaf + counterpart residue to `Before`/`Candidate`; truthful state is `After`/`Baseline`, and `Before` routes the abort into `remove_exact`/`replace_exact` whose direction-bound `classify_*` gate (kept by §2.2) refuses. No actor retires the counterpart residue ⇒ permanent `foreign` on fixed-path families L3-L7. Needs a writer-side counterpart arm (new `ExactTransition` + `classify_table` rows + pin) or an explicit retirement action under the abort's lease. | `transition.rs:43-51,:119-127`; `classification.rs:253`; `residue.rs:179-181`; `evidence.rs:250-252,:325-332,:356` |
| P2-1 | P2 | §1.4 should record that without the `foreign` gate the table already yields `After` at `:253` — it fixes the reconciled state's direction and simplifies S3. | `classification.rs:247-253` |
| P2-2 | P2 | `abort/preflight.rs:110-118` is an unlisted L5/L6 observer refusing a detached manifest/lock before `root/abort.rs` runs; S3 must widen it. | `preflight.rs:110-118` |
| P3-1 | P3 | §2.3 "13 sites move": 9 move; the 8 `observe` calls do not. | §2.3 |
| P3-2 | P3 | I1 should cite `provider.rs:58-61` and state why the same-user-boundary clause covers the survey's adoption decision (guarantee axis to rule). | `provider.rs:51-61` |
| P3-3 | P3 | Shape literals are 29, not 35. | `evidence.rs:189-265` |
| P3-4 | P3 | §7 I1 says 11 fns / ~450 removed; §4.5 says 12 / ≈350. Align to §4.5. | §7 vs §4.5 |
| P3-5 | P3 | `preflight_archived_cleanup` fn is at `gc.rs:53` (doc from `:31`); `classify_open_record` at `store/mod.rs:216`. | cosmetic |

## Tally

**Verified: 31** (§1.4 order of evaluation and all six lines; `family_key`/
`action_key`; 16-door inventory; every observer site in §1.2/§2.4; `transition.rs`
fork and gates; L7/L8 writer arms; six raw bundle siblings; `gc_archived` = 12
with `cleanup_error`; six family tests; `catalog.rs:51-63`; `FileState` = 3;
232-line pin; 806-LOC spans; `execute.rs:79/:88/:98`; E8.1 §4.4 note 1 and
rule 2; I6 no new acquisition; `service.rs:121` lease split;
`canonical_path_identity` is path bytes; §4.7 header falsehood).
**Refuted: 4** (P1-1 direction + executability; 13-sites count; 35 literals;
§7's 11/450). **UNVERIFIED: 2** (`snapshot` reachable only under
`acquire_activated`; RED-1's exact fault point — the patch was located,
`:369/:439/:42/:110` confirmed present, not read in the box).

## Conditions for GO

1. **Before S2/S2b charter:** amend §2.4(A) so an absent leaf with an exact
   counterpart residue reconciles to `After` (abort side), and add the writer's
   counterpart arm or a named retirement action to S2/S3 — re-budget with a
   `classify_table` row edit and pin re-pin, and reword the stop-trigger to
   cover `ExactTransition`.
2. Add `abort/preflight.rs:110-118` to the L5/L6 rows and to S3.
3. Fix the P3 counts (§2.3, §7 I1, shape literals) in the same revision.
4. S1 as chartered — drive-and-record only — is unaffected and may start now.

---

## Round 2 — confirmation of REVISION 2 (2026-09-03, code/mechanism axis)

Method: text diff of REVISION 2 (`GwzM5-8DR1-Reconciliation-Design.md`, 1488
lines, header "REVISION 2, 2026-09-03, folds the dual") against the findings
above. No new probe, build or tree read beyond the lines already cited at
`d6830cd` in Round 1. Time-box 15 min.

| Finding | Where folded (rev 2) | Status |
| --- | --- | --- |
| **P1-1** writer half + direction + retirement | §0 `:55-66`; §2.4(A) rows `:387-394`; **§2.5 `:422-532`** (new "THIRD COMPONENT — the reconciler"); I2 `:552`; I10 `:560`; S2b `:1308`; S3 `:1314`; stop-triggers `:1360-1373` | **FOLDED DIFFERENTLY — acceptable, with one S3 charter line (below)** |
| **P2-1** `:253` already answers `After` | §1.4 `:255-262`; §2.5 item 1 `:441-446` | FOLDED AS STATED |
| **P2-2** `abort/preflight.rs:110-118` | matrix L5 `:178`, L6 `:179`; transitions row `:391`; §2.5 `:526`; S3 `:1314` | FOLDED AS STATED |
| **P3-1** 9 moving sites, not 13 | §2.3 `:353-357` (names the eight `observe` calls as non-moving); S2b `:1308` "nine" | FOLDED AS STATED |
| **P3-2** cite `provider.rs:58-61` | I1 `:551`, rewritten jointly with State [P2-2] as a three-part exclusion proof; "untouched" withdrawn | FOLDED AS STATED (and beyond — the guarantee axis owns the proof's sufficiency) |
| **P3-3** 29 literals | `:1355-1358` with the per-predicate breakdown | FOLDED AS STATED |
| **P3-4** I1 = 12 fns / ≈350 | `:1330` | FOLDED AS STATED |
| **P3-5** `gc.rs:53` (doc `:31`), `store/mod.rs:216` | `:890`; `:420`, `:625`, `:1329` | FOLDED AS STATED (a leftover `:211-241` survives in the UNVERIFIED list at `:1429` — cosmetic) |

### P1-1, judged on the three questions asked

**(a) Does the pre-pass resolve the RED-1 analysis?** Yes for the driven fault
point. The direction is corrected to `After ⇒ FS::Baseline` for a
never-published leaf (`:387`, `:441-446`, I10 `:560` withdrawing REVISION 1's
"only ever converts to Before/Recoverable"); that matches
`classification.rs:253` and `evidence.rs:356` as Round 1 required.
`marker_after` (`evidence.rs:254-256`) then matches and the Marker step reads
`After`.

**(b) Does retirement-before-return resolve the permanent-wall regression?**
Yes. §2.5 `:498-505` times retirement *inside* `reconcile_counterpart` before
it returns, through the existing `cleanup.rs` primitives, all-or-nothing, on
the caller's already-held lease; the new §2.4 row `:394` ("the NEXT merge after
a reconciled abort") states the regression and its cure explicitly. For L3-L7
(path-stable families) this is the fix Round 1 asked for. (The `cleanup.rs`
line cites `:87/:89`, `:132/:134`, `:222/:224` are the design's §4.4 inventory,
not independently re-read in this box.)

**(c) Is "no `ExactTransition` variant, no `classify_table` row, no pin move"
honest?** Yes — because the reconciler runs *before* classification and leaves
the family empty, every existing row is correct verbatim; the writer's
direction-bound `inspect_family` stays as the writer's guard. The fallback
(option (a): variant + rows + re-pin) is named as S2b's stop-trigger and
budgeted at ~700 (`:1308`, `:1368-1373`) rather than assumed away. Honest.

**The one place the fold over-claims, and the charter line that closes it.**
§2.5 says the **marker class needs no reconciler** (`:447-451`; §0 `:62-63`;
"L2 marker … — nobody, because nothing detached" `:528-531`), and row `:387`
says `remove_exact` "classifies `After` and returns `Ok(())` without writing"
(`transition.rs:119-120`). The second sentence is not reachable as written:
`remove_exact`'s own `classify_remove_exact` re-runs the direction-bound
`inspect_family` on the still-present forward `.authority` + staged `.goal`,
takes `residue.rs:179-181`/`:204-206`, and returns `Ambiguous` at
`classification.rs:141-143` ⇒ `Err` at `transition.rs:121-127` — `:253`'s
`After` is only reached once the counterpart family is **gone**. Two
consequences: (1) if the abort planner skips execution on an observed `After`
(likely, but not traced here — UNVERIFIED), RED-1 goes GREEN and the marker's
forward residue is simply **stranded** in `.gwz/checked-artifacts` (harmless
for a per-id path, but permanent — I4's predicate does not fire because the
absent leaf *equals* the forward `Missing` endpoint); (2) the sibling fault
point — crash **after** `publish_goal`, before `finish_replace` (leaf =
`marker_yaml`, forward authority still present) — is the reverse op's *pre*
state, reads `Before ⇒ Candidate ⇒ marker_before`, routes into `remove_exact`,
and dead-ends exactly as Round 1's P1-1 described. Both vanish if S3 runs the
same per-leaf pre-pass on the marker's `CheckedArtifact` too — a
**retire-only** invocation (nothing to converge). S3 `:1314` already says
`file_states` calls the reconciler "once, up front, before computing any arm";
it only needs to say *for all three leaves*, and §2.5's "who restores … L2 —
nobody" should read "nobody restores; the reconciler still retires". This is a
two-line charter fold, not a design change; the component exists.

**One line for the guarantee axis, not judged here:** a crash between §2.5's
step 2 (restore-rename of `.source`) and step 3 (retire) leaves leaf-restored +
authority-present; "all-or-nothing" needs the re-entry rule for that state
named in NEW-2b.

### Round 2 tally

FOLDED AS STATED: 7 (P2-1, P2-2, P3-1, P3-2, P3-3, P3-4, P3-5).
FOLDED DIFFERENTLY, acceptable: 1 (P1-1 — pre-pass + retirement instead of a
writer arm; carries the marker retire-only condition into S3's charter).
NOT FOLDED: 0.

### Round 2 verdict

**CONFIRMED** — adopt REVISION 2 as E8.3's answer; S1 is charterable now (S1 is
drive-and-record and is untouched by any of the above). Carry into S3's
charter: *the `reconcile_counterpart` pre-pass runs on every leaf `file_states`
acquires, the marker included, as a retire-only act where nothing detached.*
