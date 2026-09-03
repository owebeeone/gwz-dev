# GwzM5-8 R2-E Record-Root Amendment — State axis review (RECORD COHERENCE)

Date: 2026-09-02. Axis: **State** (record coherence — the amendment against
the corpus: exact quotes, defensible readings, no orphaned or contradicted
claims anywhere in the estate's records). Peer-blind (the Code axis unseen).
Tier: amendment dual, Fable. Round: **1 of ≤2**.

Object under review: `dev-docs/GwzM5-8R2E-RecordRootAmendment.md` (DRAFT,
191 lines) against gwz-core main `7f28907` (the E4.2 landing — verified
checked out and clean), the candidate branch
`origin/probe/e4-3-detach-window-evidence` = `c9a7303` (verified: tip exact,
**not** an ancestor of main), the O13 checker at
`scripts/checks/check_checked_artifact_boundaries.py`, and the gwz-dev
document corpus. Read-only throughout; no builds, no commits.

## VERDICT: GO-WITH-CONDITIONS — 0 P0 / 0 P1 / 3 P2 / 3 P3

The amendment's core is sound and driven: the exception it mints is real
(the detach window is in the live transition code exactly where §1a cites
it), the quotes of the frozen rows are exact, the pins are correctly
specified against the checker as it exists, the routing to O14's fork is
coherent, and the authorization chain is characterized consistently with the
corpus. The three P2s are document-class: one clause of the amended row is
left undispositioned against the E4.2 review's own reading of that row; one
§6 sentence overreaches the archive files; and §7's charter omits, by name,
five records the corpus sweep shows this fork touches. All cures are text
folds inside the two-round cap.

---

## A. Mandate 1 — frozen-text exactness (every quote, checked at its home)

| # | Amendment text | Home | Verbatim check |
| --- | --- | --- | --- |
| A1 | §2: row `:280` "reads, verbatim: *'v1 checked store/root/bundle paths \| same purposes and artifact actions \| test-gated until A1; no legacy raw writer'*" | `GwzM5-8R4bR2ConsumerCheckpoint.md:280` | **EXACT** (cell text matches character-for-character; table pipes at the row edges elided, correctly) |
| A2 | §2: row `:274` characterized as "merge record rewrite \| exact existing `MergeStore` \| no parent creation; unknown fields and exact reread preserved" | `GwzM5-8R4bR2ConsumerCheckpoint.md:274` | **EXACT** |
| A3 | §1b: "E0.2 §5.2's frozen capability-free list (abort is on it)" | `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §5.2 (:909-917) | **CORRECT.** §5.2's decision text: "an ORDINARY merge, `gwz repo create`, `init-from-sources`, abort, GC and the mutation guard continue to work; a checked-artifact action refuses, typed." Abort is on it. The list is frozen (amendment pair LANDED, addendum controlling, per the checkpoint's E0-landing entry). In-tree corroboration: `service.rs:112-115`'s comment names "E0.2 §5.2's capability-free list" for the reverse arms. |
| A4 | §1b: `capability.rs:45-55`: "*`gwz merge --abort`, which needs no such filesystem*" | `src/checked_artifact/capability.rs:52-55` | **EXACT fragment** of "An open merge can be cleared with `gwz merge --abort`, which needs no such filesystem; a new merge can be started without --no-ff". The `:45-55` range covers the doc block + constant. |
| A5 | §1a: detach at `transition.rs:290-326`, goal publish at `:391-425` | `src/checked_artifact/transition.rs` | **VERIFIED.** :285-326 is the detach body (`BeforeDetachPublication` → `publish_verified_leaf_no_replace` into the private area → durability barriers → reobservation asserting the leaf `Missing` in the parent); :387-425 is `publish_goal` (`BeforeManagedPublication` at :387, publication at :391). Between those edges the open record does not exist — the claimed window is structurally present. |
| A6 | §1a: candidate row `an_interrupted_checked_rewrite_detaches_the_record_beyond_the_lifecycles_reach` at `tests/store.rs:371` | `c9a7303:src/workspace_ops/merge/v1_lifecycle/tests/store.rs:371` | **VERIFIED** (fn at :371; its doc block cites `classify_open_record` `merge/store/mod.rs:211-241`, consistent with this amendment). |
| A7 | §1a: `fail_next_checked_artifact_at(BeforeManagedPublication)` | `fault.rs:96`, `fault.rs:74`, `transition.rs:387` | **VERIFIED** — both names exist; the fault sits immediately before the goal publication. |
| A8 | §1a: `classify_open_record` (`merge/store/mod.rs:211-241`) enumerates `.gwz/merge/*.yaml`, reports no open merge | `src/workspace_ops/merge/store/mod.rs:211-244` | **VERIFIED** — fn at :211 exactly; enumerates `root.join(MERGE_DIR)`; `0 => Ok(None)`. |
| A9 | §1a: three residue files `.authority`/`.source`/`.goal` in `.gwz/checked-artifacts`, dirt-exempt (`policy.rs:33-45`) | `authority.rs:227/:235/:238` (the three suffixes are the production naming vocabulary); `policy.rs:33-45` (the legacy private area's doc naming the git-status dirt exemption; `private_parent` at :46) | **VERIFIED.** |
| A10 | §1b: `commit` shared by activated forward and plain reverse arms (`service.rs:116-121`) | `service.rs:116-121` (the lease match: `ResumeStart\|Continue → acquire_activated`, `_ → acquire`); `store.commit(&lease, …)` at `service.rs:140` and `:181` under that same lease | **VERIFIED.** |
| A11 | §1b: `acquire` takes durable object identity (`observation.rs:96,105`) | `src/checked_artifact/observation.rs:96` (`root_identity = durable_identity(…)`), `:105` (parent `durable_identity`) | **EXACT** — both cited lines are the durable-identity calls. |
| A12 | §1b: pre-publication abort constructs no `CheckedArtifact` (`abort/evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`) | `abort/evidence.rs:39-41` is exactly the `let Some(publication) … else { return Ok(()); }` early exit; `rollback_prefix.rs:76-97` gates the evidence observation on `publication_evidence_complete`; `:155-184` (`pending_selected_root_is_after`) is Git-backend-only | **VERIFIED.** |
| A13 | §1a: `MissingReplace` never detaches (create side safe) | `classification.rs:41,150` | **VERIFIED** — the identifier exists and is the Missing-leaf replace arm. |
| A14 | §1a: `RecoverableDetached` convergence unreachable (the `(expected, goal)` pair derived from the vanished record) | `classification.rs:12,261` | Name and mechanism **VERIFIED structurally**; the unreachability itself is driven at the candidate row (A6), which is the right evidence form. |
| A15 | §1: candidate at `probe/e4-3-detach-window-evidence` (`c9a7303`), "never on main" | git | **VERIFIED**: `c9a7303` = the branch tip; `git merge-base --is-ancestor c9a7303 main` → NOT on main. |
| A16 | §3 P-1: checker keeps `store/rewrite.rs: {rename_durable: 2, sync_dir: 2}` | `check_checked_artifact_boundaries.py:345-349` | **EXACT** — the live map row matches; both fail-closed directions already exist (:1198-1214: growth fires; a vanished entry fires; a moved count fires). P-1's re-shape is a marker/message change, not a mechanism change — correctly described. |
| A17 | §3 P-2 precondition: `rewrite_merge_store_record` "absent tree-wide, or present only in this amendment's quotes" | whole-estate grep | **VERIFIED**: zero hits in `gwz-core/src`, `gwz-core/scripts`, and every dev-docs file except this amendment. |
| A18 | §3 P-3: `capability.rs:45-55` stays TRUE by construction (no probe on the shared commit) | `store/rewrite.rs` | **VERIFIED**: `commit` (:77-152) contains no `checked_artifact` reference; the file's only boundary call is `create_open`'s `entry::create_merge_store_record`. Under the exception this remains true permanently. |
| A19 | §4: "`archive_terminal` is reached from abort's `respond()` on the plain lease" | `start.rs:107-125` (`respond` → `archive::archive_terminal` on Terminal/ArchiveReady), `start.rs:169` (the serve path that routes v1-record abort), `service.rs:116-121` (abort = plain `acquire`); also `service/execution.rs:26/:44` (`store.archive` on the terminal arms) | **VERIFIED** — E4.4's first-bound-step claim has a real structural anchor. |
| A20 | §3 P-2: "CRLF-normalized at the read per the f715ddf lesson" | checkpoint E4.1→E4.2 entry (`CurrentProgramCheckpoint.md:2731-2735`) | **CONSISTENT** — f715ddf is the recorded CRLF hotfix and the normalize idiom is the recorded cure. |

**Misquotes found: NONE.** One characterization defect against the row's own
cells is [P2-1] below (not a misquote — an omission of one clause from the
disposition).

## B. Mandate 2 — THE :274 RULING

**RULED: the amendment's reading is DEFENSIBLE; row `:274` needs NO textual
amendment — conditional on [P2-1]'s cure.** Grounds, each checked:

1. **Row `:274`'s own three cells carry a purpose and ordering properties,
   not a boundary mandate.** "no parent creation; unknown fields and exact
   reread preserved" are all live properties of the raw `commit` on main:
   no parent creation (the rewrite renames onto an existing path; the only
   `create_dir_all` in the file died with E4.2's `create_open` conversion);
   unknown fields and exact reread (`verify_known_diff` →
   `unknown::overlay` → `require_expected` on both the staged and the
   published bytes → the staged/published byte comparison,
   `rewrite.rs:98-151`). The pins the amendment cites already hold in-tree.
2. **The conversion mandate for the v1 store lives in row `:280`, by the
   corpus's own prior construction** — not the amendment's invention: O13's
   minted text ("*Convert the v1 checked store/root/bundle paths per §10
   row `:280`'s frozen ordering*", plan §1.1 :77-79) hangs the whole v1
   conversion obligation on `:280` and makes E4.3 the carrier of its
   rewrite half; the E4.2 review's flag-6 judgment rules the boundary
   route is "*row `:280`'s own meaning*" via its second cell ("same
   purposes and artifact actions" = the `:278`/`:279` siblings'
   `CheckedArtifact::acquire` + `replace_exact`); and the O1-enumeration
   correction (plan §1.1 :133-136) has O1's close carrying row `:280` via
   O13. Excepting `:280` therefore lands the exception exactly where the
   obligation lives; `:274` is untouched truth, not an untouched
   obligation-carrier.
3. **The plan's E4.3 step text binds `:274`'s step to O13's rewrite half**
   (plan :455-460) — and that step text is in §7's annotation list, so the
   discharge-by-exception is recorded at the place the step is defined.
4. **The condition.** Flag-6 locates the boundary mandate in `:280`'s
   SECOND cell. The amendment dispositions only the third ("an exception
   to the WRITER-PRIMITIVE clause only … adds ONE dated exception to its
   third cell, and nothing else") and its unaffected-list quotes only the
   "same purposes" fragment of cell two. A rewrite path that keeps
   `rename_durable` performs no artifact action, so cell two's "artifact
   actions" is left contradicted-but-unexcepted for the one path the
   amendment carves out — and the `:274`-discharged-through-`:280` reading
   would inherit exactly that residual breach argument. With [P2-1]'s
   one-sentence cure the reading is airtight; without it, a future reader
   (E4.6's fork, E7's close) can re-open the row on the amendment's own
   silence. Ruling stands: no `:274` text change; cure `:280`'s cell-two
   disposition instead.

## C. Mandate 3 — O13/O14 coherence

**§6 vs the plan's dated records.** The 2026-08-27 accepted-residual
(plan §1.1 :103-109) is quoted-in-substance correctly by §6: scope ("the v1
store as a production raw durable writer on the no-ff path") and expiry
("expires with E4.2/E4.3's landings") match the record. §6 supersedes ONLY
the expiry clause — correct in intent, with two coherence defects:

- The 2026-09-01 C2 correction bracket (plan :88-102) is verified real and
  correctly characterized ("the two archive files are E4.4's; E4.3 empties
  `store/rewrite.rs`" — plan :458-460; checker comment :332-344; E4.2
  review flag 1 CONFIRMED). But the amendment contradicts, without naming,
  two sentences of that same bracket: ":90 the inventory empties across
  E4.2–E4.4" (under the exception, `store/rewrite.rs` NEVER empties) and
  ":101-102 E4.2/E4.3 do complete that store's conversion" (E4.3 now
  completes it by exception, not conversion). → [P2-3](a).
- §6's "What remains is no longer an accepted residual awaiting
  conversion; it is a pinned, reviewed exception" overreaches: what
  remains in the O13 inventory after E4.3-B is the exception PLUS
  `archive.rs`/`store/archive.rs`, and those two ARE still an accepted
  residual awaiting conversion — E4.4's. → [P2-2].

**§2/§5 vs O14's minted text.** O14 (plan §1.1 :110-132; minted 2026-09-01,
E4.2 review C1) reads: the fork — "either converted leaf writes route
through the write authority (a real conversion package), or §8/§9's claims
are amended to the landed architecture (a frozen-text move needing its own
review tier)" — "is DECIDED at E4.6's chartering … escalates to the
operator if it amends frozen text". The amendment's attachment of the
record-root re-examination to that fork is **correctly placed**: E4.6's
chartering is, by O14's own words, the point "when every converted shape
exists" — the only venue where option (a) (reader-side residue
reconciliation, §5) can be judged with the write-authority architecture in
view. Two notes: (i) §2's conditional ("if converted leaf writes move onto
the write authority **with** a reader-side residue reconciliation") couples
two things O14 keeps separate — harmless as drafted, because the operative
commitment is the unconditional tail ("the record root converts there or
the exception is re-affirmed there, deliberately"); (ii) the record-root
agendum is an ADDITION to O14's fork, and O14's own ledger row does not
carry it — an E4.6 charterer reading plan §1.1 would not learn of it there.
→ [P2-3](c).

**O1's close composition — COHERENT.** O1's existing close conditions:
"O1 cannot close DISCHARGED without O13" (plan :109); "O1's close must cite
O14's disposition" (plan :132); "O1's close carries row `:280` via O13"
(plan :136, restated at :150-155 and at the E7-Acceptance §4 O1 row :168).
§6 adds: "O1's close cites this amendment alongside O14's disposition."
These compose without contradiction: at the E4 resumption's close, row
`:280` closes via O13 as creation-converted (E4.2, `7f28907`) plus
rewrite-discharged-by-exception (this amendment + §3's pins), under O14's
disposition (whatever E4.6 rules, the exception is re-examined there either
way). Nothing in the E7-Acceptance's O1 row conflicts — it never fixed the
discharge MODE of `:280`'s rewrite half. The E7-Acceptance's O13 row
(:180, "…expires with those landings") carries the superseded expiry
clause and needs a dated bracket, per that document's own precedent
(the O12 row's "[nit-1 carrier deviation 2026-09-01 …]" bracket).
→ [P2-3](d).

## D. Mandate 4 — CORPUS SWEEP (exhaustive)

Sweep method: whole-estate greps over `/Users/owebeeone/limbo/gwz-dev` for
`no legacy raw writer`, `raw durable writer`, `rewrite_merge_store_record`,
`expires with`, `retire.*empty`/`empties`, `store/rewrite`, `E4.3` (docs,
src, scripts), `:274`/`:280` citations, `converted siblings`,
`abort/plain-lease`; plus targeted reads of the plan, checker, checkpoint,
E7-Acceptance, E0.2/E0.2b pair, E0.3/E7 review records, v0.11.0
checkpoint + release notes, and the live tree's comments. Every hit that
asserts or implies the v1 store fully converts or "no legacy raw writer"
without exception:

| # | Home | The claim | Status under this amendment | Owner of the cure |
| --- | --- | --- | --- | --- |
| S1 | plan :314-316 (Phase-E4 preamble) | "One step per §10 table row, each a reviewed <500-LOC package converting that consumer onto the checked boundary with its frozen ordering" | contradicted for the E4.3 row | **UNOWNED as drafted** → [P2-3](b) |
| S2 | plan :455-460 (E4.3 step) | "`store/rewrite.rs`'s raw writer converts here"; "[C2:] E4.3 empties `store/rewrite.rs`" | contradicted | **OWNED** — §7 "the E4.3 step … annotations" |
| S3 | plan :87-88 (O13 pin half) | "retired to empty in E4.2/E4.3's own commits" | contradicted (already half-corrected by C2) | **OWNED** — §7 "O13 ledger annotations" |
| S4 | plan :90 (C2 bracket) | "the inventory empties across E4.2–E4.4" | contradicted — `store/rewrite.rs` never empties | **UNOWNED by name** → [P2-3](a) |
| S5 | plan :101-102 (C2 bracket) | "E4.2/E4.3 do complete that store's conversion" | contradicted — completion is now by exception | **UNOWNED by name** → [P2-3](a) |
| S6 | plan :103-109 (accepted-residual) | "expires with E4.2/E4.3's landings" | superseded — exactly §6's object | **OWNED** — §6 |
| S7 | checker :322-326 | "The conversion that discharges the clause is R2-E E4.2/E4.3's; until it lands, the files below are the ACCEPTED RESIDUAL" | contradicted for the rewrite row | **OWNED** — §3 P-1 re-shape |
| S8 | checker :327-331 | "The set may only SHRINK … E4.2/E4.3 retire entries to empty deliberately … each retirement taking a dated comment" | contradicted — the rewrite entry becomes permanent; shrinkage must fire as amendment-revision, not retirement | **OWNED** — §3 P-1 (its parenthetical names exactly this reversal) |
| S9 | checker :332-344 | "E4.2 owns the store's CREATION path and E4.3 its rewrite path … exactly E4.3's remaining half" | contradicted ("remaining half" implies pending conversion) | **OWNED** — §3 P-1 |
| S10 | checker :1203-1207, :1209-1214 (finding messages) | shrinkage message: "must be retired from the pin deliberately"; moved-count message | for the rewrite row, the message class must become revise-this-amendment | **OWNED** — §3 P-1 |
| S11 | LIVE tree `store/rewrite.rs:42` (create_open's doc, its closing sentence) | "`commit`'s raw writers are untouched — E4.3's half of O13." | stale post-landing: reads as conversion-pending; the file itself would carry no trace of the permanent carve-out | **UNOWNED** → [P2-3](e) |
| S12 | `GwzM5-8R2E-E7-Acceptance.md:180` (O13 row) | "substantive half RE-OWNED with E4.2/E4.3 … the dated accepted-residual record … expires with those landings" | expiry superseded; discharge mode changed | **UNOWNED** → [P2-3](d) |
| S13 | `GwzM5-8R2E-E7-Acceptance.md:168` (O1 row) | "the close carries §10 row `:280` via O13" | still true; mode now includes discharge-by-exception | fold into the same bracket as S12 (recommended, not required) |
| S14 | `CurrentProgramCheckpoint.md` :1887-1934, :2730-2770 (position entries) | "substantive conversion RIDES E4.2/E4.3"; "NEXT: E4.3 (merge record rewrite — launches now …)" | historical position entries; superseded by the NEXT entry under the checkpoint's own header rule — PROVIDED the E4.3/E4.3-B entry lands | **UNOWNED by name** → [P2-3](f) |
| S15 | `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` :1015, :1078, :1747 (O13 minting rows) | "substantive half → E4.2/E4.3 as a scope clause" | dated frozen minting analysis; OWNERSHIP stays true (E4.3 still owns the row — its discharge mode changed); resolved through plan §1.1, which is the operative ledger | none — left as written (house discipline) |
| S16 | `GwzM5-8R2E-E03-ReviewState.md` :1002-1387; `GwzM5-8R2E-E7-ReviewState.md` :399; `GwzM5-8R2E-E4.2-Review.md` flag-6 tail ("the boundary … is correct for their rows", i.e. E4.3's too) | dated review verdicts | left as written; flag-6's consequence clause is superseded FOR THE REWRITE ROW by §1a's driven evidence — which §1 records | none (covered by §1) |
| S17 | LIVE tree `checked_artifact/bootstrap/managed.rs:44-47` (allow reason) | "the unconsumed imports shrink as E4.3-E4.6 convert" | stale range — E4.3 now converts nothing | [P3-3] (E4.4's stale-reason class, or E4.3-B's option) |

Also swept and CLEAN: `GwzMergeCheckpoint-v0.11.0.md`, `GwzReleaseNotes-v0.11.0.md`
(no raw-writer/row-`:280` claims); `:274` citations estate-wide (only the
checkpoint's historical :2768); `rewrite_merge_store_record` (A17); plan §0
:16-19 ("the §10 consumer conversion table is executed") — survives via the
discharge-by-exception reading, no cure needed provided O1's close uses that
phrasing.

**Count: 17 hits. 7 owned by the amendment as drafted (S2, S3, S6, S7, S8,
S9, S10). 7 must-cure additions (S1, S4, S5, S11, S12, S14 → [P2-3];
S13 recommended alongside S12; S17 → [P3-3]). 2 no-cure dated records
(S15, S16).** E4.3-B's charter completeness depends on the must-cure list
— see [P2-3].

## E. Mandate 5 — §4, the binding clause: fit and home

**Fit against §5.2: CLEAN — no amendment of the frozen list.** §4 is a
process clause (charter-time lease-reachability enumeration + a dispositive
record before any probe lands plain-lease-reachable); it changes no
semantics of §5.2's list and correctly names list amendment as one of the
three sanctioned outs "at this amendment's tier" — consistent with the
list's dual-#1 provenance. It generalizes exactly the class already on
record: E4.1's acquire/acquire_activated split and the E4.1 carrier
"[P3-C1] R2 unpinned against E4.4's terminal-archive arm regaining
capability-dependence for abort → E4.4's charter" (plan :421-423). The E4.4
first-bound-step claim is structurally verified (A19).

**Home — RULED: the amendment is the authoritative home; the plan's
Phase-E4 gate-note block must carry a dated pointer.** The corpus's own
mechanism for binding E4 charter authors is an annotation at the plan's E4
gate notes — that is where O14's interim pattern binds ("binding
E4.3–E4.6's charters", plan :129-131) and where the E7 acceptance placed
the [R2-P3-1] scope clause "written where E4's builder will read it"
(plan :359-376; E7-Acceptance §5 act 1). A clause that exists only in this
amendment will not be read at E4.4-E4.6 chartering time. Do not duplicate
the clause text (one authority, one pointer — the citing rule): a dated
two-line pointer naming the PLAIN-LEASE PROBE CLAUSE and citing this
amendment §4 belongs in the plan's E4 gate-note block, landed by E4.3-B.
→ folded into [P2-3](b).

## F. Mandate 6 — authorization chain

- **The operator ruling.** "proceed with (c)" appears NOWHERE in the corpus
  outside this amendment — the E4.3 decision packet is unfiled (no doc, no
  checkpoint entry; the checkpoint's last position is E4.2's "NEXT: E4.3").
  The amendment's characterization is internally complete — §5 defines
  options (a) and (b) and §2 is (c)'s substance, so the amendment IS the
  packet's first filed record — but the ruling's only corpus home must not
  remain a DRAFT under review: the E4.3-B checkpoint position entry must
  quote the ruling and its date. → [P2-3](f). (Characterization itself:
  consistent — option (c) as "dated exception + pins + O14 routing"
  matches §2/§3/§5's construction; nothing in the corpus contradicts it.)
- **Tier claim.** "Dual, peer-blind Code+State, Fable×2" MATCHES the
  refined tier policy's substance: "Fable ONLY at the program-level dual
  gates — … this amendment-tier work — plus escalations and deep
  diagnosis" (checkpoint, 2026-08-22 entry), plus uniform program practice
  (thin-A1, operator-escape, durable-cursor, exact-evidence — every
  frozen-text amendment took a dual) and R2-E's own E5-era determination
  that a frozen sub-surface "must be minted BY AMENDMENT WITH DUAL REVIEW"
  (plan :470-476). But there is NO clause titled "amendment tiers" —
  grep-verified across the checkpoint, AgentProcessRules.md, and the
  process docs. → [P3-1] (cite the actual line; also state on the record
  that this dual sits outside plan §2's "two duals maximum", which
  budgeted the plan's phases — both R2-E duals were consumed at E0/E7 —
  on the amendment-tier line's own authority).
- **E4.3-B's interior tier.** "Interior single-axis review (Opus) per the
  tier policy; the amendment itself carries the dual" — CONSISTENT: the
  refined tier policy puts interior single-axis reviews on Opus with
  auto-escalation on P0/P1/P2; E4.1/E4.2 ran exactly so; the frozen-text
  move is what needs the dual, and it is this document, not the build
  package. Coherent with O14's "a frozen-text move needing its own review
  tier" — this amendment is that tier, operator-authorized.
- **§1c's charter quotes** ("abort/plain-lease paths untouched and
  capability-free" / "the leaf write rides the same `CheckedArtifact`
  boundary the converted siblings use") are verifiable NOWHERE — the E4.3
  charter was a launch brief, not a filed doc (grep: zero hits estate-wide).
  The amendment owns the defect either way ("the lane owner's drafting
  miss, on the record") and becomes the quotes' first filing. → [P3-2]
  (say so, one clause, so the quotes are not read as citations to a
  findable record).

## G. Mandate 7 — §7 completeness

§7 names: this document's landing, P-1's checker re-shape, P-2's tripwire,
"the E4.3 step and O13 ledger annotations", the no-production-change bound,
the candidate branch's retention, the salvage option, the interior tier.
That list is NOT complete against the sweep. Missing by name (each with a
sanctioned precedent):

(a) the two C2-bracket sentences in plan §1.1 O13 (S4, S5) — the "O13
    ledger annotation" must name them or a builder will annotate the
    accepted-residual record only;
(b) the plan Phase-E4 preamble's universal (S1) and §4's pointer at the
    gate-note block (§E ruling) — one dated annotation can carry both;
(c) plan §1.1 O14 row — the record-root re-examination agendum at the
    E4.6 fork (§C);
(d) `GwzM5-8R2E-E7-Acceptance.md` §4 — dated bracket at the O13 row
    (S12; O1 row S13 recommended), precedent the O12 nit-1 bracket;
(e) `store/rewrite.rs` — ONE dated `///` sentence recording the
    exception at the file (S11); this is doc-text, and §7's "NO change to
    `store/rewrite.rs` production code" should say the doc sentence is
    in-bounds so the builder does not read the bound as forbidding it;
(f) the CurrentProgramCheckpoint position entry (S14) — carrying the
    operator ruling verbatim (§F) and the tier recording, per the
    checkpoint's own header rule and the §7.6 tier-recording ritual.

Also verified PRESENT and correct in §7: the package base (`7f28907` =
live main, checked), the candidate's evidence retention (A15), P-2's
invertibility of the candidate's crash row (the row exists — A6 — and the
negative form is well-specified against A17's zero-hit baseline), and the
salvage option's bound (builder's option inside the cap, not a mandate —
consistent with the two-round/cap discipline).

## H. FINDINGS

**[P2-1] — the amended row's second cell is left undispositioned, against
the E4.2 review's own reading.** §2: "The exception is an exception to the
WRITER-PRIMITIVE clause only: the row's 'same purposes' clause, the `:274`
rewrite row's own clauses …, and the creation path's full conversion (E4.2)
are unaffected and remain binding" — and the framing sentence pins the edit
to "its third cell, and nothing else". Flag-6 (E4.2 review :402-434) rules
the boundary mandate IS the second cell: "same purposes and artifact
actions" = the `:278`/`:279` siblings' `CheckedArtifact::acquire` +
`replace_exact`. A rewrite path that keeps `rename_durable` performs no
artifact action, so cell two is contradicted-but-unexcepted on exactly the
carved-out path; the unaffected-list's careful "same purposes" fragment
shows the drafter saw the edge and left it silent. Post-landing the corpus
would hold three records that do not compose (flag-6 + cell-two-binding +
a permanent raw path). Same class as E0.3 State round-2 [P2-R1] (a frozen
row's clause left uncovered by the amendment mapping it). **Cure (one
sentence, in the §2 exception blockquote):** disposition "artifact
actions" for the excepted path — e.g. *"On this one path the publication
is deliberately NOT an artifact action: the second cell's 'artifact
actions' clause is read through this same exception for the rewrite leaf,
and 'same purposes' remains binding; every other v1 path keeps the full
second cell."*

**[P2-2] — §6 overreaches the archive files.** "What remains is no longer
an accepted residual awaiting conversion; it is a pinned, reviewed
exception" — false for `store/archive.rs`/`archive.rs`, which remain in
the O13 inventory as conversion-pending E4.4 debt (C2; checker :332-344;
E4.2 review flag 1). As written, E4.4's charter author could cite §6
against their own conversion duty. **Cure:** scope the sentence to the
record-root carve-out and state the archive rows keep their
accepted-residual, retire-on-conversion marker until E4.4.

**[P2-3] — §7's charter omits five records the fork touches.** Legs
(a)-(f) as enumerated in §G, each with its named home and precedent.
**Cure:** extend §7's package list by name (a document edit here; the
records themselves land with E4.3-B).

**[P3-1] — tier citation.** No "'amendment tiers' clause" exists by that
name; cite the refined tier policy's amendment-tier line (checkpoint,
2026-08-22) and record that this dual sits outside plan §2's two-dual
budget on that line's authority (precedent: the E5-era
"minted by amendment with dual review" determination).

**[P3-2] — §1c's charter quotes are a first filing, not a citation.** Add
one clause marking them "charter text as issued; this amendment is its
record" — the estate has no other copy to check them against.

**[P3-3] — stale allow-reason at `checked_artifact/bootstrap/managed.rs:44-47`**
("the unconsumed imports shrink as E4.3-E4.6 convert") — E4.3 now converts
nothing; the range should read E4.4-E4.6. The corpus's stale-reason class
(cured "in the caller-adding commit"); route to E4.4 or take it in E4.3-B
— either owner, named.

## I. Conditions for GO

1. [P2-1]'s cell-two disposition sentence folded into §2's exception text.
2. [P2-2]'s §6 scoping sentence folded.
3. [P2-3]'s §7 list extended with legs (a)-(f) by name.
4. [P3-1]/[P3-2] one-clause fixes: with the P2 folds or filed as riding
   P3s — drafter's choice; [P3-3] needs only a named owner.

On those folds this axis is GO without a further round. The exception
itself, its grounds, its pins, and its routing are verified against the
tree and the corpus and are the correct record of a real, driven,
structural fact.

*(Round 2, if dispatched, appends below.)*
