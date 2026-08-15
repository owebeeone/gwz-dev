# GWZ M5-8 Durable Preservation-Cursor Amendment — STATE/SEMANTICS-AXIS REVIEW

**Review object:** `dev-docs/GwzM5-8DurableCursorAmendment.md` (Status: DRAFT, dated 2026-08-16, as committed at gwz-dev `9893c5a` "Draft the durable preservation-cursor I2 amendment (adopted D3)")
**Code baseline:** gwz-core HEAD `43c37bcff338daf95f053eedbc7467e6c9368eff` (all sources read via `git show HEAD:` — working tree carries unrelated in-flight work)
**Date:** 2026-08-16
**Axis:** State machine, restart legality, fail-closed direction. Independent, adversarial, read-only. Code axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** on this text — two P1 findings and three P2 findings block. All are text-fixable in one revision round; none overturns adopted Decision 3, and none touches the wire field shape (§2.1), which is sound. I pre-commit to GO on a revised text that resolves P1-1, P1-2, P2-1, P2-2, P2-3 as specified below.

---

## 0. Evidence base

Direct reads: the amendment; `GwzM5-8A1DecisionPacket.md` (Decision 3, §0 row 3, §3.5); `GwzM5-8I2ActionJournalContract.md` §2 (:152-170 verified verbatim against the draft's quotes); `GwzM5-8I2RecordContract.md` §1 :48-50, §8, §9; `GwzM5-8R4bTransitionDesign.md` §6.7 :742-761; `GwzM5-8R4bReverseLifecycleInterface.md` §5-6; `GwzM5-8OperatorEscapeDesign.md` §3.2/§3.3 (U-table, U3 row :160), §6.1; `GwzM5-8ExactEvidencePlatformAmendment.md` §3 (F5 frame-discontinuity lesson, Clause B); `GwzM5-8ProgressReviewF5.md` §3.3, §9 item 4. At gwz-core HEAD: `…/observe/reverse/preservation/cursor.rs` (full), `phase.rs` (full), `model/v1/validate/preservation.rs` (full), `transition/reduce/preservation.rs` (full), `preserve/checked_bundle.rs` (full), `preserve/plan.rs` (owner plan + `v1_owner_evidence`), `preserve/artifacts.rs` (`v1_preservation_image`), `model/v0.rs` (shared `PreservationEvidence` row :158-164, participant :80, publication :186), `abort/preflight.rs`, `merge/mod.rs` :23-24, `record_wire/archive/cleanup.rs` (full head), `gc.rs` (evidence handling), `src/protocol/convert.rs`/`generated.rs` (grep). Every line-cite in the draft that my findings lean on was verified against HEAD and is accurate.

---

## 1. Findings

### [P1-1] The reset-retirement write produces §2.2-illegal row shapes on the draft's own degradation path — a new permanent stuck state (U-class growth)

§2.2 declares `R alone` and `B+R` illegal ("`reset_commit` requires artifact-pass completion evidence on the same row"). §3.1 edge 1 makes the `reset_commit` write **unconditional**: "the bit rides that same write" of `finish_reset_attached_ref`. §4.2 simultaneously promises that records without durable markers get "today's full re-proof path … **verbatim** … with today's outcomes" — and today's outcome includes **no marker write** (`require_artifact_complete`, cursor.rs:225-239, is an observer; verify_pending_prefix writes nothing).

Concrete interleaving (all steps legal under the draft):

1. A pre-amendment writer (a pre-A1 test binary — a population §4.2 explicitly supports) runs owner *k*'s artifact pass: backup created (row = B), stash live-proven unnecessary (no marker exists to write). Reset pass journals `ResetAttachedRef` for owner *k*. Crash or handover.
2. The amended binary resumes. `verify_pending_prefix` live-re-proves per §4.2 (verbatim, no writes). The reset executes, or is classified After.
3. `finish_reset_attached_ref` retires the journal and, per §3.1 edge 1, writes `reset_commit` → row becomes **B+R — illegal per §2.2**.
4. The RecordContract §8 write discipline rereads through the decoder and compares; the amended validator rejects B+R → the retirement write fails. The physical reset has already happened. Every subsequent dispatch re-observes the pending reset as done and re-attempts the same rejected write, forever. The record is permanently stuck in `Preserving` with an unretirable journal.

Second variant: §3.1 edge 2 (no-op reset) writes `reset_commit` for an owner with **no row at all** (fully-noop owner on a degraded record) → creates an `R alone` row → same rejection, write-ahead this time, so it wedges before rather than after the step — still a permanent classification failure at that position.

This is "later" degraded to **a new NEVER-terminates state**: not wrong evidence (fail-closed direction holds — no wrong mutation), but a stuck state that today's semantics does not have, contradicting §4.2's "never worse than the status quo" and the U3-payoff framing (item 5 of this axis: a lying/colliding row = new stuck state — here the row isn't even lying).

**Remedy (any one):** (a) specify that the two reset edges backfill the missing artifact marker in the same atomic rewrite when the row carries neither `noop_commit` nor a stash pair (producing legal `B+N+R` / `N+R`; one record write, still one durable step); or (b) legalize `B+R`/`R alone` as degraded-but-valid shapes whose artifact position falls to the §4.2 live fallback. Add the missing §8 test row: degraded record (marker-less rows) with a retained pending reset, resumed by the amended binary, retires successfully.

### [P1-2] The decode-time rejection of "later durable row + absent earlier row" (§3.3 last sentence, §4.3) rejects legitimate records, contradicts §4.2/§5, and makes the draft's own §8.2 equivalence suite unpassable

§3.3: "a later durable row (artifact, skip, or reset marker) while an earlier owner's row shows neither artifacts nor markers is now a record-bytes contradiction, rejected by checked-open validation … without any repository read." §4.3 repeats it ("later-durable-row-vs-empty-earlier-row … rejected at decode").

But **absent-earlier-row + later-artifact-row is a legitimate pre-amendment record shape** — the exact "legitimate restart after earlier no-op skips" that TransitionDesign §6.7 :751-761 declares must continue: owner 2 fully clean (live-noop, never given a row — the frozen wire has nothing to write), owner 5 dirty (stash row). Today's guard (`reject_later_durable_owner`, cursor.rs:47-62) separates the legal case from the regressed case **by a live fact** (it fires only when the earlier owner's `backup_complete`/`stash_complete` live checks fail). A decode-time rule cannot see that fact and rejects both. Since §5 deliberately takes **no version bump**, record bytes cannot attest which writer produced the shape — and even a marker-conditioned narrowing is unsound: the amended binary resuming a pre-amendment record with a pending action at owner 5 writes owner 5's evidence/markers under §4.2's no-backfill observer semantics, legitimately producing marker-at-later + row-less-earlier.

Internal contradictions this creates, in the draft's own text:

- §5: "Pre-amendment row shapes remain valid — required for graceful degradation (§4)" — falsified for this shape.
- §4.2: such records get "today's full re-proof path verbatim" — unreachable; they are rejected at open, before any observation.
- **§8.2(b) is self-refuting:** stripping markers "to the pre-amendment shape" deletes an N-only row entirely (pre-amendment shape of a fully-noop owner is *no row*), producing exactly the rejected pattern — so the mandated "identical classification verdicts" cannot hold if §3.3's rule is implemented. The draft's own test spec detects this finding.

**Remedy:** restrict decode-time rejection to **marker-bearing** contradictions (fabricated/mismatched marker values per §2.2, marker/pending-action conflicts, and *marker*-vs-earlier-emptiness only where an earlier owner's own row proves the pass ordering was violated). The absent-earlier-row leg stays on the live-fallback path, and `reject_later_durable_owner` is retained for marker-less earlier owners (it may be dropped only for owners whose retirement is durably recorded — which is the sentence's first half, and that half is correct). §7.3's TD edit already uses the sound scope ("marker contradictions"); align §3.3 and §4.3 to it.

### [P2-1] Pure skip rows break the archived-cleanup worklist derivation and their GC fate is unstated — an unpriced U8 growth on ordinary merges

`record_wire/archive/cleanup.rs` `collect_owner` (shared by `from_v0` and the test-gated, A1-destined `from_v1`) hard-rejects any evidence row with `backup_ref.is_none() && stash_id.is_none()` as `CleanupError::ContradictoryEvidence`. An N-only or N+R row — the amendment's central new state, which §2.3 explicitly says "survive[s] archival with their row" — hits this arm. Result at HEAD semantics: **every archived merge containing one fully-noop owner fails worklist derivation**, blocking all targeted cleanup (including other owners' backup refs — the error is derivation-wide) — permanent retention, the U8(b) class the escape design already calls a forever-stop. Adjacent and unstated: `gc.rs::retain_remaining_stashes` strips backup fields and **drops rows without stash pairs** at post-GC rewrite — the markers' "immutable once written" lifetime versus GC-time row retirement is unspecified.

The amendment's §5 compatibility sweep (envelope, row fields, preimage frame, bundle identity, record digest, v0/protocol) never visits the terminal plane; §9's implementation-owner list omits `record_wire/archive/cleanup.rs` and `gc.rs`; §8 has no cleanup/GC test row. This is precisely the axis-4 question (does the pure-skip-row leg create a state downstream consumers mishandle) — answer: yes, one consumer found, and it is the one whose failure mode is permanent.

**Remedy:** add both files to §9; add a §8 row: worklist derivation and targeted GC over records whose rows carry `noop_commit`/`reset_commit` (skip rows contribute no backup refs and no stash evidence, and must not error); state the markers' GC/retirement fate in §2.2 or §2.3 (retiring with the row at post-GC rewrite is consistent with today's backup-field consumption — but say it).

### [P2-2] The §2.2 validity table is not exhaustive: `S+N+R` and `B+S+N+R` are unlisted

Four optional facts (B pair, S pair, N, R) give 16 combinations; the table enumerates 14. The two missing shapes both contain the `S+N` contradiction and are presumably illegal, but the table is enumerative over "row content", and §8.1 pins "every legal and illegal combination **of the §2.2 table**" — so the gap propagates verbatim into the acceptance-time RecordContract §9 rows and the test suite. State the rule structurally ("a stash pair and `noop_commit` may never coexist, regardless of other fields; `reset_commit` requires `noop_commit` or a stash pair on the same row") or add the two rows.

### [P2-3] The highest-latency detection case — foreign interference *after* `reset_commit` — has no test row

My axis-1 interleaving with the longest legal detection deferral: owner *k*'s no-op reset proven (`live_commit == anchor`), `reset_commit` written; the branch then moves (or the worktree dirties) before restart. The reset position is decode-skipped; the owner has **no own next action left** — rollback-entry preflight is the *only* remaining catcher (`abort/preflight.rs` per-member reverse observers; verified full-tree at HEAD), and it does catch it, pre-mutation. §3.3 prices "own next action **or** rollback-entry preflight" generically, but §8.4's suite text exercises only "durably-skipped" (artifact-marker) owners with a subsequent own action. The preflight-only case is exactly the row a regression would silently drop. Add it: post-`reset_commit` branch move / dirt → exhaustion proceeds → rollback-entry preflight refuses fail-closed with no mutation.

### [P3-1] "v0 unknowns" wording does not match the shared-struct mechanism

The row struct is shared (`model/v0.rs:158-164`); after §2.1 lands, `noop_commit` in a v0 record parses into the **typed** field — it will never appear in the unknown-field manifest, and a v0 rewrite would re-emit it (`skip_serializing_if` skips only `None`). §2.3's doctrine ("never adopted, migration ineligible") is right, but its mechanism ("a v0 **unknown** field … collides") and §8.3's test phrasing ("the two names **as v0 unknowns**") assume a manifest surface that will not exist. The collision/ineligibility check must be specified as raw-YAML-path or post-decode presence detection on v0 records, and §8.3 reworded, or the pin tests will be written against the wrong surface.

### [P3-2] Old-field immutability across the new whole-row rewrite edge is not pinned

`set_evidence` (reduce/preservation.rs:238-266) replaces `rows[0]` wholesale. Today reset retirement never touches the row; the amended edge rewrites it, and per-write validation checks value *shapes*, not cross-write constancy of `backup_ref`/`backup_commit`/`stash_id`/`stash_object_id`. §8.1 pins immutability "of both fields" (the new ones) only. Extend the row to the four existing fields across the reset-retirement and marker-write edges.

### [P3-3] §9's persistence-traffic claim undercounts

"One write per skipped owner is the only persistence-traffic addition" — a standalone `reset_commit` write for a **no-op reset on a non-skipped owner** (real B+S artifacts, reset unnecessary) is also a new write. Bounded by 2×owner count either way; correct the sentence.

---

## 2. Axis findings in detail (items 1-6 of the charge)

**1. Invariant preservation — verified, with the P1-1/P1-2 exceptions.** The core claim (§3.3) survives the adversarial pass: no marker ever authorizes a physical mutation; every retained execution-edge observation was verified present at HEAD (stash preimage recheck, reset `expected_commit`/`checkout_matches_commit` in `observe_reset_phase`, backup exact-target classification in `observe_pending`, bundle live stash reads, preflight full-tree observers). Interleaving results: crash between no-op proof and row write → **same point** (live fallback re-proves; idempotent, §4.2 states it). Foreign mutation after N → **later, priced** (owner's own reset if unretired, else preflight; work is never destroyed — preflight's clean/exactness refusal stands between markers and every mutation, and preservation always feeds rollback via `VerifiedPreservationExhausted`). Row written then anchor/branch moves → anchor is an immutable record fact and cannot move; the *branch* moving after R is the preflight-only case (P2-3 — priced in text, missing in tests). Stash object dropped after the pair is durable → **same point** (bundle-prefix verification per dispatch, `checked_bundle.rs:152-171`; §8.4 pins it — verified this is real at HEAD, including object-id, head, message, and non-default dirty checks). Fabricated marker with a correct §2.2 value → never caught at decode, caught at preflight; this is the same trust model as today's fabricated backup pair (`backup_complete` already trusts the durable row with no live re-proof), so nothing is weakened in kind. **No NEVER case exists in the draft's intended semantics — no P0.** The only "never terminates" state is the P1-1 construction, which is a text defect, not a design defect.

**2. Write-ahead discipline — sound, one atomicity claim verified, one collision (P1-1).** `noop_commit`'s proof-then-write window is crash-safe: crash before the write reverts to live re-proof (the row never lied — it never existed); no journal exists for a no-op position, so there is no row-write-vs-retirement window at all. `reset_commit` edge 1 atomicity is real: `finish` mutates `pending_preservation = None` and (with the one-field extension) the evidence row in the same `next` record, persisted by the RecordContract §8 single-record atomic rewrite-reread-verify; there is no sidecar journal (RC §1 :48-50). The draft's claim that the reset finish arm "uniquely writes no evidence at :221-223" is exact at HEAD. The collision is what that write produces on marker-less rows (P1-1).

**3. Decode-terminal semantics — no new trust root of consequence; re-validation is stated and real.** What stops being re-imaged is the earlier owner's *worktree*, not the stash: stash content trust is content-addressed (object id) plus per-dispatch live existence/identity/dirty-summary through the bundle check — unchanged. The backup pair was already decode-trusted today. N and R are the genuinely new record-plane attestations, and they are validated derivations rejectable at decode when fabricated with wrong values; with correct values they only skip re-proof, and preflight independently re-establishes every live fact before any mutation. Consistency with the ExactEvidence amendment holds: the preimage frame is untouched (no second F5-style discontinuity; §5 states this correctly), and Clause B's private-area exclusion is *less* load-bearing here, since the amendment removes earlier-owner image comparisons rather than adding decode/live equalities. The root-reset carve-out dissolution (§3.2) is correct: I verified cursor.rs:264-274 exists solely to tolerate the journaled intermediate root form, and the "pair present + no pending action at that position" reading subsumes it, with the mid-`CreateStash` crash still owned by the action's own observer (`observe_plain_stash`, verified). What re-validates a durable pair at restart is stated (§3.3 bullet 2, §8.4).

**4. Validity table — near-exhaustive, two gaps (P2-2), one downstream mishandler (P2-1).** Against owner kinds: the anchor derivations match `owner_anchor` at HEAD for all three owner kinds (participant / selected root post-composition / publication root), anchor stability through `Preserving` holds (no forward re-entry, same argument the frozen reset `restore_commit` validation already relies on), and the selected-root/publication-root collision rule is untouched. Marker/pending cross-checks are complete: pending reset on an N-only row is already unconstructible (no recorded backup target), the two new explicit rejections cover the rest. The S-alone ⇒ backup-was-noop entailment is genuinely decode-derivable (verify_pending_prefix:124-137 proves the stash journal required a passed backup position). One pleasant consequence verified: the amendment removes today's odd reachability of `backup_intent` on an S-alone owner whose branch later moved (re-opening a retired position); under decode-terminal reading that regression is caught at the owner's reset instead — fail-closed both ways. Downstream consumers of the new N-only state: bundle derivation skips it (verified, `checked_bundle.rs:145-151`), status/protocol never project rows (verified — only the error code crosses the protocol), gc preflight tolerates it, **archived cleanup does not** (P2-1).

**5. U3 wedge claim — the shrink is real and correctly scoped; two growths found, both filed.** The U3 leg that shrinks is exactly the over-eager one: transient ambiguity/dirt in a *retired* earlier owner no longer blocks later classification per dispatch; the genuine-loss legs (stash dropped, bundle mangled) still block every dispatch through the unchanged bundle verification — which is right, because those artifacts are unreconstructible. `reject_later_durable_owner`'s wedge ("owner acquired new work ahead of a later durable owner") disappears only for durably-retired owners; for marker-less owners it must survive as a live guard — the draft's attempt to convert it wholesale to decode-time is P1-2. Growths: the P1-1 stuck state (new, blocking) and the P2-1 permanent-retention class (U8, blocking as unpriced). Marker corruption by external tampering adds two names to an existing brick class (invalid evidence rows already reject at open) — growth in surface, not in kind; quarantine covers it; no finding.

**6. Test obligations — good skeleton, four gaps.** §8.2's backend-call-counting and marker-stripped equivalence are strong obligations (and §8.2(b) is the very fixture that exposes P1-2). Missing: the degraded-record pending-reset retirement row (P1-1), the post-`reset_commit` preflight-only interference row (P2-3), the cleanup/GC skip-row row (P2-1), the two unlisted table combinations (P2-2), plus the P3-1 rewording and P3-2 extension. With those added, the suite list covers every interleaving I constructed.

---

## 3. What is affirmatively verified and should not be re-litigated

- The two-field wire delta itself (§2.1): names, position, encoding, absent-by-default byte-identity, not-booleans rationale — consistent with the row struct and the validated-derivation doctrine (AJ :168-170).
- §1's quotations of the four frozen texts are verbatim-accurate; every code line-cite I depended on is accurate at HEAD; the production-disabled claim (`merge/mod.rs:23-24`, `plan.rs` cfg(test)) is real, so the zero-migration pre-A1 pricing stands.
- Single-record atomicity for the reset edge; idempotent marker convergence; termination (each write monotonically extends immutable durable facts, bounded by owner count).
- Bundle-identity invariance claim (§5): `expected_bundle` reads only stash-pair fields — markers cannot enter bundle bytes.
- The escape-train decoupling (§6): the wire surfaces are disjoint; no hard coupling found in either direction.
- No preimage-frame discontinuity; the ExactEvidence amendment's remains the only one.

## 4. Conditions to flip to GO

1. **P1-1:** marker backfill at both reset edges in the same atomic rewrite (or legalize the degraded shapes); test row added.
2. **P1-2:** decode-time rejection rescoped to marker-bearing contradictions; absent-earlier-row leg explicitly retained on live fallback with the live guard; §3.3/§4.3 aligned to §7.3's narrower wording.
3. **P2-1:** cleanup/GC added to §5 blast radius, §8 tests, §9 owners; marker GC fate stated.
4. **P2-2:** table made structurally exhaustive.
5. **P2-3:** preflight-only interference test row added.
6. P3-1/2/3 at author's discretion (recommended in the same pass; P3-1 will otherwise produce tests written against a surface that cannot exist).

Filed by the State/Semantics-axis reviewer, 2026-08-16, against (`GwzM5-8DurableCursorAmendment.md` @ gwz-dev `9893c5a`, gwz-core HEAD `43c37bc`, 2026-08-16).

---

# APPENDED RE-VERDICT — STATE/SEMANTICS AXIS, FOCUSED RE-READ (remediation round 1)

**Review object:** `dev-docs/GwzM5-8DurableCursorAmendment.md` at gwz-dev HEAD `e9396a9` ("D3 cursor amendment remediation round 1"; DRAFT revision 1, dated 2026-08-16, revised 2026-08-16; full delta vs `9893c5a` read as a diff, +214/−48, document-only)
**Code baseline:** gwz-core HEAD `43c37bcff338daf95f053eedbc7467e6c9368eff` — unchanged from round 1
**Date:** 2026-08-16

## RE-VERDICT: **GO**

All five pre-committed conditions are resolved as specified, none softened. The revision's new code claims were independently verified at HEAD before this verdict. Two P4 observations; nothing at P3 or above.

## Per-condition dispositions

**Condition 1 (P1-1, backfill) — RESOLVED, remedy (a), not softened.** §3.1's new block mandates that when a reset edge finds a row with neither `noop_commit` nor a stash pair, the same atomic rewrite writes `noop_commit` valued per the §2.2 equations. I re-ran the adversarial trace: (i) the degraded pending-reset case (the P1-1 construction) — the pending owner's row always carries B (`recorded_backup_target` is a journaling precondition), retirement now produces `B+N+R`, whose equations hold at reread-verify (N = backup_commit, R = anchor), and the pending/marker cross-check cannot fire because the same write clears the journal; (ii) the no-op-reset-onto-absent-row variant — unreachable at the reset edge, because §3.1's uniform-discipline sentence makes the action-free artifact pass mark every marker-less owner (on degraded records "exactly as on new ones") before the reset loop runs, and the branch-moved rowless owner falls into today's unchanged typed-error class. **B+R and R-alone are genuinely unproducible by post-amendment writers** — I enumerated every marker-touching write edge (artifact-pass N, CreateStash advance, backup finish, both reset edges) and none can emit them; pre-amendment writers cannot write R at all, so the §2.2 illegal rows are now pure tamper tripwires. §4.2's end-to-end degraded-path promise now holds: observation writes nothing while the journal is retained, the retirement backfills, subsequent sweeps converge, monotone and bounded. §8.2(c) pins exactly this scenario including the "no rejected write, no stuck state" assertion. The §2.2 unreachability note is accurate.

**Condition 2 (P1-2, decode rescope) — RESOLVED, exactly to specification.** §3.3's hardened-at-decode list is precisely my marker-bearing surface: §2.2 value equations, marker/pending-action conflicts, stash-pair-with-`noop_commit`, `reset_commit`-without-artifact-evidence — every leg requires a marker on the row, so no pre-amendment record can trip any of them. The optional ordering-based leg I had permitted was dropped entirely — the conservative sound choice. `reject_later_durable_owner` is retained "as a live guard, unchanged" for marker-less earlier owners, with the absent/artifact-only-earlier shape explicitly named legitimate and routed to the §4 live fallback ("never rejected from record bytes"). §4.3 is rewritten as "marker-bearing only" with the explicit carve-out back to item 2. §7.3 is untouched, as required. §8.2(b) now states that stripping an N-only row deletes the row and the resulting absent-earlier shape "must classify identically via the live fallback (never reject at decode)" — the round-1 self-refutation is not just cured but pinned as a test assertion.

**Condition 3 (P2-1, terminal plane) — RESOLVED.** §2.2 terminal-plane-fate paragraph, §5 terminal-plane bullet (with the U8-growth pricing stated in my finding's own terms), §8.6 test row (derivation succeeds, marker rows contribute nothing, other owners' refs enumerate, archive deletion proceeds, post-GC retirement), §9 owners. Cites verified exact at HEAD: `collect_owner`'s all-None-pair rejection is `record_wire/archive/cleanup.rs:163-167`; `post_gc_record`/`retain_remaining_stashes` span `gc.rs:365-381`. The v0-inert claim is correct (the `from_v0` leg never sees marker rows legitimately).

**Condition 4 (P2-2, table exhaustiveness) — RESOLVED.** The two structural rules are normative, verbatim as specified, and the table now enumerates all sixteen shapes — I counted: 1+3+1+1+4+4+2 = 16, with `S+N+R`/`B+S+N+R` in the rule-1 rejection row.

**Condition 5 (P2-3, preflight-only row) — RESOLVED.** §8.5 is my interleaving verbatim: post-`reset_commit` interference, no own next action left, exhaustion proceeds, preflight the sole catcher, refuses fail-closed with no mutation.

**P3-1/2/3 (discretionary) — all applied, and verified.** P3-1's respecified mechanism is better than my sketch and checks out at HEAD: the evidence-row known-key set is a literal four-name list at exactly `record_wire/unknown_fields/extract/common.rs:237`; `map_v0_to_v1` is exactly `mod.rs:64-81` and today checks the five top-level names only with `container.is_empty()` — so the version-forked key set plus the new in-row collision leg is a complete, implementable trigger (raw-YAML extraction runs on `Value`, independent of the typed parse, so it sees the names regardless — the mechanism is coherent). §8.3 is reworded to match. P3-2: §8.1 now pins all six fields byte-constant across the new rewrite edges, citing the whole-row `set_evidence` replacement (`reduce/preservation.rs:259-264`, verified). P3-3: the §9 recount ("at most one marker write per owner per pass … bounded by two writes per owner; the backfill adds a field to an existing write, not a write") is exact.

## New-inexactness sweep (full diff audited; new claims checked at HEAD)

Verified accurate, including fresh claims the revision introduces: the corrected AJ cite `:155-158` (rollback decode-derivable cursor sentence — recounted, correct); the relocated F5 quote (":206" — exact); the rewritten §3.1 edge-1 parenthetical (whole-action vs arm granularity — checked against `finish`: :231 clears the journal, stash finish is indeed also evidence-free, backup finish carries evidence, stash ids install at the `CreateStash` advance — this *fixes* a round-1 looseness); the re-described root carve-out (":264-275", "the pending owner's **own** stash-position check" — more exact than round 1: the carve-out fires inside the all-owner sweep for the pending owner itself); `cursor.rs:29-38`; and §7.1's claim that the row struct is printed in neither I2 contract (grep-verified: no `PreservationEvidence`/`backup_ref` in either).

Two P4 observations, non-blocking, no action required for acceptance:

- **[P4-1]** §2.2 terminal-plane: "consumed with their row exactly as the backup fields are today" — the analogy is loose for *surviving* stash-bearing rows: backup fields are cleared on survivors, while a marker on a survivor (e.g. `S+R`) persists post-GC. The operative sentences describe the code correctly; only the analogy phrase overreaches. Inert either way after archival.
- **[P4-2]** §9's "the first marker write fails the overlay's unauthorized-unknown-field check" — verified directionally against RecordContract §8's manifest-mismatch pre-mutation error rather than by a line-read of `overlay::apply_surviving`; both branches of the claim are fail-closed (set updated → writes pass; omission → loud failure), so nothing rides on the unverified detail.

**Scope note:** this GO is the State-axis re-verdict on the five conditions plus the new-inexactness sweep. §10's claim that all Code-axis findings are also applied is for the Code axis's re-read to confirm; the one point of overlap (Code P2-1 = State P1-2) is resolved by Condition 2 above either way.

Filed by the State/Semantics-axis reviewer, 2026-08-16, against (`GwzM5-8DurableCursorAmendment.md` @ gwz-dev `e9396a9`, gwz-core HEAD `43c37bc`, 2026-08-16). Both round-1 P1s are closed by construction, the U-class ledger is net-shrinking as the adopted decision promised, and the amendment is, on this axis, fit to freeze pre-A1.
