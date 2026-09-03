# GwzM5-8 R2-E Capability-Free Amendment — STATE axis review (round 1)

Date: 2026-09-02. Axis: **STATE — record coherence** (exact quotes,
defensible readings, no orphaned or contradicted claim anywhere in the
estate after this lands). Peer-blind: the Code axis was not seen. Tier:
amendment-tier dual, Fable. Read-only everywhere; no build, no test, no
commit, no tree edit — this report is the one write.

Object: `GwzM5-8R2E-CapabilityFreeAmendment.md` (DRAFT, 2026-09-02), the
operator ruling it executes (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`,
verbatim on disk), and the estate it touches. Tuple as read: gwz-dev root
`cd2f665` with the working tree carrying the 2026-09-02 RecordRoot root-side
folds (plan, E7-Acceptance, checkpoint modified, uncommitted) and three
untracked files (the amendment, the ruling, the E4.4 charter prep); gwz-core
main `7f28907` (E4.2's landing); `e4/e4-3b-record-root` shows no diff against
main (E4.3-B not yet written on that branch — consistent with "in flight";
nothing in this review depends on it).

## VERDICT: GO-WITH-CONDITIONS — 0 P0 / 1 P1 (blocking, curable by fold) / 7 P2 / 8 P3

The amendment's core is ruling-faithful and record-sound: the list stands
untouched (§2), the carve-out is per-arm at the ruling's own granularity
(§3), the tier-2 deferral is a coherent "explicitly defer" (§5), and the
abort-sentence disposition (§6) is the ruling's own "fix the sentence", not a
stealth list amendment. What blocks adoption as drafted is one bullet — the
E4.7 re-scope — which silently drops content the ruling did not touch and
contradicts five adopted records; and the §7 home list, which owns 11 of the
50 corpus hits by name and leaves 34 orphaned. Every cure is a text fold
(dated annotations under L1-08's precedence-trail rule) plus one line of
operator visibility; none re-opens the ruling. Round 2 is owed only on the
[P1-1] fold and the extended §7 list; the two-round cap is not at risk.

---

## 1. Mandate 1 — RULING FIDELITY

### 1.1 The §0 excerpt against the verbatim file — EXACT where it quotes; elisions marked; nothing widened

Checked clause by clause against `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`
:9-27. Every quoted run is character-exact ("Closed. (A) — Ground 2 stands.
Ordinary merge / commit / create / abort / GC stay capability-free. Ext4-only
was for the checked feature, not "gwz dies on Fedora." Do not amend the list
to put those operations on the catalog probe."; point 2 verbatim in full;
point 4 verbatim; "Do not start E4.4–E4.6 as originally chartered."; the
closing "Terminal NO-GO … to unblock a step."). The `[…]` elisions drop:
"Now."; the tension's object in point 1; the colon clause of point 3 (which
§4 then uses in substance); the parentheticals of point 5 and "Not four
builders hitting the wall at delivery."; the FAT32 sentence's tail; the
abort-sentence paragraph; the launch-now items; "After the amendment GO …
Park the E4.4 conversion candidate …"; "Quote this ruling in the amendment
and in the E4.3-B / GC briefs." None of the elisions changes the sense of a
quoted run. The ruling's first line "D), with FAT32 out of product and out of
the lab." is not in the excerpt but is carried at §2 in substance.

### 1.2 Later paraphrases — each checked; one widening found (the P1), one tightening ruled defensible

| Amendment text | Ruling text | Ruling |
| --- | --- | --- |
| §2 "ext4-only is the CHECKED feature's posture (ratified 2026-09-01), not gwz's" | "Ext4-only was for the checked feature, not 'gwz dies on Fedora.'"; plan :448 "ext4-only Linux posture 'fine for now' (2026-09-01)"; checkpoint :2702 | faithful |
| §2 "binding on every later charter in the phase: no step may put a listed operation on the probe to complete a row" | "Do not amend the list to put those operations on the catalog probe." | faithful (the boundary's identity probe at `observation.rs:96,105` is the same capability class as the catalog's; §1 establishes the link) |
| §3 "only arms provably reached ONLY under `acquire_activated` … convert, each in its own reviewed step with the RecordRootAmendment §4 lease-reachability enumeration in its charter" | "Convert only arms already on `acquire_activated`." | **tightening, DEFENSIBLE** — an arm reached under both leases converted would put the probe on the plain lease, which point 2 + "Do not amend the list" forbid; the RR §4 clause (not revoked) supplies the proof duty |
| §3 exception: writers "that execute from `gwz repo create`, `init-from-sources`, an ordinary merge, `gwz commit`, merge abort (either form), GC, or the mutation guard" | "Ordinary merge / commit / create / abort / GC stay capability-free." | faithful to the RULING, but `gwz commit` is not on §5.2's frozen list by name — the ruling's word "commit" resolves the prep's §7.7(2) narrow/broad ambiguity in the broad direction, and that reading is unrecorded → [P2-1] |
| §4 E4.7: "E4.7's remaining content is the allowance expiry and the phase close-out records. E4.7 does NOT retire the carved writers." | "Re-scope O1, the R2-D milestone, and E4.7: checked-feature writes go through the boundary; capability-free arms are a dated exception, not unfinished work. E4.7 does not retire those writers." | **WIDENING** — "those writers" are the capability-free arms; the ruling does not drop E4.7's first clause (the legacy in-place writers) → [P1-1] |
| §5 "Nothing in DR-1's agenda starts inside E4 (ruling point 5)" | "Do not start (C) inside E4." + the routing of all three to one round | faithful (routing to a phase-end round entails not starting inside E4) |
| §5 tier-2 "EXPLICITLY DEFERRED to the design round below" | "Mint or explicitly defer the tier-2 archive sub-surface (do not invent it in E4.4)." | faithful; the deferral target is the lane's choice → §6 of this report |
| §6 the scoped sentence + dated residual | "Fix the sentence (scope it to the activated-lease / capability-free abort you actually shipped, or date the residual)." | faithful and MORE than asked (does both). Note: it scopes by PUBLICATION STATE (`abort/evidence.rs:39-45` is the tree's gate), not by the ruling's loose "activated-lease" phrase — that is the tree-accurate scoping, not a deviation; the amendment should say so in one clause |
| §7 pins package + convertible forward arms + GC fix + E4.7 | "After the amendment GO: pins package + any remaining activated-lease forward arms only." + "Launch now (standalone …): GC …; E4.3-B continues … Unaffected." | faithful |
| §7 "No E4.4 conversion candidate exists (only the prep report); nothing is parked" | "Park the E4.4 conversion candidate if it assumes …" + the ruling file's lane reading :32-33 | faithful |

### 1.3 §6 — RULED: NOT list-amendment-by-stealth, on two grounds, with one condition

**Ground 1 — the operator directed the sentence fix.** The ruling's abort
paragraph is explicit: if the post-publication path is real, "the E4.1
'`--abort` needs no such filesystem' line is over-claim. Fix the sentence".
The path IS real (`abort/evidence.rs:148-165` → `root/artifact_facts.rs:39`
`write_checked` → `entry.rs:159-166` `root_artifact` → `CheckedArtifact::acquire`
→ `durable_identity` at `observation.rs:96,105` — every link read this
session). §6 does exactly the permitted thing, both branches of the "or".

**Ground 2 — the list's own text already admits the refusal.** §5.2's
decision sentence (`SemanticsAmendment-DRAFT.md:913-917`) has two clauses:
"… abort, GC and the mutation guard continue to work; **a checked-artifact
action refuses, typed**." A post-publication v1 abort performs checked-artifact
actions (the evidence rollback) — the second clause governs those actions.
§6's reading ("the listed operations do not acquire the catalog; a
post-publication abort re-reads evidence the checked merge itself wrote") is a
READING that reconciles the two clauses; no operative word of §5.2 moves, and
the prep's §7.3 recorded the same fact ("the capability-free list is ALREADY
narrower than its text") before the ruling. That is the RecordRoot §2
"read through" device applied to §5.2, and it is the sanctioned shape.

**Condition — the reading must live at §5.2's home.** As drafted the
narrowing exists only inside this amendment; §5.2's "abort … continue[s] to
work" stands contradicted by the tree with no annotation where a reader of
the frozen text would see it. That is the orphan class this axis exists to
catch → [P2-1]. With the dated reading bracket at §5.2, §6 is a record, not
an amendment; without it, a future charterer reading §5.2 alone would be
misled in exactly the way the ruling says not to mislead users.

One precision note, not a finding: §6's residual clause says the narrowing
"shipped with A1's v1 reverse-path conversion". The E4.1 review's R2 row
(E4.1-Review :818) drove the LEASE on FAT32 (PR-3), not `abort/evidence.rs`'s
boundary calls; the prep §7.7(1) says the same. §6's "never driven on a
non-admitted filesystem" is therefore correct and the dated residual is
honestly stated.

## 2. Mandate 2 — FROZEN-TEXT EXACTNESS

| Quote in the amendment | Home | Result |
| --- | --- | --- |
| §1 "`repo create`, `init-from-sources`, ordinary merge, abort, GC and the mutation guard" (in quotation marks) | `SemanticsAmendment-DRAFT.md:913-917`: "an ORDINARY merge, `gwz repo create`, `init-from-sources`, abort, GC and the mutation guard continue to work" | **NOT VERBATIM** — reordered, `gwz` dropped, case changed → [P3-1] |
| §1 "Option (i) would cost a Linux user every mutation, not just the checked ones … turns a filesystem-capability gap into a total loss of gwz on that filesystem." | :948-949 (Ground 2 heading) and :964 ("Option (i) turns a filesystem-capability gap into a **total loss of gwz** on that filesystem") | EXACT across the ellipsis (bold dropped, acceptable); the cited range ":909-960" stops short of :964 → [P3-1] |
| §1 "`WorkspaceMutatorLock`'s nine (now ten) production sites" | :950-962 ("**nine** production call sites *[census moved to TEN at E4.1(c) …]*") | faithful |
| §1 "the durable-identity capability is required only where the catalog is consumed" | :909 heading "probe only where the catalog is consumed" | faithful paraphrase |
| §1 table's five row labels; §3's per-row names | `GwzM5-8R4bR2ConsumerCheckpoint.md:275-279` | row NAMES exact; the rows' second/third cells are never quoted → the cells are undispositioned, [P2-2] |
| §3 exception: row `:280`'s "artifact actions"; "no legacy raw writer" | :280 "same purposes and artifact actions \| test-gated until A1; no legacy raw writer" | EXACT fragments |
| §1 and §4: Phase E4 heading "milestone: production writes go through the boundary" | plan :331 | EXACT |
| §1: plan §0's object "the §10 consumer conversion table is executed" | plan :15-16 | EXACT |
| §4: E4.7 "O3 close-out: the legacy in-place writers inside `.gwz/checked-artifacts` retire behind the converted paths; the A1-era allowances expire" | plan :531-534 | EXACT (the parenthetical "(`catalog.rs:10-16` class) … with dated annotations" elided) |
| §5: "BY AMENDMENT WITH DUAL REVIEW, not by the implementing step" | plan :524-525 | EXACT |
| §6: "can be cleared with `gwz merge --abort`, which needs no such filesystem" | `capability.rs:54-55` | EXACT fragment |
| §1/§6 tree cites `observation.rs:96,105`; `abort/evidence.rs:39-41`; `:148-165`; `service.rs:120`; `archive.rs:207`; `store/archive.rs:61`; `handle_commit.rs:334`, `:200,213`; `finalize.rs:245,248`; `preserve/artifacts.rs:353`; `abort/preflight.rs:409-410`; `gc.rs:184-192` | live tree at `7f28907` | ALL VERIFIED (read this session) |
| §1 `:279` "SOLELY `gwz repo create` and `init-from-sources` (`handle_create_repo.rs:51,211,336,426`; `handle_init_from_sources.rs:203`)" | those lines call the wrapper `sync_workspace_boundary` (`sync_workspace_boundary.rs:20-28`), which has 17 production call sites incl. `handle_repo_lifecycle.rs:153,229,342`, `handle_materialize.rs:128,404,456`, `handle_branch.rs:234`, `handle_commit.rs:235,240`, `pull_head_member_preflight.rs:186,492`, plus `handle_stage.rs:97` calling `ensure_workspace_exclude` directly | the cites are real; "SOLELY" is false as a caller enumeration (inherited from the prep's own §7.5/§7.4 inconsistency) → [P2-3](b) |

## 3. Mandate 3 — THE `:280` DEVICE and O13 COHERENCE

**The device, ruled.** RecordRoot §2 reads `:280`'s second cell "artifact
actions" through its exception "for the one carved-out path" and adds ONE
dated exception to the third cell; "same purposes" stays binding; every other
v1 path keeps the full second cell. This amendment applies the same device
"for exactly these arms" — consistent IN FORM. In SUBSTANCE it is over-broad
and under-specified at once:

- `:280` governs "v1 checked store/root/bundle paths". Of the carved arms,
  only `store/archive.rs` (sites B–H) and `archive.rs::remove_archive` (site
  A) are v1 paths. `gwz commit`'s writers, the v0 merge publication
  (`finalize.rs:245,248`) and preserve (`preserve/artifacts.rs:226,353`), the
  v0 abort preflight, `ensure_workspace_exclude`, and `gwz stash`'s
  `write_bundle` are not `:280` paths at all — reading `:280` "through this
  exception" for them attaches an exception to a row that never bound them.
  Harmless, but imprecise.
- What DOES bind them is their own rows' cells, which the amendment never
  reads: `:276` "`PreservationBundles` when missing \| parent durable before
  first bundle/native stash mutation"; `:277` "`RootPreservationMarkers` when
  missing \| all ordinary preflight first; parent durable before member/root
  mutation"; `:278` "checked workspace artifact action \| exact source/goal
  and existing managed parent"; `:279` "checked Git-directory artifact action
  \| observe existing Git-owned parent; never bootstrap `.git/info`"; `:275`
  "`MergeArchive` when missing \| terminal/source validation before bootstrap;
  no creation for both-absent/destination-only". RecordRoot §2 did this work
  for `:274` explicitly ("its three clauses are properties the raw path
  already satisfies … Its CONVERSION obligation is read through the
  exception"). Here, some cells are properties the raw paths satisfy today
  (`:275`'s no-creation clause — `store/archive.rs:38-45`; `:279`'s "never
  bootstrap `.git/info`" — `sync_workspace_boundary.rs:41-42` writes only the
  leaf), some are literally NOT met (`:278`/`:279`'s "checked … artifact
  action"; `:277`'s "parent durable before member/root mutation" against the
  unsynced `fs::create_dir_all` at `handle_commit.rs:334`), and some are
  still owed by the convertible forward arms. The "one dated bracket" at §10
  cannot carry this; a per-row, per-cell disposition can → [P2-2].

**O13 coherence.** O13's substantive half "rides E4.2/E4.3" (plan :80-82);
the archive files were never that half (the 2026-09-01 C2 correction, plan
:88-102) but ARE in O13's pin inventory with the retire-on-conversion marker
"until E4.4" (RecordRoot §6 :223-226; E7-Acceptance :180). This amendment
makes them PERMANENT. The O13 inventory is therefore now permanent-documented
in ALL THREE of its rows (creation converted at E4.2; rewrite carved by RR;
archive carved here), and every sentence saying it "empties" is false
forever. Records that must carry a dated annotation (none owned by §7 by
name): plan :90 ("the inventory empties across E4.2–E4.4"), :92 ("belong to
the terminal-archive §10 row, E4.4's"), :110 ("empties across E4.2/E4.4 for
the CONVERTIBLE files"), :114 ("the archive files E4.4's"); plan :493-494
(E4.3 step's C2 bracket); RecordRoot §6 :218-227 (the archive-files sentence,
now inverted — "E4.4's charter author may not cite this section against
that duty" has no duty left to protect); E7-Acceptance :180 ("the archive
rows keep retire-on-conversion until E4.4"); the checker's O13 block
`check_checked_artifact_boundaries.py:321-344` and its finding messages
:1200-1213 ("O13 accepted residual"; "must be retired from the pin
deliberately") — the last is OWNED by §3's inventory re-shape; the rest →
[P2-7].

## 4. Mandate 4 — THE RE-SCOPES (§4)

**O1.** E7-Acceptance §4 :168 closes O1 "RE-OWNED with a named carrier …
R2-F's quarantine/relocation package → this lane's E4 resumption; the close
carries §10 row `:280` via O13", with the 2026-09-02 bracket adding the
RecordRoot citation; framing statement (i) :15-20 says the §0 object "is
deliberately not met, by the operator's ruling 'a'". The new form ("DISCHARGED
as 'the §10 table executed as far as E0.2 §5.2 permits'") COMPOSES with the
carrier chain (relocation landed → E4 resumed → the close happens here) but
fails on two points: (a) "DISCHARGED" against O1's UNRE-SCOPED text (plan :46,
"The §10 consumer conversion table, executed: …") would be false — the ruling
says "Re-scope O1", so the obligation text itself needs a dated bracket first,
then DISCHARGED-against-the-re-scoped-text is honest; (b) SEQUENCING: plan
:145 "O1's close must cite O14's disposition", RR §6 "O1's close cites this
amendment alongside O14's disposition", E7-Acceptance :168 "alongside
O13/O14" — but §5 opens DR-1 AFTER E4.7 lands and O14's fork is DR-1's, so at
E4.7's close-out O14 has no disposition to cite. Either O1 waits for DR-1 or
it closes citing O14 as RE-OWNED to DR-1, with plan :145 and RR §6 annotated
→ [P2-4]. Homes for the dated annotations: plan :46 (O1 text), plan :15-16
(§0, "second re-scope, 2026-09-02, ruling (A)-closed" beside framing (i)'s
first), E7-Acceptance :168 (extend the existing bracket), :15-20 (a pointer
only — the acceptance is CLOSED and framing (i) stays true of ITS close).

**The Phase-E4 milestone.** Heading :331, preamble :333-340, §0 :15-16:
OWNED by §4/§7. The ConsumerCheckpoint §10 bracket: owned generically —
see [P2-2] for what it must contain.

**E4.7 — the P1.** O3's DISCHARGED record (plan :182-203, R1.3, 2026-09-01)
is quoted correctly, but every O3 record that discharges it ALSO preserves
E4.7's first clause: "E4.7 remains this plan's stronger full-retirement
mechanism" (plan :179), "E4.7 remains this lane's stronger full-retirement
mechanism at the E4 resumption" (:193, and E7-Acceptance :170), "E4.7
remains the stronger full-retirement mechanism" (:203); OPEN-R1's resolution
rests on it ("the legacy area remains the legacy writer's live directory
until E4.7 retires the legacy writers — where the retire-the-area question
already lives on its own row", plan :213-216; `GwzM5-8R2F-RelocationPlan.md:78-81`);
the live tree says it (`catalog_names.rs:44` "a directory that is still
live until E4.7"); E7-Acceptance :306 carries "O3's stronger E4.7 form" as
an E4 residual; and two E4.7 DUTIES survive untouched by the ruling —
E0.2 §7.1's condition (`SemanticsAmendment-DRAFT.md:1235-1239`: "E4.7 must
record, per converted consumer, whether `cleanup.rs`'s `finish()` is still
reachable") and the A-1 rider's E4.7 reopen condition (E7-Acceptance :179,
:306). §4's bullet reduces E4.7 to "the allowance expiry and the phase
close-out records" on the ground "O3 was DISCHARGED" — a ground the records
contradict, and a re-scope the ruling did not make.

What the record supports (stated so the fold is short): the "legacy in-place
writer" of O3/E4.7 is the pre-catalog `CheckedArtifact` whose private area
is `.gwz/checked-artifacts` (`policy.rs:33-42` defines it; `observation.rs:93`
in `acquire` is its only consumer — plan :176-177) — the very boundary the
CONVERTED E4.2 leaf writes still ride (RecordRoot §1a's residue lands in
`.gwz/checked-artifacts`). Retiring it "behind the converted paths" is
therefore exactly O14's fork outcome (leaf writes onto the write authority),
which this amendment routes to DR-1. So E4.7 CANNOT retire the legacy writers
before DR-1; the honest disposition is RE-OWNED to DR-1 (O14's outcome), with
the retire-the-area question travelling with it — not silent deletion.
Because that is a scoping beyond the ruling's sentence, it must be visible
to the operator in the checkpoint entry (the ruling: "a different scoping
comes back to me"). → [P1-1].

**The RecordRoot re-examination point** — see §8 of this report.

## 5. Mandate 5 — TIER-2 DEFERRAL (§5)

The E5-landing ruling's exact words (plan :520-527): "before E4.4 executes
tier 2, a comparable sub-surface must be minted BY AMENDMENT WITH DUAL REVIEW,
not by the implementing step; deliberately unminted at E5 per §6.3's
rejected-alternative warning. Queued for the E7 dual." The E7 dual
dispositioned it "Closed for R2-E, carried at §7" (E7-Acceptance :352-354)
with carrier E4.4 under the two-part encumbrance (:175, :308). Ruling point 4
permits "explicitly defer". Deferral to DR-1 is COHERENT with all three,
PROVIDED the amendment says that DR-1's output which mints the sub-surface is
itself an amendment carrying a dual (the E5 ruling's "by amendment with dual
review" — a design round is not automatically that). Two gaps: (i) the
EXECUTION carrier is orphaned — "no E4 step executes tier 2" and DR-1 owns
only the minting; every home naming E4.4 as carrier is unowned by §7:
E7-Acceptance :175 and :308; `GwzM5-8R4bG-Evidence.md:1200` and :1264
("tier 2 is owed on all ten, carrier E4.4, R2-F-encumbered"); the eight
machine-enforced carrier strings in
`gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` (:186-187 …,
"R2-E E4.4 terminal-archive consumer, ratified by lane-owner determination
at the E5 landing …") — a gwz-core edit; (ii) O8's fourth item, "the
`gc_archived` production route" (plan :53; :512-513 "+ the O8 `gc_archived`
production route lands with the archive consumer"; `SemanticsAmendment-DRAFT.md:1170-1175`
"It rides E4.4 …"; `archive.rs:108-111`'s allow reason "no production caller
until that route lands") loses its carrier when E4.4 dissolves, and §7's GC
decode fix is described as "read-side" without saying whether it IS that
route. → [P2-5]. "The R2-F half of the encumbrance is satisfied" is
SUPPORTED — plan :170-174 (R1.1 `027da5b`, R1.2 `bb52dc0`, 2026-09-01),
E7-Acceptance :305 — but the amendment asserts it without a cite; add one.

## 6. Mandate 6 — DR-1's MINTING: authority and agenda, RULED

**Authority for the round:** the ruling's point 5 ("to one phase-end design
round") — the operator minted it; the amendment names and charters it.
**Authority for its dual:** the refined tier policy (checkpoint :37-45,
2026-08-22): "Fable ONLY at the program-level dual gates — … this
amendment-tier work — plus escalations". A chartered design object's dual
sits on that line's first clause; the amendment's header cites the
amendment-tier clause, which is the same line. Consistent with RR's precedent
(RR :10-14; checkpoint :2797-2801). BUT plan §2 :269-271 still reads "Two
duals maximum: dual #1 … dual #2 …" with no annotation, and this line now
carries THREE further duals on the same authority (RR, this amendment, DR-1).
The two-dual budget is materially superseded and its home says nothing →
[P2-6](b).

**Agenda, item by item:** (C) non-identity/degraded mode, reader-side record
reconciliation, O14's fork — the ruling's three, verbatim in substance.
Tier-2 minting — point 4's "explicitly defer" needs a target; DR-1 is the
only phase-end object; within the lane's routing authority, but it is an
ADDITION beyond point 5's list. The record-root re-examination — NOT an
addition: RR §2/§5 already attach it to O14's fork, so it travels with O14
wherever O14 goes (the relocation still needs its annotations, §8 below).
§6's already-shipped narrowing "which only (C) can cure" — a sub-item of (C),
not an addition. **Ruling:** minting DR-1 with this composite agenda is
within the lane's authority as consequential routing, on condition that
(a) the amendment records the agenda as [ruling items] + [lane-routed items
with their ruling hooks: point 4; RR §2/§5], (b) the composite scope — with
the [P1-1] re-own of the legacy-writer retirement, which also lands in DR-1
via O14 — is stated in the checkpoint entry the operator reads, as a scoping
note, not a request for approval, and (c) DR-1 gets a home a future
charterer will find: a dated row after E4.7 in plan §3 (:534) pointing at §5,
and the checkpoint's NEXT line. Open question the amendment should put on
the record rather than decide: DR-1 "opened AFTER … E4.7" is outside Phase
E4 and outside the plan's phase list — is it R2-E's (a new phase) or a new
lane? That is a charter question for the operator; the amendment can name it
as such without answering it → [P2-6](a),(c).

## 7. Mandate 7 — COMPLETENESS SWEEP (the pins package's must-cure list)

Method: whole-estate greps over `/Users/owebeeone/limbo/gwz-dev` (dev-docs,
gwz-core `src/`, `docs/`, `scripts/checks/`, `gwz-core/dev-docs/`) for
`production writes go through the boundary`, `table is executed`/`table,
executed`, `retire-on-conversion`, `until E4.4`, `E4.4`/`E4.5`/`E4.6`/`E4.7`,
`no legacy raw writer`, `needs no such filesystem`/`never refuses`/`never
asks`/`capability-free`/`always be cleared`, `comparable sub-surface`,
`O14's fork`/`E4.6 chartering`, `full-retirement`/`retires the legacy`,
`accepted residual`, `A1-era`/`allowances expire`, `E4.2-E4.6`,
`awaiting R2-E consumer conversion`, `prepare_parent`; plus targeted reads of
every home cited. Reports (`*-Review*.md`, E5/E7/E4.x reviews, RR reviews)
are dated verdicts and are left as written under house discipline; they are
listed once as S50.

| # | Home | The claim | Status under this amendment | Owned by §4/§7? |
| --- | --- | --- | --- | --- |
| S1 | plan :15-16 (§0 object) | "the §10 consumer conversion table is executed" | re-scoped a second time (first: ruling "a", E7 framing (i)) | OWNED (§7 "plan §0") — bracket must name it as the SECOND re-scope [P2-4] |
| S2 | plan :46 (O1 row) | "The §10 consumer conversion table, executed: …" | obligation text must be re-scoped before DISCHARGED | OWNED (§7 "§1 O1") — text re-scope unstated [P2-4] |
| S3 | plan :331 (Phase-E4 heading) | "milestone: production writes go through the boundary" | re-scoped | OWNED |
| S4 | plan :333-340 (preamble universal) | "One step per §10 table row, each … converting that consumer onto the checked boundary" | contradicted for five rows | OWNED (§4 "the preamble universal") |
| S5 | plan :373-384 (gate-note POINTER) | "E4.4 is the first bound step: `archive_terminal` is reached from abort's `respond()`"; "or the capability-free list amended at the amendment tier" | E4.4 dissolved; option (c) CLOSED by the ruling | **UNOWNED** [P2-7] |
| S6 | plan :88-102 (O13 C2 bracket) | ":90 the inventory empties across E4.2–E4.4"; ":92 … belong to the terminal-archive §10 row, E4.4's" | archive rows now permanent | **UNOWNED** [P2-7] |
| S7 | plan :102-115 (O13 RR bracket) | "empties across E4.2/E4.4 for the CONVERTIBLE files"; "the archive files E4.4's" | never empties; carved | **UNOWNED** [P2-7] |
| S8 | plan :138-139 (O14 row) | "the fork is DECIDED at E4.6's chartering" | → DR-1 | **UNOWNED** [P2-7] |
| S9 | plan :145-151 (O14 RR agendum) | "re-examined AT THIS FORK"; "O1's close cites the amendment alongside O14" | → DR-1; O1 sequencing | **UNOWNED** [P2-4][P2-7] |
| S10 | plan :179, :193, :203 (O3 row) | "E4.7 remains this plan's/lane's stronger full-retirement mechanism" | contradicted by §4's E4.7 | **UNOWNED** [P1-1] |
| S11 | plan :213-216 (OPEN-R1 row) | "until E4.7 retires the legacy writers — where the retire-the-area question already lives on its own row" | contradicted | **UNOWNED** [P1-1] |
| S12 | plan :454-459 (E4.1 carriers) | "[P3-C1] … → E4.4's charter"; "[P3-8] … → E4.2-E4.6 as consumers arrive" | carriers dissolve | **UNOWNED** [P3-5] |
| S13 | plan :493-494 (E4.3 step, C2 bracket) | "the two archive files are E4.4's" | carved | **UNOWNED** (§7 lists "E4.4–E4.7 steps", not E4.3) [P2-7] |
| S14 | plan :509-511 (E4.3 outcome) | "named candidate at O14's E4.6 fork" | → DR-1 | **UNOWNED** [P2-7] |
| S15 | plan :512-527 (E4.4 step) | "(+ the O8 `gc_archived` production route lands with the archive consumer)"; tier-2 "before E4.4 executes tier 2 … Queued for the E7 dual" | E4.4 does not start; O8 route carrier lost; tier-2 → DR-1 | OWNED as a home; O8 route unnamed [P2-5] |
| S16 | plan :528-530 (E4.5/E4.6) | conversion steps | dissolve into optional E4.5-B/E4.6-B | OWNED |
| S17 | plan :531-534 (E4.7 step) | "the legacy in-place writers inside `.gwz/checked-artifacts` retire behind the converted paths; the A1-era allowances … expire with dated annotations" | first clause dropped without a surviving home | OWNED as a home; content is the P1 [P1-1] |
| S18 | plan :269-271 (§2) | "Two duals maximum" | third/fourth amendment-tier dual on the line's authority | **UNOWNED** [P2-6] |
| S19 | plan :53 (O8 row) | "the `gc_archived` production route" | carrier (E4.4) lost | **UNOWNED** [P2-5] |
| S20 | ConsumerCheckpoint :275-279 (five rows' second/third cells) | the frozen purposes/orderings | contradicted or unmet per carved arm | OWNED only as "one dated bracket" [P2-2] |
| S21 | ConsumerCheckpoint :280 | "no legacy raw writer"; "artifact actions" | second exception via the RR device | OWNED (§3) — over-broad for non-v1 arms [P2-2] |
| S22 | ConsumerCheckpoint :282-286 | "`CheckedArtifact::prepare_parent` … removed after parity tests pass" | `prepare_parent` keeps its v0 store caller permanently (`store/mod.rs:131` → `entry::prepare_merge_store_parents`, prep §7.0(i)) | **UNOWNED** [P3-7] |
| S23 | SemanticsAmendment-DRAFT :913-917 (§5.2 decision) | "abort … continue[s] to work; a checked-artifact action refuses, typed"; "the mutation guard" | narrowed reading (post-publication v1 abort); broad reading adopted by the ruling's "commit" | **UNOWNED** [P2-1] |
| S24 | SemanticsAmendment-DRAFT :1170-1175 | "`gc_archived` … rides E4.4 with the terminal-archive consumer; when it lands, the `allow(dead_code)` … expire" | carrier lost | **UNOWNED** [P2-5] |
| S25 | SemanticsAmendment-DRAFT :1235-1239 | "E4.7 must record, per converted consumer, whether `cleanup.rs`'s `finish()` is still reachable" | E4.7 duty survives; absent from §4's E4.7 content | **UNOWNED** [P1-1] |
| S26 | RecordRootAmendment §2 :130-134 | "The exception is re-examined at O14's fork (E4.6 chartering)" | → DR-1 | **UNOWNED** [P2-7] |
| S27 | RecordRootAmendment §4 :184-197 | "E4.4 is the first bound step … must carry its disposition in the charter"; "or the capability-free list is amended at this amendment's tier" | E4.4 gone; option (c) closed | **UNOWNED** [P2-7] |
| S28 | RecordRootAmendment §5 :203-206 | "at O14's fork (E4.6 chartering)" | → DR-1 | **UNOWNED** [P2-7] |
| S29 | RecordRootAmendment §6 :218-227 | "which were always E4.4's"; "keep their accepted-residual, retire-on-conversion marker … until E4.4 converts them — E4.4's charter author may not cite this section against that duty"; "(O14/E4.6)" | inverted: carved permanently; no duty remains | **UNOWNED** [P2-7] |
| S30 | RecordRootAmendment §7 :236-237 | managed.rs reason "→ E4.4-E4.6" | range stale again | E4.3-B's — note to its builder, not this amendment's |
| S31 | E7-Acceptance :15-20 (framing (i)) | "§0 object … deliberately not met, by the operator's ruling 'a'" | a second, differently-grounded re-scope now exists | pointer inside the :168 bracket [P2-4] |
| S32 | E7-Acceptance :168 (O1 row) | "RE-OWNED … → this lane's E4 resumption; the close carries §10 row `:280` via O13 … additionally cites RR … alongside O13/O14" | new close form; O14 sequencing | OWNED (§7) — [P2-4] |
| S33 | E7-Acceptance :170 (O3 row) | "E4.7 remains this lane's stronger full-retirement mechanism at the E4 resumption" | contradicted | **UNOWNED** [P1-1] |
| S34 | E7-Acceptance :175 (O8 row) | "tier 2 RE-OWNED — carrier E4.4 …" | carrier → DR-1 | **UNOWNED** [P2-5] |
| S35 | E7-Acceptance :179 and :306 | "A-1 rider's E4.7 reopen condition traveling to the E4 resumption"; "O3's stronger E4.7 form" | E4.7 duties survive / contradicted | **UNOWNED** [P1-1] |
| S36 | E7-Acceptance :180 (O13 row) | "the archive rows keep retire-on-conversion until E4.4" | inverted | **UNOWNED** [P2-7] |
| S37 | E7-Acceptance :308 (residual table) | "Tier-2 archive equivalence … \| **E4.4**" | carrier → DR-1 | **UNOWNED** [P2-5] |
| S38 | R4bG-Evidence :1200, :1264 | "tier 2 is owed on all ten, carrier E4.4, R2-F-encumbered" | carrier → DR-1 | **UNOWNED** [P2-5] |
| S39 | gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json :186-187 (and the other six byte-preserved rows) | tier2 carrier "R2-E E4.4 terminal-archive consumer, ratified … at the E5 landing" (machine-enforced corpus) | carrier → DR-1 | **UNOWNED** — a gwz-core edit [P2-5] |
| S40 | GwzM5-8R2F-RelocationPlan :78-81 (ADOPTED) | "the legacy area remains the legacy writer's live directory until E4.7 retires the legacy writers — which is where the eventual retire-the-area question already lives" | contradicted | **UNOWNED** [P1-1] |
| S41 | E4.4-CharterPrep :4-5 header; §5.2 :625-626; §6 | "INPUT TO E4.4's CHARTER"; "RETIRE-ON-CONVERSION — E4.4's"; the E4.4a/E4.4b shape | superseded per §3/§7 | **UNOWNED** (needs a dated superseded line) [P3-4] |
| S42 | CurrentProgramCheckpoint :2754-2757, :2812-2814 | "DECIDED AT E4.6's CHARTERING"; "NEXT: E4.4 charters under the plain-lease probe clause …; E4.5/E4.6 (O14 fork decides …)" | superseded by the next position entry (the checkpoint's own header rule) | OWNED (§7 "the checkpoint entry carrying the ruling") |
| S43 | checker :321-344 (O13 block) + :1200-1213 (messages) | "The conversion that discharges the clause is R2-E E4.2/E4.3's; until it lands … ACCEPTED RESIDUAL"; ":336 `archive.rs` and `store/archive.rs` are the terminal-archive row's, E4.4's"; "must be retired from the pin deliberately" | archive rows permanent; message class becomes revise-this-amendment (P-1's precedent) | OWNED (§3 inventory re-shape, pins package) |
| S44 | checker :358 | "A SECOND caller is an E4.2-E4.6 conversion" | range stale | [P3-6] |
| S45 | `v1_lifecycle/archive.rs:108-111` (gc_archived allow) | "this family has no production caller until that route lands" | route's carrier lost | **UNOWNED** [P2-5] |
| S46 | `catalog_names.rs:44` | "a directory that is still live until E4.7" | contradicted if E4.7 drops the retirement | **UNOWNED** [P1-1] |
| S47 | `capability.rs:50` ("`--abort` never does") and :54-55 (remedy string); `docs/OperationModel.md:131-132` ("An abort never asks"), :147-149 ("What never refuses: `gwz merge --abort` … on a record of either version — so a `--no-ff` merge left open on a workspace that later becomes incapable can always be cleared"), :158; `v1_lifecycle/checked.rs:129-133`; `runtime/dispatch.rs:437-438`, :447-448 ("`gwz merge --abort` is capability-free and clears it"); `v1_lifecycle/service.rs:112-115` | abort capability-free, unconditionally | over-claim for the post-publication v1 abort | §6/§7 own capability.rs + OperationModel.md; the other four in-tree homes and OperationModel's three separate sentences are unnamed; "the E4.1 release-notes line" has NO on-disk home (E4.1 [P3-1], :222: it lives only in commit (b)'s message; carrier the release train) [P3-2] |
| S48 | `catalog.rs:19-22, :27-29, :34-36, :46`; `coordinator/mod.rs:16`; `entry.rs:299-300`; `bootstrap/managed.rs:45-46`; `checked_artifact/mod.rs:12,18,29-31,37,44,50,59` | "first production consumers are the E4.2-E4.6 conversions"; "conversion arrives consumer by consumer across E4.2-E4.6"; "E4.2-E4.6 convert the consumers that will read it"; "frozen interface awaiting R2-E consumer conversion (plan §5 item 1)" | E4.4-E4.6 conversions dissolve; some surfaces may never gain the promised consumer | OWNED by class (E4.7's allowance expiry) — the class must be NAMED [P3-6] |
| S49 | `store/rewrite.rs:42` | "`commit`'s raw writers are untouched — E4.3's half of O13." | RR's S11, E4.3-B's dated `///` sentence | not this amendment's |
| S50 | E0.2b :1008-1016, :1137-1139, :1242-1246; E03-ReviewState :866-868, :1031-1032, :1080; E4.1-Review :713-721, :1021; E4.2-Review :363-366, :887, :924-929; E5-Review :333-366; E7-ReviewState :162-175, :268-274, :389, :398-399; RR-ReviewCode/State; checkpoint historical entries | dated analyses and review verdicts | left as written (house discipline: reports are not edited; the operative ledger is plan §1.1) | none |

**Count: 50 hits. 11 OWNED by §4/§7 as drafted (S1-S4, S15-S17, S21, S32,
S42, S43 — six of them with a caveat folded into a finding). 2 partially
owned (S47, S48). 34 UNOWNED must-cure (S5-S14, S18-S20, S22-S29, S31,
S33-S41, S44-S46), collapsing into eleven cures: [P1-1], [P2-1], [P2-2],
[P2-4], [P2-5], [P2-6], [P2-7], [P3-2], [P3-4], [P3-5], [P3-6], [P3-7].
2 not this amendment's (S30, S49). 1 group of no-cure dated records (S50).**
The pins package's root-side list in §7 must grow from its current nine
named homes to cover every UNOWNED row above; the gwz-core side gains S39,
S45, S46, S47's four unnamed homes, and S44/S48 by class.

## 8. Mandate 8 — THE RECORD-ROOT RE-EXAMINATION RELOCATION

Yes, RecordRootAmendment needs dated annotations (it is ADOPTED; L1-08's
precedence trail forbids silent reinterpretation): §2 :130-134 (the
blockquote's operative tail "re-examined at O14's fork (E4.6 chartering)"),
§5 :203-206, §6 :222 "(O14/E4.6)" — plus, from the same amendment and for the
same reason, §4 :184-197 ("E4.4 is the first bound step"; option (c) now
closed by the operator) and §6 :218-227 (the archive-files sentence,
inverted). O14's row: plan :138-139 ("OWNER: the fork is DECIDED at E4.6's
chartering, when every converted shape exists" — the "when" clause is now
false too: the convertible shapes are the optional E4.5-B/E4.6-B arms, and
DR-1 must not wait on an optional step) and :145-151 (the RR agendum "AT
THIS FORK"). Also plan :509-511 (E4.3 outcome, "O14's E4.6 fork") and the
checkpoint position entries (superseded by rule). Each is one dated bracket
citing this amendment §4/§5. → [P2-7]. The RR-ReviewState's §C ruling that
the E4.6 placement was "correctly placed" (its :126-143) is a dated verdict
and stands as history; the relocation is grounded (E4.6 dissolves under §7).

---

## 9. FINDINGS

### P1

**[P1-1] — §4's E4.7 re-scope widens the ruling and orphans the
legacy-writer retirement.** Evidence in §4 of this report (S10, S11, S17,
S25, S33, S35, S40, S46). The ruling re-scopes E4.7 by exactly one sentence
("E4.7 does not retire those writers" — the capability-free arms). The
amendment additionally drops E4.7's first clause and two surviving duties,
on a ground ("O3 was DISCHARGED") that the discharge records themselves
negate. **Cure (fold):** (i) rewrite the bullet: E4.7 KEEPS the A1-era
allowance expiry (E4.1 [P3-5]'s class plus the `E4.2-E4.6`-range reasons of
S48), E0.2 §7.1's per-converted-consumer `finish()`-reachability record, the
A-1 rider's reopen-condition check, and the phase close-out records; the
legacy-in-place-writer retirement is RE-OWNED to DR-1 as O14's outcome, and
the retire-the-area question (OPEN-R1's resolution) travels with it; the
carved raw writers are not E4.7's to retire (ruling). (ii) Dated annotations
at plan :179/:193/:203, :213-216, :531-534; E7-Acceptance :170, :179, :306;
`GwzM5-8R2F-RelocationPlan.md:78-81`; `catalog_names.rs:44` (pins package);
a pointer at `SemanticsAmendment-DRAFT.md:1235-1239`. (iii) One sentence in
the checkpoint entry making the consequential re-own visible to the operator.
Blocking: the amendment cannot be adopted with five adopted records
contradicted and no home for the dropped content.

### P2

**[P2-1] — The §5.2 reading brackets are missing at §5.2's home.** Two
readings this amendment relies on live only inside it: (a) the BROAD reading
of "the mutation guard" — the exception lists `gwz commit` and "the mutation
guard" as operations, and the `:279` carve-out's non-create/init callers
(materialize, stage, branch, repo lifecycle, pull preflight) rest on it;
§5.2 names none of them, and the prep's §7.7(2) filed the ambiguity as
undetermined; the ruling's "commit" resolves it broad, and the record must
say so; (b) the §6 narrowing (a post-publication v1 abort's checked-evidence
re-verification is a checked-artifact action under §5.2's own second clause).
Without (a), §2's "the list is NOT amended" and §3's operation list read as an
unrecorded extension. **Cure:** one dated reading bracket at
`SemanticsAmendment-DRAFT.md:917` (after the decision blockquote) carrying
both readings, citing the ruling and this amendment; §2 gains one sentence
naming the broad reading as the operator's.

**[P2-2] — Rows `:275`-`:279`'s own cells are undispositioned; the `:280`
device is over-broad.** §3 of this report. **Cure:** a per-row, per-cell
disposition table in §3 (which cells the raw arms satisfy today — cite; which
are accepted-unmet as part of the exception — say so; which the convertible
forward arms still owe), and the ConsumerCheckpoint §10 bracket pointing at
it; scope the `:280` sentence to the v1 arms (`store/archive.rs`,
`archive.rs::remove_archive`, and any v1 forward arm that stays raw).

**[P2-3] — The carved-site enumeration and the exception's primitive
classes do not match.** (a) `:276`: "`abort --preserve`
(`v1_lifecycle/reverse/execute/preservation.rs:74`)" is the CONVERTED v1
preserve (`checked_bundle.rs` → `entry::replace_merge_preservation_bundle`,
probing today — prep §7.1) and belongs to §6's class, not to the carved set;
the raw `:276` writers are the v0 merge preserve (`preserve/artifacts.rs:353`)
and `gwz stash`'s `write_bundle` (`stash/mod.rs:260-262`, under
`guarded_workspace_root`), the latter unnamed anywhere in §3. (b) `:279`:
"SOLELY … create / init-from-sources only" is false as an enumeration (17
wrapper call sites + one direct, §2 of this report); CARVED ENTIRELY stands.
(c) The exception carves "raw `durable_fs` / `std::fs` publication
primitive[s]"; rows `:276`-`:279`'s carved writers publish through
`artifact::write_atomic` / `write_marker` / `write_lock` / `stash::write_bundle`
(`artifact/mod.rs:492-495`, itself over `durable_fs`), which an O13-shape
inventory counting `durable_fs` names would count as ZERO at those files — a
vacuous pin. **Cure:** name the `artifact::write_atomic` family as the third
carved primitive class and count it (or the carved call sites) per file;
correct (a) and (b).

**[P2-4] — O1's close form: text re-scope unstated; O14 sequencing
contradiction.** §4 of this report. **Cure:** dated bracket at plan :46
re-scoping O1's obligation text (the ruling's "Re-scope O1"), then
DISCHARGED against it; choose and record one sequencing (O1 closes at DR-1's
disposition, or closes at E4.7 citing O14 as RE-OWNED to DR-1) with brackets
at plan :145, RR §6 :226-227, E7-Acceptance :168 (extend), and a pointer
beside framing (i) :15-20.

**[P2-5] — Tier-2 execution carrier and O8's `gc_archived` route are
orphaned; the R2-F-satisfied claim is uncited.** §5 of this report (S15,
S19, S24, S34, S37, S38, S39, S45). **Cure:** state that DR-1 mints the
sub-surface BY AMENDMENT WITH ITS OWN DUAL and names the execution carrier;
state the GC decode fix's relation to O8's `gc_archived` production route
(is it that route, or does the route re-own?); annotate the eight homes (S39
is a gwz-core edit in the machine-enforced corpus — assign it to the pins
package or the GC fix explicitly); cite plan :170-174 / E7-Acceptance :305
for the R2-F half.

**[P2-6] — DR-1's authority recorded, plan §2 and a plan home.** §6 of this
report. **Cure:** (a) record DR-1's agenda as ruling items + lane-routed
items with hooks, and put the composite scope (incl. [P1-1]'s re-own) in the
checkpoint entry as a scoping note; (b) dated bracket at plan §2 :269-271
recording the amendment-tier exception and its count; (c) a dated DR-1 row
after plan :534 pointing at §5, and the checkpoint NEXT line; name the
"R2-E phase or new lane" question as the operator's.

**[P2-7] — RecordRootAmendment, plan O13/O14/E4.3, gate-note pointer, and
E7-Acceptance O13 annotations are unowned.** §3 and §8 of this report (S5,
S6, S7, S8, S9, S13, S14, S26, S27, S28, S29, S36). **Cure:** extend §7's
root-side list by name with one dated bracket per home; for S5 and S27
record that option (c) of the PLAIN-LEASE PROBE CLAUSE is closed by the
ruling (the clause otherwise survives for E4.5-B/E4.6-B).

### P3

**[P3-1]** §1's quotation of §5.2's list is not verbatim (order, `gwz`,
case) and the Ground 2 cite range ":909-960" stops before :964. Cure: quote
verbatim; ":909-967".

**[P3-2]** "All three homes" undercounts the abort-sentence class (S47: six
in-tree homes, OperationModel.md's three separate sentences, and a
release-notes "home" that does not exist on disk — E4.1 [P3-1]). Cure:
enumerate the homes the pins package cures, or define the sentence class plus
a scan; state the release-notes line's carrier status honestly.

**[P3-3]** §3's "sites B–H: `rename_noreplace` ×2, `sync_dir` ×7 … site A
`sync_dir` ×2" conflates pin REFERENCE counts (which include the `use` lines
— the prep's §1.0 explicit correction) with call sites (B–H = 1 + 6; A = 1).
Cure: label them pin counts.

**[P3-4]** The charter prep carries no superseded line (S41). Cure: one dated
status line at its header.

**[P3-5]** E4.1's carriers [P3-C1] (→ "E4.4's charter") and [P3-8] (→
"E4.2-E4.6 as consumers arrive") dissolve unnamed (S12). [P3-C1]'s guard row
("`--abort` succeeds end to end on an obstructed/non-admitted workspace") is
precisely the pins package's pre-publication pin. Cure: disposition both by
name.

**[P3-6]** The A1-era allowance class E4.7 keeps must be NAMED (S44, S48) so
E4.7's charter cannot miss the `E4.2-E4.6`-range and "awaiting R2-E consumer
conversion" reasons that are now stale as written; each expires or is
re-reasoned as permanent.

**[P3-7]** ConsumerCheckpoint :282-286's removal promise for
`CheckedArtifact::prepare_parent` (S22) is unreachable under the carve-out;
fold into the §10 bracket.

**[P3-8]** §1 "and the corpus never said so" — the prep §7.5 said it the same
day, RR §1b said it for E4.3. Soften to "no adopted record said so".

---

## 10. VERDICT AND ROUND-2 TERMS

**GO-WITH-CONDITIONS.** [P1-1] blocks adoption until folded (text only; no
tree change; one operator-visibility sentence in the checkpoint entry). The
seven P2 are folds of the same kind — dated annotations and enumeration
corrections — and the P3s are precision. On the folds, this axis asks for a
round-2 text-diff confirmation of [P1-1], [P2-1], [P2-2], [P2-3] and the
extended §7 list only; [P2-4]-[P2-7] and the P3s may be waived on the lane
owner's fold record if the diff shows them landed. Nothing here disputes the
ruling, the list, the carve-out's shape, the tier-2 deferral, or the §6
disposition; nothing here asks the operator for a new decision — only that
the consequential re-own of the legacy-writer retirement and DR-1's composite
scope be visible where the operator reads.

---

## ROUND 2 — State axis confirmation

Date: 2026-09-02. Object: the folded v2 at
`GwzM5-8R2E-CapabilityFreeAmendment.md` (480 lines) against the preserved
round-1 draft (`CapFree-v1-draft.md`, 264 lines, session scratch — its :180
matches the E4.7 bullet this axis reviewed). Text-diff confirmation per §10's
terms; read-only; 608 changed lines. The §0 ruling excerpt is textually
unchanged from v1 (verified; the mandate's elision check stands). v2 §8's
recording of both round-1 verdicts matches the filed reports (Code
`-ReviewCode.md:32-34` "NO-GO (round 1) … 2 P1, 5 P2, 7 P3"; this report's
header).

### VERDICT: GO (terminal), with ONE mandatory landing fold on [P3-1] and three precision notes recorded without cure

### Blocking and round-2-named conditions — each CONFIRMED against the folded text

**[P1-1] — CONFIRMED.** v2 §4 :245-272. Narrowed to the ruling's sentence:
*"The ruling re-scopes E4.7 by one sentence: it does not retire the
capability-free arms. Everything else E4.7 owned SURVIVES"*. The allowance
class NAMED by home: *"E4.1 [P3-5]'s stale allows (`workspace_mutator_lock.rs:44-47`,
`catalog.rs`), the `E4.2-E4.6`-range and "awaiting R2-E consumer conversion"
reasons at `catalog.rs:19-22,27-29,34-36,46`, `coordinator/mod.rs:16`,
`entry.rs:299-300`, `bootstrap/managed.rs:45-46`,
`checked_artifact/mod.rs:12,18,29-31,37,44,50,59`, checker `:358` — each
EXPIRES or is RE-REASONED AS PERMANENT"* (S44, S48). The two duties retained:
*"E0.2 §7.1's per-converted-consumer `finish()`-reachability record
(`SemanticsAmendment-DRAFT.md:1235-1239`); the A-1 rider's reopen-condition
check (E7-Acceptance :179)"*. The retirement re-owned: *"The LEGACY
IN-PLACE-WRITER RETIREMENT (E4.7's first clause) is NOT dropped: it is
RE-OWNED to DR-1 as O14's outcome — the legacy writer IS the pre-catalog
`CheckedArtifact` that every converted path still rides … and the
retire-the-area question (OPEN-R1's resolution, RelocationPlan :78-81)
travels with it."* Homes listed: *"plan :179/:193/:203; E7-Acceptance
:170/:306), OPEN-R1's row (plan :213-216), E4.7's step (plan :531-534),
RelocationPlan :78-81, `catalog_names.rs:44` ("still live until E4.7") and
E7-Acceptance :179 each gain a dated bracket"* — every home of round 1's
list (S10, S11, S17, S25, S33, S35, S40, S46) present. Operator visibility:
*"the checkpoint entry carries one sentence making this consequential re-own
visible to the operator"* (and §7 :423 "the E4.7 re-own sentence"). The
ground repudiated: *""O3 was DISCHARGED" (R1.3, 2026-09-01) stands as
recorded and is NOT the ground for any of this; O3's discharge and E4.7's
stronger mechanism were always distinct records."* The P1 is cured; the
five contradicted adopted records each have a named annotation.

**[P2-1] — CONFIRMED.** v2 §2 :134-143: *"Two READINGS of §5.2 this
amendment relies on are recorded as one dated bracket at §5.2's own home
(`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:917`, after the decision
blockquote), citing the ruling and this amendment … (a) "the mutation guard"
is read BROADLY — the operations executed under it — the operator's reading,
resolved by naming `commit`; (b) §6's narrowing — a v1 abort's
re-verification of checked artifacts it must reconcile is "a checked-artifact
action [that] refuses, typed" under §5.2's own second clause, not an operation
placed on the probe. Neither moves an operative word of the list."* Reading
(b) is phrased over "checked artifacts it must reconcile", which covers the
three paths §6 now names — the bracket does not need re-drafting if a fourth
path is ever found. §1 :86-90 restates the broad reading with its ground.

**[P2-2] — CONFIRMED.** v2 §3 :175-184, the five-row table with the three
columns asked (cell 2; cell 3 status with cites — SATISFIED `:275`
(`store/archive.rs:38-45`); ACCEPTED-UNMET `:276` (v0 preserve, `gwz stash`),
`:277` (`handle_commit.rs:334` unsynced `create_dir_all`); NOT MET-ACCEPTED
`:278`, `:279` (with the `write_atomic`-bootstraps-the-parent fact,
`artifact/mod.rs:492-495`); owed by the convertible arm — E4.5-B
`execute.rs:45`, E4.6-B `execute.rs:48` / `:51`). The `:280` device scoped:
*"read through this exception for exactly the V1 arms it carves —
`store/archive.rs`, the dead `archive.rs::remove_archive`, and any v1 forward
arm that stays raw — as RR §2 reads them for the record root; the non-v1 arms
were never `:280` paths and are governed by their own rows' cells,
dispositioned below."* §4 :239-241 routes the ConsumerCheckpoint's per-row
brackets to this table.

**[P2-3] — CONFIRMED.** (c) v2 §1 :94-102 names *"THREE primitive classes —
`durable_fs` (…), `std::fs`-direct (…), and the `artifact::write_atomic`
family (`write_atomic` / `write_marker` / `write_lock` /
`write_manifest_and_lock` / `stash::write_bundle` /
`publish_workspace_exclude_candidate` / `sync_workspace_boundary` /
`ensure_workspace_exclude`, itself over `durable_fs`) — an inventory that
counted only `durable_fs` names would read ZERO at most carved files"*; §3
:193-204 counts them: *"(ii) the `\bdurable_fs\b` population gate is dropped
… (iii) the token vocabulary widens to all three primitive classes … The
`std::fs` and `write_atomic` classes go INTO the count map"*. (a) `:276`
corrected, v2 :81: the v0 writer and `gwz stash` carved (*"`gwz stash`
(`stash/mod.rs:260-262` `write_atomic`, mutation guard `handle_stash.rs:43`)"*),
the v1 writer moved (*"The v1 preserve writer … is CONVERTED and already
probing on the PLAIN lease — §6's class, not a carved writer"*). (b) `:279`
"SOLELY" removed; v2 :84 enumerates the family's callers with their guards.
The [P3-3] labeling rides here (*"pin-reference counts `rename_noreplace` 2,
`sync_dir` 7 — call sites 1 + 6"*; §3 *"pin-reference counts, `use` lines
included, labeled as such"*).

**Extended §7 list — CONFIRMED, every UNOWNED S-row now has a named home.**
v2 §7 :413-424 (root side) and :400-413 (gwz-core side), checked row by row:
S5 (:373-384), S6/S7 (:88-115), S8 (:138-139), S9 (:145-151), S10
(:179/:193/:203), S11 (:213-216), S12 (:454-459), S13 (:493-494), S14
(:509-534), S18 (:269-271), S19 (:53), S20 (ConsumerCheckpoint :275-279),
S22 (:282-286), S23 (`:913-917`), S24 (`:1170-1175`), S25 (`:1235-1239`),
S26-S29 ("RR §2/§3/§4/§5/§6"), S31 (E7-Acceptance :15-20 pointer), S33
(:170), S34 (:175), S35 (:179, :306), S36 (:180), S37 (:308), S38
(R4bG-Evidence :1200/:1264), S39 (the JSON carrier strings, gwz-core side
:405-406), S40 (RelocationPlan :78-81), S41 (the prep's header), S44
(checker `:358`, :407-408), S45 (`archive.rs:108-111`, :406), S46
(`catalog_names.rs:44`, :407). S47's six in-tree homes and OperationModel's
three sentences (:403-405) and S48's class (:407-408) also present. 34 of
34.

### Waivable conditions — each shown LANDED by the diff; waived

**[P2-4] — LANDED.** v2 §4 :223-235: the text re-scope at plan :46 as *"the
SECOND re-scope, after the operator's ruling "a" (E7-Acceptance framing
(i))"*; the sequencing chosen and recorded: *"O1 closes DISCHARGED at E4.7
against the re-scoped text, citing O14 as RE-OWNED to DR-1 (not awaiting
DR-1's disposition, which is post-phase); brackets at plan :145 …, RR §6
:226-227, E7-Acceptance :168 (extended) and a pointer beside framing (i)
:15-20"*.

**[P2-5] — LANDED.** v2 §5 :297-307: *"DR-1 mints the sub-surface BY
AMENDMENT WITH ITS OWN DUAL and names the execution carrier … The R2-F half
… is satisfied by the landed relocation (plan :170-174; E7-Acceptance
:305). The eight machine-enforced carrier strings … are re-pointed to DR-1
by the pins package (a gwz-core edit, dated)"*; :309-318 the GC fix *"is
NOT O8's route"* and the dead family's disposition (*"E4.7's allowance expiry
either deletes the family or re-reasons its allowance as
permanent-pending-DR-1; the route itself RE-OWNS to DR-1, conditional on
(C)"*) with homes plan :53, :512-513, `:1170-1175`, `archive.rs:108-111`.

**[P2-6] — LANDED.** v2 §5 :320-344: agenda as *"[ruling items] plus
[lane-routed items with their hooks]"* (ruling: (C), reader-side
reconciliation, O14; lane-routed: tier-2 with point 4's hook, the record-root
re-examination with RR §2/§5's, §6's narrowing under (C), the E4.7 re-own
under O14, O8's route under (C)); *"the composite scope, including the E4.7
re-own, goes in the checkpoint entry as a SCOPING NOTE the operator reads
(not a request); plan §2 :269-271 gains the dated bracket on the two-dual
budget; plan §3 gains a dated DR-1 row after E4.7 (:534)"*; the question
named, not answered: *"is it R2-E's (a new phase E8) or a new lane?"* The
header :17-19 records the §2 bracket's content ("three further
amendment-tier duals (RR, this, DR-1)").

**[P2-7] — LANDED.** v2 §4 :273-289: RR §2 :130-134, §5 :203-206; plan O14
:138-139, :145-151; plan E4.3 :509-511 → DR-1 with dated brackets; RR §4
:184-197 and the gate-note pointer :373-384 gain *"the bracket that E4.4 as a
bound step is gone and that the clause's option (c) — "the capability-free
list amended at the amendment tier" — is CLOSED by the ruling; the clause
otherwise survives, binding E4.5-B/E4.6-B"*; RR §6 :218-227, E7-Acceptance
:180/:175/:308, plan :88-115 (all four quoted sentences), :493-494, and the
checker's O13 block gain *"the O13 inventory is now PERMANENT-DOCUMENTED in
all three rows … and never empties"*.

**P3s:** [P3-2] LANDED (v2 §6 :348-359 — the six in-tree homes,
OperationModel's three sentences, and *"the E4.1 release-notes line, which
has NO on-disk home (E4.1 [P3-1]: it lives in a commit message; carrier the
release train)"*). [P3-3] LANDED (above). [P3-4] LANDED (:397 *"its header
gains a dated status line"*). [P3-5] LANDED (:409-413 — [P3-C1]'s guard row
becomes *"the package's pre-publication capability-free pin"*; [P3-8]
*"closes (nothing converts, no snapshot exclusion grows)"*). [P3-6] LANDED
(§4's named class). [P3-7] LANDED (:241-244). [P3-8] LANDED (:55 *"no
ADOPTED record said so"*). **[P3-1] — NOT LANDED, and the wording
regressed:** v2 :65-66 now reads *"Its list, verbatim: *"`repo create`,
`init-from-sources`, ordinary merge, abort, GC and the mutation guard"*"*
— the same reordered, `gwz`-less, case-changed string round 1 flagged, now
LABELED verbatim. The frozen text (`SemanticsAmendment-DRAFT.md:915-917`)
is: *"an ORDINARY merge, `gwz repo create`, `init-from-sources`, abort, GC
and the mutation guard continue to work; a checked-artifact action refuses,
typed."* The range fix (":909-967") did land. A false "verbatim" label on a
frozen-text quote is exactly this axis's defect class; it is mechanical, the
correct text is above, and it needs no further verdict — **MANDATORY LANDING
FOLD:** replace the quoted string at v2 :65-66 with the frozen sentence
verbatim (or drop the word "verbatim"). GO is conditioned on that one edit
being in the landed text; no round 3.

### The §6 ruling, re-examined for three paths — STILL NOT LIST-AMENDMENT-BY-STEALTH

v2 §6 :361-391 names three checked-door paths on the plain lease
(post-publication evidence; `--preserve` with an integrated participant; a
selected `@root`'s manifest/lock) and scopes the sentence class BY PATH:
*"an abort that touches no checked artifact needs no such filesystem; aborts
that must re-verify checked artifacts — preservation bundles, a selected
root's manifest and lock, or the merge's published evidence, re-verified
through the checked boundary — need persistent file handles and a mount
identity."* Both round-1 grounds are unchanged by the count: (1) the
operator's instruction was "Fix the sentence (scope it … or date the
residual)" — a wider over-claim is still the sentence to fix, and v2 does
both branches; (2) §5.2's own second clause governs every one of the three
paths — each is a checked-artifact action refusing typed, which is what the
§2 reading bracket (b) records in path-independent words. No operative word
of the list moves. The scoping is a READING recorded at the list's home, and
the residual is dated with its cure named (DR-1's (C)). Ruling stands.

Two things the wider fact changes, recorded as notes: (i) the ruling's own
premise ("post-publication v1 abort already reaches …") was narrower than
the tree — the shipped abort is capability-free on fewer paths than the
operator was told, two of them not gated on publication. §6 states it plainly
but §7's checkpoint-entry list (:421-423) does not name it among the entry's
contents. Recommended landing addition, one clause in the scoping note: "the
shipped abort is capability-free on fewer paths than the ruling's premise —
three checked-door paths, two independent of publication (§6)". Not a
condition; the fact is fully on the record in §6. (ii) v2's "STRICTLY
WEAKER filesystem" finding (the legacy probe admits btrfs/xfs/zfs) means the
remedy string's own admitted-filesystem clause (`capability.rs:53-54`, "on
Linux that is ext4 only") is over-strict when rendered from the legacy
module on an abort path, and `OperationModel.md:139` ("On Linux the admitted
filesystem is `ext4` and nothing else") stays true only of the catalog — the
pins package's six-home scoping should keep those two sentences from
contradicting the new abort sentence at its own home. Code-axis territory;
noted for the builder, no cure asked here.

### Wording that weakens a finding (recorded; none blocks)

1. **[P3-1]'s regression** — above; the mandatory landing fold.
2. **"if taken" vs O1's close form.** v2 §7 :429-431 charters E4.5/6-B *"as
   ONE small step (E4.5/6-B) if taken"*, while §4 :226-229's re-scoped O1
   text lists *"converted (E4.1, E4.2, the convertible forward arms)"* as a
   settled category. If E4.5/6-B is not taken, those three v1 forward arms
   stay raw and fall under the exception's *"any v1 forward arm that stays
   raw"* clause (§3 :168-169) — coherent, but O1's close would then list them
   CARVED, not CONVERTED. The landing should make E4.5/6-B's status definite,
   or O1's re-scoped text should read "converted or carved-as-v1-forward-arm
   (§3)". Precision, not a contradiction: both dispositions are inside the
   amendment.
3. **RR §3's digest-backstop sentence** (v2 §3 :204-210, a Code-axis
   finding: `PROTECTED_SOURCE_TREE_DIGESTS` does not pin `store/rewrite.rs`
   at main) is routed to "RR §3 gains the dated correction" and §7 lists "RR
   §2/§3/§4/§5/§6". Adequate. Recorded here because RR's round-1 State
   review accepted the P-1 scope fold on that sentence's strength; the
   correction is the honest device and nothing in RR's exception depends on
   the digest (the O13 counts and P-2 are the pins).

### Round-2 close

All four round-2-named conditions and the extended §7 list CONFIRMED against
quoted text; [P2-4]–[P2-7] and seven of eight P3s LANDED and waived; [P3-1]
regressed and is the one mandatory landing fold (text supplied above). No
new-claim flags on this axis beyond the Code-axis tree findings v2 adopts,
which this axis takes as the Code axis's and checked only for record
consequence. **THE STATE AXIS IS GO.** On the Code axis's round-2
confirmation, the dual is complete.
